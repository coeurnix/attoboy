# Embedding

## Overview

The `attoboy::Embedding` class represents a **high-dimensional numeric vector** produced by an AI model, typically used for **semantic similarity** tasks. Instead of directly generating text, an embedding model maps a piece of text (for example, a sentence, paragraph, or document) to a vector of `float` values. Vectors for semantically similar texts end up “close together” in this high-dimensional space.

In the attoboy library:

* Embeddings are created by the `AI` class via `AI::createEmbedding`.
* `Embedding` objects are **read-only views** over these vectors.
* You can:

  * Compute **cosine similarity** between two embeddings using `compare`.
  * Inspect their dimension count via `getDimensions`.
  * Access the raw float data via `getRawArray` when you need manual processing.

You never construct an `Embedding` directly with `new` or a public constructor; instead, you ask an `AI` instance to create one for you.

### What Is an Embedding Vector?

An embedding is a vector:

* A **vector** in this context is simply an ordered list of numbers: [v₀, v₁, …, vₙ₋₁].
* Each `Embedding` exposes:

  * `getDimensions()` – how many numbers it contains.
  * `getRawArray()` – a pointer to the first element of the underlying `float` array.

The exact meaning of each dimension is model-specific and opaque. You treat the vector as a **feature representation** that captures semantic properties of the original text.

### Cosine Similarity

The main operation you perform on embeddings is **cosine similarity**, implemented by `Embedding::compare`:

* Cosine similarity measures the cosine of the angle between two vectors in high-dimensional space.
* The result is in the range **[-1, 1]**:

  * `1.0` → vectors point in exactly the same direction (maximally similar).
  * `0.0` → vectors are orthogonal (no linear correlation).
  * `-1.0` → vectors point in opposite directions (maximally dissimilar, in a specific sense).

In practice, for text embeddings produced by modern AI models:

* Values are usually between `0.0` and `1.0`.
* Higher values indicate more similar text.
* Very low values indicate unrelated content.

### Typical Workflow

A common embedding workflow in attoboy looks like this:

1. Configure an `AI` client.
2. Call `AI::createEmbedding(text)` to obtain an `Embedding`.
3. Store the embedding (or its raw floats) for later use.
4. Compare embeddings using `Embedding::compare` to measure similarity.

For example, you might:

* Build a **semantic search index** by embedding all documents and comparing user query embeddings with stored ones.
* Cluster similar texts by comparing pairwise similarity scores.
* Implement recommendation systems that find “similar items” based on their textual descriptions.

### Immutability and Lifetime

`Embedding` objects are conceptually **immutable**:

* Once created, you cannot modify the underlying vector via the public API.
* Copying or assigning an `Embedding` shares the underlying data rather than duplicating it.

Lifetime and ownership:

* The `Embedding` object manages its underlying memory via RAII.
* When the last `Embedding` referring to a particular vector is destroyed, the memory is released.
* Pointers returned by `getRawArray()` remain valid only as long as the `Embedding` object remains alive.

If `AI::createEmbedding` fails, it returns an `Embedding` with `getDimensions() == 0`. Always check the dimension before using an embedding.

---

## Reference

Each entry below covers a public constructor, method, or operator of `attoboy::Embedding`. For each, you will find:

* **Signature** – the exact declaration from the header.
* **Synopsis** – the original one-line Doxygen comment.
* **Parameters** and **Return value** descriptions.
* **In Depth** – behavior details, caveats, and a usage example.

> All examples assume `using namespace attoboy;`.

---

#### `Embedding(const Embedding &other)`

**Signature**

```cpp
Embedding(const Embedding &other);
```

**Synopsis**
Creates a copy (shares the underlying data).

**Parameters**

* `other` – Existing `Embedding` instance to copy.

**Return value**

* *(constructor; not applicable)*

**In Depth**

The copy constructor creates a new `Embedding` object that **shares** the underlying vector data with `other`:

* No new vector is allocated; the existing vector is reference-counted or otherwise shared internally.
* Both `Embedding` objects view the same data.
* Destroying one does not invalidate the other as long as at least one `Embedding` referencing the data remains.

This shallow sharing makes copying `Embedding` objects efficient, which is useful when:

* Passing them by value between functions.
* Storing them in collections managed by your own code (for example, an attoboy `List` of embeddings represented indirectly, such as by indices or external storage).

Remember that you do not construct embeddings directly; you obtain them from `AI::createEmbedding` and then copy as needed.

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding original = ai.createEmbedding("Hello world", -1, 1536);

if (original.getDimensions() > 0) {
  // Create another handle to the same underlying embedding vector.
  Embedding copy(original);

  float similarity = original.compare(copy);  // Should be very close to 1.0
}
```

*This example creates an embedding with `AI::createEmbedding` and then copies it using the copy constructor; both handles share the same underlying vector.*

---

#### `~Embedding()`

**Signature**

```cpp
~Embedding();
```

**Synopsis**
Destroys the embedding and frees resources.

**Parameters**

* *(none)*

**Return value**

* *(destructor; not applicable)*

**In Depth**

The destructor releases the `Embedding`’s handle to its underlying data:

* If this is the last `Embedding` referring to the vector, the data is freed.
* Any pointers previously obtained from `getRawArray()` become invalid after destruction.

Because `Embedding` uses RAII, you do not need to manually free memory or perform cleanup. The usual C++ lifetime rules apply: when an `Embedding` goes out of scope, its destructor runs automatically.

**Example**

```cpp
using namespace attoboy;

Embedding storedEmbedding = /* ... obtained from AI::createEmbedding ... */;

// When `storedEmbedding` goes out of scope, its destructor is called and
// its resources are released when no other references remain.
```

*This example shows an embedding whose resources are automatically reclaimed when it leaves scope.*

---

#### `Embedding &operator=(const Embedding &other)`

**Signature**

```cpp
Embedding &operator=(const Embedding &other);
```

**Synopsis**
Assigns another embedding (shares the underlying data).

**Parameters**

* `other` – Existing `Embedding` to assign from.

**Return value**

* Reference to `*this`, enabling assignment chaining.

**In Depth**

The assignment operator makes `*this` refer to the **same underlying vector** as `other`:

* Any previous association of `*this` is released; if this was the last reference to that data, it is freed.
* After assignment, `*this` and `other` share the same embedding values.
* Both can be used independently as handles to the same read-only vector.

Self-assignment is safe and handled by the implementation.

This behavior is consistent with the copy constructor: both copying and assignment share underlying data rather than deep-copying it.

**Example**

```cpp
using namespace attoboy;

Embedding e1 = /* ... */;
Embedding e2 = /* ... */;

// Now e2 will share the same vector as e1.
e2 = e1;

float sim = e1.compare(e2);  // Should be near 1.0
```

*This example assigns one embedding to another, resulting in two handles to the same underlying vector.*

---

#### `float compare(const Embedding &other) const`

**Signature**

```cpp
float compare(const Embedding &other) const;
```

**Synopsis**
Computes cosine similarity with another embedding. Returns [-1, 1].

**Parameters**

* `other` – Another `Embedding` to compare with `*this`.

**Return value**

* A `float` representing the cosine similarity between the two embeddings:

  * Range: **[-1.0, 1.0]**.
  * Higher values indicate more similar vectors.

**In Depth**

`compare` computes the **cosine similarity** between two embeddings:

* Conceptually, this measures the angle between the two vectors:

  * `1.0` → vectors are identical in direction.
  * `0.0` → vectors are orthogonal (no linear correlation).
  * `-1.0` → vectors point in opposite directions.

Practical guidance:

* For typical text embeddings from modern models:

  * Values near `1.0` mean highly similar text.
  * Values around `0.5`–`0.8` often indicate related but not identical concepts.
  * Values near `0.0` or negative indicate unrelated or strongly different content.

You should only compare embeddings that:

* Come from the **same model** (or models that intentionally share an embedding space).
* Use the **same dimensionality** (`getDimensions()` returns the same value).

Comparing embeddings with different dimensions or from unrelated models does not produce meaningful similarity scores.

Edge cases:

* If one or both embeddings have `getDimensions() == 0`, the result is undefined from your perspective; treat this as an error case and avoid calling `compare` until you have verified the dimensions.
* If the internal implementation detects incompatible embeddings, it may return a default value (for example, `0.0`); always ensure embeddings are compatible by construction, not by relying on the return value.

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding eQuery  = ai.createEmbedding("best pizza in New York", -1, 1536);
Embedding eDoc1   = ai.createEmbedding("A guide to New York pizza slices", -1, 1536);
Embedding eDoc2   = ai.createEmbedding("How to care for indoor plants", -1, 1536);

if (eQuery.getDimensions() > 0 &&
    eDoc1.getDimensions()   > 0 &&
    eDoc2.getDimensions()   > 0) {

  float sim1 = eQuery.compare(eDoc1);
  float sim2 = eQuery.compare(eDoc2);

  // sim1 should be significantly higher than sim2, indicating that
  // the pizza article is more relevant to the query than the plants article.
}
```

*This example compares a query embedding with two document embeddings and uses cosine similarity to determine which document is more relevant.*

---

#### `const float *getRawArray() const`

**Signature**

```cpp
const float *getRawArray() const;
```

**Synopsis**
Returns a pointer to the raw float array.

**Parameters**

* *(none)*

**Return value**

* A pointer to the first element of the internal `float` array representing the embedding vector.
* May be `nullptr` if the embedding is invalid (for example, `getDimensions() == 0`).

**In Depth**

`getRawArray()` provides low-level access to the embedding’s underlying data:

* The array has exactly `getDimensions()` elements.
* The data is **read-only**; you must not modify it through this pointer.
* The pointer remains valid as long as the `Embedding` object (and any shared copies) stay alive.

Typical use cases:

* Implementing your own similarity or distance metrics beyond cosine similarity.
* Exporting the vector to another system (for example, writing to a file, sending over the network, or passing into another library).
* Converting the embedding into a different representation (for example, quantized integers) for storage.

Always check `getDimensions()` before using the pointer. If the embedding is invalid (dimensions `0`), do not dereference the pointer.

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding emb = ai.createEmbedding("example text", -1, 1536);

int dims = emb.getDimensions();
if (dims > 0) {
  const float *data = emb.getRawArray();

  // For demonstration: compute the sum of all components.
  float sum = 0.0f;
  for (int i = 0; i < dims; ++i) {
    sum = sum + data[i];
  }

  // `sum` now holds the simple aggregate of the embedding components.
}
```

*This example accesses the raw float array and computes a simple aggregate over all dimensions.*

---

#### `int getDimensions() const`

**Signature**

```cpp
int getDimensions() const;
```

**Synopsis**
Returns the number of dimensions.

**Parameters**

* *(none)*

**Return value**

* The number of `float` components in the embedding vector.
* `0` if the embedding is invalid or uninitialized.

**In Depth**

`getDimensions()` tells you how many values the embedding contains. This is essential for:

* Validating that an embedding was created successfully:

  * `> 0` → valid embedding.
  * `0` → error or uninitialized embedding (for example, `AI::createEmbedding` failed).
* Ensuring that two embeddings can be meaningfully compared or processed together.
* Determining loop bounds when iterating over the raw array from `getRawArray()`.

As noted in the `AI` class documentation, the typical contract is:

* If `AI::createEmbedding` fails, it returns an `Embedding` for which `getDimensions() == 0`.

Always check this value before performing any operations that assume a non-empty vector.

**Example**

```cpp
using namespace attoboy;

AI ai(String("https://api.openai.com/v1"),
      String("sk-123456"),
      String("text-embedding-3-small"));

Embedding emb = ai.createEmbedding("hello world", -1, 1536);

int dims = emb.getDimensions();
if (dims <= 0) {
  // Embedding creation failed; handle the error.
} else {
  // Safe to use the embedding: dims components are available.
}
```

*This example checks the dimension count to determine whether embedding creation succeeded before using the embedding.*
