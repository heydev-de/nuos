# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.html.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.html.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## HTML Class and Related Functions

The `lib.html.inc` file provides utilities for parsing and extracting information from HTML documents. It includes:

1. **`html_page_info()`** – A standalone function that extracts metadata, content, and links from a webpage.
2. **`html` class** – A low-level HTML parser for traversing and extracting elements, attributes, and content.

---

## Constants

| Name               | Value | Description                          |
|--------------------|-------|--------------------------------------|
| `CMS_HTML_GET_FIRST` | `1`   | Reset parser position to document start. |
| `CMS_HTML_GET_NEXT`  | `2`   | Continue parsing from current position. |

---

## `html_page_info($url)`

### Purpose
Extracts structured information from a webpage, including:
- Meta tags
- Title
- Headings (`h1`, `h2`, `h3`)
- Main content (`copy`, `text`)
- Side content (`aside`, `footer`, `nav`)
- Links (`<a>` tags)

### Parameters

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$url` | string | URL of the webpage to analyze.       |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| array  | Structured data with keys: `meta`, `title`, `h1`, `h2`, `h3`, `copy`, `text`, `side`, `links`. |
| FALSE  | If the URL cannot be loaded or the document is not valid HTML.             |

### Inner Mechanisms
1. **Initialization**: Creates an `html` parser instance and loads the webpage.
2. **Validation**: Checks for basic HTML structure (`<html>`, `<head>`, `<body>`).
3. **Meta Extraction**: Parses `<meta>` tags, normalizing `name`/`http-equiv` and `content`.
4. **Content Extraction**:
   - Uses `html->get()` to traverse the document.
   - Splits content into headings, main text, and side content.
   - Converts HTML to plain text using `htmltoplain()`.
5. **Link Extraction**: Collects `<a>` tags, resolving relative URLs and filtering `nofollow` links.

### Usage Example
```php
$data = html_page_info("https://example.com");
if ($data !== FALSE) {
    echo "Title: " . $data["title"] . "\n";
    echo "H1: " . $data["h1"] . "\n";
    echo "Links:\n";
    foreach ($data["links"] as $link) {
        echo "- " . $link["text"] . ": " . $link["url"] . "\n";
    }
}
```
**Scenario**: Scraping a blog post to extract its title, headings, and outbound links for SEO analysis.

---

## `html` Class

### Purpose
A lightweight, zero-dependency HTML parser for traversing and extracting elements, attributes, and content.

### Properties

| Name       | Default | Description                          |
|------------|---------|--------------------------------------|
| `$file`    | `NULL`  | Raw HTML content.                    |
| `$position`| `0`     | Current parser position in the file. |

---

### `__construct($url)`

#### Purpose
Initializes the parser by fetching and preprocessing HTML from a URL.

#### Parameters

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$url` | string | URL of the HTML document to load.    |

#### Inner Mechanisms
1. **Fetching**: Uses `http_fopen()` and `http_fetch_data()` to retrieve the document.
2. **Character Set Handling**:
   - Detects UTF-8 encoding; falls back to ISO-8859-1 if not detected.
   - Extracts charset from `<meta>` tags or `Content-Type` headers.
3. **Normalization**: Converts the document to UTF-8 and normalizes it.
4. **Noise Removal**: Strips comments, `<script>`, and `<style>` tags.

#### Usage Example
```php
$parser = new html("https://example.com");
if ($parser->file !== NULL) {
    echo "Document loaded successfully.\n";
}
```
**Scenario**: Loading a webpage to analyze its structure or extract specific elements.

---

### `get_attributes($string)`

#### Purpose
Parses HTML attributes from a string (e.g., `class="example" id='main'`).

#### Parameters

| Name      | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$string` | string | String containing HTML attributes.   |

#### Return Values

| Type  | Description                                                                 |
|-------|-----------------------------------------------------------------------------|
| array | Associative array of attributes (e.g., `["class" => "example", "id" => "main"]`). |

#### Inner Mechanisms
- Uses regex to match:
  - `attribute="value"`
  - `attribute='value'`
  - `attribute=value`
  - Standalone attributes (e.g., `disabled`).

#### Usage Example
```php
$attrs = $parser->get_attributes('class="header" data-id=123');
print_r($attrs);
// Output: ["class" => "header", "data-id" => "123"]
```

---

### `get($option = CMS_HTML_GET_NEXT, $element = NULL, $get_pcdata = TRUE, $ignore_nested = TRUE)`

#### Purpose
Extracts the next or first occurrence of an HTML element, including its attributes and content.

#### Parameters

| Name             | Type    | Default            | Description                                                                 |
|------------------|---------|--------------------|-----------------------------------------------------------------------------|
| `$option`        | int     | `CMS_HTML_GET_NEXT`| `CMS_HTML_GET_FIRST` (reset position) or `CMS_HTML_GET_NEXT` (continue).    |
| `$element`       | string  | `NULL` (all)       | Element name (e.g., `"div"`, `"h1"`) or regex (e.g., `"h[1-3]"`).          |
| `$get_pcdata`    | bool    | `TRUE`             | Whether to include inner text/content.                                      |
| `$ignore_nested` | bool    | `TRUE`             | If `FALSE`, includes nested elements of the same type in the result.        |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| array  | Element data with keys: `#element`, `#attribute`, `#pcdata`, `#offset`.    |
| NULL   | No more elements found.                                                    |

#### Inner Mechanisms
1. **Element Matching**: Uses regex to find opening tags (e.g., `<div class="example">`).
2. **Attribute Parsing**: Delegates to `get_attributes()`.
3. **Content Extraction**:
   - For self-closing tags (e.g., `<img/>`), returns immediately.
   - For other tags, searches for the closing tag, handling nested elements if `$ignore_nested` is `FALSE`.
4. **Position Tracking**: Updates `$this->position` to the end of the current element.

#### Usage Example
```php
$parser->reset();
while ($div = $parser->get(CMS_HTML_GET_NEXT, "div")) {
    echo "Found div with class: " . ($div["#attribute"]["class"] ?? "none") . "\n";
    echo "Content: " . $div["#pcdata"] . "\n";
}
```
**Scenario**: Extracting all `<div>` elements with their classes and content from a webpage.

---

### `reset()`

#### Purpose
Resets the parser's position to the start of the document.

#### Return Values

| Type   | Description                          |
|--------|--------------------------------------|
| bool   | `TRUE` on success, `FALSE` if no file is loaded. |

#### Usage Example
```php
$parser->reset();
$first_h1 = $parser->get(CMS_HTML_GET_FIRST, "h1");
echo "First H1: " . $first_h1["#pcdata"] . "\n";
```
**Scenario**: Reusing the parser to traverse the document multiple times.


<!-- HASH:3c853762c88b76fafb1e44d8d6aee766 -->
