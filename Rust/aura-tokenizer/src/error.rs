use thiserror::Error;

/// The main error type for the `aura-tokenizer` library.
#[derive(Error, Debug)]
pub enum AuraError {
    #[error("A null pointer was encountered at the FFI boundary.")]
    NullPointer,

    #[error("Invalid configuration provided: {0}")]
    InvalidConfig(String),

    #[error("An I/O error occurred: {0}")]
    IoError(String),

    #[error("A tokenization error occurred: {0}")]
    Tokenization(String),

    #[error("A trainer error occurred: {0}")]
    Trainer(String),

    #[error("An unknown error occurred at the C-API boundary.")]
    Unknown,

    #[error("Failed to convert string from UTF-8.")]
    FromUtf8Error(#[from] std::string::FromUtf8Error),
}

// A helper function to convert a C-API error code and message into an AuraError.
pub(crate) fn ffi_error(message: String) -> AuraError {
    // In a real implementation, we might parse the message to return a more specific error type.
    // For now, we'll use a generic error for any FFI failure.
    AuraError::Unknown // Simplified for now
}
