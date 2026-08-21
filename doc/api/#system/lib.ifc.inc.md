# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.ifc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.ifc.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## IFC Class and Utility Functions

The `lib.ifc.inc` file provides the **Interface Control (IFC)** system for the PWNC Web Platform. It handles the creation, management, and rendering of modal dialogs, forms, and interactive UI components used throughout the CMS. The IFC system is designed to be lightweight, context-aware, and reusable, supporting features like:

- **Modal dialogs** with configurable menus, titles, and content
- **Form elements** (text, select, checkbox, file uploads, code editors, etc.)
- **Tabbed interfaces** and popovers
- **Permission-based UI rendering**
- **Language-aware input fields**
- **Return value handling** for external callers
- **Scroll position restoration** across page reloads

---

## Global Variables and Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$ifc_message` | `NULL` | Control message for the IFC dialog (e.g., action to perform). |
| `$ifc_page` | `NULL` | Selected page/module within the IFC. |
| `$ifc_option` | `NULL` | Controls external window behavior (e.g., `"external"`). |
| `$ifc_select` | `NULL` | Name of the form field to return a value from. |
| `$ifc_select_action` | `NULL` | URL or JavaScript to call with the return value (`%return%` placeholder). |
| `$ifc_response` | `NULL` | Response message (e.g., `CMS_MSG_DONE`, `CMS_MSG_ERROR`, or custom text). |
| `$ifc_param` | `NULL` | Default parameter for the IFC. |

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_IFC_PAGE` | `$ifc_page` | Current IFC page/module. |
| `CMS_IFC_MESSAGE` | `$ifc_message` | Current IFC message/action. |
| `CMS_IFC_OPTION` | `$ifc_option` | Current IFC option. |
| `CMS_IFC_SELECT` | `$ifc_select` | Field name for return value. |
| `CMS_IFC_SELECT_ACTION` | `$ifc_select_action` | Action to perform with return value. |
| `CMS_IFC_INPUT_PLACEHOLDER` | `CMS_L_IFC_012 . " …"` | Default placeholder text for inputs. |

---

## Utility Functions

### `ifc_permission($array = NULL)`

**Purpose**: Manages static permission settings for IFC modules.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array|null` | Associative array of permissions (e.g., `["module" => "Label"]`). If `NULL`, returns current permissions. |

**Return Value**: `array|null` – Current permission array or `NULL` if not set.

**Inner Mechanisms**:
- Uses a static variable to persist permissions across calls.
- Validates input: only non-empty arrays are stored.

**Usage Context**:
- Used to restrict access to IFC modules based on user permissions.
- Called during IFC initialization to set or retrieve permissions.

**Example**:
```php
// Set permissions for the current IFC session
ifc_permission([
    "content" => "Content Management",
    "image"   => "Image Manager"
]);

// Retrieve permissions later
$permissions = ifc_permission();
```

---

### `ifc_available($module)`

**Purpose**: Checks if a given IFC module is available (i.e., its interface file exists).

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$module` | `string` | Module name (e.g., `"image"`, `"content"`). |

**Return Value**: `bool` – `TRUE` if the module's interface file exists.

**Inner Mechanisms**:
- Checks for the file `#interface/ifc.$module.inc` in `CMS_MODULES_PATH`.

**Usage Context**:
- Used before loading or linking to an IFC module to ensure it exists.

**Example**:
```php
if (ifc_available("image")) {
    echo '<a href="' . cms_url(["ifc_page" => "image"]) . '">Image Manager</a>';
}
```

---

### `ifc_default($ifc_page = NULL)`

**Purpose**: Renders the default IFC page (dashboard) with system info, logo, and optional update check.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$ifc_page` | `string|null` | Optional page identifier to override default. |

**Return Value**: None (outputs HTML directly).

**Inner Mechanisms**:
- Sets default permissions (`["" => CMS_L_ACCESS]`).
- Displays system info (version, PHP version, OS).
- Checks for updates if user has `interface.setup` permission and no update/backup daemons are running.
- Shows license text.

**Usage Context**:
- Used as the landing page when no specific IFC module is requested.

**Example**:
```php
// Display the default IFC dashboard
ifc_default();
```

---

### `ifc_inactive($ifc_page = NULL)`

**Purpose**: Displays a simple "inactive" message in an IFC dialog.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$ifc_page` | `string|null` | Optional page identifier. |

**Return Value**: None (outputs HTML).

**Usage Context**:
- Used when a module is disabled or not accessible.

**Example**:
```php
if (!cms_permission("interface.content")) {
    ifc_inactive("content");
}
```

---

### `ifc_table_open($class = NULL)`

**Purpose**: Opens an HTML `<table>` with optional CSS class.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$class` | `string|null` | CSS class for the table. |

**Return Value**: None (outputs HTML).

**Example**:
```php
ifc_table_open("data-grid");
echo "<tr><th>Name</th><th>Value</th></tr>";
// ... rows ...
ifc_table_close();
```

---

### `ifc_table_close()`

**Purpose**: Closes an HTML `<table>`.

**Return Value**: None (outputs HTML).

---

### `ifc_tab_open($label, $command = NULL)`

**Purpose**: Opens a tabbed interface or switches to the next tab.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Tab label (may include `|icon.svg` for icons). |
| `$command` | `string` | Controls tab behavior: `"next"` (adds a new tab), `"close"` (closes current tab group), or `NULL` (starts a new tab group). |

**Return Value**: None (outputs HTML).

**Inner Mechanisms**:
- Uses static variables `$level` and `$count` to manage nested tab groups.
- Each tab group uses radio inputs for selection.
- Supports nested tab groups.

**Usage Context**:
- Used to create multi-tab interfaces within IFC dialogs.

**Example**:
```php
ifc_tab_open("General");
echo "<p>General settings here.</p>";
ifc_tab_next("Advanced");
echo "<p>Advanced settings here.</p>";
ifc_tab_close();
```

---

### `ifc_tab_next($label)`

**Purpose**: Adds a new tab to the current tab group.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Tab label. |

**Return Value**: None (outputs HTML).

**Example**: See `ifc_tab_open()`.

---

### `ifc_tab_close()`

**Purpose**: Closes the current tab group.

**Return Value**: None (outputs HTML).

---

### `ifc_popover_open($label)`

**Purpose**: Opens a popover (modal overlay) with a toggle button.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Button label (may include `|icon.svg`). |

**Return Value**: None (outputs HTML).

**Inner Mechanisms**:
- Uses the HTML `popover` API.
- Auto-increments popover ID for uniqueness.

**Example**:
```php
ifc_popover_open("Help|help.svg");
echo "<p>This is help text.</p>";
ifc_popover_close();
```

---

### `ifc_popover_close()`

**Purpose**: Closes a popover.

**Return Value**: None (outputs HTML).

---

### `ifc_close_external()`

**Purpose**: Closes an externally opened IFC window (e.g., from `ifc_option = "external"`).

**Return Value**: None (outputs HTML and exits script).

**Usage Context**:
- Used when an IFC dialog is opened in a separate window and needs to close itself.

**Example**:
```php
if (CMS_IFC_OPTION === "external") {
    ifc_close_external();
}
```

---

### `ifc_varied($option = NULL, $index = 0)`

**Purpose**: Generates a varied CSS class name (e.g., for alternating row colors).

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$option` | `string|null` | Base class name. |
| `$index` | `int` | Index to vary (e.g., row number). |

**Return Value**: `string` – Varied class name (e.g., `"row-0"`, `"row-1"`).

**Inner Mechanisms**:
- Delegates to `class_varied()`.

**Example**:
```php
echo '<div class="' . ifc_varied("row", $i) . '">Row ' . $i . '</div>';
```

---

### `ifc_parse_label($text, $size = NULL)`

**Purpose**: Parses a label string into text, URL, formatted label, and icon.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Label text (may include `|icon.svg` for icons). |
| `$size` | `int|null` | Optional max length for text truncation. |

**Return Value**: `array` – `[original_text, icon_url, formatted_label, icon_html]`.

**Inner Mechanisms**:
- Splits text at `|` to separate label from icon path.
- Supports both full paths and short specifiers (e.g., `"icon"` → looks for `icon.svg` or `icon.png` in `CMS_IMAGES_PATH`).
- Truncates text if `$size` is provided.

**Example**:
```php
[$text, $url, $label, $img] = ifc_parse_label("Save|save.svg");
echo $img . " " . $label; // <img src=".../save.svg"> Save
```

---

## Class: `ifc`

The `ifc` class is the core of the IFC system. It renders a complete HTML dialog with form, menu, and content area.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `has_page_menu` | `bool` | `TRUE` if the IFC has a page menu (left sidebar). |

---

### Constructor: `__construct($ifc_response = NULL, $ifc_page = NULL, $menu = TRUE, $param = NULL, $message = NULL, $subpage = NULL, $content_container_id = NULL)`

**Purpose**: Initializes and renders an IFC dialog.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$ifc_response` | `string|null` | Response message (e.g., success/error). |
| `$ifc_page` | `array|string|null` | Page/module identifier or associative array of available modules (for menu). |
| `$menu` | `bool|array` | `TRUE` (default menu), `FALSE` (no menu), or array of menu items. |
| `$param` | `array|string|null` | Default parameters (hidden inputs). |
| `$message` | `string|null` | Custom message for hidden `ifc_message` input. |
| `$subpage` | `string|null` | Subpage title (displayed above content). |
| `$content_container_id` | `string|null` | ID for the content container (default: `"ifc-content"` or `"ifc-content-full"`). |

**Return Value**: None (outputs HTML).

**Inner Mechanisms**:
- **Title Resolution**: Uses `CMS_IFC_PAGE` or falls back to application name.
- **Scroll Position**: Restores scroll position if the same page/subpage is reloaded.
- **Menu Construction**: If `$menu` is `TRUE`, uses default confirm/cancel buttons. If `FALSE`, uses close button. If an array, uses custom buttons.
- **Permission Overlay**: Shows permission settings if user has `interface.permission` access.
- **Language Selector**: Shows language flags if `CMS_LANGUAGE_ENABLED`.
- **Form Setup**: Includes hidden inputs for `ifc_page`, `ifc_message`, scroll position, and parameters.

**Usage Context**:
- Used to create all IFC dialogs in the CMS.

**Example**:
```php
// Open a simple IFC dialog with a custom message
$ifc = new ifc(
    "Please confirm your action.",
    NULL, // no page menu
    [
        "Confirm|check.svg" => "#confirm_action", // # prefix = confirm dialog
        "Cancel|close.svg"  => NULL
    ],
    ["id" => 123]
);
$ifc->set("Are you sure?", "title");
$ifc->close();
```

---

### Method: `param($param, $value = NULL)`

**Purpose**: Adds hidden form inputs for parameters.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$param` | `string|array` | Parameter name or associative array of parameters. |
| `$value` | `mixed` | Parameter value (ignored if `$param` is an array). |

**Return Value**: `int` – Number of hidden inputs added.

**Inner Mechanisms**:
- Recursively processes arrays (e.g., `["user[name]" => "Alice"]`).
- Escapes parameter names and values using `x()`.

**Example**:
```php
$ifc = new ifc();
$ifc->param("id", 123);
$ifc->param(["user" => ["name" => "Alice", "role" => "admin"]]);
$ifc->close();
```

---

### Method: `set($text = NULL, $type = "new text 40 60 b", $value = NULL, $checked = NULL, $name = NULL, $language = NULL)`

**Purpose**: Renders a form element (input, select, button, etc.) based on type.

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string|null` | Label text (may include `|icon.svg`). |
| `$type` | `string` | Element type and options (e.g., `"text 20 50 b"`, `"select 10 *b"`). See below for syntax. |
| `$value` | `mixed` | Default value. |
| `$checked` | `bool|null` | For checkboxes/radios: `TRUE` if checked. |
| `$name` | `string|null` | Element name (auto-generated if `NULL`). |
| `$language` | `string|null` | Comma-separated list of languages (for language-aware fields). |

**Type Syntax**:
```
[new] type [length] [maxlength] [flags]
```
- `new`: Reset internal counters (e.g., for radio groups).
- `type`: `text`, `select`, `checkbox`, `button`, `code`, `texteditor`, etc.
- `length`: Width in characters (e.g., `20`, `10x5` for multi-line).
- `maxlength`: Maximum input length.
- `flags`:
  - `*`: Show label as title attribute.
  - `:`: Inline label (no `<br>`).
  - `b`: Line break after element.
  - `c`: Checked (for checkboxes/radios).
  - `d`: Disabled.
  - `f`: Full size (takes full width).
  - `i`: Add image selector (for `texteditor`).
  - `l`: Language-aware.
  - `n`: No word wrap.
  - `r`: Add reference/link selector (for `texteditor`).
  - `t`: Add token selector (for `texteditor`).
  - `w`: Full width (takes available width).

**Return Value**: None (outputs HTML).

**Inner Mechanisms**:
- **Static State**: Tracks element indices, radio groups, and previous type.
- **Language Support**: For `text`, `textarea`, and `texteditor`, creates hidden inputs for each language and a combined value.
- **Code Editors**: Uses `contenteditable` with syntax highlighting (HTML, PHP, CSS, JS).
- **Select Elements**: Supports both native `<select>` and custom-styled `<div class="select">`.
- **Text Editors**: Supports image, link, and token insertion via external IFC dialogs.

**Example**:
```php
$ifc = new ifc();
$ifc->set("Username", "text 20 50 b", "admin");
$ifc->set("Bio", "textarea 40x5 500 b", "I am an admin.", NULL, "bio");
$ifc->set("Role", "select 20 b", "admin", NULL, "role", NULL, [
    "admin" => "Administrator",
    "editor" => "Editor",
    "guest" => "Guest"
]);
$ifc->set("Active", "checkbox", 1, TRUE);
$ifc->set("Save", "button f", NULL, TRUE); // Full-width button
$ifc->close();
```

---

### Method: `dummy($count = 1)`

**Purpose**: Skips one or more form elements (reserves space in layout).

**Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$count` | `int` | Number of elements to skip. |

**Return Value**: None.

**Example**:
```php
$ifc = new ifc();
$ifc->set("Name", "text 20 b", "Alice");
$ifc->dummy(2); // Skip two elements
$ifc->set("Save", "button");
$ifc->close();
```

---

### Method: `close()`

**Purpose**: Closes the IFC dialog and exits the script.

**Return Value**: None (outputs HTML and calls `exit()`).

**Inner Mechanisms**:
- **Return Value Handling**: If `CMS_IFC_SELECT` is set, injects JavaScript to return a value to the opener window.
  - Supports both JavaScript and URL-based return actions.
  - Replaces placeholders (`%return%`, `%title%`, `%left%`, `%top%`).
- **External Window Handling**: Closes the window if no opener is available.

**Example**:
```php
// Open a file selector and return the selected file to the opener
$ifc = new ifc(
    NULL,
    NULL,
    FALSE, // no menu
    NULL,
    NULL,
    NULL,
    "file-selector"
);
$ifc->set("Select a file:", "select 30 b", NULL, NULL, "file", NULL, [
    "doc.pdf" => "Document",
    "img.jpg" => "Image"
]);
$ifc->close(); // Returns selected file to opener
```

---

## Usage Patterns

### 1. **Simple Confirmation Dialog**
```php
$ifc = new ifc(
    "Are you sure you want to delete this item?",
    NULL,
    [
        "Delete|trash.svg" => "#delete_item", // # = confirm dialog
        "Cancel|close.svg" => NULL
    ],
    ["id" => 123]
);
$ifc->close();
```

### 2. **Multi-Tab Settings Dialog**
```php
$ifc = new ifc("Settings", [
    "general" => "General",
    "advanced" => "Advanced"
]);

ifc_tab_open("General");
$ifc->set("Site Name", "text 30 100 b", "My Site");
$ifc->set("Description", "textarea 40x3 500 b", "Welcome to my site.");
ifc_tab_next("Advanced");
$ifc->set("Debug Mode", "checkbox", 1, FALSE);
$ifc->set("API Key", "text 30 50 b", "abc123");
ifc_tab_close();

$ifc->close();
```

### 3. **External File Selector (Return Value)**
```php
// In the opener:
echo '<input type="text" id="image_path" readonly>';
echo '<button onclick="load_page(\'' . cms_url([
    "ifc_page" => "image",
    "ifc_option" => "external",
    "ifc_select" => "image_path",
    "ifc_select_action" => "javascript:document.getElementById('%return%').value='%return%';"
], TRUE) . '\')">Select Image</button>';

// In ifc.image.inc:
$ifc = new ifc();
$ifc->set("Select an image:", "select 30 b", NULL, NULL, "image", NULL, $image_list);
$ifc->close();
```

### 4. **Language-Aware Text Input**
```php
$ifc = new ifc();
$ifc->set("Title", "text 30 100 bl", "Default Title", NULL, "title", "en,de,fr");
$ifc->close();
```

### 5. **Code Editor**
```php
$ifc = new ifc();
$ifc->set("CSS Styles", "code_style 50x10 f", "body { color: #333; }");
$ifc->close();
```


<!-- HASH:61ae8db1697e9f4c7b4590f1f71ff385 -->
