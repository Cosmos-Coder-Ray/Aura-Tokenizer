//! Hugging Face compatibility layer.
//!
//! This module provides structs and logic to make `aura-tokenizer` compatible
//! with the expectations of the Hugging Face ecosystem, including the
//! `tokenizer.json` format and the rich `Encoding` object.

use serde::{Deserialize, Serialize};

/// Represents a rich encoding, similar to the one provided by HF's `tokenizers`.
///
/// This struct contains all the information needed for transformer models,
/// such as token IDs, attention masks, and offsets.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct Encoding {
    /// The token IDs.
    #[serde(rename = "ids")]
    pub ids: Vec<u32>,

    /// The string values of the tokens.
    #[serde(rename = "tokens")]
    pub tokens: Vec<String>,

    /// The (start, end) byte offsets of each token in the original string.
    #[serde(rename = "offsets")]
    pub offsets: Vec<(usize, usize)>,

    /// The attention mask, typically a vector of 1s.
    #[serde(rename = "attention_mask")]
    pub attention_mask: Vec<u32>,

    /// The token type IDs, used for sequence-pair tasks (e.g., all 0s for a single sequence).
    #[serde(rename = "token_type_ids")]
    pub token_type_ids: Vec<u32>,
    // Special tokens mask and overflowing tokens could be added here in the future.
}
