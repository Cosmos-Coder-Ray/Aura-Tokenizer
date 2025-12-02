import pytest
import os
from aura_tokenizer import Tokenizer

@pytest.fixture
def corpus_file(tmp_path):
    f = tmp_path / "corpus.txt"
    f.write_text("Hello world! This is a test corpus for Aura Tokenizer.", encoding="utf-8")
    return str(f)

def test_full_pipeline(corpus_file):
    # 1. Initialize
    tokenizer = Tokenizer()

    # 2. Train
    tokenizer.train([corpus_file], 1000)

    # 3. Configure
    tokenizer.set_normalization_form("NFC")
    tokenizer.set_lowercase(True)
    tokenizer.create_bert_post_processor(True)

    # 4. Encode
    text = "Hello World"
    encoding = tokenizer.encode(text)
    
    assert len(encoding.input_ids) > 0
    print(f"IDs: {encoding.input_ids}")

    # 5. Decode
    decoded = tokenizer.decode(encoding.input_ids)
    assert "hello" in decoded.lower()

def test_special_tokens():
    tokenizer = Tokenizer()
    tokenizer.add_special_tokens(["[MY_TOKEN]"])
    assert tokenizer.token_to_id("[MY_TOKEN]") != -1
