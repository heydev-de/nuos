# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/text.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Text Processing Utilities (`text.inc`)

This file provides core text processing utilities for the NUOS platform, enabling:
- **Text formatting** with a lightweight markup syntax (e.g., `[b]bold[/b]` → `<b>bold</b>`)
- **HTML-to-plaintext conversion** with configurable formatting retention
- **Text analysis** (keyword extraction, similarity comparison, tokenization)
- **Image extraction** from formatted text

---

## `parse_text()`

Converts NUOS markup into HTML while preserving tokens, links, and structural formatting.

### Parameters

| Name       | Type     | Default       | Description                                                                 |
|------------|----------|---------------|-----------------------------------------------------------------------------|
| `$text`    | `string` | -             | Input text containing NUOS markup.                                          |
| `$token`   | `bool`   | `TRUE`        | If `TRUE`, processes tokens (e.g., `%token%`).                              |
| `$base_url`| `string` | `CMS_HOST`    | Base URL for resolving relative links. Falls back to `CMS_HOST` if invalid. |

### Return Value
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | HTML-formatted text with resolved links, images, and applied formatting.   |

### Inner Mechanisms
1. **Markup Parsing**:
   - Uses a state machine (`$mode`) to track parsing context (default, token, or formatting).
   - Supports nested formatting (e.g., `[b][i]text[/i][/b]`).
   - Handles escaped characters (e.g., `\[` → `[`).

2. **Formatting Rules**:
   - **Inline**: `+bold+`, `/italic/`, `_underline_`, `<larger>`, `>smaller<`.
   - **Block**: `<-left-aligned->`, `<->centered<->`, `->right-aligned->`, `*heading*`.
   - **Tables**: `#table#` with `|` as cell/row delimiters.
   - **Images**: `[IMG url]`, `[<-IMG url]` (left-aligned), `[IMG-> url]` (right-aligned).

3. **Link Handling**:
   - Resolves logical URLs (e.g., `image://id` → `/path/to/image`).
   - Adds `rel="nofollow"` and `onclick` handlers for external links.
   - Preserves directory entry descriptions as `title` attributes.

4. **Token Processing**:
   - Delegates to `parse_token()` if `$token=TRUE`.

### Usage Context
- **Content Rendering**: Convert user-generated markup (e.g., forum posts, articles) to HTML.
- **Email Templates**: Format plaintext emails with lightweight markup.
- **Dynamic Content**: Render database-stored text with embedded images/links.

---

## `remove_format()`

Strips NUOS markup from text, optionally preserving or discarding links and tokens.

### Parameters

| Name              | Type     | Default | Description                                                                 |
|-------------------|----------|---------|-----------------------------------------------------------------------------|
| `$text`           | `string` | -       | Input text with NUOS markup.                                                |
| `$token`          | `bool`   | `TRUE`  | If `TRUE`, processes tokens (e.g., `%token%`).                              |
| `$discard_links`  | `bool`   | `FALSE` | If `TRUE`, removes link URLs (e.g., `[link](url)` → `link`).                |

### Return Value
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Plaintext with markup removed.                                              |

### Inner Mechanisms
1. **Markup Removal**:
   - Mirrors `parse_text()`'s state machine but discards HTML tags.
   - Converts tables to tab-separated values (e.g., `|cell1|cell2|` → `cell1\tcell2`).

2. **Link Handling**:
   - If `$discard_links=FALSE`, appends URLs in parentheses (e.g., `[link](url)` → `link (url)`).
   - Skips `javascript:` links entirely.

3. **Token Processing**:
   - Delegates to `parse_token()` if `$token=TRUE`.

### Usage Context
- **Search Indexing**: Prepare text for full-text search by removing markup.
- **Plaintext Export**: Generate clean text for emails or PDFs.
- **Accessibility**: Provide fallback content for screen readers.

---

## `get_first_image()`

Extracts the first image URL from NUOS-formatted text.

### Parameters

| Name    | Type     | Default | Description                     |
|---------|----------|---------|---------------------------------|
| `$text` | `string` | -       | Text containing `[IMG]` markup. |

### Return Value
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | URL of the first image (e.g., `image://id`), or `NULL` if no image exists.  |

### Inner Mechanisms
- Uses regex to match `[IMG]`, `[<-IMG]`, or `[IMG->]` tags and capture the URL.

### Usage Context
- **Thumbnails**: Extract preview images for articles or listings.
- **SEO**: Provide OpenGraph image metadata.

---

## `parse_token()`

Processes tokens (e.g., `%token%`) in text using the `token` module.

### Parameters

| Name    | Type     | Default | Description                     |
|---------|----------|---------|---------------------------------|
| `$text` | `string` | -       | Text containing tokens.         |

### Return Value
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Text with tokens replaced by their resolved values.                         |

### Inner Mechanisms
1. **Lazy Loading**:
   - Loads the `token` module only once (static `$token` variable).
2. **Delegation**:
   - Calls `token->apply()` to resolve tokens (e.g., `%date%` → `2023-10-01`).

### Usage Context
- **Dynamic Content**: Replace tokens in templates or user-generated content.
- **Localization**: Process language tokens (e.g., `%welcome_message%`).

---

## `quote_text()`

Extracts and highlights keyword-relevant passages from text.

### Parameters

| Name                      | Type     | Default | Description                                                                 |
|---------------------------|----------|---------|-----------------------------------------------------------------------------|
| `$text`                   | `string` | -       | Input text to search.                                                       |
| `$keyword`                | `string` | -       | Keyword(s) to match (space-separated).                                     |
| `$minimum_keyword_length` | `int`    | `3`     | Minimum keyword length to consider (shorter keywords are ignored).         |

### Return Value
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Highlighted passages with ellipses (`…`) indicating omitted text.           |

### Inner Mechanisms
1. **Keyword Processing**:
   - Splits `$keyword` into individual terms and sorts by length (longest first).
   - Uses regex to match whole words, prefixes, suffixes, or substrings.

2. **Passage Extraction**:
   - Captures up to 50 characters before and 150 after each match.
   - Merges overlapping ranges and adds ellipses for gaps.

3. **Highlighting**:
   - Wraps matches in `<strong>` tags.

### Usage Context
- **Search Results**: Show snippets with keywords highlighted.
- **Content Moderation**: Extract relevant passages for review.

---

## `htmltoplain()`

Converts HTML to plaintext with optional formatting retention.

### Parameters

| Name               | Type     | Default | Description                                                                 |
|--------------------|----------|---------|-----------------------------------------------------------------------------|
| `$string`          | `string` | -       | HTML input.                                                                 |
| `$format`          | `bool`   | `FALSE` | If `TRUE`, preserves line breaks, lists, and tables.                        |
| `$discard_links`   | `bool`   | `FALSE` | If `TRUE`, removes link URLs.                                               |
| `$discard_images`  | `bool`   | `FALSE` | If `TRUE`, removes image alt text.                                          |

### Return Value
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Plaintext with HTML tags removed and formatting simplified.                 |

### Inner Mechanisms
1. **Cleanup**:
   - Removes control characters, comments, scripts, and styles.
   - Replaces `<br>` with newlines if `$format=TRUE`.

2. **Structural Formatting**:
   - Converts `<div>`, `<p>`, `<table>`, etc., to double newlines.
   - Replaces `<li>` with `* ` and `<td>`/`<th>` with tabs.

3. **Link/Image Handling**:
   - Links: `<a href="url">text</a>` → `text <url>` (unless `$discard_links=TRUE`).
   - Images: `<img alt="alt">` → `[alt]` (unless `$discard_images=TRUE`).

### Usage Context
- **Email Previews**: Generate plaintext versions of HTML emails.
- **SEO**: Provide search engines with clean text for indexing.
- **Accessibility**: Create readable text for screen readers.

---

## `text_similarity()`

Calculates the similarity between two texts as a percentage.

### Parameters

| Name     | Type     | Default | Description                     |
|----------|----------|---------|---------------------------------|
| `$text1` | `string` | -       | First text to compare.          |
| `$text2` | `string` | -       | Second text to compare.         |

### Return Value
| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `int`    | Similarity percentage (0–100).                                              |

### Inner Mechanisms
1. **Fingerprinting**:
   - Uses `fingerprint()` (not shown) to generate token sets for each text.
2. **Comparison**:
   - Computes the ratio of shared tokens to total unique tokens.

### Usage Context
- **Plagiarism Detection**: Identify similar content.
- **Duplicate Filtering**: Group near-identical articles.

---

## `tokenize_text()`

Splits text into tokens (words) for analysis.

### Parameters

| Name                 | Type     | Default | Description                                                                 |
|----------------------|----------|---------|-----------------------------------------------------------------------------|
| `$text`              | `string` | -       | Input text.                                                                 |
| `$cleanup_repeats`   | `bool`   | `FALSE` | If `TRUE`, reduces repeated characters (e.g., `aaa` → `aa`).                |

### Return Value
| Type       | Description                                                                 |
|------------|-----------------------------------------------------------------------------|
| `string[]` | Array of tokens (words).                                                    |

### Inner Mechanisms
1. **Preprocessing**:
   - Removes control characters and isolates ideograms (e.g., Chinese/Japanese/Korean characters).
   - Optionally reduces repeated characters (e.g., `loooong` → `loong`).
2. **Tokenization**:
   - Splits text on word boundaries (`CMS_REGEX_BORDER`).

### Usage Context
- **Search Indexing**: Prepare text for full-text search.
- **Text Analysis**: Enable keyword extraction or similarity comparison.


<!-- HASH:04ad610a03c1a25905f791ddcffb6b48 -->
