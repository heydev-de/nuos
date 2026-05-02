# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.flexview.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/%23system/lib.flexview.inc)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## FlexView Module

The `lib.flexview.inc` file defines the **FlexView** class, a core component of the NUOS platform for rendering hierarchical data structures in various visual formats (trees, columns, folders, paths, etc.). It provides a flexible way to display nested data with customizable appearance, behavior, and interaction patterns.

---

## Constants

| Name                          | Value | Description                                                                 |
|-------------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | `0`   | No entry type (default state).                                              |
| `CMS_FLEXVIEW_ENTRY_TYPE_BASE` | `1`   | Base/root entry of the hierarchy.                                          |
| `CMS_FLEXVIEW_ENTRY_TYPE_ENTRY`| `2`   | Regular entry in the hierarchy.                                            |
| `CMS_FLEXVIEW_ENTRY_TYPE_END`  | `3`   | End of hierarchy (cleanup/closing event).                                  |

---

## Class: `flexview_entry`

Represents a single node in the FlexView hierarchy with metadata about its position and state.

### Properties

| Name         | Default                     | Description                                                                 |
|--------------|-----------------------------|-----------------------------------------------------------------------------|
| `type`       | `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | Type of entry (base, entry, end).                                          |
| `index`      | `NULL`                      | Unique identifier of the entry.                                             |
| `parent`     | `NULL`                      | Parent entry index.                                                         |
| `position`   | `0`                         | Position among siblings (1-based).                                         |
| `count`      | `0`                         | Total number of siblings.                                                   |
| `subcount`   | `0`                         | Number of direct children.                                                  |
| `indentation`| `0`                         | Depth level in the hierarchy.                                               |
| `open`       | `FALSE`                     | Whether the entry is expanded (visible children).                           |

---

## Class: `flexview`

Core class for managing and rendering hierarchical data.

### Properties

| Name                  | Default / Initial Value                                                                 | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `object`              | `["" => ["#data" => ["#type" => "base"]]]`                                              | Internal data structure storing the hierarchy.                             |
| `icon_default`        | Loaded from `#system/flexview.icon` or fallback values                                  | Default icons for base, entry, container, and open container states.       |
| `value_function`      | `NULL`                                                                                  | Callback to transform entry indices before use.                            |
| `encoding_function`   | `__NAMESPACE__ . "\\x"` (XML escaping)                                                  | Callback to encode values (e.g., for HTML/JS output).                      |
| `display_function`    | `NULL`                                                                                  | Custom display callback (overrides default rendering).                     |
| `display`             | `"%checkbox%<a[ href=\"%action%\"][ class=\"%class%\"]>%mark%[%icon% ]%name%</a>"`      | Template string for rendering entries.                                     |
| `index`               | `""`                                                                                    | Currently selected entry.                                                  |
| `checkbox_identifier` | `NULL`                                                                                  | Name attribute for checkboxes (enables multi-selection).                   |
| `checkbox_index`      | `0`                                                                                     | Counter for generating unique checkbox names.                              |
| `checkbox_list`       | `NULL`                                                                                  | Array of pre-selected entries (keys are indices).                          |
| `mark`                | `NULL`                                                                                  | Custom marks (e.g., status icons) for entries.                             |
| `icon_custom`         | `NULL`                                                                                  | Custom icons for specific entry types/subtypes.                            |
| `action`              | `"%index%"`                                                                             | URL template for entry links (replaces `%index%` with entry value).        |
| `name_key`            | `"name"`                                                                                | Key in `#data` for the entry's display name.                               |
| `image_button_key`    | `"image_button"`                                                                        | Key for default image URL.                                                 |
| `image_hover_key`     | `"image_hover"`                                                                         | Key for hover-state image URL.                                             |
| `image_active_key`    | `"image_active"`                                                                        | Key for active-state image URL.                                            |
| `description_key`     | `"description"`                                                                         | Key for entry description (supports formatted text).                       |
| `base`                | `""`                                                                                    | Root entry of the hierarchy.                                               |
| `param`               | `NULL`                                                                                  | Internal storage for method-specific parameters.                           |

---

### Constructor: `__construct()`

**Purpose:**
Initializes the FlexView instance, loading default icons and setting the default encoding function.

**Inner Mechanisms:**
1. Loads default icons from `#system/flexview.icon` using the `data` class.
2. Falls back to hardcoded icon identifiers if no custom icons are defined.
3. Sets the default encoding function to `x()` (XML escaping).

**Usage:**
```php
$flexview = new flexview();
```

---

### Method: `set_value_function($callback_function)`

**Purpose:**
Sets a callback to transform entry indices before use (e.g., for URL generation or value encoding).

**Parameters:**

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting an entry index and returning a transformed value.       |

**Return Values:**
- None.

**Inner Mechanisms:**
- Validates that the callback is callable before assignment.

**Usage:**
```php
$flexview->set_value_function(function($index) { return "item_$index"; });
```

---

### Method: `set_encoding_function($callback_function)`

**Purpose:**
Sets a callback to encode values (e.g., for HTML/JS output).

**Parameters:**

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting a value and returning an encoded string.                |

**Return Values:**
- None.

**Inner Mechanisms:**
- Validates that the callback is callable before assignment.

**Usage:**
```php
$flexview->set_encoding_function(function($value) { return htmlspecialchars($value); });
```

---

### Method: `set_display_function($callback_function)`

**Purpose:**
Sets a custom display callback to override default rendering logic.

**Parameters:**

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting `(flexview $instance, string $index, bool $open)`.      |

**Return Values:**
- None.

**Inner Mechanisms:**
- Validates that the callback is callable before assignment.

**Usage:**
```php
$flexview->set_display_function(function($flexview, $index, $open) {
    echo "Custom display for $index";
});
```

---

### Method: `set_index($value)`

**Purpose:**
Sets the currently selected entry.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Entry index. Falls back to `base` if the index does not exist.             |

**Return Values:**
- None.

**Inner Mechanisms:**
- Validates the existence of the index; falls back to `base` if invalid.

**Usage:**
```php
$flexview->set_index("category_123");
```

---

### Method: `set_checkbox_identifier($value)`

**Purpose:**
Enables checkboxes for multi-selection and sets their name attribute.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Name attribute for checkboxes (e.g., `"selected_items"`).                  |

**Return Values:**
- None.

**Inner Mechanisms:**
- Resets the checkbox counter to `0`.

**Usage:**
```php
$flexview->set_checkbox_identifier("selected_items");
```

---

### Method: `set_checkbox_list($value)`

**Purpose:**
Pre-selects entries by providing an array of indices.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `array`  | Array of entry indices to pre-select.                                       |

**Return Values:**
- None.

**Inner Mechanisms:**
- Flips the array for faster lookup (keys become values).

**Usage:**
```php
$flexview->set_checkbox_list(["category_123", "category_456"]);
```

---

### Method: `get_checkbox($index)`

**Purpose:**
Generates an HTML checkbox for an entry.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Values:**
- `string`: HTML checkbox markup.

**Inner Mechanisms:**
1. Outputs JavaScript for checkbox behavior (once per FlexView instance).
2. Generates a unique name for the checkbox using the identifier and counter.
3. Checks if the entry is pre-selected.
4. Applies the `value_function` to the index if set.
5. Returns a `<label>` containing the checkbox and a spacer SVG.

**Usage:**
```php
echo $flexview->get_checkbox("category_123");
```

---

### Method: `set_mark($value)`

**Purpose:**
Sets custom marks (e.g., status icons) for entries.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `array`  | Associative array of entry indices to mark values (e.g., image URLs).      |

**Return Values:**
- None.

**Usage:**
```php
$flexview->set_mark(["category_123" => "status/active.svg"]);
```

---

### Method: `get_mark($index)`

**Purpose:**
Retrieves the mark for an entry.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Values:**
- `string|NULL`: Mark value or `NULL` if not set.

**Usage:**
```php
$mark = $flexview->get_mark("category_123");
```

---

### Method: `set_icon($value)`

**Purpose:**
Sets custom icons for specific entry types/subtypes.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `array`  | Associative array of icon values (keys: `#base`, `entry`, `container`, `+container`, or type/subtype strings). |

**Return Values:**
- None.

**Usage:**
```php
$flexview->set_icon([
    "#base" => "custom/base.svg",
    "category" => "custom/category.svg"
]);
```

---

### Method: `get_icon($index, $open = FALSE)`

**Purpose:**
Retrieves the icon for an entry, considering its type, subtype, and open state.

**Parameters:**

| Name    | Type      | Description                                                                 |
|---------|-----------|-----------------------------------------------------------------------------|
| `$index`| `string`  | Entry index.                                                                |
| `$open` | `bool`    | Whether the entry is expanded (affects container icons).                   |

**Return Values:**
- `string|NULL`: Icon identifier or `NULL` if no icon is set.

**Inner Mechanisms:**
1. Returns `NULL` if custom icons are disabled (`$this->icon_custom === FALSE`).
2. Prioritizes custom icons for the base entry, then type/subtype, then default icons.
3. For containers, uses `+container` icon if the entry is open.

**Usage:**
```php
$icon = $flexview->get_icon("category_123", TRUE);
```

---

### Method: `set_action($value)`

**Purpose:**
Sets the URL template for entry links.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Template string (e.g., `"edit.php?id=%index%"`).                           |

**Return Values:**
- None.

**Usage:**
```php
$flexview->set_action("edit.php?id=%index%");
```

---

### Method: `get_action($index)`

**Purpose:**
Generates the URL for an entry by replacing `%index%` in the action template.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Values:**
- `string`: Generated URL.

**Usage:**
```php
$url = $flexview->get_action("category_123");
```

---

### Method: `set_name_key($value)`

**Purpose:**
Sets the key in `#data` used to retrieve the entry's display name.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Key name (e.g., `"title"`).                                                |

**Return Values:**
- None.

**Usage:**
```php
$flexview->set_name_key("title");
```

---

### Method: `get_name($index)`

**Purpose:**
Retrieves the display name for an entry.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Values:**
- `string`: Display name or a fallback label if not found.

**Inner Mechanisms:**
1. Checks for the name in `#data` using the `name_key`.
2. Falls back to `CMS_L_FLEXVIEW_002` (localized "Root") for the base entry or `CMS_L_UNKNOWN` for others.

**Usage:**
```php
$name = $flexview->get_name("category_123");
```

---

### Method: `set_image_key($button, $hover, $active)`

**Purpose:**
Sets keys for image states (default, hover, active).

**Parameters:**

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$button`| `string` | Key for default image URL.                                                  |
| `$hover` | `string` | Key for hover-state image URL.                                              |
| `$active`| `string` | Key for active-state image URL.                                             |

**Return Values:**
- None.

**Usage:**
```php
$flexview->set_image_key("image", "image_hover", "image_active");
```

---

### Method: `get_image($index, $open = FALSE)`

**Purpose:**
Retrieves the image URL for an entry, considering its state.

**Parameters:**

| Name    | Type      | Description                                                                 |
|---------|-----------|-----------------------------------------------------------------------------|
| `$index`| `string`  | Entry index.                                                                |
| `$open` | `bool`    | Whether the entry is expanded (affects active-state image).                |

**Return Values:**
- `string|NULL`: Image URL or `NULL` if not found.

**Inner Mechanisms:**
1. Prioritizes the active-state image if the entry is open.
2. Falls back to the default image if no state-specific image is found.
3. Uses `translate_url()` to resolve logical image identifiers.

**Usage:**
```php
$image_url = $flexview->get_image("category_123", TRUE);
```

---

### Method: `set_base($value)`

**Purpose:**
Sets the root entry of the hierarchy.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Entry index. Falls back to `""` if the index does not exist.               |

**Return Values:**
- None.

**Usage:**
```php
$flexview->set_base("root_category");
```

---

### Method: `display($index, $open = FALSE)`

**Purpose:**
Renders an entry using the display template.

**Parameters:**

| Name    | Type      | Description                                                                 |
|---------|-----------|-----------------------------------------------------------------------------|
| `$index`| `string`  | Entry index.                                                                |
| `$open` | `bool`    | Whether the entry is expanded.                                              |

**Return Values:**
- None (outputs HTML directly).

**Inner Mechanisms:**
1. Uses the `display_function` if set, otherwise processes the `display` template.
2. Replaces placeholders (e.g., `%name%`, `%icon%`) with entry-specific values.
3. Handles conditional blocks (e.g., `[%icon% ]`) by removing them if the value is empty.
4. Applies the `encoding_function` to all dynamic values.

**Template Placeholders:**

| Placeholder          | Replaced With                                                                 |
|----------------------|-------------------------------------------------------------------------------|
| `%index%`            | Entry index (encoded).                                                        |
| `%action%`           | Entry URL.                                                                    |
| `%image_url%`        | Image URL (encoded).                                                          |
| `%image%`            | `<img>` tag for the image.                                                    |
| `%image_button_url%` | Default image URL (encoded).                                                  |
| `%image_button%`     | `<img>` tag for the default image.                                            |
| `%image_hover_url%`  | Hover-state image URL (encoded).                                              |
| `%image_hover%`      | `<img>` tag for the hover-state image.                                        |
| `%image_active_url%` | Active-state image URL (encoded).                                             |
| `%image_active%`     | `<img>` tag for the active-state image.                                       |
| `%checkbox%`         | Checkbox markup (if enabled).                                                 |
| `%mark%`             | Mark image (if set).                                                          |
| `%icon%`             | Icon image (if set).                                                          |
| `%name%`             | Entry name (wrapped in `<strong>` if the entry is selected).                  |
| `%description%`      | Formatted description (if set).                                               |
| `%description_plain%`| Plain-text description (if set).                                              |
| `%class%`            | CSS classes (`active` for selected entries, `container` for containers).     |

**Usage:**
```php
$flexview->display("category_123", TRUE);
```

---

### Method: `get_value($index)`

**Purpose:**
Transforms and encodes an entry index using the `value_function` and `encoding_function`.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Values:**
- `string`: Transformed and encoded index.

**Usage:**
```php
$value = $flexview->get_value("category_123");
```

---

### Method: `set($index, $data = NULL, $parent = "")`

**Purpose:**
Adds or updates an entry in the hierarchy.

**Parameters:**

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$index` | `string` | Entry index.                                                                |
| `$data`  | `array`  | Entry data (e.g., `["name" => "Example"]`).                                 |
| `$parent`| `string` | Parent entry index.                                                         |

**Return Values:**
- None.

**Inner Mechanisms:**
1. Stores the entry data in `#data`.
2. Sets the parent reference in `#parent`.
3. Adds the entry to the parent's child list.

**Usage:**
```php
$flexview->set("category_123", ["name" => "Example"], "root_category");
```

---

### Method: `get_path()`

**Purpose:**
Retrieves the path from the base entry to the currently selected entry.

**Return Values:**
- `array`: Array of entry indices from base to selected entry, or empty array if no path exists.

**Usage:**
```php
$path = $flexview->get_path();
```

---

### Method: `import_data(&$data)`

**Purpose:**
Imports hierarchical data from a `data` object.

**Parameters:**

| Name   | Type      | Description                                                                 |
|--------|-----------|-----------------------------------------------------------------------------|
| `$data`| `data`    | `data` object containing the hierarchy (must support `move()` and `get()`). |

**Return Values:**
- None.

**Inner Mechanisms:**
1. Iterates through the `data` object.
2. Handles container start/end markers to manage nesting.

**Usage:**
```php
$data = new data();
$data->load("#system/categories");
$flexview->import_data($data);
```

---

### Method: `import_database(&$result, $index_key = "id", $parent_key = "container")`

**Purpose:**
Imports hierarchical data from a MySQL result set.

**Parameters:**

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$result`    | `resource`| MySQL result set.                                                           |
| `$index_key` | `string`  | Column name for entry indices.                                              |
| `$parent_key`| `string`  | Column name for parent indices (use `NULL` for root entries).               |

**Return Values:**
- None.

**Inner Mechanisms:**
1. Fetches rows as associative arrays.
2. Uses `$index_key` and `$parent_key` to build the hierarchy.

**Usage:**
```php
$result = mysql_query("SELECT id, name, container FROM categories");
$flexview->import_database($result, "id", "container");
```

---

### Method: `show_custom($callback_function)`

**Purpose:**
Renders the hierarchy using a custom callback function.

**Parameters:**

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting a `flexview_entry` object.                              |

**Return Values:**
- None.

**Inner Mechanisms:**
1. Traverses the hierarchy depth-first.
2. Calls the callback for each entry (base, entry, end).
3. Passes a `flexview_entry` object with metadata about the current entry.

**Usage:**
```php
$flexview->show_custom(function($entry) {
    if ($entry->type == CMS_FLEXVIEW_ENTRY_TYPE_ENTRY) {
        echo "Entry: {$entry->index}<br>";
    }
});
```

---

### Method: `show_hierarchy($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "", $dragdrop_event_function = NULL, $dragdrop_type_accept = NULL)`

**Purpose:**
Renders the hierarchy as an interactive tree with drag-and-drop support.

**Parameters:**

| Name                     | Type       | Description                                                                 |
|--------------------------|------------|-----------------------------------------------------------------------------|
| `$index`                 | `string`   | Selected entry index.                                                       |
| `$action`                | `string`   | URL template for entry links.                                               |
| `$name_key`              | `string`   | Key for entry names.                                                        |
| `$mark`                  | `array`    | Custom marks for entries.                                                   |
| `$icon`                  | `array`    | Custom icons for entries.                                                   |
| `$base`                  | `string`   | Root entry index.                                                           |
| `$dragdrop_event_function`| `string`  | JavaScript function name for drag-and-drop events.                          |
| `$dragdrop_type_accept`  | `array`    | Rules for drag-and-drop target acceptance (e.g., `["type" => ["insert" => 1]]`). |

**Return Values:**
- None (outputs HTML and JavaScript).

**Inner Mechanisms:**
1. Outputs JavaScript for drag-and-drop behavior (once per FlexView instance).
2. Uses `_show_hierarchy()` to render the tree.
3. Supports dynamic expansion/collapse of containers.

**Usage:**
```php
$flexview->show_hierarchy(
    "category_123",
    "edit.php?id=%index%",
    "name",
    ["category_123" => "status/active.svg"],
    ["category" => "icons/category.svg"],
    "root_category",
    "handle_dragdrop"
);
```

---

### Method: `_show_hierarchy($flexview_entry)`

**Purpose:**
Internal callback for `show_hierarchy()` to render individual entries.

**Parameters:**

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Values:**
- None (outputs HTML directly).

**Inner Mechanisms:**
1. Handles indentation and nesting using `<div>` elements.
2. Renders expand/collapse toggles for containers.
3. Applies drag-and-drop attributes based on `dragdrop_type_accept`.

---

### Method: `show_tree($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose:**
Renders the hierarchy as a traditional tree with indentation and branch icons.

**Parameters:**

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$mark`     | `array`  | Custom marks for entries.                                                   |
| `$icon`     | `array`  | Custom icons for entries.                                                   |
| `$base`     | `string` | Root entry index.                                                           |

**Return Values:**
- None (outputs HTML directly).

**Usage:**
```php
$flexview->show_tree("category_123", "edit.php?id=%index%");
```

---

### Method: `_show_tree($flexview_entry)`

**Purpose:**
Internal callback for `show_tree()` to render individual entries.

**Parameters:**

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Values:**
- `bool`: `TRUE` to skip children, `FALSE` to continue traversal.

**Inner Mechanisms:**
1. Uses indentation images to visualize nesting.
2. Renders branch icons (`tree_branchopen.svg` or `tree_branchclose.svg`) for containers.

---

### Method: `show_target($index = "", $action = NULL, $action_insert = NULL, $action_append = NULL, $name_key = "name", $base = "", $type_insert = NULL, $subtype_insert = NULL, $type_append = NULL, $subtype_append = NULL)`

**Purpose:**
Renders the hierarchy as a target list for drag-and-drop operations, with insert/append actions.

**Parameters:**

| Name               | Type     | Description                                                                 |
|--------------------|----------|-----------------------------------------------------------------------------|
| `$index`           | `string` | Selected entry index.                                                       |
| `$action`          | `string` | URL template for entry links.                                               |
| `$action_insert`   | `string` | URL template for insert actions.                                            |
| `$action_append`   | `string` | URL template for append actions.                                            |
| `$name_key`        | `string` | Key for entry names.                                                        |
| `$base`            | `string` | Root entry index.                                                           |
| `$type_insert`     | `array`  | Allowed types for insert actions (e.g., `["category" => TRUE]`).            |
| `$subtype_insert`  | `array`  | Allowed subtypes for insert actions.                                        |
| `$type_append`     | `array`  | Allowed types for append actions.                                           |
| `$subtype_append`  | `array`  | Allowed subtypes for append actions.                                        |

**Return Values:**
- None (outputs HTML directly).

**Usage:**
```php
$flexview->show_target(
    "category_123",
    "edit.php?id=%index%",
    "insert.php?parent=%index%",
    "append.php?parent=%index%",
    "name",
    "root_category",
    ["category" => TRUE],
    NULL,
    ["category" => TRUE]
);
```

---

### Method: `_show_target($flexview_entry)`

**Purpose:**
Internal callback for `show_target()` to render individual entries.

**Parameters:**

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Values:**
- None (outputs HTML directly).

**Inner Mechanisms:**
1. Validates insert/append actions based on entry type/subtype.
2. Renders insert/append action links if allowed.

---

### Method: `show_path($index = "", $action = NULL, $name_key = "name", $delimiter = "›", $base = "")`

**Purpose:**
Renders the path from the base entry to the selected entry as a breadcrumb trail.

**Parameters:**

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$delimiter`| `string` | Separator between path elements.                                            |
| `$base`     | `string` | Root entry index.                                                           |

**Return Values:**
- None (outputs HTML directly).

**Usage:**
```php
$flexview->show_path("category_123", "edit.php?id=%index%");
```

---

### Method: `show_column($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose:**
Renders the hierarchy as a column view (e.g., macOS Finder).

**Parameters:**

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$mark`     | `array`  | Custom marks for entries.                                                   |
| `$icon`     | `array`  | Custom icons for entries.                                                   |
| `$base`     | `string` | Root entry index.                                                           |

**Return Values:**
- None (outputs HTML directly).

**Usage:**
```php
$flexview->show_column("category_123", "edit.php?id=%index%");
```

---

### Method: `_show_column()`

**Purpose:**
Internal method for `show_column()` to render the column view.

**Return Values:**
- None (outputs HTML directly).

**Inner Mechanisms:**
1. Renders the base entry and path to the selected entry.
2. Lists child entries in a column with checkboxes.

---

### Method: `show_folder($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose:**
Renders the hierarchy as a folder list (e.g., Windows Explorer).

**Parameters:**

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$mark`     | `array`  | Custom marks for entries.                                                   |
| `$icon`     | `array`  | Custom icons for entries.                                                   |
| `$base`     | `string` | Root entry index.                                                           |

**Return Values:**
- None (outputs HTML directly).

**Usage:**
```php
$flexview->show_folder("category_123", "edit.php?id=%index%");
```

---

### Method: `_show_folder()`

**Purpose:**
Internal method for `show_folder()` to render the folder list.

**Return Values:**
- None (outputs HTML directly).

**Inner Mechanisms:**
1. Renders the base entry and path to the selected entry.
2. Lists child entries with checkboxes.

---

### Method: `space($count = 1, $size = 18)`

**Purpose:**
Generates an invisible spacer image for layout purposes.

**Parameters:**

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$count` | `int`    | Number of spaces (width multiplier).                                        |
| `$size`  | `int`    | Base size of the spacer (pixels).                                           |

**Return Values:**
- `string`: HTML `<img>` tag for the spacer.

**Usage:**
```php
echo $flexview->space(2, 20);
```


<!-- HASH:b3ed24cd23cfeb2c3ce532b923e329c7 -->
