mod comms;
mod protocol;

use comms::Connection;
use protocol::Command;

fn start(mut connection: Connection) -> std::io::Result<()> {
    connection.send(&[
        Command::Init(0, "01234567890123456789".to_owned()),
        Command::Init(1, "TEST WIDGET".to_owned()),
        Command::Init(2, "MORE TESTS".to_owned()),
        Command::Init(3, "RUN A PROCESS".to_owned()),
        Command::Init(4, "BREW SOME TEA".to_owned()),
        Command::Init(5, "LAUNCH THE SATELLITE".to_owned()),
    ])?;

    loop {
        let request = connection.get_next_request()?;
        dbg!(request);
    }
}

fn main() {
    match std::env::args().nth(1) {
        Some(arg) => {
            let connection = Connection::open(&arg).expect("Failed to open a connection");
            start(connection).expect("Port communication failed");
        }
        None => {}
    }
}
