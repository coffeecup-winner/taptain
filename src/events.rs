use crate::{comms::Connection, protocol::Message};

pub struct EventProcessor {
    connection: Connection,
    launch_handler: Box<dyn FnMut(&mut Connection, u8) -> std::io::Result<()>>,
}

impl EventProcessor {
    pub fn new(connection: Connection) -> EventProcessor {
        EventProcessor {
            connection,
            launch_handler: Box::new(|_, _| Ok(())),
        }
    }

    pub fn register_launch_handler(
        &mut self,
        handler: Box<dyn FnMut(&mut Connection, u8) -> std::io::Result<()>>,
    ) {
        self.launch_handler = handler;
    }

    pub fn run_event_loop(&mut self) -> std::io::Result<()> {
        loop {
            let message = dbg!(self.connection.get_next_message()?);
            self.dispatch(message)?;
        }
    }

    fn dispatch(&mut self, message: Message) -> std::io::Result<()> {
        match message {
            Message::OK => panic!("Programmer error: should be handled elsewhere"),
            Message::Error => panic!("Programmer error: should be handler elsewhere"),
            Message::Launch(i_widget) => (self.launch_handler)(&mut self.connection, i_widget),
        }
    }
}
