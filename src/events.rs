use std::sync::{Arc, Mutex};

use crate::{comms::Connection, protocol::Message};

pub type AMConnection = Arc<Mutex<Connection>>;

pub trait MessageHandler {
    fn new(connection: AMConnection) -> Self;
    fn init(&mut self) -> std::io::Result<()>;
    fn launch(&mut self, i_widget: u8) -> std::io::Result<()>;
    fn pause(&mut self, i_widget: u8) -> std::io::Result<()>;
    fn resume(&mut self, i_widget: u8) -> std::io::Result<()>;
    fn cancel(&mut self, i_widget: u8) -> std::io::Result<()>;
}

pub struct EventProcessor<MH: MessageHandler> {
    connection: AMConnection,
    handler: MH,
}

impl<MH: MessageHandler> EventProcessor<MH> {
    pub fn new(connection: Connection) -> EventProcessor<MH> {
        let connection = Arc::new(Mutex::new(connection));
        EventProcessor {
            connection: connection.clone(),
            handler: MH::new(connection),
        }
    }

    pub fn run_event_loop(&mut self) -> std::io::Result<()> {
        self.handler.init()?;
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
                Some(Message::Launch(i_widget)) => self.handler.launch(i_widget)?,
                Some(Message::Pause(i_widget)) => self.handler.pause(i_widget)?,
                Some(Message::Resume(i_widget)) => self.handler.resume(i_widget)?,
                Some(Message::Cancel(i_widget)) => self.handler.cancel(i_widget)?,
            }
        }
    }
}
