use std::collections::HashMap;

use crate::{
    config::Config,
    events::{AMConnection, MessageHandler},
    protocol::Command,
    tasks::Task,
    views::{View, WidgetAction},
};

pub struct TaptainBackend {
    connection: AMConnection,
    view: View,
    views: HashMap<String, View>,
    tasks: Vec<Option<Task>>,
}

impl TaptainBackend {
    fn ensure_size_for_index(&mut self, i_widget: u8) {
        let new_len = i_widget as usize + 1;
        if self.tasks.len() < new_len {
            self.tasks.resize_with(new_len, || None);
        }
    }
}

pub struct WidgetConnection {
    connection: AMConnection,
    i_widget: u8,
}

impl WidgetConnection {
    pub fn index(&self) -> u8 {
        self.i_widget
    }

    pub fn set_progress(&self, progress: u8) -> std::io::Result<()> {
        self.connection
            .lock()
            .unwrap()
            .send(&[Command::Progress(self.i_widget, progress)])
    }
}

impl MessageHandler for TaptainBackend {
    fn new(connection: AMConnection, config: Config) -> Self {
        let mut views = HashMap::new();
        for v in config.views {
            let view = crate::views::build_view(v)
                .expect("Programmer error: should have been caught by config validation");
            views.insert(view.name().to_owned(), view);
        }
        let view = views.get(&config.default_view).unwrap().clone();
        TaptainBackend {
            connection,
            view,
            views: views,
            tasks: vec![],
        }
    }

    fn init(&mut self) -> std::io::Result<()> {
        let mut commands = vec![Command::Configure(self.view.width(), self.view.height())];
        for (i, w) in self.view.widgets().iter().enumerate() {
            commands.push(Command::Init(i as u8, w.type_, w.name.clone()));
        }
        self.connection.lock().unwrap().send(&commands)?;
        for (i, w) in self.view.widgets().iter().enumerate() {
            match &w.action {
                WidgetAction::Task(launcher) if launcher.auto_launch() => {
                    launcher.launch(WidgetConnection {
                        connection: self.connection.clone(),
                        i_widget: i as u8,
                    });
                }
                _ => {}
            }
        }
        Ok(())
    }

    fn launch(&mut self, i_widget: u8) -> std::io::Result<()> {
        self.ensure_size_for_index(i_widget);
        if i_widget as usize > self.view.widgets().len() {
            return Ok(());
        }
        match &self.view.widgets()[i_widget as usize].action {
            WidgetAction::Task(launcher) => {
                let task = launcher.launch(WidgetConnection {
                    connection: self.connection.clone(),
                    i_widget,
                });
                self.tasks[i_widget as usize] = Some(task);
            }
            WidgetAction::Goto(target) => {
                self.view = self.views.get(target).expect("Invalid goto target").clone();
                return self.init();
            }
        }
        Ok(())
    }

    fn pause(&mut self, i_widget: u8) -> std::io::Result<()> {
        self.ensure_size_for_index(i_widget);
        if let Some(task) = self.tasks[i_widget as usize].as_ref() {
            task.pause();
        }
        Ok(())
    }

    fn resume(&mut self, i_widget: u8) -> std::io::Result<()> {
        self.ensure_size_for_index(i_widget);
        if let Some(task) = self.tasks[i_widget as usize].as_ref() {
            task.resume();
        }
        Ok(())
    }

    fn cancel(&mut self, i_widget: u8) -> std::io::Result<()> {
        self.ensure_size_for_index(i_widget);
        if let Some(task) = self.tasks[i_widget as usize].take() {
            task.cancel()?;
        }
        Ok(())
    }
}
