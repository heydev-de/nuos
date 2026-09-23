# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.ifc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.ifc.inc)

- **Version:** `26.9.21.15`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

The file `#system/lib.ifc.inc` defines the **IFC (Interface) framework** for the PWNC Web Platform. It provides a complete system for rendering backend/admin interface pages, including forms, menus, tabs, popovers, and various input field types. The `ifc` class is the central component, responsible for generating the HTML structure, managing form parameters, and rendering individual form elements through its flexible `set()` method.

The file also includes a set of standalone helper functions for permission management, interface availability checks, default page rendering, tab/popover management, label parsing with icon support, and external window handling.

## Global Variables

| Name | Description |
|------|-------------|
| `$ifc_message` | Control message displayed in the interface |
| `$ifc_page` | Selected page identifier |
| `$ifc_option` | Interface options (e.g., `'external'` for externally opened windows) |
| `$ifc_select` | Field name of the return value for select operations |
| `$ifc_select_action` | URL of the return call; `%return%` is replaced by the value |
| `$ifc_response` | Response message: `CMS_MSG_DONE`, `CMS_MSG_ERROR`, or custom text |
| `$ifc_param` | Default parameter value |

## Constants

| Name | Default | Description |
|------|---------|-------------|
| `CMS_IFC_PAGE` | `$ifc_page ?? ""` | Current interface page identifier |
| `CMS_IFC_MESSAGE` | `$ifc_message ?? ""` | Current control message |
| `CMS_IFC_OPTION` | `$ifc_option ?? ""` | Current interface option |
| `CMS_IFC_SELECT` | `$ifc_select ?? ""` | Select return field name |
| `CMS_IFC_SELECT_ACTION` | `$ifc_select_action ?? ""` | Select return action URL |
| `CMS_IFC_INPUT_PLACEHOLDER` | `CMS_L_IFC_012 . " …"` | Placeholder text for input fields |

## Related Functions

### ifc_permission

Sets or retrieves the global permission array used for the interface permission overlay.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array\|NULL` | Permission array to set; `NULL` to retrieve the current value |

**Return Value**

- `array\|NULL` — Returns the stored permission array when called with `NULL`, or `NULL` when setting.

**Inner Mechanisms**

Uses a `static` variable to persist the permission array across calls. When an array with at least one element is passed, it is stored; otherwise, `NULL` is stored.

**Usage Example**

```php
// Set permissions for the overlay
ifc_permission(["admin" => "Administrator", "editor" => "Editor"]);

// Retrieve permissions elsewhere
$perms = ifc_permission();
```

---

### ifc_available

Checks whether a specific interface module file exists.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$module` | `string` | Module name to check |

**Return Value**

- `bool` — `TRUE` if the file `ifc.$module.inc` exists in the interface directory, `FALSE` otherwise.

**Inner Mechanisms**

Constructs the path `CMS_MODULES_PATH . "#interface/ifc.$module.inc"` and checks with `is_file()`.

**Usage Example**

```php
if (ifc_available("image")) {
    // Image interface module is available
}
```

---

### ifc_default

Renders the default interface landing page with greeting, version info, update notification, and MCP (Model Context Protocol) configuration controls.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_page` | `string\|NULL` | Page identifier (optional) |

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

1. Sets a default permission array with a single empty-keyed entry mapped to `CMS_L_ACCESS`.
2. Determines a time-based greeting (morning, afternoon, evening, night) using `date("G")`.
3. Creates an `ifc` instance and outputs a welcome panel with avatar image, software name, version, copyright, PHP info, and homepage link.
4. If the user has `interface.setup` permission and no update/backup daemon is running, checks for available updates and displays an update button.
5. Outputs MCP configuration controls: download MCPB button, copy server config buttons, and license text.
6. Closes the interface with `$ifc->close()`.

**Usage Example**

```php
ifc_default(); // Renders the default dashboard page
```

---

### ifc_inactive

Renders a simple inactive interface page with a message.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_page` | `string\|NULL` | Page identifier (optional) |

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

Creates an `ifc` instance with `CMS_L_IFC_001` as the response message and immediately closes it.

**Usage Example**

```php
ifc_inactive(); // Shows "interface inactive" message
```

---

### ifc_table_open

Opens an HTML table element for interface layout.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$class` | `string\|NULL` | Optional CSS class for the table |

**Return Value**

- `void` — Outputs `<table>` tag directly.

**Usage Example**

```php
ifc_table_open("data-table");
// ... table content ...
ifc_table_close();
```

---

### ifc_table_close

Closes the HTML table element.

**Return Value**

- `void` — Outputs `</table>` tag directly.

**Usage Example**

```php
ifc_table_open();
// ... table content ...
ifc_table_close();
```

---

### ifc_tab_open

Opens a tab container or tab panel within the interface.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Tab label text (may include icon via `\|` separator) |
| `$command` | `string\|NULL` | Command: `NULL` (default, opens new tab), `"next"` (next tab panel), `"close"` (closes tab level) |

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

Uses static variables `$level` and `$count` to track nesting depth and tab numbering. The default command opens a new tab div and a radio input for tab selection. The `"next"` command increments the panel counter and renders a new labeled panel. The `"close"` command pops the level stack.

**Usage Example**

```php
ifc_tab_open("Settings");
// ... tab content ...
ifc_tab_next("Advanced");
// ... advanced content ...
ifc_tab_close();
```

---

### ifc_tab_next

Closes the current tab panel and opens the next one.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Label for the next tab panel |

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

Outputs `</div>` to close the current panel, then calls `ifc_tab_open($label, "next")` to start a new panel.

**Usage Example**

```php
ifc_tab_open("General");
// general settings
ifc_tab_next("Permissions");
// permission settings
ifc_tab_close();
```

---

### ifc_tab_close

Closes the current tab panel and the tab container.

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

Outputs `</div></div>` to close the current panel and container, then calls `ifc_tab_open("", "close")` to pop the level stack.

**Usage Example**

```php
ifc_tab_open("Tab 1");
// content
ifc_tab_close();
```

---

### ifc_popover_open

Opens a popover element with a toggle button and content container.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Popover toggle button label (may include icon via `\|` separator) |

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

Uses a static counter to generate unique IDs. Outputs a `<button>` with `popovertarget` attribute and a `<div>` with `popover` attribute containing a backdrop and content area with a close button.

**Usage Example**

```php
ifc_popover_open("Help|ifc/icon_help");
echo("<p>Additional information here.</p>");
ifc_popover_close();
```

---

### ifc_popover_close

Closes the popover content and container divs.

**Return Value**

- `void` — Outputs `</div></div>` directly.

**Usage Example**

```php
ifc_popover_open("Info");
echo("<p>Popover content</p>");
ifc_popover_close();
```

---

### ifc_close_external

Closes an externally opened interface window.

**Return Value**

- `void` — Outputs a minimal HTML document with a script that calls `this.close()`, then exits.

**Inner Mechanisms**

Outputs a complete HTML document with a `<script>` block that calls `this.close()` to close the popup window. The page title uses `CMS_L_IFC_003`.

**Usage Example**

```php
ifc_close_external(); // Called when an external interface window should close itself
```

---

### ifc_varied

Returns a varied class instance based on an option and index.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$option` | `mixed\|NULL` | Option value |
| `$index` | `int` | Index for variation |

**Return Value**

- `mixed` — Result of `class_varied($option, $index)`.

**Inner Mechanisms**

Delegates to the global `class_varied()` function.

**Usage Example**

```php
$varied = ifc_varied("some_option", 2);
```

---

### ifc_parse_label

Parses a label string that may contain an icon specifier separated by `\|`.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Label text, optionally with `\|icon_path` suffix |
| `$size` | `int\|NULL` | Maximum text length for truncation |

**Return Value**

- `array` — Returns `[$text, $url, $label, $img]` where:
  - `$text` — The label text without the icon specifier
  - `$url` — The resolved image URL (or `NULL`)
  - `$label` — The full label HTML including the image (or just text)
  - `$img` — The `<img>` tag HTML (or `NULL`)

**Inner Mechanisms**

1. Checks for a `\|` separator (not escaped with `\`).
2. If found, extracts the path portion and resolves it:
   - If the path starts with `CMS_ROOT_PATH`, treats it as a fully specified local path and processes it through `image_process()`.
   - Otherwise, tries `svg` and `png` extensions in `CMS_IMAGES_PATH`.
3. Generates an `<img>` tag if a URL is found.
4. Replaces escaped `\|` with `|` in the text.
5. Truncates text to `$size` using `strabridge()` if specified.
6. Returns the parsed components.

**Usage Example**

```php
[$text, $url, $label, $img] = ifc_parse_label("Save|ifc/icon_save", 20);
// $text = "Save"
// $url = "/images/ifc/icon_save.svg"
// $label = "<img src=\"...\" ...> Save"
// $img = "<img src=\"...\" ...>"
```

## ifc Class

The `ifc` class is the core interface builder. It generates the complete HTML document structure for an interface page, manages form parameters, and provides the `set()` method for rendering individual form elements.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$has_page_menu` | `bool` | `FALSE` | Whether the interface has a page menu (set during construction) |

### Constructor

#### __construct

Builds the complete interface HTML document structure including head, body, form, menu, and content container.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_response` | `mixed\|NULL` | Response message to display |
| `$ifc_page` | `array\|NULL` | Page menu array (keys are page identifiers, values are labels) |
| `$menu` | `bool\|array` | Menu configuration: `TRUE` for default confirm/cancel, `FALSE` for close-only, or custom array |
| `$param` | `mixed\|NULL` | Additional parameters to embed as hidden inputs |
| `$message` | `string\|NULL` | Message parameter value |
| `$subpage` | `string\|NULL` | Subpage title appended to the page title |
| `$content_container_id` | `string\|NULL` | ID for the content container div |

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

1. Resolves the page title from `$ifc_page` array or language constants.
2. Manages scroll position restoration using `hash32()` of the title.
3. Sets up default menu buttons (Confirm/Cancel or Close).
4. Outputs the full HTML document with:
   - DOCTYPE, `<html>`, `<head>` with meta tags, stylesheets, and scripts
   - JavaScript for scroll restoration, active element highlighting, and service worker registration
   - A `<form>` with hidden inputs for `ifc_page`, `ifc_message`, `ifc_prev`, `ifc_left`, `ifc_top`
   - Additional parameters via `param()` method
   - Response display area
   - Page menu (if `$ifc_page` is an array) with logo, desktop/website links, page navigation, language selector, and logout
   - Permission overlay (if permissions are set)
   - Command menu with accesskey assignment and confirmation handling
   - Content container div
5. Sets `$has_page_menu` based on whether `$ifc_page` is an array.

**Usage Example**

```php
$ifc = new ifc(
    CMS_MSG_DONE,
    ["dashboard" => "Dashboard", "settings" => "Settings"],
    TRUE,
    ["extra_param" => "value"]
);
$ifc->set("Username|ifc/icon_user", "text 20", NULL, NULL, "username");
$ifc->set("Save|ifc/icon_save", "button", "save");
$ifc->close();
```

---

### param

Outputs hidden input fields for form parameters.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$param` | `string\|array` | Parameter name (or array of name/value pairs) |
| `$value` | `mixed\|NULL` | Parameter value (or array for nested parameters) |

**Return Value**

- `int` — Number of input fields output.

**Inner Mechanisms**

- If `$param` is an array, iterates and calls itself recursively for each key/value pair.
- If `$value` is an array, creates nested parameter names like `param[key]`.
- Otherwise, outputs a single `<input type="hidden">` element.
- All values are escaped with `x()`.

**Usage Example**

```php
$ifc = new ifc();
$count = $ifc->param(["user_id" => 42, "action" => "edit"]);
// Outputs two hidden inputs
$ifc->close();
```

---

### set

Renders a form element based on the specified type and options. This is the primary method for building interface forms.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string\|NULL` | Label text (may include `\|icon` specifier) |
| `$type` | `string` | Element type specification string (see below) |
| `$value` | `mixed\|NULL` | Element value |
| `$checked` | `bool\|NULL` | Whether the element is checked/selected |
| `$name` | `string\|NULL` | Element name (auto-generated if empty) |
| `$language` | `string\|NULL` | Language list for multilingual fields |

**Type Specification Format**

The `$type` parameter follows the pattern: `[new] <type> [length] [maxlength] [flags]`

- `new` — Forces a new element index
- `<type>` — Element type: `button`, `text`, `password`, `date`, `datetime`, `file`, `multifile`, `texteditor`, `textarea`, `code`, `code_html`, `code_php`, `code_style`, `code_script`, `checkbox`, `select`, `multiselect`, `list`, `radio`, `title`, `label`, `description`, `info`, `dummy`
- `length` — Width in characters or `WxH` for height
- `maxlength` — Maximum character length
- Flags: `*` (title attribute), `:` (inline/no break), `b` (line break), `c` (checked), `d` (disabled), `f` (full size), `i` (image selector), `l` (language), `n` (nowrap), `r` (reference/link selector), `t` (token selector), `w` (full width)

**Return Value**

- `void` — Outputs HTML directly.

**Inner Mechanisms**

1. Parses the type specification using a regex to extract type, size, maxlength, and flags.
2. Processes the label text through `ifc_parse_label()` to extract icon and text.
3. Handles language mode: when `l` flag is set, generates language-specific input names (`l_<name>`) and renders language selector buttons.
4. For each element type, generates appropriate HTML:
   - **button**: Outputs a `<button>` with command handling, accesskey assignment, and optional confirmation.
   - **text/password**: Outputs an `<input>` with placeholder, width styling, delete button, and language support.
   - **date/datetime**: Outputs date/datetime-local inputs with value sanitization.
   - **file/multifile**: Outputs file inputs with optional multiple attribute.
   - **texteditor**: Outputs a contenteditable `<code>` element with syntax highlighting, image/link/token selectors, and hidden form submission input.
   - **textarea**: Outputs a `<textarea>` with language support.
   - **code variants**: Outputs contenteditable code editors with mode-specific highlighting.
   - **checkbox**: Outputs a checkbox input.
   - **select/multiselect**: Outputs a `<select>` or custom list-based selector with option parsing.
   - **list**: Outputs a text input with a `<datalist>` for autocomplete.
   - **radio**: Outputs radio inputs with group management via static variables.
   - **title/label/description/info**: Outputs static text elements.
   - **dummy**: Skips element indices without output.
5. Appends affix elements (line breaks, closing divs) in reverse order.
6. Stores the current type in `$prev_type` for potential future use.

**Usage Example**

```php
$ifc = new ifc();

// Text input with icon
$ifc->set("Username|ifc/icon_user", "text 20", NULL, NULL, "username");

// Password field
$ifc->set("Password|ifc/icon_lock", "password 20", NULL, NULL, "password");

// Checkbox
$ifc->set("Active", "checkbox", "1", TRUE, "active");

// Select dropdown
$ifc->set("Role", "select 15", "editor", NULL, "role", "admin,editor,viewer");

// Text editor with image selector
$ifc->set("Content|ifc/icon_content", "texteditor 60 10i", $content, NULL, "content");

// Submit button
$ifc->set("Save|ifc/icon_save", "button", "save");

$ifc->close();
```

---

### dummy

Outputs dummy (invisible) elements to skip index positions.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$count` | `int` | `1` | Number of element indices to skip |

**Return Value**

- `void` — Calls `set()` with type `"dummy $count"`.

**Inner Mechanisms**

Delegates to `set()` with `NULL` text and a dummy type specification, which increments the internal index without producing output.

**Usage Example**

```php
$ifc->set("Field 1", "text 20", NULL, NULL, "field1");
$ifc->dummy(2); // Skip 2 indices
$ifc->set("Field 4", "text 20", NULL, NULL, "field4");
```

---

### close

Closes the interface form and content container, outputs the return-value JavaScript for select operations, and terminates execution.

**Parameters**

- None

**Return Value**

- `void` — Outputs HTML/JavaScript and calls `exit()`.

**Inner Mechanisms**

1. Outputs `</div></form>` to close the content container and form.
2. If `CMS_IFC_SELECT` is set (indicating a select/return operation), outputs JavaScript functions:
   - `ifc_return(value, title)` — Sends the selected value back to the opener window, either via a JavaScript action or URL replacement.
   - `ifc_return_close()` — Closes the window if the opener is no longer valid.
   - `ifc_return_clear()` — Clears the return value.
   - Auto-closes the window if the target element doesn't exist and this is the top window.
3. Outputs `</body></html>` to complete the document.
4. Calls `exit()` to terminate script execution.

**Usage Example**

```php
$ifc = new ifc("Select an option", NULL, FALSE);
$ifc->set("Option 1", "button", "option1");
$ifc->set("Option 2", "button", "option2");
$ifc->close(); // Outputs closing tags and return JS, then exits
```


<!-- HASH:f83ef8c00e2a9514c06f56a472891c6f -->
