# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/hash.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Hash Utilities (`hash.inc`)

Core hashing and fingerprinting utilities for the NUOS platform. Provides specialized algorithms for content similarity detection, secure hashing, and compact representation of data. All functions operate in a multibyte-safe manner and are optimized for performance.

---

### `fingerprint`

Generates a **winnowing fingerprint** for a given string, used to detect near-duplicate content or measure similarity between documents. The algorithm identifies stable, noise-resistant substrings (k-grams) and selects their hashes as "fingerprint points".

#### Parameters

| Name                     | Type    | Default | Description                                                                                     |
|--------------------------|---------|---------|-------------------------------------------------------------------------------------------------|
| `$string`                | string  | –       | Input text to fingerprint.                                                                      |
| `$guarantee_threshold`   | int     | 8       | Minimum length of substring that must appear in both documents to guarantee a match.            |
| `$noise_threshold`       | int     | 5       | Maximum length of noise (insertions/deletions) tolerated within a matching substring.           |

#### Return Value

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `int[]`    | Array of hash values representing the fingerprint points.                                       |

#### Inner Mechanisms

1. **Parameter Adjustment**: Ensures `$noise_threshold` ≤ `$guarantee_threshold`. Computes derived values:
   - `$window_size = $guarantee_threshold - $noise_threshold + 1`
   - `$kgram_size = $noise_threshold`
2. **Initial Hashing**: Computes rolling hash for the first k-gram using a polynomial rolling hash (base 257, mod 65537).
3. **Window Filling**: Populates a circular buffer (`$window`) with hashes of subsequent k-grams.
4. **Winnowing**: Scans the window for the rightmost minimum hash value, adding it to the result if it is the minimum in its window.
5. **Termination**: Stops when the entire string is processed.

#### Usage Context

- **Plagiarism Detection**: Compare fingerprints of documents to detect partial or near-duplicate content.
- **Content Deduplication**: Identify similar articles, comments, or user-generated content.
- **Efficient Storage**: Store fingerprints instead of full text for similarity queries.

---

### `hmac_md5`

Computes a **Hash-based Message Authentication Code (HMAC)** using MD5, ensuring data integrity and authenticity. Uses the standard HMAC construction with inner/outer padding.

#### Parameters

| Name      | Type   | Default | Description                                                                                     |
|-----------|--------|---------|-------------------------------------------------------------------------------------------------|
| `$string` | string | –       | Data to authenticate.                                                                           |
| `$key`    | string | –       | Secret key. If longer than 64 bytes, it is hashed to 16 bytes using MD5.                        |

#### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | 32-character hexadecimal HMAC-MD5 digest.                                                       |

#### Inner Mechanisms

1. **Key Normalization**: If `$key` > 64 bytes, it is hashed to 16 bytes using MD5.
2. **Padding**: Key is padded to 64 bytes with null bytes.
3. **Inner/Outer Pads**: XORs key with `0x36` (inner) and `0x5C` (outer) to create two 64-byte pads.
4. **HMAC Construction**: `HMAC = MD5(outer_pad || MD5(inner_pad || message))`.

#### Usage Context

- **API Authentication**: Sign requests to ensure they originate from trusted clients.
- **Password Storage**: Derive secure tokens from passwords (not for direct storage).
- **Data Integrity**: Verify that data has not been tampered with in transit.

---

### `simhash`

Generates a **64-bit SimHash** for a string, a locality-sensitive hash used to detect near-duplicate content. SimHash maps similar documents to similar hash values, enabling efficient similarity search.

#### Parameters

| Name      | Type   | Default | Description                                                                                     |
|-----------|--------|---------|-------------------------------------------------------------------------------------------------|
| `$string` | string | –       | Input text to hash.                                                                             |

#### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | 64-character binary string (`0`/`1`) representing the SimHash.                                  |

#### Inner Mechanisms

1. **Histogram Initialization**: Creates a 64-element array initialized to `0`.
2. **Shingle Hashing**: For each 16-byte shingle in the string:
   - Computes MD5 hash, splitting it into four 32-bit integers.
   - For each bit position, increments/decrements the histogram based on the bit value.
3. **Hash Generation**: Converts the histogram into a 64-bit binary string (`1` if positive, `0` otherwise).

#### Usage Context

- **Near-Duplicate Detection**: Compare SimHashes to find similar documents (e.g., news articles, product descriptions).
- **Clustering**: Group similar items without pairwise comparisons.
- **Efficient Storage**: Store SimHashes in databases for fast similarity queries.

---

### `crc32_base62`

Computes a **CRC32 checksum** of a string and encodes it in **Base62**, producing a compact, URL-safe representation.

#### Parameters

| Name      | Type   | Default | Description                                                                                     |
|-----------|--------|---------|-------------------------------------------------------------------------------------------------|
| `$string` | string | –       | Input data to checksum.                                                                         |

#### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | Base62-encoded CRC32 checksum (8 characters or fewer).                                          |

#### Inner Mechanisms

1. **CRC32 Calculation**: Computes the unsigned 32-bit CRC32 checksum of the string.
2. **Base62 Encoding**: Converts the checksum to a Base62 string (digits + uppercase + lowercase letters).

#### Usage Context

- **URL Shortening**: Generate compact identifiers for resources.
- **Data Integrity**: Verify file or message integrity in a URL-friendly format.
- **Caching Keys**: Use as a short, unique key for cache entries.

---

### `hash32`

Generates a **128-bit RIPEMD-128 hash** of a string, providing a compact, cryptographic hash suitable for non-security-critical applications.

#### Parameters

| Name      | Type    | Default | Description                                                                                     |
|-----------|---------|---------|-------------------------------------------------------------------------------------------------|
| `$string` | string  | –       | Input data to hash.                                                                             |
| `$binary` | bool    | `FALSE` | If `TRUE`, returns raw binary output (16 bytes); otherwise, returns 32-character hexadecimal.    |

#### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | RIPEMD-128 hash in hexadecimal (default) or binary format.                                      |

#### Usage Context

- **Non-Cryptographic Hashing**: Unique identifiers, checksums, or cache keys.
- **Data Deduplication**: Detect identical files or records.
- **Bloom Filters**: Use as a hash function for probabilistic data structures.

---

### `hash64`

Generates a **256-bit SHA-256 hash** of a string, providing a cryptographically secure hash for security-sensitive applications.

#### Parameters

| Name      | Type    | Default | Description                                                                                     |
|-----------|---------|---------|-------------------------------------------------------------------------------------------------|
| `$string` | string  | –       | Input data to hash.                                                                             |
| `$binary` | bool    | `FALSE` | If `TRUE`, returns raw binary output (32 bytes); otherwise, returns 64-character hexadecimal.    |

#### Return Value

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| string | SHA-256 hash in hexadecimal (default) or binary format.                                         |

#### Usage Context

- **Password Hashing**: Securely hash passwords (though `password_hash()` is preferred for this purpose).
- **Digital Signatures**: Verify data integrity and authenticity.
- **Blockchain Applications**: Generate hashes for blocks or transactions.


<!-- HASH:743fb866c9194c4c7474293607d085e9 -->
