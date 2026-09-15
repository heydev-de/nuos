# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.insert.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.insert.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# Insert Interface Module

This file implements the **Insert Interface** (`ifc.insert.inc`) for the PWNC Web Platform. It provides a dual-panel management interface for handling two related data structures:

1. **Insert Codes** (`#system/insert.code`) — Reusable code snippets that can be inserted into content.
2. **Insert Positions** (`#system/insert`) — A hierarchical tree structure defining where insert codes are placed within the system.

The interface allows users to create, edit, delete, and assign insert codes to specific positions in the tree.

---

## Overview

The module operates in two main modes:
- **Insert Code Management** (`insert` message): Manage reusable code snippets.
- **Position Management** (default/main display): Manage the hierarchical tree of insert positions.

It uses the `data` class for data manipulation, `flexview` for tree rendering, and the `ifc` class for form generation and UI components.

---

## Constants and Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IFC_MESSAGE` | (dynamic) | Current interface message/action being processed |
| `CMS_IFC_PAGE` | (dynamic) | Current interface page identifier |
| `CMS_USER` | (dynamic) | Current user identifier |
| `$object` | (from cache) | Currently selected object in the main tree |
| `$list` | (array) | List of selected objects for batch operations |
| `$insert_object` | (dynamic) | Currently selected insert code object |
| `$ifc_param` | (dynamic) | Primary parameter for the current action |
| `$ifc_param1` | (dynamic) | Secondary parameter (e.g., name input) |
| `$ifc_param2` | (dynamic) | Tertiary parameter (text content) |
| `$ifc_param3` | (dynamic) | Quaternary parameter (HTML content) |
| `$ifc_tab_1` | (dynamic) | Tab selection state (1=text, 2=html) |
| `$ifc_response` | (dynamic) | Response status (success/error) |

---

## Message Handling

### `insert`, `insert_select`, `insert_display`, `insert_add`, `insert_save`, `insert_delete`

These cases handle operations related to **insert codes** stored in `#system/insert.code`.

#### Data Structure
Each insert code entry contains:
- `name`: Display name of the code snippet
- `html`: Boolean flag indicating if content is HTML (1) or plain text (0)
- `code`: The actual code content

---

### `insert_select`

Selects an insert code object for editing.

**Parameters:**
- `$ifc_param`: The key/identifier of the insert code to select

**Mechanism:**
Sets `$insert_object` to the provided parameter value, making it the active insert code for subsequent operations.

**Usage Example:**
```php
// Triggered via JavaScript: ifc_post('insert_select', 'my_code_key')
$insert_object = $ifc_param; // Selects the insert code with key 'my_code_key'
```

---

### `insert_display`

Displays a preview of the selected insert code.

**Parameters:**
- `$insert_object`: The key of the insert code to display

**Mechanism:**
1. Retrieves the code content using `$data->get($insert_object, "code")`
2. If HTML mode is enabled, displays raw code; otherwise parses text
3. Outputs the result and exits

**Usage Example:**
```php
// Shows a live preview of the insert code
$code = l($data->get($insert_object, "code"));
preview($data->get($insert_object, "html") ? $code : parse_text($code));
exit();
```

---

### `insert_add`

Creates a new insert code entry.

**Mechanism:**
1. Creates a new buffer with a default name (`CMS_L_IFC_INSERT_002`)
2. Appends the new entry to the data structure
3. Saves and sets the new object as active

**Usage Example:**
```php
// Creates a new insert code with default name
$data->set_buffer([["name" => CMS_L_IFC_INSERT_002]]);
$_insert_object = $data->append();
if ($data->save()) {
    $insert_object = $_insert_object;
    $ifc_response = CMS_MSG_DONE;
}
```

---

### `insert_save`

Saves changes to the currently selected insert code.

**Parameters:**
- `$ifc_param1`: Name of the insert code
- `$ifc_param2`: Text content (when not in HTML mode)
- `$ifc_param3`: HTML content (when in HTML mode)
- `$ifc_tab_1`: Tab selection (1=text, 2=html)

**Mechanism:**
1. Preserves existing name if none provided
2. Determines content type based on tab selection
3. Updates name, html flag, and code content
4. Saves to data store

**Usage Example:**
```php
// Saves insert code with name "Header Snippet" and HTML content
$ifc_param1 = "Header Snippet";
$ifc_tab_1 = 2; // HTML mode
$ifc_param3 = "<div class='header'>Welcome!</div>";

$data->set([
    "name" => $ifc_param1,
    "html" => 1,
    "code" => $ifc_param3
], $insert_object);
$ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `insert_delete`

Deletes selected insert code entries.

**Parameters:**
- `$_list`: Array of insert code keys to delete

**Mechanism:**
1. Iterates through selected items and deletes them
2. Cleans up references in `#system/insert` positions
3. Deselects the object if it was deleted

**Usage Example:**
```php
// Deletes insert codes with keys 'old_snippet' and 'temp_code'
$_list = ['old_snippet', 'temp_code'];
foreach ($_list AS $value) $data->del($value);
$data->save();

// Remove references from position assignments
$data_insert = new data("#system/insert");
$data_insert->move("first");
while ($key = $data_insert->move("next")) {
    $_key = $data_insert->get($key, "insert");
    if ($data->get($_key) === NULL) $data_insert->del($key, "insert");
}
$data_insert->save();
```

---

## Position Management

### `select`

Selects a position in the insert tree.

**Parameters:**
- `$ifc_param`: The key/identifier of the position to select

**Mechanism:**
Sets `$object` to the provided parameter and initializes `$list` with the selected object.

**Usage Example:**
```php
// Selects position 'header.main' in the tree
$object = $ifc_param; // 'header.main'
$list = [$object];
```

---

### `_add`

Creates a new position in the hierarchical tree.

**Parameters:**
- `$ifc_param1`: Name/path component for the new position
- `$object`: Parent object context

**Mechanism:**
1. Parses dot-separated path components
2. Creates intermediate containers as needed
3. Saves the new position structure

**Usage Example:**
```php
// Creates position 'sidebar.widgets.recent_posts'
$object = "sidebar";
$ifc_param1 = "widgets.recent_posts";

$array = explode(".", "$object.$ifc_param1");
// Creates containers: sidebar.widgets, sidebar.widgets.recent_posts
$data->save();
$object = "sidebar.widgets.recent_posts";
```

---

### `add`

Displays a form to add a new position.

**Mechanism:**
1. Creates an `ifc` instance with response handling
2. Provides a text input for the new position name
3. Submits to the `_add` action

**Usage Example:**
```php
$ifc = new ifc($ifc_response, $ifc_page, TRUE, 
    ["object" => $object, "list" => $list], "_add", CMS_L_COMMAND_ADD);
$ifc->set(CMS_L_NAME, "text 40 256 b");
$ifc->close();
```

---

### `delete`

Deletes selected positions from the tree.

**Parameters:**
- `$list`: Array of position keys to delete

**Mechanism:**
1. Builds a path stack to find parent objects
2. Deletes selected positions
3. Selects the last existing parent object

**Usage Example:**
```php
// Deletes positions 'temp_section' and 'old_widget'
$list = ['temp_section', 'old_widget'];
$stack = [];
$_object = $object;

// Build path to current object
while (($_object = $data->move("parent", $_object)) !== FALSE)
    array_unshift($stack, $_object);
array_unshift($stack, "");

// Delete and save
foreach ($list AS $value) $data->del($value);
$data->save();

// Select last existing parent
while ($data->get($object) === NULL) {
    $object = array_pop($stack);
    if (stre($object)) break;
}
```

---

### `save`

Assigns an insert code to a position.

**Parameters:**
- `$insert_object`: The insert code key to assign
- `$object`: The position key to assign to

**Mechanism:**
1. Sets the `insert` field of the position to the selected insert code
2. Saves the assignment

**Usage Example:**
```php
// Assigns insert code 'header_snippet' to position 'header.main'
$insert_object = "header_snippet";
$object = "header.main";

$data->set($insert_object, $object, "insert");
$ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

## Main Display

The main display section renders the hierarchical tree of insert positions using `flexview`, and shows details for the selected position including its assigned insert code.

### Tree Rendering

Uses `flexview` to display the hierarchical structure:
- Imports data from `#system/insert`
- Uses QR encoding for safe output
- Supports checkbox selection for batch operations
- Renders tree nodes with expandable/collapsible functionality

### Position Details Panel

When a position is selected, displays:
1. **Insert Code Assignment**: Dropdown to select which insert code to assign
2. **Preview Button**: Links to display the assigned insert code
3. **Save Button**: Persists the assignment

### JavaScript Integration

Includes a script that synchronizes text and HTML editor content when switching tabs:
- Copies content between text and HTML editors
- Maintains language consistency across editors
- Handles dynamic tab switching

**Example:**
```javascript
// Syncs content when switching between text and HTML tabs
fx_event_listen([ifc_object("ifc_tab_1"), ifc_object("ifc_tab_1", 1)], "change", () => {
    const value = parseInt(ifc_get("ifc_tab_1") || ifc_get("ifc_tab_1", 1));
    const flag = value === 1;
    // Copy content between editors based on active tab
    ifc_copy(from_source, to_source);
});
```

---

## Menu Configuration

### Insert Code Management Menu
| Label | Action | Message |
|-------|--------|---------|
| Add | `insert/command_add` | `insert_add` |
| Delete Selected | `insert/command_delete` | `#insert_delete` |
| Close | `insert/command_cancel` | `FALSE` |

### Position Management Menu
| Label | Action | Message |
|-------|--------|---------|
| Add | `insert/command_add` | `add` |
| Manage Codes | `insert/command_code` | `insert` |
| Delete Selected | `insert/command_delete` | `#delete` |

---

## Usage Scenarios

1. **Creating Reusable Code Snippets**: Use the insert code management panel to create and maintain reusable HTML/text snippets.

2. **Positioning Content**: Use the tree view to organize where insert codes appear in the system hierarchy.

3. **Batch Operations**: Select multiple positions or codes for simultaneous deletion or modification.

4. **Content Preview**: Use the display functionality to preview how insert codes will render before assignment.

5. **Hierarchical Organization**: Create nested position structures to organize complex content layouts.


<!-- HASH:7706a5cd02f745b4b2160c411a7e08eb -->
