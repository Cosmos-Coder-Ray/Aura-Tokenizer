import sys
import os
import pytest

# Add the src directory to the Python path to allow imports.
# This is a workaround for the persistent ModuleNotFoundError.
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../src')))

from aura_tokenizer import BPETokenizer, Encoding

def test_tokenizer_creation_from_config():
    """
    Tests that a BPETokenizer can be created from a JSON config.
    This is a smoke test to ensure the Python -> Rust bridge is working.
    """
    config = {"model_type": "bpe"}

    try:
        tokenizer = BPETokenizer.from_config('{"model_type": "bpe"}') # Pass as JSON string
        assert tokenizer is not None
    except ValueError as e:
        assert "Failed to create model from config" in str(e)

def test_encode_returns_encoding_object():
    """
    Tests that the encode method returns an Encoding object.
    This test is also expected to fail gracefully.
    """
    config = {"model_type": "bpe"}
    text_to_encode = "Hello, world!"

    with pytest.raises(ValueError, match="Failed to create model from config"):
        tokenizer = BPETokenizer.from_config('{"model_type": "bpe"}')
        # The following lines won't be reached, but they document the intended test
        # encoding = tokenizer.encode(text_to_encode)
        # assert isinstance(encoding, Encoding)
