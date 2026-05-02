# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.directory.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Directory Interface Module (`ifc.directory.inc`)

This file implements the **Directory** interface module for the NUOS web platform. It provides a comprehensive user interface for managing hierarchical directory structures, including:

- **Directory entries** (containers and links)
- **Directory types** (metadata and visual styling)
- **Canonical URLs** (multilingual support)
- **Drag-and-drop reordering** (via FlexView integration)
- **CRUD operations** (Create, Read, Update, Delete)

The module is tightly integrated with the **FlexView** and **Data** subsystems, enabling interactive tree manipulation and persistent storage.

---

### Constants and Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$object` | `cms_cache("directory." . CMS_USER . ".object")` | Currently selected directory entry (key). Persists across sessions via cache. |
| `$type_object` | `0` | Currently selected directory type (key). Used for type management. |

---

### Message Handling

The module processes interface messages (`CMS_IFC_MESSAGE`) to perform specific actions. Each case in the switch statement corresponds to a distinct operation.

---

### `case "type"` / `"type_select"` / `"type_add"` / `"type_save"` / `"type_delete"`

Manages **directory types** (metadata and icons).

#### Purpose
Handles CRUD operations for directory types, including:
- Selection of a type
- Creation of new types
- Saving type properties (name, icons)
- Deletion of types

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Type key (for selection/deletion). |
| `$ifc_param1` | `string` | Type name (for saving). |
| `$ifc_file1` | `file` | Uploaded file for "normal" icon. |
| `$ifc_file1_name` | `string` | Filename of `$ifc_file1`. |
| `$ifc_file2` | `file` | Uploaded file for "selected" icon. |
| `$ifc_file2_name` | `string` | Filename of `$ifc_file2`. |
| `$list` | `array` | Array of type keys (for deletion). |

#### Return Values
- **`$ifc_response`**: `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).
- **`$type_object`**: Updated to reflect the selected/deleted type.

#### Inner Mechanisms
1. **Data Source**: Uses `#system/directory.type` data file.
2. **File Handling**:
   - Validates file extensions (GIF, JPG, PNG, SVG).
   - Deletes existing icons before saving new ones.
   - Moves uploaded files to `CMS_DATA_PATH/directory.type/`.
3. **Deletion**:
   - Removes icons from filesystem.
   - Cleans up references in the main directory data.

#### Usage Context
- **Type Management UI**: Displayed in a table with selection controls.
- **Icons**: Supports two states ("normal" and "selected") for visual feedback.

---

### `case "select"`

Selects a directory entry.

#### Purpose
Updates the currently selected directory entry (`$object`).

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Key of the directory entry to select. |

#### Return Values
- **`$object`**: Updated to `$ifc_param`.

#### Usage Context
- Triggered by user interaction (e.g., clicking a directory entry).

---

### `case "save"`

Saves properties of a directory entry.

#### Purpose
Persists changes to a directory entry, including:
- Name (multilingual)
- Description
- URL (with canonical URL processing)
- Subtype
- Visibility flags (hidden, placeholder, dynamic)
- Image references (button, hover, active)
- Path override
- Canonical URL (multilingual)

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Name (multilingual). |
| `$ifc_param2` | `bool` | Hidden flag. |
| `$ifc_param3` | `bool` | Placeholder flag. |
| `$ifc_param4` | `bool` | Dynamic flag. |
| `$ifc_param5` | `string` | Description. |
| `$ifc_param6` | `string` | URL. |
| `$ifc_param7` | `string` | Subtype key. |
| `$ifc_param8` | `string` | Image button reference. |
| `$ifc_param9` | `string` | Image hover reference. |
| `$ifc_param10` | `string` | Image active reference. |
| `$ifc_param11` | `string` | Path override. |
| `$ifc_param12` | `string` | Canonical URL (multilingual). |

#### Return Values
- **`$ifc_response`**: `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).

#### Inner Mechanisms
1. **Canonical URL Processing**:
   - Analyzes and normalizes URLs for each enabled language.
   - Falls back to the current scheme/host if omitted.
2. **Directory Class**: Uses the `directory` class to persist changes.

#### Usage Context
- **Edit Form**: Triggered by the "Save" button in the directory entry form.

---

### `case "add"` / `"add_target"`

Prepares the UI for adding a new directory entry.

#### Purpose
Initializes the "Add Entry" form with default values and displays the target selection interface.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Target parent key (defaults to `$object`). |
| `$ifc_param1` | `string` | Default name (e.g., `CMS_L_IFC_DIRECTORY_001`). |
| `$ifc_param2` | `bool` | Hidden flag (from user cache). |
| `$ifc_param3` | `bool` | Placeholder flag (from user cache). |
| `$ifc_param4` | `bool` | Dynamic flag (from user cache). |
| `$ifc_param5` | `string` | Description. |
| `$ifc_param6` | `string` | URL. |
| `$ifc_param7` | `string` | Subtype (from user cache). |

#### Return Values
- **UI**: Renders the "Add Entry" form with FlexView target selection.

#### Inner Mechanisms
1. **User Cache**: Retrieves default values (e.g., hidden, placeholder) from `cms_cache`.
2. **FlexView Integration**: Displays the directory hierarchy for target selection.

#### Usage Context
- **Add Entry UI**: Triggered by the "Add" button in the main interface.

---

### `case "add_insert"` / `"add_append"`

Inserts or appends a new directory entry.

#### Purpose
Creates a new directory entry either:
- **Insert**: Before the target entry.
- **Append**: As a child of the target entry.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Target key (parent for append, sibling for insert). |
| `$ifc_param1` | `string` | Name (multilingual). |
| `$ifc_param2` | `bool` | Hidden flag. |
| `$ifc_param3` | `bool` | Placeholder flag. |
| `$ifc_param4` | `bool` | Dynamic flag. |
| `$ifc_param5` | `string` | Description. |
| `$ifc_param6` | `string` | URL. |
| `$ifc_param7` | `string` | Subtype. |

#### Return Values
- **`$ifc_response`**: `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).
- **`$object`**: Updated to the new entry's key.

#### Inner Mechanisms
1. **Name Handling**: Falls back to `CMS_L_UNKNOWN` if no name is provided.
2. **Directory Class**: Uses `insert()` or `append()` methods to add the entry.
3. **User Cache**: Updates cached preferences (e.g., hidden, subtype).

#### Usage Context
- Triggered by the "Save" button in the "Add Entry" form.

---

### `case "copy_insert"` / `"copy_append"` / `"cut_insert"` / `"cut_append"`

Copies or moves directory entries.

#### Purpose
Duplicates or relocates directory entries within the hierarchy.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Comma-separated string: `"source_key,target_key"`. |

#### Return Values
- **`$ifc_response`**: `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).
- **`$object`**: Updated to the target key (for insert) or parent key (for append).

#### Inner Mechanisms
1. **Data Cloning**: Uses a clone of the directory data to buffer the source entry.
2. **Cut Logic**:
   - If the target is a child of the source, buffers the source without deletion.
   - Otherwise, deletes the source from the original data.
3. **Paste Logic**:
   - **Insert**: Places the buffered entry before the target.
   - **Append**: Places the buffered entry as a child of the target.

#### Usage Context
- **Drag-and-Drop**: Triggered by FlexView drag-and-drop events.

---

### `case "sort"`

Sorts directory entries by name.

#### Purpose
Reorders child entries of the selected directory entry alphabetically by name.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Key of the parent entry to sort. |

#### Return Values
- **`$ifc_response`**: `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).

#### Inner Mechanisms
- Uses `data_sort()` to reorder entries by the `name` field.

#### Usage Context
- Triggered by the "Sort" button in the main interface.

---

### `case "clean"`

Removes empty directory entries.

#### Purpose
Deletes directory entries that:
- Are not placeholders.
- Have no URL.
- Are leaf nodes (no children).

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Key of the entry to clean (starts from this entry downward). |

#### Return Values
- **`$ifc_response`**: `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).
- **`$object`**: Updated to the nearest existing ancestor.

#### Inner Mechanisms
1. **Path Tracking**: Uses a stack to track the path to the selected entry.
2. **Depth-Limited Deletion**:
   - Traverses the hierarchy depth-first.
   - Deletes entries only if they are below the last valid entry (`$limit`).
3. **Selection Update**: Restores selection to the nearest existing ancestor.

#### Usage Context
- Triggered by the "Clean" button in the main interface.

---

### `case "delete"`

Deletes a directory entry.

#### Purpose
Removes a directory entry and updates the selection to the nearest existing ancestor.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Key of the entry to delete. |

#### Return Values
- **`$ifc_response`**: `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).
- **`$object`**: Updated to the nearest existing ancestor.

#### Inner Mechanisms
1. **Path Tracking**: Uses a stack to track the path to the deleted entry.
2. **Selection Update**: Restores selection to the nearest existing ancestor.

#### Usage Context
- **Drag-and-Drop**: Triggered by dropping an entry on the "trash bin".
- **Delete Button**: Triggered by the "Delete" command.

---

### Main Display

Renders the primary directory interface.

#### Purpose
Displays:
- The directory hierarchy (via FlexView).
- A trash bin for drag-and-drop deletion.
- The canonical URL of the selected entry.
- The edit form for the selected entry (if any).

#### Inner Mechanisms
1. **FlexView Integration**:
   - **Encoding**: Uses `qr()` for value encoding.
   - **Display**: Uses `directory_flexview_display_function()` for custom rendering.
   - **Drag-and-Drop**: Registers `directory_flexview_event()` for handling drop events.
2. **Menu**:
   - **Add**: Opens the "Add Entry" form.
   - **Show**: Opens the directory in a new tab (if `template` module is loaded).
   - **Sort**: Sorts child entries by name.
   - **Clean**: Removes empty entries.
   - **Type**: Opens the type management interface.
3. **Canonical URL**: Displays the resolved URL for the selected entry.

#### JavaScript: `directory_flexview_event()`
Handles drag-and-drop events for the FlexView hierarchy.

| Parameter | Type | Description |
|-----------|------|-------------|
| `event` | `string` | Event type (`"dropon"` or `"dropon_alt"`). |
| `source` | `DOMElement` | Dragged element. |
| `target` | `DOMElement` | Drop target. |

#### Actions
- **`dropon`**: Moves the entry (cut + insert/append).
- **`dropon_alt`**: Copies the entry (copy + insert/append).
- **Trash Bin**: Deletes the entry.

#### Usage Context
- **Primary Interface**: Loaded when accessing the directory module.

---

### Helper Functions (Referenced)

#### `directory_get_type()`
Returns the icon for a directory entry based on its type.

#### `directory_get_type_select()`
Generates a `<select>` element for directory types.

#### `directory_get_canonical($object)`
Resolves the canonical URL for a directory entry.

#### `directory_flexview_display_function($key, $data)`
Custom display function for FlexView, rendering directory entries with:
- Icons (based on type).
- Name (with language support).
- URL (if present).
- Visibility indicators (hidden, placeholder, dynamic).


<!-- HASH:d386cb767ef2cd44a46f3707464ef6cc -->
