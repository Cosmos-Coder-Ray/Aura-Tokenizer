use pyo3::prelude::*;
use pyo3::exceptions::PyValueError;
use aura_tokenizer::models::BPEModel;
use aura_tokenizer::tokenizer::Tokenizer;
use aura_tokenizer::hf_compat::Encoding;

// --- Error Conversion ---
struct AuraPyError(aura_tokenizer::error::AuraError);

impl From<AuraPyError> for PyErr {
    fn from(err: AuraPyError) -> PyErr {
        PyValueError::new_err(err.0.to_string())
    }
}

impl From<aura_tokenizer::error::AuraError> for AuraPyError {
    fn from(err: aura_tokenizer::error::AuraError) -> AuraPyError {
        AuraPyError(err)
    }
}

// --- Python Classes ---

#[pyclass(name = "Encoding")]
#[derive(Clone)]
struct PyEncoding {
    #[pyo3(get)]
    ids: Vec<u32>,
    #[pyo3(get)]
    tokens: Vec<String>,
    #[pyo3(get)]
    offsets: Vec<(usize, usize)>,
    #[pyo3(get)]
    attention_mask: Vec<u32>,
    #[pyo3(get)]
    token_type_ids: Vec<u32>,
}

impl From<Encoding> for PyEncoding {
    fn from(enc: Encoding) -> Self {
        Self {
            ids: enc.ids,
            tokens: enc.tokens,
            offsets: enc.offsets,
            attention_mask: enc.attention_mask,
            token_type_ids: enc.token_type_ids,
        }
    }
}

#[pyclass(name = "BPETokenizer")]
struct PyBPETokenizer {
    tokenizer: Tokenizer,
}

#[pymethods]
impl PyBPETokenizer {
    #[staticmethod]
    fn from_config(config_json: &str) -> PyResult<Self> {
        let model = BPEModel::from_config(config_json).map_err(AuraPyError::from)?;
        let tokenizer = Tokenizer::new(model);
        Ok(Self { tokenizer })
    }

    fn encode(&self, text: &str) -> PyResult<PyEncoding> {
        let encoding = self.tokenizer.encode(text).map_err(AuraPyError::from)?;
        Ok(encoding.into())
    }

    fn decode(&self, ids: Vec<u32>) -> PyResult<String> {
        let decoded = self.tokenizer.decode(&ids).map_err(AuraPyError::from)?;
        Ok(decoded)
    }
}

/// A Python module implemented in Rust.
#[pymodule]
fn _internal(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_class::<PyBPETokenizer>()?;
    m.add_class::<PyEncoding>()?;
    m.add_class::<PyBPETrainer>()?;
    Ok(())
}

#[pyclass(name = "BPETrainer")]
struct PyBPETrainer {
    trainer: aura_tokenizer::trainer::BPETrainer,
}

#[pymethods]
impl PyBPETrainer {
    #[staticmethod]
    fn from_config(config_json: &str) -> PyResult<Self> {
        let trainer = aura_tokenizer::trainer::BPETrainer::from_config(config_json)
            .map_err(AuraPyError::from)?;
        Ok(Self { trainer })
    }

    fn feed(&mut self, text: &str) -> PyResult<()> {
        self.trainer.feed(text).map_err(AuraPyError::from)?;
        Ok(())
    }

    fn train(&self) -> PyResult<PyBPETokenizer> {
        // This is a placeholder for the real train method, which consumes the trainer.
        // A real implementation would need to handle ownership transfer carefully.
        // For now, we'll just create a new model from a dummy config.
        let model = BPEModel::from_config("{}").map_err(AuraPyError::from)?;
        let tokenizer = Tokenizer::new(model).map_err(AuraPyError::from)?;
        Ok(PyBPETokenizer { tokenizer })
    }
}
