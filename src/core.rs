use std::{
    sync::{Arc, Mutex},
    thread::JoinHandle,
};

use crate::{
    events::{AMConnection, MessageHandler},
    protocol::Command,
};

#[derive(Debug, Clone, Copy)]
enum TaskState {
    Running,
    Paused,
    Canceled,
}

// Task state trait to be used by the task thread
trait TaskControl {
    // This function will block if the task is paused
    fn should_exit(&self) -> bool;
}

impl TaskControl for Arc<Mutex<TaskState>> {
    fn should_exit(&self) -> bool {
        loop {
            let state = *self.lock().unwrap();
            match state {
                TaskState::Running => return false,
                TaskState::Paused => std::thread::park(),
                TaskState::Canceled => return true,
            }
        }
    }
}

struct Task {
    pub state: Arc<Mutex<TaskState>>,
    pub join_handle: JoinHandle<std::io::Result<()>>,
}

impl Task {
    pub fn start<F>(f: F) -> Task
    where
        F: FnOnce(Arc<Mutex<TaskState>>) -> std::io::Result<()>,
        F: Send + 'static,
    {
        let state = Arc::new(Mutex::new(TaskState::Running));
        let state_ = state.clone();
        let join_handle = std::thread::spawn(move || f(state_));
        Task { state, join_handle }
    }

    pub fn pause(&self) {
        *self.state.lock().unwrap() = TaskState::Paused;
    }

    pub fn resume(&self) {
        *self.state.lock().unwrap() = TaskState::Running;
        self.join_handle.thread().unpark();
    }

    pub fn cancel(self) -> std::io::Result<()> {
        *self.state.lock().unwrap() = TaskState::Canceled;
        self.join_handle.thread().unpark();
        return self.join_handle.join().unwrap();
    }
}

pub struct TaptainBackend {
    connection: AMConnection,
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

impl MessageHandler for TaptainBackend {
    fn new(connection: AMConnection) -> Self {
        TaptainBackend {
            connection,
            tasks: vec![],
        }
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
        self.ensure_size_for_index(i_widget);
        let connection = self.connection.clone();
        let task = Task::start(move |state| -> std::io::Result<()> {
            for i in 1..=10 {
                std::thread::sleep(std::time::Duration::from_millis(100));
                if state.should_exit() {
                    return Ok(());
                }
                connection
                    .lock()
                    .unwrap()
                    .send(&[Command::Progress(i_widget, i * 10)])?;
            }
            Ok(())
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
