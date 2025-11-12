//! Safe wrappers around the C-API's model objects.

use crate::error::AuraError;
use aura_tokenizer_sys as ffi;
use std::ffi::CString;
use std::ptr::NonNull;

// --- BPE Model ---

/// A safe, ergonomic wrapper around the C-API's `AuraBPEModel`.
///
/// This struct manages the lifecycle of the underlying C++ BPE model object,
/// automatically freeing it when it's dropped.
#[derive(Debug)]
pub struct BPEModel {
    pub(crate) ptr: NonNull<ffi::AuraBPEModel>,
}

impl BPEModel {
    /// Creates a new BPE model from a JSON configuration.
    pub fn from_config(config_json: &str) -> Result<Self, AuraError> {
        let c_config = CString::new(config_json)
            .map_err(|_| AuraError::InvalidConfig("Config contains null byte".to_string()))?;

        // Unsafe call to the C-API
        let model_ptr =
            unsafe { ffi::aura_bpe_model_from_config(c_config.as_ptr(), config_json.len()) };

        // Check for null pointer, which indicates an error from the C-API
        if let Some(ptr) = NonNull::new(model_ptr) {
            Ok(Self { ptr })
        } else {
            // In a real scenario, we would call `aura_get_last_error_message` here.
            Err(AuraError::InvalidConfig(
                "Failed to create model from config".to_string(),
            ))
        }
    }
}

/// Implement the Drop trait to automatically clean up the C++ object.
impl Drop for BPEModel {
    fn drop(&mut self) {
        // Unsafe call to the C-API's destructor function.
        // This is safe because the pointer is guaranteed to be non-null.
        unsafe {
            ffi::aura_bpe_model_destroy(self.ptr.as_mut());
        }
    }
}

// Marking the struct as Send + Sync.
// This is safe because we are assuming the underlying C++ object is thread-safe,
// as mandated by the project requirements. If it weren't, we would need to
// wrap the pointer in a Mutex or RwLock.
unsafe impl Send for BPEModel {}
unsafe impl Sync for BPEModel {}
