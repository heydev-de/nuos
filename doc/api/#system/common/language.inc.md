# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/language.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/language.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Language Management Functions

This file provides the core internationalization (i18n) functionality for the PWNC Web Platform. It handles multilingual text storage, retrieval, and language detection using a separator-based encoding scheme within single string fields.

### Constants Used

| Constant | Description |
|----------|-------------|
| `CMS_LANGUAGE_SEPARATOR` | Character(s) used to separate language segments in encoded strings |
| `CMS_LANGUAGE` | Current active language code |
| `CMS_LANGUAGE_ENABLED` | Comma-separated list of enabled languages |
| `CMS_REGEX_BORDER` | Regex pattern for word boundaries in multibyte contexts |
| `CMS_PATH` | Base filesystem path for the application |

---

## l

### l($text)

**Purpose**: Shortcut alias for `language_get()` to retrieve translated text.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Text containing language-encoded segments |

**Return Value**: 
- **string**: The appropriate language segment based on current `CMS_LANGUAGE`

**Inner Mechanism**: Simply delegates to `language_get()` with default parameters.

**Usage Example**:
```php
// Assuming CMS_LANGUAGE = 'en' and CMS_LANGUAGE_SEPARATOR = '|'
echo l("Hello|en:Hello|de:Hallo"); // Outputs: "Hello"
```

---

## language_get

### language_get($text, $language = NULL, $explicit = NULL)

**Purpose**: Extracts the appropriate language segment from a separator-encoded string.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Encoded text with language segments |
| `$language` | string\|NULL\|FALSE | Target language code; NULL uses current language |
| `$explicit` | mixed | If TRUE, returns NULL when no match found |

**Return Value**:
- **string**: Matching language segment or default text
- **NULL**: When `$explicit` is TRUE and no match found

**Inner Mechanism**:
1. Parses the default text before the first separator
2. If language is NULL, uses `CMS_LANGUAGE`
3. Searches for the specific language segment using `strpos()`
4. Returns the segment content or falls back to default

**Usage Example**:
```php
// Get German translation if available, otherwise default
$text = "Welcome|en:Welcome|de:Willkommen";
echo language_get($text, 'de'); // Outputs: "Willkommen"
echo language_get($text); // Uses CMS_LANGUAGE
```

---

## language_get_array

### language_get_array($text)

**Purpose**: Converts a separator-encoded string into an associative array of language segments.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Encoded text with language segments |

**Return Value**:
- **array**: Keys are language codes, values are corresponding text segments

**Inner Mechanism**:
1. Splits text by `CMS_LANGUAGE_SEPARATOR`
2. Initializes array with empty key for default text
3. If `CMS_LANGUAGE_ENABLED` is set, pre-populates enabled languages with NULL
4. Parses each segment to extract language key and value

**Usage Example**:
```php
$text = "Hello|en:Hello|de:Hallo|fr:Bonjour";
$array = language_get_array($text);
// Result: ["", "en" => "Hello", "de" => "Hallo", "fr" => "Bonjour"]
```

---

## language_set

### language_set($text, $value = NULL, $language = NULL)

**Purpose**: Sets or updates a specific language segment within an encoded string.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Original encoded text |
| `$value` | string\|NULL | New value for the language segment |
| `$language` | string\|NULL | Target language code |

**Return Value**:
- **string**: Modified encoded text with updated segment

**Inner Mechanism**:
1. Handles setting default text when language is empty
2. Removes separators from value to prevent injection
3. Either replaces existing segment or appends new one
4. Uses `substr_replace()` for precise segment manipulation

**Usage Example**:
```php
$text = "Hello|en:Hello|de:Hallo";
$newText = language_set($text, "Bonjour", "fr");
// Result: "Hello|en:Hello|de:Hallo|fr:Bonjour"
```

---

## language_set_array

### language_set_array($array)

**Purpose**: Converts an associative array of language segments back into a separator-encoded string.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$array` | array | Language codes as keys, text as values |

**Return Value**:
- **string**: Separator-encoded text string

**Inner Mechanism**:
1. Iterates through array elements
2. Skips empty values
3. Removes separators from values
4. Constructs encoded string with proper format

**Usage Example**:
```php
$array = ["en" => "Hello", "de" => "Hallo", "fr" => "Bonjour"];
$encoded = language_set_array($array);
// Result: "|en:Hello|de:Hallo|fr:Bonjour"
```

---

## language_detect

### language_detect($text)

**Purpose**: Detects the language of given text by comparing against stopword lists.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Text to analyze for language detection |

**Return Value**:
- **string**: Detected language code (empty string if none found)

**Inner Mechanism**:
1. Loads language data from `#system/language` dataset
2. Tokenizes and lowercases input text
3. Compares tokens against stopword lists for each language
4. Returns language with highest token match count

**Usage Example**:
```php
$text = "This is a sample English text with common words";
$language = language_detect($text); // Likely returns "en"
```

---

## language_strip_stopword

### language_strip_stopword($text, $language)

**Purpose**: Removes stop words from text for a specific language.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Input text to process |
| `$language` | string | Language code for stopword list |

**Return Value**:
- **string**: Text with stop words removed

**Inner Mechanism**:
1. Retrieves stopword list for specified language
2. Escapes regex special characters in stopwords
3. Builds regex pattern with word boundaries
4. Replaces matched stopwords while preserving surrounding context

**Usage Example**:
```php
$text = "The quick brown fox jumps over the lazy dog";
$clean = language_strip_stopword($text, 'en');
// Result: "quick brown fox jumps over lazy dog"
```

---

## language_name

### language_name($string)

**Purpose**: Retrieves the human-readable name for a language code.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$string` | string | Language code |

**Return Value**:
- **string**: Human-readable language name

**Inner Mechanism**: Queries the `#system/language` dataset for the "name" field of the given language code.

**Usage Example**:
```php
echo language_name('de'); // Outputs: "German"
echo language_name('fr'); // Outputs: "French"
```

---

## t

### t($text)

**Purpose**: Translation function that looks up text in language files and caches results.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$text` | string | Text key to translate |

**Return Value**:
- **string**: Translated text if found, otherwise original text

**Inner Mechanism**:
1. Maintains static cache of translation tables
2. Loads translation files from `#language/` directory
3. Uses `debug_backtrace()` to identify calling script
4. Parses PHP tokens to extract translatable strings
5. Implements file modification time checking for cache invalidation
6. Registers shutdown function to save updated translation templates

**Usage Example**:
```php
// In a script file within CMS_PATH
echo t("Welcome to our website"); 
// Looks up translation in test.{language}.language.inc
// Falls back to original text if no translation found
```


<!-- HASH:8a69fb3b06fb24ad9efb933509b511b4 -->
