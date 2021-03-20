mod builtin;
mod comms;
mod config;
mod core;
mod events;
mod protocol;
mod tasks;
mod views;

use crate::{comms::Connection, config::Config, core::TaptainBackend, events::EventProcessor};

fn main() {
    match &std::env::args().collect::<Vec<String>>()[..] {
        [_, port, config_file] => {
            let connection = Connection::open(&port).expect("Failed to open a connection");
            let config: Config = toml::from_str(
                &std::fs::read_to_string(config_file).expect("Failed to read the config file"),
            )
            .expect("Failed to parse the config file");
            config.validate();
            EventProcessor::<TaptainBackend>::new(connection, config)
                .run_event_loop()
                .expect("Port communication failed");
        }
        _ => eprintln!("Usage: taptain PORT CONFIG"),
    }
}
