# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/language.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/language.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Language Management Utilities

This file provides core language management functionality for the PWNC Web Platform. It handles multilingual text storage, retrieval, and translation within the system. The utilities allow for:

- Storing and retrieving language-specific strings in a compact format
- Detecting language from text content
- Managing stopwords for language processing
- Future-proof translation system with automatic source scanning

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_LANGUAGE_SEPARATOR` | `␟` (ASCII 31) | Special character used to separate language variants in stored strings |
| `CMS_LANGUAGE` | System default | Current active language code |
| `CMS_LANGUAGE_ENABLED` | Comma-separated list | Enabled language codes in the system |
| `CMS_REGEX_BORDER` | `\b` | Word boundary regex pattern |

---

### `l()`

**Alias for `language_get()`**

Provides shorthand access to the language translation system.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to translate, potentially containing language variants |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Translated text for current language or default if not available |

#### Usage Example

```php
// Simple translation lookup
echo l("Hello␟de:Hallo␟fr:Bonjour");
// Outputs "Hallo" if current language is German
```

---

### `language_get()`

Retrieves language-specific text from a multilingual string.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | - | Multilingual string with language variants |
| `$language` | `string\|null` | `NULL` | Target language code. `NULL` uses system default, `FALSE` forces default text |
| `$explicit` | `bool\|null` | `NULL` | If `TRUE`, returns `NULL` when translation not found instead of default |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|null` | Translated text or `NULL` if `$explicit=TRUE` and translation not found |

#### Inner Mechanisms

1. Extracts default text (before first separator)
2. If no language specified, uses system default
3. Searches for language-specific variant in format `␟{lang}:{text}`
4. Returns default if variant not found (unless `$explicit=TRUE`)

#### Usage Example

```php
// Complex translation with fallback
$greeting = "Welcome␟de:Willkommen␟fr:Bienvenue";
echo language_get($greeting, "es", TRUE); // NULL (Spanish not available)
echo language_get($greeting, "de");       // "Willkommen"
echo language_get($greeting);             // "Welcome" (system default)
```

---

### `language_get_array()`

Converts a multilingual string into an associative array of language variants.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Multilingual string with language variants |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array with language codes as keys and translations as values |

#### Inner Mechanisms

1. Splits string by separator character
2. First element becomes default (`""` key)
3. Processes remaining elements as `␟{lang}:{text}` pairs
4. Initializes array with all enabled languages (value `NULL` if not provided)

#### Usage Example

```php
// Get all translations for processing
$translations = language_get_array("Submit␟de:Senden␟fr:Envoyer");
/*
Returns:
[
    "" => "Submit",
    "de" => "Senden",
    "fr" => "Envoyer"
]
*/
```

---

### `language_set()`

Sets or updates a language variant in a multilingual string.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | - | Original multilingual string |
| `$value` | `string\|null` | `NULL` | New text for the language variant |
| `$language` | `string\|null` | `NULL` | Target language code. `NULL` replaces default text |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Updated multilingual string with new variant |

#### Inner Mechanisms

1. If no language specified, replaces default text
2. If language specified:
   - Removes existing variant for that language
   - Appends new variant in format `␟{lang}:{text}`
3. Handles edge cases (empty values, separator characters)

#### Usage Example

```php
// Update existing translation
$greeting = "Hello␟de:Hallo";
$updated = language_set($greeting, "Hi", "en"); // Add English variant
// Result: "Hello␟de:Hallo␟en:Hi"

// Replace default text
$updated = language_set($greeting, "Greetings");
// Result: "Greetings␟de:Hallo"
```

---

### `language_set_array()`

Converts an associative array of language variants into a multilingual string.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Associative array with language codes as keys and translations as values |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Multilingual string with all variants |

#### Inner Mechanisms

1. Processes each key-value pair:
   - Empty keys become default text
   - Non-empty keys become language variants
2. Skips empty values
3. Removes separator characters from values
4. Joins all parts into final string

#### Usage Example

```php
// Create multilingual string from array
$translations = [
    "" => "Color",
    "de" => "Farbe",
    "fr" => "Couleur"
];
$multilingual = language_set_array($translations);
// Result: "Color␟de:Farbe␟fr:Couleur"
```

---

### `language_detect()`

Detects the most likely language of a text by comparing against stopwords.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to analyze for language detection |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Detected language code or empty string if no match |

#### Inner Mechanisms

1. Loads stopword data from `#system/language` data file
2. Tokenizes input text (lowercase, split into words)
3. Compares against each language's stopwords
4. Returns language with most matches

#### Usage Example

```php
// Detect language of user input
$userText = "Le chat noir est sur la table";
$language = language_detect($userText); // Returns "fr"
```

---

### `language_strip_stopword()`

Removes stopwords from text for a specific language.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to process |
| `$language` | `string` | Target language code |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Text with stopwords removed |

#### Inner Mechanisms

1. Loads stopwords for specified language
2. Escapes each stopword for regex
3. Builds regex pattern matching whole words only
4. Replaces matches with empty string

#### Usage Example

```php
// Clean text for search indexing
$frenchText = "Le chat noir est sur la table";
$cleaned = language_strip_stopword($frenchText, "fr");
// Result: "chat noir table"
```

---

### `language_name()`

Retrieves the human-readable name of a language.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | Language code |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Human-readable language name or empty string if not found |

#### Inner Mechanisms

1. Loads language data from `#system/language`
2. Retrieves the "name" field for the specified language code

#### Usage Example

```php
// Display language selector
$languages = ["en" => "English", "de" => "Deutsch", "fr" => "Français"];
foreach ($languages as $code => $name) {
    echo "<option value='$code'>" . language_name($code) . "</option>";
}
```

---

### `t()`

**Future Translation System**

Automatically extracts translatable strings from source code and manages translations.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to translate (typically a string literal from source) |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Translated text or original if no translation available |

#### Inner Mechanisms

1. **Source Scanning**:
   - Tracks file modification times
   - Scans PHP files for `t("...")` calls
   - Extracts strings for translation

2. **Translation Management**:
   - Maintains index file (`test.language.inc`)
   - Merges with language-specific translation files
   - Schedules file updates on shutdown

3. **Caching**:
   - Static cache of translations
   - Only processes files that have changed

#### Usage Context

- Designed for future implementation
- Currently returns original text
- Will automatically populate translation files from source code

#### Usage Example

```php
// In source code (will be automatically extracted)
echo t("Welcome to our application");

// In translation file (test.de.language.inc)
return [
    'path/to/file.php' => [
        'Welcome to our application' => 'Willkommen in unserer Anwendung'
    ]
];
```


<!-- HASH:8a69fb3b06fb24ad9efb933509b511b4 -->
