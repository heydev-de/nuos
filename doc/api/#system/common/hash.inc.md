# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/hash.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/hash.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Hash Utilities (`hash.inc`)

Core hashing utilities for the PWNC Web Platform. Provides specialized algorithms for fingerprinting, similarity hashing, checksums, and cryptographic hashing. Designed for high performance, minimal dependencies, and multibyte safety.

---

### `fingerprint`

Generates a **winnowing fingerprint** for content similarity detection. Uses a rolling hash to extract characteristic k-grams from the input string, producing a compact array of hash values that can be compared for similarity.

#### Parameters

| Name                  | Value/Default | Description                                                                                     |
|-----------------------|---------------|-------------------------------------------------------------------------------------------------|
| `$string`             | (string)      | Input string to fingerprint.                                                                    |
| `$guarantee_threshold`| 8             | Minimum number of characters that must match to guarantee a fingerprint collision.              |
| `$noise_threshold`    | 5             | Maximum number of characters that can differ without affecting the fingerprint (noise tolerance).|

#### Return Values

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `int[]`    | Array of hash values representing the fingerprint. Empty if input is too short.                 |

#### Inner Mechanisms

1. **K-gram Extraction**: Sliding window of size `$noise_threshold` over the input string.
2. **Rolling Hash**: Polynomial rolling hash (Rabin-Karp) with modulus `65537` and base `257`.
3. **Winnowing**: Selects the minimum hash in each window of size `$guarantee_threshold - $noise_threshold + 1`.
4. **Collision Guarantee**: Any two strings sharing a substring of length `$guarantee_threshold` will produce at least one matching hash in their fingerprints.

#### Usage Context

- **Plagiarism Detection**: Compare documents for partial matches.
- **Duplicate Content Filtering**: Identify near-duplicate content in CMS modules.
- **Version Control**: Detect incremental changes in large texts.

#### Example

```php
$text = "The quick brown fox jumps over the lazy dog.";
$fp = \cms\fingerprint($text, 10, 6);
print_r($fp);
// Output: Array of hash values representing 6-grams in the text.
```

---

### `hmac_md5`

Computes an **HMAC-MD5** signature for message authentication. Uses the standard HMAC construction with MD5 as the underlying hash function.

#### Parameters

| Name      | Value/Default | Description                                                                                     |
|-----------|---------------|-------------------------------------------------------------------------------------------------|
| `$string` | (string)      | Message to authenticate.                                                                        |
| `$key`    | (string)      | Secret key. If longer than 64 bytes, it is hashed to 16 bytes.                                  |

#### Return Values

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | 32-character hexadecimal HMAC-MD5 signature.                                                    |

#### Inner Mechanisms

1. **Key Normalization**: Keys longer than 64 bytes are hashed to 16 bytes using MD5.
2. **Padding**: Key is padded to 64 bytes with null bytes.
3. **Inner/Outer Pads**: XOR key with `0x36` (inner) and `0x5C` (outer) to create two pads.
4. **HMAC Construction**: `MD5(outer_pad || MD5(inner_pad || message))`.

#### Usage Context

- **API Authentication**: Sign requests to prevent tampering.
- **Session Validation**: Verify integrity of session tokens.
- **Secure Cookies**: Ensure cookies have not been altered.

#### Example

```php
$message = "user=123&action=delete";
$secret = "s3cr3t_k3y";
$signature = \cms\hmac_md5($message, $secret);
echo $signature; // e.g., "a1b2c3d4e5f6..."
```

---

### `simhash`

Generates a **64-bit SimHash** for near-duplicate detection. Converts the input string into a compact binary hash where similar strings produce similar hashes.

#### Parameters

| Name      | Value/Default | Description                                                                                     |
|-----------|---------------|-------------------------------------------------------------------------------------------------|
| `$string` | (string)      | Input string to hash.                                                                           |

#### Return Values

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | 64-character binary string (`0`/`1`) representing the SimHash.                                  |

#### Inner Mechanisms

1. **Shingling**: Sliding window of 16 characters over the input.
2. **MD5 Hashing**: Each shingle is hashed to 128 bits.
3. **Bit Histogram**: 64-bit histogram where each bit is incremented/decremented based on MD5 bits.
4. **Thresholding**: Bits with positive counts become `1`; others become `0`.

#### Usage Context

- **Document Deduplication**: Cluster similar articles or posts.
- **Spam Filtering**: Detect slight variations of spam content.
- **Image/Video Fingerprinting**: Compare perceptual hashes of media.

#### Example

```php
$doc1 = "The cat sat on the mat.";
$doc2 = "The cat sat on a mat.";
$hash1 = \cms\simhash($doc1);
$hash2 = \cms\simhash($doc2);
$distance = substr_count($hash1 ^ $hash2, '1'); // Hamming distance
echo $distance; // e.g., 3 (low distance = high similarity)
```

---

### `crc32_base62`

Computes a **CRC32 checksum** and encodes it in **Base62** for URL-safe compactness.

#### Parameters

| Name      | Value/Default | Description                                                                                     |
|-----------|---------------|-------------------------------------------------------------------------------------------------|
| `$string` | (string)      | Input string to checksum.                                                                       |

#### Return Values

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | Base62-encoded CRC32 checksum (e.g., `"3g7H"`).                                                 |

#### Inner Mechanisms

1. **CRC32**: Computes the 32-bit checksum using PHP’s `crc32()`.
2. **Base62 Encoding**: Converts the unsigned 32-bit integer to a compact alphanumeric string.

#### Usage Context

- **Cache Keys**: Generate short, unique identifiers for cached resources.
- **File Versioning**: Append checksums to filenames for cache busting.
- **Data Integrity**: Verify downloads or API responses.

#### Example

```php
$file = "example.txt";
$checksum = \cms\crc32_base62(file_get_contents($file));
echo $checksum; // e.g., "aBc123"
```

---

### `hash32`

Computes a **128-bit RIPEMD-128** hash. Provides a cryptographic hash with a fixed 32-character hexadecimal output.

#### Parameters

| Name      | Value/Default | Description                                                                                     |
|-----------|---------------|-------------------------------------------------------------------------------------------------|
| `$string` | (string)      | Input string to hash.                                                                           |
| `$binary` | `FALSE`       | If `TRUE`, returns raw binary output (16 bytes).                                                |

#### Return Values

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | 32-character hexadecimal hash (or 16-byte binary if `$binary=TRUE`).                            |

#### Usage Context

- **Password Hashing**: Store hashed passwords (though `password_hash()` is preferred for security).
- **Data Integrity**: Verify files or messages.
- **Unique Identifiers**: Generate UUIDs or nonces.

#### Example

```php
$password = "s3cur3_p@ss";
$hash = \cms\hash32($password);
echo $hash; // e.g., "a1b2c3d4e5f6..."
```

---

### `hash64`

Computes a **256-bit SHA-256** hash. Provides a cryptographic hash with a fixed 64-character hexadecimal output.

#### Parameters

| Name      | Value/Default | Description                                                                                     |
|-----------|---------------|-------------------------------------------------------------------------------------------------|
| `$string` | (string)      | Input string to hash.                                                                           |
| `$binary` | `FALSE`       | If `TRUE`, returns raw binary output (32 bytes).                                                |

#### Return Values

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | 64-character hexadecimal hash (or 32-byte binary if `$binary=TRUE`).                            |

#### Usage Context

- **Secure Hashing**: Verify file integrity or generate secure tokens.
- **Blockchain**: Generate transaction hashes.
- **Digital Signatures**: Pre-hash data before signing.

#### Example

```php
$file = "contract.pdf";
$hash = \cms\hash64(file_get_contents($file));
echo $hash; // e.g., "a1b2c3...64chars"
```

---

### `djb2`

Computes the **djb2** non-cryptographic hash. A fast, simple hash function with good distribution for general-purpose use.

#### Parameters

| Name      | Value/Default | Description                                                                                     |
|-----------|---------------|-------------------------------------------------------------------------------------------------|
| `$string` | (string)      | Input string to hash.                                                                           |

#### Return Values

| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `int`      | 32-bit unsigned integer hash value.                                                             |

#### Inner Mechanisms

1. **Initialization**: Starts with a magic constant `5381`.
2. **Iteration**: For each character, updates the hash as `hash = ((hash << 5) + hash) + char_code`.
3. **Masking**: Ensures the result is a 32-bit unsigned integer.

#### Usage Context

- **Hash Tables**: Key distribution in in-memory caches.
- **Bloom Filters**: Probabilistic data structure membership tests.
- **Load Balancing**: Distribute requests across servers.

#### Example

```php
$key = "user_123";
$hash = \cms\djb2($key) % 10; // Distribute across 10 buckets
echo $hash; // e.g., 7
```


<!-- HASH:2f799262d08593ed5d5de73d5fac8660 -->
