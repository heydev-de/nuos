# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/codec.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Overview
`codec.inc` provides core cryptographic and encoding utilities for the NUOS platform. It includes functions for symmetric encryption, character encoding/decoding, SQL escaping, and context-aware string escaping for JavaScript, URLs, and XML. These utilities ensure secure data handling, safe string representation, and interoperability across different output contexts (e.g., database queries, JSON, HTML, URLs).

---

## Functions

### `encrypt`
Encrypts a string using authenticated encryption (AEAD) via libsodium.

#### Parameters
| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$string`  | `string` | Plaintext to encrypt.                                                       |
| `$password`| `string` | Secret key/password used for encryption.                                    |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Base64-encoded ciphertext containing salt, nonce, HMAC, and encrypted data. |
| `NULL`   | On failure (e.g., invalid password or corrupted ciphertext).                |

#### Inner Mechanisms
1. **Salt Generation**: A cryptographically secure random salt is generated for key derivation.
2. **Key Derivation**: Uses `sodium_crypto_pwhash` with interactive limits (moderate CPU/memory usage).
3. **Nonce Generation**: A unique nonce is generated for each encryption.
4. **Encryption**: Uses `sodium_crypto_secretbox` (XSalsa20-Poly1305).
5. **Integrity Protection**: HMAC is generated over the nonce + ciphertext using `sodium_crypto_auth`.
6. **Concatenation**: Salt, nonce, HMAC, and ciphertext are concatenated and base64-encoded.

#### Usage
- Secure storage of sensitive data (e.g., user tokens, API keys).
- Encrypting data before database storage or transmission.
- **Note**: The same password must be used for decryption.

---

### `decrypt`
Decrypts a string encrypted with `encrypt`.

#### Parameters
| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$string`  | `string` | Base64-encoded ciphertext (from `encrypt`).                                 |
| `$password`| `string` | Secret key/password used for decryption.                                    |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Decrypted plaintext.                                                        |
| `NULL`   | On failure (e.g., invalid password, corrupted ciphertext, or tampered data).|

#### Inner Mechanisms
1. **Base64 Decoding**: Decodes the input string.
2. **Part Extraction**: Splits the decoded string into salt, nonce, HMAC, and ciphertext.
3. **Key Derivation**: Recreates the encryption key using the salt and password.
4. **Integrity Check**: Verifies HMAC using `sodium_crypto_auth_verify`.
5. **Decryption**: Uses `sodium_crypto_secretbox_open` to decrypt the ciphertext.

#### Usage
- Decrypting data retrieved from storage or transmission.
- **Note**: Returns `NULL` if integrity checks fail (tampering or incorrect password).

---

### `rc4encrypt`
Encrypts/decrypts a string using the RC4 stream cipher (symmetric).

#### Parameters
| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$string`  | `string` | Plaintext (encryption) or ciphertext (decryption).                          |
| `$password`| `string` | Secret key/password.                                                        |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Encrypted/decrypted string.                                                 |
| `NULL`   | If `$password` is empty.                                                    |

#### Inner Mechanisms
1. **Key Scheduling**: Initializes the RC4 state array using the password.
2. **Pseudo-Random Generation**: Generates a keystream by permuting the state array.
3. **XOR Operation**: Encrypts/decrypts the input string by XORing with the keystream.

#### Usage
- Legacy systems requiring RC4 compatibility.
- **Note**: RC4 is cryptographically broken and should be avoided for new applications. Use `encrypt`/`decrypt` instead.

---

### `rc4decrypt`
Alias for `rc4encrypt` (RC4 is symmetric).

#### Parameters
| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$string`  | `string` | Ciphertext to decrypt.                                                      |
| `$password`| `string` | Secret key/password.                                                        |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Decrypted plaintext.                                                        |

#### Usage
- Decrypting data encrypted with `rc4encrypt`.

---

### `encchr`
Escapes special characters in strings for safe storage or transmission.

#### Parameters
| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Input string to escape.                                                     |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | String with special characters replaced by `[chrXX]` placeholders.          |

#### Mappings
| Original | Escaped   |
|----------|-----------|
| `\0`     | `[chr0]`  |
| `\n`     | `[chr10]` |
| `\r`     | `[chr13]` |
| `;`      | `[chr59]` |
| `=`      | `[chr61]` |
| `[`      | `[chr91]` |
| `\`      | `[chr92]` |

#### Usage
- Serializing strings containing control characters or delimiters.
- Storing configuration data or user input where raw special characters may cause parsing issues.

---

### `decchr`
Reverses `encchr` by converting `[chrXX]` placeholders back to original characters.

#### Parameters
| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Escaped string (from `encchr`).                                             |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | String with placeholders replaced by original characters.                   |

#### Usage
- Deserializing strings processed by `encchr`.

---

### `sqlesc`
Recursively escapes values for safe SQL query interpolation.

#### Parameters
| Name                | Type      | Description                                                                 |
|---------------------|-----------|-----------------------------------------------------------------------------|
| `$value`            | `mixed`   | Value to escape (string, number, boolean, or array).                        |
| `$escape_backticks` | `boolean` | If `TRUE`, escapes backticks (`` ` ``) for MySQL identifiers. Default: `FALSE`. |

#### Return Values
| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `string`  | Escaped string (or original value for numbers/booleans).                   |
| `array`   | Recursively escaped array.                                                  |
| `string`  | Empty string for unsupported types (objects, resources).                   |

#### Escaping Rules
| Type      | Transformation                                                                 |
|-----------|-------------------------------------------------------------------------------|
| `boolean` | `TRUE` → `"1"`, `FALSE` → `""`.                                               |
| `integer` | Unchanged.                                                                    |
| `double`  | Unchanged.                                                                    |
| `string`  | `\0` → `\\0`, `\n` → `\\n`, `\r` → `\\r`, `\` → `\\`, `'` → `\'`, `"` → `\"`. |
| `array`   | Recursively escaped.                                                          |

#### Usage
- Preparing user input for SQL queries to prevent injection.
- **Note**: Always use this function for dynamic query values (even for numeric inputs).

---

### `q`
Encodes strings for JavaScript/JSON output with UTF-16 or binary-safe escaping.

#### Parameters
| Name                   | Type      | Description                                                                 |
|------------------------|-----------|-----------------------------------------------------------------------------|
| `$string`              | `string`  | Input string to encode.                                                     |
| `$escape_closing_tag`  | `boolean` | If `TRUE`, escapes `</` as `<\/` (for inline scripts). Default: `TRUE`.     |
| `$binary`              | `boolean` | If `TRUE`, uses binary-safe escaping (no UTF-16 conversion). Default: `FALSE`. |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Encoded string safe for JavaScript/JSON.                                    |

#### Encoding Rules
- **Binary Mode**: Escapes control characters (0x00–0x1F) and quotes.
- **UTF-16 Mode**: Converts UTF-8 characters to `\uXXXX` or surrogate pairs (`\uXXXX\uXXXX`).
- **Closing Tag**: Escapes `</` to `<\/` to prevent premature script termination.

#### Usage
- Generating JSON responses.
- Embedding strings in JavaScript code.
- **Note**: Use `qb` for binary data (e.g., raw bytes).

---

### `qb`
Alias for `q($string, $escape_closing_tag, TRUE)` (binary mode).

#### Parameters
| Name                   | Type      | Description                                                                 |
|------------------------|-----------|-----------------------------------------------------------------------------|
| `$string`              | `string`  | Input string to encode.                                                     |
| `$escape_closing_tag`  | `boolean` | If `TRUE`, escapes `</` as `<\/`. Default: `TRUE`.                          |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Binary-safe encoded string.                                                 |

#### Usage
- Encoding binary data for JavaScript/JSON.

---

### `r`
Alias for `rawurlencode`.

#### Parameters
| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Input string to URL-encode.                                                 |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | URL-encoded string (RFC 3986).                                              |

#### Usage
- Encoding URL components (e.g., query parameters, path segments).

---

### `x`
Escapes strings for XML/HTML output.

#### Parameters
| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Input string to escape.                                                     |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | String with XML/HTML special characters escaped.                            |

#### Escaping Rules
| Original | Escaped  |
|----------|----------|
| `"`      | `&quot;` |
| `'`      | `&apos;` |
| `&`      | `&amp;`  |
| `<`      | `&lt;`   |
| `>`      | `&gt;`   |

#### Usage
- Generating XML/HTML content dynamically.

---

### Combined Escapers
Combine `q`, `r`, and `x` for nested contexts:

| Function | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `rq`     | URL-encodes then JavaScript-escapes (`q(r($string), FALSE)`).               |
| `qr`     | JavaScript-escapes then URL-encodes (`r(q($string, FALSE))`).               |
| `qx`     | JavaScript-escapes then XML-escapes (`x(q($string, FALSE))`).               |
| `rx`     | URL-encodes then XML-escapes (`x(r($string))`).                             |
| `qrx`    | JavaScript-escapes, URL-encodes, then XML-escapes (`x(r(q($string, FALSE)))`). |

#### Usage
- Generating URLs with JSON data (`rq`).
- Embedding JSON in XML (`qx`).
- Complex nested contexts (e.g., URLs in JSON in XML).

---

### `xmlspecialchars`
Escapes or unescapes XML/HTML special characters.

#### Parameters
| Name      | Type      | Description                                                                 |
|-----------|-----------|-----------------------------------------------------------------------------|
| `$string` | `string`  | Input string to process.                                                    |
| `$decode` | `boolean` | If `TRUE`, decodes entities. Default: `FALSE` (encodes).                    |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Encoded or decoded string.                                                  |

#### Usage
- Encoding/decoding XML/HTML content.

---

### `htmlentities_decode`
Decodes HTML entities to UTF-8 characters.

#### Parameters
| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Input string containing HTML entities.                                      |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | String with entities replaced by UTF-8 characters.                          |

#### Inner Mechanisms
1. **Numeric Entities**: Converts `&#xHHHH;` and `&#DDDD;` to UTF-8 using `utf8_chr`.
2. **Literal Entities**: Uses `get_html_translation_table` to map entities to UTF-8.

#### Usage
- Processing user input containing HTML entities.
- Converting HTML content to plain text.

---

### `escape`
URL-encodes every byte in a string (percent-encoding).

#### Parameters
| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | Input string to encode.                                                     |

#### Return Values
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | String with every byte encoded as `%XX`.                                    |

#### Usage
- Low-level URL encoding (e.g., for binary data in URLs).
- **Note**: Prefer `r` for standard URL encoding.


<!-- HASH:cd5173d32f23e61d8aa0e8567aa3c1d3 -->
