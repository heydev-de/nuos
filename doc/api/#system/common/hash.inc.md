# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/hash.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/hash.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Hash Utilities

This file provides a collection of hashing and fingerprinting functions used throughout the PWNC Web Platform. These utilities support content deduplication, integrity verification, similarity detection, and compact identifier generation.

### Constants

| Name | Value | Description |
| --- | --- | --- |
| `$table` | `65537` | Prime modulus used in rolling hash computation |
| `$base` | `257` | Base multiplier for polynomial rolling hash |

---

### fingerprint

Computes a set of hash values representing the input string using a winnowing algorithm. This technique is commonly used for near-duplicate detection in large text corpora.

#### Parameters

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| `$string` | `string` | — | Input string to fingerprint |
| `$guarantee_threshold` | `int` | `8` | Maximum allowed gap between selected hashes |
| `$noise_threshold` | `int` | `5` | Minimum window size for selecting representative hashes |

#### Return Values

- **Type:** `array<int, int>`
- **Description:** Array of integer hash values representing key positions in the string

#### Inner Mechanisms

1. Adjusts thresholds if noise exceeds guarantee
2. Computes k-gram size and window size based on thresholds
3. Uses a polynomial rolling hash with modular arithmetic
4. Applies winnowing to select minimum hashes from sliding windows
5. Handles edge cases for very short strings

#### Usage Example

```php
// Detect similar documents
$doc1 = "The quick brown fox jumps over the lazy dog";
$doc2 = "The quick brown fox leaps over the sleepy dog";

$fingerprint1 = fingerprint($doc1);
$fingerprint2 = fingerprint($doc2);

// Compare fingerprints to detect similarity
$similarity = count(array_intersect($fingerprint1, $fingerprint2)) / 
              max(count($fingerprint1), count($fingerprint2));
```

---

### hmac_md5

Computes an HMAC (Hash-based Message Authentication Code) using MD5 as the underlying hash function.

#### Parameters

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| `$string` | `string` | — | Message to authenticate |
| `$key` | `string` | — | Secret key for HMAC computation |

#### Return Values

- **Type:** `string`
- **Description:** 32-character hexadecimal HMAC-MD5 digest

#### Inner Mechanisms

1. Pads or truncates key to 64 bytes
2. Creates inner and outer padding by XORing key with specific byte patterns
3. Applies MD5 twice: once with inner pad, then with outer pad
4. Returns final hexadecimal digest

#### Usage Example

```php
// Generate secure token for form validation
$message = "user_id=123&action=update";
$secret_key = "my_secret_key_123";

$token = hmac_md5($message, $secret_key);
// Store $token with form and verify on submission
```

---

### simhash

Computes a 64-bit SimHash value for the input string. SimHash is a technique for efficiently estimating the similarity between documents.

#### Parameters

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| `$string` | `string` | — | Input string to hash |

#### Return Values

- **Type:** `string`
- **Description:** 64-character binary string representing the SimHash

#### Inner Mechanisms

1. Initializes a 64-element histogram
2. For each position in the string, generates a 128-bit MD5 hash of a 16-character shingle
3. Splits hash into four 32-bit segments
4. Updates histogram based on bit values in each segment
5. Converts histogram to final 64-bit binary string

#### Usage Example

```php
// Find near-duplicate content
$content1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
$content2 = "Lorem ipsum dolor sit amet, sed do eiusmod tempor incididunt.";

$hash1 = simhash($content1);
$hash2 = simhash($content2);

// Calculate Hamming distance
$distance = substr_count(decbin(bindec($hash1) ^ bindec($hash2)), '1');
$isSimilar = $distance < 10; // Threshold for similarity
```

---

### crc32_base62

Computes a CRC32 hash of the input string and encodes it in Base62 format.

#### Parameters

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| `$string` | `string` | — | Input string to hash |

#### Return Values

- **Type:** `string`
- **Description:** Base62-encoded CRC32 hash

#### Inner Mechanisms

1. Computes CRC32 hash of input string
2. Formats as unsigned integer
3. Encodes result using Base62 encoding

#### Usage Example

```php
// Generate short unique identifiers
$id = crc32_base62("user@example.com");
echo $id; // e.g., "3y4sK9"
```

---

### hash32

Computes a 128-bit (16-byte) RIPEMD-128 hash of the input string.

#### Parameters

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| `$string` | `string` | — | Input string to hash |
| `$binary` | `bool` | `FALSE` | Whether to return raw binary output |

#### Return Values

- **Type:** `string`
- **Description:** Hexadecimal (32 chars) or binary (16 bytes) hash

#### Inner Mechanisms

Wraps PHP's `hash()` function with RIPEMD-128 algorithm.

#### Usage Example

```php
// Generate compact hash for cache keys
$data = "some_cacheable_content";
$key = hash32($data); // 32-character hex string
```

---

### hash64

Computes a 256-bit (32-byte) SHA-256 hash of the input string.

#### Parameters

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| `$string` | `string` | — | Input string to hash |
| `$binary` | `bool` | `FALSE` | Whether to return raw binary output |

#### Return Values

- **Type:** `string`
- **Description:** Hexadecimal (64 chars) or binary (32 bytes) hash

#### Inner Mechanisms

Wraps PHP's `hash()` function with SHA-256 algorithm.

#### Usage Example

```php
// Secure password hashing (though password_hash() is preferred)
$password = "user_password";
$hash = hash64($password);
```

---

### djb2

Computes a DJB2 hash of the input string. This is a simple but effective string hashing algorithm.

#### Parameters

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| `$string` | `string` | — | Input string to hash |

#### Return Values

- **Type:** `int`
- **Description:** 32-bit unsigned integer hash value

#### Inner Mechanisms

1. Initializes hash to 5381
2. For each character, applies formula: `hash = ((hash << 5) + hash) + ord(char)`
3. Masks result to 32 bits to prevent overflow

#### Usage Example

```php
// Simple hash table implementation
$key = "example_key";
$hash_value = djb2($key);
$bucket = $hash_value % $table_size; // Distribute across hash table
```


<!-- HASH:2f799262d08593ed5d5de73d5fac8660 -->
