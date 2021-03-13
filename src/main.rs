use std::{io::{Read, Write}, time::Duration};

use serial::SerialPort;

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

    for x in 20..=40u8 {
        for y in 20..=40u8 {
            port.write(&[x, y, b'\n'])?;
            let mut data = [0u8; 1024];
            match port.read(&mut data) {
                Ok(_) => panic!("{}", String::from_utf8_lossy(&data)),
                Err(err) if err.kind() == std::io::ErrorKind::TimedOut => {},
                Err(err) => return Err(err),
            }
        }
    }

    Ok(())
}

fn main() {
    match std::env::args().nth(1) {
        Some(arg) => {
            let port = serial::open(&arg).expect(&format!("Failed to open the serial port {}", arg));
            start(port).expect("Port communication failed");
        }
        None => {}
    }
}
