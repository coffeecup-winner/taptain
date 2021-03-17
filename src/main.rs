mod comms;
mod events;
mod protocol;

use comms::Connection;
use events::EventProcessor;
use protocol::Command;

fn start(mut event_processor: EventProcessor) -> std::io::Result<()> {
    event_processor.register_init_handler(Box::new(|connection| {
        connection.send(&[
            Command::Init(0, "01234567890123456789".to_owned()),
            Command::Init(1, "TEST WIDGET".to_owned()),
            Command::Init(2, "MORE TESTS".to_owned()),
            Command::Init(3, "RUN A PROCESS".to_owned()),
            Command::Init(4, "BREW SOME TEA".to_owned()),
            Command::Init(5, "LAUNCH THE SATELLITE".to_owned()),
        ])?;
        Ok(())
    }));

    event_processor.register_launch_handler(Box::new(|connection, i_widget| {
        for i in 1..=10 {
            std::thread::sleep(std::time::Duration::from_millis(100));
            connection.send(&[Command::Progress(i_widget, i * 10)])?;
        }
        Ok(())
    }));

    event_processor.run_event_loop()
}

fn main() {
    match std::env::args().nth(1) {
        Some(arg) => {
            let connection = Connection::open(&arg).expect("Failed to open a connection");
            start(EventProcessor::new(connection)).expect("Port communication failed");
        }
        None => {}
    }
}
