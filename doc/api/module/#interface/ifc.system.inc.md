# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.system.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.system.inc)

- **Version:** `26.9.23.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# PWNC System Interface Documentation

## Overview

The `ifc.system.inc` file serves as the interface for the PWNC System Manager. It provides a comprehensive tree-view interface for managing data structures (files, configurations, or modules) stored in a flat file (`#system/system.dat`). It supports CRUD operations (Create, Read, Update, Delete) and Move operations (Copy/Cut/Paste) within a hierarchical data structure.

The interface relies on the `flexview` class for rendering the tree and the `data` class for manipulating the underlying file structure. It uses a message-driven architecture where `CMS_IFC_MESSAGE` controls the execution flow (e.g., `save`, `add`, `copy`).

## Classes

### `data`

The `data` class is instantiated to manage the state of the system file. It handles serialization, tree traversal, and attribute management.

**Usage Example:**
```php
$file = CMS_DATA_PATH . "#system/system.dat";
$data = new data($file);
$data->set($key, $parent, $value);
$data->save();
```

**Methods:**

| Method | Parameters | Return Value | Description |
| :--- | :--- | :--- | :--- |
| `__construct($file)` | `$file` (string): Path to the data file. | `void` | Initializes the data structure from the file. |
| `has($key)` | `$key` (string): The identifier to check. | `bool` | Checks if an entry exists. |
| `get($key, $attr=NULL)` | `$key` (string): Identifier.<br>`$attr` (string): Attribute key. | `mixed` | Retrieves the value of an attribute or the whole entry. |
| `set($key, $parent, $value)` | `$key` (string): Identifier.<br>`$parent` (string): Parent identifier.<br>`$value` (mixed): Value to set. | `void` | Sets an attribute for a specific entry. |
| `save()` | None | `bool` | Persists changes to the file. Returns `TRUE` on success. |
| `move($mode, $key=NULL)` | `$mode` (string): 'first', 'next', 'prev', 'parent', 'last'.<br>`$key` (string): Current key. | `string|bool` | Traverses the tree. Returns the next key or `FALSE`. |
| `del($key)` | `$key` (string): Identifier. | `void` | Deletes an entry. |
| `insert($parent)` | `$parent` (string): Parent identifier. | `string|bool` | Inserts the buffered data as a child of `$parent`. |
| `append($parent)` | `$parent` (string): Parent identifier. | `string|bool` | Appends the buffered data to `$parent`. |
| `copy($key)` | `$key` (string): Identifier. | `void` | Copies the entry to the internal buffer. |
| `cut($key)` | `$key` (string): Identifier. | `void` | Moves the entry to the internal buffer (deletes from master). |
| `is_child($parent, $child)` | `$parent` (string): Parent ID.<br>`$child` (string): Child ID. | `bool` | Checks if `$child` is a descendant of `$parent`. |

### `ifc`

The `ifc` class generates the HTML forms, buttons, and layout elements for the interface.

**Methods:**

| Method | Parameters | Return Value | Description |
| :--- | :--- | :--- | :--- |
| `__construct($response, $page, $menu, $params)` | `$response` (string): Message response.<br>`$page` (string): Page identifier.<br>`$menu` (array): Menu items.<br>`$params` (array): Initial parameters. | `void` | Initializes the interface form. |
| `set($label, $type, $value=NULL, $checked=NULL)` | `$label` (string): Label text.<br>`$type` (string): Input type (text, button, checkbox).<br>`$value` (string): Value.<br>`$checked` (bool): Checkbox state. | `void` | Adds an input field to the form. |
| `param($name, $value)` | `$name` (string): Parameter name.<br>`$value` (string): Parameter value. | `void` | Sets a hidden parameter. |
| `close()` | None | `void` | Closes the form and outputs the final HTML. |

### `flexview`

The `flexview` class renders the tree view and target selection lists.

**Methods:**

| Method | Parameters | Return Value | Description |
| :--- | :--- | :--- | :--- |
| `import_data($data)` | `$data` (object): `data` instance. | `void` | Loads the data structure to be rendered. |
| `set_encoding_function($func)` | `$func` (string): Callback function name. | `void` | Sets the function used to encode output (e.g., `qr`). |
| `set_checkbox_identifier($id)` | `$id` (string): Checkbox name. | `void` | Sets the identifier for checkboxes. |
| `set_checkbox_list($list)` | `$list` (array): Array of checked IDs. | `void` | Sets which items are pre-checked. |
| `show_tree($index, $action, $nameKey, $mark, $icons)` | `$index` (string): Root index.<br>`$action` (string): JS action on click.<br>`$nameKey` (string): Key for display name.<br>`$mark` (array): Marking options.<br>`$icons` (array): Icon mappings. | `void` | Renders the hierarchical tree. |
| `show_target($index, $action1, $action2, $action3)` | `$index` (string): Target index.<br>`$action1` (string): JS action for selection.<br>`$action2` (string): JS action for insert.<br>`$action3` (string): JS action for append. | `void` | Renders a target selection list. |

### `plist`

The `plist` class manages the list of recent files.

**Methods:**

| Method | Parameters | Return Value | Description |
| :--- | :--- | :--- | :--- |
| `add($path, $unique)` | `$path` (string): Path to add.<br>`$unique` (bool): Check for duplicates. | `void` | Adds a path to the list. |
| `remove($count, $index)` | `$count` (int): Number to remove.<br>`$index` (int): Index to start from. | `void` | Removes items from the list. |

## Interface Logic (Switch Cases)

The core logic is driven by the `CMS_IFC_MESSAGE` constant.

### `save`

Handles the saving of attributes for the currently selected object.

**Inner Mechanisms:**
1.  Iterates through parameters (`$ifc_param1`, `attribute3`, `attribute4`, etc.) to update the data structure.
2.  Calls `$data->save()` to persist changes.
3.  If the object becomes empty (no attributes left), it deletes the object and moves the view to the parent object (`move("parent")`).

**Usage Context:**
Triggered when the user clicks the "Save" button in the data panel.

### `add` / `add_target` / `add_insert` / `add_append`

Handles the creation of new entries.

**Inner Mechanisms:**
1.  **`add`**: Sets parameters to prepare for a new entry.
2.  **`add_target`**: Renders a form with a text input for the key and a checkbox for "container" mode. Uses `flexview` to show the target tree.
3.  **`add_insert` / `add_append`**: Executes the insertion.
    *   If no key is provided, generates a unique ID (`unique_id()`).
    *   If "container" is checked, creates a wrapper structure (`#type` => "container").
    *   Inserts the new entry into the data structure.

**Usage Context:**
Triggered by the "Add" menu command.

### `copy` / `cut` / `target` / `copy_insert` / `copy_append` / `cut_insert` / `cut_append`

Handles moving data between locations.

**Inner Mechanisms:**
1.  **`copy` / `cut`**: Sets the `$status` variable to indicate the operation type (e.g., "copy", "cut") and sets the `$target`.
2.  **`target`**: Updates the `$target` variable based on user selection.
3.  **`copy_insert` / `copy_append` / `cut_insert` / `cut_append`**:
    *   Clones the `data` object (`$_data = clone $data`) to create a safe working copy.
    *   Iterates through the selected list in reverse order.
    *   **Buffering**: Uses `$_data->copy()` or `$_data->cut()` to move data into the clone's buffer.
    *   **Pasting**: Moves the buffer from the clone to the master `data` object using `insert()` or `append()`.
    *   Handles edge cases where the target is a child of the source (prevents infinite loops).

**Usage Context:**
Triggered by "Copy", "Cut", and subsequent selection actions in the target panel.

### `delete`

Handles the removal of selected entries.

**Inner Mechanisms:**
1.  Traverses up the tree from the current object to build a stack of valid parents.
2.  Deletes all items in the `$list`.
3.  Saves changes.
4.  Attempts to select the last valid parent in the stack to keep the view active.

**Usage Context:**
Triggered by the "Delete" menu command.

### `filemanager`

Updates the recent files list in the file manager.

**Inner Mechanisms:**
1.  Loads the `plist` class.
2.  Adds the current file path to the list.
3.  Removes the oldest entries to keep the list short.

## Helper Functions & Constants

### `cms_load($lib, $exit=FALSE, $test=FALSE)`

Loads a library file. If `$exit` is `TRUE`, the script terminates if the library is missing.

### `ifc_permission($perms)`

Checks if the current user has the required permissions defined in `$perms`.

### `blank($v)`

Checks if a value is empty or whitespace.

### `stre($v)`

Checks if a value is not empty.

### `streq($a, $b)`

Checks if two strings are equal.

### `x($s)`

Escapes a string for XML (escapes `", ', &, <, >`).

### `qr($s)`

Encodes a string for JavaScript/JSON (UTF-16/Binary/ASCII).

### `ifc_post($action, $index)`

Generates a JavaScript `post` command to submit the interface state.

### `ifc_table_open()` / `ifc_table_close()`

Helper functions to open and close HTML tables used for layout.


<!-- HASH:77fdd09819e55301436ac985121d6d73 -->
