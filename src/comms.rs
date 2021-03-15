use std::{
    ffi::CString,
    io::{Read, Write},
    time::Duration,
};

use serial::{SerialPort, SystemPort};

use crate::protocol::{Command, Request};

pub struct Connection {
    port: SystemPort,
}

impl Connection {
    pub fn open(port_string: &str) -> std::io::Result<Connection> {
        let mut port =
            serial::open(port_string).expect(&format!("Failed to open port {}", port_string));

        port.reconfigure(&|settings| {
            settings.set_baud_rate(serial::Baud57600)?;
            settings.set_char_size(serial::Bits8);
            settings.set_parity(serial::ParityNone);
            settings.set_stop_bits(serial::Stop1);
            settings.set_flow_control(serial::FlowNone);
            Ok(())
        })?;

        Ok(Connection { port })
    }

    pub fn send(&mut self, commands: &[Command]) -> std::io::Result<()> {
        // Command batching prevents device redrawing and makes it receive
        // and process the commands in a tight loop.
        if commands.len() != 1 {
            self.send_command(&Command::BeginBatch)?;
        }
        for cmd in commands {
            self.send_command(cmd)?;
        }
        if commands.len() != 1 {
            self.send_command(&Command::EndBatch)?;
        }

        let result = self.port_read_string()?;

        Ok(if result != "OK" {
            panic!("{}", result);
        })
    }

    pub fn get_next_request(&mut self) -> std::io::Result<Request> {
        self.port.set_timeout(Duration::from_millis(50))?;
        let mut data = [0u8; 2];
        let mut idx = 0;
        loop {
            match self.port.read(&mut data[idx..]) {
                Ok(read) => {
                    if idx + read == data.len() {
                        return Ok(Request::from_bytes(&data));
                    } else {
                        idx += read;
                    }
                }
                Err(e) if e.kind() == std::io::ErrorKind::TimedOut => continue,
                Err(e) => return Err(e),
            }
        }
    }

    fn send_command(&mut self, cmd: &Command) -> std::io::Result<()> {
        self.port.set_timeout(Duration::from_millis(100))?;
        for _ in 0..3 {
            match self.port.write(&Self::encode(&cmd.to_protocol_bytes())) {
                Ok(_) => break,
                Err(e) if e.kind() == std::io::ErrorKind::TimedOut => {
                    eprintln!("Warning: timed out sending the command");
                    continue;
                }
                Err(e) => return Err(e),
            }
        }
        Ok(())
    }

    fn port_read_string(&mut self) -> std::io::Result<String> {
        self.port.set_timeout(Duration::from_secs(1))?;
        let mut data = [0u8; 1024];
        let result = match self.port.read(&mut data) {
            Ok(_) => CString::new(
                data.iter()
                    .cloned()
                    .take_while(|b| *b != 0)
                    .collect::<Vec<u8>>(),
            )
            .expect("Failed to parse a c-string")
            .into_string()
            .expect("Failed to convert a c-string into a string"),
            Err(e) => return Err(dbg!(e)),
        };
        Ok(result)
    }

    // Packet-encode the data (see CommandBuffer.h)
    fn encode(buf: &[u8]) -> Vec<u8> {
        let mut result = vec![];
        for b in buf {
            match b {
                0x00 => {
                    result.push(0xff);
                    result.push(0x00);
                }
                0xff => {
                    result.push(0xff);
                    result.push(0xff);
                }
                _ => result.push(*b),
            }
        }
        result.push(0x00);
        result
    }
}
