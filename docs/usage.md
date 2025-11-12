# Usage Guide

## Installation

```bash
pip install aura-tokenizer
```

## Tokenizer Types

### BPE Tokenizer
```python
from aura_tokenizer import BPETokenizer
bpe = BPETokenizer()
bpe.train(["Hello world!"], vocab_size=100)
ids = bpe.encode("Hello world!")
print(ids)
print(bpe.decode(ids))
```

### Unigram Tokenizer
```python
from aura_tokenizer import UnigramTokenizer
uni = UnigramTokenizer()
uni.train(["foo bar", "baz"], vocab_size=50)
ids = uni.encode("foo bar")
print(ids)
print(uni.decode(ids))
```

### CharLevel Tokenizer
```python
from aura_tokenizer import CharLevelTokenizer
char = CharLevelTokenizer()
char.train(["abc def"], vocab_size=10)
ids = char.encode("abc")
print(ids)
print(char.decode(ids))
```

### Encoder
```python
from aura_tokenizer import Encoder
enc = Encoder()
ids = enc.encode("encode this")
print(ids)
print(enc.decode(ids))
```

### WordPiece Tokenizer
```python
from aura_tokenizer import WordPieceTokenizer
wp = WordPieceTokenizer()
wp.train(["word piece example"], vocab_size=100)
ids = wp.encode("word piece example")
print(ids)
print(wp.decode(ids))
```

## Batch Encoding/Decoding
```python
batch = ["Hello world!", "How are you?"]
ids_batch = bpe.batch_encode(batch)
print(ids_batch)
print(bpe.batch_decode(ids_batch))
```

## Special Tokens
```python
bpe.add_special_tokens(["[CLS]", "[SEP]"])
print(bpe.get_special_tokens())
print(bpe.is_special_token("[CLS]"))
print(bpe.get_special_token_id(0))  # 0 = SpecialTokenType.UNK
```

## Save/Load
```python
bpe.save("bpe.model")
bpe.load("bpe.model")
```

## Vocab Management
```python
from aura_tokenizer import Vocab
vocab = Vocab()
vocab.add_token("foo", 1)
print(vocab.get_token_id("foo"))
print(vocab.get_token(1))
print(vocab.size())
```

## Config
```python
from aura_tokenizer import TokenizerConfig
config = TokenizerConfig()
config.vocab_size = 5000
print(config.to_string())
```

## Trie and DoubleArrayTrie
```python
from aura_tokenizer import Trie, DoubleArrayTrie
trie = Trie()
trie.insert("abc", 1)
print(trie.contains("abc"))
print(trie.get_id("abc"))
trie.clear()
print(trie.size())
dat = DoubleArrayTrie()
dat.build([("foo", 1), ("bar", 2)])
print(dat.find("foo"))
dat.clear()
print(dat.size())
```

## Unicode Normalization and ICU Utilities
```python
from aura_tokenizer import UnicodeNormalizer, ICU
norm = UnicodeNormalizer()
print(norm.normalize("Café", 1))  # NFC
print(ICU.is_whitespace(ord(" ")))
print(ICU.to_lower("ABC"))
print(ICU.segment_words("hello world"))
```

## WordPiece Model
```python
from aura_tokenizer import WordPieceModel
wpm = WordPieceModel()
# TODO: vocab = {"word": 1, "##piece": 2, "[UNK]": 0}
wpm.initialize({}, "[UNK]")
print(wpm.tokenize("wordpiece"))
print(wpm.batch_tokenize(["wordpiece", "example"]))
print(wpm.get_unk_token())
```

## Utility Functions
```python
from aura_tokenizer import utf8_to_codepoints_py, codepoints_to_utf8_py, is_valid_utf8_py
print(utf8_to_codepoints_py("hello"))
print(codepoints_to_utf8_py([104, 101, 108, 108, 111]))
print(is_valid_utf8_py("hello"))
```

## Enums and Constants
```python
from aura_tokenizer import SpecialTokenType, TokenizationMode
print(SpecialTokenType.UNK)
print(TokenizationMode.BERT_STYLE)
```

## Error Handling
```python
try:
    bpe.decode([999999])
except Exception as e:
    print("Error:", e)
```

## Serialization
```python
bpe.save("bpe.model")
bpe.load("bpe.model")
```

## Integration with ML/LLM
```python
import torch
input_ids = torch.tensor(ids)
```

## Advanced Pre-tokenizers
```python
from aura_tokenizer.pre_tokenizers import ByteLevelPreTokenizer, MetaspacePreTokenizer, RegexPreTokenizer, DigitPreTokenizer, SplitPreTokenizer, PreTokenizerChain
pt = ByteLevelPreTokenizer()
tokens = pt.pre_tokenize("Hello world!")
print(tokens)
# Chain example:
chain = PreTokenizerChain([RegexPreTokenizer(), DigitPreTokenizer()])
print(chain.pre_tokenize("abc 123!"))
```

## Advanced Post-processors
```python
from aura_tokenizer.post_processors import TemplatePostProcessor, ByteLevelPostProcessor, PostProcessorChain
pp = TemplatePostProcessor('[CLS] $0 [SEP]')
print(pp.post_process(["hello", "world"]))
# Chain example:
chain = PostProcessorChain([TemplatePostProcessor('[CLS] $0 [SEP]'), ByteLevelPostProcessor()])
print(chain.post_process(["h", "e", "l", "l", "o"]))
```

## Streaming & Batch Utilities
```python
from aura_tokenizer.streaming import batch_encode_stream, batch_decode_stream, mmap_file_iterator
# Efficiently process huge files:
for batch in batch_encode_stream(bpe, mmap_file_iterator('big.txt'), batch_size=1024):
    print(batch)
```

## Offset/Alignment Utilities
```python
from aura_tokenizer.offsets import token_to_char_offsets, align_tokens_to_words, batch_offset_mapping
offsets = token_to_char_offsets("Hello world!", ["Hello", "world", "!"])
print(offsets)
```

## Incremental/Streaming Training
```python
from aura_tokenizer.streaming_train import streaming_trainer
streaming_trainer(bpe, mmap_file_iterator('big.txt'), vocab_size=10000, chunk_size=100000)
```

## Serialization/Interoperability
```python
from aura_tokenizer.serialization import save_tokenizer_json, load_tokenizer_json, save_tokenizer_binary, load_tokenizer_binary
save_tokenizer_json(bpe, 'bpe.json')
bpe2 = load_tokenizer_json(BPETokenizer, 'bpe.json')
save_tokenizer_binary(bpe, 'bpe.pkl')
bpe3 = load_tokenizer_binary('bpe.pkl')
```

## Plug-in System
```python
from aura_tokenizer.plugins import plugin_registry
plugin_registry.register_plugin('my_pre', MyCustomPreTokenizer)
print(plugin_registry.list_plugins())
```

## Model-Specific Features
```python
from aura_tokenizer.model_utils import AutoTemplateHandler, pad_sequences, truncate_sequences, generate_attention_mask
handler = AutoTemplateHandler('bert')
tokens = handler.apply_template(["hello", "world"])
print(tokens)
ids = [[1,2,3], [4,5]]
padded = pad_sequences(ids, pad_id=0)
truncated = truncate_sequences(ids, max_length=2)
mask = generate_attention_mask(padded, pad_id=0)
print(padded, truncated, mask)
```

## WASM/Node.js/Browser Stubs
```python
from aura_tokenizer.wasm_stub import WASMTokenizerStub, NodeJSTokenizerStub
# All methods raise NotImplementedError (see docs/wasm.md)
```

## Hugging Face Transformers & Datasets Integration
```python
from aura_tokenizer.hf_integration import AuraTokenizerWrapper, dataset_map_fn
wrapper = AuraTokenizerWrapper(bpe)
print(wrapper("Hello world!", padding=True, truncation=True, max_length=8, return_attention_mask=True))
# For datasets:
# dataset = load_dataset(...)
# dataset = dataset.map(lambda ex: dataset_map_fn(ex, wrapper))
```

## More
See [API Reference](api.html) and [Advanced Guide](advanced.html). 