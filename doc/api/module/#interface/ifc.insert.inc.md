# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.insert.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Interface: Insert Management (`ifc.insert.inc`)

This file provides the interface for managing reusable HTML/Code insert snippets within the NUOS platform. It handles creation, modification, deletion, and assignment of these snippets to container objects in the system.

The interface supports two primary contexts:
1. **Insert Management** – Create, edit, and delete code snippets (stored in `#system/insert.code`).
2. **Container Assignment** – Assign snippets to hierarchical container objects (stored in `#system/insert`).

---

## Core Logic Flow

### Initialization
- Loads required libraries (`flexview`).
- Checks user permissions (`CMS_L_ACCESS`).
- Initializes or restores the currently selected object and list from cache or request state.

### Message Handling
The interface responds to predefined messages (`CMS_IFC_MESSAGE`) to perform specific actions:
- **Insert Management**: `insert`, `insert_select`, `insert_display`, `insert_add`, `insert_save`, `insert_delete`
- **Container Management**: `select`, `add`, `_add`, `delete`, `save`

---

## Message Handlers

### `insert` (and related sub-messages)
Handles the lifecycle of code snippets.

#### Data Source
- Uses `data("#system/insert.code")` to manage snippets.

#### Sub-Messages

##### `insert_select`
**Purpose**: Selects a snippet for editing.
**Parameters**:
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$ifc_param`    | `string` | Snippet key (identifier).            |

**Mechanism**: Sets `$insert_object` to the provided key.

---

##### `insert_display`
**Purpose**: Renders a preview of the selected snippet.
**Parameters**:
| Name              | Type     | Description                          |
|-------------------|----------|--------------------------------------|
| `$insert_object`  | `string` | Snippet key.                         |

**Mechanism**:
- Retrieves the snippet's `code` and `html` flag.
- If `html` is set, renders the code directly; otherwise, parses it as text.
- Outputs the result and exits.

---

##### `insert_add`
**Purpose**: Creates a new snippet with a default name.
**Mechanism**:
- Buffers a new entry with the name `CMS_L_IFC_INSERT_002` (localized "New Insert").
- Appends it to the data store and saves.
- On success, sets `$insert_object` to the new key and returns `CMS_MSG_DONE`.
- On failure, returns `CMS_MSG_ERROR`.

---

##### `insert_save`
**Purpose**: Saves changes to a snippet.
**Parameters**:
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$ifc_param1`   | `string` | Snippet name (trimmed).              |
| `$ifc_param2`   | `bool`   | HTML flag presence (checkbox).       |
| `$ifc_param3`   | `string` | Snippet code.                        |

**Mechanism**:
- If no name is provided, retains the existing name.
- Updates the snippet's `name`, `html`, and `code` fields.
- Saves the data store and returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

---

##### `insert_delete`
**Purpose**: Deletes selected snippets and cleans up references.
**Parameters**:
| Name      | Type       | Description                          |
|-----------|------------|--------------------------------------|
| `$_list`  | `string[]` | Array of snippet keys to delete.     |

**Mechanism**:
- Deletes each snippet in `$_list`.
- Removes references to deleted snippets from `#system/insert` (container assignments).
- If the currently selected snippet is deleted, deselects it.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

---

### `select`
**Purpose**: Selects a container object for assignment.
**Parameters**:
| Name         | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `$ifc_param` | `string` | Container key.                       |

**Mechanism**: Sets `$object` and `$list` to the provided key.

---

### `_add` (Internal Handler)
**Purpose**: Processes the addition of a new container object.
**Parameters**:
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$ifc_param1`   | `string` | Container name (trimmed).            |

**Mechanism**:
- Splits the name by `.` to create a hierarchical path (e.g., `parent.child`).
- For each segment, checks if it exists; if not, creates a new container.
- On success, sets `$object` and `$list` to the new key and returns `CMS_MSG_DONE`.
- On failure, returns `CMS_MSG_ERROR`.

---

### `add`
**Purpose**: Displays the form for adding a new container.
**Mechanism**:
- Renders an `ifc` form with a text input for the container name.
- Submits to `_add` for processing.

---

### `delete`
**Purpose**: Deletes selected container objects.
**Parameters**:
| Name    | Type       | Description                          |
|---------|------------|--------------------------------------|
| `$list` | `string[]` | Array of container keys to delete.   |

**Mechanism**:
- Deletes each container in `$list`.
- Traverses the hierarchy to find the nearest existing parent for reselection.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

---

### `save`
**Purpose**: Assigns a snippet to the selected container.
**Parameters**:
| Name              | Type     | Description                          |
|-------------------|----------|--------------------------------------|
| `$insert_object`  | `string` | Snippet key.                         |
| `$object`         | `string` | Container key.                       |

**Mechanism**:
- Updates the container's `insert` field with the snippet key.
- Saves the data store and returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

---

## Main Display

### Container Tree
- Uses `flexview` to render a hierarchical tree of containers.
- Allows selection of containers via `ifc_post('select', '%index%')`.
- Supports checkbox selection for bulk operations.

### Snippet Assignment
- Displays a dropdown of all available snippets (sorted alphabetically).
- Provides buttons to preview or save the assignment.

### UI Components
- **Menu**: Add, code management, and delete operations.
- **Tables**: Layout for tree and snippet assignment panels.
- **Selection Controls**: Buttons to select all, invert, or deselect items.

---

## Key Functions and Classes

### `ifc` (Interface Class)
**Purpose**: Renders forms and UI elements.
**Usage**:
- Initialized with response code, page, menu, data, and title.
- Methods like `set()` generate form fields (text, checkbox, select, buttons).

### `data` (Data Class)
**Purpose**: Manages structured data storage.
**Usage**:
- Loads data from `#system/insert` or `#system/insert.code`.
- Methods: `get()`, `set()`, `del()`, `append()`, `save()`, `move()`.

### `flexview` (Flexible View Class)
**Purpose**: Renders hierarchical data as a tree.
**Usage**:
- `import_data()`: Loads data.
- `set_encoding_function()`: Sets the URL encoder (e.g., `qr`).
- `show_tree()`: Renders the tree with actions and names.

---

## Constants and Localization
- **Labels**: Localized strings (e.g., `CMS_L_IFC_INSERT_002`, `CMS_L_COMMAND_ADD`).
- **Messages**: Status codes (`CMS_MSG_DONE`, `CMS_MSG_ERROR`).

---

## Usage Scenarios

### Managing Snippets
1. Navigate to the "Insert" interface.
2. Use the "Add" button to create a new snippet.
3. Edit the snippet's name, HTML flag, and code.
4. Preview or save changes.

### Assigning Snippets to Containers
1. Select a container from the tree.
2. Choose a snippet from the dropdown.
3. Click "Save" to assign the snippet to the container.

### Bulk Operations
- Use checkboxes to select multiple snippets or containers.
- Use the "Delete Selected" button to remove them.

---

## Notes
- **Caching**: The selected object is cached per user (`insert.{CMS_USER}.object`).
- **Hierarchy**: Containers support nested paths (e.g., `parent.child.grandchild`).
- **Security**: All user input is escaped (e.g., `x()`, `qrx()`).


<!-- HASH:5e561c159a7aa81f55ada73e982c9f59 -->
