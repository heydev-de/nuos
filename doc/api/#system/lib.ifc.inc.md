# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.ifc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.ifc.inc)

- **Version:** `26.9.8.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## IFC Class and Interface Functions

The `lib.ifc.inc` file is the core of PWNC's interface system. It provides the `ifc` class — a comprehensive HTML form builder that generates the entire interface page structure including headers, navigation menus, command buttons, form fields, and JavaScript initialization. It also includes a suite of helper functions for managing interface permissions, parsing labels with icons, creating tabbed interfaces, popovers, and rendering default/inactive interface pages.

The IFC (Interface) system is used by every module's interface page. When a module needs to render its admin or frontend interface, it instantiates the `ifc` class, calls `set()` to add form fields and buttons, and finally calls `close()` to finalize the page. The class handles all boilerplate HTML, CSS, JavaScript, scroll-position restoration, language switching, permission overlays, and responsive layout.

### Global Variables

| Name | Default | Description |
|---|---|---|
| `$ifc_message` | `""` | Control message passed between interface requests |
| `$ifc_page` | `""` | Currently selected interface page identifier |
| `$ifc_option` | `""` | Interface option (e.g., `'external'` for externally opened windows) |
| `$ifc_select` | `""` | Field name of the return value when used as a selector dialog |
| `$ifc_select_action` | `""` | URL/action of the return call; `%return%` is replaced by the selected value |
| `$ifc_response` | `""` | Response message: `CMS_MSG_DONE`, `CMS_MSG_ERROR`, or custom text |
| `$ifc_param` | `""` | Default parameter value |

### Constants

| Name | Value | Description |
|---|---|---|
| `CMS_IFC_PAGE` | `$ifc_page ?? ""` | The current interface page identifier |
| `CMS_IFC_MESSAGE` | `$ifc_message ?? ""` | The current control message |
| `CMS_IFC_OPTION` | `$ifc_option ?? ""` | The current interface option |
| `CMS_IFC_SELECT` | `$ifc_select ?? ""` | The selector return field name |
| `CMS_IFC_SELECT_ACTION` | `$ifc_select_action ?? ""` | The selector return action URL |
| `CMS_IFC_INPUT_PLACEHOLDER` | `CMS_L_IFC_012 . " …"` | Placeholder text for input fields |

---

## ifc_permission

### ifc_permission($array = NULL)

Manages the static permission array used by the IFC permission overlay. When called with an array, it sets the permission list. When called without arguments, it returns the currently stored permission array.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$array` | `array\|NULL` | Permission array to store, or `NULL` to retrieve the current value |

**Return Value**

- `array\|NULL` — The stored permission array, or `NULL` if none has been set

**Inner Mechanisms**

Uses a `static` variable to persist the permission array across calls within the same request. If the provided array is valid (is an array with at least one element), it is stored; otherwise, `NULL` is stored.

**Usage Example**

```php
// Set permissions for the permission overlay
ifc_permission([
    "interface.content" => "Content Management",
    "interface.image"   => "Image Management"
]);

// Later, retrieve the stored permissions
$perms = ifc_permission();
```

---

## ifc_available

### ifc_available($module)

Checks whether an interface file for the given module exists in the interface directory.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$module` | `string` | Module name to check |

**Return Value**

- `bool` — `TRUE` if the file `ifc.$module.inc` exists in the `#interface` directory, `FALSE` otherwise

**Inner Mechanisms**

Constructs the path `CMS_MODULES_PATH . "#interface/ifc.$module.inc"` and checks if it is a file using `is_file()`.

**Usage Example**

```php
if (ifc_available("image")) {
    // The image interface module is available
    $ifc->set("Image Settings|ifc/icon_image", "button", "javascript:ifc_image();");
}
```

---

## ifc_default

### ifc_default($ifc_page = NULL)

Renders the default interface page — a welcome/dashboard screen with greeting, version info, update notification, and MCP (Model Context Protocol) configuration tools.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$ifc_page` | `string\|NULL` | Optional page override |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

1. Sets a default permission array with a single empty-keyed entry mapped to `CMS_L_ACCESS`.
2. Determines a time-based greeting (morning, afternoon, evening, night) using `date("G")`.
3. Creates a new `ifc` instance with the greeting as the response.
4. Outputs an avatar image, greeting heading, software version, copyright, homepage link, and PHP/server info.
5. If the user has `interface.setup` permission and no update/backup daemon is running, checks for available updates and displays an update button if a new version exists.
6. Outputs MCP configuration section with download and copy-config buttons.
7. Outputs the license text.
8. Calls `$ifc->close()` to finalize the page.

**Usage Example**

```php
// Render the default dashboard interface
ifc_default();
```

---

## ifc_inactive

### ifc_inactive($ifc_page = NULL)

Renders a minimal inactive interface page with a "not available" message.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$ifc_page` | `string\|NULL` | Optional page override |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

Creates a new `ifc` instance with `CMS_L_IFC_001` (a "not available" language constant) as the response message, then immediately closes it.

**Usage Example**

```php
// Show an inactive page when a module is not available
ifc_inactive();
```

---

## ifc_table_open

### ifc_table_open($class = NULL)

Opens an HTML `<table>` element, optionally with a CSS class.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$class` | `string\|NULL` | Optional CSS class for the table |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

Uses `nstre()` to check if a class was provided. If so, outputs `<table class="$class">`; otherwise outputs `<table>`.

**Usage Example**

```php
ifc_table_open("data-table");
// ... table rows ...
ifc_table_close();
```

---

## ifc_table_close

### ifc_table_close()

Closes the current HTML table element.

**Parameters**

- None

**Return Value**

- `void` — Outputs `</table>`

**Usage Example**

```php
ifc_table_open();
echo("<tr><td>Data</td></tr>");
ifc_table_close();
```

---

## ifc_tab_open

### ifc_tab_open($label, $command = NULL)

Opens a tabbed interface container. Supports three modes: default (open new tab level), `next` (switch to next tab within the same level), and `close` (close the current tab level).

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$label` | `string` | Label text for the tab (may include icon via `ifc_parse_label`) |
| `$command` | `string\|NULL` | Command mode: `NULL` (default), `"next"`, or `"close"` |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

Uses static variables `$level` (array tracking tab nesting levels) and `$count` (global tab counter).

- **Default mode**: Pushes a new level onto `$level` with a new count and zero sub-tabs. Sets `$flag = TRUE` and outputs `<div class="tab">`. Falls through to the `next` case.
- **Next mode**: Increments the sub-tab counter for the current level. Generates a unique radio input name (`ifc_tab_N`). Checks if the global variable with that name matches the current value to determine if this tab should be checked. Outputs a `<label>` with a radio input and a `<div>` for tab content.
- **Close mode**: Pops the current level from `$level`.

**Usage Example**

```php
ifc_tab_open("Settings");
$ifc->set("General", "text");
ifc_tab_next("Advanced");
$ifc->set("Debug Mode", "checkbox");
ifc_tab_close();
```

---

## ifc_tab_next

### ifc_tab_next($label)

Closes the current tab content div and opens the next tab within the same tab level.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$label` | `string` | Label for the next tab |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

Outputs `</div>` to close the current tab content, then calls `ifc_tab_open($label, "next")` to start the next tab.

**Usage Example**

```php
ifc_tab_open("Tab 1");
$ifc->set("Field A", "text");
ifc_tab_next("Tab 2");
$ifc->set("Field B", "text");
ifc_tab_close();
```

---

## ifc_tab_close

### ifc_tab_close()

Closes the current tab content div, the tab container div, and pops the tab level.

**Parameters**

- None

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

Outputs `</div></div>` to close the tab content and tab container, then calls `ifc_tab_open("", "close")` to pop the level from the static `$level` array.

**Usage Example**

```php
ifc_tab_open("My Tab");
$ifc->set("Content", "texteditor");
ifc_tab_close();
```

---

## ifc_popover_open

### ifc_popover_open($label)

Opens a popover element (a toggle button that reveals a floating content panel).

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$label` | `string` | Label for the popover toggle button (may include icon via `ifc_parse_label`) |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

Uses a static counter to generate unique IDs. Outputs a `<button>` with `popovertarget` attribute pointing to a `<div>` with `popover` attribute. The popover contains a backdrop div, a content div with a close button, and the parsed label.

**Usage Example**

```php
ifc_popover_open("Help|ifc/icon_help");
echo("<p>Additional information goes here.</p>");
ifc_popover_close();
```

---

## ifc_popover_close

### ifc_popover_close()

Closes the popover content and backdrop divs.

**Parameters**

- None

**Return Value**

- `void` — Outputs `</div></div>`

**Usage Example**

```php
ifc_popover_open("Settings");
$ifc->set("Option", "checkbox");
ifc_popover_close();
```

---

## ifc_close_external

### ifc_close_external()

Outputs a minimal HTML page that automatically closes itself via JavaScript. Used for externally opened interface windows that need to close after completing an action.

**Parameters**

- None

**Return Value**

- `void` — Outputs HTML and calls `exit()`

**Inner Mechanisms**

Outputs a complete HTML document with a `<script>` block containing `this.close()`, then exits. This is typically called when an external interface window has completed its task and needs to close itself.

**Usage Example**

```php
// After processing an external interface action
ifc_close_external();
```

---

## ifc_varied

### ifc_varied($option = NULL, $index = 0)

Delegates to `class_varied()` to retrieve a varied class name based on the given option and index.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$option` | `mixed\|NULL` | Option value for class variation |
| `$index` | `int` | Index for class variation |

**Return Value**

- `mixed` — The result of `class_varied($option, $index)`

**Inner Mechanisms**

A simple pass-through wrapper around the `class_varied()` utility function, which likely generates CSS class names with alternating patterns (e.g., odd/even row classes).

**Usage Example**

```php
$class = ifc_varied("row", $i);
echo("<div class=\"$class\">Row $i</div>");
```

---

## ifc_parse_label

### ifc_parse_label($text, $size = NULL)

Parses a label string that may contain an icon specifier (separated by `|`) and returns the text, icon URL, formatted label, and image HTML.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$text` | `string` | Label text, optionally with `|icon_path` suffix |
| `$size` | `int\|NULL` | Maximum length for text truncation via `strabridge()` |

**Return Value**

- `array` — A 4-element array: `[$text, $url, $label, $img]`
  - `$text` — The text portion without the icon specifier
  - `$url` — The icon image URL, or `NULL` if no icon
  - `$label` — The full formatted label (image HTML + text, optionally truncated)
  - `$img` — The `<img>` HTML tag for the icon, or `NULL`

**Inner Mechanisms**

1. Searches for the last `|` in the text that is not escaped (preceded by `\`).
2. If found, splits the text into the label portion and the icon path.
3. Resolves the icon path:
   - If it starts with `CMS_ROOT_PATH`, treats it as a fully specified local path and processes it through `image_process()`.
   - Otherwise, tries `.svg` and `.png` extensions in `CMS_IMAGES_PATH`.
4. If an icon URL is found, generates an `<img>` tag.
5. Unescapes `\|` to `|` in the text.
6. Truncates the text using `strabridge()` if `$size` is provided and positive.
7. Returns the array with text, URL, label, and image HTML.

**Usage Example**

```php
// Parse a label with an icon
$result = ifc_parse_label("Settings|ifc/icon_settings", 20);
// $result[0] = "Settings"
// $result[1] = "/pwnc/image/interface/icon_settings.svg"
// $result[2] = "<img src=\"...\" ...> Settings"
// $result[3] = "<img src=\"...\" ...>"
```

---

## ifc Class

The `ifc` class is the main interface builder. It generates a complete HTML page with form structure, navigation, command buttons, and JavaScript initialization.

### Properties

| Name | Type | Default | Description |
|---|---|---|---|
| `$has_page_menu` | `bool` | `FALSE` | Whether the interface has a page navigation menu |

### Constructor

#### __construct($ifc_response = NULL, $ifc_page = NULL, $menu = TRUE, $param = NULL, $message = NULL, $subpage = NULL, $content_container_id = NULL)

Initializes the interface page, outputting the HTML document head, body, form, navigation menu, permission overlay, and command button container.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$ifc_response` | `string\|NULL` | Response message to display (e.g., success/error) |
| `$ifc_page` | `array\|NULL` | Array of page identifiers and labels for the navigation menu |
| `$menu` | `bool\|array` | Menu configuration: `TRUE` for default confirm/cancel buttons, `FALSE` for close-only, or a custom array of menu items |
| `$param` | `array\|string\|NULL` | Additional hidden form parameters |
| `$message` | `string\|NULL` | Message parameter value |
| `$subpage` | `string\|NULL` | Subpage title appended to the page title |
| `$content_container_id` | `string\|NULL` | ID for the content container div |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

1. **Title resolution**: Determines the page title from `$ifc_page[CMS_IFC_PAGE]`, falling back to language constants, `CMS_IFC_PAGE`, and `CMS_APPLICATION`.
2. **Scroll position restoration**: Uses `hash32()` to compare the current title hash with `$ifc_prev`. If the same page, restores `$ifc_left` and `$ifc_top`; otherwise resets to 0.
3. **Menu setup**: If `$menu` is `TRUE`, creates default confirm/cancel buttons. If `FALSE`, creates a close button. If an array, uses it as-is.
4. **HTML document output**: Outputs the full HTML document with meta tags, CSS/JS includes, and base URL.
5. **JavaScript initialization**: Outputs scripts for scroll-to-active-element, scroll restoration, busy animation, service worker registration, and response click-outside handling.
6. **Form output**: Creates the `<form>` element with hidden inputs for `ifc_page`, `ifc_message`, `ifc_prev`, `ifc_left`, `ifc_top`, and any additional parameters.
7. **Response display**: Outputs the response message div.
8. **Page menu**: If `$ifc_page` is an array, outputs the navigation menu with logo, desktop/website links, page links, language selector, and expand button.
9. **Permission overlay**: If the user has `interface.permission` permission and a permission array is set, outputs the permission overlay with switch and buttons.
10. **Command menu**: If `$menu` is truthy, outputs the command button container with JavaScript for `ifc_command()`.
11. **Subpage display**: If `$subpage` is set, outputs it.
12. **Menu button rendering**: Iterates over menu items, parsing labels, handling JavaScript commands, messages, and cancel actions. Assigns access keys automatically.

**Usage Example**

```php
$ifc = new ifc("Settings saved", ["home" => "Dashboard", "settings" => "Settings"], TRUE);
$ifc->set("Site Title", "text", "My Website");
$ifc->set("Enable Cache", "checkbox", "1", TRUE);
$ifc->close();
```

---

### param

#### param($param, $value = NULL)

Outputs hidden form input fields for the given parameters. Supports recursive array parameters.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$param` | `string\|array` | Parameter name or array of name/value pairs |
| `$value` | `mixed\|NULL` | Parameter value (ignored if `$param` is an array) |

**Return Value**

- `int` — The number of input fields output

**Inner Mechanisms**

- If `$param` is an array, iterates over each key/value pair and recursively calls `param()` for each.
- If `$value` is an array, iterates over each sub-value and recursively calls `param()` with a bracketed parameter name (e.g., `param[key]`).
- Otherwise, outputs a single `<input type="hidden">` with the parameter name and value, and returns 1.

**Usage Example**

```php
// Single parameter
$ifc->param("ifc_param", "some_value");

// Array of parameters
$ifc->param(["action" => "save", "id" => 42]);

// Nested array parameter
$ifc->param("config", ["key1" => "val1", "key2" => "val2"]);
```

---

### set

#### set($text = NULL, $type = "new text 40 b", $value = NULL, $checked = NULL, $name = NULL, $language = NULL)

Renders a form field or button element. This is the primary method for adding content to the interface.

**Parameters**

| Name | Type | Description |
|---|---|---|
| `$text` | `string\|NULL` | Label text (may include icon via `ifc_parse_label`) |
| `$type` | `string` | Type specification string (see below) |
| `$value` | `mixed\|NULL` | Field value or button command |
| `$checked` | `bool\|NULL` | Whether the field is checked/selected |
| `$name` | `string\|NULL` | Custom field name (auto-generated if empty) |
| `$language` | `string\|NULL` | Comma-separated list of languages for multilingual fields |

**Type String Format**

The `$type` parameter is a space-separated string parsed by regex: `^(new)?\s*([a-z_]+)\s*([0-9x]*)\s*([0-9]*)\s*([\*:bcdfilnrtw]*)$`

| Position | Meaning |
|---|---|
| 1 | `new` — If present, starts a new radio group |
| 2 | Field type: `button`, `text`, `password`, `date`, `datetime`, `file`, `multifile`, `texteditor`, `textarea`, `code`, `code_html`, `code_php`, `code_style`, `code_script`, `checkbox`, `select`, `multiselect`, `list`, `radio`, `title`, `label`, `description`, `info`, `dummy` |
| 3 | Size: numeric for width, `WxH` for dimensions |
| 4 | Maximum length (for text inputs) |
| 5 | Flags: `*` (title attribute), `:` (inline/no break), `b` (line break), `c` (checked), `d` (disabled), `f` (full size), `i` (image selector), `l` (language), `n` (nowrap), `r` (reference/link selector), `t` (token selector), `w` (full width) |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

1. **Type parsing**: Uses regex to extract the `new` flag, type, size, maxlength, and flags from the type string.
2. **Flag extraction**: Converts flag characters into boolean variables (`$title`, `$break`, `$checked`, `$disabled`, `$fullsize`, `$image`, `$_language`, `$nowrap`, `$reference`, `$token`, `$fullwidth`).
3. **Language setup**: If the `l` flag is set, determines the active language and prepares the language list.
4. **Label parsing**: Calls `ifc_parse_label()` to extract icon and text from the label.
5. **Type-specific rendering**: Each type case outputs appropriate HTML:
   - **button**: Outputs a `<button>` with onclick handler. Supports JavaScript commands, `ifc_post()` calls, and confirm dialogs.
   - **text/password**: Outputs a text or password input with placeholder, maxlength, delete button, and optional language selector.
   - **date/datetime**: Outputs date or datetime-local inputs with sanitized values.
   - **file/multifile**: Outputs file inputs with optional multiple attribute.
   - **texteditor**: Outputs a contenteditable `<code>` element with syntax highlighting, image/link/token selectors, and hidden form submission field.
   - **textarea**: Outputs a `<textarea>` with language support.
   - **code variants**: Outputs a contenteditable code editor with syntax highlighting for HTML, PHP, CSS, or JavaScript.
   - **checkbox**: Outputs a checkbox input.
   - **select/multiselect**: Outputs a `<select>` or custom select list with options.
   - **list**: Outputs a text input with a `<datalist>` for autocomplete.
   - **radio**: Outputs a radio input, managing radio group state via static variables.
   - **title/label/description/info**: Outputs static text elements.
   - **dummy**: Skips rendering (used for spacing).
6. **Language selector**: If the `l` flag is set, outputs language buttons and a hidden combined submission field.
7. **Affix output**: Outputs any accumulated affix HTML (line breaks, closing divs/labels) in reverse order.

**Usage Example**

```php
// Text input with label
$ifc->set("Site Title|ifc/icon_title", "text 40", "My Website");

// Checkbox
$ifc->set("Enable Feature", "checkbox c", "1");

// Select dropdown
$ifc->set("Theme", "select", "dark", NULL, NULL, NULL);
$ifc->set("Options", "select", ["light" => "Light", "dark" => "Dark"]);

// Text editor with image selector
$ifc->set("Content", "texteditor fi 60x10", $content);

// Button with JavaScript command
$ifc->set("Save|ifc/icon_save", "button", "javascript:ifc_save();");

// Radio group
$ifc->set("Alignment", "new radio", "left", TRUE);
$ifc->set(NULL, "radio", "center");
$ifc->set(NULL, "radio", "right");

// Multilingual text field
$ifc->set("Title", "text l", $value, NULL, NULL, "en,de,fr");
```

---

### dummy

#### dummy($count = 1)

Outputs one or more dummy (invisible) fields to create spacing in the form layout.

**Parameters**

| Name | Type | Default | Description |
|---|---|---|---|
| `$count` | `int` | `1` | Number of dummy fields to skip |

**Return Value**

- `void` — Outputs HTML directly

**Inner Mechanisms**

Calls `set(NULL, "dummy $count")` which increments the internal index by `$count` without rendering any visible output.

**Usage Example**

```php
$ifc->set("Field 1", "text");
$ifc->dummy(2); // Skip 2 positions
$ifc->set("Field 4", "text");
```

---

### close

#### close()

Finalizes the interface page by closing the content div, form, and HTML document. If the interface is being used as a selector dialog (i.e., `CMS_IFC_SELECT` is set), outputs JavaScript to return the selected value to the opener window.

**Parameters**

- None

**Return Value**

- `void` — Outputs HTML/JavaScript and calls `exit()`

**Inner Mechanisms**

1. Outputs `</div></form>` to close the content container and form.
2. If `CMS_IFC_SELECT` is set (selector mode):
   - Outputs JavaScript with `ifc_return()` function that:
     - Checks if the opener window is still open and at the same URL.
     - Retrieves the value and title from the selected element.
     - Gets the opener window's scroll position.
     - If `CMS_IFC_SELECT_ACTION` starts with `javascript:`, executes the action as JavaScript with `%return%`, `%title%`, `%left%`, and `%top%` placeholders replaced.
     - Otherwise, replaces the opener's location with the action URL (URL-encoded values).
   - Outputs `ifc_return_close()` to close the window if the opener is gone.
   - Outputs `ifc_return_clear()` to return an empty value.
   - Auto-closes if the target element doesn't exist and this is the top window.
3. Outputs `</body></html>` and calls `exit()`.

**Usage Example**

```php
$ifc = new ifc("Select an option", NULL, FALSE);
$ifc->set("Option 1", "button", "value1");
$ifc->set("Option 2", "button", "value2");
$ifc->close();
// When used as a selector, clicking a button will return the value to the opener
```


<!-- HASH:6344094153d5c94036a1a0145cb7d366 -->
