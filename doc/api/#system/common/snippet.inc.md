# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/snippet.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Snippet Utility Functions

This file provides a collection of utility functions for rendering common UI elements and handling repetitive frontend tasks in the NUOS platform. These functions abstract complex HTML generation, permission checks, and dynamic content insertion to streamline development.

---

### `permission`

Renders a permission management interface for the current application or a specific permission key.

#### Parameters

| Name    | Value/Default | Description                                                                                     |
|---------|---------------|-------------------------------------------------------------------------------------------------|
| `$array` | Array         | Associative array of permission keys (suffixes) and their display labels.                      |
| `$return` | `FALSE`       | If `TRUE`, returns the HTML as a string; otherwise, echoes it directly.                        |

#### Return Values

| Type    | Description                                                                                     |
|---------|-------------------------------------------------------------------------------------------------|
| `void`  | Echoes HTML if `$return` is `FALSE`.                                                            |
| `string`| Returns HTML string if `$return` is `TRUE`.                                                    |

#### Inner Mechanisms

1. **Visibility Check**: Hides the UI if in non-editing template mode (`CMS_TEMPLATE_OPTION_NONE`).
2. **Permission Check**: Verifies the user has the `interface.permission` permission via `cms_permission()`.
3. **Application Context**: Fetches the current application name using `cms_application()`.
4. **Dynamic Links**: Generates links for each permission key, appending it to the application name (e.g., `app.key`).
5. **CSRF Protection**: Uses `cms_url()` to generate secure URLs with CSRF tokens.

#### Usage

- **Context**: Used in admin interfaces to provide quick access to permission settings for the current application or module.
- **Example**:
  ```php
  permission(["edit" => "Edit Access", "delete" => "Delete Access"]);
  ```

---

### `insert`

Renders dynamic content snippets or an edit button for inserting content at predefined positions.

#### Parameters

| Name       | Value/Default | Description                                                                                     |
|------------|---------------|-------------------------------------------------------------------------------------------------|
| `$position` | `NULL`        | Optional suffix to the current application name to target a specific insertion point.          |

#### Return Values

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| `void` | Echoes HTML directly.                                                                           |

#### Inner Mechanisms

1. **Visibility Check**: Hides in non-editing template mode (`CMS_TEMPLATE_OPTION_NONE`).
2. **Data Fetching**: Retrieves insertion data from `#system/insert` using the `data` class.
3. **Edit Button**: Displays an edit button if the user has `interface.insert` permission and is in editing mode.
4. **Content Rendering**: Renders the snippet HTML or parsed text/code if a valid insertion key exists.

#### Usage

- **Context**: Used in templates to inject dynamic content (e.g., ads, widgets) or provide edit controls for such content.
- **Example**:
  ```php
  insert("header"); // Renders content for the "app.header" position.
  ```

---

### `class_varied`

Generates alternating CSS class names for styling rows or elements with varied backgrounds.

#### Parameters

| Name     | Value/Default | Description                                                                                     |
|----------|---------------|-------------------------------------------------------------------------------------------------|
| `$option` | `NULL`        | If `TRUE`, resets the alternation flag. If a string, appends it to the `varied` class.         |
| `$index`  | `0`           | Index to track multiple independent alternation sequences.                                     |

#### Return Values

| Type     | Description                                                                                     |
|----------|-------------------------------------------------------------------------------------------------|
| `string` | Returns a `class` attribute string (e.g., `class="varied"` or `class="custom"`).               |

#### Inner Mechanisms

1. **Static Flag**: Uses a static array to track alternation state per `$index`.
2. **Toggle Logic**: Flips the flag on each call unless `$option` is `TRUE` (reset).
3. **String Handling**: If `$option` is a string, appends it to the `varied` class or returns it standalone.

#### Usage

- **Context**: Used in loops to alternate row colors or styles (e.g., table rows, list items).
- **Example**:
  ```php
  foreach ($items as $item) {
      echo "<div" . class_varied() . ">$item</div>";
  }
  ```

---

### `jscript`

Wraps JavaScript code in `<script>` tags with automatic escaping of closing tags.

#### Parameters

| Name   | Value/Default | Description                                                                                     |
|--------|---------------|-------------------------------------------------------------------------------------------------|
| `$code` | String        | JavaScript code to wrap.                                                                        |

#### Return Values

| Type     | Description                                                                                     |
|----------|-------------------------------------------------------------------------------------------------|
| `string` | Returns the code wrapped in `<script>` tags with `</` escaped to `<\/`.                        |

#### Inner Mechanisms

1. **Escaping**: Replaces `</` with `<\/` to prevent premature script termination in inline HTML.

#### Usage

- **Context**: Used to embed inline JavaScript safely in HTML templates.
- **Example**:
  ```php
  echo jscript("alert('Hello');");
  ```

---

### `stylesheet`

Generates a `<link>` tag for CSS files with optional asynchronous loading.

#### Parameters

| Name    | Value/Default | Description                                                                                     |
|---------|---------------|-------------------------------------------------------------------------------------------------|
| `$url`   | String        | URL of the stylesheet.                                                                          |
| `$async` | `TRUE`        | If `TRUE`, uses `preload` with `onload` to load the stylesheet asynchronously.                  |

#### Return Values

| Type     | Description                                                                                     |
|----------|-------------------------------------------------------------------------------------------------|
| `string` | Returns the `<link>` tag(s) for the stylesheet.                                                 |

#### Inner Mechanisms

1. **Async Loading**: Uses `preload` with `onload` to defer stylesheet loading if `$async` is `TRUE`.
2. **Fallback**: Wraps a `<noscript>` fallback for browsers without JavaScript.

#### Usage

- **Context**: Used to load CSS files with performance optimizations (async loading).
- **Example**:
  ```php
  echo stylesheet("/css/main.css");
  ```

---

### `select`

Generates an HTML `<select>` dropdown or multi-select list.

#### Parameters

| Name         | Value/Default | Description                                                                                     |
|--------------|---------------|-------------------------------------------------------------------------------------------------|
| `$option`     | Array         | Associative array of options (keys as labels, values as option values).                        |
| `$preset`     | `NULL`        | Preselected value(s). Can be a string (single) or array (multiple).                            |
| `$name`       | `NULL`        | Name attribute for the `<select>` element.                                                      |
| `$set_value`  | `FALSE`       | If `TRUE`, uses option values as both labels and values.                                        |
| `$disabled`   | `FALSE`       | If `TRUE`, disables the dropdown.                                                               |
| `$height`     | `NULL`        | If set, enables multi-select with the given height (number of visible rows).                   |

#### Return Values

| Type     | Description                                                                                     |
|----------|-------------------------------------------------------------------------------------------------|
| `bool`   | Returns `TRUE` on success, `FALSE` if `$option` is not an array.                               |

#### Inner Mechanisms

1. **Validation**: Checks if `$option` is an array.
2. **Multi-Select**: Adds `multiple` attribute if `$height` is set.
3. **Preselection**: Compares `$preset` with option values to set `selected` attributes.
4. **Escaping**: Uses `x()` to escape HTML special characters.

#### Usage

- **Context**: Used to render dropdowns for forms (e.g., user roles, categories).
- **Example**:
  ```php
  select(["Red" => "red", "Blue" => "blue"], "red", "color");
  ```

---

### `info`

Generates an informational message with an icon.

#### Parameters

| Name   | Value/Default | Description                                                                                     |
|--------|---------------|-------------------------------------------------------------------------------------------------|
| `$text` | String        | The message text.                                                                               |

#### Return Values

| Type     | Description                                                                                     |
|----------|-------------------------------------------------------------------------------------------------|
| `string` | Returns the HTML for the info message.                                                          |

#### Inner Mechanisms

1. **Icon**: Uses `image("icon_info")` to fetch the info icon.
2. **Styling**: Wraps the text in a `<div>` with a bold `<strong>` tag.

#### Usage

- **Context**: Used to display non-critical informational messages to users.
- **Example**:
  ```php
  echo info("Your changes have been saved.");
  ```

---

### `alert`

Generates an alert message with an icon and red text.

#### Parameters

| Name   | Value/Default | Description                                                                                     |
|--------|---------------|-------------------------------------------------------------------------------------------------|
| `$text` | String        | The alert text.                                                                                 |

#### Return Values

| Type     | Description                                                                                     |
|----------|-------------------------------------------------------------------------------------------------|
| `string` | Returns the HTML for the alert message.                                                         |

#### Inner Mechanisms

1. **Icon**: Uses `image("icon_alert")` to fetch the alert icon.
2. **Styling**: Wraps the text in a `<div>` with a bold red `<strong>` tag.

#### Usage

- **Context**: Used to display critical warnings or errors to users.
- **Example**:
  ```php
  echo alert("Invalid input detected.");
  ```

---

### `pagination`

Generates a pagination control for navigating multi-page content.

#### Parameters

| Name     | Value/Default       | Description                                                                                     |
|----------|---------------------|-------------------------------------------------------------------------------------------------|
| `$url`    | String              | URL template with `%page%` placeholder for the page number.                                    |
| `$page`   | Integer             | Current page number.                                                                            |
| `$count`  | Integer             | Total number of pages.                                                                          |
| `$next`   | `CMS_L_COMMAND_NEXT`| Label for the "next" button.                                                                    |
| `$class`  | `NULL`              | CSS class for the `<nav>` element.                                                              |
| `$offset` | `0`                 | Offset for page numbering (e.g., `1` for 1-based indexing).                                     |

#### Return Values

| Type   | Description                                                                                     |
|--------|-------------------------------------------------------------------------------------------------|
| `void` | Echoes the pagination HTML directly.                                                            |

#### Inner Mechanisms

1. **Bounds Checking**: Ensures `$page` is within valid range (`$offset` to `$count`).
2. **Range Calculation**: Shows 5 pages centered on the current page (e.g., `3 4 [5] 6 7`).
3. **Ellipsis**: Adds `…` for gaps between the first/last page and the visible range.
4. **Navigation Links**: Generates links for "previous", "next", "first", and individual pages.
5. **Accessibility**: Uses `rel="prev"`/`rel="next"` for SEO and screen readers.

#### Usage

- **Context**: Used in lists or tables to navigate large datasets (e.g., search results, logs).
- **Example**:
  ```php
  pagination("/items?page=%page%", 3, 10);
  ```


<!-- HASH:013659253c910eefae332c4f5ecd4bd3 -->
