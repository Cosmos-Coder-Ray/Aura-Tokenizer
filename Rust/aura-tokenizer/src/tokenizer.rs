//! The core Tokenizer struct and its main functionality.

use crate::error::AuraError;
use crate::hf_compat::Encoding;
use crate::models::BPEModel;
use aura_tokenizer_sys as ffi;
use std::ffi::{CStr, CString};
use std::ptr::NonNull;

/// The main tokenizer interface.
///
/// This struct wraps a model and provides the core encoding and decoding methods.
#[derive(Debug)]
pub struct Tokenizer {
    ptr: NonNull<ffi::AuraTokenizer>,
}

impl Tokenizer {
    /// Creates a new tokenizer from a given model.
    pub fn new(model: BPEModel) -> Result<Self, AuraError> {
        // Unsafe call to the C-API to create the tokenizer.
        let tokenizer_ptr = unsafe { ffi::aura_tokenizer_create(model.ptr.as_ptr()) };

        if let Some(ptr) = NonNull::new(tokenizer_ptr) {
            Ok(Self { ptr })
        } else {
            Err(AuraError::Unknown) // Assuming null means creation failed
        }
    }

    /// Encodes a string into a rich `Encoding` object.
    pub fn encode(&self, text: &str) -> Result<Encoding, AuraError> {
        let c_text = CString::new(text)
            .map_err(|_| AuraError::Tokenization("Input text contains null byte".to_string()))?;

        let raw_encoding_ptr =
            unsafe { ffi::aura_tokenizer_encode(self.ptr.as_ptr(), c_text.as_ptr(), text.len()) };

        if let Some(raw_encoding) = unsafe { raw_encoding_ptr.as_ref() } {
            let mut ids = Vec::with_capacity(raw_encoding.num_tokens);
            let mut tokens = Vec::with_capacity(raw_encoding.num_tokens);
            let mut offsets = Vec::with_capacity(raw_encoding.num_tokens);

            for i in 0..raw_encoding.num_tokens {
                let token = unsafe { *raw_encoding.tokens.add(i) };
                ids.push(token.id);
                let token_value =
                    unsafe { CStr::from_ptr(token.value).to_string_lossy().into_owned() };
                tokens.push(token_value);
                offsets.push((token.start_offset, token.end_offset));
            }

            // Clean up the C-API object
            unsafe { ffi::aura_encoding_destroy(raw_encoding_ptr) };

            let num_tokens = ids.len();
            Ok(Encoding {
                ids,
                tokens,
                offsets,
                attention_mask: vec![1; num_tokens],
                token_type_ids: vec![0; num_tokens],
            })
        } else {
            Err(AuraError::Tokenization("Encoding failed".to_string()))
        }
    }

    /// Decodes a sequence of token IDs back into a string.
    pub fn decode(&self, ids: &[u32]) -> Result<String, AuraError> {
        let c_str_ptr =
            unsafe { ffi::aura_tokenizer_decode(self.ptr.as_ptr(), ids.as_ptr(), ids.len()) };

        if c_str_ptr.is_null() {
            return Err(AuraError::Tokenization("Decoding failed".to_string()));
        }

        // Convert the C string to a Rust String
        let result = unsafe { CStr::from_ptr(c_str_ptr).to_string_lossy().into_owned() };

        // Free the string that was allocated by the C++ library
        unsafe {
            ffi::aura_string_destroy(c_str_ptr);
        }

        Ok(result)
    }
}

impl Drop for Tokenizer {
    fn drop(&mut self) {
        // Unsafe call to the C-API's destructor function.
        unsafe {
            ffi::aura_tokenizer_destroy(self.ptr.as_mut());
        }
    }
}
