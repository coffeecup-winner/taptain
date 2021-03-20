use std::{
    sync::{Arc, Mutex},
    thread::JoinHandle,
};

use crate::core::WidgetConnection;

#[derive(Debug, Clone, Copy)]
pub enum TaskState {
    Running,
    Paused,
    Canceled,
}

// Task state trait to be used by the task thread
pub trait TaskControl {
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

pub struct Task {
    state: Arc<Mutex<TaskState>>,
    join_handle: JoinHandle<std::io::Result<()>>,
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

pub trait TaskLauncher: std::fmt::Debug + dyn_clone::DynClone {
    fn auto_launch(&self) -> bool;
    fn launch(&self, connection: WidgetConnection) -> Task;
}
dyn_clone::clone_trait_object!(TaskLauncher);

#[derive(Debug, Clone)]
struct ExampleTaskLauncher;
impl TaskLauncher for ExampleTaskLauncher {
    fn auto_launch(&self) -> bool {
        false
    }

    fn launch(&self, connection: WidgetConnection) -> Task {
        Task::start(move |state| -> std::io::Result<()> {
            for i in 1..=10 {
                std::thread::sleep(std::time::Duration::from_millis(100));
                if state.should_exit() {
                    return Ok(());
                }
                connection.set_progress(i * 10)?;
            }
            Ok(())
        })
    }
}

pub fn get_task_launcher(name: &str) -> Option<Box<dyn TaskLauncher>> {
    match name {
        "example" => Some(Box::new(ExampleTaskLauncher)),
        _ => None,
    }
}
