# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/snippet.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/snippet.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Snippet Utilities (`snippet.inc`)

This file provides a collection of utility functions for generating common UI elements, handling dynamic content insertion, and managing frontend assets. These functions are designed to simplify repetitive tasks such as rendering permission indicators, inserting dynamic content, managing CSS/JS assets, and generating interactive UI components like dropdowns and pagination.

---

## Functions

### `permission`
Renders a permission indicator for the current application, showing available permission levels and linking to the permission management interface.

#### Parameters
| Name    | Type     | Default | Description                                                                                     |
|---------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$array`| `array`  | -       | Associative array of permission levels (key) and their display labels (value).                 |
| `$return`| `bool`  | `FALSE` | If `TRUE`, returns the HTML as a string; otherwise, echoes it directly.                         |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `string`| HTML markup for the permission indicator (if `$return` is `TRUE`).                              |
| `void`  | Echoes the HTML directly (if `$return` is `FALSE`).                                             |

#### Inner Mechanisms
1. **Visibility Check**: Hides the indicator in non-editing mode (`CMS_TEMPLATE_OPTION_NONE`).
2. **Permission Check**: Verifies the user has `interface.permission` access.
3. **Dynamic Links**: Generates links to the permission management interface for each permission level in `$array`.
4. **Application Context**: Uses `cms_application()` to fetch the current application name and constructs keys as `application.permission_key`.

#### Usage Example
```php
// Display permission levels for the current application
permission([
    "read" => "Read Access",
    "write" => "Write Access",
    "admin" => "Admin Access"
]);
```
**Explanation**: Renders a clickable permission indicator for the current application, allowing users to navigate to the permission management interface.

---

### `insert`
Dynamically inserts content or a placeholder for editable content based on the current application and position.

#### Parameters
| Name       | Type     | Default | Description                                                                                     |
|------------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$position`| `string` | `NULL`  | Optional sub-position identifier (e.g., `"header"`, `"footer"`).                                |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `void`  | Echoes the HTML for the insert or edit button directly.                                        |

#### Inner Mechanisms
1. **Visibility Check**: Hides in non-editing mode (`CMS_TEMPLATE_OPTION_NONE`).
2. **Data Fetching**: Retrieves insert data from `#system/insert` using the current application and `$position` as the key.
3. **Edit Button**: Shows an edit button if the user has `interface.insert` permission and is in editing mode.
4. **Content Rendering**: If an insert is defined, fetches the associated code from `#system/insert.code` and renders it as HTML or parsed text.

#### Usage Example
```php
// Insert dynamic content in the "header" position
insert("header");
```
**Explanation**: Renders a header-specific insert or an edit button if the user has permission and is in editing mode.

---

### `class_varied`
Toggles a `varied` CSS class on successive calls, useful for alternating row styles or dynamic UI states.

#### Parameters
| Name     | Type      | Default | Description                                                                                     |
|----------|-----------|---------|-------------------------------------------------------------------------------------------------|
| `$option`| `mixed`   | `NULL`  | If `TRUE`, resets the flag. If a string, returns a class string with `$option` and `varied`.    |
| `$index` | `int`     | `0`     | Index for multiple independent toggles.                                                         |

#### Return Values
| Type     | Description                                                                                     |
|----------|-------------------------------------------------------------------------------------------------|
| `string` | HTML class attribute (e.g., `class="varied"` or `class="custom varied"`).                       |

#### Inner Mechanisms
1. **Static Flag**: Uses a static array `$flag` to track toggle state per `$index`.
2. **Toggle Logic**: Flips the flag on each call unless `$option` is `TRUE` (reset) or the flag is unset.
3. **String Handling**: If `$option` is a string, appends `varied` to it.

#### Usage Example
```php
// Alternate row classes in a loop
for ($i = 0; $i < 5; $i++) {
    echo "<div" . class_varied("row") . ">Row $i</div>";
}
```
**Explanation**: Alternates between `class="row"` and `class="row varied"` for each row.

---

### `jscript`
Wraps JavaScript code in `<script>` tags, escaping closing tags to prevent syntax errors.

#### Parameters
| Name   | Type     | Default | Description                                                                                     |
|--------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$code`| `string` | -       | JavaScript code to wrap.                                                                        |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `string`| HTML `<script>` tag containing the escaped code.                                                |

#### Inner Mechanisms
1. **Escaping**: Replaces `</` with `<\/` to avoid premature script termination.

#### Usage Example
```php
echo jscript("alert('Hello, PWNC!');");
```
**Explanation**: Outputs `<script>alert('Hello, PWNC!');</script>`.

---

### `stylesheet`
Generates a `<link>` tag for CSS files with optional asynchronous loading.

#### Parameters
| Name    | Type     | Default | Description                                                                                     |
|---------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$url`  | `string` | -       | URL of the stylesheet.                                                                          |
| `$async`| `bool`   | `TRUE`  | If `TRUE`, uses `preload` with `onload` to load asynchronously.                                 |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `string`| HTML `<link>` tag(s) for the stylesheet.                                                        |

#### Inner Mechanisms
1. **Async Loading**: Uses `preload` and `onload` to load stylesheets without render-blocking.
2. **Fallback**: Wraps a `<noscript>` fallback for non-JS environments.

#### Usage Example
```php
echo stylesheet("styles/main.css");
```
**Explanation**: Outputs an async-loading stylesheet link with a fallback.

---

### `javascript`
Generates a `<script>` tag for JavaScript files with optional `async` or `defer` attributes.

#### Parameters
| Name    | Type     | Default | Description                                                                                     |
|---------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$url`  | `string` | -       | URL of the JavaScript file.                                                                     |
| `$async`| `bool`   | `TRUE`  | If `TRUE`, adds the `async` attribute.                                                          |
| `$defer`| `bool`   | `FALSE` | If `TRUE`, adds the `defer` attribute.                                                          |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `string`| HTML `<script>` tag for the JavaScript file.                                                    |

#### Usage Example
```php
echo javascript("scripts/app.js", async: true, defer: false);
```
**Explanation**: Outputs `<script src="scripts/app.js" async></script>`.

---

### `select`
Renders an HTML `<select>` dropdown with options, preselection, and optional multi-select.

#### Parameters
| Name         | Type      | Default | Description                                                                                     |
|--------------|-----------|---------|-------------------------------------------------------------------------------------------------|
| `$option`    | `array`   | -       | Associative array of options (keys as values or labels).                                        |
| `$preset`    | `mixed`   | `NULL`  | Preselected value(s). Can be a single value or an array for multi-select.                       |
| `$name`      | `string`  | `NULL`  | Name attribute for the `<select>` element.                                                      |
| `$set_value` | `bool`    | `FALSE` | If `TRUE`, uses option values as both values and labels.                                        |
| `$disabled`  | `bool`    | `FALSE` | If `TRUE`, disables the dropdown.                                                               |
| `$height`    | `int`     | `NULL`  | If set, enables multi-select with the specified height.                                         |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `bool`  | `TRUE` on success, `FALSE` if `$option` is not an array.                                       |

#### Inner Mechanisms
1. **Multi-Select**: Enables multi-select if `$height` is set.
2. **Preselection**: Marks options as `selected` if they match `$preset`.
3. **Value Handling**: Uses `$set_value` to determine whether to use option keys or values as `<option>` values.

#### Usage Example
```php
select(
    ["red" => "Red", "green" => "Green", "blue" => "Blue"],
    "green",
    "color",
    height: 3
);
```
**Explanation**: Renders a multi-select dropdown with "Green" preselected.

---

### `info`
Renders an informational message with an info icon.

#### Parameters
| Name   | Type     | Default | Description                                                                                     |
|--------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$text`| `string` | -       | The message text.                                                                               |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `string`| HTML markup for the info message.                                                               |

#### Usage Example
```php
echo info("This is an informational message.");
```
**Explanation**: Outputs a styled info message with an icon.

---

### `alert`
Renders an error/warning message with an alert icon.

#### Parameters
| Name   | Type     | Default | Description                                                                                     |
|--------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$text`| `string` | -       | The alert text.                                                                                 |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `string`| HTML markup for the alert message.                                                              |

#### Usage Example
```php
echo alert("Invalid input detected!");
```
**Explanation**: Outputs a red-styled alert message with an icon.

---

### `pagination`
Generates a pagination control for navigating between pages.

#### Parameters
| Name     | Type     | Default               | Description                                                                                     |
|----------|----------|-----------------------|-------------------------------------------------------------------------------------------------|
| `$url`   | `string` | -                     | URL template with `%page%` placeholder (e.g., `"/page/%page%"`).                                |
| `$page`  | `int`    | -                     | Current page number.                                                                            |
| `$count` | `int`    | -                     | Total number of pages.                                                                          |
| `$next`  | `string` | `CMS_L_COMMAND_NEXT`  | Label for the "next" button.                                                                    |
| `$class` | `string` | `NULL`                | CSS class for the `<nav>` element.                                                              |
| `$offset`| `int`    | `0`                   | Page offset (e.g., `1` for 1-based indexing).                                                   |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `void`  | Echoes the pagination HTML directly.                                                            |

#### Inner Mechanisms
1. **Range Calculation**: Shows 5 pages centered on the current page (e.g., `3 4 [5] 6 7`).
2. **Ellipsis**: Adds `…` for truncated ranges.
3. **Navigation Links**: Includes "previous", "next", "first", and "last" links where applicable.

#### Usage Example
```php
pagination("/blog/page/%page%", 3, 10);
```
**Explanation**: Renders pagination for a blog with 10 pages, highlighting page 3.

---

### `language_selector`
Renders a language selector with flags or default icons, linking to language-specific URLs.

#### Parameters
| Name       | Type     | Default                          | Description                                                                                     |
|------------|----------|----------------------------------|-------------------------------------------------------------------------------------------------|
| `$language`| `string` | `NULL`                           | Currently selected language.                                                                    |
| `$url`     | `string` | `"javascript:console.log('%language%');"` | URL template with `%language%` placeholder.                                                     |
| `$encoding`| `callable`| `__NAMESPACE__ . "\\q"`          | Function to encode language values (default: `q()`).                                            |
| `$width`   | `int`    | `16`                             | Width of language icons.                                                                        |
| `$height`  | `int`    | `12`                             | Height of language icons.                                                                       |

#### Return Values
| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `void`  | Echoes the language selector HTML directly.                                                     |

#### Inner Mechanisms
1. **Language Check**: Exits if no languages are enabled (`CMS_LANGUAGE_ENABLED`).
2. **Icon Mapping**: Uses `#system/language.image` to map language codes to flag images.
3. **URL Handling**: Replaces `%language%` in `$url` with the encoded language value.

#### Usage Example
```php
language_selector(
    "en",
    "/set-language/%language%",
    width: 24,
    height: 18
);
```
**Explanation**: Renders a language selector with 24x18px flags, linking to `/set-language/en` for English.


<!-- HASH:e490287d1a9a5e0f3eff505a55ce22ac -->
