# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/codec.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/codec.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Codec Utilities (`codec.inc`)

This file provides cryptographic, encoding, and escaping utilities for the PWNC Web Platform. It includes functions for encryption/decryption (via `libsodium` or RC4 fallback), character encoding/decoding, SQL escaping, JavaScript/JSON string encoding, URL encoding, XML escaping, and Punycode conversion.

---

## **Sodium-Based Encryption (Requires `ext-sodium`)**

### **`secretbox_key`**
Generates a cryptographic key from a password using `libsodium`'s `crypto_pwhash`.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$password`| `string` | Password used to derive the key.                                           |
| `$salt`    | `string` | Optional salt. If omitted, a random salt is generated and stored statically.|

**Return Value:**
- If `$salt` is provided: `string` (32-byte key).
- If `$salt` is omitted: `array` (`[salt, key]`).

**Inner Mechanisms:**
- Uses `sodium_crypto_pwhash` with `OPSLIMIT_INTERACTIVE` and `MEMLIMIT_INTERACTIVE` for key derivation.
- Caches salts and keys statically to avoid redundant computations.

**Usage Example:**
```php
[$salt, $key] = secretbox_key("my_password");
$key_with_salt = secretbox_key("my_password", $salt);
```

---

### **`encrypt`**
Encrypts a string using `libsodium`'s `crypto_secretbox`.

| Parameter  | Type     | Description                     |
|------------|----------|---------------------------------|
| `$string`  | `string` | Plaintext to encrypt.           |
| `$password`| `string` | Password for key derivation.    |

**Return Value:**
`string` (Base64-encoded ciphertext: `salt + nonce + encrypted_data`).

**Inner Mechanisms:**
- Generates a random nonce for each encryption.
- Concatenates salt, nonce, and ciphertext before Base64 encoding.

**Usage Example:**
```php
$encrypted = encrypt("secret data", "my_password");
```

---

### **`decrypt`**
Decrypts a string encrypted with `encrypt`.

| Parameter  | Type     | Description                     |
|------------|----------|---------------------------------|
| `$string`  | `string` | Base64-encoded ciphertext.      |
| `$password`| `string` | Password for key derivation.    |

**Return Value:**
`string|false` (Decrypted plaintext or `false` on failure).

**Inner Mechanisms:**
- Splits the Base64-decoded string into salt, nonce, and ciphertext.
- Uses `sodium_crypto_secretbox_open` for decryption.

**Usage Example:**
```php
$decrypted = decrypt($encrypted, "my_password");
```

---

## **RC4 Fallback Encryption (No `ext-sodium`)**

### **`rc4encrypt` / `rc4decrypt`**
RC4 stream cipher implementation (symmetric encryption/decryption).

| Parameter  | Type     | Description                     |
|------------|----------|---------------------------------|
| `$string`  | `string` | Data to encrypt/decrypt.        |
| `$password`| `string` | Password for key scheduling.    |

**Return Value:**
`string|null` (Encrypted/decrypted data or `null` if `$password` is empty).

**Inner Mechanisms:**
- Uses the RC4 key-scheduling algorithm (KSA) and pseudo-random generation algorithm (PRGA).
- Identical logic for encryption/decryption (XOR operation).

**Usage Example:**
```php
$encrypted = rc4encrypt("fallback data", "weak_password");
$decrypted = rc4decrypt($encrypted, "weak_password");
```

---

## **Character Encoding/Decoding**

### **`encchr` / `decchr`**
Escapes/unescapes special characters for safe storage in text formats (e.g., INI files).

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string.                   |

**Return Value:**
`string` (Escaped/unescaped string).

**Mapped Characters:**

| Original | Escaped   |
|----------|-----------|
| `\0`     | `[chr0]`  |
| `\n`     | `[chr10]` |
| `\r`     | `[chr13]` |
| `;`      | `[chr59]` |
| `=`      | `[chr61]` |
| `[`      | `[chr91]` |
| `\`      | `[chr92]` |

**Usage Example:**
```php
$escaped = encchr("key=value;[data]");
$unescaped = decchr($escaped); // "key=value;[data]"
```

---

## **SQL Escaping**

### **`sqlesc`**
Escapes values for SQL queries (recursive for arrays).

| Parameter           | Type      | Description                                                                 |
|---------------------|-----------|-----------------------------------------------------------------------------|
| `$value`            | `mixed`   | Value to escape (string, number, boolean, or array).                        |
| `$escape_backticks` | `bool`    | If `true`, escapes backticks (for MySQL identifiers). Default: `false`.     |

**Return Value:**
`mixed` (Escaped value or array of escaped values).

**Inner Mechanisms:**
- Handles booleans (`true` → `"1"`, `false` → `""`).
- Escapes strings with backslash sequences (e.g., `\n` → `\\n`).
- Recursively processes arrays.

**Usage Example:**
```php
$escaped = sqlesc("O'Reilly", false); // "O\\'Reilly"
$escaped_array = sqlesc(["a", "b'c"], true); // ["a", "b\\'c"]
```

---

## **JavaScript/JSON String Encoding**

### **`q`**
Encodes strings for JavaScript/JSON (UTF-16 or binary-safe).

| Parameter              | Type      | Description                                                                 |
|------------------------|-----------|-----------------------------------------------------------------------------|
| `$string`              | `string`  | Input string.                                                               |
| `$escape_closing_tag`  | `bool`    | If `true`, escapes `</` to `<\/`. Default: `true`.                          |
| `$binary`              | `bool`    | If `true`, uses binary-safe encoding (no UTF-16). Default: `false`.         |

**Return Value:**
`string` (Encoded string).

**Inner Mechanisms:**
- **UTF-16 Mode:** Converts Unicode characters to `\uXXXX` or surrogate pairs.
- **Binary Mode:** Escapes control characters (e.g., `\n` → `\\n`).

**Usage Example:**
```php
$js_string = q("Line 1\nLine 2", true); // "Line 1\\nLine 2"
$binary_string = q("\x00\x01", true, true); // "\\0\\x01"
```

---

### **`qb`**
Alias for `q($string, $escape_closing_tag, true)` (binary mode).

---

## **URL Encoding**

### **`r`**
Alias for `rawurlencode`.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string.                   |

**Return Value:**
`string` (URL-encoded string).

**Usage Example:**
```php
$url_encoded = r("hello world"); // "hello%20world"
```

---

## **XML Escaping**

### **`x`**
Escapes XML special characters.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string.                   |

**Return Value:**
`string` (XML-escaped string).

**Mapped Characters:**

| Original | Escaped   |
|----------|-----------|
| `"`      | `&quot;`  |
| `'`      | `&apos;`  |
| `&`      | `&amp;`   |
| `<`      | `&lt;`    |
| `>`      | `&gt;`    |

**Usage Example:**
```php
$xml_safe = x("<tag attr='value'>"); // "&lt;tag attr=&apos;value&apos;&gt;"
```

---

## **Combined Encoders**

| Function | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `rq`     | `q(rawurlencode($string), false)` (URL-encoded + JS-escaped).               |
| `qr`     | `rawurlencode(q($string, false))` (JS-escaped + URL-encoded).               |
| `qx`     | `xmlspecialchars(q($string, false))` (JS-escaped + XML-escaped).            |
| `rx`     | `xmlspecialchars(rawurlencode($string))` (URL-encoded + XML-escaped).       |
| `qrx`    | `xmlspecialchars(rawurlencode(q($string, false)))` (JS + URL + XML-escaped).|

**Usage Example:**
```php
$safe_for_js_url = rq("hello & world"); // "hello%20%26%20world"
$safe_for_xml_js = qx("<script>alert('xss')</script>"); // "&lt;script&gt;alert(\\'xss\\')&lt;/script&gt;"
```

---

## **XML Utilities**

### **`xmlspecialchars`**
Escapes/unescapes XML special characters.

| Parameter | Type      | Description                     |
|-----------|-----------|---------------------------------|
| `$string` | `string`  | Input string.                   |
| `$decode` | `bool`    | If `true`, decodes instead.     |

**Return Value:**
`string` (Escaped/unescaped string).

**Usage Example:**
```php
$escaped = xmlspecialchars("<tag>"); // "&lt;tag&gt;"
$unescaped = xmlspecialchars($escaped, true); // "<tag>"
```

---

### **`htmlentities_decode`**
Decodes HTML entities (including numeric entities) to UTF-8.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string.                   |

**Return Value:**
`string` (Decoded string).

**Inner Mechanisms:**
- Uses `preg_replace_callback` to handle numeric entities (`&#xHH;` and `&#DD;`).
- Falls back to `html_entity_decode` for named entities.

**Usage Example:**
```php
$decoded = htmlentities_decode("&lt;tag&#x20;attr=&quot;value&quot;&gt;"); // "<tag attr="value">"
```

---

## **Punycode Conversion**

### **`punycode`**
Converts Unicode domain names to Punycode (RFC 3492).

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string (domain name).     |

**Return Value:**
`string` (Punycode-encoded string prefixed with `xn--` or original string if ASCII-only).

**Inner Mechanisms:**
- Uses the Bootstring algorithm (adaptive bias calculation).
- Skips ASCII characters.

**Usage Example:**
```php
$punycode = punycode("münchen.de"); // "xn--mnchen-3ya.de"
```


<!-- HASH:795c28c023f403e15020311400264b5c -->
