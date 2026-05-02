# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.ifc.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## IFC (Interface Control) Class and Utility Functions

The `lib.ifc.inc` file provides the core interface control system for the NUOS web platform. It handles the creation, management, and rendering of modal dialogs, forms, and interactive UI elements used throughout the CMS. The `ifc` class is the primary component, offering methods to build and control interface elements, while utility functions manage permissions, page selection, and tabbed interfaces.

---

## Global Variables and Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$ifc_message` | - | Control message for the interface. |
| `$ifc_page` | - | Currently selected page in the interface. |
| `$ifc_option` | - | Options for externally opened windows. |
| `$ifc_select` | - | Field name of the return value for interface callbacks. |
| `$ifc_select_action` | - | URL for the return call, with `%return%` replaced by the selected value. |
| `$ifc_response` | - | Response message (e.g., `CMS_MSG_DONE`, `CMS_MSG_ERROR`, or custom text). |
| `$ifc_param` | - | Default parameter for the interface. |

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_IFC_MESSAGE` | `$ifc_message` | Alias for `$ifc_message`. |
| `CMS_IFC_PAGE` | `CMS_INSTANCE` | Alias for the current instance. |
| `CMS_IFC_OPTION` | `$ifc_option` | Alias for `$ifc_option`. |
| `CMS_IFC_SELECT` | `$ifc_select` | Alias for `$ifc_select`. |
| `CMS_IFC_SELECT_ACTION` | `$ifc_select_action` | Alias for `$ifc_select_action`. |
| `CMS_IFC_INPUT_PLACEHOLDER` | `CMS_L_IFC_012 . " …"` | Placeholder text for input fields. |

---

## Utility Functions

### `ifc_permission($array = NULL)`
Manages interface permissions for the current user.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | `array\|null` | Associative array of permissions. If `NULL`, returns the current permissions. |

| Return Value | Type | Description |
|--------------|------|-------------|
| `$permission` | `array\|null` | Current permissions or `NULL` if no permissions are set. |

**Inner Mechanisms:**
- Uses a static variable to store permissions.
- If `$array` is provided and is a non-empty array, it updates the static permissions.

**Usage:**
- Used to check or set permissions for interface modules.
- Example: `ifc_permission(["module1" => "Permission 1"]);`

---

### `ifc_available($module)`
Checks if an interface module is available.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$module` | `string` | Name of the module to check. |

| Return Value | Type | Description |
|--------------|------|-------------|
| `bool` | `TRUE` if the module exists, `FALSE` otherwise. |

**Inner Mechanisms:**
- Checks for the existence of the file `CMS_MODULES_PATH . "#interface/ifc.$module.inc`.

**Usage:**
- Used to verify module availability before loading or rendering.
- Example: `if (ifc_available("image")) { ... }`

---

### `ifc_default($ifc_page = NULL)`
Renders the default interface page (dashboard).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_page` | `string\|null` | Page identifier. If `NULL`, uses the current page. |

**Inner Mechanisms:**
- Sets default permissions for the interface.
- Displays system information (version, PHP details, license).
- Checks for updates if the user has permission and no update/backup daemons are running.

**Usage:**
- Called when no specific interface page is selected.
- Example: `ifc_default();`

---

### `ifc_inactive($ifc_page = NULL)`
Renders an "inactive" interface message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_page` | `string\|null` | Page identifier. If `NULL`, uses the current page. |

**Inner Mechanisms:**
- Creates an `ifc` instance with a message indicating inactivity.

**Usage:**
- Used when the interface is inactive or no valid page is selected.
- Example: `ifc_inactive();`

---

### `ifc_table_open($class = NULL)`
Opens an HTML table.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$class` | `string\|null` | CSS class for the table. |

**Usage:**
- Used to start a table layout in the interface.
- Example: `ifc_table_open("my-table");`

---

### `ifc_table_close()`
Closes an HTML table.

**Usage:**
- Used to end a table layout in the interface.
- Example: `ifc_table_close();`

---

### `ifc_tab_open($label, $command = NULL)`
Opens a tabbed interface section.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$label` | `string` | Label for the tab. |
| `$command` | `string\|null` | Command to control tab behavior (`"next"` or `"close"`). |

**Inner Mechanisms:**
- Uses a static array `$level` to track tab nesting.
- Generates radio inputs for tab selection.
- Supports nested tabs with automatic level management.

**Usage:**
- Used to create tabbed interfaces.
- Example:
  ```php
  ifc_tab_open("Tab 1");
  // Content for Tab 1
  ifc_tab_next("Tab 2");
  // Content for Tab 2
  ifc_tab_close();
  ```

---

### `ifc_tab_next($label)`
Moves to the next tab in a tabbed interface.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$label` | `string` | Label for the next tab. |

**Usage:**
- Used to transition to the next tab in a tabbed interface.
- Example: `ifc_tab_next("Next Tab");`

---

### `ifc_tab_close()`
Closes the current tabbed interface section.

**Usage:**
- Used to end a tabbed interface section.
- Example: `ifc_tab_close();`

---

### `ifc_close_external()`
Closes an externally opened interface window.

**Inner Mechanisms:**
- Renders a minimal HTML page with a script to close the window.

**Usage:**
- Used to close pop-up windows or external interfaces.
- Example: `ifc_close_external();`

---

### `ifc_varied($option = NULL, $index = 0)`
Generates a varied CSS class name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$option` | `string\|null` | Base class name. |
| `$index` | `int` | Index for variation. |

| Return Value | Type | Description |
|--------------|------|-------------|
| `string` | Varied class name. |

**Inner Mechanisms:**
- Delegates to `class_varied()` to generate a varied class name.

**Usage:**
- Used to apply dynamic CSS classes.
- Example: `ifc_varied("my-class", 1);`

---

## `ifc` Class

### Constructor: `__construct($ifc_response = NULL, $ifc_page = NULL, $menu = TRUE, $param = NULL, $message = NULL, $subpage = NULL, $content_container_id = NULL)`
Initializes an interface dialog.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_response` | `string\|null` | Response message to display. |
| `$ifc_page` | `string\|array\|null` | Page identifier or array of pages for the control menu. |
| `$menu` | `bool\|array` | If `TRUE`, displays a default menu. If an array, uses custom menu items. If `FALSE`, hides the menu. |
| `$param` | `array\|string\|null` | Parameters to include in the form. |
| `$message` | `string\|null` | Additional message to include in the form. |
| `$subpage` | `string\|null` | Subpage title. |
| `$content_container_id` | `string\|null` | ID for the content container. |

**Inner Mechanisms:**
- Sets the page title based on `$ifc_page` or system defaults.
- Restores scroll position if the same page/subpage is reloaded.
- Builds the HTML structure for the interface, including:
  - Meta tags and base URL.
  - JavaScript for scroll restoration and focus management.
  - Form with hidden inputs for state management.
  - Control menu (if `$ifc_page` is an array).
  - Permission overlay (if permissions are set).
  - Command menu (if `$menu` is provided).

**Usage:**
- Used to create modal dialogs or interface pages.
- Example:
  ```php
  $ifc = new ifc("Welcome to the interface", ["page1" => "Page 1"], TRUE);
  ```

---

### `param($param, $value = NULL)`
Adds hidden form parameters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$param` | `string\|array` | Parameter name or associative array of parameters. |
| `$value` | `string\|array\|null` | Parameter value or associative array of values. |

| Return Value | Type | Description |
|--------------|------|-------------|
| `int` | Number of parameters added. |

**Inner Mechanisms:**
- Recursively processes arrays to handle nested parameters.
- Outputs hidden input fields for each parameter.

**Usage:**
- Used to pass state or data through the form.
- Example:
  ```php
  $ifc->param("ifc_param", "value");
  $ifc->param(["param1" => "value1", "param2" => "value2"]);
  ```

---

### `set($text = NULL, $type = "new text 40 60 b", $value = NULL, $checked = NULL, $name = NULL, $language = NULL)`
Renders a form element based on the specified type.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$text` | `string\|null` | Label or text for the element. |
| `$type` | `string` | Type of element (e.g., `"text"`, `"checkbox"`, `"select"`). See below for format. |
| `$value` | `string\|array\|null` | Default value for the element. |
| `$checked` | `bool\|null` | Whether the element is checked (for checkboxes/radio buttons). |
| `$name` | `string\|null` | Name of the element. If `NULL`, auto-generates a name. |
| `$language` | `string\|null` | Comma-separated list of languages for multilingual fields. |

**Type Format:**
The `$type` parameter follows the format:
`[new] [type] [length] [maxlength] [parameters]`
- `new`: Resets the element index.
- `type`: Element type (e.g., `text`, `checkbox`, `select`).
- `length`: Width of the element (e.g., `40` or `40x10` for width x height).
- `maxlength`: Maximum input length.
- `parameters`: Flags for element behavior (e.g., `b` for line break, `c` for checked, `l` for language support).

**Supported Element Types:**
| Type | Description |
|------|-------------|
| `button` | Button element. |
| `text` | Text input. |
| `password` | Password input. |
| `date` | Date input. |
| `file` | File upload input. |
| `multifile` | Multiple file upload input. |
| `texteditor` | Rich text editor. |
| `textarea` | Textarea. |
| `code`, `code_html`, `code_php`, `code_style`, `code_script` | Code editor with syntax highlighting. |
| `checkbox` | Checkbox. |
| `select`, `multiselect` | Dropdown or multi-select list. |
| `list` | Text input with datalist. |
| `radio` | Radio button. |
| `image` | Image radio button. |
| `title` | Title text. |
| `label` | Label for subsequent elements. |
| `description` | Descriptive text. |
| `info` | Informational text. |
| `alert` | Alert message. |
| `dummy` | Skips one or more elements. |

**Inner Mechanisms:**
- Uses static variables to manage element indices and radio button groups.
- Supports multilingual fields with language selectors.
- Handles nested parameters and dynamic element generation.
- Integrates with external modules (e.g., image, content, token selectors for `texteditor`).

**Usage:**
- Used to build forms with various input types.
- Example:
  ```php
  $ifc->set("Username", "text 40 60", "default_value");
  $ifc->set("Description", "textarea 40x5 500", "default text");
  $ifc->set("Language", "select 20", ["en" => "English", "de" => "German"], "en");
  ```

---

### `dummy($count = 1)`
Skips one or more form elements.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$count` | `int` | Number of elements to skip. |

**Usage:**
- Used to reserve space in a form layout.
- Example: `$ifc->dummy(2);`

---

### `close()`
Closes the interface and renders the final HTML.

**Inner Mechanisms:**
- Outputs the closing tags for the form and HTML document.
- Includes JavaScript for handling return values if `CMS_IFC_SELECT` is set.

**Usage:**
- Called to finalize and render the interface.
- Example: `$ifc->close();`


<!-- HASH:0feb715026ced526ec39467d26765497 -->
