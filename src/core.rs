use crate::{
    events::{AMConnection, MessageHandler},
    protocol::Command,
};

pub struct TaptainBackend {
    connection: AMConnection,
}

impl MessageHandler for TaptainBackend {
    fn new(connection: AMConnection) -> Self {
        TaptainBackend { connection }
    }

    fn init(&mut self) -> std::io::Result<()> {
        self.connection.lock().unwrap().send(&[
            Command::Init(0, "01234567890123456789".to_owned()),
            Command::Init(1, "TEST WIDGET".to_owned()),
            Command::Init(2, "MORE TESTS".to_owned()),
            Command::Init(3, "RUN A PROCESS".to_owned()),
            Command::Init(4, "BREW SOME TEA".to_owned()),
            Command::Init(5, "LAUNCH THE SATELLITE".to_owned()),
        ])?;
        Ok(())
    }

    fn launch(&mut self, i_widget: u8) -> std::io::Result<()> {
        let connection = self.connection.clone();
        std::thread::spawn(move || -> std::io::Result<()> {
            for i in 1..=10 {
                std::thread::sleep(std::time::Duration::from_millis(100));
                connection
                    .lock()
                    .unwrap()
                    .send(&[Command::Progress(i_widget, i * 10)])?;
            }
            Ok(())
        });
        // TODO: track this thread
        Ok(())
    }
}
