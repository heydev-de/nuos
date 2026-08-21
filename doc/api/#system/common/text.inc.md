# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/text.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/text.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Text Processing Utilities (`text.inc`)

This file provides core text manipulation utilities for the PWNC Web Platform. It includes functions for parsing formatted text, removing formatting, extracting images, token processing, text quoting, HTML-to-plaintext conversion, text similarity comparison, and text tokenization.

---

## `parse_text()`

Parses a custom text format into HTML, supporting inline formatting, links, images, and tables.

### Purpose
Converts a lightweight markup language into HTML. The markup supports:
- **Inline formatting**: bold (`+text+`), italic (`/text/`), underline (`_text_`), etc.
- **Links**: `[url]` or `[description|url]`
- **Images**: `[IMG url]`, `[<-IMG url]` (left-aligned), `[IMG-> url]` (right-aligned)
- **Tables**: `[# |cell1|cell2|]`
- **Tokens**: `%token%` (processed via `parse_token()`)

### Parameters

| Name       | Type    | Default      | Description                                                                 |
|------------|---------|--------------|-----------------------------------------------------------------------------|
| `$text`    | string  | —            | Input text in custom markup format.                                        |
| `$token`   | bool    | `TRUE`       | Whether to process `%token%` placeholders.                                 |
| `$base_url`| string  | `CMS_HOST`   | Base URL for determining internal vs. external links.                      |

### Return Value
- **string**: HTML representation of the parsed text.

### Inner Mechanisms
1. **Base URL Analysis**: Uses `analyze_url()` to determine the platform's host for link classification.
2. **State Machine**: Processes text character-by-character using modes (`0`: normal, `1`: token, `2`: formatting).
3. **Stack-Based Formatting**: Uses a stack to handle nested formatting (e.g., bold inside italic).
4. **Whitespace Handling**: Preserves and converts whitespace (spaces → `&nbsp;`, newlines → `<br>`).
5. **Table Parsing**: Supports nested tables with `|` as cell/row delimiters.
6. **Image Handling**: Delegates to the `image()` function (from `image` module) for rendering.

### Usage Example
```php
$markup = "Hello [+world+]! [https://pwnc.it PWNC]";
$html = parse_text($markup);
// Output: Hello <b>world</b>! <a href="https://pwnc.it">PWNC</a>
```

---

## `remove_format()`

Strips custom formatting from text, converting it to plaintext.

### Purpose
Reverts `parse_text()` output back to a plaintext representation, optionally preserving or discarding links and tokens.

### Parameters

| Name              | Type    | Default | Description                                                                 |
|-------------------|---------|---------|-----------------------------------------------------------------------------|
| `$text`           | string  | —       | Input text (formatted or HTML).                                             |
| `$token`          | bool    | `TRUE`  | Whether to process `%token%` placeholders.                                 |
| `$discard_links`  | bool    | `FALSE` | If `TRUE`, discards link URLs (keeps link text only).                      |

### Return Value
- **string**: Plaintext version of the input.

### Inner Mechanisms
1. **State Machine**: Mirrors `parse_text()` but outputs plaintext.
2. **Link Handling**: Preserves link text and appends URLs in parentheses (unless `$discard_links` is `TRUE`).
3. **Table Conversion**: Converts tables to tab-separated values with newlines for rows.
4. **Image Handling**: Discards image URLs (retains alt text if available).

### Usage Example
```php
$formatted = "Check [+this+] [https://pwnc.it link]!";
$plain = remove_format($formatted);
// Output: Check this link (https://pwnc.it)!
```

---

## `get_first_image()`

Extracts the first image URL from formatted text.

### Purpose
Retrieves the URL of the first image in a text block (e.g., for previews or thumbnails).

### Parameters

| Name   | Type   | Default | Description               |
|--------|--------|---------|---------------------------|
| `$text`| string | —       | Input text with `[IMG]` tags. |

### Return Value
- **string|null**: URL of the first image, or `NULL` if no image is found.

### Inner Mechanisms
- Uses a regex to match `[IMG]`, `[<-IMG]`, or `[IMG->]` tags and capture the URL.

### Usage Example
```php
$text = "Header [IMG image://logo.png] Content";
$url = get_first_image($text);
// Output: "image://logo.png"
```

---

## `parse_token()`

Processes `%token%` placeholders in text using the `token` module.

### Purpose
Replaces dynamic tokens (e.g., `%username%`) with their resolved values.

### Parameters

| Name   | Type   | Default | Description               |
|--------|--------|---------|---------------------------|
| `$text`| string | —       | Input text with tokens.   |

### Return Value
- **string**: Text with tokens replaced by their values.

### Inner Mechanisms
1. **Lazy Loading**: Loads the `token` module only when first needed.
2. **Delegation**: Uses the `token` class's `apply()` method to resolve tokens.

### Usage Example
```php
$text = "Hello, %username%!";
$resolved = parse_token($text);
// Output: "Hello, Admin!" (if %username% resolves to "Admin")
```

---

## `quote_text()`

Extracts and highlights relevant passages from text based on keywords.

### Purpose
Generates a snippet of text with keywords highlighted, useful for search results or previews.

### Parameters

| Name                      | Type    | Default | Description                                                                 |
|---------------------------|---------|---------|-----------------------------------------------------------------------------|
| `$text`                   | string  | —       | Input text to search.                                                       |
| `$keyword`                | string  | —       | Keyword(s) to match (space-separated).                                     |
| `$minimum_keyword_length` | int     | `3`     | Minimum length of keywords to consider (shorter keywords are ignored).     |

### Return Value
- **string**: Snippet with keywords wrapped in `<strong>` tags, or an empty string if no matches are found.

### Inner Mechanisms
1. **Keyword Processing**: Splits keywords, removes duplicates, and sorts by length (longest first).
2. **Pattern Matching**: Uses regex to find keyword matches in the text, including partial matches.
3. **Range Merging**: Combines overlapping or adjacent matches into contiguous snippets.
4. **Highlighting**: Wraps matched keywords in `<strong>` tags.

### Usage Example
```php
$text = "The quick brown fox jumps over the lazy dog.";
$snippet = quote_text($text, "fox jumps");
// Output: "… quick brown <strong>fox</strong> <strong>jumps</strong> over the …"
```

---

## `htmltoplain()`

Converts HTML to plaintext, optionally preserving formatting (e.g., line breaks, lists).

### Purpose
Strips HTML tags and converts entities to plaintext, with options to retain structural formatting.

### Parameters

| Name               | Type    | Default | Description                                                                 |
|--------------------|---------|---------|-----------------------------------------------------------------------------|
| `$string`          | string  | —       | Input HTML string.                                                          |
| `$format`          | bool    | `FALSE` | If `TRUE`, preserves line breaks, lists, and tables.                        |
| `$discard_links`   | bool    | `FALSE` | If `TRUE`, discards link URLs (keeps link text only).                      |
| `$discard_images`  | bool    | `FALSE` | If `TRUE`, discards images (keeps alt text if available).                  |

### Return Value
- **string**: Plaintext version of the input.

### Inner Mechanisms
1. **Cleanup**: Removes control characters, comments, scripts, and styles.
2. **Tag Conversion**:
   - `<br>` → newline.
   - Block elements (`<div>`, `<p>`, etc.) → double newline.
   - Lists → `*` for items.
   - Links → `text <url>` (unless `$discard_links` is `TRUE`).
   - Images → `[alt text]` (unless `$discard_images` is `TRUE`).
3. **Entity Decoding**: Converts HTML entities to UTF-8 characters.
4. **Whitespace Normalization**: Trims and conflates whitespace.

### Usage Example
```php
$html = "<p>Hello <a href='https://pwnc.it'>PWNC</a>!</p>";
$plain = htmltoplain($html, TRUE);
// Output: "Hello PWNC <https://pwnc.it>!"
```

---

## `text_similarity()`

Calculates the similarity between two texts as a percentage.

### Purpose
Compares two texts and returns a similarity score (0–100%), useful for deduplication or plagiarism detection.

### Parameters

| Name    | Type   | Default | Description               |
|---------|--------|---------|---------------------------|
| `$text1`| string | —       | First text to compare.    |
| `$text2`| string | —       | Second text to compare.   |

### Return Value
- **int**: Similarity percentage (0 = no similarity, 100 = identical).

### Inner Mechanisms
1. **Fingerprinting**: Uses the `fingerprint()` function to generate token sets for each text.
2. **Intersection**: Counts matching tokens between the two sets.
3. **Scoring**: Calculates similarity as `(matches / total_tokens) * 100`.

### Usage Example
```php
$text1 = "The quick brown fox";
$text2 = "A quick brown dog";
$score = text_similarity($text1, $text2);
// Output: ~66 (2/3 tokens match)
```

---

## `tokenize_text()`

Splits text into an array of tokens (words, ideograms, etc.).

### Purpose
Prepares text for indexing, search, or fingerprinting by breaking it into meaningful units.

### Parameters

| Name                 | Type    | Default | Description                                                                 |
|----------------------|---------|---------|-----------------------------------------------------------------------------|
| `$text`              | string  | —       | Input text to tokenize.                                                     |
| `$cleanup_repeats`   | bool    | `FALSE` | If `TRUE`, reduces repeated characters (e.g., "loooong" → "loong").         |

### Return Value
- **array**: List of tokens.

### Inner Mechanisms
1. **Cleanup**: Removes control characters, normalizes whitespace, and isolates ideograms (e.g., Chinese characters).
2. **Repeat Reduction**: Optionally reduces repeated characters (e.g., "!!!" → "!!").
3. **Splitting**: Uses `CMS_REGEX_BORDER` to split text into tokens (words, numbers, ideograms).

### Usage Example
```php
$text = "Hello, 世界!";
$tokens = tokenize_text($text);
// Output: ["Hello", "世界"]
```


<!-- HASH:e0970628e01895735f93adceded63d63 -->
