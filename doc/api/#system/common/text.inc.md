# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/text.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/text.inc)

- **Version:** `26.9.9.7`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Text Processing Functions

This file contains core text processing utilities for the PWNC Web Platform, including a custom markup parser, HTML-to-plain-text converter, text similarity analyzer, and tokenizer.

### parse_text

Parses custom markup syntax into HTML.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text` | string | — | Input text containing markup |
| `$token` | bool | TRUE | Whether to process tokens |
| `$base_url` | string | CMS_HOST | Base URL for link detection |

**Return:** string — Parsed HTML output

**Mechanism:** Uses a state machine with three modes (default, token, formatting) to process markup like `[+bold+]`, `[/italic/]`, `[url description]`, `[IMG url]`, and `[#table#]`. Maintains a stack for nested formatting and handles escaping with backslashes.

**Usage:**
```php
$html = parse_text("[+Hello+] [world/]");
// Output: <b>Hello</b> <i>world</i>
```

### remove_format

Strips markup formatting from text, converting it to plain text.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text` | string | — | Input text with markup |
| `$token` | bool | TRUE | Whether to process tokens |
| `$discard_links` | bool | FALSE | Whether to discard link URLs |

**Return:** string — Plain text without markup

**Mechanism:** Similar state machine to `parse_text` but outputs plain text instead of HTML. Converts tables to tab-indented rows, links to parenthetical URLs, and images to bracketed placeholders.

**Usage:**
```php
$plain = remove_format("[+Bold text+] and [/italic/]");
// Output: "Bold text and italic"
```

### get_first_image

Extracts the first image URL from markup text.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text` | string | — | Input text with markup |

**Return:** string|null — First image URL or NULL if none found

**Mechanism:** Uses regex to find `[IMG`, `[<-IMG`, or `[IMG->` markers and extracts the URL parameter.

**Usage:**
```php
$url = get_first_image("[IMG image://photo.jpg 200*300]");
// Returns: "image://photo.jpg"
```

### parse_token

Applies token processing to text.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text` | string | — | Input text |

**Return:** string — Text with tokens applied

**Mechanism:** Loads the token library and applies its transformations. Returns original text if token library unavailable.

**Usage:**
```php
$processed = parse_token("Hello %username%");
// Output depends on token definitions
```

### quote_text

Extracts and highlights text passages containing specified keywords.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text` | string | — | Input text to search |
| `$keyword` | string | — | Keywords to find |
| `$minimum_keyword_length` | int | 3 | Minimum keyword length |

**Return:** string — HTML with highlighted keywords and ellipsis separators

**Mechanism:** Tokenizes keywords, finds matching passages within 50-150 character windows, merges overlapping ranges, and wraps matches in `<strong>` tags.

**Usage:**
```php
$excerpt = quote_text("The quick brown fox jumps", "fox");
// Output: "The quick brown <strong>fox</strong> jumps …"
```

### htmltoplain

Converts HTML to plain text with optional formatting preservation.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$string` | string | — | HTML input |
| `$format` | bool | FALSE | Preserve basic formatting |
| `$discard_links` | bool | FALSE | Remove link URLs |
| `$discard_images` | bool | FALSE | Remove image alt text |

**Return:** string — Plain text representation

**Mechanism:** Uses regex patterns to strip HTML tags, convert structural elements to whitespace, decode entities, and optionally preserve line breaks and indentation.

**Usage:**
```php
$plain = htmltoplain("<p>Hello <b>world</b></p>", TRUE);
// Output: "Hello world"
```

### text_similarity

Calculates similarity percentage between two texts.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text1` | string | — | First text |
| `$text2` | string | — | Second text |

**Return:** float — Similarity percentage (0-100)

**Mechanism:** Generates fingerprints (word sets) for both texts, counts total and matching words, and calculates percentage match.

**Usage:**
```php
$similarity = text_similarity("Hello world", "Hello there world");
// Returns: ~66.67
```

### tokenize_text

Splits text into tokens for analysis.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$text` | string | — | Input text |
| `$cleanup_repeats` | bool | FALSE | Collapse repeated characters |

**Return:** array — Array of text tokens

**Mechanism:** Cleans control characters, isolates ideograms (Chinese, Japanese, Korean), optionally collapses repeated characters, and splits on word boundaries.

**Usage:**
```php
$tokens = tokenize_text("Hello, world!");
// Returns: ["Hello", "world"]
```


<!-- HASH:3f4376f011206726e9caba85234e618d -->
