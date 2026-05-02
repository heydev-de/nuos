# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.system.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Interface: System Data Management (`ifc.system.inc`)

This file provides the interface for managing hierarchical system data structures within the NUOS platform. It enables users to interact with a tree-like data storage (e.g., configuration settings, content hierarchies) through a web-based UI, supporting operations such as selection, addition, copying, cutting, pasting, and deletion of nodes.

The interface leverages the `flexview` library for visual representation and the `data` class for underlying data manipulation. It is context-aware, responding to messages (`CMS_IFC_MESSAGE`) sent via the interface framework (`ifc`).

---

### Initialization and Setup

#### Variables and State
| Name      | Default/Value                     | Description                                                                                     |
|-----------|-----------------------------------|-------------------------------------------------------------------------------------------------|
| `$file`   | `CMS_DATA_PATH . "#system/system.dat"` | Path to the data file. Defaults to the system data file if not provided or invalid.            |
| `$data`   | Instance of `data`                | Data handler for the specified file.                                                            |
| `$object` | `0`                               | Currently selected node in the data tree. Defaults to root if not provided or invalid.          |
| `$list`   | `[]`                              | List of selected node IDs (for multi-selection operations like delete).                         |
| `$target` | `NULL`                            | Target node for copy/cut operations.                                                            |
| `$status` | `NULL`                            | Current operation status (`"copy"`, `"cut"`).                                                   |

#### Dependencies
- **`flexview`**: Required for rendering the tree and target views. If not loaded, the interface is marked inactive.
- **Permission**: Requires `CMS_L_ACCESS` permission to proceed.

---

### Message Handling

The interface responds to the following `CMS_IFC_MESSAGE` values, each triggering a specific operation:

| Message            | Description                                                                                     |
|--------------------|-------------------------------------------------------------------------------------------------|
| `"select"`         | Selects a node. Sets `$object` to `$ifc_param` and updates the selection list.                  |
| `"save"`           | Saves changes to a node's attributes. Adds new attributes or updates existing ones.              |
| `"add"`            | Initiates the addition of a new node. Falls through to `"add_target"`.                          |
| `"add_target"`     | Displays the UI for adding a new node, allowing selection of a target parent.                   |
| `"add_insert"`     | Inserts a new node as a child of the target.                                                    |
| `"add_append"`     | Appends a new node as a sibling of the target.                                                  |
| `"copy"`           | Marks the current node for copying. Sets `$status` to `"copy"`.                                 |
| `"cut"`            | Marks the current node for cutting. Sets `$status` to `"cut"`.                                  |
| `"target"`         | Sets the target node for copy/cut operations.                                                   |
| `"copy_insert"`    | Inserts copied nodes as children of the target.                                                 |
| `"copy_append"`    | Appends copied nodes as siblings of the target.                                                 |
| `"cut_insert"`     | Inserts cut nodes as children of the target.                                                    |
| `"cut_append"`     | Appends cut nodes as siblings of the target.                                                    |
| `"delete"`         | Deletes the selected nodes.                                                                     |
| `"filemanager"`    | Records the current file in the recent file list (if `plist` is loaded).                        |

---

### Core Methods and Logic

#### `save` Operation
**Purpose**: Saves changes to a node's attributes.
**Parameters**:
- `$ifc_param1`: (Optional) New attribute name to add.
- `$ifc_param2`: (Optional) New attribute value to add.
- `$ifc_param3`, `$ifc_param4`, etc.: Attribute names to update.
- `$attribute3`, `$attribute4`, etc.: Corresponding attribute values to update.

**Mechanism**:
1. Adds a new attribute if `$ifc_param1` is provided.
2. Iterates over dynamically named parameters (`$ifc_param3`, `$attribute3`, etc.) to update existing attributes.
3. Saves the data. If the node has no attributes left, it is removed.
4. Updates `$object` to the parent if the node is deleted.

**Usage**: Triggered when the user submits the attribute form for a node.

---

#### `add_insert` / `add_append` Operations
**Purpose**: Adds a new node to the data tree.
**Parameters**:
- `$ifc_param`: Target parent node ID.
- `$ifc_param1`: (Optional) Key for the new node. Auto-generated if not provided.
- `$ifc_param2`: (Optional) Flag indicating if the new node is a container.

**Mechanism**:
1. Generates a unique ID if no key is provided.
2. Creates a buffer for the new node:
   - Non-container: `["name" => $key]`.
   - Container: `["name" => $key, "#type" => "container"]` and a closing tag `["#type" => "/container"]`.
3. Inserts or appends the buffer to the target.
4. Saves the data and updates `$object` to the new node.

**Usage**: Triggered when the user confirms the addition of a new node.

---

#### `copy_insert` / `copy_append` / `cut_insert` / `cut_append` Operations
**Purpose**: Copies or moves nodes to a new location.
**Parameters**:
- `$ifc_param`: Target node ID.
- `$list`: List of node IDs to copy or move.

**Mechanism**:
1. Uses a clone of the data structure to avoid modifying the original during traversal.
2. For each node in `$list` (processed in reverse order):
   - **Copy**: Buffers the node in the clone.
   - **Cut**: Buffers the node in the clone and removes it from the original if the target is not a descendant.
3. Transfers the buffer from the clone to the original data.
4. Inserts or appends the buffer to the target.
5. Saves the data and updates `$object` to the new node.

**Usage**: Triggered when the user pastes copied or cut nodes.

---

#### `delete` Operation
**Purpose**: Deletes selected nodes.
**Parameters**:
- `$list`: List of node IDs to delete.

**Mechanism**:
1. Builds a path from the current node to the root.
2. Deletes each node in `$list`.
3. Saves the data.
4. Selects the last existing node in the path or defaults to the root.

**Usage**: Triggered when the user confirms the deletion of selected nodes.

---

### Main Display

#### Menu
The interface provides a context-sensitive menu with the following options:

| Label                          | Message/Action       | Description                                                                                     |
|--------------------------------|----------------------|-------------------------------------------------------------------------------------------------|
| `CMS_L_COMMAND_ADD`            | `"add"`              | Initiates the addition of a new node.                                                           |
| `CMS_L_COMMAND_COPY`           | `"copy"`             | Marks the current node for copying.                                                             |
| `CMS_L_COMMAND_CUT`            | `"cut"`              | Marks the current node for cutting.                                                             |
| `CMS_L_COMMAND_DELETE_SELECTED`| `"#delete"`          | Deletes the selected nodes.                                                                     |

#### Tree View
- **Rendering**: Uses `flexview` to display the data tree.
- **Encoding**: Uses `qr()` for escaping node names.
- **Checkboxes**: Enabled for multi-selection (`$list`).
- **Actions**:
  - Clicking a node triggers `"select"`.
  - Icons indicate container nodes.

#### Target View
- **Purpose**: Displays the target selection UI for copy/cut operations.
- **Actions**:
  - Clicking a node sets it as the target (`"target"`).
  - Buttons for inserting or appending nodes.

#### Data View
- **Purpose**: Displays and edits attributes of the selected node.
- **Fields**:
  - New attribute name and value (for adding attributes).
  - Existing attributes (editable via textarea).
- **Buttons**:
  - Save: Triggers the `"save"` operation.

#### Selection Controls
- **All**: Selects all nodes.
- **Invert**: Inverts the current selection.
- **None**: Deselects all nodes.

---

### Key Functions and Helpers

#### `ifc_table_open()` / `ifc_table_close()`
**Purpose**: Outputs opening and closing HTML tags for standardized interface tables.

#### `unique_id()`
**Purpose**: Generates a unique identifier for new nodes (implementation not shown in this file).

#### `ifc_post()`
**Purpose**: JavaScript function (not PHP) used to send interface messages (e.g., `"select"`, `"add_insert"`).

#### `ifc_list_activate()` / `ifc_list_invert()` / `ifc_list_deactivate()`
**Purpose**: JavaScript functions for managing the selection state of nodes.

---

### Usage Context

#### Typical Scenarios
1. **Configuration Management**:
   - Edit hierarchical settings (e.g., site navigation, permissions).
2. **Content Hierarchies**:
   - Manage nested content structures (e.g., pages, categories).
3. **Data Organization**:
   - Rearrange or duplicate sections of data (e.g., templates, snippets).

#### Integration
- **Interface Framework**: Uses the `ifc` class to render UI elements and handle responses.
- **Data Layer**: Relies on the `data` class for persistent storage and manipulation.
- **Visualization**: Uses `flexview` for tree and target views.

#### Example Workflow
1. User selects a node in the tree.
2. User clicks "Add" to create a new child node.
3. User selects a target parent and confirms.
4. The new node is inserted, and its attributes are displayed for editing.
5. User saves the changes.


<!-- HASH:d36853c1634c93eb7a06d671abd38e38 -->
