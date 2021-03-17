use std::sync::{Arc, Mutex};

use crate::{comms::Connection, protocol::Message};

pub struct EventProcessor {
    connection: Arc<Mutex<Connection>>,
    init_handler: Box<dyn FnMut(Arc<Mutex<Connection>>) -> std::io::Result<()>>,
    launch_handler: Box<dyn FnMut(Arc<Mutex<Connection>>, u8) -> std::io::Result<()>>,
}

impl EventProcessor {
    pub fn new(connection: Connection) -> EventProcessor {
        EventProcessor {
            connection: Arc::new(Mutex::new(connection)),
            init_handler: Box::new(|_| Ok(())),
            launch_handler: Box::new(|_, _| Ok(())),
        }
    }

    pub fn register_init_handler(
        &mut self,
        handler: Box<dyn FnMut(Arc<Mutex<Connection>>) -> std::io::Result<()>>,
    ) {
        self.init_handler = handler;
    }

    pub fn register_launch_handler(
        &mut self,
        handler: Box<dyn FnMut(Arc<Mutex<Connection>>, u8) -> std::io::Result<()>>,
    ) {
        self.launch_handler = handler;
    }

    pub fn run_event_loop(&mut self) -> std::io::Result<()> {
        (self.init_handler)(self.connection.clone())?;
        loop {
            let message = {
                let mut guard = self.connection.lock().unwrap();
                let message = dbg!(guard.get_next_message()?);
                message
            };
            match message {
                None => std::thread::sleep(std::time::Duration::from_millis(15)),
                Some(Message::OK) => panic!("Programmer error: should be handled elsewhere"),
                Some(Message::Error) => panic!("Programmer error: should be handler elsewhere"),
                Some(Message::Launch(i_widget)) => {
                    (self.launch_handler)(self.connection.clone(), i_widget)?
                }
            }
        }
    }
}
