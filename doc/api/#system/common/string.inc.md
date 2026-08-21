# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/string.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/string.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## String Utilities (`string.inc`)

Core string manipulation and validation utilities for the PWNC Web Platform. This file provides multibyte-safe, locale-aware, and context-sensitive string operations for content processing, user input sanitization, and dynamic text generation.

---

### `replace_placeholder`

Replaces placeholders in a string with dynamic values while preserving optional surrounding brackets.

#### Parameters

| Name          | Value/Default | Description                                                                 |
|---------------|---------------|-----------------------------------------------------------------------------|
| `$string`     | -             | Input string containing placeholders in the format `%key%` or `[...%key%...]` |
| `$replacement`| -             | Single value or associative array of `key => value` pairs for replacement   |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Processed string with placeholders replaced or removed if value is empty   |

#### Inner Mechanisms

- Placeholders are matched using regex with optional surrounding brackets (`[...]`).
- If a replacement value is non-empty (`nstre`), the placeholder is replaced with the escaped (`x()`) value, preserving any surrounding text in brackets.
- If the replacement value is empty, the entire placeholder (including brackets) is removed.
- Keys in `$replacement` are regex-escaped to prevent injection.

#### Usage Context

Used in templating, dynamic content generation, and localized strings where placeholders need runtime substitution.

#### Example

```php
$text = "Hello, [dear %name%]! Your score is %score%.";
$replaced = replace_placeholder($text, [
    "name"  => "Alice",
    "score" => 95
]);
// Output: "Hello, dear Alice! Your score is 95."
```

---

### `strtoalphanum`

Converts a string to alphanumeric characters only, replacing non-alphanumeric characters with a specified separator.

#### Parameters

| Name           | Value/Default | Description                                                                 |
|----------------|---------------|-----------------------------------------------------------------------------|
| `$string`      | -             | Input string to be sanitized                                                |
| `$replacement` | `" "`         | String used to replace non-alphanumeric sequences; empty string removes them |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Cleaned string containing only letters, numbers, and the replacement string |

#### Inner Mechanisms

- Uses `html_entity_decode` to normalize HTML entities.
- Regex `/[^\p{L}\p{N}]+/u` matches any non-letter and non-number Unicode characters.
- Leading/trailing separators and consecutive separators are collapsed.

#### Usage Context

Used for generating URL slugs, filenames, search tokens, or any context requiring alphanumeric-only strings.

#### Example

```php
$slug = strtoalphanum("Café 2024: New Menu!", "-");
// Output: "Café-2024-New-Menu"
```

---

### `strtonum`

Extracts and parses the first numeric value from a string, respecting locale-specific thousand and decimal separators.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | -             | Input string containing numbers |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| float   | Parsed numeric value                                                        |
| bool    | `FALSE` if no valid number is found                                         |

#### Inner Mechanisms

- Uses locale constants `CMS_L_THOUSAND_SEPARATOR` and `CMS_L_DECIMAL_SEPARATOR`.
- Regex extracts the first sequence matching optional sign, digits, and separators.
- Thousand separators are removed; decimal separator is replaced with `.` for `floatval`.

#### Usage Context

Used in form processing, data import, or any scenario where numeric input is embedded in text.

#### Example

```php
setlocale(LC_ALL, 'de_DE');
$value = strtonum("Preis: 1.234,56 €");
// Returns: 1234.56
```

---

### `stripspaces`

Normalizes whitespace in a string, optionally preserving newlines and limiting empty lines.

#### Parameters

| Name                  | Value/Default | Description                                                                 |
|-----------------------|---------------|-----------------------------------------------------------------------------|
| `$string`             | -             | Input string to process                                                     |
| `$preserve_newlines`  | `FALSE`       | If `TRUE`, newlines are preserved; spaces around them are collapsed         |
| `$limit_empty_lines`  | `TRUE`        | If `TRUE` and `$preserve_newlines`, limits consecutive newlines to 2        |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | String with normalized whitespace                                           |

#### Inner Mechanisms

- If `$preserve_newlines` is `FALSE`, all whitespace is collapsed to single spaces.
- If `TRUE`, spaces are collapsed except around newlines; leading/trailing spaces are trimmed.
- Empty lines are limited to 2 if `$limit_empty_lines` is `TRUE`.

#### Usage Context

Used for cleaning user-generated content, code formatting, or preparing text for display.

#### Example

```php
$text = "  Hello    \n\n  World  \n  ";
echo stripspaces($text, TRUE, TRUE);
// Output: "Hello\n\nWorld"
```

---

### `nl2br`

Converts newlines to `<br>` tags, with special handling for multiple consecutive newlines.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | -             | Input string with newlines       |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | String with `\n` replaced by `<br>`; multiple newlines get `class="multiple"` |

#### Inner Mechanisms

- Normalizes `\r\n` to `\n`.
- Uses regex lookbehind to identify consecutive newlines and assigns a special class.

#### Usage Context

Used for rendering plaintext content in HTML while preserving paragraph-like structure.

#### Example

```php
$text = "Line 1\n\nLine 2";
echo nl2br($text);
// Output: "Line 1<br class="multiple"><br>Line 2"
```

---

### `limitstr`

Truncates a string to a specified length, respecting word boundaries defined by a separator.

#### Parameters

| Name        | Value/Default | Description                                                                 |
|-------------|---------------|-----------------------------------------------------------------------------|
| `$string`   | -             | Input string to truncate                                                    |
| `$length`   | `255`         | Maximum length of the output string                                         |
| `$separator`| `","`         | Character used to split the string into words (e.g., comma, space)          |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Truncated string, never exceeding `$length`                                |

#### Inner Mechanisms

- Splits string by `$separator`.
- Reconstructs string word by word until length limit is reached.
- Uses `utf8_substr` for multibyte safety.

#### Usage Context

Used for generating previews, summaries, or metadata where length constraints exist.

#### Example

```php
$tags = "php,web,development,backend,frontend";
echo limitstr($tags, 20);
// Output: "php,web,development"
```

---

### `first_paragraph`

Extracts the first paragraph from a string, defined as the initial sequence of non-vertical whitespace characters.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | -             | Input string with paragraphs    |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | First paragraph; if none found, returns the entire string                   |

#### Inner Mechanisms

- Regex `^\V+` matches one or more non-vertical whitespace characters (anything not `\n`, `\r`, `\f`, `\v`).

#### Usage Context

Used for generating excerpts or previews from long-form content.

#### Example

```php
$content = "Hello world!\n\nThis is a test.";
echo first_paragraph($content);
// Output: "Hello world!"
```

---

### `first_words`

Truncates a string to a specified number of characters, adding an ellipsis if truncated, while avoiding word breaks.

#### Parameters

| Name        | Value/Default | Description                                                                 |
|-------------|---------------|-----------------------------------------------------------------------------|
| `$string`   | -             | Input string to truncate                                                    |
| `$length`   | `250`         | Maximum length (in characters)                                              |
| `$ellipsis` | `" …"`        | String appended if truncation occurs                                        |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Truncated string with ellipsis if needed; full string if under length limit |

#### Inner Mechanisms

- Uses regex to match up to `$length` characters, preferring word boundaries (via `CMS_REGEX_SEPARATOR`).
- Falls back to simple truncation if no boundary is found.

#### Usage Context

Used for generating teaser text, search results, or any UI element with limited space.

#### Example

```php
$text = "The quick brown fox jumps over the lazy dog.";
echo first_words($text, 20);
// Output: "The quick brown fox …"
```

---

### `zerofill`

Pads a string with null bytes (`\0`) to a specified length.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | -             | Input string to pad             |
| `$length` | -             | Desired total length            |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Padded string; original string if already longer than `$length`            |

#### Inner Mechanisms

- Calculates padding length and appends null bytes.

#### Usage Context

Used in binary data handling, fixed-width file formats, or low-level data serialization.

#### Example

```php
$binary = zerofill("ID", 4);
// Output: "ID\0\0" (4 bytes)
```

---

### String Check Functions

#### `stre`, `nstre`, `streq`, `nstreq`, `strieq`

Lightweight string comparison utilities.

| Function  | Purpose                                                                 | Return Type | Example |
|-----------|-------------------------------------------------------------------------|-------------|---------|
| `stre`    | Checks if string is empty (`""`)                                        | bool        | `stre("")` → `TRUE` |
| `nstre`   | Checks if string is not empty                                           | bool        | `nstre("a")` → `TRUE` |
| `streq`   | Checks if two strings are equal (case-sensitive)                        | bool        | `streq("a", "A")` → `FALSE` |
| `nstreq`  | Checks if two strings are not equal (case-sensitive)                    | bool        | `nstreq("a", "b")` → `TRUE` |
| `strieq`  | Checks if two strings are equal (case-insensitive) using regex          | bool        | `strieq("a", "A")` → `TRUE` |

#### Usage Context

Used throughout the codebase for input validation, conditional logic, and data filtering.

#### Example

```php
if (nstre($username) && streq($username, "admin")) {
    // Grant access
}
```

---

### `substri_count`

Counts the number of non-overlapping occurrences of a substring in a string (case-insensitive).

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$source` | -             | String to search in             |
| `$string` | -             | Substring to count              |

#### Return Values

| Type | Description                                                                 |
|------|-----------------------------------------------------------------------------|
| int  | Number of occurrences                                                       |

#### Inner Mechanisms

- Uses `preg_match_all` with case-insensitive regex.

#### Usage Context

Used for text analysis, keyword density checks, or input validation.

#### Example

```php
$count = substri_count("Hello hello HELLO", "hello");
// Returns: 3
```

---

### `verify_email`

Validates an email address according to RFC standards, including local part, domain, and IP address support.

#### Parameters

| Name    | Value/Default | Description                     |
|---------|---------------|---------------------------------|
| `$email`| -             | Email address to validate       |

#### Return Values

| Type | Description                                                                 |
|------|-----------------------------------------------------------------------------|
| bool | `TRUE` if valid; `FALSE` otherwise                                         |

#### Inner Mechanisms

- Uses a comprehensive regex covering quoted local parts, IP literals (IPv4/IPv6), and domain names.
- Validates length constraints (local ≤ 64, total ≤ 254).
- Supports Punycode for internationalized domain names.
- Uses `verify_ip` for IP validation.

#### Usage Context

Used in user registration, contact forms, and any input requiring email validation.

#### Example

```php
if (verify_email("user@example.com")) {
    // Proceed with registration
}
```

---

### `verify_ip`

Validates an IPv4 or IPv6 address according to RFC 3986.

#### Parameters

| Name  | Value/Default | Description                     |
|-------|---------------|---------------------------------|
| `$ip` | -             | IP address to validate          |

#### Return Values

| Type | Description                                                                 |
|------|-----------------------------------------------------------------------------|
| bool | `TRUE` if valid; `FALSE` otherwise                                         |

#### Inner Mechanisms

- Uses regex patterns for both IPv4 and IPv6, including compressed IPv6 notation.
- Supports mixed IPv4-mapped IPv6 addresses.

#### Usage Context

Used in access control, logging, and network-related features.

#### Example

```php
if (verify_ip("2001:0db8::1")) {
    // Allow IPv6 access
}
```

---

### `unique_id`

Generates a random alphanumeric string of specified length.

#### Parameters

| Name    | Value/Default | Description                     |
|---------|---------------|---------------------------------|
| `$count`| `8`           | Length of the generated ID      |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Random string from `[0-9a-zA-Z]`                                           |

#### Inner Mechanisms

- Uses `mt_rand` for randomness.
- Static character set for consistency.

#### Usage Context

Used for generating session tokens, temporary filenames, or unique identifiers.

#### Example

```php
$token = unique_id(16);
// Example: "aB3x9KpL2qR7vYz8"
```

---

### `reference_code`

Generates a human-readable reference code with optional chunking.

#### Parameters

| Name            | Value/Default | Description                     |
|-----------------|---------------|---------------------------------|
| `$count`        | `6`           | Total number of characters      |
| `$chunk_length` | `3`           | Length of each chunk; `-1` disables chunking |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Reference code with optional hyphens (e.g., `ABC-123`)                     |

#### Inner Mechanisms

- Uses a restricted character set (`346789ABCDEFGHJKLMNPQRTUVWXY`) to avoid ambiguity.
- Inserts hyphens every `$chunk_length` characters.

#### Usage Context

Used for order numbers, voucher codes, or any user-facing identifier.

#### Example

```php
$code = reference_code(8, 4);
// Example: "ABCD-1234"
```

---

### `strabridge`

Truncates a string to a specified length, adding an ellipsis in the middle if truncated.

#### Parameters

| Name        | Value/Default | Description                                                                 |
|-------------|---------------|-----------------------------------------------------------------------------|
| `$string`   | -             | Input string to truncate                                                    |
| `$length`   | `50`          | Maximum length of the output string                                         |
| `$cut_end`  | `FALSE`       | If `TRUE`, truncates from the end; if `FALSE`, uses middle ellipsis         |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Truncated string with ellipsis; original string if under length limit      |

#### Inner Mechanisms

- If `$cut_end`, truncates from the end.
- Otherwise, splits the string 65/35 and inserts `" … "` in the middle.

#### Usage Context

Used for displaying long filenames, URLs, or paths in UI elements with limited space.

#### Example

```php
$path = "/var/www/html/project/assets/images/logo.png";
echo strabridge($path, 30);
// Output: "/var/www/html/ … /logo.png"
```

---

### `generate_pseudonym`

Generates a Japanese-style pseudonym from a seed, with a specified number of syllables.

#### Parameters

| Name        | Value/Default | Description                                                                 |
|-------------|---------------|-----------------------------------------------------------------------------|
| `$seed`     | `NULL`        | Optional seed for reproducible output                                       |
| `$syllables`| `3`           | Number of syllables in the generated name                                   |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Generated pseudonym (e.g., `Kirino`, `Sasuke`)                             |

#### Inner Mechanisms

- Uses a predefined array of Japanese syllables.
- If `$seed` is provided, seeds the random generator for reproducibility.
- Always ends with a syllable from an extended set including `n`.

#### Usage Context

Used for generating usernames, display names, or placeholder content.

#### Example

```php
$name = generate_pseudonym("user123", 4);
// Example: "Yamamoto"
```

---

### `strtocolor`

Converts a string into a deterministic HSL color, with optional minimum hue difference from previous calls.

#### Parameters

| Name         | Value/Default | Description                                                                 |
|--------------|---------------|-----------------------------------------------------------------------------|
| `$string`    | -             | Input string used to generate color                                         |
| `$lightness` | `75`          | Lightness value (0–100) for the HSL color                                   |
| `$span`      | `TRUE`        | If `TRUE`, wraps the string in a `<span>` with the generated color          |
| `$diff_min`  | `0`           | Minimum hue difference (in degrees) from the previous color                 |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | HSL color string (e.g., `hsl(120,75%,75%)`) or `<span>` with inline style   |

#### Inner Mechanisms

- Uses `djb2` hash function to generate a hue value from the string.
- Adjusts hue to maintain minimum difference from the previous call.
- Static variable `$hue_prev` tracks the last used hue.

#### Usage Context

Used for visual differentiation in lists, tags, or user avatars.

#### Example

```php
echo strtocolor("admin");
// Output: <span style="COLOR:hsl(123,75%,75%)">admin</span>
```

---

### Constants (Implicit)

| Name                     | Description                                                                 |
|--------------------------|-----------------------------------------------------------------------------|
| `CMS_L_THOUSAND_SEPARATOR` | Locale-specific thousand separator (e.g., `,` or `.`)                       |
| `CMS_L_DECIMAL_SEPARATOR`  | Locale-specific decimal separator (e.g., `.` or `,`)                        |
| `CMS_REGEX_SEPARATOR`      | Regex pattern for word separators (used in `first_words`)                   |


<!-- HASH:f36144de3071aae600454c0950842a28 -->
