use std::{
    ffi::CString,
    io::{Read, Write},
    time::Duration,
};

use serial::SerialPort;

mod command;

use command::Command;

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

fn start(mut port: serial::SystemPort) -> std::io::Result<()> {
    port.reconfigure(&|settings| {
        settings.set_baud_rate(serial::Baud57600)?;
        settings.set_char_size(serial::Bits8);
        settings.set_parity(serial::ParityNone);
        settings.set_stop_bits(serial::Stop1);
        settings.set_flow_control(serial::FlowNone);
        Ok(())
    })?;

    port.set_timeout(Duration::from_millis(10))?;

    let init = vec![
        Command::Init(0, "01234567890123456789".to_owned()),
        Command::Init(1, "TEST WIDGET".to_owned()),
        Command::Init(2, "MORE TESTS".to_owned()),
        Command::Init(3, "RUN A PROCESS".to_owned()),
        Command::Init(4, "BREW SOME TEA".to_owned()),
        Command::Init(5, "LAUNCH THE SATELLITE".to_owned()),
    ];

    for cmd in init {
        port.set_timeout(Duration::from_millis(1000))?;
        port.write(&encode(&cmd.to_protocol_bytes()))?;
        let mut data = [0u8; 1024];
        let result = match port.read(&mut data) {
            Ok(_) => CString::new(
                data.iter()
                    .cloned()
                    .take_while(|b| *b != 0)
                    .collect::<Vec<u8>>(),
            )
            .expect("Failed to parse a c-string")
            .into_string()
            .expect("Failed to convert a c-string into a string"),
            Err(err) => return Err(err),
        };
        if result != "OK" {
            panic!("{}", result);
        }
    }

    Ok(())
}

fn main() {
    match std::env::args().nth(1) {
        Some(arg) => {
            let port =
                serial::open(&arg).expect(&format!("Failed to open the serial port {}", arg));
            start(port).expect("Port communication failed");
        }
        None => {}
    }
}
