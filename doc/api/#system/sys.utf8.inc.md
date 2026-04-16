# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.utf8.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## UTF-8 Utility Module (`sys.utf8.inc`)

This file provides a comprehensive set of UTF-8 string manipulation utilities for the NUOS platform. It ensures multibyte-safe operations when the PHP `mbstring` extension is unavailable, offering fallback mechanisms for core string operations like conversion, detection, substring extraction, case transformation, and Unicode normalization.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_UTF8_CHARSET_UTF_8` | `"utf-8"` | UTF-8 character set identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_1` | `"iso-8859-1"` | ISO-8859-1 (Latin-1) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_2` | `"iso-8859-2"` | ISO-8859-2 (Latin-2) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_3` | `"iso-8859-3"` | ISO-8859-3 (Latin-3) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_4` | `"iso-8859-4"` | ISO-8859-4 (Latin-4) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_5` | `"iso-8859-5"` | ISO-8859-5 (Cyrillic) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_6` | `"iso-8859-6"` | ISO-8859-6 (Arabic) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_7` | `"iso-8859-7"` | ISO-8859-7 (Greek) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_8` | `"iso-8859-8"` | ISO-8859-8 (Hebrew) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_9` | `"iso-8859-9"` | ISO-8859-9 (Latin-5) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_10` | `"iso-8859-10"` | ISO-8859-10 (Latin-6) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_11` | `"iso-8859-11"` | ISO-8859-11 (Thai) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_13` | `"iso-8859-13"` | ISO-8859-13 (Latin-7) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_14` | `"iso-8859-14"` | ISO-8859-14 (Latin-8) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_15` | `"iso-8859-15"` | ISO-8859-15 (Latin-9) character set. |
| `CMS_UTF8_CHARSET_ISO_8859_16` | `"iso-8859-16"` | ISO-8859-16 (Latin-10) character set. |

---

### Functions

---

#### `utf8_convert`

**Purpose:**
Converts a string from a specified character set to UTF-8.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string to convert. |
| `$charset` | `string` | Source character set (default: `CMS_UTF8_CHARSET_ISO_8859_1`). |

**Return Values:**
- `string`: Converted UTF-8 string.

**Inner Mechanisms:**
- Prefers `mb_convert_encoding` if available.
- Falls back to a static mapping table loaded from included files for ISO-8859-* character sets.
- Uses `strtr` for conversion if no mapping exists.

**Usage Context:**
- Used when processing user input or external data in non-UTF-8 encodings.
- Essential for legacy system integration.

---

#### `utf8_detect`

**Purpose:**
Detects if a string is valid UTF-8.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | String to validate. |

**Return Values:**
- `bool`: `TRUE` if valid UTF-8, `FALSE` otherwise.

**Inner Mechanisms:**
- Checks for Byte Order Mark (BOM).
- Prefers `mb_check_encoding` if available.
- Uses regex for shorter strings (<1MB) to avoid backtrack limits.
- Falls back to bit-level validation for longer strings.

**Usage Context:**
- Input validation for user-submitted text.
- Sanitization of external data sources.

---

#### `utf8_chr`

**Purpose:**
Generates a UTF-8 character from a Unicode code point.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | Unicode code point. |

**Return Values:**
- `string|bool`: UTF-8 character or `FALSE` on failure.

**Inner Mechanisms:**
- Prefers `mb_chr` if available.
- Manually constructs UTF-8 bytes for code points outside ASCII range.

**Usage Context:**
- Dynamic character generation.
- Unicode-aware string construction.

---

#### `utf8_ord`

**Purpose:**
Returns the Unicode code point of the first character in a string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `int|bool`: Unicode code point or `FALSE` on failure.

**Inner Mechanisms:**
- Prefers `mb_ord` if available.
- Manually decodes UTF-8 byte sequences.

**Usage Context:**
- Character analysis.
- Unicode-aware string processing.

---

#### `utf8_substr`

**Purpose:**
Extracts a substring from a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |
| `$offset` | `int` | Starting position. |
| `$count` | `int|null` | Number of characters to extract (default: `NULL` for end of string). |

**Return Values:**
- `string`: Extracted substring.

**Inner Mechanisms:**
- Prefers `mb_substr` if available.
- Manually calculates byte offsets for character positions.
- Handles negative offsets and counts.

**Usage Context:**
- Safe substring extraction in multibyte environments.
- Text truncation and manipulation.

---

#### `utf8_clean_edges`

**Purpose:**
Removes invalid UTF-8 byte sequences from the start and end of a string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: Cleaned string.

**Inner Mechanisms:**
- Uses regex to strip incomplete UTF-8 sequences at edges.

**Usage Context:**
- Sanitization of binary data or malformed UTF-8 strings.
- Preprocessing before further UTF-8 operations.

---

#### `utf8_strcut`

**Purpose:**
Cuts a UTF-8 string to a specified byte length while preserving character integrity.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |
| `$offset` | `int` | Starting byte position. |
| `$count` | `int|null` | Number of bytes to extract (default: `NULL` for end of string). |

**Return Values:**
- `string`: Extracted substring.

**Inner Mechanisms:**
- Uses `substr` for byte-level cutting.
- Cleans edges with `utf8_clean_edges`.

**Usage Context:**
- Binary-safe string truncation.
- Memory-constrained environments.

---

#### `utf8_strlen`

**Purpose:**
Returns the number of characters in a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `int`: Number of characters.

**Inner Mechanisms:**
- Prefers `mb_strlen` if available.
- Falls back to regex-based counting.

**Usage Context:**
- Length validation.
- Pagination and text processing.

---

#### `utf8_strtoupper`

**Purpose:**
Converts a UTF-8 string to uppercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: Uppercase string.

**Inner Mechanisms:**
- Prefers `mb_strtoupper` if available.
- Falls back to a static mapping table.

**Usage Context:**
- Case normalization.
- Text formatting.

---

#### `utf8_strtolower`

**Purpose:**
Converts a UTF-8 string to lowercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: Lowercase string.

**Inner Mechanisms:**
- Prefers `mb_strtolower` if available.
- Falls back to a static mapping table.

**Usage Context:**
- Case normalization.
- Text comparison and storage.

---

#### `utf8_ucfirst`

**Purpose:**
Capitalizes the first character of a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: String with first character capitalized.

**Inner Mechanisms:**
- Uses `utf8_substr` and `utf8_strtoupper`.

**Usage Context:**
- Sentence capitalization.
- Proper noun formatting.

---

#### `utf8_ucwords`

**Purpose:**
Capitalizes the first character of each word in a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: String with each word capitalized.

**Inner Mechanisms:**
- Uses regex to identify word boundaries.
- Applies `utf8_ucfirst` to each match.

**Usage Context:**
- Title case conversion.
- Name formatting.

---

#### `utf8_ltrim`, `utf8_rtrim`, `utf8_trim`

**Purpose:**
Trims whitespace and Unicode separators from the left, right, or both ends of a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: Trimmed string.

**Inner Mechanisms:**
- Uses regex with Unicode property escapes (`\p{Z}`).

**Usage Context:**
- Input sanitization.
- Text normalization.

---

#### `utf8_strcasecmp`, `utf8_strnatcasecmp`

**Purpose:**
Performs case-insensitive string comparison for UTF-8 strings.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | First string. |
| `$value2` | `string` | Second string. |

**Return Values:**
- `int`: Comparison result (`<0`, `0`, `>0`).

**Inner Mechanisms:**
- Converts strings to lowercase using `utf8_strtolower`.
- Uses `strcmp` or `strnatcmp` for comparison.

**Usage Context:**
- Case-insensitive sorting.
- User input matching.

---

#### `utf8_strspn`

**Purpose:**
Finds the length of the initial segment of a UTF-8 string containing only characters from a mask.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |
| `$mask` | `string` | Allowed characters. |
| `$start` | `int|null` | Starting position (default: `NULL`). |
| `$length` | `int|null` | Maximum length (default: `NULL`). |

**Return Values:**
- `int`: Length of the initial segment.

**Inner Mechanisms:**
- Uses `utf8_substr` for range extraction.
- Uses regex for mask matching.

**Usage Context:**
- Input validation.
- Token extraction.

---

#### `utf8_wordwrap`

**Purpose:**
Wraps a UTF-8 string to a specified line length.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |
| `$length` | `int` | Line length (default: `75`). |
| `$break` | `string` | Line break character (default: `"\n"`). |
| `$cut` | `bool` | Whether to cut words (default: `FALSE`). |

**Return Values:**
- `string`: Wrapped string.

**Inner Mechanisms:**
- Uses regex to split strings at word boundaries or punctuation.
- Supports hard breaks if `$cut` is `TRUE`.

**Usage Context:**
- Text formatting for display.
- Email and document generation.

---

#### `utf8_normalize`

**Purpose:**
Normalizes a UTF-8 string to NFC (composed) or NFD (decomposed) form.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |
| `$compose` | `bool` | `TRUE` for NFC, `FALSE` for NFD (default: `TRUE`). |

**Return Values:**
- `string`: Normalized string.

**Inner Mechanisms:**
- Prefers `Normalizer` class if available.
- Falls back to manual decomposition and composition using static mapping tables.
- Handles Hangul syllables and canonical combining classes.

**Usage Context:**
- Text comparison and storage.
- Unicode compliance.

---

## `utf8` Class

Static utility class for advanced UTF-8 operations and Unicode data processing.

---

### Methods

---

#### `utf8::build`

**Purpose:**
Generates static mapping files for UTF-8 operations from Unicode data files.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Parses Unicode data files (`UnicodeData.txt`, `DerivedNormalizationProps.txt`, etc.).
- Generates mapping files for:
  - Canonical combining classes.
  - Decomposition and composition mappings.
  - Quick check tables for normalization.
  - ISO-8859-* to UTF-8 conversion tables.
- Handles Hangul syllable decomposition and composition.

**Usage Context:**
- Development and build processes.
- Unicode data updates.

---

#### `utf8::test`

**Purpose:**
Validates UTF-8 normalization against the Unicode Normalization Test file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$limit` | `int` | Maximum number of test lines to process (default: `20000`). |

**Return Values:**
- `bool`: `FALSE` if all tests pass, `TRUE` if errors are found.

**Inner Mechanisms:**
- Parses `NormalizationTest.txt`.
- Compares expected and actual normalization results.

**Usage Context:**
- Quality assurance.
- Unicode compliance testing.

---

#### `utf8::get_canonical_combining_class`

**Purpose:**
Returns the canonical combining class of a Unicode character.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Unicode character. |

**Return Values:**
- `int`: Combining class (0 for starters).

**Inner Mechanisms:**
- Uses a static mapping table.

**Usage Context:**
- Unicode normalization.
- Text rendering.

---

#### `utf8::decompose`

**Purpose:**
Decomposes a UTF-8 string into its canonical decomposed form (NFD).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string (passed by reference). |

**Return Values:**
- `string`: Decomposed string.

**Inner Mechanisms:**
- Recursively decomposes characters using `_decompose` and `__decompose`.

**Usage Context:**
- Unicode normalization.
- Text processing.

---

#### `utf8::sort`

**Purpose:**
Sorts combining characters in a UTF-8 string according to their canonical combining class.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: Sorted string.

**Inner Mechanisms:**
- Groups characters by combining class.
- Sorts and reassembles the string.

**Usage Context:**
- Unicode normalization.
- Text rendering.

---

#### `utf8::compose`

**Purpose:**
Composes a UTF-8 string from its canonical decomposed form (NFC).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string. |

**Return Values:**
- `string`: Composed string.

**Inner Mechanisms:**
- Uses `_compose` to combine characters.
- Handles Hangul syllables and combining sequences.

**Usage Context:**
- Unicode normalization.
- Text storage.

---

#### `utf8::get_character`

**Purpose:**
Extracts a single UTF-8 character from a string at a given offset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string (passed by reference). |
| `$offset` | `int` | Byte offset (updated to next character). |

**Return Values:**
- `string|bool`: UTF-8 character or `FALSE` on failure.

**Inner Mechanisms:**
- Uses regex to match valid UTF-8 sequences.
- Updates the offset to the next character.

**Usage Context:**
- Character-by-character processing.
- UTF-8 validation.

---

#### `utf8::ucd_get_record`

**Purpose:**
Parses a record from a Unicode data file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | File handle. |

**Return Values:**
- `array|bool`: Parsed record or `FALSE` on failure.

**Inner Mechanisms:**
- Reads lines from the file.
- Splits records by semicolons and trims fields.

**Usage Context:**
- Unicode data processing.
- Build scripts.

---

#### `utf8::ucd_extract_code_point`

**Purpose:**
Extracts Unicode code points from a UCD record field.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | UCD field value. |

**Return Values:**
- `array`: Array of UTF-8 characters.

**Inner Mechanisms:**
- Handles code point ranges (e.g., `0041..0045`).
- Converts code points to UTF-8 characters.

**Usage Context:**
- Unicode data processing.
- Build scripts.


<!-- HASH:c484360be2f08ed98425c4f213b9a819 -->
