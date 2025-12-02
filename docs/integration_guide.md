# Integration Guide

Learn how to integrate Aura Tokenizer with popular machine learning frameworks and tools.

## Table of Contents

- [HuggingFace Transformers](#huggingface-transformers)
- [PyTorch](#pytorch)
- [TensorFlow](#tensorflow)
- [FastAPI](#fastapi)
- [Deployment](#deployment)

---

## HuggingFace Transformers

### Custom Tokenizer Integration

Aura Tokenizer can be used with HuggingFace models by creating a custom PreTrainedTokenizer:

```python
from transformers import PreTrainedTokenizer
from aura_tokenizer import BPETokenizer
from typing import List, Optional

class AuraHFTokenizer(PreTrainedTokenizer):
    def __init__(self, vocab_file, merges_file, **kwargs):
        super().__init__(**kwargs)
        self.tokenizer = BPETokenizer(vocab_file, merges_file)
    
    def _tokenize(self, text: str, **kwargs) -> List[str]:
        """Tokenize a string into tokens."""
        encoding = self.tokenizer.encode(text)
        return [self.tokenizer.id_to_token(id) for id in encoding.input_ids]
    
    def _convert_token_to_id(self, token: str) -> int:
        """Convert a token to its ID."""
        return self.tokenizer.token_to_id(token)
    
    def _convert_id_to_token(self, index: int) -> str:
        """Convert an ID to its token."""
        return self.tokenizer.id_to_token(index)
    
    def get_vocab(self) -> dict:
        """Return the vocabulary dictionary."""
        vocab_size = self.tokenizer.get_vocab_size()
        return {self.tokenizer.id_to_token(i): i for i in range(vocab_size)}
    
    @property
    def vocab_size(self) -> int:
        return self.tokenizer.get_vocab_size()
    
    def save_vocabulary(self, save_directory: str, filename_prefix: Optional[str] = None):
        """Save the tokenizer vocabulary."""
        import os
        vocab_file = os.path.join(save_directory, (filename_prefix + "-" if filename_prefix else "") + "vocab.json")
        self.tokenizer.save(vocab_file)
        return (vocab_file,)

# Usage
tokenizer = AuraHFTokenizer(vocab_file="vocab.json", merges_file="merges.txt")

# Use with HuggingFace models
from transformers import AutoModel

model = AutoModel.from_pretrained("bert-base-uncased")
inputs = tokenizer("Hello, world!", return_tensors="pt")
outputs = model(**inputs)
```

### Dataset Processing

```python
from datasets import load_dataset
from aura_tokenizer import BPETokenizer

tokenizer = BPETokenizer()
dataset = load_dataset("wikitext", "wikitext-2-raw-v1")

def tokenize_function(examples):
    encodings = [tokenizer.encode(text) for text in examples["text"]]
    return {
        "input_ids": [enc.input_ids for enc in encodings],
        "attention_mask": [enc.attention_mask for enc in encodings]
    }

tokenized_dataset = dataset.map(tokenize_function, batched=True)
```

---

## PyTorch

### Custom Dataset

```python
import torch
from torch.utils.data import Dataset, DataLoader
from aura_tokenizer import BPETokenizer

class TextDataset(Dataset):
    def __init__(self, texts: List[str], tokenizer: BPETokenizer, max_length: int = 512):
        self.texts = texts
        self.tokenizer = tokenizer
        self.max_length = max_length
    
    def __len__(self):
        return len(self.texts)
    
    def __getitem__(self, idx):
        text = self.texts[idx]
        encoding = self.tokenizer.encode(text)
        
        # Pad/truncate to max_length
        input_ids = encoding.input_ids[:self.max_length]
        attention_mask = encoding.attention_mask[:self.max_length]
        
        # Pad if needed
        padding_length = self.max_length - len(input_ids)
        input_ids += [0] * padding_length
        attention_mask += [0] * padding_length
        
        return {
            "input_ids": torch.tensor(input_ids, dtype=torch.long),
            "attention_mask": torch.tensor(attention_mask, dtype=torch.long)
        }

# Usage
tokenizer = BPETokenizer()
texts = ["Hello world", "Another example", "More text"]
dataset = TextDataset(texts, tokenizer)
dataloader = DataLoader(dataset, batch_size=2, shuffle=True)

for batch in dataloader:
    input_ids = batch["input_ids"]
    attention_mask = batch["attention_mask"]
    # Feed to model
```

### Training Loop Integration

```python
import torch
import torch.nn as nn
from aura_tokenizer import BPETokenizer

class SimpleModel(nn.Module):
    def __init__(self, vocab_size, embed_dim=128):
        super().__init__()
        self.embedding = nn.Embedding(vocab_size, embed_dim)
        self.fc = nn.Linear(embed_dim, vocab_size)
    
    def forward(self, input_ids):
        embeds = self.embedding(input_ids)
        return self.fc(embeds.mean(dim=1))

tokenizer = BPETokenizer()
model = SimpleModel(vocab_size=tokenizer.get_vocab_size())

# Training
optimizer = torch.optim.Adam(model.parameters())
criterion = nn.CrossEntropyLoss()

for epoch in range(10):
    for batch in dataloader:
        optimizer.zero_grad()
        
        outputs = model(batch["input_ids"])
        loss = criterion(outputs, batch["labels"])
        
        loss.backward()
        optimizer.step()
```

---

## TensorFlow

### tf.data Integration

```python
import tensorflow as tf
from aura_tokenizer import BPETokenizer

tokenizer = BPETokenizer()

def encode_text(text):
    """Encode text using Aura Tokenizer."""
    encoding = tokenizer.encode(text.numpy().decode('utf-8'))
    return encoding.input_ids, encoding.attention_mask

def tf_encode(text):
    """TensorFlow wrapper for encoding."""
    input_ids, attention_mask = tf.py_function(
        encode_text,
        inp=[text],
        Tout=[tf.int32, tf.int32]
    )
    input_ids.set_shape([None])
    attention_mask.set_shape([None])
    return {"input_ids": input_ids, "attention_mask": attention_mask}

# Create dataset
texts = ["Hello world", "TensorFlow example", "More data"]
dataset = tf.data.Dataset.from_tensor_slices(texts)
dataset = dataset.map(tf_encode)
dataset = dataset.padded_batch(2, padded_shapes={"input_ids": [512], "attention_mask": [512]})

for batch in dataset:
    print(batch["input_ids"].shape)  # (batch_size, 512)
```

### Keras Model

```python
from tensorflow import keras
from tensorflow.keras import layers

class TokenizerLayer(layers.Layer):
    def __init__(self, tokenizer, **kwargs):
        super().__init__(**kwargs)
        self.tokenizer = tokenizer
    
    def call(self, inputs):
        # Note: This is a simplified example
        # For production, use tf.py_function
        return inputs

vocab_size = tokenizer.get_vocab_size()
model = keras.Sequential([
    layers.Embedding(vocab_size, 128),
    layers.GlobalAveragePooling1D(),
    layers.Dense(64, activation='relu'),
    layers.Dense(1, activation='sigmoid')
])

model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
```

---

## FastAPI

### REST API Endpoint

```python
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from aura_tokenizer import BPETokenizer
from typing import List

app = FastAPI(title="Aura Tokenizer API")

# Initialize tokenizer
tokenizer = BPETokenizer()

class EncodeRequest(BaseModel):
    text: str
    skip_special_tokens: bool = True

class EncodeResponse(BaseModel):
    input_ids: List[int]
    attention_mask: List[int]

class DecodeRequest(BaseModel):
    ids: List[int]
    skip_special_tokens: bool = True

@app.post("/encode", response_model=EncodeResponse)
async def encode_text(request: EncodeRequest):
    """Encode text into token IDs."""
    try:
        encoding = tokenizer.encode(request.text)
        return EncodeResponse(
            input_ids=encoding.input_ids,
            attention_mask=encoding.attention_mask
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/decode")
async def decode_ids(request: DecodeRequest):
    """Decode token IDs back to text."""
    try:
        text = tokenizer.decode(request.ids, request.skip_special_tokens)
        return {"text": text}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/vocab-size")
async def get_vocab_size():
    """Get vocabulary size."""
    return {"vocab_size": tokenizer.get_vocab_size()}

# Run with: uvicorn main:app --reload
```

### Batch Processing

```python
from fastapi import BackgroundTasks

class BatchEncodeRequest(BaseModel):
    texts: List[str]

@app.post("/encode-batch")
async def encode_batch(request: BatchEncodeRequest, background_tasks: BackgroundTasks):
    """Encode multiple texts in parallel."""
    from concurrent.futures import ThreadPoolExecutor
    
    with ThreadPoolExecutor(max_workers=4) as executor:
        encodings = list(executor.map(tokenizer.encode, request.texts))
    
    return {
        "results": [
            {
                "input_ids": enc.input_ids,
                "attention_mask": enc.attention_mask
            }
            for enc in encodings
        ]
    }
```

---

## Deployment

### Docker

```dockerfile
FROM python:3.10-slim

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install Rust
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"

# Copy project
WORKDIR /app
COPY . .

# Build and install
RUN cd python && pip install maturin && maturin build --release
RUN pip install target/wheels/*.whl

# Run
CMD ["uvicorn", "api:app", "--host", "0.0.0.0", "--port", "8000"]
```

### Kubernetes

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: tokenizer-api
spec:
  replicas: 3
  selector:
    matchLabels:
      app: tokenizer-api
  template:
    metadata:
      labels:
        app: tokenizer-api
    spec:
      containers:
      - name: api
        image: aura-tokenizer:latest
        ports:
        - containerPort: 8000
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1"
        env:
        - name: WORKERS
          value: "4"
---
apiVersion: v1
kind: Service
metadata:
  name: tokenizer-service
spec:
  selector:
    app: tokenizer-api
  ports:
  - port: 80
    targetPort: 8000
  type: LoadBalancer
```

### AWS Lambda

```python
# lambda_function.py
import json
from aura_tokenizer import BPETokenizer

# Initialize outside handler for reuse
tokenizer = BPETokenizer()

def lambda_handler(event, context):
    """AWS Lambda handler."""
    body = json.loads(event['body'])
    text = body.get('text', '')
    
    encoding = tokenizer.encode(text)
    
    return {
        'statusCode': 200,
        'body': json.dumps({
            'input_ids': encoding.input_ids,
            'attention_mask': encoding.attention_mask
        })
    }
```

---

## Performance Optimization

### Caching

```python
from functools import lru_cache
from aura_tokenizer import BPETokenizer

tokenizer = BPETokenizer()

@lru_cache(maxsize=10000)
def cached_encode(text: str):
    """Cache frequently encoded texts."""
    encoding = tokenizer.encode(text)
    return tuple(encoding.input_ids), tuple(encoding.attention_mask)

# Usage
ids, mask = cached_encode("Frequently used text")
```

### Multiprocessing

```python
from multiprocessing import Pool
from aura_tokenizer import BPETokenizer

def encode_worker(text):
    tokenizer = BPETokenizer()  # Each process gets its own tokenizer
    return tokenizer.encode(text)

if __name__ == "__main__":
    texts = ["text1", "text2", "text3", ...]
    
    with Pool(processes=4) as pool:
        encodings = pool.map(encode_worker, texts)
```

---

## See Also

- [API Reference](api_reference.md)
- [Usage Guide](usage.md)
- [Performance Benchmarks](benchmarks.md)
