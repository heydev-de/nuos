# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.flexview.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.flexview.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## FlexView Module

The FlexView module provides a flexible hierarchical data visualization system for the PWNC Web Platform. It enables developers to render tree-like structures (e.g., file systems, navigation menus, or organizational charts) in multiple formats while maintaining consistent data handling and customizable display logic.

---

## Constants

| Name                          | Value | Description                                                                 |
|-------------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | `0`   | No entry type (default/uninitialized state).                               |
| `CMS_FLEXVIEW_ENTRY_TYPE_BASE` | `1`   | Base/root entry of the hierarchy.                                          |
| `CMS_FLEXVIEW_ENTRY_TYPE_ENTRY`| `2`   | Regular entry in the hierarchy.                                            |
| `CMS_FLEXVIEW_ENTRY_TYPE_END`  | `3`   | End marker (post-processing or cleanup).                                    |

---

## Class: `flexview_entry`

Represents a single node in the FlexView hierarchy with metadata about its position and state.

### Properties

| Name         | Default/Value               | Description                                                                 |
|--------------|-----------------------------|-----------------------------------------------------------------------------|
| `$type`      | `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | Type of entry (base, entry, or end).                                       |
| `$index`     | `NULL`                      | Unique identifier for the entry.                                            |
| `$parent`    | `NULL`                      | Parent entry's index.                                                       |
| `$position`  | `0`                         | Position among siblings (1-based).                                          |
| `$count`     | `0`                         | Total number of siblings.                                                   |
| `$subcount`  | `0`                         | Number of direct children.                                                  |
| `$indentation`| `0`                        | Depth level in the hierarchy.                                               |
| `$open`      | `FALSE`                     | Whether the entry is expanded (children visible).                           |

---

## Class: `flexview`

Core class for managing and rendering hierarchical data structures.

### Properties

| Name                  | Default/Value                                                                 | Description                                                                 |
|-----------------------|-------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `$object`             | `["" => ["#data" => ["#type" => "base"]]]`                                    | Internal data structure storing the hierarchy.                             |
| `$icon_default`       | Loaded from `#system/flexview.icon`                                           | Default icons for base, entry, container, and open container states.       |
| `$value_function`     | `NULL`                                                                        | Callback to transform entry indices (e.g., for encoding or formatting).    |
| `$encoding_function`  | `__NAMESPACE__ . "\\x"`                                                       | Default encoding function (XML escaping).                                  |
| `$display_function`   | `NULL`                                                                        | Custom display callback (overrides `$display` template).                   |
| `$display`            | `"%checkbox%<a[ href=\"%action%\"][ class=\"%class%\"]>%mark%[%icon% ]%name%</a>"` | Template string for rendering entries.                                     |
| `$index`              | `""`                                                                          | Currently selected entry.                                                  |
| `$checkbox_identifier`| `NULL`                                                                        | Name attribute for checkboxes (enables bulk selection).                    |
| `$checkbox_index`     | `0`                                                                           | Auto-incremented index for checkbox names.                                 |
| `$checkbox_list`      | `NULL`                                                                        | Array of pre-selected checkbox values.                                     |
| `$mark`               | `NULL`                                                                        | Custom markers (e.g., status indicators) keyed by entry index.             |
| `$icon_custom`        | `NULL`                                                                        | Custom icons keyed by type/subtype.                                        |
| `$action`             | `"%index%"`                                                                   | URL template for entry actions (replaces `%index%` with entry value).      |
| `$name_key`           | `"name"`                                                                      | Key in `#data` for the entry's display name.                               |
| `$image_button_key`   | `"image_button"`                                                              | Key for default image URL.                                                 |
| `$image_hover_key`    | `"image_hover"`                                                               | Key for hover-state image URL.                                             |
| `$image_active_key`   | `"image_active"`                                                              | Key for active-state image URL.                                            |
| `$description_key`    | `"description"`                                                               | Key for entry description (supports formatted text).                       |
| `$base`               | `""`                                                                          | Root entry of the hierarchy.                                               |
| `$param`              | `NULL`                                                                        | Internal parameter storage (e.g., for drag-and-drop configuration).        |

---

### Constructor: `__construct()`

**Purpose**:
Initializes the FlexView instance with default icons and encoding function.

**Inner Mechanisms**:
1. Loads default icons from `#system/flexview.icon` using the `data` class.
2. Falls back to hardcoded paths if icons are not configured.
3. Sets the default encoding function to `x()` (XML escaping).

**Usage Example**:
```php
$flexview = new flexview();
```

---

### Method: `set_value_function($callback_function)`

**Purpose**:
Sets a callback to transform entry indices before rendering (e.g., for URL encoding or formatting).

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting an entry index and returning a transformed value.        |

**Return Value**:
`void`

**Inner Mechanisms**:
- Validates the callback using `is_callable()` before assignment.

**Usage Example**:
```php
$flexview->set_value_function(function($index) {
    return urlencode($index); // Encode indices for URLs
});
```

---

### Method: `set_encoding_function($callback_function)`

**Purpose**:
Sets a callback to escape/encode entry indices (e.g., for HTML, XML, or SQL).

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting a string and returning an encoded string.                |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_encoding_function(function($value) {
    return htmlspecialchars($value, ENT_QUOTES, 'UTF-8');
});
```

---

### Method: `set_display_function($callback_function)`

**Purpose**:
Overrides the default display logic with a custom callback.

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting `(flexview $instance, string $index, bool $open)`.       |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_display_function(function($flexview, $index, $open) {
    echo "<div class='custom-entry'>" . $flexview->get_name($index) . "</div>";
});
```

---

### Method: `set_index($value)`

**Purpose**:
Sets the currently selected entry.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Entry index. Falls back to `$base` if invalid.                              |

**Return Value**:
`void`

**Inner Mechanisms**:
- Validates the index against `$this->object`.
- Falls back to `$this->base` if the index is invalid.

**Usage Example**:
```php
$flexview->set_index("folder1/subfolder");
```

---

### Method: `set_checkbox_identifier($value)`

**Purpose**:
Enables checkboxes for bulk selection and sets their name attribute.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Name attribute for checkboxes (e.g., `"selected_items"`).                   |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_checkbox_identifier("selected_files");
```

---

### Method: `set_checkbox_list($value)`

**Purpose**:
Pre-selects checkboxes based on an array of entry indices.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `array`  | Array of indices to pre-select.                                             |

**Return Value**:
`void`

**Inner Mechanisms**:
- Flips the array for O(1) lookup during rendering.

**Usage Example**:
```php
$flexview->set_checkbox_list(["file1.txt", "file2.txt"]);
```

---

### Method: `get_checkbox($index)`

**Purpose**:
Generates an HTML checkbox for an entry.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Value**:
`string` HTML checkbox input, or `NULL` if checkboxes are disabled.

**Inner Mechanisms**:
- Uses `$value_function` to transform the index if set.
- Auto-increments `$checkbox_index` for unique names.

**Usage Example**:
```php
echo $flexview->get_checkbox("file1.txt");
// Output: <input name="selected_files[0]" type="checkbox" value="file1.txt" class="fv-cb">
```

---

### Method: `set_mark($value)`

**Purpose**:
Sets custom markers (e.g., status icons) for entries.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `array`  | Associative array of `[index => marker]` pairs.                             |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_mark([
    "file1.txt" => "icons/read_only.svg",
    "file2.txt" => "icons/modified.svg"
]);
```

---

### Method: `get_mark($index)`

**Purpose**:
Retrieves the marker for an entry.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Value**:
`string|null` Marker path or `NULL` if none exists.

---

### Method: `set_icon($value)`

**Purpose**:
Sets custom icons for entries based on type/subtype.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `array`  | Associative array of `[type => icon_path]` or `["+type" => open_icon_path]`.|

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_icon([
    "folder" => "icons/folder.svg",
    "+folder" => "icons/folder_open.svg",
    "file" => "icons/file.svg"
]);
```

---

### Method: `get_icon($index, $open = FALSE)`

**Purpose**:
Retrieves the icon for an entry, considering its type, subtype, and open state.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |
| `$open` | `bool`   | Whether the entry is expanded.                                              |

**Return Value**:
`string|null` Icon path or `NULL` if no icon is set.

**Inner Mechanisms**:
1. Checks for custom icons in this order:
   - Base entry (`#base`).
   - Open-state subtype (`+subtype`).
   - Subtype.
   - Open-state type (`+type`).
   - Type.
2. Falls back to default icons for containers/entries.

---

### Method: `set_action($value)`

**Purpose**:
Sets the URL template for entry actions.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Template string (e.g., `"edit.php?id=%index%"`).                            |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_action("edit.php?id=%index%");
```

---

### Method: `get_action($index)`

**Purpose**:
Generates the action URL for an entry by replacing `%index%` in the template.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Value**:
`string` Resolved URL.

---

### Method: `set_name_key($value)`

**Purpose**:
Sets the key in `#data` used to retrieve the entry's display name.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Key name (e.g., `"title"`).                                                 |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_name_key("title");
```

---

### Method: `get_name($index)`

**Purpose**:
Retrieves the display name for an entry.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Value**:
`string` Display name or a fallback label.

**Inner Mechanisms**:
- Falls back to `CMS_L_FLEXVIEW_002` (localized "Root") for the base entry or `CMS_L_UNKNOWN` for others.

---

### Method: `set_image_key($button, $hover, $active)`

**Purpose**:
Sets keys for image states (button, hover, active).

**Parameters**:

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$button`| `string` | Key for default image.                                                      |
| `$hover` | `string` | Key for hover-state image.                                                  |
| `$active`| `string` | Key for active-state image.                                                 |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_image_key("img_default", "img_hover", "img_active");
```

---

### Method: `get_image($index, $open = FALSE)`

**Purpose**:
Retrieves the image URL for an entry based on its state.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |
| `$open` | `bool`   | Whether the entry is expanded.                                              |

**Return Value**:
`string|null` Image URL or `NULL` if none exists.

**Inner Mechanisms**:
- Prioritizes active-state images if `$open` is `TRUE`.
- Falls back to hover-state, then default images.

---

### Method: `set_base($value)`

**Purpose**:
Sets the root entry of the hierarchy.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value`| `string` | Entry index. Falls back to `""` if invalid.                                 |

**Return Value**:
`void`

---

### Method: `display($index, $open = FALSE)`

**Purpose**:
Renders an entry using the configured display template.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |
| `$open` | `bool`   | Whether the entry is expanded.                                              |

**Return Value**:
`void` (Outputs HTML directly.)

**Inner Mechanisms**:
1. Uses `$display_function` if set (overrides template).
2. Replaces placeholders in `$display` with entry-specific values (e.g., `%name%`, `%icon%`).
3. Handles conditional blocks (e.g., `[ class="%class%"]` omits the attribute if `%class%` is empty).

**Usage Example**:
```php
$flexview->display("folder1", TRUE);
// Output: <a href="folder1" class="active container">[icon] Folder 1</a>
```

---

### Method: `get_value($index)`

**Purpose**:
Transforms an entry index using `$value_function` and `$encoding_function`.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Entry index.                                                                |

**Return Value**:
`string` Transformed value.

---

### Method: `set($index, $data = NULL, $parent = "")`

**Purpose**:
Adds or updates an entry in the hierarchy.

**Parameters**:

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$index` | `string` | Entry index.                                                                |
| `$data`  | `array`  | Entry data (e.g., `["name" => "Folder 1", "#type" => "folder"]`).            |
| `$parent`| `string` | Parent entry index.                                                         |

**Return Value**:
`void`

**Inner Mechanisms**:
- Updates `$this->object` with bidirectional references (parent → child and child → parent).

**Usage Example**:
```php
$flexview->set("folder1", ["name" => "Folder 1", "#type" => "folder"], "");
$flexview->set("file1.txt", ["name" => "File 1"], "folder1");
```

---

### Method: `get_path()`

**Purpose**:
Retrieves the path from the base entry to the current index.

**Return Value**:
`array` Ordered list of indices from base to current entry.

**Usage Example**:
```php
$flexview->set_index("folder1/subfolder");
$path = $flexview->get_path();
// Result: ["", "folder1", "folder1/subfolder"]
```

---

### Method: `import_data(&$data)`

**Purpose**:
Imports hierarchical data from a `data` object.

**Parameters**:

| Name   | Type      | Description                                                                 |
|--------|-----------|-----------------------------------------------------------------------------|
| `$data`| `data`    | `data` object with entries marked as `"container"` or `"/container"`.       |

**Return Value**:
`void`

**Inner Mechanisms**:
- Processes entries sequentially, tracking container nesting with a stack.

**Usage Example**:
```php
$data = new data();
$data->set("folder1", ["#type" => "container", "name" => "Folder 1"]);
$data->set("file1.txt", ["name" => "File 1"], "folder1");
$flexview->import_data($data);
```

---

### Method: `import_database(&$result, $index_key = "id", $parent_key = "container")`

**Purpose**:
Imports hierarchical data from a MySQL result set.

**Parameters**:

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$result`    | `resource`| MySQL result resource.                                                      |
| `$index_key` | `string`  | Column name for entry indices.                                              |
| `$parent_key`| `string`  | Column name for parent indices.                                             |

**Return Value**:
`void`

**Usage Example**:
```php
$result = mysql_query("SELECT id, name, parent_id FROM folders");
$flexview->import_database($result, "id", "parent_id");
```

---

### Method: `show_custom($callback_function)`

**Purpose**:
Renders the hierarchy using a custom callback for each entry.

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting a `flexview_entry` object. Returns `TRUE` to skip children.|

**Return Value**:
`void`

**Inner Mechanisms**:
- Traverses the hierarchy depth-first.
- Passes a `flexview_entry` object to the callback for each node.

**Usage Example**:
```php
$flexview->show_custom(function($entry) {
    echo str_repeat("&nbsp;", $entry->indentation * 2) . $entry->index . "<br>";
    return FALSE; // Continue traversal
});
```

---

### Method: `show_hierarchy($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "", $dragdrop_event_function = NULL, $dragdrop_type_accept = NULL)`

**Purpose**:
Renders the hierarchy as an interactive collapsible tree with drag-and-drop support.

**Parameters**:

| Name                     | Type       | Description                                                                 |
|--------------------------|------------|-----------------------------------------------------------------------------|
| `$index`                 | `string`   | Selected entry index.                                                       |
| `$action`                | `string`   | URL template for entry actions.                                             |
| `$name_key`              | `string`   | Key for display names.                                                      |
| `$mark`                  | `array`    | Custom markers.                                                             |
| `$icon`                  | `array`    | Custom icons.                                                               |
| `$base`                  | `string`   | Root entry index.                                                           |
| `$dragdrop_event_function`| `string`  | JavaScript callback for drag-and-drop events.                               |
| `$dragdrop_type_accept`  | `array`    | Drag-and-drop type acceptance rules.                                        |

**Return Value**:
`void`

**Inner Mechanisms**:
- Generates JavaScript to initialize drag-and-drop functionality.
- Uses `_show_hierarchy()` for rendering.

**Usage Example**:
```php
$flexview->show_hierarchy(
    "folder1",
    "edit.php?id=%index%",
    "name",
    ["folder1" => "icons/important.svg"],
    ["folder" => "icons/folder.svg"],
    "",
    "handleDragDrop",
    ["folder" => ["index" => 1, "insert" => 1, "append" => 1]]
);
```

---

### Method: `_show_hierarchy($flexview_entry)`

**Purpose**:
Internal callback for `show_hierarchy()` to render individual entries.

**Parameters**:

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Value**:
`void`

**Inner Mechanisms**:
- Handles indentation and nesting with `<div>` elements.
- Adds checkboxes, icons, and action links.
- Configures drag-and-drop targets/sources based on `$dragdrop_type_accept`.

---

### Method: `show_tree($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a static tree with ASCII-style connectors.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry actions.                                             |
| `$name_key` | `string` | Key for display names.                                                      |
| `$mark`     | `array`  | Custom markers.                                                             |
| `$icon`     | `array`  | Custom icons.                                                               |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_tree("folder1", "edit.php?id=%index%");
```

---

### Method: `_show_tree($flexview_entry)`

**Purpose**:
Internal callback for `show_tree()` to render individual entries.

**Parameters**:

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Value**:
`bool` `TRUE` to skip children, `FALSE` to continue traversal.

**Inner Mechanisms**:
- Uses SVG-based indentation and branch connectors.
- Tracks indentation state with a static string.

---

### Method: `show_target($index = "", $action = NULL, $action_insert = NULL, $action_append = NULL, $name_key = "name", $base = "", $type_insert = NULL, $subtype_insert = NULL, $type_append = NULL, $subtype_append = NULL)`

**Purpose**:
Renders the hierarchy as a target for drag-and-drop operations with insert/append actions.

**Parameters**:

| Name               | Type     | Description                                                                 |
|--------------------|----------|-----------------------------------------------------------------------------|
| `$index`           | `string` | Selected entry index.                                                       |
| `$action`          | `string` | URL template for entry actions.                                             |
| `$action_insert`   | `string` | URL template for insert actions.                                            |
| `$action_append`   | `string` | URL template for append actions.                                            |
| `$name_key`        | `string` | Key for display names.                                                      |
| `$base`            | `string` | Root entry index.                                                           |
| `$type_insert`     | `array`  | Type/subtype rules for insert actions.                                      |
| `$subtype_insert`  | `array`  | Subtype rules for insert actions.                                           |
| `$type_append`     | `array`  | Type/subtype rules for append actions.                                      |
| `$subtype_append`  | `array`  | Subtype rules for append actions.                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_target(
    "folder1",
    "edit.php?id=%index%",
    "insert.php?parent=%index%",
    "append.php?parent=%index%",
    "name",
    "",
    ["folder" => TRUE],
    NULL,
    ["folder" => TRUE],
    NULL
);
```

---

### Method: `_show_target($flexview_entry)`

**Purpose**:
Internal callback for `show_target()` to render individual entries.

**Parameters**:

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Value**:
`void`

**Inner Mechanisms**:
- Validates insert/append actions against type/subtype rules.
- Renders collapsible containers with action links.

---

### Method: `show_path($index = "", $action = NULL, $name_key = "name", $delimiter = "›", $base = "")`

**Purpose**:
Renders a breadcrumb trail from the base entry to the current index.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry actions.                                             |
| `$name_key` | `string` | Key for display names.                                                      |
| `$delimiter`| `string` | Separator between breadcrumb items.                                         |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_path("folder1/subfolder", "edit.php?id=%index%");
// Output: Root › Folder 1 › Subfolder
```

---

### Method: `show_column($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a single column of entries with a breadcrumb trail.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry actions.                                             |
| `$name_key` | `string` | Key for display names.                                                      |
| `$mark`     | `array`  | Custom markers.                                                             |
| `$icon`     | `array`  | Custom icons.                                                               |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_column("folder1", "edit.php?id=%index%");
```

---

### Method: `show_folder($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a folder view with a breadcrumb trail.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry actions.                                             |
| `$name_key` | `string` | Key for display names.                                                      |
| `$mark`     | `array`  | Custom markers.                                                             |
| `$icon`     | `array`  | Custom icons.                                                               |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

---

### Method: `_show_breadcrumb(&$index)`

**Purpose**:
Internal helper to render a breadcrumb trail for the current entry.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `string` | Reference to the current entry index.                                       |

**Return Value**:
`bool` `TRUE` if a breadcrumb was rendered, `FALSE` otherwise.

---

### Method: `space($count = 1, $size = 20)`

**Purpose**:
Generates an SVG-based spacer for indentation.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$count`| `int`    | Number of spaces.                                                           |
| `$size` | `int`    | Width of each space in pixels.                                              |

**Return Value**:
`string` SVG HTML.

**Usage Example**:
```php
echo $flexview->space(3);
// Output: <svg xmlns="http://www.w3.org/2000/svg" width="60" height="20"></svg>
```


<!-- HASH:a5ce2b756efdd7b53a3b8bcea667fb98 -->
