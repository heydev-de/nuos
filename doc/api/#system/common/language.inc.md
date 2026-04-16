# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/language.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Language Management Utilities

This file provides core language management utilities for the NUOS platform. It handles multilingual text storage, retrieval, and processing using a compact string-based format where translations are embedded within the same string, separated by a configurable delimiter (`CMS_LANGUAGE_SEPARATOR`). This approach enables efficient storage and retrieval of translations without requiring separate database fields for each language.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_LANGUAGE_SEPARATOR` | `//` (configurable) | Delimiter used to separate language segments within a multilingual string. |
| `CMS_LANGUAGE` | Current active language (e.g., `en`, `de`) | Default language code used when no language is specified. |
| `CMS_LANGUAGE_ENABLED` | Comma-separated list (e.g., `en,de,fr`) | List of enabled languages for the system. |
| `CMS_REGEX_BORDER` | `\b` | Regex word boundary used in stopword processing. |

---

### Functions

---

#### `l($text)`

**Purpose:**
Shortcut function for `language_get()`. Retrieves the active translation of a multilingual string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Multilingual string in NUOS format (e.g., `"Hello//de:Hallo//fr:Bonjour"`). |

**Return Values:**
- `string`: The translation in the current active language, or the default (first) segment if no translation exists.

**Inner Mechanisms:**
- Delegates to `language_get($text)` without additional parameters, using the system’s current language context.

**Usage Context:**
- Used throughout the codebase for quick, context-aware translation of UI strings, labels, and messages.
- Ideal for inline use in templates, forms, and API responses.

---

#### `language_get($text, $language = NULL, $explicit = NULL)`

**Purpose:**
Extracts a specific language translation from a multilingual string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Multilingual string in NUOS format. |
| `$language` | `string\|NULL\|FALSE` | Language code (e.g., `en`, `de`). If `NULL`, uses `CMS_LANGUAGE`. If `FALSE`, forces return of default segment. |
| `$explicit` | `bool\|NULL` | If `TRUE`, returns `NULL` when no translation exists; otherwise, falls back to default. |

**Return Values:**
- `string\|NULL`: The requested translation, the default segment, or `NULL` if `$explicit` is `TRUE` and no translation exists.

**Inner Mechanisms:**
1. Extracts the default segment (first part before any separator).
2. If `$language` is `FALSE` or `$explicit` is set, returns early.
3. Searches for a segment matching `$language:` after the separator.
4. If found, extracts the value; otherwise, returns default or `NULL` based on `$explicit`.

**Usage Context:**
- Core function for runtime language resolution.
- Used in templates, API responses, and dynamic content generation.
- Enables language switching without database schema changes.

---

#### `language_get_array($text)`

**Purpose:**
Converts a multilingual string into an associative array of all available translations.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Multilingual string in NUOS format. |

**Return Values:**
- `array`: Associative array where keys are language codes and values are translations. The default segment is stored under `""`.

**Inner Mechanisms:**
1. Splits the string by `CMS_LANGUAGE_SEPARATOR`.
2. Initializes the return array with the default segment (`""` key).
3. If `CMS_LANGUAGE_ENABLED` is set, pre-populates the array with `NULL` values for each enabled language.
4. Parses each segment for `language:value` pairs and populates the array.

**Usage Context:**
- Used in admin interfaces to display all translations of a field.
- Enables bulk editing of multilingual content.
- Supports export/import of multilingual data.

---

#### `language_set($text, $value = NULL, $language = NULL)`

**Purpose:**
Updates or adds a translation within a multilingual string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Original multilingual string. |
| `$value` | `string\|NULL` | New translation value. If `NULL`, removes the language segment. |
| `$language` | `string\|NULL` | Language code to update. If `NULL`, replaces the default segment. |

**Return Values:**
- `string`: Updated multilingual string.

**Inner Mechanisms:**
1. Extracts the default segment.
2. If `$language` is `NULL`, replaces the entire default segment.
3. If `$value` is empty, removes the language segment.
4. Searches for an existing segment for the language.
5. If found, replaces it; otherwise, appends the new segment.

**Usage Context:**
- Used in content management interfaces to save translations.
- Enables dynamic updates to multilingual fields in forms.
- Supports both creation and modification of translations.

---

#### `language_set_array($array)`

**Purpose:**
Converts an associative array of translations into a NUOS multilingual string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Associative array where keys are language codes and values are translations. |

**Return Values:**
- `string`: Multilingual string in NUOS format.

**Inner Mechanisms:**
1. Iterates over the input array.
2. Skips empty values.
3. For empty keys, treats as default segment.
4. For non-empty keys, formats as `//language:value`.
5. Joins all segments into a single string.

**Usage Context:**
- Used when importing multilingual data from external sources.
- Converts form submissions into storable format.
- Enables bulk creation of multilingual strings.

---

#### `language_detect($text)`

**Purpose:**
Detects the most likely language of a given text using stopword analysis.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text to analyze. |

**Return Values:**
- `string`: Language code (e.g., `en`, `de`) of the best match, or empty string if no match.

**Inner Mechanisms:**
1. Loads stopword data from `#system/language` data file.
2. Tokenizes and lowercases the input text.
3. Compares tokens against stopwords for each language.
4. Returns the language with the highest number of matching stopwords.

**Usage Context:**
- Used in content ingestion (e.g., user-generated content, imports).
- Enables automatic language tagging.
- Supports multilingual search and indexing.

---

#### `language_strip_stopword($text, $language)`

**Purpose:**
Removes stopwords from a text in a specific language.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text to process. |
| `$language` | `string` | Language code for stopword list. |

**Return Values:**
- `string`: Text with stopwords removed.

**Inner Mechanisms:**
1. Loads stopword list for the specified language.
2. Escapes stopwords for regex.
3. Uses regex with word boundaries to remove stopwords.

**Usage Context:**
- Used in search indexing and full-text processing.
- Improves relevance by filtering out common words.
- Supports multilingual search optimization.

---

#### `language_name($string)`

**Purpose:**
Retrieves the human-readable name of a language.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Language code (e.g., `en`, `de`). |

**Return Values:**
- `string`: Human-readable name (e.g., `English`, `Deutsch`).

**Inner Mechanisms:**
- Uses the `#system/language` data file to look up the `name` field for the given language code.

**Usage Context:**
- Used in UI dropdowns, language selectors, and reports.
- Provides user-friendly labels for language codes.


<!-- HASH:34edef9a87a52130f9adfd757d8bd6da -->
