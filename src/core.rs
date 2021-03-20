use std::collections::HashMap;

use crate::{
    config::Config,
    events::{AMConnection, MessageHandler},
    protocol::Command,
    tasks::Task,
    views::View,
};

pub struct TaptainBackend {
    connection: AMConnection,
    view: View,
    _views: HashMap<String, View>,
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
            let view = if v.builtin {
                crate::views::get_view_builder(&v.name)
                    .expect("Programmer error: should have been caught by config validation")
                    .build_view()
            } else {
                View::new(v.widgets, v.width.unwrap(), v.height.unwrap())
            };
            views.insert(v.name.clone(), view);
        }
        let view = views.get(&config.default_view).unwrap().clone();
        TaptainBackend {
            connection,
            view,
            _views: views,
            tasks: vec![],
        }
    }

    fn init(&mut self) -> std::io::Result<()> {
        let mut commands = vec![Command::Configure(self.view.width(), self.view.height())];
        for (i, w) in self.view.widgets().iter().enumerate() {
            commands.push(Command::Init(i as u8, w.name.clone()));
        }
        self.connection.lock().unwrap().send(&commands)?;
        Ok(())
    }

    fn launch(&mut self, i_widget: u8) -> std::io::Result<()> {
        self.ensure_size_for_index(i_widget);
        if i_widget as usize > self.view.widgets().len() {
            return Ok(());
        }
        let launcher =
            crate::tasks::get_task_launcher(&self.view.widgets()[i_widget as usize].task.type_)
                .expect("Programmer error: should have been caught by config validation");
        let task = launcher.launch(WidgetConnection {
            connection: self.connection.clone(),
            i_widget,
        });
        self.tasks[i_widget as usize] = Some(task);
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
