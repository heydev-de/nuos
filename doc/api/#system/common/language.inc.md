# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/language.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/language.inc)

- **Version:** `26.9.22.5`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Language Management

This file provides the core internationalization (i18n) and localization (l10n) utilities for the PWNC Web Platform. It handles multilingual content storage, retrieval, detection, and translation through a compact string-based format using `CMS_LANGUAGE_SEPARATOR` as a delimiter.

The primary data format is a delimited string where the default value comes first, followed by language-specific segments in the form `§language:value`. For example:

```
Hello§en:Hello§de:Hallo§fr:Bonjour
```

### Constants

| Name | Description |
|------|-------------|
| `CMS_LANGUAGE_SEPARATOR` | Delimiter character(s) separating language segments in stored strings |
| `CMS_LANGUAGE` | The currently active language code (e.g., `"en"`, `"de"`) |
| `CMS_LANGUAGE_ENABLED` | Comma-separated list of enabled language codes |
| `CMS_REGEX_BORDER` | Regex pattern for word boundaries used in stopword matching |
| `CMS_PATH` | Root filesystem path of the application |

### Helper Functions (Referenced)

| Function | Description |
|----------|-------------|
| `blank($v)` | Returns `TRUE` if `$v` is blank (empty or whitespace-only) |
| `stre($v)` | Returns `TRUE` if `$v` is empty |
| `nstre($v)` | Returns `TRUE` if `$v` is not empty |
| `utf8_strtolower($s)` | Multibyte-safe lowercase conversion |
| `tokenize_text($s)` | Splits text into tokens (words) for analysis |
| `read_file($f)` | Reads a file's contents |
| `write_file($f, $c)` | Writes content to a file |

---

## l

### l($text)

**Purpose:** Convenience alias for `language_get()`. Provides a short, memorable function name for retrieving the current language's translation of a text.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The delimited multilingual string |

**Return Value:** `string` — The translated text for the current language, or the default if no translation exists.

**Inner Mechanism:** Simply delegates to `language_get($text)` with default parameters, using the global `CMS_LANGUAGE` constant.

**Usage Example:**

```php
// Given CMS_LANGUAGE = "de" and CMS_LANGUAGE_SEPARATOR = "§"
$label = l("Save§en:Save§de:Speichern§fr:Sauvegarder");
// Returns: "Speichern"
```

---

## language_get

### language_get($text, $language = NULL, $explicit = NULL)

**Purpose:** Extracts the appropriate language-specific value from a delimited multilingual string. This is the core function for resolving which translation to display.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | — | The delimited multilingual string (e.g., `"Hello§en:Hello§de:Hallo"`) |
| `$language` | `string\|NULL\|FALSE` | `NULL` | Language code to retrieve. `NULL` uses `CMS_LANGUAGE`. `FALSE` returns only the default. |
| `$explicit` | `bool\|NULL` | `NULL` | If `TRUE`, returns `NULL` when no translation is found instead of falling back to default. |

**Return Value:** `string\|NULL` — The translated text, the default text, or `NULL` (when `$explicit` is `TRUE` and no match is found).

**Inner Mechanism:**

1. Casts `$text` to string and finds the position of the first `CMS_LANGUAGE_SEPARATOR` to isolate the default value.
2. If `$language` is blank:
   - If `$language === FALSE` or `$explicit` is truthy, returns the default immediately.
   - Otherwise, sets `$language` to the global `CMS_LANGUAGE`.
3. Searches for `CMS_LANGUAGE_SEPARATOR . "$language:"` starting after the default segment.
4. If not found, returns `NULL` (if `$explicit`) or the default.
5. Extracts the value between the language marker and the next separator (or end of string).
6. Returns the extracted value if non-empty, otherwise falls back to the default.

**Usage Example:**

```php
// CMS_LANGUAGE = "fr", CMS_LANGUAGE_SEPARATOR = "§"
$text = "Bonjour§en:Hello§de:Hallo§fr:Bonjour";

language_get($text);           // Returns: "Bonjour" (current language)
language_get($text, "de");     // Returns: "Hallo" (explicit language)
language_get($text, FALSE);    // Returns: "Bonjour" (default only)
language_get($text, "es", TRUE); // Returns: NULL (no Spanish translation, explicit mode)
```

---

## language_get_array

### language_get_array($text)

**Purpose:** Parses a delimited multilingual string into an associative array mapping language codes to their respective values.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The delimited multilingual string |

**Return Value:** `array` — An associative array with:
- Key `""` (empty string) → the default value
- Keys for each enabled language (from `CMS_LANGUAGE_ENABLED`) → their values or `NULL` if not present

**Inner Mechanism:**

1. Splits the text by `CMS_LANGUAGE_SEPARATOR` into segments.
2. Initializes the return array with `"" => $array[0]` (the default value).
3. If `CMS_LANGUAGE_ENABLED` is set, pre-populates keys for each enabled language with `NULL`.
4. Iterates over remaining segments (index 1+), splitting each on `:` to extract the language key and value.
5. Populates the return array with found language-value pairs.

**Usage Example:**

```php
// CMS_LANGUAGE_ENABLED = "en,de,fr"
// CMS_LANGUAGE_SEPARATOR = "§"
$text = "Hello§en:Hello§de:Hallo§fr:Bonjour";

$result = language_get_array($text);
// Returns:
// [
//     ""   => "Hello",
//     "en" => "Hello",
//     "de" => "Hallo",
//     "fr" => "Bonjour"
// ]
```

---

## language_set

### language_set($text, $value = NULL, $language = NULL)

**Purpose:** Sets or updates a language-specific value within a delimited multilingual string. Can also replace the default value.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | — | The original delimited multilingual string |
| `$value` | `string\|NULL` | `NULL` | The new value to set. `NULL` clears the value. |
| `$language` | `string\|NULL` | `NULL` | Language code to set. `NULL` or empty replaces the default value. |

**Return Value:** `string` — The modified multilingual string.

**Inner Mechanism:**

1. Casts `$text` to string and isolates the default value (before first separator).
2. Strips any `CMS_LANGUAGE_SEPARATOR` characters from `$value` to prevent injection.
3. If `$language` is empty/stre, replaces the default value portion using `substr_replace`.
4. If `$value` is not empty, prepends `CMS_LANGUAGE_SEPARATOR . "$language:"` to form the segment.
5. Searches for an existing `CMS_LANGUAGE_SEPARATOR . "$language:"` segment:
   - If not found, appends the new segment to the end.
   - If found, replaces the existing segment (up to the next separator or end of string).

**Usage Example:**

```php
// CMS_LANGUAGE_SEPARATOR = "§"
$text = "Hello§en:Hello§de:Hallo";

language_set($text, "Hola", "es");
// Returns: "Hello§en:Hello§de:Hallo§es:Hola"

language_set($text, "Bonjour", "fr");
// Returns: "Hello§en:Hello§de:Hallo§fr:Bonjour"

language_set($text, "Greetings", NULL);
// Returns: "Greetings§en:Hello§de:Hallo" (default replaced)

language_set($text, "Aloha", "en");
// Returns: "Hello§en:Aloha§de:Hallo" (existing en segment updated)
```

---

## language_set_array

### language_set_array($array)

**Purpose:** Converts an associative array of language-value pairs back into a delimited multilingual string. This is the inverse of `language_get_array()`.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Associative array where keys are language codes (or empty string for default) and values are the corresponding text |

**Return Value:** `string` — A delimited multilingual string.

**Inner Mechanism:**

1. Iterates over each key-value pair in the input array.
2. Skips entries where the value is empty (`stre($value)`).
3. Strips `CMS_LANGUAGE_SEPARATOR` from values to prevent injection.
4. For entries with an empty key, appends the value directly (as the default).
5. For entries with a non-empty key, appends `CMS_LANGUAGE_SEPARATOR . "$key:$value"`.
6. Joins all segments into a single string.

**Usage Example:**

```php
// CMS_LANGUAGE_SEPARATOR = "§"
$array = [
    ""   => "Hello",
    "en" => "Hello",
    "de" => "Hallo",
    "fr" => "Bonjour"
];

$result = language_set_array($array);
// Returns: "Hello§en:Hello§de:Hallo§fr:Bonjour"
```

---

## language_detect

### language_detect($text)

**Purpose:** Detects the most likely language of a given text by comparing its word tokens against stopword lists stored in the system data.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The text to analyze |

**Return Value:** `string` — The detected language code, or an empty string if no match is found.

**Inner Mechanism:**

1. Loads the `#system/language` data source.
2. Tokenizes the input text (lowercased) into word tokens.
3. Iterates over all language entries in the data source.
4. For each language, retrieves its stopword list, splits it into individual words, and counts how many stopwords appear in the input text (using `array_intersect`).
5. Tracks the language with the highest match count.
6. Returns the language code with the most matches.

**Usage Example:**

```php
// Assuming #system/language data contains stopword lists for "en", "de", "fr"
$text = "The quick brown fox jumps over the lazy dog";

$language = language_detect($text);
// Returns: "en" (English stopwords like "the", "over", "the" match most frequently)
```

---

## language_strip_stopword

### language_strip_stopword($text, $language)

**Purpose:** Removes all stopwords for a specified language from the given text. Useful for text analysis, search indexing, or normalization.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The input text |
| `$language` | `string` | The language code whose stopwords should be removed |

**Return Value:** `string` — The text with all stopwords removed.

**Inner Mechanism:**

1. Loads the `#system/language` data source.
2. Retrieves the stopword list for the specified language.
3. Splits the stopword list into individual words.
4. Escapes each stopword for use in a regex pattern using `preg_quote`.
5. Constructs a regex pattern that matches any stopword at word boundaries (using `CMS_REGEX_BORDER`).
6. Performs a case-insensitive, Unicode-aware regex replacement, removing matched stopwords while preserving surrounding text.

**Usage Example:**

```php
// Assuming "en" stopwords include "the", "a", "an", "is", "are"
$text = "The cat is on the mat";

$result = language_strip_stopword($text, "en");
// Returns: "cat on mat"
```

---

## language_name

### language_name($string)

**Purpose:** Retrieves the human-readable name of a language given its code.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The language code (e.g., `"en"`, `"de"`) |

**Return Value:** `string\|NULL` — The language name (e.g., `"English"`, `"German"`), or `NULL` if not found.

**Inner Mechanism:** Instantiates a `data` object for `#system/language` and calls `get($string, "name")` to retrieve the name field for the given language code.

**Usage Example:**

```php
$name = language_name("de");
// Returns: "German" (or whatever is stored in the system data)
```

---

## t

### t($text)

**Purpose:** Translation function that provides a developer-friendly way to mark strings for translation. It scans the calling source file for `t("...")` calls, builds an index of translatable strings, and returns the appropriate translation from a language-specific translation file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | The source string to translate |

**Return Value:** `string` — The translated string if a translation exists, otherwise the original `$text`.

**Inner Mechanism:**

1. Uses static variables (`$table`, `$check`, `$flag`, `$length`) to cache state across calls.
2. Determines the calling script's file path via `debug_backtrace`.
3. If the file is outside `CMS_PATH`, returns `$text` unchanged (not an application file).
4. On first call, loads the translation index from `#language/test.language.inc` (a template file).
5. If `CMS_LANGUAGE` is set, merges translations from `#language/test.{language}.language.inc`.
6. For each unique source file (keyed by relative path), checks if the file has been modified since the last scan:
   - If modified, re-tokenizes the file using PHP's `token_get_all` to find all `t("...")` calls.
   - Extracts string literals from these calls and updates the index.
7. Registers a shutdown function to write the updated index back to the template file.
8. Looks up `$text` in the translation table for the current file and returns the translation if non-empty, otherwise returns the original text.

**Usage Example:**

```php
// In a source file at /app/pages/index.php
echo t("Welcome to our website");
// If a translation exists for CMS_LANGUAGE, returns the translated string
// Otherwise returns: "Welcome to our website"

// The function automatically scans the file for all t("...") calls
// and builds/updates a translation index at #language/test.language.inc
```

**Translation File Structure:**

The index file (`test.language.inc`) has this format:

```php
<?php
return [
    'pages/index.php' => [
        '#time' => 1234567890,
        'Welcome to our website' => NULL,
        'Click here' => NULL,
    ],
];
```

Language-specific translation files (`test.{lang}.language.inc`) override `NULL` values with actual translations:

```php
<?php
return [
    'pages/index.php' => [
        'Welcome to our website' => 'Willkommen auf unserer Website',
        'Click here' => 'Klicken Sie hier',
    ],
];
```


<!-- HASH:2f54eab05556a88f1ce079dab96501a0 -->
