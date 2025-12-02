use aura_rs::{BPETokenizer, CharLevelTokenizer, Encoding, UnigramTokenizer, WordPieceTokenizer};
use aura_tokenizer as aura_rs;
use pyo3::prelude::*;
use std::sync::{Arc, Mutex};

// ============= BPE Tokenizer =============
#[pyclass(name = "BPETokenizer")]
pub struct PyBPETokenizer {
    inner: Arc<Mutex<BPETokenizer>>,
}

#[pymethods]
impl PyBPETokenizer {
    #[new]
    #[pyo3(signature = (vocab_path=None, merges_path=None))]
    fn new(vocab_path: Option<String>, merges_path: Option<String>) -> Self {
        let inner = match (vocab_path, merges_path) {
            (Some(v), Some(m)) => BPETokenizer::from_files(&v, &m),
            _ => BPETokenizer::new(),
        };
        PyBPETokenizer {
            inner: Arc::new(Mutex::new(inner)),
        }
    }

    fn train(&self, files: Vec<String>, vocab_size: usize) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .train_from_files(files, vocab_size);
        Ok(())
    }

    fn encode(&self, text: String) -> PyResult<PyEncoding> {
        let encoding = self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .encode(&text);
        Ok(PyEncoding {
            input_ids: encoding.input_ids,
            attention_mask: encoding.attention_mask,
        })
    }

    fn decode(&self, ids: Vec<i32>, skip_special_tokens: Option<bool>) -> PyResult<String> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .decode(&ids, skip_special_tokens.unwrap_or(true)))
    }

    fn save(&self, path: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .save(&path);
        Ok(())
    }

    fn get_vocab_size(&self) -> PyResult<usize> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .get_vocab_size())
    }

    fn set_normalization_form(&self, form: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_normalization_form(&form);
        Ok(())
    }

    fn set_strip_accents(&self, strip: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_strip_accents(strip);
        Ok(())
    }

    fn set_lowercase(&self, lowercase: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_lowercase(lowercase);
        Ok(())
    }

    fn add_pre_tokenizer_pattern(&self, pattern: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .add_pre_tokenizer_pattern(&pattern);
        Ok(())
    }

    fn create_bert_post_processor(&self, add_special_tokens: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .create_bert_post_processor(add_special_tokens);
        Ok(())
    }

    fn add_special_tokens(&self, tokens: Vec<String>) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .add_special_tokens(tokens);
        Ok(())
    }
}

// ============= WordPiece Tokenizer =============
#[pyclass(name = "WordPieceTokenizer")]
pub struct PyWordPieceTokenizer {
    inner: Arc<Mutex<WordPieceTokenizer>>,
}

#[pymethods]
impl PyWordPieceTokenizer {
    #[new]
    #[pyo3(signature = (vocab_path=None))]
    fn new(vocab_path: Option<String>) -> Self {
        let inner = match vocab_path {
            Some(p) => WordPieceTokenizer::from_file(&p),
            None => WordPieceTokenizer::new(),
        };
        PyWordPieceTokenizer {
            inner: Arc::new(Mutex::new(inner)),
        }
    }

    fn encode(&self, text: String) -> PyResult<PyEncoding> {
        let encoding = self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .encode(&text);
        Ok(PyEncoding {
            input_ids: encoding.input_ids,
            attention_mask: encoding.attention_mask,
        })
    }

    fn decode(&self, ids: Vec<i32>, skip_special_tokens: Option<bool>) -> PyResult<String> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .decode(&ids, skip_special_tokens.unwrap_or(true)))
    }

    fn save(&self, path: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .save(&path);
        Ok(())
    }

    fn get_vocab_size(&self) -> PyResult<usize> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .get_vocab_size())
    }

    fn set_normalization_form(&self, form: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_normalization_form(&form);
        Ok(())
    }

    fn set_lowercase(&self, lowercase: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_lowercase(lowercase);
        Ok(())
    }

    fn create_bert_post_processor(&self, add_special_tokens: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .create_bert_post_processor(add_special_tokens);
        Ok(())
    }
}

// ============= Unigram Tokenizer =============
#[pyclass(name = "UnigramTokenizer")]
pub struct PyUnigramTokenizer {
    inner: Arc<Mutex<UnigramTokenizer>>,
}

#[pymethods]
impl PyUnigramTokenizer {
    #[new]
    #[pyo3(signature = (vocab_path=None))]
    fn new(vocab_path: Option<String>) -> Self {
        let inner = match vocab_path {
            Some(p) => UnigramTokenizer::from_file(&p),
            None => UnigramTokenizer::new(),
        };
        PyUnigramTokenizer {
            inner: Arc::new(Mutex::new(inner)),
        }
    }

    fn train(&self, files: Vec<String>, vocab_size: usize) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .train_from_files(files, vocab_size);
        Ok(())
    }

    fn encode(&self, text: String) -> PyResult<PyEncoding> {
        let encoding = self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .encode(&text);
        Ok(PyEncoding {
            input_ids: encoding.input_ids,
            attention_mask: encoding.attention_mask,
        })
    }

    fn decode(&self, ids: Vec<i32>, skip_special_tokens: Option<bool>) -> PyResult<String> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .decode(&ids, skip_special_tokens.unwrap_or(true)))
    }

    fn save(&self, path: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .save(&path);
        Ok(())
    }

    fn get_vocab_size(&self) -> PyResult<usize> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .get_vocab_size())
    }

    fn set_normalization_form(&self, form: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_normalization_form(&form);
        Ok(())
    }

    fn set_lowercase(&self, lowercase: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_lowercase(lowercase);
        Ok(())
    }
}

// ============= CharLevel Tokenizer =============
#[pyclass(name = "CharLevelTokenizer")]
pub struct PyCharLevelTokenizer {
    inner: Arc<Mutex<CharLevelTokenizer>>,
}

#[pymethods]
impl PyCharLevelTokenizer {
    #[new]
    fn new() -> Self {
        PyCharLevelTokenizer {
            inner: Arc::new(Mutex::new(CharLevelTokenizer::new())),
        }
    }

    fn train(&self, files: Vec<String>, vocab_size: usize) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .train_from_files(files, vocab_size);
        Ok(())
    }

    fn encode(&self, text: String) -> PyResult<PyEncoding> {
        let encoding = self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .encode(&text);
        Ok(PyEncoding {
            input_ids: encoding.input_ids,
            attention_mask: encoding.attention_mask,
        })
    }

    fn decode(&self, ids: Vec<i32>, skip_special_tokens: Option<bool>) -> PyResult<String> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .decode(&ids, skip_special_tokens.unwrap_or(true)))
    }

    fn save(&self, path: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .save(&path);
        Ok(())
    }

    fn get_vocab_size(&self) -> PyResult<usize> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .get_vocab_size())
    }

    fn set_normalization_form(&self, form: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_normalization_form(&form);
        Ok(())
    }
}

// ============= Encoding =============
#[pyclass(name = "Encoding")]
pub struct PyEncoding {
    #[pyo3(get)]
    input_ids: Vec<i32>,
    #[pyo3(get)]
    attention_mask: Vec<i32>,
}

// Legacy generic Tokenizer (kept for backward compatibility)
#[pyclass(name = "Tokenizer")]
pub struct PyTokenizer {
    inner: Arc<Mutex<aura_rs::Tokenizer>>,
}

#[pymethods]
impl PyTokenizer {
    #[new]
    #[pyo3(signature = (path=None))]
    fn new(path: Option<String>) -> Self {
        let inner = if let Some(p) = path {
            aura_rs::Tokenizer::from_file(&p)
        } else {
            aura_rs::Tokenizer::new_bpe()
        };

        PyTokenizer {
            inner: Arc::new(Mutex::new(inner)),
        }
    }

    fn encode(&self, text: String) -> PyResult<PyEncoding> {
        let encoding = self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .encode(&text);
        Ok(PyEncoding {
            input_ids: encoding.input_ids,
            attention_mask: encoding.attention_mask,
        })
    }

    fn decode(&self, ids: Vec<i32>, skip_special_tokens: Option<bool>) -> PyResult<String> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .decode(&ids, skip_special_tokens.unwrap_or(true)))
    }

    fn save(&self, path: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .save(&path);
        Ok(())
    }

    fn train(&self, files: Vec<String>, vocab_size: usize) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .train_from_files(files, vocab_size);
        Ok(())
    }

    fn get_vocab_size(&self) -> PyResult<usize> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .get_vocab_size())
    }

    fn token_to_id(&self, token: String) -> PyResult<i32> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .token_to_id(&token))
    }

    fn id_to_token(&self, id: i32) -> PyResult<String> {
        Ok(self
            .inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .id_to_token(id))
    }

    fn add_special_tokens(&self, tokens: Vec<String>) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .add_special_tokens(tokens);
        Ok(())
    }

    fn set_normalization_form(&self, form: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_normalization_form(&form);
        Ok(())
    }

    fn set_strip_accents(&self, strip: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_strip_accents(strip);
        Ok(())
    }

    fn set_lowercase(&self, lowercase: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .set_lowercase(lowercase);
        Ok(())
    }

    fn add_pre_tokenizer_pattern(&self, pattern: String) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .add_pre_tokenizer_pattern(&pattern);
        Ok(())
    }

    fn create_bert_post_processor(&self, add_special_tokens: bool) -> PyResult<()> {
        self.inner
            .lock()
            .map_err(|e| {
                PyErr::new::<pyo3::exceptions::PyRuntimeError, _>(format!("Lock error: {}", e))
            })?
            .create_bert_post_processor(add_special_tokens);
        Ok(())
    }
}

#[pymodule]
#[pyo3(name = "aura_tokenizer")]
fn aura_tokenizer_module(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_class::<PyBPETokenizer>()?;
    m.add_class::<PyWordPieceTokenizer>()?;
    m.add_class::<PyUnigramTokenizer>()?;
    m.add_class::<PyCharLevelTokenizer>()?;
    m.add_class::<PyTokenizer>()?;
    m.add_class::<PyEncoding>()?;
    Ok(())
}
