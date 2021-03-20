use sysinfo::{ProcessorExt, RefreshKind, System, SystemExt};

use crate::{
    core::WidgetConnection,
    protocol::WidgetType,
    tasks::{Task, TaskControl, TaskLauncher},
    views::{View, ViewBuilder, Widget},
};

pub struct CpuViewBuilder;
impl ViewBuilder for CpuViewBuilder {
    fn build_view(&self) -> View {
        let mut widgets = vec![];
        let cpu_count = System::new().get_processors().len().min(24);
        for i in 0..cpu_count {
            widgets.push(Widget {
                name: format!("CPU {}", i),
                type_: WidgetType::Display,
                task_launcher: Box::new(CpuTaskLauncher),
            });
        }
        let (width, height) = crate::views::widget_count_to_width_height(cpu_count as u8);
        View::new("cpu".to_owned(), widgets, width, height)
    }
}

#[derive(Debug, Clone)]
struct CpuTaskLauncher;
impl TaskLauncher for CpuTaskLauncher {
    fn auto_launch(&self) -> bool {
        true
    }

    fn launch(&self, connection: WidgetConnection) -> Task {
        Task::start(move |state| -> std::io::Result<()> {
            let mut system = System::new_with_specifics(RefreshKind::new().with_cpu());
            system.get_processors()[connection.index() as usize].get_cpu_usage();
            while !state.should_exit() {
                std::thread::sleep(std::time::Duration::from_secs(1));
                system.refresh_cpu();
                let usage = system.get_processors()[connection.index() as usize].get_cpu_usage();
                connection.set_progress(usage as u8)?;
            }
            Ok(())
        })
    }
}
