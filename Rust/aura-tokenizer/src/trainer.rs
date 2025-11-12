//! Safe wrappers for the C-API's trainer objects.

use crate::error::AuraError;
use crate::models::BPEModel;
use aura_tokenizer_sys as ffi;
use std::ffi::CString;
use std::ptr::NonNull;

// --- BPE Trainer ---

/// A safe, ergonomic wrapper around the C-API's `AuraBPETrainer`.
///
/// This struct manages the lifecycle of the underlying C++ BPE trainer object.
#[derive(Debug)]
pub struct BPETrainer {
    ptr: NonNull<ffi::AuraBPETrainer>,
}

impl BPETrainer {
    /// Creates a new BPE trainer from a JSON configuration.
    pub fn from_config(config_json: &str) -> Result<Self, AuraError> {
        let c_config = CString::new(config_json)
            .map_err(|_| AuraError::InvalidConfig("Config contains null byte".to_string()))?;

        let trainer_ptr =
            unsafe { ffi::aura_bpe_trainer_from_config(c_config.as_ptr(), config_json.len()) };

        if let Some(ptr) = NonNull::new(trainer_ptr) {
            Ok(Self { ptr })
        } else {
            Err(AuraError::Trainer(
                "Failed to create trainer from config".to_string(),
            ))
        }
    }

    /// Feeds a text corpus to the trainer. Can be called multiple times.
    pub fn feed(&mut self, text: &str) -> Result<(), AuraError> {
        let c_text = CString::new(text)
            .map_err(|_| AuraError::Trainer("Input text contains null byte".to_string()))?;

        let result =
            unsafe { ffi::aura_bpe_trainer_feed(self.ptr.as_mut(), c_text.as_ptr(), text.len()) };

        if result == ffi::AuraError_AURA_OK {
            Ok(())
        } else {
            Err(AuraError::Trainer(
                "Failed to feed text to trainer".to_string(),
            ))
        }
    }

    /// Finalizes the training and returns a new BPE model.
    pub fn train(self) -> Result<BPEModel, AuraError> {
        let model_ptr = unsafe { ffi::aura_bpe_trainer_train(self.ptr.as_ptr()) };

        if let Some(ptr) = NonNull::new(model_ptr) {
            Ok(BPEModel { ptr })
        } else {
            Err(AuraError::Trainer(
                "Training failed to produce a model".to_string(),
            ))
        }
    }
}

impl Drop for BPETrainer {
    fn drop(&mut self) {
        unsafe {
            ffi::aura_bpe_trainer_destroy(self.ptr.as_mut());
        }
    }
}
