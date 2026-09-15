# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.system.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.system.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## System Interface Module

The `ifc.system.inc` file is the core interface controller for managing system-level data structures within the PWNC Web Platform. It provides a tree-based interface for viewing, editing, and organizing hierarchical data stored in `.dat` files using the `data` class. This module supports operations such as adding, copying, cutting, deleting, and saving entries in a structured format.

### Key Concepts

- **Data Structure**: Hierarchical key-value storage with support for containers and attributes.
- **Flexview Integration**: Visual tree representation of the data structure.
- **Message Handling**: Switch-based routing for different interface actions (e.g., `select`, `save`, `add`, `copy`, `cut`, `delete`).
- **State Management**: Uses global variables like `$object`, `$list`, `$target`, and `$status` to maintain context across requests.

---

## Global Variables

| Variable | Type | Description |
|---------|------|-------------|
| `$file` | string | Path to the system data file. Defaults to `CMS_DATA_PATH . "#system/system.dat"` if not set or invalid. |
| `$object` | mixed | Current selected object index. Defaults to `0` if not set or invalid. |
| `$list` | array | List of selected objects for batch operations. |
| `$target` | mixed | Target object for copy/cut operations. |
| `$status` | string | Current operation status (e.g., `"copy"`, `"cut"`). |

---

## Message Handling

### `switch (CMS_IFC_MESSAGE)`

Routes execution based on the current interface message/action.

#### Case: `"select"`

Selects an object and initializes the list for display.

**Parameters:**
- `$ifc_param`: Object identifier to select.

**Behavior:**
Sets `$object` to `$ifc_param`, initializes `$list` with the selected object, and clears `$status`.

---

#### Case: `"save"`

Saves changes to the data structure.

**Parameters:**
- `$ifc_param1`: New attribute key (optional).
- `$ifc_param2`: New attribute value (optional).
- `$ifc_param3+`: Additional attribute key-value pairs passed via dynamic variables (`attribute3`, `attribute4`, etc.).

**Behavior:**
1. Adds new attribute if `$ifc_param1` is provided.
2. Iterates through dynamically named parameters to update existing attributes.
3. Saves the data structure.
4. If the object has no remaining attributes, moves its parent reference and deletes it.
5. Updates `$list` and sets response to `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` otherwise.

---

#### Case: `"add"` / `"add_target"`

Displays a form for adding a new entry.

**Parameters:**
- `$ifc_param`: Target object where the new entry will be added.
- `$ifc_param1`: Key/name for the new entry.
- `$ifc_param2`: Container flag indicating whether the new entry should be a container.

**Behavior:**
1. Ensures all existing entries have a `name` attribute.
2. Creates an `ifc` instance with cancel command and context parameters.
3. Renders a table with input fields for the new entry's name and container flag.
4. Displays a flexview target selector for choosing insertion point.

---

#### Case: `"add_insert"` / `"add_append"`

Inserts or appends a new entry into the data structure.

**Parameters:**
- `$ifc_param`: Target object for insertion.
- `$ifc_param1`: Key/name for the new entry.
- `$ifc_param2`: Container flag.

**Behavior:**
1. Generates a unique ID if no key is provided.
2. Buffers the new entry (with optional container markers).
3. Inserts or appends the buffered entry relative to the target.
4. Saves the data structure and updates state on success.

---

#### Case: `"copy"` / `"cut"`

Initiates a copy or cut operation.

**Parameters:**
- `$object`: Source object to copy/cut.

**Behavior:**
Sets `$target` to `$object` and `$status` to the current message (`"copy"` or `"cut"`).

---

#### Case: `"target"`

Sets the target for a pending copy/cut operation.

**Parameters:**
- `$ifc_param`: Target object identifier.

**Behavior:**
Updates `$target` to the specified object.

---

#### Case: `"copy_insert"` / `"copy_append"` / `"cut_insert"` / `"cut_append"`

Executes the actual copy or cut operation.

**Parameters:**
- `$ifc_param`: Target object for insertion/appending.
- `$list`: Array of source objects to copy/cut.

**Behavior:**
1. Clones the data structure for safe manipulation.
2. For each source object:
   - Copies or cuts the object in the clone.
   - Transmits the buffer from clone to master.
   - Inserts or appends the buffer relative to the target.
3. Saves the data structure and updates state on success.

---

#### Case: `"delete"`

Deletes selected objects from the data structure.

**Parameters:**
- `$list`: Array of object identifiers to delete.

**Behavior:**
1. Builds a path stack from root to current object.
2. Deletes all objects in `$list`.
3. Saves the data structure.
4. Selects the last existing object in the path on success.

---

#### Case: `"filemanager"`

Registers the current file in the file manager's recent files list.

**Behavior:**
1. Loads the `plist` library.
2. Adds the current file path to the recent files list.
3. Limits the list to 10 entries.

---

## Main Display

Renders the primary interface layout including:

1. **Menu Bar**: Commands for add, copy, cut, and delete.
2. **Tree View**: Interactive tree of the data structure using `flexview`.
3. **Selection Controls**: Buttons to activate, invert, or deactivate selections.
4. **Target Panel** (when copying/cutting): Shows available targets.
5. **Data Panel** (when an object is selected): Form for editing object attributes.

### Layout Structure

```html
<table class="layout">
  <colgroup>
    <col>
    [target column if status]
    [data column if object]
  </colgroup>
  <tr>
    <td>
      <!-- Menu, Tree View, Selection Controls -->
    </td>
    [Target Panel]
    [Data Panel]
  </tr>
</table>
```

---

## Usage Example

```php
// Initialize system interface
$file = CMS_DATA_PATH . "#system/custom.dat";
$object = "my_entry";
$list = ["entry1", "entry2"];
$target = NULL;
$status = NULL;

// Trigger interface rendering
include("module/#interface/ifc.system.inc");
```

This example loads a custom system data file and displays the interface with pre-selected entries.


<!-- HASH:afa0ffbf59a16447dbddce4f8e7fd1a1 -->
