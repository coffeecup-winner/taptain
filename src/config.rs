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
    #[serde(rename(deserialize = "type"))]
    pub type_: String,
    pub action: String,
    #[serde(default)]
    pub target: String,
    pub task: Option<TaskConfig>,
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
                if let None = crate::views::build_view(view.clone()) {
                    panic!("Failed to build view {}", view.name);
                }
            }
            for widget in &view.widgets {
                if widget.name.len() > 20 {
                    panic!("Widget name is limited by 20 characters");
                }
                match widget.type_.as_str() {
                    "display" => {}
                    "task" => {}
                    unknown => panic!("Unknown widget type {}", unknown),
                }
                match widget.action.as_str() {
                    "task" => match &widget.task {
                        Some(task) => {
                            if let None = crate::tasks::get_task_launcher(&task.type_) {
                                panic!("Unknown task type {}", task.type_);
                            }
                        }
                        None => panic!("Widget task is not set"),
                    },
                    "goto" => assert!(widget.target.len() > 0, "Widget goto target is not set"),
                    _ => panic!("Unknown widget type {}", widget.type_),
                }
            }
        }
    }
}
