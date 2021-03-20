use crate::config::WidgetConfig;

#[derive(Debug, Clone)]
pub struct View {
    widgets: Vec<WidgetConfig>,
    width: u8,
    height: u8,
}

impl View {
    pub fn new(widgets: Vec<WidgetConfig>, width: u8, height: u8) -> View {
        View {
            widgets,
            width,
            height,
        }
    }

    pub fn widgets(&self) -> &[WidgetConfig] {
        &self.widgets[..]
    }

    pub fn width(&self) -> u8 {
        self.width
    }

    pub fn height(&self) -> u8 {
        self.height
    }
}

pub trait ViewBuilder {
    fn build_view(&self) -> View;
}

pub fn get_view_builder(name: &str) -> Option<Box<dyn ViewBuilder>> {
    match name {
        "cpu" => Some(Box::new(crate::builtin::cpu::CpuViewBuilder)),
        _ => None,
    }
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
