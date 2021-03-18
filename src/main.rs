mod comms;
mod core;
mod events;
mod protocol;

use crate::{comms::Connection, core::TaptainBackend, events::EventProcessor};

fn main() {
    match std::env::args().nth(1) {
        Some(arg) => {
            let connection = Connection::open(&arg).expect("Failed to open a connection");
            EventProcessor::<TaptainBackend>::new(connection)
                .run_event_loop()
                .expect("Port communication failed");
        }
        None => {}
    }
}
