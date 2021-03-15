#[derive(Debug)]
pub enum Command {
    Init(u8, String),
    BeginBatch,
    EndBatch,
}

#[derive(Debug)]
pub enum RequestType {
    Launch,
}

#[derive(Debug)]
pub struct Request {
    type_: RequestType,
    i_widget: u8,
}

impl Command {
    pub fn to_protocol_bytes(&self) -> Vec<u8> {
        let mut result = vec![];
        match self {
            Command::Init(i_wigdet, text) => {
                result.push(0);
                result.push(*i_wigdet);
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
                assert!(result.len() == 23);
            }
            Command::BeginBatch => {
                result.push(1);
            }
            Command::EndBatch => {
                result.push(2);
            }
        }
        result
    }
}

impl Request {
    pub fn from_bytes(bytes: &[u8]) -> Request {
        if bytes.len() != 2 {
            panic!("Invalid request size");
        }
        let type_ = match bytes[0] {
            1 => RequestType::Launch,
            _ => panic!("Invalid request type {}", bytes[0]),
        };
        Request {
            type_,
            i_widget: bytes[1],
        }
    }
}
