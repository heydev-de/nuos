# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.directory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.directory.inc)

- **Version:** `26.8.14.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Directory Interface Module (`ifc.directory.inc`)

This file implements the **Directory Management Interface** for the PWNC Web Platform. It provides a hierarchical, drag-and-drop enabled UI for managing website navigation structures (menus, sitemaps, etc.), including:

- **Tree-based directory structure** (containers and links)
- **Type management** (icons, visual styling)
- **URL handling** (canonical URLs, external links)
- **Multilingual support** (language-specific URLs and names)
- **Drag-and-drop operations** (copy/move/cut/paste/delete)

The interface integrates with the `directory` and `flexview` modules to provide a visual, interactive editing experience.

---

## Core Functionality

### Message Handling
The interface processes messages (actions) via `CMS_IFC_MESSAGE`, triggering different workflows:

| Message | Purpose |
|---------|---------|
| `type`, `type_select`, `type_add`, `type_save`, `type_delete` | Manage directory **types** (visual styles/icons) |
| `select` | Select a directory entry |
| `save` | Save directory entry data |
| `add`, `add_target`, `add_insert`, `add_append` | Add new directory entries |
| `copy_insert`, `copy_append`, `cut_insert`, `cut_append` | Copy/move entries via drag-and-drop |
| `sort` | Sort entries alphabetically |
| `clean` | Remove empty/placeholder entries |
| `del` | Delete entries |

---

## Key Functions and Workflows

### `type` Message Handling
Manages **directory type definitions** (visual styles/icons).

#### Workflow
1. **Initialization**: Loads the selected type from cache or data.
2. **Display**: Shows a table of all types with their icons.
3. **Actions**:
   - **Add**: Creates a new type with a default name.
   - **Save**: Updates type name and icons (supports GIF/JPG/PNG/SVG/WEBP).
   - **Delete**: Removes selected types and their icons.

#### Example: Adding a New Type
```php
// Triggered via UI button "Add Type"
case "type_add":
    $data = new data("#system/directory.type");
    $data->set_buffer([["name" => CMS_L_IFC_DIRECTORY_017]]);
    $_type_object = $data->append();
    $data->set($_type_object, $_type_object, "#subtype");
    if ($data->save()) {
        $type_object = $_type_object;
        $ifc_response = CMS_MSG_DONE;
    }
    break;
```
**Usage**: Click "Add" in the type management UI to create a new type with a default name.

---

### `save` Message Handling
Saves directory entry data, including **multilingual canonical URLs**.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Directory entry key |
| `$ifc_param1` | `string` | Name (multilingual) |
| `$ifc_param5` | `string` | Description (multilingual) |
| `$ifc_param6` | `string` | URL |
| `$ifc_param7` | `string` | Subtype (visual style) |
| `$ifc_param8` | `string` | Canonical URL (multilingual) |
| `$ifc_param9` | `string` | Subtype (repeated for legacy) |
| `$ifc_param2` | `bool` | Hidden flag |
| `$ifc_param3` | `bool` | Placeholder flag |
| `$ifc_param4` | `bool` | Dynamic flag |
| `$ifc_param10` | `string` | Image (button) |
| `$ifc_param11` | `string` | Image (hover) |
| `$ifc_param12` | `string` | Image (active) |

#### Inner Mechanisms
1. **Canonical URL Processing**:
   - Splits multilingual URLs into language-specific entries.
   - Completes partial URLs (e.g., `/path` → `https://example.com/path`).
2. **Data Storage**:
   - Uses the `directory` class to persist changes.

#### Example: Saving a Directory Entry
```php
case "save":
    $directory = new directory();
    $directory->set(
        $object,          // Key
        $ifc_param1,      // Name
        $ifc_param5,      // Description
        $ifc_param6,      // URL
        $ifc_param9,      // Subtype
        isset($ifc_param2), // Hidden
        isset($ifc_param3), // Placeholder
        isset($ifc_param4), // Dynamic
        $ifc_param10,     // Image (button)
        $ifc_param11,     // Image (hover)
        $ifc_param12,     // Image (active)
        NULL,             // Path (overwritten)
        $ifc_param8       // Canonical URL
    );
    $ifc_response = $directory->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
**Usage**: Fill out the directory entry form and click "Save".

---

### `add_insert` / `add_append` Message Handling
Adds a new directory entry **before** (`insert`) or **after** (`append`) a target.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Target key (parent container) |
| `$ifc_param1` | `string` | Name (multilingual) |
| `$ifc_param5` | `string` | Description (multilingual) |
| `$ifc_param6` | `string` | URL |
| `$ifc_param7` | `string` | Subtype |
| `$ifc_param2` | `bool` | Hidden flag |
| `$ifc_param3` | `bool` | Placeholder flag |
| `$ifc_param4` | `bool` | Dynamic flag |

#### Inner Mechanisms
1. **Name Handling**:
   - If no name is provided, defaults to `CMS_L_UNKNOWN`.
2. **Data Persistence**:
   - Uses `directory->insert()` or `directory->append()`.
   - Caches user preferences (e.g., hidden/placeholder flags).

#### Example: Adding a New Entry
```php
case "add_insert":
    $directory = new directory();
    $_object = $directory->insert(
        $ifc_param,      // Target key
        $ifc_param1,     // Name
        $ifc_param5,     // Description
        $ifc_param6,     // URL
        $ifc_param7,     // Subtype
        isset($ifc_param2), // Hidden
        isset($ifc_param3), // Placeholder
        isset($ifc_param4)  // Dynamic
    );
    if ($directory->save()) {
        $object = $_object;
        $ifc_response = CMS_MSG_DONE;
        cms_cache("directory." . CMS_USER . ".hidden", isset($ifc_param2), TRUE);
    }
    break;
```
**Usage**: Drag-and-drop a new entry into the hierarchy or use the "Add" button.

---

### `copy_insert` / `copy_append` / `cut_insert` / `cut_append` Message Handling
Handles **drag-and-drop operations** for copying/moving entries.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Comma-separated: `source_key,target_key` |

#### Inner Mechanisms
1. **Cloning**:
   - Uses a clone of the directory data to buffer the source entry.
2. **Cut vs. Copy**:
   - **Cut**: Removes the source entry from the original location.
   - **Copy**: Retains the source entry.
3. **Paste**:
   - Inserts/appends the buffered entry at the target.

#### Example: Moving an Entry
```php
case "cut_insert":
    $directory = new directory();
    $data = clone $directory->data;
    $ifc_param = explode(",", $ifc_param);
    $value = $ifc_param[0]; // Source
    $ifc_param = $ifc_param[1]; // Target
    if (!$directory->data->is_child($ifc_param, $value)) {
        $directory->data->del($value); // Remove from original location
    }
    $data->cut($value); // Buffer the entry
    $directory->data->buffer = $data->buffer;
    $_object = $directory->data->insert($ifc_param); // Insert at target
    if ($directory->save()) {
        $object = $ifc_param; // Update selection
        $ifc_response = CMS_MSG_DONE;
    }
    break;
```
**Usage**: Drag an entry to a new location in the hierarchy.

---

### `sort` Message Handling
Sorts directory entries **alphabetically by name** under a selected parent.

#### Inner Mechanisms
1. **Data Sorting**:
   - Uses `data_sort()` to reorder entries.
2. **Persistence**:
   - Saves the reordered data structure.

#### Example: Sorting Entries
```php
case "sort":
    if (stre($object)) break; // Not allowed for root
    $directory = new directory();
    data_sort($directory->data, "name", $object);
    $ifc_response = $directory->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
**Usage**: Select a parent entry and click "Sort" in the UI.

---

### `clean` Message Handling
Removes **empty or placeholder entries** from a directory branch.

#### Inner Mechanisms
1. **Path Traversal**:
   - Walks the directory tree to identify removable entries.
2. **Deletion**:
   - Removes entries that are:
     - Not placeholders.
     - Have no URL.
3. **Selection Update**:
   - Selects the nearest valid parent after cleanup.

#### Example: Cleaning a Branch
```php
case "clean":
    $directory = new directory();
    $stack = [];
    $_object = $object;
    while ($_object = $directory->data->move("parent", $_object)) {
        array_unshift($stack, $_object);
    }
    // ... (traversal logic)
    if ($directory->save()) {
        while (!$directory->data->get($object)) {
            $object = array_pop($stack);
        }
        $ifc_response = CMS_MSG_DONE;
    }
    break;
```
**Usage**: Select a branch and click "Clean" to remove empty entries.

---

### `del` Message Handling
Deletes a directory entry and updates the selection.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Key of the entry to delete |

#### Inner Mechanisms
1. **Path Traversal**:
   - Builds a stack of parent keys to handle selection updates.
2. **Deletion**:
   - Uses `directory->del()` to remove the entry.
3. **Selection Update**:
   - Selects the nearest valid parent after deletion.

#### Example: Deleting an Entry
```php
case "del":
    $directory = new directory();
    $stack = [];
    $_object = $object;
    while ($_object = $directory->data->move("parent", $_object)) {
        array_unshift($stack, $_object);
    }
    $directory->del($ifc_param);
    if ($directory->save()) {
        while (!$directory->data->get($object)) {
            $object = array_pop($stack);
        }
        $ifc_response = CMS_MSG_DONE;
    }
    break;
```
**Usage**: Drag an entry to the trash bin or use the "Delete" button.

---

## Helper Functions

### `directory_get_type()`
Retrieves the **icon path** for a directory entry based on its subtype.

#### Return Value
| Type | Description |
|------|-------------|
| `string` | URL to the icon (e.g., `directory.type/123.png`) |

#### Example
```php
$icon = directory_get_type();
```
**Usage**: Displays the icon for a directory entry in the hierarchy.

---

### `directory_get_type_select()`
Generates a **dropdown select** of all available directory types.

#### Return Value
| Type | Description |
|------|-------------|
| `string` | HTML `<select>` element with type options |

#### Example
```php
$select = directory_get_type_select();
```
**Usage**: Renders a type selection dropdown in the directory entry form.

---

### `directory_get_canonical($object)`
Retrieves the **canonical URL** for a directory entry.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Directory entry key |

#### Return Value
| Type | Description |
|------|-------------|
| `string` | Canonical URL (e.g., `https://example.com/page`) |

#### Example
```php
$url = directory_get_canonical("home");
```
**Usage**: Displays the canonical URL in the directory entry form.

---

## UI Components

### FlexView Integration
The interface uses the `flexview` module to render the **hierarchical directory tree** with drag-and-drop support.

#### Key Methods
| Method | Purpose |
|--------|---------|
| `set_encoding_function()` | Sets the encoding function for data (e.g., `qr` for URL encoding). |
| `set_display_function()` | Sets the function to render each entry (e.g., `directory_flexview_display_function`). |
| `show_hierarchy()` | Renders the tree structure. |
| `show_target()` | Renders a target selection UI for adding entries. |

#### Example: Rendering the Directory Tree
```php
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
$flexview->show_hierarchy(
    $object, // Selected entry
    "javascript:ifc_post('select','%index%');", // Click action
    "name", // Name key
    NULL, // Mark (unused)
    directory_get_type(), // Icon
    "", // Base (unused)
    "directory_flexview_event" // Drag-and-drop handler
);
```
**Usage**: Displays the interactive directory tree in the UI.

---

### Drag-and-Drop Events
Handled via JavaScript (`directory_flexview_event`).

#### Event Types
| Event | Description |
|-------|-------------|
| `dropon` | Triggered on drop (moves/cuts the entry). |
| `dropon_alt` | Triggered on drop with modifier key (copies the entry). |

#### Example: Event Handler
```javascript
function directory_flexview_event(event, source, target) {
    var source_id = source.getAttribute("data-fv-hir-index");
    var target_id = target.getAttribute("data-fv-hir-index");
    var action = target.classList.contains("fv-hir-source") ? "insert" : "append";
    switch (event) {
        case "dropon":
            if (target.id === "trashbin") ifc_post("del", source_id);
            else if (source_id !== target_id) ifc_post("cut_" + action, source_id + "," + target_id);
            break;
        case "dropon_alt":
            ifc_post("copy_" + action, source_id + "," + target_id);
            break;
    }
}
```
**Usage**: Enables drag-and-drop operations in the UI.

---

## Constants and Labels
The interface uses **language constants** for UI labels (e.g., `CMS_L_IFC_DIRECTORY_001` for "New Entry"). These are defined in the platform's language files.

---

## Usage Scenarios

### 1. Creating a Navigation Menu
1. **Add Entries**:
   - Use the "Add" button to create top-level entries (e.g., "Home", "About").
   - Drag-and-drop to nest entries (e.g., "Team" under "About").
2. **Set URLs**:
   - Enter internal paths (e.g., `/about/team`) or external URLs.
3. **Assign Types**:
   - Select a visual style (e.g., "Dropdown Menu") for each entry.
4. **Save**:
   - Click "Save" to persist changes.

### 2. Managing Multilingual URLs
1. **Edit Canonical URLs**:
   - Open the "Canonical Base" field in the directory entry form.
   - Enter language-specific URLs (e.g., `/en/about`, `/de/ueber-uns`).
2. **Save**:
   - The system automatically completes partial URLs (e.g., `/about` → `https://example.com/about`).

### 3. Cleaning Up Empty Entries
1. **Select a Branch**:
   - Click on a parent entry (e.g., "Products").
2. **Clean**:
   - Click "Clean" to remove empty or placeholder entries.
3. **Verify**:
   - The UI updates to show only valid entries.

---

## Dependencies
| Module | Purpose |
|--------|---------|
| `directory` | Core directory data management. |
| `flexview` | Hierarchical tree rendering and drag-and-drop. |
| `data` | Data storage and manipulation. |
| `ifc` | Interface component rendering. |

---

## Caching
- **User-Specific Cache**:
  - `directory.{CMS_USER}.object`: Stores the currently selected directory entry.
  - `directory.{CMS_USER}.hidden`: Caches the "hidden" flag preference.
  - `directory.{CMS_USER}.placeholder`: Caches the "placeholder" flag preference.
  - `directory.{CMS_USER}.dynamic`: Caches the "dynamic" flag preference.
  - `directory.{CMS_USER}.subtype`: Caches the last used subtype.


<!-- HASH:498a10a395532bf817ff6af99688cffa -->
