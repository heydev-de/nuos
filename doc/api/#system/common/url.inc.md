# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/url.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## URL Management Utilities (`url.inc`)

This file provides core URL handling utilities for the NUOS platform. It includes functions for:
- Resolving logical URL identifiers (e.g., `content://`, `image://`) into physical URLs
- URL analysis and path manipulation
- Query string generation and parameter management
- URL encoding and escaping

---

## Functions

### `translate_url`
Resolves logical NUOS URL identifiers into fully qualified physical URLs.

#### Parameters
| Name               | Type       | Default         | Description                                                                                     |
|--------------------|------------|-----------------|-------------------------------------------------------------------------------------------------|
| `$address`         | `string`   | -               | Logical URL (e.g., `content://index`, `image://logo.png`) or external URL                      |
| `$param`           | `array`    | `NULL`          | Additional query parameters to merge into the resolved URL                                      |
| `$language`        | `string`   | `CMS_LANGUAGE`  | Language code for content resolution (e.g., `en`, `de`)                                         |
| `$omit_cms_param`  | `bool`     | `FALSE`         | If `TRUE`, omits global CMS parameters (e.g., CSRF tokens) from the generated query string      |

#### Return Values
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Fully resolved physical URL, or the original `$address` if resolution fails                     |

#### Inner Mechanisms
1. **Scheme Detection**: Parses the URL scheme (e.g., `content://`, `image://`) to determine resolution logic.
2. **External URLs**: Passes through `http://` or `https://` URLs unchanged, but merges parameters.
3. **Content/Directory URLs**:
   - Resolves `content://` or `directory://` to physical paths using language-specific maps (`#system/{lang}.directory.content`).
   - Falls back to `content.php` with query parameters if no map entry exists.
4. **Asset URLs**:
   - `image://`: Resolves to `CMS_DATA_URL/image/{filename}` or an external URL if configured.
   - `media://`: Resolves to `CMS_DATA_URL/media/{filename}` or an external URL.
   - `download://`: Resolves to `download.php` with the asset index as a parameter.
5. **Fallbacks**: Returns the original `$address` if resolution fails (e.g., missing asset).

#### Usage
- **Content Links**: `translate_url("content://about")` → Resolves to a physical URL for the "about" content page.
- **Image Assets**: `translate_url("image://logo.png")` → Returns the path to the image file.
- **External URLs**: `translate_url("https://example.com")` → Passes through with merged parameters.

---

### `analyze_url`
Parses a URL into its components, extending PHP’s `parse_url` with additional metadata.

#### Parameters
| Name       | Type     | Default | Description                     |
|------------|----------|---------|---------------------------------|
| `$address` | `string` | -       | URL to analyze                  |

#### Return Values
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `array`   | Associative array of URL components (see table below), or `FALSE` if parsing fails              |

#### Returned Array Structure
| Key         | Type      | Description                                                                                     |
|-------------|-----------|-------------------------------------------------------------------------------------------------|
| `url`       | `string`  | Original URL                                                                                    |
| `scheme`    | `string`  | Protocol (e.g., `http`, `content`)                                                              |
| `host`      | `string`  | Domain or logical identifier (e.g., `index` for `content://index`)                              |
| `path`      | `string`  | Path component                                                                                  |
| `query`     | `string`  | Query string (without `?`)                                                                      |
| `fragment`  | `string`  | Fragment (without `#`)                                                                          |
| `relative`  | `bool`    | `TRUE` if the URL lacks a scheme (e.g., `path/to/file`)                                         |
| `dirname`   | `string`  | Directory path (from `pathinfo`)                                                                |
| `basename`  | `string`  | Filename with extension (from `pathinfo`)                                                       |
| `filename`  | `string`  | Filename without extension (from `pathinfo`)                                                    |
| `extension` | `string`  | File extension (from `pathinfo`)                                                                |

#### Inner Mechanisms
- Uses `parse_url` for initial parsing.
- Augments the result with `pathinfo` data (e.g., `dirname`, `extension`).
- Normalizes paths (e.g., replaces backslashes with forward slashes).

#### Usage
- **URL Inspection**: `analyze_url("content://about?param=1")` → Returns components for further processing.
- **Path Manipulation**: Extract `dirname` or `extension` for file operations.

---

### `absolute_path`
Resolves a relative path into an absolute URL based on a source URL.

#### Parameters
| Name      | Type     | Default | Description                                                                                     |
|-----------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$source` | `string` | -       | Base URL (e.g., `https://example.com/path/`)                                                    |
| `$target` | `string` | -       | Relative or absolute path (e.g., `../file.php`, `/root/file.php`)                               |

#### Return Values
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Absolute URL, or `FALSE` if resolution fails                                                   |

#### Inner Mechanisms
1. **Relative Path Handling**:
   - If `$target` starts with `/`, replaces the `$source` path entirely.
   - Otherwise, resolves `../` and `./` segments relative to `$source`.
2. **Query/Fragment Preservation**: Retains the query string and fragment from `$target`.
3. **Edge Cases**: Returns `FALSE` if `$source` is not an absolute URL.

#### Usage
- **Link Generation**: `absolute_path("https://example.com/path/", "../file.php")` → `https://example.com/file.php`.
- **Canonicalization**: Convert user-provided relative paths to absolute URLs.

---

### `querystring`
Generates a query string from an array of parameters, merging with optional alterations.

#### Parameters
| Name     | Type     | Default | Description                                                                                     |
|----------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$param` | `array`  | -       | Base parameters                                                                                 |
| `$alter` | `array`  | `NULL`  | Parameters to merge into `$param` (recursive)                                                   |

#### Return Values
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Query string (e.g., `?key=value&foo=bar`), or `FALSE` if `$param` is not an array               |

#### Inner Mechanisms
- Uses `cms_param` with `omit_cms_param=TRUE` to generate the query string.
- Recursively merges `$alter` into `$param` using `array_replace_recursive`.

#### Usage
- **Dynamic Queries**: `querystring(["page" => 1], ["sort" => "asc"])` → `?page=1&sort=asc`.
- **URL Construction**: Combine with `u()` to build full URLs.

---

### `u`
Generates a full URL by combining a base address with query parameters.

#### Parameters
| Name       | Type       | Default         | Description                                                                                     |
|------------|------------|-----------------|-------------------------------------------------------------------------------------------------|
| `$address` | `string`   | `NULL`          | Base URL (defaults to `CMS_ACTIVE_URL`). If an array, treated as `$param`.                     |
| `$param`   | `array`    | `NULL`          | Query parameters to append                                                                      |

#### Return Values
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Full URL (e.g., `https://example.com/path?key=value`)                                           |

#### Inner Mechanisms
- **Overload Handling**: If `$address` is an array, treats it as `$param` and uses `CMS_ACTIVE_URL` as the base.
- **Parameter Merging**: Uses `cms_param` to generate the query string.

#### Usage
- **Link Generation**: `u("page.php", ["id" => 1])` → `https://example.com/page.php?id=1`.
- **Current URL Extension**: `u(NULL, ["filter" => "active"])` → Appends parameters to the current URL.

---

### `qu`
Generates a URL-encoded JSON string from a URL and parameters.

#### Parameters
| Name       | Type       | Default | Description                                                                                     |
|------------|------------|---------|-------------------------------------------------------------------------------------------------|
| `$address` | `string`   | `NULL`  | Base URL (see `u()`)                                                                            |
| `$param`   | `array`    | `NULL`  | Query parameters (see `u()`)                                                                    |

#### Return Values
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | JSON-encoded URL (e.g., `"https:\/\/example.com\/path?key=value"`)                              |

#### Inner Mechanisms
- Combines `u()` and `q()` to generate a URL and encode it for JavaScript/JSON contexts.

#### Usage
- **JavaScript Integration**: `qu("api.php", ["action" => "fetch"])` → Safe for embedding in `<script>` tags.

---

### `import_querystring`
Parses a query string from a URL and imports its parameters into `$_GET` and global variables.

#### Parameters
| Name       | Type     | Default | Description                                                                                     |
|------------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$address` | `string` | -       | URL containing a query string (e.g., `https://example.com?key=value`)                           |

#### Return Values
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `bool`    | `TRUE` if parameters were imported, `FALSE` if no query string exists                           |

#### Inner Mechanisms
1. Extracts the query string using `parse_url`.
2. Parses the query string into an array with `parse_str`.
3. Normalizes UTF-8 values with `cms_utf8_normalize`.
4. Injects parameters into `$_GET` and global scope.

#### Usage
- **Legacy Support**: Import parameters from external URLs into the global state.
- **Testing**: Simulate query parameters for debugging.


<!-- HASH:500effd75a6a4aafdccb10118d97df93 -->
