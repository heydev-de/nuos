# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.flexview.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## FlexView Module (`lib.flexview.inc`)

The FlexView module provides a flexible, hierarchical data visualization system for the NUOS platform. It enables developers to render tree-like structures (e.g., navigation menus, file explorers, or organizational charts) with customizable display logic, interactive elements, and drag-and-drop support.

---

## Constants

| Name                          | Value | Description                                                                 |
|-------------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | `0`   | Default entry type (unused).                                                |
| `CMS_FLEXVIEW_ENTRY_TYPE_BASE` | `1`   | Root/base entry of the hierarchy.                                           |
| `CMS_FLEXVIEW_ENTRY_TYPE_ENTRY`| `2`   | Regular entry in the hierarchy.                                             |
| `CMS_FLEXVIEW_ENTRY_TYPE_END`  | `3`   | Terminal entry (signals end of traversal).                                  |

---

## Class: `flexview_entry`

Represents a single node in the FlexView hierarchy with metadata for rendering and traversal.

### Properties

| Name          | Default                     | Description                                                                 |
|---------------|-----------------------------|-----------------------------------------------------------------------------|
| `$type`       | `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | Entry type (base/entry/end).                                               |
| `$index`      | `NULL`                      | Unique identifier for the entry.                                            |
| `$parent`     | `NULL`                      | Parent entry's index.                                                       |
| `$position`   | `0`                         | Position among siblings (1-based).                                          |
| `$count`      | `0`                         | Total siblings (including itself).                                           |
| `$subcount`   | `0`                         | Number of direct children.                                                  |
| `$indentation`| `0`                         | Depth level in the hierarchy.                                               |
| `$open`       | `FALSE`                     | Whether the entry is expanded (children visible).                           |

---

## Class: `flexview`

Core class for managing and rendering hierarchical data structures.

### Properties

| Name                  | Default/Value                                                                 | Description                                                                 |
|-----------------------|-------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `$object`             | `["" => ["#data" => ["#type" => "base"]]]`                                    | Internal data structure storing the hierarchy.                              |
| `$icon_default`       | System-loaded or fallback icons                                               | Default icons for base/entry/container states.                              |
| `$value_function`     | `NULL`                                                                        | Callback to transform entry indices (e.g., for encoding).                   |
| `$encoding_function`  | `__NAMESPACE__ . "\\x"`                                                       | Default: XML-escaping function.                                             |
| `$display_function`   | `NULL`                                                                        | Custom rendering callback (overrides `$display` template).                  |
| `$display`            | `"%checkbox%<a[...]>%mark%[%icon% ]%name%</a>"`                              | Template string for entry rendering (placeholders: `%name%`, `%icon%`, etc).|
| `$index`              | `""`                                                                          | Currently selected entry.                                                   |
| `$checkbox_*`         | `NULL`/`0`/`[]`                                                               | Checkbox state management (identifier, index, selected list).               |
| `$mark`               | `NULL`                                                                        | Custom markers (e.g., badges) for entries.                                  |
| `$icon_custom`        | `NULL`                                                                        | Custom icons for specific entry types/subtypes.                             |
| `$action`             | `"%index%"`                                                                   | URL template for entry actions (placeholder: `%index%`).                    |
| `$name_key`           | `"name"`                                                                      | Key in `#data` for the entry's display name.                                |
| `$image_*_key`        | `"image_button"`, `"image_hover"`, `"image_active"`                           | Keys for image states in `#data`.                                           |
| `$description_key`    | `"description"`                                                               | Key for entry descriptions.                                                 |
| `$base`               | `""`                                                                          | Root entry of the hierarchy.                                                |
| `$param`              | `NULL`                                                                        | Internal parameter storage (e.g., for drag-and-drop).                       |

---

### Constructor: `__construct()`

**Purpose**:
Initializes the FlexView instance with default icons and encoding function.

**Inner Mechanisms**:
1. Loads default icons from `#system/flexview.icon` data file.
2. Falls back to built-in icon paths if system icons are unavailable.
3. Sets the default encoding function to `x()` (XML-escaping).

**Usage**:
```php
$flexview = new \cms\flexview();
```

---

### Method: `set_value_function($callback_function)`

**Purpose**:
Sets a callback to transform entry indices before rendering (e.g., for encoding or formatting).

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting an index and returning a transformed value.              |

**Return**: `void`

**Usage**:
```php
$flexview->set_value_function(function($index) { return "ID_" . $index; });
```

---

### Method: `set_encoding_function($callback_function)`

**Purpose**:
Sets a callback to escape/encode entry values (e.g., for HTML/XML output).

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting a string and returning an escaped version.               |

**Return**: `void`

**Usage**:
```php
$flexview->set_encoding_function(function($value) { return htmlspecialchars($value); });
```

---

### Method: `set_display_function($callback_function)`

**Purpose**:
Overrides the default template-based rendering with a custom callback.

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting `(flexview $this, string $index, bool $open)`.           |

**Return**: `void`

**Usage**:
```php
$flexview->set_display_function(function($flexview, $index, $open) {
    echo "<div class='custom-entry'>" . $flexview->get_name($index) . "</div>";
});
```

---

### Method: `set_index($value)`

**Purpose**:
Sets the currently selected entry, falling back to the base if invalid.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$value`| `string` | Entry index to select.          |

**Return**: `void`

**Inner Mechanisms**:
- Validates the index against `$this->object`.
- Falls back to `$this->base` if the index is invalid.

---

### Method: `set_checkbox_identifier($value)`

**Purpose**:
Enables checkboxes for entries with the given form field name.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$value`| `string` | Form field name for checkboxes. |

**Return**: `void`

**Usage**:
```php
$flexview->set_checkbox_identifier("selected_items");
```

---

### Method: `set_checkbox_list($value)`

**Purpose**:
Pre-selects checkboxes based on a list of entry indices.

**Parameters**:

| Name    | Type       | Description                     |
|---------|------------|---------------------------------|
| `$value`| `array`    | Array of indices to pre-select. |

**Return**: `void`

**Inner Mechanisms**:
- Flips the array for O(1) lookup during rendering.

---

### Method: `get_checkbox($index)`

**Purpose**:
Generates an HTML checkbox for an entry, with JavaScript for visual feedback.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `string` | Entry index.                    |

**Return**: `string` HTML checkbox markup.

**Inner Mechanisms**:
1. Outputs JavaScript for checkbox styling on first call.
2. Uses `$this->value_function` to transform the index if set.
3. Generates a hidden SVG spacer for consistent layout.

**Usage**:
```php
echo $flexview->get_checkbox("entry_123");
```

---

### Method: `set_mark($value)`

**Purpose**:
Sets custom markers (e.g., badges) for entries.

**Parameters**:

| Name    | Type       | Description                     |
|---------|------------|---------------------------------|
| `$value`| `array`    | Associative array of `index => marker`. |

**Return**: `void`

---

### Method: `get_mark($index)`

**Purpose**:
Retrieves the marker for an entry.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `string` | Entry index.                    |

**Return**: `string|null` Marker value or `NULL`.

---

### Method: `set_icon($value)`

**Purpose**:
Sets custom icons for specific entry types/subtypes.

**Parameters**:

| Name    | Type       | Description                                                                 |
|---------|------------|-----------------------------------------------------------------------------|
| `$value`| `array`    | Associative array of `type => icon_path` (e.g., `["container" => "folder"]`).|

**Return**: `void`

**Usage**:
```php
$flexview->set_icon([
    "#base" => "custom/base_icon",
    "entry" => "custom/entry_icon"
]);
```

---

### Method: `get_icon($index, $open = FALSE)`

**Purpose**:
Retrieves the icon for an entry, considering its type, subtype, and open state.

**Parameters**:

| Name    | Type      | Description                     |
|---------|-----------|---------------------------------|
| `$index`| `string`  | Entry index.                    |
| `$open` | `bool`    | Whether the entry is expanded.  |

**Return**: `string` Icon path.

**Inner Mechanisms**:
1. Prioritizes custom icons over defaults.
2. Checks `#type` and `#subtype` in `#data` for icon resolution.
3. Falls back to default icons if no custom match is found.

---

### Method: `set_action($value)`

**Purpose**:
Sets the URL template for entry actions (e.g., navigation).

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$value`| `string` | Template with `%index%` placeholder. |

**Return**: `void`

---

### Method: `get_action($index)`

**Purpose**:
Generates the action URL for an entry by replacing `%index%` in the template.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `string` | Entry index.                    |

**Return**: `string` Resolved URL.

---

### Method: `set_name_key($value)`

**Purpose**:
Sets the key in `#data` used to retrieve the entry's display name.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$value`| `string` | Key name (e.g., `"title"`).     |

**Return**: `void`

---

### Method: `get_name($index)`

**Purpose**:
Retrieves the display name for an entry, falling back to localized strings if missing.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `string` | Entry index.                    |

**Return**: `string` Display name.

---

### Method: `set_image_key($button, $hover, $active)`

**Purpose**:
Sets keys in `#data` for image states (button, hover, active).

**Parameters**:

| Name      | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$button` | `string` | Key for default image.          |
| `$hover`  | `string` | Key for hover image.            |
| `$active` | `string` | Key for active (selected) image.|

**Return**: `void`

---

### Method: `get_image($index, $open = FALSE)`

**Purpose**:
Retrieves the image URL for an entry based on its state.

**Parameters**:

| Name    | Type      | Description                     |
|---------|-----------|---------------------------------|
| `$index`| `string`  | Entry index.                    |
| `$open` | `bool`    | Whether the entry is expanded.  |

**Return**: `string|null` Image URL or `NULL`.

**Inner Mechanisms**:
1. Prioritizes the active image if `$open` is `TRUE`.
2. Falls back to the button image if no active image is set.

---

### Method: `set_base($value)`

**Purpose**:
Sets the root entry of the hierarchy.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$value`| `string` | Entry index.                    |

**Return**: `void`

**Inner Mechanisms**:
- Validates the index against `$this->object`.
- Falls back to an empty string if invalid.

---

### Method: `display($index, $open = FALSE)`

**Purpose**:
Renders an entry using the configured template or custom display function.

**Parameters**:

| Name    | Type      | Description                     |
|---------|-----------|---------------------------------|
| `$index`| `string`  | Entry index.                    |
| `$open` | `bool`    | Whether the entry is expanded.  |

**Return**: `void`

**Inner Mechanisms**:
1. Uses `$this->display_function` if set, otherwise processes the template.
2. Replaces placeholders (e.g., `%name%`, `%icon%`) with resolved values.
3. Omits optional sections (e.g., `[%icon% ]`) if the value is empty.

**Template Placeholders**:

| Placeholder          | Description                                                                 |
|----------------------|-----------------------------------------------------------------------------|
| `%index%`            | Entry index (escaped).                                                      |
| `%action%`           | Action URL.                                                                 |
| `%name%`             | Display name (strong if selected).                                          |
| `%icon%`             | Icon image.                                                                 |
| `%mark%`             | Marker image.                                                               |
| `%checkbox%`         | Checkbox (if enabled).                                                      |
| `%description%`      | Parsed description (HTML).                                                  |
| `%description_plain%`| Plain-text description.                                                     |
| `%class%`            | CSS classes (e.g., `"active container"`).                                   |

---

### Method: `get_value($index)`

**Purpose**:
Transforms and encodes an entry index using the configured callbacks.

**Parameters**:

| Name    | Type     | Description                     |
|---------|----------|---------------------------------|
| `$index`| `string` | Entry index.                    |

**Return**: `string` Transformed and encoded value.

---

### Method: `set($index, $data = NULL, $parent = "")`

**Purpose**:
Adds or updates an entry in the hierarchy.

**Parameters**:

| Name      | Type       | Description                     |
|-----------|------------|---------------------------------|
| `$index`  | `string`   | Entry index.                    |
| `$data`   | `array`    | Entry data (e.g., `["name" => "Home"]`). |
| `$parent` | `string`   | Parent entry index.             |

**Return**: `void`

**Inner Mechanisms**:
1. Stores data in `$this->object[$index]["#data"]`.
2. Links the entry to its parent in `$this->object[$parent][$index]`.

---

### Method: `get_path()`

**Purpose**:
Retrieves the path from the base to the current entry as an array of indices.

**Return**: `array` Path indices (e.g., `["base", "parent", "current"]`).

---

### Method: `import_data(&$data)`

**Purpose**:
Imports hierarchical data from a `data` object (e.g., loaded from a file).

**Parameters**:

| Name    | Type      | Description                     |
|---------|-----------|---------------------------------|
| `&$data`| `data`    | Data object with `#type` markers.|

**Return**: `void`

**Inner Mechanisms**:
1. Traverses the data object, handling `container` and `/container` markers.
2. Uses a stack to manage parent-child relationships.

---

### Method: `import_database(&$result, $index_key = "id", $parent_key = "container")`

**Purpose**:
Imports hierarchical data from a MySQL result set.

**Parameters**:

| Name          | Type      | Description                     |
|---------------|-----------|---------------------------------|
| `&$result`    | `resource`| MySQL result resource.          |
| `$index_key`  | `string`  | Column name for entry indices.  |
| `$parent_key` | `string`  | Column name for parent indices. |

**Return**: `void`

**Inner Mechanisms**:
1. Fetches rows as associative arrays.
2. Uses `$index_key` and `$parent_key` to build the hierarchy.

---

### Method: `show_custom($callback_function)`

**Purpose**:
Traverses the hierarchy and invokes a callback for each entry.

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function`| `callable` | Function accepting a `flexview_entry` object.                              |

**Return**: `void`

**Inner Mechanisms**:
1. Uses depth-first traversal to visit entries.
2. Populates `flexview_entry` with metadata (position, indentation, etc.).
3. Calls the callback for `BASE`, `ENTRY`, and `END` events.

**Usage**:
```php
$flexview->show_custom(function($entry) {
    if ($entry->type == CMS_FLEXVIEW_ENTRY_TYPE_ENTRY) {
        echo "Entry: " . $entry->index . "<br>";
    }
});
```

---

### Method: `show_hierarchy($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "", $dragdrop_event_function = NULL, $dragdrop_type_accept = NULL)`

**Purpose**:
Renders the hierarchy as an interactive tree with drag-and-drop support.

**Parameters**:

| Name                      | Type       | Description                                                                 |
|---------------------------|------------|-----------------------------------------------------------------------------|
| `$index`                  | `string`   | Selected entry index.                                                       |
| `$action`                 | `string`   | URL template for entry actions.                                             |
| `$name_key`               | `string`   | Key for display names.                                                      |
| `$mark`                   | `array`    | Custom markers for entries.                                                 |
| `$icon`                   | `array`    | Custom icons for entry types.                                               |
| `$base`                   | `string`   | Root entry index.                                                           |
| `$dragdrop_event_function`| `string`   | JavaScript callback for drag-and-drop events.                               |
| `$dragdrop_type_accept`   | `array`    | Rules for drag-and-drop target acceptance (e.g., `["type" => ["insert" => 1]]`). |

**Return**: `void`

**Inner Mechanisms**:
1. Outputs JavaScript for drag-and-drop initialization on first call.
2. Uses `_show_hierarchy` for rendering.

---

### Method: `_show_hierarchy($flexview_entry)`

**Purpose**:
Internal callback for `show_hierarchy` to render individual entries.

**Parameters**:

| Name               | Type              | Description                     |
|--------------------|-------------------|---------------------------------|
| `$flexview_entry`  | `flexview_entry`  | Entry metadata.                 |

**Return**: `void`

**Inner Mechanisms**:
1. Handles indentation and sublist management.
2. Renders checkboxes, icons, and action links.
3. Adds drag-and-drop attributes (`data-fv-hir-type`, `data-fv-hir-accept`).

---

### Method: `show_tree($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a traditional tree with indentation and branch icons.

**Parameters**:

| Name        | Type     | Description                     |
|-------------|----------|---------------------------------|
| `$index`    | `string` | Selected entry index.           |
| `$action`   | `string` | URL template for entry actions. |
| `$name_key` | `string` | Key for display names.          |
| `$mark`     | `array`  | Custom markers for entries.     |
| `$icon`     | `array`  | Custom icons for entry types.   |
| `$base`     | `string` | Root entry index.               |

**Return**: `void`

**Inner Mechanisms**:
- Uses `_show_tree` for rendering.
- Displays branch icons (`tree_branchopen.png`/`tree_branchclose.png`) for containers.

---

### Method: `show_target($index = "", $action = NULL, $action_insert = NULL, $action_append = NULL, $name_key = "name", $base = "", $type_insert = NULL, $subtype_insert = NULL, $type_append = NULL, $subtype_append = NULL)`

**Purpose**:
Renders the hierarchy as a target list for drag-and-drop operations, with insert/append actions.

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$index`            | `string`   | Selected entry index.                                                       |
| `$action`           | `string`   | URL template for entry actions.                                             |
| `$action_insert`    | `string`   | URL template for insert actions.                                            |
| `$action_append`    | `string`   | URL template for append actions.                                            |
| `$name_key`         | `string`   | Key for display names.                                                      |
| `$base`             | `string`   | Root entry index.                                                           |
| `$type_insert`      | `array`    | Rules for insert target types (e.g., `["folder" => TRUE]`).                 |
| `$subtype_insert`   | `array`    | Rules for insert target subtypes.                                           |
| `$type_append`      | `array`    | Rules for append target types.                                              |
| `$subtype_append`   | `array`    | Rules for append target subtypes.                                           |

**Return**: `void`

**Inner Mechanisms**:
- Uses `_show_target` for rendering.
- Validates target types/subtypes before rendering insert/append actions.

---

### Method: `show_path($index = "", $action = NULL, $name_key = "name", $delimiter = "›", $base = "")`

**Purpose**:
Renders a breadcrumb trail from the base to the current entry.

**Parameters**:

| Name        | Type     | Description                     |
|-------------|----------|---------------------------------|
| `$index`    | `string` | Current entry index.            |
| `$action`   | `string` | URL template for entry actions. |
| `$name_key` | `string` | Key for display names.          |
| `$delimiter`| `string` | Separator between entries.      |
| `$base`     | `string` | Root entry index.               |

**Return**: `void`

---

### Method: `show_column($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a multi-column list (e.g., for file explorers).

**Parameters**:

| Name        | Type     | Description                     |
|-------------|----------|---------------------------------|
| `$index`    | `string` | Current entry index.            |
| `$action`   | `string` | URL template for entry actions. |
| `$name_key` | `string` | Key for display names.          |
| `$mark`     | `array`  | Custom markers for entries.     |
| `$icon`     | `array`  | Custom icons for entry types.   |
| `$base`     | `string` | Root entry index.               |

**Return**: `void`

**Inner Mechanisms**:
- Uses `_show_column` for rendering.
- Displays the base and path as a header, followed by child entries.

---

### Method: `show_folder($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a flat folder list (e.g., for navigation menus).

**Parameters**:

| Name        | Type     | Description                     |
|-------------|----------|---------------------------------|
| `$index`    | `string` | Current entry index.            |
| `$action`   | `string` | URL template for entry actions. |
| `$name_key` | `string` | Key for display names.          |
| `$mark`     | `array`  | Custom markers for entries.     |
| `$icon`     | `array`  | Custom icons for entry types.   |
| `$base`     | `string` | Root entry index.               |

**Return**: `void`

**Inner Mechanisms**:
- Uses `_show_folder` for rendering.
- Displays the base and path as a header, followed by child entries.

---

### Method: `space($count = 1, $size = 18)`

**Purpose**:
Generates an invisible spacer image for consistent layout.

**Parameters**:

| Name      | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$count`  | `int`    | Number of spaces.               |
| `$size`   | `int`    | Size of each space in pixels.   |

**Return**: `string` HTML image tag.

**Usage**:
```php
echo $flexview->space(2); // Two spaces
```


<!-- HASH:6e16f44d7f47b67221a916d864a320fa -->
