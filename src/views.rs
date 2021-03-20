use crate::{
    config::{ViewConfig, WidgetConfig},
    tasks::TaskLauncher,
};

#[derive(Debug, Clone)]
pub struct View {
    name: String,
    widgets: Vec<Widget>,
    width: u8,
    height: u8,
}

impl View {
    pub fn new(name: String, widgets: Vec<Widget>, width: u8, height: u8) -> View {
        View {
            name,
            widgets,
            width,
            height,
        }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn widgets(&self) -> &[Widget] {
        &self.widgets[..]
    }

    pub fn width(&self) -> u8 {
        self.width
    }

    pub fn height(&self) -> u8 {
        self.height
    }
}

#[derive(Debug, Clone)]
pub struct Widget {
    pub name: String,
    pub task_launcher: Box<dyn TaskLauncher>,
}

pub trait ViewBuilder {
    fn build_view(&self) -> View;
}

pub fn build_view(config: ViewConfig) -> Option<View> {
    Some(if config.builtin {
        let builder = match config.name.as_str() {
            "cpu" => Some(Box::new(crate::builtin::cpu::CpuViewBuilder)),
            _ => None,
        };
        builder?.build_view()
    } else {
        View::new(
            config.name,
            config
                .widgets
                .into_iter()
                .map(|w| build_widget(w))
                .collect::<Option<Vec<_>>>()?,
            config.width.unwrap(),
            config.height.unwrap(),
        )
    })
}

fn build_widget(config: WidgetConfig) -> Option<Widget> {
    Some(Widget {
        name: config.name,
        task_launcher: crate::tasks::get_task_launcher(&config.task.type_)?,
    })
}

pub fn widget_count_to_width_height(count: u8) -> (u8, u8) {
    match count {
        0 => (0, 0),
        1 | 2 | 3 | 4 | 5 | 6 => (2, 3),
        7 | 8 => (2, 4),
        9 => (3, 3),
        10 | 11 | 12 => (3, 4),
        13 | 14 | 15 => (3, 5),
        16 | 17 | 18 => (3, 6),
        19 | 20 => (4, 5),
        21 | 22 | 23 | 24 => (4, 6),
        _ => panic!("Invalid widget count {}", count),
    }
}
