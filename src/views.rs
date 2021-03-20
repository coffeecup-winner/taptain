use crate::config::{TaskConfig, WidgetConfig};

#[derive(Debug, Clone)]
pub struct View {
    widgets: Vec<WidgetConfig>,
}

impl View {
    pub fn new(widgets: Vec<WidgetConfig>) -> View {
        View { widgets }
    }

    pub fn widgets(&self) -> &[WidgetConfig] {
        &self.widgets[..]
    }
}

pub trait ViewBuilder {
    fn build_view(&self) -> View;
}

struct CpuViewBuilder;

impl ViewBuilder for CpuViewBuilder {
    fn build_view(&self) -> View {
        let mut widgets = vec![];
        // TODO: N
        for i in 0..6 {
            widgets.push(WidgetConfig {
                name: format!("CPU {}", i),
                task: TaskConfig {
                    type_: "cpu".to_owned(),
                },
            });
        }
        View { widgets }
    }
}

pub fn get_view_builder(name: &str) -> Option<Box<dyn ViewBuilder>> {
    match name {
        "cpu" => Some(Box::new(CpuViewBuilder)),
        _ => None,
    }
}
