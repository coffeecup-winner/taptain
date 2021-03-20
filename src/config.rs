use serde::Deserialize;

#[derive(Debug, Deserialize, Clone)]
pub struct Config {
    pub default_view: String,
    pub views: Vec<ViewConfig>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct ViewConfig {
    pub name: String,
    pub width: u8,
    pub height: u8,
    pub widgets: Vec<WidgetConfig>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct WidgetConfig {
    pub name: String,
    pub task: TaskConfig,
}

#[derive(Debug, Deserialize, Clone)]
pub struct TaskConfig {
    #[serde(rename(deserialize = "type"))]
    pub type_: String,
}

impl Config {
    pub fn validate(&self) {
        for view in &self.views {
            if view.width > 4 {
                panic!("View width is limited by 4");
            }
            if view.height > 6 {
                panic!("View height is limited by 6");
            }
            if view.widgets.len() > (view.width * view.height) as usize {
                panic!("View contains more widgets than there are slots");
            }
            for widget in &view.widgets {
                if widget.name.len() > 20 {
                    panic!("Widget name is limited by 20 characters");
                }
                if let None = crate::tasks::get_task_launcher(&widget.task.type_) {
                    panic!("Unknown task type {}", widget.task.type_);
                }
            }
        }
    }
}
