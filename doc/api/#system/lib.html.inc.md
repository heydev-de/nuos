# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.html.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.html.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# HTML Parsing Library (`lib.html.inc`)

## Overview

The `lib.html.inc` file provides a lightweight, dependency-free HTML parsing and analysis toolkit for the PWNC Web Platform. It enables developers to load remote or local HTML documents, extract structured information such as metadata, headings, links, and body content, and traverse elements using a simple cursor-based approach.

This library is particularly useful for:

- Web scraping and content aggregation
- SEO analysis (meta tags, titles, link extraction)
- Content migration or transformation workflows
- Building preview systems for external URLs

---

## Constants

| Name               | Value | Description                                      |
|--------------------|-------|--------------------------------------------------|
| `CMS_HTML_GET_FIRST` | `1`   | Retrieve the first matching element from the start of the document. |
| `CMS_HTML_GET_NEXT`  | `2`   | Retrieve the next matching element after the current position. |

---

## Functions

### `html_page_info($url)`

#### Purpose
Fetches and parses an HTML page from a given URL, extracting key structural and semantic information including title, meta tags, headings (`h1`, `h2`, `h3`), copy text, side content, and outbound links.

#### Parameters

| Name | Type   | Description                             |
|------|--------|-----------------------------------------|
| `$url` | string | The absolute or relative URL of the HTML document to analyze. |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| array   | Associative array containing parsed page data on success.                   |
| boolean | `FALSE` if the document could not be loaded or lacks valid HTML structure.  |

#### Structure of Returned Array

| Key       | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `title`   | string   | Page title extracted from `<title>` tag.                                    |
| `meta`    | array    | Key-value pairs of meta tags (e.g., description, keywords).                 |
| `h1`      | string   | Concatenated H1 heading texts.                                              |
| `h2`      | string   | Concatenated H2 heading texts.                                              |
| `h3`      | string   | Concatenated H3 heading texts.                                              |
| `copy`    | string   | Main textual content excluding headings and side elements.                  |
| `text`    | string   | Combined heading and copy text separated by em-dashes.                      |
| `side`    | string   | Side content from `<aside>`, `<footer>`, and `<nav>` elements.              |
| `links`   | array    | List of associative arrays with `url` and `text` keys for each link.        |

#### Inner Mechanisms
1. Instantiates the `html` class with the provided URL.
2. Validates basic HTML structure (`<html>`, `<head>`, `<body>`).
3. Iterates over `<meta>` tags to collect name/http-equiv/content attributes.
4. Extracts the `<title>` element's text.
5. Traverses the `<body>` section to gather headings, paragraphs, and side content.
6. Resolves all `<a>` tags into absolute URLs while filtering out `nofollow` links.
7. Uses helper functions like `htmltoplain`, `stripspaces`, and `absolute_path` for normalization.

#### Usage Example

```php
$pageData = html_page_info("https://example.com/article");
if ($pageData !== FALSE) {
    echo "Title: " . $pageData["title"] . "\n";
    echo "Description: " . ($pageData["meta"]["description"] ?? "N/A") . "\n";
    echo "H1 Headings: " . $pageData["h1"] . "\n";
    foreach ($pageData["links"] as $link) {
        echo "Link: " . $link["url"] . " (" . $link["text"] . ")\n";
    }
}
```

---

## Class: `html`

A low-level HTML parser that loads and normalizes HTML content, then allows sequential traversal of elements via a position pointer.

### Properties

| Name       | Type    | Default | Description                                      |
|------------|---------|---------|--------------------------------------------------|
| `$file`    | string  | `NULL`  | Raw HTML content after loading and normalization. |
| `$position`| integer | `0`     | Current byte offset within `$file` used for traversal. |

---

### Constructor: `__construct($url)`

#### Purpose
Initializes the HTML parser by fetching and preparing the document from a given URL.

#### Parameters

| Name | Type   | Description                             |
|------|--------|-----------------------------------------|
| `$url` | string | URL of the HTML document to load.       |

#### Inner Mechanisms
1. Loads the HTTP library via `cms_load("http")`.
2. Fetches raw HTML using `http_fopen()` and `http_fetch_data()`.
3. Detects character encoding using `utf8_detect()`; falls back to ISO-8859-1 if needed.
4. Parses `<meta>` tags to determine actual charset.
5. Converts content to UTF-8 using `utf8_convert()`.
6. Normalizes Unicode with `utf8_normalize()`.
7. Strips comments, scripts, and styles using regex replacements.

#### Usage Example

```php
$doc = new html("https://example.com");
if ($doc->file !== NULL) {
    $titleElement = $doc->get(CMS_HTML_GET_FIRST, "title");
    echo "Page Title: " . $titleElement["#pcdata"] . "\n";
}
```

---

### Method: `get_attributes($string)`

#### Purpose
Parses a string of HTML attributes into an associative array.

#### Parameters

| Name     | Type   | Description                                         |
|----------|--------|-----------------------------------------------------|
| `$string`| string | A substring of HTML representing element attributes. |

#### Return Values

| Type  | Description                                                                 |
|-------|-----------------------------------------------------------------------------|
| array | Associative array where keys are attribute names and values are their string values or `TRUE` for boolean attributes. |

#### Inner Mechanisms
Uses three regular expressions to match:
1. Double-quoted attribute values (`attr="value"`)
2. Single-quoted attribute values (`attr='value'`)
3. Unquoted attribute values (`attr=value`)
4. Boolean attributes without values (`disabled`)

Each match updates the internal position counter to continue parsing sequentially.

#### Usage Example

```php
$attrs = $doc->get_attributes('class="main" id="header" disabled');
print_r($attrs);
// Output:
// Array ( [class] => main [id] => header [disabled] => 1 )
```

---

### Method: `get($option, $element, $get_pcdata, $ignore_nested)`

#### Purpose
Retrieves the next or first occurrence of a specified HTML element, optionally capturing its inner content (PCDATA).

#### Parameters

| Name            | Type      | Default             | Description                                                                 |
|-----------------|-----------|---------------------|-----------------------------------------------------------------------------|
| `$option`       | integer   | `CMS_HTML_GET_NEXT` | Either `CMS_HTML_GET_FIRST` or `CMS_HTML_GET_NEXT`.                         |
| `$element`      | string    | `NULL`              | Element selector pattern (e.g., `"div"`, `"h[1-3]"`, `"a"`). Defaults to any alphanumeric tag. |
| `$get_pcdata`   | boolean   | `TRUE`              | Whether to capture inner text/content between opening and closing tags.     |
| `$ignore_nested`| boolean   | `TRUE`              | If `TRUE`, ignores nested instances of the same element when finding closure. |

#### Return Values

| Type           | Description                                                                 |
|----------------|-----------------------------------------------------------------------------|
| array          | Structured element data with keys: `#element`, `#attribute`, `#pcdata`, `#offset`. |
| `NULL`         | No matching element found.                                                  |
| `FALSE`        | Document not loaded (`$file === NULL`).                                     |

#### Returned Array Keys

| Key           | Type    | Description                                                                 |
|---------------|---------|-----------------------------------------------------------------------------|
| `#element`    | string  | Lowercase name of the matched element.                                      |
| `#attribute`  | array   | Parsed attributes from the opening tag.                                     |
| `#pcdata`     | string  | Inner content between opening and closing tags (if applicable).             |
| `#offset`     | integer | Byte offset of the opening tag in `$file`.                                  |

#### Inner Mechanisms
1. Optionally resets position to beginning based on `$option`.
2. Matches opening tag using regex with optional attribute capture.
3. Handles self-closing tags and empty elements.
4. Searches for corresponding closing tag.
5. Balances nested occurrences of the same element unless `$ignore_nested` is set.
6. Updates internal `$position` to point past the matched element.

#### Usage Example

```php
// Get first paragraph
$para = $doc->get(CMS_HTML_GET_FIRST, "p");
echo "Paragraph Text: " . $para["#pcdata"] . "\n";

// Get all links
while ($link = $doc->get(CMS_HTML_GET_NEXT, "a")) {
    echo "Link URL: " . $link["#attribute"]["href"] . "\n";
}
```

---

### Method: `reset()`

#### Purpose
Resets the internal cursor position to the beginning of the document.

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| boolean | `TRUE` if reset was successful, `FALSE` if no document is loaded.           |

#### Usage Example

```php
$doc->reset();
$firstDiv = $doc->get(CMS_HTML_GET_FIRST, "div");
```

---

## Integration Notes

- Relies on several core PWNC utilities:
  - `cms_load()` for lazy-loading dependencies (`http`, `mime`)
  - `utf8_*` functions for multibyte handling
  - `htmltoplain()` for converting HTML fragments to plain text
  - `absolute_path()` and `analyze_url()` for URL resolution
- Designed for performance with minimal overhead—ideal for batch processing or real-time previews.
- Not intended for full DOM manipulation; better suited for read-only analysis tasks.


<!-- HASH:3c853762c88b76fafb1e44d8d6aee766 -->
