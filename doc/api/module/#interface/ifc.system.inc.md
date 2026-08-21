# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.system.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.system.inc)

- **Version:** `26.8.8.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Interface: System Data Management (`ifc.system.inc`)

This file implements the **System Interface** for managing hierarchical data structures within the PWNC Web Platform. It provides a **visual tree-based editor** for system configuration files, allowing users to:

- **Browse** nested data structures (containers and key-value pairs)
- **Add, copy, cut, paste, and delete** entries
- **Edit** attributes (names and values) of individual entries
- **Save** changes to persistent storage

The interface is **context-aware**, responding to messages (e.g., `select`, `save`, `add`, `delete`) sent via the PWNC inter-module communication system (`CMS_IFC_MESSAGE`). It integrates with the **FlexView** component for tree visualization and the **IFC** (Interface Controller) for form generation.

---

### Core Workflow

1. **Initialization**
   - Loads the target data file (default: `#system/system.dat`)
   - Validates permissions (`CMS_L_ACCESS`)
   - Initializes the `data` object (hierarchical key-value store)

2. **Message Handling**
   - Processes incoming messages (`CMS_IFC_MESSAGE`) to modify the data structure
   - Updates UI state (`$object`, `$list`, `$status`) based on user actions

3. **UI Rendering**
   - Displays a **tree view** of the data structure (via `flexview`)
   - Shows **attribute editors** for the selected object
   - Provides **contextual menus** for operations (add, copy, cut, delete)

---

### Key Components

| Component          | Purpose                                                                 |
|--------------------|-------------------------------------------------------------------------|
| `$data`            | Instance of the `data` class (hierarchical key-value store)             |
| `$object`          | Currently selected entry (key in `$data`)                               |
| `$list`            | Array of selected entries (for bulk operations)                         |
| `$status`          | Tracks clipboard state (`"copy"` or `"cut"`)                            |
| `$file`            | Path to the data file being edited                                      |
| `flexview`         | Visualizes the tree structure                                           |
| `ifc`              | Generates forms and handles UI state                                    |

---

### Message Handlers

#### ### `select`
**Purpose**: Selects an entry in the tree.
**Parameters**:
| Parameter      | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$ifc_param`   | `string` | Key of the entry to select           |

**Mechanism**:
- Updates `$object` to the specified key
- Resets `$list` to contain only the selected entry
- Clears `$status`

**Usage**:
```php
// Triggered via JavaScript:
// ifc_post('select', 'some_key');
```

---

#### ### `save`
**Purpose**: Saves changes to the data structure.
**Parameters**:
| Parameter          | Type      | Description                                                                 |
|--------------------|-----------|-----------------------------------------------------------------------------|
| `$ifc_param1`      | `string`  | New attribute name (if provided)                                            |
| `$ifc_param2`      | `string`  | New attribute value (if `$ifc_param1` is provided)                          |
| `$attribute{N}`    | `mixed`   | Dynamic parameters for existing attributes (e.g., `$attribute3`, `$attribute4`) |

**Return**:
- `CMS_MSG_DONE` on success
- `CMS_MSG_ERROR` on failure

**Mechanism**:
1. Adds a new attribute if `$ifc_param1` is provided.
2. Updates existing attributes from dynamic parameters (`$attribute3`, `$attribute4`, etc.).
3. Saves the data file.
4. If the entry has no attributes left, it is deleted, and the parent becomes the new selection.

**Usage**:
```php
// Example: Adding/updating attributes for the selected object
// ifc_post('save', 'new_attr', 'new_value', 'attr1', 'updated_value');
```

---

#### ### `add` / `add_target`
**Purpose**: Prepares the UI for adding a new entry.
**Parameters**:
| Parameter      | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param`   | `string`  | Target parent key (default: `$object`) |
| `$ifc_param1`  | `string`  | New entry key (optional)             |
| `$ifc_param2`  | `bool`    | Whether the entry is a container     |

**Mechanism**:
- Displays a form with fields for:
  - **Key** (text input, auto-generated if empty)
  - **Container flag** (checkbox)
  - **Target selection** (via `flexview`)
- On submission, triggers `add_insert` or `add_append`.

**Usage**:
```php
// Triggered via JavaScript:
// ifc_post('add', 'parent_key');
```

---

#### ### `add_insert` / `add_append`
**Purpose**: Inserts or appends a new entry to the data structure.
**Parameters**:
| Parameter      | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param`   | `string`  | Target parent key                    |
| `$ifc_param1`  | `string`  | New entry key (auto-generated if empty) |
| `$ifc_param2`  | `bool`    | Whether the entry is a container     |

**Return**:
- `CMS_MSG_DONE` on success
- `CMS_MSG_ERROR` on failure

**Mechanism**:
1. Generates a unique key if none is provided.
2. Creates a new entry (with optional container markers).
3. Inserts/appends the entry under the target parent.
4. Saves the data file and updates `$object` to the new entry.

**Usage**:
```php
// Example: Adding a container under 'parent_key'
// ifc_post('add_insert', 'parent_key', 'new_container', true);
```

---

#### ### `copy` / `cut`
**Purpose**: Copies or cuts selected entries to the clipboard.
**Parameters**:
| Parameter      | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$object`      | `string`  | Currently selected entry (source)    |

**Mechanism**:
- Sets `$status` to `"copy"` or `"cut"`.
- Stores the selected entries (`$list`) for later pasting.

**Usage**:
```php
// Triggered via JavaScript:
// ifc_post('copy');
// ifc_post('cut');
```

---

#### ### `copy_insert` / `copy_append` / `cut_insert` / `cut_append`
**Purpose**: Pastes copied/cut entries into the data structure.
**Parameters**:
| Parameter      | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param`   | `string`  | Target parent key                    |

**Return**:
- `CMS_MSG_DONE` on success
- `CMS_MSG_ERROR` on failure

**Mechanism**:
1. Uses a **clone** of `$data` to avoid modifying the original during traversal.
2. For **cut operations**, checks if the target is a descendant of the source (to prevent cycles).
3. Pastes entries via `insert` or `append`.
4. Saves the data file and updates `$object` to the last pasted entry.

**Usage**:
```php
// Example: Pasting copied entries under 'parent_key'
// ifc_post('copy_insert', 'parent_key');
```

---

#### ### `delete`
**Purpose**: Deletes selected entries.
**Parameters**:
| Parameter      | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$list`        | `array`   | Array of keys to delete              |

**Return**:
- `CMS_MSG_DONE` on success
- `CMS_MSG_ERROR` on failure

**Mechanism**:
1. Deletes all entries in `$list`.
2. On success, selects the nearest existing ancestor of the deleted entries.
3. Saves the data file.

**Usage**:
```php
// Triggered via JavaScript:
// ifc_post('delete');
```

---

#### ### `filemanager`
**Purpose**: Updates the recent files list in the file manager.
**Mechanism**:
- Uses the `plist` class to track recently accessed files.
- Limits the list to the 10 most recent entries.

**Usage**:
```php
// Automatically triggered when the interface loads a new file.
```

---

### UI Rendering

#### Tree View
- **Component**: `flexview`
- **Features**:
  - Displays hierarchical data as an expandable tree.
  - Supports checkboxes for bulk selection.
  - Icons distinguish containers (`flexview/icon_entrycontainer`) from leaf nodes.
- **Actions**:
  - Clicking an entry triggers `select`.
  - Checkboxes update `$list`.

**Example**:
```php
$flexview->show_tree(
    $object, // Current selection
    "javascript:ifc_post('select','%index%');", // Action on click
    "name", // Key for display names
    NULL, // No special marking
    [ // Icon mappings
        "container" => "flexview/icon_entrycontainer",
        "+container" => "flexview/icon_entryopen"
    ]
);
```

---

#### Target Selection
- **Purpose**: Allows users to choose a destination for paste operations.
- **Component**: `flexview->show_target()`
- **Actions**:
  - Clicking an entry sets it as the target.
  - Buttons trigger `*_insert` or `*_append`.

**Example**:
```php
$flexview->show_target(
    $target, // Current target
    "javascript:ifc_post('target','%index%');", // Set target
    "javascript:ifc_post('copy_insert','%index%');", // Insert
    "javascript:ifc_post('copy_append','%index%');" // Append
);
```

---

#### Attribute Editor
- **Purpose**: Edits attributes of the selected entry.
- **Fields**:
  - **New attribute name** (text input)
  - **New attribute value** (textarea)
  - **Existing attributes** (dynamically generated textareas)
- **Actions**:
  - Clicking "Save" triggers the `save` message.

**Example**:
```php
// Dynamically generated for each attribute
$ifc->param("attribute3", "attr_name");
$ifc->set("attr_name", "textarea 50x4 bl", "attr_value");
```

---

### Constants and Labels

| Constant                     | Description                          |
|------------------------------|--------------------------------------|
| `CMS_L_ACCESS`               | Permission required to access the interface |
| `CMS_L_COMMAND_ADD`          | Label for "Add" button               |
| `CMS_L_COMMAND_COPY`         | Label for "Copy" button              |
| `CMS_L_COMMAND_CUT`          | Label for "Cut" button               |
| `CMS_L_COMMAND_DELETE_SELECTED` | Label for "Delete Selected" button |
| `CMS_L_IFC_SYSTEM_001`       | Label for "New Attribute Name"       |
| `CMS_L_IFC_SYSTEM_002`       | Label for "Container" checkbox       |
| `CMS_L_IFC_SYSTEM_006`       | Label for "Data" column              |
| `CMS_L_IFC_SYSTEM_007`       | Label for "New Attribute Value"      |
| `CMS_L_IFC_SYSTEM_008`       | Label for "Target" column            |
| `CMS_L_IFC_SYSTEM_009`       | Label for "Key" input                |

---

### Usage Example

#### Scenario: Adding a New System Setting
1. **Navigate** to the system interface (`/system`).
2. **Select** the parent container (e.g., `settings`).
3. **Click** "Add" in the menu.
4. **Enter** a key (e.g., `max_upload_size`) and value (e.g., `10M`).
5. **Click** "Save" to persist the change.

**Code Flow**:
1. `add` message prepares the form.
2. User submits the form, triggering `add_insert`.
3. The new entry is inserted under `settings`.
4. `save` message updates the attributes.

---

### Error Handling
- **Missing Data File**: Falls back to `#system/system.dat`.
- **Invalid Object**: Resets to the root (`0`).
- **Save Failures**: Returns `CMS_MSG_ERROR` and retains unsaved changes.
- **Circular References**: Prevents cutting a parent into its own child.

---

### Dependencies
| Dependency       | Purpose                                                                 |
|------------------|-------------------------------------------------------------------------|
| `flexview`       | Tree visualization                                                     |
| `ifc`            | Form generation and UI state management                                 |
| `data`           | Hierarchical key-value storage                                         |
| `plist`          | Recent files tracking (for `filemanager`)                               |
| `cms_cache`      | Temporary caching of data structures                                    |
| `cms_load`       | Library loading                                                         |

---

### Best Practices
1. **Backup Data**: Always back up system files before editing.
2. **Bulk Operations**: Use checkboxes to select multiple entries for copy/cut/delete.
3. **Containers**: Mark entries as containers if they will hold child entries.
4. **Unique Keys**: Let the system auto-generate keys for new entries to avoid conflicts.
5. **Permissions**: Ensure the user has `CMS_L_ACCESS` permission.


<!-- HASH:afa0ffbf59a16447dbddce4f8e7fd1a1 -->
