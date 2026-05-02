# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.html.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## HTML Class and Utility Function

This file provides the `html` class for parsing and extracting structured data from HTML documents, along with the `html_page_info` utility function for extracting metadata, content, and links from web pages.

---

## Constants

| Name               | Value | Description                          |
|--------------------|-------|--------------------------------------|
| `CMS_HTML_GET_FIRST` | `1`   | Flag to retrieve the first occurrence of an HTML element. |
| `CMS_HTML_GET_NEXT`  | `2`   | Flag to retrieve the next occurrence of an HTML element.  |

---

## `html_page_info($url)`

Extracts structured information (metadata, title, headings, content, and links) from an HTML page.

### Purpose
Parses an HTML document to extract:
- Meta tags (name or http-equiv attributes)
- Page title
- Headings (h1, h2, h3)
- Main content (excluding side content like asides, footers, and navs)
- Side content (asides, footers, navs)
- All outgoing links (excluding nofollow links)

### Parameters

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$url` | string | URL of the HTML page to analyze.     |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| array  | Associative array with keys: `meta`, `title`, `h1`, `h2`, `h3`, `copy`, `text`, `side`, `links`. Returns `FALSE` on failure. |
| FALSE  | If the URL cannot be loaded, or the document lacks basic HTML structure.   |

### Inner Mechanisms
1. **Initialization**: Creates an `html` object and verifies basic HTML structure.
2. **Meta Extraction**: Iterates through `<meta>` tags, normalizing keys and values.
3. **Title Extraction**: Retrieves the `<title>` tag content.
4. **Content Extraction**:
   - Uses the `html` class to iterate through headings (`h1`, `h2`, `h3`) and side content (`aside`, `footer`, `nav`).
   - Extracts text between elements using `htmltoplain()`.
   - Separates main content (`copy`) from side content (`side`).
5. **Link Extraction**: Collects all `<a>` tags with `href` attributes, resolving relative URLs and filtering nofollow links.

### Usage Context
- **SEO Analysis**: Extract metadata, headings, and links for SEO audits.
- **Content Scraping**: Retrieve structured content from external pages.
- **Data Mining**: Analyze page structure and outgoing links.

---

## `html` Class

Parses and navigates HTML documents using a lightweight, regex-based approach.

---

### Properties

| Name       | Default | Description                          |
|------------|---------|--------------------------------------|
| `$file`    | `NULL`  | The HTML document content.           |
| `$position`| `0`     | Current parsing position in the document. |

---

### `__construct($url)`

Initializes the `html` object by fetching and preprocessing an HTML document.

#### Purpose
- Fetches the HTML document from the given URL.
- Converts the document to UTF-8 if necessary.
- Removes noise (comments, scripts, styles).

#### Parameters

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$url` | string | URL of the HTML document to load.    |

#### Return Values
None (constructor).

#### Inner Mechanisms
1. **Fetching**: Uses `http_fopen()` and `http_fetch_data()` to retrieve the document.
2. **Character Set Conversion**:
   - Detects UTF-8 encoding.
   - Falls back to ISO-8859-1 if not UTF-8.
   - Extracts charset from `<meta>` tags or HTTP headers if available.
3. **Normalization**: Normalizes UTF-8 encoding and removes noise (comments, scripts, styles).

#### Usage Context
- **Document Parsing**: Initialize the parser before extracting elements.
- **Preprocessing**: Clean and standardize HTML for consistent parsing.

---

### `get_attributes($string)`

Extracts attributes from an HTML element's attribute string.

#### Purpose
Parses an attribute string (e.g., `class="example" id='test' disabled`) into an associative array.

#### Parameters

| Name      | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$string` | string | The attribute string to parse.       |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| array  | Associative array of attributes (key: attribute name, value: attribute value or `TRUE` for boolean attributes). Returns `FALSE` on failure. |

#### Inner Mechanisms
- Uses regex to match attribute patterns:
  - `attribute="value"`
  - `attribute='value'`
  - `attribute=value`
  - `attribute` (boolean attributes)

#### Usage Context
- **Element Parsing**: Used internally by `get()` to extract attributes from HTML elements.

---

### `get($option = CMS_HTML_GET_NEXT, $element = NULL, $get_pcdata = TRUE, $ignore_nested = TRUE)`

Retrieves the next or first occurrence of an HTML element.

#### Purpose
Finds and returns an HTML element, its attributes, and its content (PCDATA).

#### Parameters

| Name             | Type    | Default            | Description                                                                 |
|------------------|---------|--------------------|-----------------------------------------------------------------------------|
| `$option`        | int     | `CMS_HTML_GET_NEXT`| `CMS_HTML_GET_FIRST` to start from the beginning, `CMS_HTML_GET_NEXT` to continue from the current position. |
| `$element`       | string  | `NULL`             | Element name or regex pattern (e.g., `"div"`, `"h[1-3]"`). If `NULL`, matches any element. |
| `$get_pcdata`    | bool    | `TRUE`             | If `FALSE`, skips PCDATA extraction (faster for empty elements).           |
| `$ignore_nested` | bool    | `TRUE`             | If `TRUE`, skips nested elements of the same type.                         |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| array  | Associative array with keys: `#element` (element name), `#attribute` (attributes), `#pcdata` (content), `#offset` (start position in the document). Returns `NULL` if no element is found. |
| NULL   | If no matching element is found.                                            |

#### Inner Mechanisms
1. **Element Matching**: Uses regex to find the opening tag of the specified element.
2. **Attribute Extraction**: Calls `get_attributes()` to parse the element's attributes.
3. **PCDATA Extraction**:
   - For non-empty elements, searches for the closing tag.
   - Handles nested elements by counting opening/closing tags.
4. **Position Tracking**: Updates `$this->position` to the end of the current element.

#### Usage Context
- **Iterative Parsing**: Traverse the document to extract specific elements (e.g., `<meta>`, `<a>`).
- **Content Extraction**: Retrieve text content or attributes from elements.

---

### `reset()`

Resets the parser's position to the start of the document.

#### Purpose
Allows re-parsing the document from the beginning.

#### Parameters
None.

#### Return Values

| Type   | Description                          |
|--------|--------------------------------------|
| bool   | `TRUE` on success, `FALSE` on failure. |

#### Usage Context
- **Re-parsing**: Reset the parser before iterating through elements again.


<!-- HASH:e299e08e364eba27bb8b6c84c31d09e0 -->
