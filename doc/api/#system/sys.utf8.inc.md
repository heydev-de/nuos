# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.utf8.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.utf8.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/sys.utf8.inc

## Overview

This file is the core UTF-8 multibyte string handling library for the PWNC Web Platform. It provides a comprehensive set of functions and a static `utf8` class that together offer full Unicode support including encoding conversion, character detection, string manipulation, case conversion, normalization (NFC/NFD), and composition/decomposition of Unicode characters.

The library follows a **prefer-native, fallback-to-pure-PHP** strategy: every function first attempts to use PHP's built-in `mb_*` functions (Multibyte String extension) or the `Normalizer` class. If those extensions are unavailable, it falls back to custom pure-PHP implementations that operate at the byte level, ensuring consistent UTF-8 handling regardless of the server environment.

The `utf8` class also includes a `build()` method that generates lookup tables from official Unicode data files, and a `test()` method that validates normalization correctness against the Unicode NormalizationTest.txt test suite.

## Character Set Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_UTF8_CHARSET_UTF_8` | `"utf-8"` | UTF-8 encoding identifier |
| `CMS_UTF8_CHARSET_CP1252` | `"iso-8859-1"` | Windows-1252 / CP1252 mapped to ISO-8859-1 |
| `CMS_UTF8_CHARSET_WINDOWS_1252` | `"iso-8859-1"` | Windows-1252 mapped to ISO-8859-1 |
| `CMS_UTF8_CHARSET_ISO_8859_1` | `"iso-8859-1"` | ISO-8859-1 (Latin-1) |
| `CMS_UTF8_CHARSET_ISO_8859_2` | `"iso-8859-2"` | ISO-8859-2 (Latin-2) |
| `CMS_UTF8_CHARSET_ISO_8859_3` | `"iso-8859-3"` | ISO-8859-3 (Latin-3) |
| `CMS_UTF8_CHARSET_ISO_8859_4` | `"iso-8859-4"` | ISO-8859-4 (Latin-4) |
| `CMS_UTF8_CHARSET_ISO_8859_5` | `"iso-8859-5"` | ISO-8859-5 (Cyrillic) |
| `CMS_UTF8_CHARSET_ISO_8859_6` | `"iso-8859-6"` | ISO-8859-6 (Arabic) |
| `CMS_UTF8_CHARSET_ISO_8859_7` | `"iso-8859-7"` | ISO-8859-7 (Greek) |
| `CMS_UTF8_CHARSET_ISO_8859_8` | `"iso-8859-8"` | ISO-8859-8 (Hebrew) |
| `CMS_UTF8_CHARSET_ISO_8859_9` | `"iso-8859-9"` | ISO-8859-9 (Turkish) |
| `CMS_UTF8_CHARSET_ISO_8859_10` | `"iso-8859-10"` | ISO-8859-10 (Latin-6) |
| `CMS_UTF8_CHARSET_ISO_8859_11` | `"iso-8859-11"` | ISO-8859-11 (Thai) |
| `CMS_UTF8_CHARSET_ISO_8859_13` | `"iso-8859-13"` | ISO-8859-13 (Latin-7) |
| `CMS_UTF8_CHARSET_ISO_8859_14` | `"iso-8859-14"` | ISO-8859-14 (Latin-8) |
| `CMS_UTF8_CHARSET_ISO_8859_15` | `"iso-8859-15"` | ISO-8859-15 (Latin-9) |
| `CMS_UTF8_CHARSET_ISO_8859_16` | `"iso-8859-16"` | ISO-8859-16 (Latin-10) |

---

## Standalone Functions

### utf8_convert

Converts a string from a specified single-byte or legacy character encoding to UTF-8.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | — | The input string to convert |
| `$charset` | `string` | `CMS_UTF8_CHARSET_ISO_8859_1` | The source character set (e.g., `"iso-8859-1"`, `"iso-8859-15"`) |

**Return Value**

`string` — The UTF-8 encoded string. If conversion fails or the charset is unsupported, the original string is returned unchanged.

**Inner Mechanisms**

1. Casts input to string.
2. If `mb_convert_encoding` is available, uses it with error suppression (`@`). On failure or exception, returns the original value.
3. If `mbstring` is unavailable, loads a static lookup map from a pre-generated `.inc` file (e.g., `iso_8859_1.inc`) located in `CMS_SYSTEM_PATH/utf8/`.
4. Normalizes `cp1252`/`windows-1252` to `iso-8859-1`.
5. Uses `strtr()` to perform byte-level replacement using the loaded map.

**Usage Example**

```php
// Convert a Latin-1 string to UTF-8
$latin1 = "Caf\xe9"; // "Café" in ISO-8859-1
$utf8 = utf8_convert($latin1, CMS_UTF8_CHARSET_ISO_8859_1);
echo $utf8; // "Café" in UTF-8
```

---

### utf8_detect

Detects whether a string is valid UTF-8.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The string to check |

**Return Value**

`bool` — `TRUE` if the string is valid UTF-8 (or empty), `FALSE` otherwise.

**Inner Mechanisms**

1. Returns `TRUE` for empty strings.
2. Checks for a UTF-8 BOM (`\xEF\xBB\xBF`) and returns `TRUE` if found.
3. If `mb_check_encoding` is available, delegates to it.
4. For strings under 1 MB, uses a comprehensive regex that validates all UTF-8 byte sequences (1–4 bytes), excluding overlong encodings, surrogates, and out-of-range code points.
5. For larger strings, performs a manual byte-level scan checking continuation byte patterns.

**Usage Example**

```php
if (utf8_detect($input)) {
    echo "Valid UTF-8";
} else {
    echo "Invalid UTF-8 — needs conversion";
}
```

---

### utf8_chr

Returns the UTF-8 byte sequence for a given Unicode code point.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | Unicode code point (e.g., `0x00E9` for é) |

**Return Value**

`string|false` — The UTF-8 encoded character, or `FALSE` if the code point is invalid (≥ 0x200000).

**Inner Mechanisms**

1. If `mb_chr` is available, delegates to it.
2. Otherwise, manually constructs the UTF-8 byte sequence:
   - 1 byte for code points < 128 (ASCII)
   - 2 bytes for code points < 2048
   - 3 bytes for code points < 65536
   - 4 bytes for code points < 2097152

**Usage Example**

```php
echo utf8_chr(0x1F600); // 😀 (grinning face emoji)
echo utf8_chr(0x00E9);  // é
```

---

### utf8_ord

Returns the Unicode code point of the first character in a UTF-8 string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | A UTF-8 string (only the first character is examined) |

**Return Value**

`int|false` — The Unicode code point, or `FALSE` if the string is empty or the first byte is invalid.

**Inner Mechanisms**

1. Returns `FALSE` for empty strings (using `stre()`).
2. If `mb_ord` is available, delegates to it.
3. Otherwise, manually decodes the first UTF-8 character by examining the leading byte's bit pattern and reading the appropriate number of continuation bytes.

**Usage Example**

```php
$code = utf8_ord("é"); // Returns 233 (0xE9)
$code = utf8_ord("😀"); // Returns 128512 (0x1F600)
```

---

### utf8_substr

Returns a substring of a UTF-8 string, operating on characters rather than bytes.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | — | The input string |
| `$offset` | `int` | — | Character offset to start from. Negative values count from the end |
| `$count` | `int\|null` | `NULL` | Number of characters to extract. `NULL` means "to the end". Negative values stop that many characters from the end |

**Return Value**

`string` — The extracted substring.

**Inner Mechanisms**

1. Returns `""` if `$count` is `0`.
2. If `mb_substr` is available, delegates to it.
3. Otherwise, uses a closure `$f` that walks the string byte-by-byte, counting UTF-8 characters and returning the byte offset for a given character position.
4. Handles negative offsets and counts by first computing the total character length.
5. Uses `substr()` for the final byte-level extraction.

**Usage Example**

```php
$str = "Hello 🌍 World";
echo utf8_substr($str, 6, 1);  // "🌍"
echo utf8_substr($str, 0, 5);  // "Hello"
echo utf8_substr($str, -5);    // "World"
```

---

### utf8_clean_edges

Removes incomplete UTF-8 byte sequences from the beginning and end of a string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The cleaned string with partial multi-byte sequences stripped from both edges.

**Inner Mechanisms**

Uses two regex patterns:
1. `/^[\x80-\xBF]+/` — Strips leading continuation bytes (0x80–0xBF) that don't form a valid character.
2. `/[\xC0-\xDF]$|[\xE0-\xEF][\x80-\xBF]?$|[\xF0-\xF4][\x80-\xBF]{0,2}$/` — Strips trailing incomplete 2-, 3-, or 4-byte sequences.

**Usage Example**

```php
// After a byte-level truncation, clean up partial characters
$truncated = substr($utf8_string, 0, 10);
$clean = utf8_clean_edges($truncated);
```

---

### utf8_strcut

Cuts a UTF-8 string to a specified byte length, cleaning up any partial characters at the boundary.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | — | The input string |
| `$offset` | `int` | — | Byte offset to start from |
| `$count` | `int\|null` | `NULL` | Maximum byte length. `NULL` means "to the end" |

**Return Value**

`string` — The cut string with partial characters removed from edges.

**Inner Mechanisms**

1. Casts input to string.
2. Uses `substr()` for byte-level cutting (with a workaround for PHP < 8.0 where `substr` doesn't accept `NULL` for length).
3. Passes the result through `utf8_clean_edges()` to remove any partial multi-byte sequences at the cut boundary.

**Usage Example**

```php
$str = "Hello 🌍 World";
echo utf8_strcut($str, 0, 8); // "Hello " (emoji bytes truncated and cleaned)
```

---

### utf8_strlen

Returns the number of UTF-8 characters in a string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`int` — The character count.

**Inner Mechanisms**

1. If `mb_strlen` is available, delegates to it with `"utf-8"` encoding.
2. Otherwise, uses `preg_match_all("/./u", $value)` to count all valid UTF-8 characters.

**Usage Example**

```php
echo utf8_strlen("Hello 🌍"); // 7 (not 9 bytes)
```

---

### utf8_strtoupper

Converts a UTF-8 string to uppercase.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The uppercased string.

**Inner Mechanisms**

1. If `mb_strtoupper` is available, delegates to it.
2. Otherwise, loads a static lookup table from `simple_uppercase_mapping.inc` (generated by `utf8::build()`) and applies it via `strtr()`.

**Usage Example**

```php
echo utf8_strtoupper("straße"); // "STRASSE"
```

---

### utf8_strtolower

Converts a UTF-8 string to lowercase.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The lowercased string.

**Inner Mechanisms**

1. If `mb_strtolower` is available, delegates to it.
2. Otherwise, loads a static lookup table from `simple_lowercase_mapping.inc` and applies it via `strtr()`.

**Usage Example**

```php
echo utf8_strtolower("ÉÀÇ"); // "éàç"
```

---

### utf8_ucfirst

Uppercases the first character of a UTF-8 string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The string with the first character uppercased.

**Inner Mechanisms**

Combines `utf8_strtoupper()` on the first character (via `utf8_substr($value, 0, 1)`) with the rest of the string unchanged.

**Usage Example**

```php
echo utf8_ucfirst("hello"); // "Hello"
echo utf8_ucfirst("éclair"); // "Éclair"
```

---

### utf8_ucwords

Uppercases the first character of each word in a UTF-8 string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The string with each word's first character uppercased.

**Inner Mechanisms**

Uses `preg_replace_callback` with a Unicode-aware regex that matches:
- A letter not preceded by a letter, number, or currency symbol (word boundary)
- A letter at the start of the string or after a math symbol, separator, or joint character

Each match is passed through `utf8_ucfirst()`.

**Usage Example**

```php
echo utf8_ucwords("hello world éclair"); // "Hello World Éclair"
```

---

### utf8_ltrim

Left-trims whitespace and Unicode separator characters from a UTF-8 string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The trimmed string.

**Inner Mechanisms**

Uses `preg_replace("/^[\s\p{Z}\0\x0B]+/u", "", ...)` to strip leading whitespace, Unicode separators (`\p{Z}`), null bytes, and vertical tabs.

**Usage Example**

```php
echo utf8_ltrim("  hello"); // "hello"
```

---

### utf8_rtrim

Right-trims whitespace and Unicode separator characters from a UTF-8 string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The trimmed string.

**Inner Mechanisms**

Uses `preg_replace("/[\s\p{Z}\0\x0B]+$/u", "", ...)` to strip trailing whitespace, Unicode separators, null bytes, and vertical tabs.

**Usage Example**

```php
echo utf8_rtrim("hello  "); // "hello"
```

---

### utf8_trim

Trims whitespace and Unicode separator characters from both ends of a UTF-8 string.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |

**Return Value**

`string` — The trimmed string.

**Inner Mechanisms**

Combines the patterns from `utf8_ltrim` and `utf8_rtrim` into a single regex: `/^[\s\p{Z}\0\x0B]+|[\s\p{Z}\0\x0B]+$/u`.

**Usage Example**

```php
echo utf8_trim("  hello  "); // "hello"
```

---

### utf8_strcasecmp

Case-insensitive string comparison for UTF-8 strings.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | First string |
| `$value2` | `string` | Second string |

**Return Value**

`int` — `< 0` if `$value1` is less than `$value2`, `0` if equal, `> 0` if greater.

**Inner Mechanisms**

1. Converts both strings to lowercase using `utf8_strtolower()`.
2. Compares using PHP's native `strcmp()`.

**Usage Example**

```php
if (utf8_strcasecmp("École", "école") === 0) {
    echo "Equal (case-insensitive)";
}
```

---

### utf8_strnatcasecmp

Natural-order, case-insensitive string comparison for UTF-8 strings.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | First string |
| `$value2` | `string` | Second string |

**Return Value**

`int` — `< 0`, `0`, or `> 0` as in `utf8_strcasecmp`.

**Inner Mechanisms**

1. Converts both strings to lowercase using `utf8_strtolower()`.
2. Compares using PHP's native `strnatcmp()` (natural order algorithm).

**Usage Example**

```php
echo utf8_strnatcasecmp("file2.txt", "file10.txt"); // -1 (2 comes before 10 in natural order)
```

---

### utf8_strspn

Returns the length of the initial segment of a UTF-8 string that consists entirely of characters from a given mask.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | — | The input string |
| `$mask` | `string` | — | The character mask |
| `$start` | `int\|null` | `NULL` | Character offset to start from |
| `$length` | `int\|null` | `NULL` | Maximum character length to examine |

**Return Value**

`int` — The number of characters in the initial segment matching the mask.

**Inner Mechanisms**

1. If `$start` or `$length` is provided, extracts the relevant substring using `utf8_substr()`.
2. Escapes the mask for regex use with `preg_quote()`.
3. Uses `preg_match("/^[$mask]+/u", ...)` to find the matching prefix.
4. Returns the character length of the match (via `utf8_strlen()`), or `0` if no match.

**Usage Example**

```php
echo utf8_strspn("hello123", "abcdefghijklmnopqrstuvwxyz"); // 5
```

---

### utf8_wordwrap

Wraps a UTF-8 string to a specified character length, breaking at word boundaries.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | — | The input string |
| `$length` | `int` | `75` | Maximum line length (clamped to 1–65535) |
| `$break` | `string` | `"\n"` | The line break string |
| `$cut` | `bool` | `FALSE` | If `TRUE`, hard-breaks long words at `$length` |

**Return Value**

`string` — The wrapped string.

**Inner Mechanisms**

1. Clamps `$length` to the range [1, 65535].
2. Builds a regex pattern that matches:
   - Up to `$length` characters followed by a separator or end of string
   - Up to `$length` characters followed by punctuation (hyphen, closing brackets, etc.)
   - If `$cut` is `TRUE`: exactly `$length` characters (hard break)
   - If `$cut` is `FALSE`: one or more characters (no break for very long words)
3. Uses `preg_match_all()` to find all segments, then joins them with `$break`.

**Usage Example**

```php
$text = "This is a long UTF-8 string that needs wrapping.";
echo utf8_wordwrap($text, 20);
// This is a long UTF-8
// string that needs
// wrapping.
```

---

### utf8_normalize

Normalizes a UTF-8 string to Unicode Normalization Form C (NFC) or Form D (NFD).

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | — | The input string |
| `$compose` | `bool` | `TRUE` | `TRUE` for NFC (composed), `FALSE` for NFD (decomposed) |

**Return Value**

`string` — The normalized string.

**Inner Mechanisms**

1. Returns the string unchanged if it contains only ASCII characters.
2. If the `Normalizer` class is available, delegates to `Normalizer::normalize()` with `FORM_C` or `FORM_D`.
3. Otherwise, implements the Unicode normalization algorithm manually:
   - Loads quick-check tables (`nfd_quick_check.inc`, `nfc_quick_check.inc`) to skip already-normalized characters.
   - Uses a buffer to accumulate non-starter characters (combining marks).
   - When a starter character is encountered, flushes the buffer by either composing (NFC) or sorting (NFD) the accumulated characters.
   - Uses `utf8::get_canonical_combining_class()`, `utf8::decompose()`, `utf8::compose()`, and `utf8::sort()` for the actual normalization operations.

**Usage Example**

```php
// Normalize a string that may contain decomposed characters
$normalized = utf8_normalize("é", TRUE); // Ensures NFC form
```

---

## Class: utf8

The `utf8` class provides static methods for Unicode normalization internals, lookup table generation, and Unicode Character Database (UCD) file parsing.

### utf8::build

Generates all UTF-8 lookup table files from official Unicode data files.

**Parameters**

None.

**Return Value**

`bool` — `TRUE` on success, `FALSE` if required Unicode data files cannot be opened.

**Inner Mechanisms**

1. **Composition Exclusions**: Reads `CompositionExclusions.txt` to build a set of code points that should not be composed.
2. **Unicode Data**: Reads `UnicodeData.txt` and generates five lookup tables:
   - `canonical_combining_class.inc` — Maps code points to their canonical combining class
   - `canonical_decomposition_mapping.inc` — Maps code points to their canonical decomposition
   - `canonical_composition_mapping.inc` — Maps decompositions back to composed forms (excluding composition exclusions)
   - `simple_uppercase_mapping.inc` — Maps code points to their uppercase equivalents
   - `simple_lowercase_mapping.inc` — Maps code points to their lowercase equivalents
3. **Hangul Syllables**: Generates decomposition and composition mappings for all 11,172 Hangul syllables using the algorithmic formula.
4. **Derived Normalization Props**: Reads `DerivedNormalizationProps.txt` to generate:
   - `nfd_quick_check.inc` — Quick-check table for NFD
   - `nfc_quick_check.inc` — Quick-check table for NFC
5. **ISO Mapping**: Reads ISO-8859 mapping files (e.g., `8859-1.TXT`) and generates per-charset conversion tables (e.g., `iso_8859_1.inc`).

**Usage Example**

```php
// Run once during installation or when Unicode data is updated
if (utf8::build()) {
    echo "UTF-8 lookup tables generated successfully";
}
```

---

### utf8::test

Validates the normalization implementation against the official Unicode NormalizationTest.txt test suite.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$limit` | `int` | `20000` | Maximum number of test lines to process. `0` means no limit |

**Return Value**

`bool` — `TRUE` if all tests pass (no errors), `FALSE` if errors were found.

**Inner Mechanisms**

1. Opens `NormalizationTest.txt` from `CMS_PATH/unicode/`.
2. For each test line, extracts 5 code point sequences (c1–c5).
3. Normalizes each sequence using `utf8_normalize()`.
4. Compares results against expected values:
   - `NFC(c1)` should equal `c2`
   - `NFC(c2)` should equal `c2`
   - `NFC(c3)` should equal `c2`
   - `NFC(c4)` should equal `c4`
   - `NFC(c5)` should equal `c4`
5. Reports any mismatches and returns `FALSE` if any errors were found.

**Usage Example**

```php
// Verify normalization correctness after building tables
if (utf8::test()) {
    echo "All normalization tests passed";
} else {
    echo "Normalization errors detected!";
}
```

---

### utf8::get_canonical_combining_class

Returns the canonical combining class (CCC) for a given Unicode code point.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | A UTF-8 character (single character) |

**Return Value**

`int` — The canonical combining class (0 for starters, non-zero for combining marks).

**Inner Mechanisms**

1. Loads the static lookup table from `canonical_combining_class.inc` (cached after first load).
2. Returns the class if found, or `0` if the character is not in the table.

**Usage Example**

```php
$ccc = utf8::get_canonical_combining_class("é"); // 0 (starter)
$ccc = utf8::get_canonical_combining_class("\u{0301}"); // 230 (combining acute accent)
```

---

### utf8::decompose

Recursively decomposes a UTF-8 string into its canonical decomposition form.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string (passed by reference) |

**Return Value**

`string` — The fully decomposed string.

**Inner Mechanisms**

1. Delegates to `utf8::_decompose()` which iterates through each character.
2. For each character, calls `utf8::__decompose()` to look up its canonical decomposition mapping.
3. If a decomposition exists, recursively decomposes the result.
4. If no decomposition exists, appends the character as-is.

**Usage Example**

```php
$decomposed = utf8::decompose("é"); // "e" + combining acute accent
```

---

### utf8::_decompose

Internal recursive decomposition worker.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string |
| `$result` | `string` | Output parameter (passed by reference) accumulating the decomposed result |

**Return Value**

`void` (result is accumulated in `$result`).

**Inner Mechanisms**

1. Iterates through the string character by character using `utf8::get_character()`.
2. For each character, looks up its decomposition via `utf8::__decompose()`.
3. If a decomposition exists, recursively calls itself on the decomposed form.
4. If no decomposition exists, appends the character directly to `$result`.

---

### utf8::__decompose

Looks up the canonical decomposition mapping for a single character.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | A single UTF-8 character |

**Return Value**

`string|false` — The decomposed form, or `FALSE` if no decomposition mapping exists.

**Inner Mechanisms**

1. Loads the static lookup table from `canonical_decomposition_mapping.inc` (cached after first load).
2. Returns the mapped value if found, or `FALSE` otherwise.

---

### utf8::sort

Sorts combining characters in a string by their canonical combining class, implementing the Unicode Canonical Ordering Algorithm.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string (already decomposed) |

**Return Value**

`string` — The string with combining characters sorted by CCC.

**Inner Mechanisms**

1. Iterates through the string character by character.
2. For starter characters (CCC = 0), flushes the accumulated non-starter buffer: sorts it by CCC (using `ksort` with a composite key of `CCC * 1000 + position`), appends to result, then appends the starter.
3. For non-starter characters (CCC ≠ 0), adds them to a buffer keyed by `CCC * 1000 + position` to preserve insertion order within the same class.
4. At the end, flushes any remaining buffer.

**Usage Example**

```php
// After decomposition, sort combining marks by canonical order
$sorted = utf8::sort($decomposed_string);
```

---

### utf8::compose

Composes a decomposed UTF-8 string into its composed form (NFC), implementing the Unicode composition algorithm.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string (already decomposed and sorted) |

**Return Value**

`string` — The composed string.

**Inner Mechanisms**

1. Gets the first character as the initial starter.
2. Iterates through remaining characters:
   - **Starter + Starter**: Attempts to compose the two using `utf8::_compose()`. If successful (e.g., Hangul LV composition), checks for a third character to form an LVT syllable. If not, flushes the starter and starts a new one.
   - **Non-starter with higher CCC**: Attempts to compose with the current starter if not blocked by canonical ordering rules.
   - **Otherwise**: Appends the character as a non-starter to the current sequence.
3. At the end, appends the final starter and any remaining non-starters.

**Usage Example**

```php
// After decomposition and sorting, compose back to NFC
$composed = utf8::compose($decomposed_sorted_string);
```

---

### utf8::_compose

Looks up the canonical composition mapping for a two-character sequence.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | A two-character UTF-8 sequence |

**Return Value**

`string|false` — The composed character, or `FALSE` if no composition mapping exists.

**Inner Mechanisms**

1. Loads the static lookup table from `canonical_composition_mapping.inc` (cached after first load).
2. Returns the mapped value if found, or `FALSE` otherwise.

---

### utf8::get_character

Extracts a single UTF-8 character from a string at a given byte offset, advancing the offset.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string (passed by reference) |
| `$offset` | `int` | Byte offset (passed by reference, advanced by the character's byte length) |

**Return Value**

`string|false` — The extracted character, or `FALSE` if the byte at the offset is invalid.

**Inner Mechanisms**

1. Uses a single regex with the `/S` (study) modifier to match one UTF-8 character at the given offset.
2. The regex covers all valid UTF-8 byte sequences (1–4 bytes) and includes a fallback `(.)` pattern to match invalid bytes.
3. If the fallback matched (result[1] is set), returns `FALSE`.
4. Otherwise, advances `$offset` by the matched character's byte length and returns the character.

**Usage Example**

```php
$str = "Hello 🌍";
$offset = 6;
$char = utf8::get_character($str, $offset); // "🌍"
echo $offset; // 10 (advanced by 4 bytes)
```

---

### utf8::ucd_get_record

Reads a single record from a Unicode Character Database (UCD) file handle.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | A file handle opened for reading |

**Return Value**

`array|false` — An array of trimmed field values, or `FALSE` if EOF is reached.

**Inner Mechanisms**

1. Reads lines from the file handle until a non-empty, non-comment line is found.
2. Strips comments (everything after `#`).
3. Splits the line by `;` (UCD field separator).
4. Trims each field.
5. Returns the array of fields, or `FALSE` at EOF.

**Usage Example**

```php
$hfile = fopen(CMS_PATH . "unicode/UnicodeData.txt", "rb");
while ($record = utf8::ucd_get_record($hfile)) {
    $code = $record[0]; // Code point
    $name = $record[1]; // Character name
    // ...
}
fclose($hfile);
```

---

### utf8::ucd_extract_code_point

Expands a Unicode code point specification (which may include ranges) into individual characters.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | A code point specification (e.g., `"0041"`, `"0041..005A"`) |

**Return Value**

`array` — An array of UTF-8 characters corresponding to each code point in the specification.

**Inner Mechanisms**

1. Splits the input by spaces (to handle multiple ranges/values).
2. For each token, splits by `..` to detect ranges.
3. If a range is found, iterates from the start to end code point, converting each to a UTF-8 character via `utf8_chr()`.
4. If a single code point, converts it directly.
5. Returns the array of UTF-8 characters.

**Usage Example**

```php
$chars = utf8::ucd_extract_code_point("0041..0043");
// Returns ["A", "B", "C"]
```


<!-- HASH:a1e3dcb4ab2ee019fd6fcccb0fb8710f -->
