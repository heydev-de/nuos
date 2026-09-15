# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/snippet.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/snippet.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/common/snippet.inc

This file contains a collection of utility functions used throughout the PWNC Web Platform for rendering common UI components, managing permissions, inserting dynamic content, and handling navigation elements such as pagination and language selection. These functions are part of the `cms` namespace and rely on core platform utilities like `cms_permission`, `cms_application`, `cms_url`, `image`, `x`, and others.

---

## permission

### Overview
Renders a permission indicator block showing the current application context and its associated permissions. It is typically used in templates to provide visibility into access control settings.

### Parameters

| Name     | Type     | Default | Description |
|----------|----------|---------|-------------|
| `$array` | array    | —       | Associative array of permission keys and labels to render. |
| `$return`| boolean  | FALSE   | If `TRUE`, returns the HTML instead of echoing it. |

### Return Values

| Type     | Description |
|----------|-------------|
| string   | The generated HTML when `$return` is `TRUE`. |
| void     | Outputs directly via `echo` when `$return` is `FALSE`. |

### Inner Mechanisms
- Checks if the template is in editing mode (`CMS_TEMPLATE_OPTION`) and hides output accordingly.
- Verifies user permission using `cms_permission("interface.permission")`.
- Builds an HTML div containing links to each permission entry, using `cms_url` to generate URLs pointing to the interface module's permission page.
- Uses `x()` for XML escaping and `image()` for icon rendering.

### Usage Example
```php
$permissions = [
    'read' => 'Can Read',
    'write' => 'Can Write'
];
echo permission($permissions, TRUE);
```
This will output a styled div listing clickable permission entries for the current application.

---

## insert

### Overview
Displays an insert point in the template where dynamic content can be added or edited. It also renders any existing inserted content if available.

### Parameters

| Name       | Type     | Default | Description |
|------------|----------|---------|-------------|
| `$position`| string   | NULL    | Optional position identifier appended to the application key. |

### Return Values

| Type     | Description |
|----------|-------------|
| void     | Outputs directly via `echo`. |

### Inner Mechanisms
- Hides output in editing view mode unless explicitly allowed.
- Retrieves stored insert data using the `data` class.
- Renders an edit button linking to the interface module's insert management page.
- Parses and displays stored code either as tokens (for PHP) or as parsed text.

### Usage Example
```php
insert('sidebar');
```
This would display an editable insert area for the "sidebar" position within the current application.

---

## class_varied

### Overview
Generates alternating CSS classes for list/table rows to create visual variety (e.g., zebra striping).

### Parameters

| Name     | Type           | Default | Description |
|----------|----------------|---------|-------------|
| `$option`| mixed          | NULL    | Either a string class name or `TRUE`/`FALSE` to toggle state. |
| `$index`| integer        | 0       | Index of the flag tracker to use (for multiple independent sequences). |

### Return Values

| Type     | Description |
|----------|-------------|
| string   | A class attribute string like ` class="varied"` or ` class="myclass"`. |

### Inner Mechanisms
- Maintains a static array of flags to track alternating states per index.
- Toggles the flag on each call when `$option` is `TRUE` or unset.
- Returns appropriate class attributes based on the current flag state.

### Usage Example
```php
foreach ($items as $item) {
    echo "<tr" . class_varied('row') . ">";
    // ...
}
```
This alternates between `class="row"` and `class="varied row"` for each row.

---

## jscript

### Overview
Wraps JavaScript code in a `<script>` tag, safely escaping closing script tags to prevent XSS issues.

### Parameters

| Name     | Type     | Default | Description |
|----------|----------|---------|-------------|
| `$code`  | string   | —       | Raw JavaScript code to embed. |

### Return Values

| Type     | Description |
|----------|-------------|
| string   | A complete `<script>` tag with escaped content. |

### Inner Mechanisms
- Replaces `</` with `<\/` to avoid premature script termination.
- Wraps the result in standard `<script>` tags.

### Usage Example
```php
echo jscript('alert("Hello World");');
```
Outputs:
```html
<script>alert("Hello World");</script>
```

---

## stylesheet

### Overview
Generates one or more `<link>` tags for including stylesheets, optionally with async loading support.

### Parameters

| Name     | Type     | Default | Description |
|----------|----------|---------|-------------|
| `$url`   | string   | —       | URL of the stylesheet. |
| `$async` | boolean  | TRUE    | Whether to use asynchronous preload technique. |

### Return Values

| Type     | Description |
|----------|-------------|
| string   | HTML markup for linking the stylesheet. |

### Inner Mechanisms
- When `$async` is `TRUE`, uses `preload` with `onload` swap for non-blocking load.
- Includes a `<noscript>` fallback for browsers without JS.
- Applies `x()` for URL escaping.

### Usage Example
```php
echo stylesheet('/css/style.css');
```
Generates optimized async stylesheet inclusion.

---

## javascript

### Overview
Creates a `<script>` tag for external JavaScript files with optional async/defer attributes.

### Parameters

| Name     | Type     | Default | Description |
|----------|----------|---------|-------------|
| `$url`   | string   | —       | Source URL of the script. |
| `$async` | boolean  | TRUE    | Add `async` attribute. |
| `$defer` | boolean  | FALSE   | Add `defer` attribute. |

### Return Values

| Type     | Description |
|----------|-------------|
| string   | HTML `<script>` tag. |

### Inner Mechanisms
- Constructs a script tag with optional `async` and/or `defer`.
- Escapes the URL using `x()`.

### Usage Example
```php
echo javascript('/js/app.js', TRUE, TRUE);
```
Outputs:
```html
<script src="/js/app.js" async defer></script>
```

---

## select

### Overview
Renders an HTML `<select>` dropdown element from an associative array of options.

### Parameters

| Name        | Type           | Default | Description |
|-------------|----------------|---------|-------------|
| `$option`   | array          | —       | Key-value pairs representing option values and labels. |
| `$preset`   | mixed          | NULL    | Preselected value(s). Can be scalar or array. |
| `$name`     | string         | NULL    | Name attribute of the select element. |
| `$set_value`| boolean        | FALSE   | Use values instead of keys for option values. |
| `$disabled` | boolean        | FALSE   | Disable the select element. |
| `$height`   | integer        | NULL    | Sets size and enables multiple selection. |

### Return Values

| Type     | Description |
|----------|-------------|
| boolean  | `TRUE` on success, `FALSE` if `$option` is not an array. |

### Inner Mechanisms
- Iterates over `$option` to build `<option>` elements.
- Supports single/multiple selections via `in_array` or `streq`.
- Adds `name`, `size`, and `multiple` attributes conditionally.

### Usage Example
```php
select(['en' => 'English', 'de' => 'German'], 'en', 'lang');
```
Renders a select box with English preselected.

---

## info

### Overview
Displays an informational message with an icon.

### Parameters

| Name     | Type     | Default | Description |
|----------|----------|---------|-------------|
| `$text`  | string   | —       | Message text to display. |

### Return Values

| Type     | Description |
|----------|-------------|
| string   | HTML markup for the info box. |

### Inner Mechanisms
- Wraps the message in a div with an info icon and bold styling.

### Usage Example
```php
echo info('Configuration saved successfully.');
```

---

## alert

### Overview
Displays an alert/error message with an icon and red styling.

### Parameters

| Name     | Type     | Default | Description |
|----------|----------|---------|-------------|
| `$text`  | string   | —       | Alert message text. |

### Return Values

| Type     | Description |
|----------|-------------|
| string   | HTML markup for the alert box. |

### Inner Mechanisms
- Similar to `info()` but uses error color styling and an alert icon.

### Usage Example
```php
echo alert('Invalid input provided.');
```

---

## pagination

### Overview
Generates a pagination navigation bar for browsing through pages of results.

### Parameters

| Name     | Type     | Default | Description |
|----------|----------|---------|-------------|
| `$url`   | string   | —       | Base URL with `%page%` placeholder. |
| `$page`  | integer  | —       | Current page number. |
| `$count` | integer  | —       | Total number of pages. |
| `$next`  | string   | CMS_L_COMMAND_NEXT | Label for next link. |
| `$class` | string   | NULL    | Additional CSS class for nav element. |
| `$offset`| integer  | 0       | Starting page offset. |

### Return Values

| Type     | Description |
|----------|-------------|
| void     | Outputs directly via `echo`. |

### Inner Mechanisms
- Calculates start/end range around current page.
- Outputs first/prev/next links conditionally.
- Highlights current page number.
- Uses `str_replace` to inject page numbers into URL.

### Usage Example
```php
pagination('/list/%page%', 3, 10);
```
Renders a navigation bar for 10 pages with page 3 active.

---

## language_selector

### Overview
Renders a language selector dropdown with flag icons for enabled languages.

### Parameters

| Name       | Type     | Default | Description |
|------------|----------|---------|-------------|
| `$language`| string   | NULL    | Currently selected language code. |
| `$url`     | string   | javascript:console.log('%language%'); | URL pattern with `%language%` placeholder. |
| `$encoding`| callable | NULL    | Function to encode language codes (defaults to `q()`). |
| `$width`   | integer  | 16      | Icon width. |
| `$height`  | integer  | 12      | Icon height. |

### Return Values

| Type     | Description |
|----------|-------------|
| void     | Outputs directly via `echo`. |

### Inner Mechanisms
- Checks if multilingual feature is enabled.
- Iterates over comma-separated list of enabled languages.
- Maps language codes to flag images using a map object.
- Encodes URLs using specified encoding function.

### Usage Example
```php
language_selector('en', '/switch/%language%');
```
Renders a language switcher with flags for all enabled languages.


<!-- HASH:e490287d1a9a5e0f3eff505a55ce22ac -->
