# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/codec.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/codec.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## codec.inc

The `codec.inc` file provides a comprehensive set of encoding, encryption, and string manipulation utilities for the PWNC Web Platform. It includes functions for secure data encryption using libsodium (with RC4 fallback), SQL escaping, JavaScript/JSON string encoding, URL encoding, XML escaping, HTTP parameter quoting, and Punycode conversion for internationalized domain names.

### Encryption Functions

#### secretbox_key

Generates or retrieves a cryptographic key derived from a password using libsodium's password hashing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$password` | string | The password to derive the key from |
| `$salt` | string\|NULL | Optional salt; if provided, returns only the key |

**Returns:** string (key) when `$salt` is provided, or array containing salt and key when `$salt` is NULL.

**Inner Mechanisms:** Uses static arrays to cache salts and keys for performance. When no salt is provided, generates a random salt and derives a key using `sodium_crypto_pwhash`.

**Usage Example:**
```php
// Generate new salt and key
list($salt, $key) = secretbox_key("my_password");
// Retrieve key with known salt
$key = secretbox_key("my_password", $salt);
```

#### encrypt

Encrypts a string using libsodium's secret box encryption.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Data to encrypt |
| `$password` | string | Password for encryption |

**Returns:** Base64-encoded string containing salt, nonce, and encrypted data.

**Inner Mechanisms:** Generates a random nonce, encrypts data with `sodium_crypto_secretbox`, and concatenates salt, nonce, and ciphertext before base64 encoding.

**Usage Example:**
```php
$encrypted = encrypt("Sensitive data", "password123");
// Store $encrypted in database
```

#### decrypt

Decrypts data previously encrypted with `encrypt`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Base64-encoded encrypted data |
| `$password` | string | Password for decryption |

**Returns:** Decrypted string or FALSE on failure.

**Inner Mechanisms:** Extracts salt, nonce, and ciphertext from the base64-decoded input, then uses `sodium_crypto_secretbox_open` to decrypt.

**Usage Example:**
```php
$data = decrypt($encrypted, "password123");
echo $data; // Outputs original "Sensitive data"
```

### Fallback Encryption (No Sodium)

When libsodium is not available, `encrypt` and `decrypt` simply return the input unchanged.

### RC4 Encryption

#### rc4encrypt

Encrypts/decrypts data using the RC4 stream cipher.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Data to process |
| `$password` | string | Key for encryption |

**Returns:** Encrypted/decrypted string, or NULL if password is empty.

**Inner Mechanisms:** Implements the full RC4 algorithm including key scheduling and pseudo-random generation.

**Usage Example:**
```php
$encrypted = rc4encrypt("Hello World", "secret");
$decrypted = rc4decrypt($encrypted, "secret");
```

#### rc4decrypt

Decrypts data encrypted with `rc4encrypt` (identical operation due to RC4's symmetric nature).

### Character Encoding Functions

#### encchr

Encodes special characters in a string to readable placeholders.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** String with special characters replaced by `[chrN]` placeholders.

**Usage Example:**
```php
$encoded = encchr("Hello\nWorld"); // "Hello[chr10]World"
```

#### decchr

Decodes placeholders created by `encchr` back to original characters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Encoded string |

**Returns:** Original string with placeholders converted back.

**Usage Example:**
```php
$decoded = decchr("Hello[chr10]World"); // "Hello\nWorld"
```

### SQL Escaping

#### sqlesc

Escapes values for safe inclusion in SQL queries.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | mixed | Value to escape (string, int, bool, array) |
| `$escape_backticks` | boolean | Whether to escape backticks |

**Returns:** Escaped value (string for scalars, array for arrays).

**Inner Mechanisms:** Handles different data types appropriately - booleans become "1" or "", numbers pass through, strings get escaped, arrays are processed recursively.

**Usage Example:**
```php
$safe_value = sqlesc("O'Reilly");
$query = "SELECT * FROM users WHERE name = '" . $safe_value . "'";
```

### JavaScript/JSON Encoding

#### q

Encodes a string for safe inclusion in JavaScript/JSON contexts.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |
| `$escape_closing_tag` | boolean | Whether to escape `</` sequences |
| `$binary` | boolean | Use binary encoding instead of UTF-16 |

**Returns:** Encoded string safe for JS/JSON contexts.

**Inner Mechanisms:** Converts characters to escape sequences - control characters to `\xNN`, quotes to `\"` or `\'`, and UTF-8 characters to `\uNNNN` sequences.

**Usage Example:**
```php
$js_string = q("Hello \"World\"");
echo "<script>var msg = '$js_string';</script>";
```

#### qb

Alias for `q()` with binary mode enabled.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |
| `$escape_closing_tag` | boolean | Whether to escape `</` sequences |

**Returns:** Binary-encoded string.

### URL Encoding

#### r

URL-encodes a string using raw encoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** Raw URL-encoded string.

**Usage Example:**
```php
$url = "search.php?q=" . r("hello world");
```

### XML Escaping

#### x

Escapes special XML characters in a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** XML-escaped string.

**Usage Example:**
```php
$xml = "<name>" . x("<script>alert('xss')</script>") . "</name>";
```

#### xmlspecialchars

Core XML escaping function with encode/decode capability.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |
| `$decode` | boolean\|NULL | If true, decodes entities; if NULL, encodes |

**Returns:** Escaped or decoded string.

### Combined Encoding Functions

#### rq

URL-encodes then JS-encodes a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** Double-encoded string.

#### qr

JS-encodes then URL-encodes a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** Double-encoded string.

#### qx

JS-encodes then XML-escapes a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** Double-encoded string.

#### rx

URL-encodes then XML-escapes a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** Double-encoded string.

#### qrx

JS-encodes, URL-encodes, then XML-escapes a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string |

**Returns:** Triple-encoded string.

### HTML Entity Handling

#### htmlentities_decode

Decodes HTML entities in a string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Input string with entities |

**Returns:** Decoded string.

**Inner Mechanisms:** Handles numeric entities first, then standard HTML entities, replacing invalid UTF-8 sequences with spaces.

**Usage Example:**
```php
$text = htmlentities_decode("&lt;p&gt;Hello&lt;/p&gt;");
// Returns "<p>Hello</p>"
```

### HTTP Parameter Handling

#### quote_http_param

Quotes an HTTP parameter value according to RFC 7230.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | Parameter value |

**Returns:** Properly quoted parameter value.

**Inner Mechanisms:** Removes control characters, then either returns the value unquoted (if it contains only valid token characters) or wraps it in quotes with escaped special characters.

**Usage Example:**
```php
$header = "Content-Disposition: form-data; name=\"file\"; filename=" . quote_http_param("test\"file.txt");
```

### Punycode Conversion

#### punycode

Converts UTF-8 strings to Punycode representation for IDN support.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$string` | string | UTF-8 string to convert |

**Returns:** Punycode-encoded string prefixed with "xn--", or original string if ASCII-only.

**Inner Mechanisms:** Implements the IDNA Punycode algorithm including bias adaptation and variable-length integer encoding.

**Usage Example:**
```php
$domain = punycode("münchen.de");
// Returns "xn--mnchen-3ya.de"
```


<!-- HASH:b9dd27af52dd3a2006d3fd1306a04fc3 -->
