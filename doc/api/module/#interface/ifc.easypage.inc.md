# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.easypage.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## EasyPage Module Interface (`ifc.easypage.inc`)

The EasyPage module provides a user interface for managing hierarchical directory structures (navigation trees) and their associated content entries. It enables users to create, modify, delete, and reorganize directory nodes, link them to content entries, and edit metadata. The module integrates with the NUOS content, directory, flexview, and template subsystems.

---

### Dependencies and Initialization

#### Loaded Libraries
| Library      | Purpose                                                                 |
|--------------|-------------------------------------------------------------------------|
| `content`    | Manages content entries (creation, editing, publishing, deletion).      |
| `directory`  | Manages hierarchical directory structures (navigation trees).           |
| `flexview`   | Provides interactive hierarchical data display and drag-and-drop.      |
| `template`   | Manages content templates and preview functionality.                   |

#### Initial Checks
- Verifies user has **CMS_L_ACCESS** permission.
- Initializes the `content` object and checks if content management is enabled.
- Retrieves the currently selected directory object from cache (`directory.<user>.object`).

---

### Message Handling

The module responds to various interface messages (`CMS_IFC_MESSAGE`) to perform actions.

---

#### ### `select`
**Purpose:**
Sets the currently selected directory object.

**Parameters:**
| Parameter   | Type   | Description                          |
|-------------|--------|--------------------------------------|
| `$ifc_param`| string | Directory index to select.           |

**Return Values:**
None.

**Mechanism:**
Updates the `$object` variable to the provided directory index.

**Usage:**
Triggered when a user clicks on a directory node in the hierarchy.

---

#### ### `add` / `add_target`
**Purpose:**
Prepares and displays the interface for adding a new directory entry.

**Parameters:**
| Parameter    | Type    | Description                                                                 |
|--------------|---------|-----------------------------------------------------------------------------|
| `$ifc_param` | string  | Parent directory index (for `add_target`).                                  |
| `$ifc_param1`| string  | Default name for the new entry.                                             |
| `$ifc_param2`| bool    | Whether the entry should be hidden (default: user's cached hidden setting). |
| `$ifc_param3`| string  | Template identifier for the linked content.                                 |

**Return Values:**
None (outputs HTML/JS interface).

**Mechanism:**
- For `add`: Sets default parameters and falls through to `add_target`.
- For `add_target`: Displays a form with:
  - A text field for the entry name.
  - A checkbox for hidden status.
  - A template selector with preview functionality.
  - A flexview panel showing the directory hierarchy for target selection.

**Template Preview JavaScript:**
- `easypage_template_preview()`: Opens a popup preview of the selected template.

**Usage:**
Triggered when the user clicks the "Add" button.

---

#### ### `add_insert` / `add_append`
**Purpose:**
Creates a new directory entry and optionally links it to a content entry.

**Parameters:**
| Parameter    | Type    | Description                                                                 |
|--------------|---------|-----------------------------------------------------------------------------|
| `$ifc_param` | string  | Parent directory index.                                                     |
| `$ifc_param1`| string  | Entry name.                                                                 |
| `$ifc_param2`| bool    | Hidden status.                                                              |
| `$ifc_param3`| string  | Template identifier.                                                        |

**Return Values:**
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism:**
1. Sets a temporary `writer` permission on the `content` object.
2. Creates a new content entry using the provided template.
3. Sets temporary `editor` and `publisher` permissions.
4. Publishes the content and links it to the directory:
   - `add_insert`: Inserts the entry at the target position.
   - `add_append`: Appends the entry to the target.
5. Updates the directory's hidden status and caches the template selection.
6. Updates the `$object` to the new content index.

**Usage:**
Triggered when the user confirms the "Add" form.

---

#### ### `save`
**Purpose:**
Saves metadata changes for the selected directory entry.

**Parameters:**
| Parameter    | Type    | Description                                                                 |
|--------------|---------|-----------------------------------------------------------------------------|
| `$ifc_param1`| string  | New name for the entry.                                                     |
| `$ifc_param2`| bool    | Hidden status.                                                              |
| `$ifc_param3`| string  | Page title (for linked content).                                            |
| `$ifc_param4`| string  | Description (for linked content).                                           |
| `$ifc_param5`| string  | Keywords (for linked content).                                              |

**Return Values:**
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism:**
1. Updates the directory entry's name and hidden status.
2. If the entry links to content (`content://` URL):
   - Extracts the content index.
   - Updates the content's title, description, and keywords in the database.
3. Saves the directory changes.

**Usage:**
Triggered when the user clicks the "Save" button.

---

#### ### `delete`
**Purpose:**
Deletes a directory entry and its entire subtree, including linked content.

**Parameters:**
| Parameter   | Type   | Description                          |
|-------------|--------|--------------------------------------|
| `$ifc_param`| string | Directory index to delete.           |

**Return Values:**
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism:**
1. Traverses the directory tree to collect:
   - All linked content entries (`content://` URLs).
   - All scheduled publish/withdraw actions for the subtree.
2. Deletes the directory subtree.
3. For each linked content entry:
   - Verifies it is not linked elsewhere in the directory.
   - Deletes the content entry (with temporary `publisher` permission).
4. Deletes all scheduled actions for the subtree.
5. Updates `$object` to the nearest existing ancestor.

**Usage:**
Triggered when a user drags an entry to the trash bin or confirms deletion.

---

#### ### `insert` / `append`
**Purpose:**
Moves a directory entry to a new position in the hierarchy.

**Parameters:**
| Parameter   | Type   | Description                          |
|-------------|--------|--------------------------------------|
| `$ifc_param`| string | Comma-separated: `source_index,target_index`. |

**Return Values:**
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism:**
1. Splits the parameter into source and target indices.
2. Verifies the target is not a descendant of the source (prevents cycles).
3. Moves the source entry:
   - `insert`: Places it before the target.
   - `append`: Places it after the target.
4. Updates `$object` to the moved entry's new index.

**Usage:**
Triggered via drag-and-drop in the hierarchy.

---

#### ### `template_preview`
**Purpose:**
Displays a preview of the selected template.

**Parameters:**
| Parameter   | Type   | Description                          |
|-------------|--------|--------------------------------------|
| `$object`   | string | Content index for context.           |

**Return Values:**
None (exits after rendering).

**Mechanism:**
Calls `template_preview()` with the current object context.

**Usage:**
Triggered by the template preview button.

---

### Main Display

#### Hierarchy Panel
- Displays the directory hierarchy using `flexview->show_hierarchy()`.
- Supports drag-and-drop for reordering and deletion.
- JavaScript event handler (`directory_easypage_event`) manages:
  - `dropon`: Moves entries or deletes them (if dropped on trash bin).
  - `dropon_alt`: Alternative drop action (e.g., for touch devices).

#### Data Panel (for selected entry)
- Displays metadata for the selected directory entry:
  - **Name**: Text field for the entry name.
  - **Hidden**: Checkbox to hide the entry from navigation.
  - **Linked Content Metadata** (if URL starts with `content://`):
    - **Page Title**: Text field.
    - **Description**: Rich-text editor.
    - **Keywords**: Text field.
- Buttons:
  - **Edit**: Opens the linked content for editing.
  - **Save**: Saves metadata changes.

#### Trash Bin
- Visual target for drag-and-drop deletion.
- Registered as a drop zone with `dd_register()`.

---

### Key Functions and Helpers

#### `directory_flexview_display_function()`
- Custom display function for flexview, rendering directory entries with icons and names.

#### `directory_get_type()`
- Returns the icon type for a directory entry based on its properties.

#### `content_template_select()`
- Generates a `<select>` element for template selection.

#### Caching
- `cms_cache("directory.<user>.object", $object, TRUE)`: Persists the selected object across requests.
- `cms_cache("directory.<user>.hidden", ...)`: Caches the user's hidden preference.
- `cms_cache("template.<user>.page", ...)`: Caches the user's template preference.

---

### Usage Context

#### Typical Scenarios
1. **Navigation Management**:
   - Create, rename, or delete navigation nodes.
   - Reorder nodes via drag-and-drop.
   - Hide nodes from navigation.

2. **Content Linking**:
   - Link directory nodes to content entries.
   - Edit metadata (title, description, keywords) for linked content.

3. **Template Selection**:
   - Choose templates for new content entries.
   - Preview templates before selection.

#### Integration Points
- **Content Module**: Manages linked content entries.
- **Directory Module**: Manages the hierarchical structure.
- **Flexview Module**: Provides interactive display and drag-and-drop.
- **Template Module**: Manages content templates and previews.

#### Permissions
- Requires `CMS_L_ACCESS` for basic access.
- Temporary permissions (`writer`, `editor`, `publisher`) are granted for specific actions.


<!-- HASH:7a565322fd7a943f373458d071cd7ae4 -->
