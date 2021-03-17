mod comms;
mod events;
mod protocol;

use comms::Connection;
use events::EventProcessor;
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

    let mut event_processor = EventProcessor::new(connection);

    event_processor.register_launch_handler(Box::new(|conn, i_widget| {
        for i in 1..=10 {
            std::thread::sleep(std::time::Duration::from_millis(100));
            conn.send(&[Command::Progress(i_widget, i * 10)])?;
        }
        Ok(())
    }));

    event_processor.run_event_loop()
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
