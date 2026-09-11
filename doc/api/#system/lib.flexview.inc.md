# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.flexview.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.flexview.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/lib.flexview.inc

## Overview

The `flexview` class provides a flexible hierarchical view rendering system for the PWNC Web Platform. It enables developers to display tree-like data structures (e.g., nested categories, file systems, navigation menus) in various visual formats including collapsible trees, drag-and-drop hierarchies, breadcrumb paths, column layouts, and folder views.

The system works by maintaining an internal object graph where each node can have a parent and children. Data can be imported from either a `data` object (with container markers) or a MySQL result set. The class supports customizable display templates, icons, marks, checkboxes, and drag-and-drop functionality.

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | 0 | No entry type |
| `CMS_FLEXVIEW_ENTRY_TYPE_BASE` | 1 | Base/root entry |
| `CMS_FLEXVIEW_ENTRY_TYPE_ENTRY` | 2 | Regular entry |
| `CMS_FLEXVIEW_ENTRY_TYPE_END` | 3 | End marker |

## Classes

### flexview_entry

A simple data transfer object representing a single entry during traversal.

#### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `$type` | int | `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | Entry type constant |
| `$index` | mixed | `NULL` | Unique identifier for the entry |
| `$parent` | mixed | `NULL` | Parent entry index |
| `$position` | int | 0 | Position among siblings |
| `$count` | int | 0 | Total siblings count |
| `$subcount` | int | 0 | Number of child entries |
| `$indentation` | int | 0 | Depth level in hierarchy |
| `$open` | bool | FALSE | Whether the entry is expanded |

### flexview

Main class for managing and rendering hierarchical views.

#### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `$object` | array | `["" => ["#data" => ["#type" => "base"]]]` | Internal object graph |
| `$icon_default` | array | NULL | Default icon mappings |
| `$value_function` | callable | NULL | Callback to transform index values |
| `$encoding_function` | callable | NULL | Callback to encode index values |
| `$display_function` | callable | NULL | Custom display callback |
| `$display` | string | `"%checkbox%<a[ href=\"%action%\"][ class=\"%class%\"]>%mark%[%icon% ]%name%</a>"` | Display template |
| `$index` | string | "" | Currently selected index |
| `$checkbox_identifier` | string | NULL | Base name for checkbox inputs |
| `$checkbox_index` | int | 0 | Counter for checkbox generation |
| `$checkbox_list` | array | NULL | Pre-selected checkbox indices |
| `$mark` | array | NULL | Mark indicators per index |
| `$icon_custom` | array | NULL | Custom icon overrides |
| `$action` | string | `"%index%"` | Action URL template |
| `$name_key` | string | "name" | Key for name data |
| `$image_button_key` | string | "image_button" | Key for button image |
| `$image_hover_key` | string | "image_hover" | Key for hover image |
| `$image_active_key` | string | "image_active" | Key for active image |
| `$description_key` | string | "description" | Key for description data |
| `$base` | string | "" | Root index identifier |
| `$param` | array | NULL | Additional parameters |

## Methods

### __construct()

Initializes the flexview instance by loading default icons from the system data store.

**Parameters:** None

**Return:** None

**Mechanism:** Loads icon defaults from `#system/flexview.icon` data using the current application context. Falls back to standard icon names if not found. Sets the default encoding function to `x()` (XML escaping).

**Usage:**
```php
$view = new flexview();
```

### set_value_function($callback_function)

Sets a callback to transform index values before display.

**Parameters:**
- `$callback_function` (callable): Function to transform index values

**Return:** None

**Mechanism:** Stores the callback if it's callable. Used in `get_value()` and `get_checkbox()`.

**Usage:**
```php
$view->set_value_function(function($index) {
    return "item_" . $index;
});
```

### set_encoding_function($callback_function)

Sets a callback to encode index values for safe output.

**Parameters:**
- `$callback_function` (callable): Function to encode values

**Return:** None

**Mechanism:** Stores the callback if callable. Used in `get_value()` after value transformation.

**Usage:**
```php
$view->set_encoding_function('urlencode');
```

### set_display_function($callback_function)

Sets a custom display function that overrides the default template-based rendering.

**Parameters:**
- `$callback_function` (callable): Custom display callback

**Return:** None

**Mechanism:** When set, `display()` calls this function instead of using the template. Receives `$this`, `$index`, and `$open` parameters.

**Usage:**
```php
$view->set_display_function(function($view, $index, $open) {
    echo "<div class='custom-item'>{$view->get_name($index)}</div>";
});
```

### set_index($value)

Sets the currently selected/active index.

**Parameters:**
- `$value` (string): Index to mark as active

**Return:** None

**Mechanism:** If the index doesn't exist in the object graph, falls back to the base index.

**Usage:**
```php
$view->set_index("category_123");
```

### set_checkbox_identifier($value)

Configures checkbox generation for bulk operations.

**Parameters:**
- `$value` (string): Base name for checkbox inputs

**Return:** None

**Mechanism:** Resets the checkbox counter and sets the identifier used in `get_checkbox()`.

**Usage:**
```php
$view->set_checkbox_identifier("selected_items");
```

### set_checkbox_list($value)

Pre-selects specific checkboxes.

**Parameters:**
- `$value` (array): Indices to pre-check

**Return:** None

**Mechanism:** Converts the array to a flipped map for O(1) lookup in `get_checkbox()`.

**Usage:**
```php
$view->set_checkbox_list(["item_1", "item_3", "item_5"]);
```

### get_checkbox($index)

Generates an HTML checkbox input for the given index.

**Parameters:**
- `$index` (mixed): Entry index

**Return:** string|NULL - HTML input element or NULL if no identifier set

**Mechanism:** Creates a checkbox with incrementing name suffix. Applies value function if set. Checks against pre-selected list.

**Usage:**
```php
echo $view->get_checkbox("item_123");
// Output: <input name="selected_items[0]" type="checkbox" value="item_123" class="fv-cb">
```

### set_mark($value)

Sets mark indicators for entries.

**Parameters:**
- `$value` (array): Map of index => mark value

**Return:** None

**Mechanism:** Stores the mark map used by `get_mark()`.

**Usage:**
```php
$view->set_mark(["item_1" => "★", "item_2" => "✓"]);
```

### get_mark($index)

Retrieves the mark for a given index.

**Parameters:**
- `$index` (mixed): Entry index

**Return:** mixed|NULL - Mark value or NULL if not set

**Usage:**
```php
$mark = $view->get_mark("item_1"); // Returns "★"
```

### set_icon($value)

Sets custom icon mappings.

**Parameters:**
- `$value` (array): Icon mapping array

**Return:** None

**Mechanism:** Stores custom icons. Setting to FALSE disables all icons.

**Usage:**
```php
$view->set_icon([
    "#base" => "my_base_icon",
    "entry" => "my_entry_icon",
    "container" => "my_container_icon"
]);
```

### get_icon($index, $open = FALSE)

Determines the appropriate icon for an entry.

**Parameters:**
- `$index` (mixed): Entry index
- `$open` (bool): Whether the entry is expanded

**Return:** string|NULL - Icon identifier or NULL

**Mechanism:** Checks custom icons first, then falls back to defaults. Differentiates between base, container (open/closed), and regular entries.

**Usage:**
```php
$icon = $view->get_icon("category_1", TRUE);
```

### set_action($value)

Sets the action URL template.

**Parameters:**
- `$value` (string): URL template with `%index%` placeholder

**Return:** None

**Usage:**
```php
$view->set_action("/edit/%index%");
```

### get_action($index)

Generates the action URL for a given index.

**Parameters:**
- `$index` (mixed): Entry index

**Return:** string - Resolved action URL

**Mechanism:** Applies value and encoding functions, then replaces `%index%` in the action template.

**Usage:**
```php
$url = $view->get_action("item_123"); // "/edit/item_123"
```

### set_name_key($value)

Sets the data key used for entry names.

**Parameters:**
- `$value` (string): Data key name

**Return:** None

**Usage:**
```php
$view->set_name_key("title");
```

### get_name($index)

Retrieves the display name for an entry.

**Parameters:**
- `$index` (mixed): Entry index

**Return:** string - Entry name or default text

**Mechanism:** Looks up the name key in the entry's data. Falls back to `CMS_L_FLEXVIEW_002` for base entries or `CMS_L_UNKNOWN` for others.

**Usage:**
```php
$name = $view->get_name("item_123"); // "My Item"
```

### set_image_key($button, $hover, $active)

Sets the data keys for different image states.

**Parameters:**
- `$button` (string): Key for button image
- `$hover` (string): Key for hover image
- `$active` (string): Key for active image

**Return:** None

**Usage:**
```php
$view->set_image_key("btn_img", "hover_img", "active_img");
```

### get_image($index, $open = FALSE)

Retrieves the appropriate image URL for an entry.

**Parameters:**
- `$index` (mixed): Entry index
- `$open` (bool): Whether the entry is expanded

**Return:** string|NULL - Image URL or NULL

**Mechanism:** Returns active image if open, otherwise button image. Uses `translate_url()` for resolution.

**Usage:**
```php
$img = $view->get_image("item_123", TRUE);
```

### set_base($value)

Sets the root/base index.

**Parameters:**
- `$value` (string): Base index identifier

**Return:** None

**Mechanism:** Validates that the index exists in the object graph, falling back to empty string if not.

**Usage:**
```php
$view->set_base("root_category");
```

### display($index, $open = FALSE)

Renders an entry using the display template.

**Parameters:**
- `$index` (mixed): Entry index
- `$open` (bool): Whether the entry is expanded

**Return:** None (outputs HTML)

**Mechanism:** If a custom display function is set, delegates to it. Otherwise, processes the template by replacing placeholders with computed values. Handles conditional attributes with bracket syntax `[ attr="%value%"]`.

**Usage:**
```php
$view->display("item_123", TRUE);
```

### get_value($index)

Transforms and encodes an index value.

**Parameters:**
- `$index` (mixed): Raw index

**Return:** string - Processed index value

**Mechanism:** Applies value function (if set), then encoding function (if set).

**Usage:**
```php
$processed = $view->get_value("item_123");
```

### set($index, $data = NULL, $parent = "")

Adds or updates an entry in the object graph.

**Parameters:**
- `$index` (mixed): Entry identifier
- `$data` (array): Entry data
- `$parent` (string): Parent index

**Return:** None

**Mechanism:** Sets the entry's data and parent reference, and creates a reference from the parent to this entry.

**Usage:**
```php
$view->set("item_123", ["name" => "My Item"], "parent_456");
```

### get_path()

Computes the path from base to the currently selected index.

**Parameters:** None

**Return:** array - Path indices from base to current index

**Mechanism:** Traverses parent references from the current index back to the base, then reverses the path.

**Usage:**
```php
$path = $view->get_path(); // ["root", "category", "subcategory"]
```

### import_data(&$data)

Imports hierarchical data from a data object.

**Parameters:**
- `$data` (data): Data object with container markers

**Return:** None

**Mechanism:** Iterates through the data object, using "container"/"/container" types to manage nesting depth. Calls `set()` for each entry.

**Usage:**
```php
$data = new data("#system/my_data");
$view->import_data($data);
```

### import_database(&$result, $index_key = "id", $parent_key = "container")

Imports hierarchical data from a MySQL result set.

**Parameters:**
- `$result` (resource): MySQL result resource
- `$index_key` (string): Column name for entry identifiers
- `$parent_key` (string): Column name for parent references

**Return:** None

**Mechanism:** Fetches rows and calls `set()` for each, extracting index and parent from specified columns.

**Usage:**
```php
$result = mysql_query("SELECT id, container, name FROM categories");
$view->import_database($result);
```

### show_custom($callback_function)

Traverses the object graph and calls a callback for each entry.

**Parameters:**
- `$callback_function` (callable): Callback receiving `flexview_entry` objects

**Return:** None

**Mechanism:** Performs a depth-first traversal, creating `flexview_entry` objects with type, position, count, indentation, and open state. Calls the callback for BASE, ENTRY, and END events.

**Usage:**
```php
$view->show_custom(function($entry) {
    if ($entry->type === CMS_FLEXVIEW_ENTRY_TYPE_ENTRY) {
        echo str_repeat("-", $entry->indentation) . $view->get_name($entry->index);
    }
});
```

### show_hierarchy($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "", $dragdrop_event_function = NULL, $dragdrop_type_accept = NULL)

Renders a drag-and-drop enabled hierarchical view.

**Parameters:**
- `$index` (string): Active index
- `$action` (string): Action URL template
- `$name_key` (string): Name data key
- `$mark` (array): Mark indicators
- `$icon` (array): Custom icons
- `$base` (string): Base index
- `$dragdrop_event_function` (string): JavaScript callback for drop events
- `$dragdrop_type_accept` (array): Type acceptance rules for drag/drop

**Return:** None (outputs HTML and JavaScript)

**Mechanism:** Sets up configuration, outputs initialization script, then calls `show_custom()` with `_show_hierarchy()` callback.

**Usage:**
```php
$view->show_hierarchy(
    "root",
    "/move/%index%",
    "name",
    NULL,
    NULL,
    "root",
    "handleDrop",
    ["category" => ["index" => 1, "insert" => 1, "append" => 1]]
);
```

### _show_hierarchy($flexview_entry)

Internal callback for rendering hierarchy entries.

**Parameters:**
- `$flexview_entry` (flexview_entry): Current entry data

**Return:** bool - Whether to continue traversal

**Mechanism:** Manages indentation levels, outputs source/target divs with drag-drop attributes, handles checkbox toggles for containers.

### show_tree($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")

Renders a collapsible tree view.

**Parameters:**
- `$index` (string): Active index
- `$action` (string): Action URL template
- `$name_key` (string): Name data key
- `$mark` (array): Mark indicators
- `$icon` (array): Custom icons
- `$base` (string): Base index

**Return:** None (outputs HTML)

**Mechanism:** Sets up configuration and calls `show_custom()` with `_show_tree()` callback.

**Usage:**
```php
$view->show_tree("root", "/view/%index%", "name", NULL, NULL, "root");
```

### _show_tree($flexview_entry)

Internal callback for rendering tree entries.

**Parameters:**
- `$flexview_entry` (flexview_entry): Current entry data

**Return:** bool - Whether to continue traversal

**Mechanism:** Outputs tree branch images (using SVG), manages indentation tracking, handles container open/close states.

### show_target($index = "", $action = NULL, $action_insert = NULL, $action_append = NULL, $name_key = "name", $base = "", $type_insert = NULL, $subtype_insert = NULL, $type_append = NULL, $subtype_append = NULL)

Renders a target-based view with insert/append actions.

**Parameters:**
- `$index` (string): Active index
- `$action` (string): Default action URL template
- `$action_insert` (string): Insert action URL template
- `$action_append` (string): Append action URL template
- `$name_key` (string): Name data key
- `$base` (string): Base index
- `$type_insert` (array): Type rules for insert actions
- `$subtype_insert` (array): Subtype rules for insert actions
- `$type_append` (array): Type rules for append actions
- `$subtype_append` (array): Subtype rules for append actions

**Return:** None (outputs HTML)

**Mechanism:** Sets up configuration and calls `show_custom()` with `_show_target()` callback.

**Usage:**
```php
$view->show_target(
    "root",
    "/view/%index%",
    "/insert/%index%",
    "/append/%index%",
    "name",
    "root"
);
```

### _show_target($flexview_entry)

Internal callback for rendering target entries.

**Parameters:**
- `$flexview_entry` (flexview_entry): Current entry data

**Return:** None

**Mechanism:** Manages indentation, outputs target items with insert/append action links based on type validation.

### show_path($index = "", $action = NULL, $name_key = "name", $delimiter = "›", $base = "")

Renders a breadcrumb path view.

**Parameters:**
- `$index` (string): Active index
- `$action` (string): Action URL template
- `$name_key` (string): Name data key
- `$delimiter` (string): Path separator
- `$base` (string): Base index

**Return:** None (outputs HTML)

**Mechanism:** Computes path from base to current index, displays base and each path element with delimiter.

**Usage:**
```php
$view->show_path("subcategory_123", "/view/%index%", "name", " > ", "root");
```

### show_column($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")

Renders a column-based view with breadcrumb.

**Parameters:**
- `$index` (string): Active index
- `$action` (string): Action URL template
- `$name_key` (string): Name data key
- `$mark` (array): Mark indicators
- `$icon` (array): Custom icons
- `$base` (string): Base index

**Return:** None (outputs HTML)

**Mechanism:** Displays breadcrumb followed by direct children of the current index in a column layout.

**Usage:**
```php
$view->show_column("category_123", "/view/%index%", "name", NULL, NULL, "root");
```

### show_folder($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")

Renders a folder-style view with breadcrumb.

**Parameters:**
- `$index` (string): Active index
- `$action` (string): Action URL template
- `$name_key` (string): Name data key
- `$mark` (array): Mark indicators
- `$icon` (array): Custom icons
- `$base` (string): Base index

**Return:** None (outputs HTML)

**Mechanism:** Similar to column view but uses line breaks instead of CSS columns.

**Usage:**
```php
$view->show_folder("category_123", "/view/%index%", "name", NULL, NULL, "root");
```

### _show_breadcrumb(&$index)

Internal method for rendering breadcrumb navigation.

**Parameters:**
- `$index` (string): Current index (passed by reference)

**Return:** bool - Whether breadcrumb was displayed

**Mechanism:** Displays base entry, then traverses path from base to current index, displaying each ancestor.

### space($count = 1, $size = 20)

Generates an SVG spacer element.

**Parameters:**
- `$count` (int): Number of spaces
- `$size` (int): Size of each space in pixels

**Return:** string - SVG element

**Mechanism:** Creates an empty SVG with specified width and height for indentation purposes.

**Usage:**
```php
echo $view->space(3); // 60px wide spacer
```


<!-- HASH:a5ce2b756efdd7b53a3b8bcea667fb98 -->
