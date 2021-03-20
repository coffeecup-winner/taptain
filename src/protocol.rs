#[derive(Debug, Clone)]
pub enum Command {
    Init(u8, WidgetType, String),
    BeginBatch,
    EndBatch,
    Progress(u8, u8),
    Configure(u8, u8),
}

#[derive(Debug, Clone, Copy)]
pub enum WidgetType {
    Display,
    Task,
}

#[derive(Debug, Clone, Copy)]
pub enum Message {
    OK,
    Error,
    Launch(u8),
    Pause(u8),
    Resume(u8),
    Cancel(u8),
}

impl Command {
    pub fn to_protocol_bytes(&self) -> Vec<u8> {
        let mut result = vec![];
        match self {
            Command::Init(i_wigdet, type_, text) => {
                result.push(0);
                result.push(*i_wigdet);
                result.push(match type_ {
                    WidgetType::Display => 0,
                    WidgetType::Task => 1,
                });
                let slice = if text.len() > 20 {
                    &text[0..20]
                } else {
                    &text[0..text.len()]
                };
                result.extend_from_slice(slice.as_bytes());
                for _ in (slice.len() + 1)..=20 {
                    result.push(0);
                }
                result.push(0);
                assert!(result.len() == 24);
            }
            Command::BeginBatch => {
                result.push(1);
            }
            Command::EndBatch => {
                result.push(2);
            }
            Command::Progress(i_widget, percent) => {
                result.push(3);
                result.push(*i_widget);
                result.push(*percent);
            }
            Command::Configure(width, height) => {
                result.push(4);
                result.push(*width);
                result.push(*height);
            }
        }
        result
    }
}

impl Message {
    pub fn from_bytes(bytes: &[u8]) -> Message {
        if bytes.len() != 2 {
            panic!("Invalid message size");
        }
        match bytes[0] {
            1 => Message::OK,
            2 => Message::Error,
            3 => Message::Launch(bytes[1]),
            4 => Message::Pause(bytes[1]),
            5 => Message::Resume(bytes[1]),
            6 => Message::Cancel(bytes[1]),
            _ => panic!("Invalid message type {}", bytes[0]),
        }
    }
}
