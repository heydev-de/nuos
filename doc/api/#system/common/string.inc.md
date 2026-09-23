# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/string.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/string.inc)

- **Version:** `26.9.11.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# String Utilities

This file provides a comprehensive collection of string manipulation, validation, and generation functions for the PWNC Web Platform. These utilities handle everything from basic string checks and transformations to advanced operations like email/IP validation, placeholder replacement, and color generation from strings.

## Constants

| Name | Default | Description |
|------|---------|-------------|
| `CMS_L_THOUSAND_SEPARATOR` | Locale-dependent | Character used as thousands separator in numeric strings |
| `CMS_L_DECIMAL_SEPARATOR` | Locale-dependent | Character used as decimal separator in numeric strings |
| `CMS_REGEX_SEPARATOR` | Platform-defined | Separator character used in regex patterns for word boundaries |

## Dependencies

| Function | Description |
|----------|-------------|
| `x($s)` | XML escaping function (escapes `"`, `'`, `&`, `<`, `>`) |
| `utf8_trim($s)` | Multibyte-safe string trim |
| `utf8_strlen($s)` | Multibyte-safe string length |
| `utf8_substr($s, $start, $length)` | Multibyte-safe substring |
| `punycode($s)` | Converts Unicode domain segments to Punycode |
| `djb2($s)` | Hash function producing an integer from a string |

---

## replace_placeholder

Replaces `%key%` placeholders in a string with corresponding values from an array or scalar. Supports optional bracket-wrapping syntax `[prefix%key%suffix]` where the prefix and suffix are preserved around the replacement value.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The input string containing placeholders |
| `$replacement` | `array\|string` | Key-value pairs for replacement, or a single value |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The string with all placeholders replaced |

### Inner Mechanisms

1. If `$replacement` is not an array, it is wrapped in a single-element array.
2. For each key-value pair, the key is regex-quoted to prevent injection.
3. If the value is non-empty (`nstre`), a `preg_replace_callback` replaces `%key%` (optionally wrapped in `[...]` brackets) with the XML-escaped value, preserving any bracket content.
4. If the value is empty, the placeholder and its surrounding brackets are removed entirely.

### Usage Example

```php
$result = replace_placeholder(
    "Hello [Mr.]%name%, your order #[order_id] is ready.",
    ["name" => "John", "order_id" => "12345"]
);
// Result: "Hello [Mr.]John, your order #12345 is ready."

$result = replace_placeholder(
    "User: %username%, Email: %email%",
    ["username" => "alice", "email" => ""]
);
// Result: "User: alice, Email: " (email placeholder removed)
```

---

## strtoalphanum

Converts a string to contain only alphanumeric characters (Unicode-aware), replacing all other characters with a specified separator. Consecutive separators are collapsed into one, and leading/trailing separators are trimmed.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | — | The input string to sanitize |
| `$replacement` | `string` | `" "` | Character(s) to replace non-alphanumeric characters with |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The sanitized alphanumeric string |

### Inner Mechanisms

1. Decodes HTML entities in the input string.
2. Replaces all non-letter/non-number Unicode characters with the replacement string.
3. If the replacement is non-empty, trims leading/trailing occurrences and collapses consecutive occurrences into a single instance.

### Usage Example

```php
$result = strtoalphanum("Hello, World! 日本語テスト");
// Result: "Hello World 日本語テスト"

$result = strtoalphanum("foo_bar-baz", "_");
// Result: "foo_bar_baz"
```

---

## strtonum

Extracts the first numeric value from a string, respecting locale-specific thousand and decimal separators. Returns `FALSE` if no number is found.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The input string to search for a number |

### Return Values

| Type | Description |
|------|-------------|
| `float\|false` | The extracted number as a float, or `FALSE` if none found |

### Inner Mechanisms

1. Builds a regex pattern that matches optional negative sign, digits, and locale-specific thousand/decimal separators.
2. Searches the string for the first match.
3. Strips thousand separators and converts the decimal separator to a dot.
4. Returns the value as a float.

### Usage Example

```php
// Assuming CMS_L_THOUSAND_SEPARATOR = "." and CMS_L_DECIMAL_SEPARATOR = ","
$result = strtonum("Price: 1.234,56 EUR");
// Result: 1234.56

$result = strtonum("No numbers here");
// Result: false
```

---

## stripspaces

Normalizes whitespace in a string by trimming leading/trailing whitespace and collapsing internal whitespace. Optionally preserves newlines and limits consecutive empty lines.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | — | The input string to process |
| `$preserve_newlines` | `bool` | `FALSE` | Whether to preserve newline characters |
| `$limit_empty_lines` | `bool` | `TRUE` | Whether to limit consecutive empty lines to two |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The whitespace-normalized string |

### Inner Mechanisms

**Without newline preservation:**
- Trims leading/trailing whitespace.
- Collapses all internal whitespace sequences into a single space.

**With newline preservation:**
- Trims leading/trailing whitespace.
- Collapses non-newline whitespace into single spaces.
- Normalizes spaces around newlines.
- If `$limit_empty_lines` is `TRUE`, collapses 3+ consecutive newlines into 2.

### Usage Example

```php
$result = stripspaces("  Hello    World  ");
// Result: "Hello World"

$result = stripspaces("Line 1\n\n\n\nLine 2", TRUE, TRUE);
// Result: "Line 1\n\nLine 2"
```

---

## nl2br

Converts newline characters in a string to HTML `<br>` tags. Single newlines become `<br>`, while consecutive newlines become `<br class="multiple">` to allow for distinct styling.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The input string containing newlines |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The string with newlines converted to `<br>` tags |

### Inner Mechanisms

1. Normalizes `\r\n` to `\n`.
2. Replaces consecutive newlines (`\n\n` and beyond) with `<br class="multiple">`.
3. Replaces remaining single newlines with `<br>`.

### Usage Example

```php
$result = nl2br("First line\nSecond line\n\nParagraph break");
// Result: "First line<br>Second line<br class=\"multiple\">Paragraph break"
```

---

## limitstr

Truncates a string to a maximum length by splitting on a separator and reassembling parts until the length limit is reached. Uses multibyte-safe operations.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | — | The input string to limit |
| `$length` | `int` | `255` | Maximum length of the result |
| `$separator` | `string` | `","` | Separator used to split the string into parts |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The truncated string, not exceeding `$length` characters |

### Inner Mechanisms

1. Splits the string by the separator.
2. Iterates through parts, trimming each with `utf8_trim`.
3. Accumulates parts (with separator) until adding the next would exceed the length limit.
4. Returns the accumulated string, truncated to exactly `$length` characters using `utf8_substr`.

### Usage Example

```php
$result = limitstr("apple,banana,cherry,date", 15, ",");
// Result: "apple,banana" (12 chars, adding ",cherry" would exceed 15)

$result = limitstr("short", 255);
// Result: "short"
```

---

## first_paragraph

Extracts the first paragraph from a string. A paragraph is defined as the initial sequence of non-newline characters.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The input string |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The first paragraph, or the entire string if no newlines exist |

### Inner Mechanisms

Uses a regex to match the first sequence of non-newline characters (`\V+` in Unicode mode). If a match is found, returns it; otherwise returns the original string.

### Usage Example

```php
$result = first_paragraph("First paragraph.\n\nSecond paragraph.");
// Result: "First paragraph."

$result = first_paragraph("No newlines here");
// Result: "No newlines here"
```

---

## first_words

Truncates a string to a specified character length, appending an ellipsis if truncation occurs. Uses multibyte-safe operations and respects word boundaries when possible.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | — | The input string to truncate |
| `$length` | `int` | `250` | Maximum length before truncation |
| `$ellipsis` | `string` | `" …"` | String appended when truncation occurs |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The truncated string with ellipsis, or the original if within length |

### Inner Mechanisms

1. If the string length is within the limit, returns it unchanged.
2. Reduces the target length by 2 to account for the ellipsis.
3. Uses a regex to match up to `$length` characters, preferring to stop at a word boundary (defined by `CMS_REGEX_SEPARATOR`).
4. Appends the ellipsis to the matched portion.

### Usage Example

```php
$result = first_words("This is a very long string that needs truncation", 20);
// Result: "This is a very long …"

$result = first_words("Short", 250);
// Result: "Short"
```

---

## zerofill

Pads a string with null bytes (`\0`) on the right to reach a specified length.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The input string to pad |
| `$length` | `int` | The target length |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The padded string, or the original if already at or beyond the target length |

### Inner Mechanisms

1. Casts both parameters to their expected types.
2. Calculates how many null bytes are needed: `$length - strlen($string)`.
3. Appends the required number of null bytes if positive; otherwise returns the string unchanged.

### Usage Example

```php
$result = zerofill("test", 8);
// Result: "test\0\0\0\0"

$result = zerofill("already long enough", 5);
// Result: "already long enough"
```

---

## stre

Checks whether a string is empty.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The value to check |

### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the string is empty, `FALSE` otherwise |

### Inner Mechanisms

Casts the input to a string and compares it to an empty string using strict equality (`===`).

### Usage Example

```php
stre("");        // true
stre("hello");   // false
stre(0);         // false (cast to "0")
```

---

## nstre

Checks whether a string is not empty.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The value to check |

### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the string is not empty, `FALSE` otherwise |

### Inner Mechanisms

Casts the input to a string and compares it to an empty string using strict inequality (`!==`).

### Usage Example

```php
nstre("");        // false
nstre("hello");   // true
nstre(0);         // true (cast to "0")
```

---

## streq

Checks whether two strings are strictly equal.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string1` | `string` | First string |
| `$string2` | `string` | Second string |

### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if both strings are identical, `FALSE` otherwise |

### Inner Mechanisms

Casts both inputs to strings and compares using strict equality (`===`).

### Usage Example

```php
streq("hello", "hello");  // true
streq("hello", "world");  // false
streq(123, "123");        // false (different types after cast)
```

---

## nstreq

Checks whether two strings are not equal.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string1` | `string` | First string |
| `$string2` | `string` | Second string |

### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the strings differ, `FALSE` if identical |

### Inner Mechanisms

Casts both inputs to strings and compares using strict inequality (`!==`).

### Usage Example

```php
nstreq("hello", "world");  // true
nstreq("hello", "hello");  // false
```

---

## strieq

Performs a case-insensitive comparison of two strings.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string1` | `string` | First string (used as the pattern) |
| `$string2` | `string` | Second string (the subject to match against) |

### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the strings match case-insensitively, `FALSE` otherwise |

### Inner Mechanisms

Uses `preg_match` with the `i` (case-insensitive) and `u` (UTF-8) flags to match `$string2` against a pattern built from `$string1`. The pattern is anchored with `^` and `$` to ensure full-string matching.

### Usage Example

```php
strieq("Hello", "HELLO");  // true
strieq("Hello", "World");  // false
```

---

## substri_count

Counts the number of case-insensitive occurrences of a substring within a source string.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$source` | `string` | The string to search within |
| `$string` | `string` | The substring to search for |

### Return Values

| Type | Description |
|------|-------------|
| `int` | The number of matches found |

### Inner Mechanisms

Uses `preg_match_all` with the `i` (case-insensitive) and `u` (UTF-8) flags. The search string is regex-quoted to treat it as a literal pattern.

### Usage Example

```php
$result = substri_count("Hello hello HELLO world", "hello");
// Result: 3
```

---

## verify_email

Validates an email address against RFC 5321/5322 standards, including support for quoted local identifiers, IP address literals (IPv4 and IPv6), and internationalized domain names via Punycode conversion.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$email` | `string` | The email address to validate |

### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the email is valid, `FALSE` otherwise |

### Inner Mechanisms

1. Matches the email against a comprehensive regex that handles:
   - Quoted local identifiers (with escape sequences)
   - Unquoted local identifiers (with dot-atom rules)
   - IP address literals in brackets (IPv4 and IPv6)
   - Domain names with proper label structure
2. Validates the local identifier length (max 64 characters).
3. For IP-based addresses, delegates to `verify_ip()` and checks total length.
4. For domain-based addresses, converts each label to Punycode and validates label lengths (max 63 characters per label, max 255 total).

### Usage Example

```php
verify_email("user@example.com");           // true
verify_email("user@[192.168.1.1]");          // true
verify_email("user@[IPv6:2001:db8::1]");     // true
verify_email("invalid.email@");             // false
verify_email("a@b.c");                      // false (TLD too short)
```

---

## verify_ip

Validates an IP address (IPv4 or IPv6) against RFC 3986 ABNF grammar.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$ip` | `string` | The IP address to validate |

### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the IP is valid, `FALSE` otherwise |

### Inner Mechanisms

1. Constructs an ABNF-based regex pattern covering:
   - All valid IPv6 address forms (full, compressed with `::`, mixed with IPv4)
   - Standard IPv4 dotted-quad notation
2. Uses `preg_match` with the `i` (case-insensitive) and `D` (end-of-string anchor) flags.
3. Returns the boolean result of the match.

### Usage Example

```php
verify_ip("192.168.1.1");           // true
verify_ip("2001:db8::1");           // true
verify_ip("::1");                   // true
verify_ip("999.999.999.999");       // false
verify_ip("not.an.ip");             // false
```

---

## unique_id

Generates a cryptographically secure random alphanumeric string.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$count` | `int` | `8` | Length of the generated string |

### Return Values

| Type | Description |
|------|-------------|
| `string` | A random string of `$count` characters from `[0-9a-zA-Z]` |

### Inner Mechanisms

1. Uses a static character pool of 62 alphanumeric characters.
2. Iterates `$count` times, selecting a random character using `random_int()` (cryptographically secure).
3. Concatenates and returns the result.

### Usage Example

```php
$result = unique_id(12);
// Result: e.g., "aB3xK9mN2pQ7"

$result = unique_id();
// Result: e.g., "xK9mN2pQ"
```

---

## reference_code

Generates a human-readable reference code using a restricted character set (excluding easily confused characters like `0`, `1`, `O`, `I`, `S`, `Z`) and optional chunking with hyphens.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$count` | `int` | `6` | Total number of characters (excluding hyphens) |
| `$chunk_length` | `int` | `3` | Number of characters per chunk; `0` disables chunking |

### Return Values

| Type | Description |
|------|-------------|
| `string` | A reference code like `"AB3-CD9-E2F"` |

### Inner Mechanisms

1. Uses a static character pool of 25 unambiguous characters: `346789ABCDEFGHJKLMNPQRTUVWXY`.
2. Iterates `$count` times, selecting random characters using `mt_rand()`.
3. Inserts a hyphen after every `$chunk_length` characters (except at position 0).

### Usage Example

```php
$result = reference_code(9, 3);
// Result: e.g., "AB3-CD9-E2F"

$result = reference_code(6, 0);
// Result: e.g., "AB3CD9E2F"
```

---

## strabridge

Truncates a string to a specified length, inserting an ellipsis in the middle to preserve both the beginning and end of the string. Alternatively, can truncate from the end.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | — | The input string to truncate |
| `$length` | `int` | `50` | Maximum total length of the result |
| `$cut_end` | `bool` | `FALSE` | If `TRUE`, truncates from the end instead of the middle |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The truncated string with ellipsis |

### Inner Mechanisms

1. If the string length is within the limit, returns it unchanged.
2. **Middle truncation (default):**
   - Reserves 3 characters for `" … "` (space-ellipsis-space).
   - Allocates 65% of the remaining length to the start and 35% to the end.
   - Concatenates: `start + " … " + end`.
3. **End truncation (`$cut_end = TRUE`):**
   - Reserves 2 characters for `" …"`.
   - Returns the first `$length - 2` characters followed by `" …"`.

### Usage Example

```php
$result = strabridge("The quick brown fox jumps over the lazy dog", 20);
// Result: "The quick brown … over the"

$result = strabridge("The quick brown fox jumps over the lazy dog", 20, TRUE);
// Result: "The quick brown fox …"
```

---

## generate_pseudonym

Generates a random pseudonym composed of Japanese-inspired syllables (Hiragana-like phonetic patterns). Optionally uses a seed for reproducible output.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$seed` | `string\|NULL` | `NULL` | Seed value for reproducible generation |
| `$syllables` | `int` | `3` | Number of syllables in the generated name |

### Return Values

| Type | Description |
|------|-------------|
| `string` | A capitalized pseudonym like `"Takara"` or `"Minori"` |

### Inner Mechanisms

1. Defines a static array of Japanese phonetic syllables (vowels, kana, etc.).
2. If a seed is provided, seeds the random number generator with `srand(crc32($seed))`.
3. Generates `$syllables - 1` random syllables from the base array.
4. Appends one final syllable from the array plus the special character `"n"`.
5. If a seed was used, resets the RNG with `srand()`.
6. Capitalizes the first letter with `ucfirst()`.

### Usage Example

```php
$result = generate_pseudonym();
// Result: e.g., "Takara"

$result = generate_pseudonym("user123", 4);
// Result: e.g., "Minori" (same seed always produces same result)
```

---

## strtocolor

Generates a deterministic HSL color from a string using the djb2 hash function. Optionally wraps the result in an HTML `<span>` with the color applied. Includes logic to ensure consecutive calls produce sufficiently different hues.

### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$string` | `string` | — | The input string to derive a color from |
| `$lightness` | `int` | `75` | Lightness percentage (0–100) |
| `$span` | `bool` | `TRUE` | If `TRUE`, returns an HTML `<span>` with the color; if `FALSE`, returns just the color string |
| `$diff_min` | `int` | `0` | Minimum hue difference from the previous call (0 disables) |

### Return Values

| Type | Description |
|------|-------------|
| `string` | An HTML `<span>` element with inline color style, or a raw `hsl(...)` color string |

### Inner Mechanisms

1. Computes a hue value from the djb2 hash of the string, modulo 360.
2. If a previous hue was set (static variable) and `$diff_min > 0`:
   - Calculates the difference between the current and previous hue.
   - Normalizes the difference to the range [-180, 180].
   - If the absolute difference is less than `$diff_min`, adjusts the hue to maintain minimum separation.
3. Stores the current hue in the static variable for the next call.
4. Constructs an HSL color string.
5. If `$span` is `TRUE`, wraps the XML-escaped input string in a `<span>` with the color as an inline style.

### Usage Example

```php
$result = strtocolor("Alice");
// Result: '<span style="COLOR:hsl(120,75%,75%)">Alice</span>'

$result = strtocolor("Bob", 50, FALSE);
// Result: "hsl(240,50%,50%)"

// Ensuring different hues for consecutive calls
strtocolor("first", 75, FALSE, 60);  // hsl(100,...)
strtocolor("second", 75, FALSE, 60); // hsl(160,...) — at least 60° apart
```


<!-- HASH:9d618ebf63cd1768173f356691dff50b -->
