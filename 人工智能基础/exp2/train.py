#!/usr/bin/env python3
import torch
import torch.nn as nn
import torch.nn.functional as F
from typing import List
from math import sin, cos


class char_tokenizer:
    """
    a very simple char-based tokenizer. the tokenizer turns a string into a list of integers.
    """

    def __init__(self, corpus: List[str]):
        self.corpus = corpus
        """create a dictionary that maps each character to a unique integer"""
        self.char_to_int = {char: i for i, char in enumerate(corpus)}

    def encode(self, string: str):
        """convert a string into a list of integers and return"""
        return [self.char_to_int[char] for char in string]

    def decode(self, code: int):
        """convert a integer into a string and return"""
        return self.corpus[code]


class Head(nn.Module):
    """single head of self-attention"""

    def __init__(self, head_size):
        super().__init__()
        self.Key = nn.Linear(head_size, head_size)
        self.Query = nn.Linear(head_size, head_size)
        self.Value = nn.Linear(head_size, head_size)
        self.register_buffer("tril", torch.tril(torch.ones(block_size, block_size)))

    def forward(self, inputs):
        keys = self.Key(inputs)
        queries = self.Query(inputs)
        values = self.Value(inputs)

        attn_scores = torch.matmul(queries, keys.transpose(-2, -1)) / torch.sqrt(
            torch.tensor(inputs.shape[-1], dtype=torch.float)
        )
        attn_probs = F.softmax(
            attn_scores.masked_fill(
                self.tril[: attn_scores.size(1), : attn_scores.size(2)] == 0,
                float("-inf"),
            ),
            dim=-1,
        )
        return torch.matmul(attn_probs, values)


class MultiHeadAttention(nn.Module):
    def __init__(self, n_heads, head_size):
        super().__init__()
        self.heads = nn.ModuleList([Head(head_size) for _ in range(n_heads)])
        self.projection = nn.Linear(n_heads * head_size, head_size)

    def forward(self, inputs):
        heads_out = [head(inputs) for head in self.heads]
        out = torch.cat(heads_out, dim=-1)
        return self.projection(out)


FeedForward = lambda n_embd: nn.Sequential(
    nn.Linear(n_embd, 4 * n_embd),
    nn.ReLU(),
    nn.Linear(4 * n_embd, n_embd),
)


class Block(nn.Module):
    def __init__(self, n_embd, n_heads):
        super().__init__()
        self.attention = MultiHeadAttention(n_heads, n_embd)
        self.norm1 = nn.LayerNorm(n_embd)
        self.norm2 = nn.LayerNorm(n_embd)
        self.ff = FeedForward(n_embd)

    def forward(self, inputs):
        attention_out = self.attention(inputs)
        attention_out = self.norm1(attention_out + inputs)
        ff_out = self.ff(attention_out)
        out = self.norm2(ff_out + attention_out)
        return out


class Transformer(nn.Module):
    def __init__(self):
        super().__init__()
        self.embedding = nn.Embedding(n_vocab, n_embd)
        self.blocks = nn.ModuleList([Block(n_embd, n_heads) for _ in range(n_layers)])
        self.norm = nn.LayerNorm(n_embd)
        self.linear = nn.Linear(n_embd, n_vocab)
        self.positional_encoding = self.get_positional_encoding(block_size, n_embd)

    @staticmethod
    def get_positional_encoding(seq_len, n_embd):
        """return the positional encoding tensor of shape (seq_len, n_embd)"""
        positional_encoding = torch.zeros(seq_len, n_embd)
        for pos in range(seq_len):
            for i in range(0, n_embd, 2):
                positional_encoding[pos, i] = sin(pos / (10000 ** (i / n_embd)))
                positional_encoding[pos, i + 1] = cos(
                    pos / (10000 ** ((i + 1) / n_embd))
                )
        return positional_encoding.to(device)

    def forward(self, inputs, labels=None):
        embedding = self.embedding(inputs)

        # add positional encoding
        positional_encoding = self.positional_encoding[: embedding.size(1), :]
        embedding += positional_encoding.unsqueeze(0)

        out = self.norm(embedding)
        for block in self.blocks:
            out = block(out)
        logits = self.linear(out)

        # for inference
        if labels is None:
            return logits, None

        batch, time, channel = logits.shape
        logits = logits.view(batch * time, channel)
        labels = labels.view(batch * time)
        loss = F.cross_entropy(logits, labels)
        return logits, loss

    def generate(self, inputs, max_new_tokens):
        for _ in range(max_new_tokens):
            logits, _ = self.forward(inputs[:, -block_size:])
            token = torch.multinomial(F.softmax(logits[:, -1], dim=-1), num_samples=1)
            inputs = torch.cat((inputs, token), dim=1)
            yield token


def get_batch(split):
    data = train_data if split == "train" else val_data
    ix = torch.randint(len(data) - block_size, (batch_size,))
    x = torch.stack([data[i : i + block_size] for i in ix])
    y = torch.stack([data[i + 1 : i + block_size + 1] for i in ix])
    return x.to(device), y.to(device)


@torch.no_grad()
def estimate_loss(model):
    out = {}
    model.eval()
    for split in ["train", "val"]:
        losses = torch.zeros(eval_iters)
        for k in range(eval_iters):
            x, y = get_batch(split)
            _, loss = model(x, y)
            losses[k] = loss.item()
        out[split] = losses.mean()
    return out


def generate(model, inputs):
    print(inputs, end="", flush=True)
    context = torch.tensor(encode(inputs), device=device, dtype=torch.long).unsqueeze(0)
    for token in model.generate(context, max_new_tokens=500):
        print(decode(token), end="", flush=True)


def train(model):
    optimizer = torch.optim.AdamW(model.parameters(), lr=learning_rate)

    for iter in range(max_iters + 1):
        if iter % eval_interval == 0:
            losses = estimate_loss(model)
            print(
                f"step {iter}: train loss {losses['train']:.4f}, val loss {losses['val']:.4f}"
            )

        inputs, labels = get_batch("train")

        _, loss = model(inputs, labels)
        optimizer.zero_grad(set_to_none=True)
        loss.backward()
        optimizer.step()


# define the hyperparameters
batch_size = 16
block_size = 256
max_iters = 5000  # set the number of training iterations as you like
eval_interval = 50
learning_rate = 1e-3
device = "cuda" if torch.cuda.is_available() else "cpu"
eval_iters = 10
n_embd = 64
n_heads = 8
n_layers = 6

# read the dataset
with open("../data/input.txt", "r", encoding="utf-8") as f:
    text = f.read()
chars = sorted(list(set(text)))

# initialize the vocabulary
n_vocab = len(chars)
tokenizer = char_tokenizer(chars)
encode = tokenizer.encode
decode = tokenizer.decode

# separate the dataset into train and validation
train_data = torch.tensor(encode(text[: -len(text) // 10]), dtype=torch.long)
val_data = torch.tensor(encode(text[-len(text) // 10 :]), dtype=torch.long)

# define the model
model = Transformer().to(device)
train(model)
torch.save(model.state_dict(), "model.pt")
generate(
    model,
    "The meaning of life is ",
)
