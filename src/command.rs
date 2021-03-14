pub enum Command {
    Init(u8, String),
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
        }
        result
    }
}
