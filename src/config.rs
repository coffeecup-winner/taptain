use serde::Deserialize;

#[derive(Debug, Deserialize, Clone)]
pub struct Config {
    pub default_view: String,
    pub views: Vec<ViewConfig>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct ViewConfig {
    pub name: String,
    pub width: Option<u8>,
    pub height: Option<u8>,
    #[serde(default)]
    pub builtin: bool,
    #[serde(default)]
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
            if let Some(width) = view.width {
                if width > 4 {
                    panic!("View width is limited by 4");
                }
                if view
                    .height
                    .expect("View height should be set if width is set")
                    > 6
                {
                    panic!("View height is limited by 6");
                }
                if view.widgets.len() > (width * view.height.unwrap()) as usize {
                    panic!("View contains more widgets than there are slots");
                }
            } else {
                assert!(
                    view.builtin,
                    "View must be builtin if width/height are not set"
                );
                if let None = crate::views::get_view_builder(&view.name) {
                    panic!("Unknown view type {}", view.name);
                }
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
