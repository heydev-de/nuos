# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.utf8.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.utf8.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## UTF-8 Utility Functions and Class

This file provides a comprehensive set of UTF-8 string manipulation utilities for the PWNC Web Platform. It includes functions for character encoding conversion, detection, substring extraction, case conversion, normalization, and Unicode data processing. The `utf8` class contains static methods for building Unicode data mappings and performing advanced normalization operations.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_UTF8_CHARSET_UTF_8` | `"utf-8"` | UTF-8 character set identifier |
| `CMS_UTF8_CHARSET_CP1252` | `"iso-8859-1"` | Windows-1252 (CP1252) character set identifier (alias for ISO-8859-1) |
| `CMS_UTF8_CHARSET_WINDOWS_1252` | `"iso-8859-1"` | Windows-1252 character set identifier (alias for ISO-8859-1) |
| `CMS_UTF8_CHARSET_ISO_8859_1` | `"iso-8859-1"` | ISO-8859-1 (Latin-1) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_2` | `"iso-8859-2"` | ISO-8859-2 (Latin-2) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_3` | `"iso-8859-3"` | ISO-8859-3 (Latin-3) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_4` | `"iso-8859-4"` | ISO-8859-4 (Latin-4) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_5` | `"iso-8859-5"` | ISO-8859-5 (Cyrillic) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_6` | `"iso-8859-6"` | ISO-8859-6 (Arabic) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_7` | `"iso-8859-7"` | ISO-8859-7 (Greek) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_8` | `"iso-8859-8"` | ISO-8859-8 (Hebrew) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_9` | `"iso-8859-9"` | ISO-8859-9 (Latin-5) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_10` | `"iso-8859-10"` | ISO-8859-10 (Latin-6) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_11` | `"iso-8859-11"` | ISO-8859-11 (Thai) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_13` | `"iso-8859-13"` | ISO-8859-13 (Latin-7) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_14` | `"iso-8859-14"` | ISO-8859-14 (Latin-8) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_15` | `"iso-8859-15"` | ISO-8859-15 (Latin-9) character set identifier |
| `CMS_UTF8_CHARSET_ISO_8859_16` | `"iso-8859-16"` | ISO-8859-16 (Latin-10) character set identifier |

---

### Functions

---

#### `utf8_convert`

**Purpose:**
Converts a string from a specified character set to UTF-8. Falls back to custom mapping if `mb_convert_encoding` is unavailable.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string to convert |
| `$charset` | `string` | Source character set (default: `CMS_UTF8_CHARSET_ISO_8859_1`) |

**Return Values:**
- `string`: Converted UTF-8 string or original string if conversion fails

**Inner Mechanisms:**
- Uses `mb_convert_encoding` if available
- Falls back to static mapping files for ISO-8859-* character sets
- Handles CP1252 and Windows-1252 as aliases for ISO-8859-1

**Usage Context:**
- Converting legacy encoded strings to UTF-8 for consistent processing
- Handling user input from different character encodings

**Example:**
```php
$isoString = "Héllò Wörld";
$utf8String = utf8_convert($isoString, CMS_UTF8_CHARSET_ISO_8859_1);
// $utf8String now contains "Héllò Wörld" in UTF-8 encoding
```

---

#### `utf8_detect`

**Purpose:**
Detects if a string is valid UTF-8 encoded.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | String to check |

**Return Values:**
- `bool`: `TRUE` if valid UTF-8, `FALSE` otherwise

**Inner Mechanisms:**
- Checks for UTF-8 Byte Order Mark (BOM)
- Uses `mb_check_encoding` if available
- Falls back to regex pattern matching for shorter strings
- Performs bit-level validation for longer strings

**Usage Context:**
- Validating user input or external data before processing
- Ensuring data integrity before storage or transmission

**Example:**
```php
$userInput = $_POST['text'];
if (utf8_detect($userInput)) {
    // Process UTF-8 string
} else {
    // Convert or reject non-UTF-8 input
    $userInput = utf8_convert($userInput);
}
```

---

#### `utf8_chr`

**Purpose:**
Generates a UTF-8 character from a Unicode code point.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | Unicode code point |

**Return Values:**
- `string|false`: UTF-8 character or `FALSE` for invalid code points

**Inner Mechanisms:**
- Uses `mb_chr` if available
- Manually constructs UTF-8 byte sequences for 1-4 byte characters

**Usage Context:**
- Generating special characters programmatically
- Building strings from Unicode code points

**Example:**
```php
$copyrightSymbol = utf8_chr(0x00A9); // "©"
$emoji = utf8_chr(0x1F600); // "😀"
```

---

#### `utf8_ord`

**Purpose:**
Gets the Unicode code point of the first character in a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | UTF-8 string |

**Return Values:**
- `int|false`: Unicode code point or `FALSE` for invalid UTF-8

**Inner Mechanisms:**
- Uses `mb_ord` if available
- Manually decodes UTF-8 byte sequences for 1-4 byte characters

**Usage Context:**
- Analyzing character properties
- Implementing custom string processing algorithms

**Example:**
```php
$code = utf8_ord("A"); // 65
$code = utf8_ord("©"); // 169
```

---

#### `utf8_substr`

**Purpose:**
Extracts a substring from a UTF-8 string with character-based offsets.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |
| `$offset` | `int` | Starting position (negative counts from end) |
| `$count` | `int|null` | Number of characters to extract (negative counts from end) |

**Return Values:**
- `string`: Extracted substring

**Inner Mechanisms:**
- Uses `mb_substr` if available
- Manually calculates byte offsets for character positions
- Handles negative offsets and counts

**Usage Context:**
- Truncating text for display
- Extracting portions of multibyte strings

**Example:**
```php
$text = "Héllò Wörld";
$substring = utf8_substr($text, 6, 5); // "Wörld"
$end = utf8_substr($text, -5); // "örld"
```

---

#### `utf8_clean_edges`

**Purpose:**
Removes invalid UTF-8 byte sequences from the start and end of a string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `string`: Cleaned string

**Inner Mechanisms:**
- Uses regex to remove incomplete UTF-8 sequences at edges
- Preserves valid UTF-8 characters

**Usage Context:**
- Cleaning strings after byte-level operations
- Ensuring valid UTF-8 after substring extraction

**Example:**
```php
$dirtyString = "\xC3\xA9llo\xC3"; // "éllo" with incomplete UTF-8 at end
$cleanString = utf8_clean_edges($dirtyString); // "éllo"
```

---

#### `utf8_strcut`

**Purpose:**
Extracts a substring with byte-based offsets while ensuring valid UTF-8 at edges.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |
| `$offset` | `int` | Starting byte position |
| `$count` | `int|null` | Number of bytes to extract |

**Return Values:**
- `string`: Extracted substring with cleaned edges

**Inner Mechanisms:**
- Uses `substr` for byte-level extraction
- Cleans edges with `utf8_clean_edges`

**Usage Context:**
- Implementing byte-limited string operations
- Working with binary-safe string manipulation

**Example:**
```php
$text = "Héllò Wörld";
$substring = utf8_strcut($text, 1, 5); // "éll"
```

---

#### `utf8_strlen`

**Purpose:**
Gets the number of characters in a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `int`: Number of characters

**Inner Mechanisms:**
- Uses `mb_strlen` if available
- Falls back to regex-based counting

**Usage Context:**
- Validating string length for display or storage
- Implementing character-based algorithms

**Example:**
```php
$length = utf8_strlen("Héllò"); // 5
```

---

#### `utf8_strtoupper`

**Purpose:**
Converts a UTF-8 string to uppercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `string`: Uppercase string

**Inner Mechanisms:**
- Uses `mb_strtoupper` if available
- Falls back to static mapping file for simple case conversion

**Usage Context:**
- Normalizing text for case-insensitive comparisons
- Formatting text for display

**Example:**
```php
$upper = utf8_strtoupper("héllò"); // "HÉLLÒ"
```

---

#### `utf8_strtolower`

**Purpose:**
Converts a UTF-8 string to lowercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `string`: Lowercase string

**Inner Mechanisms:**
- Uses `mb_strtolower` if available
- Falls back to static mapping file for simple case conversion

**Usage Context:**
- Normalizing text for case-insensitive comparisons
- Formatting text for display

**Example:**
```php
$lower = utf8_strtolower("HÉLLÒ"); // "héllò"
```

---

#### `utf8_ucfirst`

**Purpose:**
Capitalizes the first character of a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `string`: String with first character capitalized

**Inner Mechanisms:**
- Uses `utf8_substr` and `utf8_strtoupper` for character manipulation

**Usage Context:**
- Formatting proper nouns or titles
- Capitalizing user input

**Example:**
```php
$text = utf8_ucfirst("héllò"); // "Héllò"
```

---

#### `utf8_ucwords`

**Purpose:**
Capitalizes the first character of each word in a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `string`: String with words capitalized

**Inner Mechanisms:**
- Uses regex to identify word boundaries
- Applies `utf8_ucfirst` to each word

**Usage Context:**
- Formatting titles or headings
- Normalizing user input

**Example:**
```php
$text = utf8_ucwords("héllò wörld"); // "Héllò Wörld"
```

---

#### `utf8_ltrim`, `utf8_rtrim`, `utf8_trim`

**Purpose:**
Removes whitespace and control characters from the left, right, or both ends of a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `string`: Trimmed string

**Inner Mechanisms:**
- Uses regex with Unicode character properties
- Handles various whitespace and control characters

**Usage Context:**
- Cleaning user input
- Normalizing strings for comparison or storage

**Example:**
```php
$text = utf8_trim("  héllò  "); // "héllò"
```

---

#### `utf8_strcasecmp`

**Purpose:**
Performs a case-insensitive comparison of two UTF-8 strings.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | First string |
| `$value2` | `string` | Second string |

**Return Values:**
- `int`: Negative if `$value1` < `$value2`, 0 if equal, positive if `$value1` > `$value2`

**Inner Mechanisms:**
- Converts strings to lowercase with `utf8_strtolower`
- Uses `strcmp` for comparison

**Usage Context:**
- Sorting or searching strings case-insensitively
- Validating user input against known values

**Example:**
```php
$result = utf8_strcasecmp("Héllò", "héllò"); // 0
```

---

#### `utf8_strnatcasecmp`

**Purpose:**
Performs a natural order case-insensitive comparison of two UTF-8 strings.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | First string |
| `$value2` | `string` | Second string |

**Return Values:**
- `int`: Negative if `$value1` < `$value2`, 0 if equal, positive if `$value1` > `$value2`

**Inner Mechanisms:**
- Converts strings to lowercase with `utf8_strtolower`
- Uses `strnatcmp` for natural order comparison

**Usage Context:**
- Sorting strings with numbers in natural order
- Implementing human-friendly sorting algorithms

**Example:**
```php
$result = utf8_strnatcasecmp("file2.txt", "file10.txt"); // -1
```

---

#### `utf8_strspn`

**Purpose:**
Finds the length of the initial segment of a UTF-8 string containing only characters from a mask.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |
| `$mask` | `string` | Allowed characters |
| `$start` | `int|null` | Starting position |
| `$length` | `int|null` | Maximum length to check |

**Return Values:**
- `int`: Length of initial segment

**Inner Mechanisms:**
- Uses `utf8_substr` for position handling
- Uses regex to match allowed characters

**Usage Context:**
- Validating string formats
- Parsing structured text

**Example:**
```php
$length = utf8_strspn("123abc", "1234567890"); // 3
```

---

#### `utf8_wordwrap`

**Purpose:**
Wraps a UTF-8 string to given number of characters per line.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |
| `$length` | `int` | Maximum line length (default: 75) |
| `$break` | `string` | Line break character (default: `"\n"`) |
| `$cut` | `bool` | Whether to cut words (default: `FALSE`) |

**Return Values:**
- `string`: Wrapped string

**Inner Mechanisms:**
- Uses regex to match line break positions
- Handles punctuation and whitespace for natural breaks

**Usage Context:**
- Formatting text for display or email
- Implementing text wrapping for UI elements

**Example:**
```php
$text = "This is a long UTF-8 string that needs wrapping.";
$wrapped = utf8_wordwrap($text, 20, "\n", false);
// "This is a long\nUTF-8 string that\nneeds wrapping."
```

---

#### `utf8_normalize`

**Purpose:**
Normalizes a UTF-8 string to NFC (composed) or NFD (decomposed) form.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |
| `$compose` | `bool` | Whether to compose characters (default: `TRUE`) |

**Return Values:**
- `string`: Normalized string

**Inner Mechanisms:**
- Uses PHP's `Normalizer` class if available
- Falls back to custom implementation using static mapping files
- Handles canonical decomposition and composition

**Usage Context:**
- Ensuring consistent string representation
- Preparing text for comparison or storage

**Example:**
```php
$normalized = utf8_normalize("é"); // "é" in NFC form
$decomposed = utf8_normalize("é", false); // "é" in NFD form
```

---

## `utf8` Class

Static utility class for Unicode data processing and normalization.

---

### Methods

---

#### `utf8::build`

**Purpose:**
Generates static mapping files for UTF-8 processing from Unicode data files.

**Parameters:**
None

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
- Processes Unicode data files to generate:
  - Canonical combining class mappings
  - Decomposition and composition mappings
  - Case conversion mappings
  - Quick check tables for normalization
  - ISO-8859-* to UTF-8 conversion tables
- Handles Hangul syllable composition/decomposition
- Processes composition exclusions

**Usage Context:**
- System setup or maintenance
- Generating required data files for UTF-8 processing

**Example:**
```php
if (utf8::build()) {
    echo "UTF-8 data files generated successfully";
} else {
    echo "Failed to generate UTF-8 data files";
}
```

---

#### `utf8::test`

**Purpose:**
Tests the normalization implementation against the Unicode normalization test file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$limit` | `int` | Maximum number of test lines to process (0 for all) |

**Return Values:**
- `bool`: `FALSE` if all tests pass, `TRUE` if errors found

**Inner Mechanisms:**
- Processes the Unicode NormalizationTest.txt file
- Compares results with expected values
- Reports errors to output

**Usage Context:**
- Verifying normalization implementation
- Debugging or validating UTF-8 processing

**Example:**
```php
if (utf8::test(1000)) {
    echo "Normalization tests failed";
} else {
    echo "All normalization tests passed";
}
```

---

#### `utf8::get_canonical_combining_class`

**Purpose:**
Gets the canonical combining class for a Unicode character.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Unicode character |

**Return Values:**
- `int`: Combining class (0 for starters)

**Inner Mechanisms:**
- Uses static mapping file generated by `build()`

**Usage Context:**
- Implementing Unicode normalization
- Analyzing character properties

**Example:**
```php
$class = utf8::get_canonical_combining_class("́"); // 230 (acute accent)
```

---

#### `utf8::decompose`

**Purpose:**
Decomposes a string into its canonical decomposed form (NFD).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string (passed by reference) |

**Return Values:**
- `string`: Decomposed string

**Inner Mechanisms:**
- Recursively decomposes characters using mapping files
- Handles Hangul syllables

**Usage Context:**
- Implementing Unicode normalization
- Preparing text for comparison

**Example:**
```php
$decomposed = utf8::decompose("é"); // "é"
```

---

#### `utf8::sort`

**Purpose:**
Sorts combining characters in canonical order.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string with combining characters |

**Return Values:**
- `string`: String with combining characters sorted

**Inner Mechanisms:**
- Groups characters by combining class
- Sorts characters within each group

**Usage Context:**
- Implementing Unicode normalization
- Ensuring consistent representation of combining characters

**Example:**
```php
$sorted = utf8::sort("é"); // "é" (acute accent after base character)
```

---

#### `utf8::compose`

**Purpose:**
Composes a string into its canonical composed form (NFC).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |

**Return Values:**
- `string`: Composed string

**Inner Mechanisms:**
- Processes characters in sequence
- Composes valid character sequences using mapping files
- Handles Hangul syllables

**Usage Context:**
- Implementing Unicode normalization
- Preparing text for storage or display

**Example:**
```php
$composed = utf8::compose("é"); // "é"
```

---

#### `utf8::get_character`

**Purpose:**
Extracts a single UTF-8 character from a string at a given byte offset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string |
| `$offset` | `int` | Byte offset (passed by reference) |

**Return Values:**
- `string|false`: UTF-8 character or `FALSE` for invalid sequences

**Inner Mechanisms:**
- Uses regex to match valid UTF-8 sequences
- Updates offset to point after the extracted character

**Usage Context:**
- Implementing character-based string processing
- Analyzing UTF-8 strings at the character level

**Example:**
```php
$string = "Héllò";
$offset = 0;
$char = utf8::get_character($string, $offset); // "H", $offset = 1
$char = utf8::get_character($string, $offset); // "é", $offset = 3
```

---

#### `utf8::ucd_get_record`

**Purpose:**
Reads a record from a Unicode data file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | File handle |

**Return Values:**
- `array|false`: Array of record fields or `FALSE` at end of file

**Inner Mechanisms:**
- Reads lines from Unicode data files
- Strips comments and splits fields
- Skips empty lines

**Usage Context:**
- Processing Unicode data files
- Building mapping tables

**Example:**
```php
$hfile = fopen("UnicodeData.txt", "rb");
while ($record = utf8::ucd_get_record($hfile)) {
    // Process Unicode character record
}
fclose($hfile);
```

---

#### `utf8::ucd_extract_code_point`

**Purpose:**
Extracts Unicode code points from a Unicode data file field.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Field value from Unicode data file |

**Return Values:**
- `array`: Array of UTF-8 characters

**Inner Mechanisms:**
- Handles code point ranges (e.g., "0041..0045")
- Converts hexadecimal code points to UTF-8 characters

**Usage Context:**
- Processing Unicode data files
- Building character mapping tables

**Example:**
```php
$codePoints = utf8::ucd_extract_code_point("0041..0043 0045");
// ["A", "B", "C", "E"]
```


<!-- HASH:a1e3dcb4ab2ee019fd6fcccb0fb8710f -->
