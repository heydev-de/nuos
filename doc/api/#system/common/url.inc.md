# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/url.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/url.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## URL Utilities (`url.inc`)

This file provides core URL manipulation utilities for the PWNC Web Platform. It handles:
- **URL translation**: Converting logical identifiers (e.g., `content://`, `image://`) into physical URLs.
- **URL analysis**: Parsing and decomposing URLs into structured components.
- **Path resolution**: Converting relative paths to absolute paths.
- **Query string management**: Generating and merging query strings with CSRF protection.
- **URL generation**: Building full URLs from components or logical identifiers.

---

## Functions

### `translate_url($address, $param = NULL, $language = CMS_LANGUAGE, $omit_cms_param = FALSE)`

#### Purpose
Converts logical URL identifiers (e.g., `content://index`, `image://index`) into fully qualified physical URLs. Supports external HTTP/HTTPS URLs, content directories, images, media, and downloadable files.

#### Parameters
| Name               | Type          | Default         | Description                                                                                     |
|--------------------|---------------|-----------------|-------------------------------------------------------------------------------------------------|
| `$address`         | `string`      | -               | Logical or physical URL (e.g., `content://about`, `https://example.com`, `/path/to/page`).      |
| `$param`           | `array\|null` | `NULL`          | Additional query parameters to merge into the final URL.                                        |
| `$language`        | `string`      | `CMS_LANGUAGE`  | Language code for content resolution (e.g., `en`, `de`).                                        |
| `$omit_cms_param`  | `bool\|null`  | `FALSE`         | If `TRUE`, omits global CMS parameters (e.g., CSRF tokens) from the query string.               |

#### Return Values
| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | Fully qualified URL (e.g., `https://pwnc.it/content.php?content_index=about`).                  |
| `FALSE`    | If the resource (e.g., image, media) does not exist or the module fails to load.                |

#### Inner Mechanisms
1. **Scheme Detection**: Uses `preg_match` to bypass `javascript:` and `mailto:` schemes.
2. **Path Resolution**: Converts relative paths to absolute using `absolute_path()`.
3. **URL Analysis**: Decomposes the URL into components (scheme, host, path, query, fragment) via `analyze_url()`.
4. **Scheme-Specific Logic**:
   - **HTTP/HTTPS**: Preserves external URLs; appends query parameters for non-PHP paths.
   - **Directory/Content**: Resolves logical indices to physical paths using the `map` class. Falls back to `content.php` if the index is not found.
   - **Image/Media**: Loads the respective module (`image` or `media`) and resolves the resource to a physical path. Falls back to a "no image" placeholder or external URL.
   - **Download**: Redirects to `download.php` with the download index as a parameter.

#### Usage Context
- **Content Management**: Resolving `content://about` to a physical URL for rendering.
- **Asset Handling**: Generating URLs for images (`image://logo`) or media files (`media://video`).
- **External Links**: Preserving external HTTP/HTTPS URLs while ensuring query parameters are properly escaped.

#### Example
```php
// Resolve a content page in German
$url = translate_url("content://about", ["ref" => "footer"], "de");
echo $url;
// Output: https://pwnc.it/content.php?content_index=about&ref=footer&cms_language=de

// Resolve an image (falls back to "no_image.svg" if not found)
$image_url = translate_url("image://logo");
echo $image_url;
// Output: https://pwnc.it/data/image/logo.png
```

---

### `analyze_url($address)`

#### Purpose
Parses a URL into its components (scheme, host, path, query, fragment) and extends the result with path-specific details (dirname, basename, filename, extension).

#### Parameters
| Name       | Type     | Default | Description                     |
|------------|----------|---------|---------------------------------|
| `$address` | `string` | -       | URL to analyze (e.g., `https://pwnc.it/path?query=1`). |

#### Return Values
| Type               | Description                                                                                     |
|--------------------|-------------------------------------------------------------------------------------------------|
| `array\|FALSE`     | Associative array of URL components (see below) or `FALSE` if parsing fails.                    |

#### Returned Array Structure
| Key         | Type     | Description                                                                                     |
|-------------|----------|-------------------------------------------------------------------------------------------------|
| `url`       | `string` | Original URL.                                                                                   |
| `scheme`    | `string` | Protocol (e.g., `http`, `content`).                                                             |
| `host`      | `string` | Domain or logical index (e.g., `pwnc.it`, `about`).                                             |
| `path`      | `string` | Full path (e.g., `/path/to/page`).                                                              |
| `query`     | `string` | Query string (e.g., `query=1`).                                                                 |
| `fragment`  | `string` | Fragment identifier (e.g., `section1`).                                                         |
| `relative`  | `bool`   | `TRUE` if the URL is relative (no scheme).                                                      |
| `dirname`   | `string` | Directory path (e.g., `/path/to`).                                                              |
| `basename`  | `string` | Filename with extension (e.g., `page.php`).                                                     |
| `filename`  | `string` | Filename without extension (e.g., `page`).                                                      |
| `extension` | `string` | File extension (e.g., `php`).                                                                   |

#### Inner Mechanisms
1. Uses PHP’s `parse_url()` to extract basic components.
2. Uses `pathinfo()` to decompose the path into dirname, basename, filename, and extension.
3. Merges the results into a unified array with default values for missing keys.

#### Usage Context
- **URL Manipulation**: Modifying specific components of a URL (e.g., changing the query string).
- **Validation**: Checking if a URL is relative or absolute.
- **Path Resolution**: Extracting filenames or extensions for further processing.

#### Example
```php
$components = analyze_url("https://pwnc.it/path/to/page.php?query=1#section1");
print_r($components);
/*
Output:
[
    "url" => "https://pwnc.it/path/to/page.php?query=1#section1",
    "scheme" => "https",
    "host" => "pwnc.it",
    "path" => "/path/to/page.php",
    "query" => "query=1",
    "fragment" => "section1",
    "relative" => false,
    "dirname" => "/path/to",
    "basename" => "page.php",
    "filename" => "page",
    "extension" => "php"
]
*/
```

---

### `absolute_path($source, $target)`

#### Purpose
Resolves a relative path (`$target`) against a base URL (`$source`) to produce an absolute URL.

#### Parameters
| Name      | Type     | Default | Description                                                                                     |
|-----------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$source` | `string` | -       | Base URL (must be absolute, e.g., `https://pwnc.it/base/`).                                     |
| `$target` | `string` | -       | Relative or absolute path (e.g., `../path`, `/absolute`).                                       |

#### Return Values
| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | Absolute URL (e.g., `https://pwnc.it/path`).                                                    |
| `FALSE`    | If either `$source` or `$target` is invalid or `$source` is not absolute.                      |

#### Inner Mechanisms
1. Validates both `$source` and `$target` using `analyze_url()`.
2. If `$target` is absolute, returns it unchanged.
3. Resolves the path using `resolve_path()` (e.g., `../path` becomes `/base/path`).
4. Preserves the query string and fragment from `$target`.
5. Reconstructs the URL using `cms_build_url()`.

#### Usage Context
- **Link Generation**: Converting relative links in templates to absolute URLs.
- **Asset Paths**: Resolving relative asset paths (e.g., `../images/logo.png`) to full URLs.

#### Example
```php
$base = "https://pwnc.it/base/";
$relative = "../path/to/page";
$absolute = absolute_path($base, $relative);
echo $absolute;
// Output: https://pwnc.it/path/to/page
```

---

### `querystring($param, $alter = NULL)`

#### Purpose
Generates a query string from an associative array of parameters, with optional merging of additional parameters. Uses `cms_param()` for CSRF protection and recursive parameter handling.

#### Parameters
| Name    | Type          | Default | Description                                                                                     |
|---------|---------------|---------|-------------------------------------------------------------------------------------------------|
| `$param`| `array`       | -       | Base parameters (e.g., `["page" => 1, "sort" => "asc"]`).                                       |
| `$alter`| `array\|null` | `NULL`  | Additional parameters to merge into `$param` (e.g., `["filter" => "active"]`).                  |

#### Return Values
| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | Query string (e.g., `?page=1&sort=asc&filter=active`).                                          |
| `FALSE`    | If `$param` is not an array.                                                                    |

#### Inner Mechanisms
1. Validates `$param` as an array.
2. Merges `$alter` into `$param` using `array_replace_recursive()`.
3. Delegates to `cms_param($param, TRUE, TRUE)` to generate the query string.

#### Usage Context
- **Dynamic Links**: Generating query strings for pagination, filtering, or sorting.
- **Form Submissions**: Appending parameters to URLs for GET requests.

#### Example
```php
$params = ["page" => 1, "sort" => "asc"];
$additional = ["filter" => "active"];
$query = querystring($params, $additional);
echo $query;
// Output: ?page=1&sort=asc&filter=active
```

---

### `u($address = NULL, $param = NULL)`

#### Purpose
Generates a full URL by combining a base address with query parameters. Acts as a smart wrapper for `cms_url()` and `cms_param()`.

#### Parameters
| Name       | Type          | Default         | Description                                                                                     |
|------------|---------------|-----------------|-------------------------------------------------------------------------------------------------|
| `$address` | `string\|array`| `NULL`          | Base URL (e.g., `/path`) or associative array of parameters (if `$param` is omitted).          |
| `$param`   | `array`       | `NULL`          | Query parameters to append (e.g., `["ref" => "footer"]`).                                       |

#### Return Values
| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | Full URL (e.g., `https://pwnc.it/path?ref=footer`).                                             |

#### Inner Mechanisms
1. **Overload Handling**: If `$address` is an array, treats it as `$param` and uses `CMS_ACTIVE_URL` as the base.
2. **Default Values**: Uses `CMS_ACTIVE_URL` if `$address` is not a string.
3. **Query String Generation**: Delegates to `cms_param($param, TRUE, TRUE)` to append parameters.

#### Usage Context
- **Link Generation**: Creating URLs for navigation menus, buttons, or API endpoints.
- **Parameter Passing**: Appending context-specific parameters (e.g., referrers, tracking IDs).

#### Example
```php
// Generate a URL with parameters
$url = u("/products", ["category" => "electronics", "page" => 2]);
echo $url;
// Output: https://pwnc.it/products?category=electronics&page=2

// Overload: Use current URL with new parameters
$url = u(["ref" => "sidebar"]);
echo $url;
// Output: https://pwnc.it/current/page?ref=sidebar
```

---

### `qu($address = NULL, $param = NULL)`

#### Purpose
Generates a URL-encoded string (using `q()`) from a URL constructed via `u()`. Useful for embedding URLs in JavaScript or JSON.

#### Parameters
| Name       | Type          | Default | Description                                                                                     |
|------------|---------------|---------|-------------------------------------------------------------------------------------------------|
| `$address` | `string\|array`| `NULL`  | Base URL or parameters (see `u()`).                                                            |
| `$param`   | `array`       | `NULL`  | Query parameters (see `u()`).                                                                   |

#### Return Values
| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `string`   | URL-encoded string (e.g., `"https:\/\/pwnc.it\/path?ref=footer"`).                              |

#### Inner Mechanisms
1. Delegates to `u()` to generate the URL.
2. Passes the result to `q()` for JSON-style encoding.

#### Usage Context
- **JavaScript Integration**: Embedding URLs in inline scripts or `data-*` attributes.
- **API Responses**: Returning URLs in JSON payloads.

#### Example
```php
$js_url = qu("/api/data", ["id" => 123]);
echo $js_url;
// Output: "https:\/\/pwnc.it\/api\/data?id=123"
```

---

### `import_querystring($address)`

#### Purpose
Parses a query string from a URL and imports its parameters into the global `$_GET` and `$GLOBALS` arrays. Normalizes UTF-8 values using `cms_utf8_normalize()`.

#### Parameters
| Name       | Type     | Default | Description                                                                                     |
|------------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$address` | `string` | -       | URL containing a query string (e.g., `https://pwnc.it/?param=value`).                           |

#### Return Values
| Type       | Description                                                                                     |
|------------|-------------------------------------------------------------------------------------------------|
| `bool`     | `TRUE` if successful, `FALSE` if the URL lacks a query string or parsing fails.                 |

#### Inner Mechanisms
1. Uses `parse_url()` to extract the query string.
2. Parses the query string into an associative array using `parse_str()`.
3. Normalizes each value with `cms_utf8_normalize()`.
4. Updates `$_GET` and `$GLOBALS` with the parsed parameters.

#### Usage Context
- **Legacy Systems**: Importing query parameters from external URLs into the global scope.
- **URL Rewriting**: Processing query strings from rewritten URLs (e.g., `/page?param=value`).

#### Example
```php
import_querystring("https://pwnc.it/?search=café&page=1");
echo $_GET["search"]; // Output: café
echo $GLOBALS["page"]; // Output: 1
```


<!-- HASH:f7f775d72621cb1538b0983338e81e63 -->
