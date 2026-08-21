# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.easypage.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.easypage.inc)

- **Version:** `26.8.11.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## EasyPage Interface Module

The `ifc.easypage.inc` file provides a user-friendly interface for managing website content structure through a hierarchical directory system. It enables users to create, modify, delete, and reorganize content pages while maintaining relationships between them. The module integrates with the PWNC content, directory, flexview, and template systems to offer a drag-and-drop interface for content organization.

### Overview

This module handles:
- **Content Structure Management**: Creation, deletion, and reorganization of content pages in a hierarchical directory.
- **Template Assignment**: Selection and preview of templates for content pages.
- **Metadata Management**: Editing of page titles, descriptions, and keywords.
- **Drag-and-Drop Interface**: Visual reorganization of content structure with immediate feedback.
- **Content Linking**: Association of directory entries with underlying content items.

---

### Constants and Dependencies

| Name | Value/Default | Description |
|------|---------------|-------------|
| **Dependencies** | `content`, `directory`, `flexview`, `template` | Required PWNC modules for functionality. |
| **Permissions** | `CMS_L_ACCESS` | Minimum permission level required to access the interface. |

---

### Message Handling

The module processes various interface messages to perform actions. Each message corresponds to a specific user interaction.

#### `case "select"`

**Purpose**:
Selects a directory object for viewing or editing.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `mixed` | The object index to select. |

**Return Values**:
None (modifies `$object` directly).

**Inner Mechanisms**:
- Updates the current object context to the selected directory entry.

**Usage Context**:
Triggered when a user clicks on a directory entry in the hierarchy.

---

#### `case "add"`

**Purpose**:
Prepares the interface for adding a new content page.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `mixed` | The parent directory object index. |
| `$ifc_param1` | `string` | Default name for the new page. |
| `$ifc_param2` | `bool` | Whether the page should be hidden. |
| `$ifc_param3` | `string` | Template identifier for the new page. |

**Return Values**:
None (sets up the interface for user input).

**Inner Mechanisms**:
- Initializes the flexview for directory selection.
- Sets up form fields for name, visibility, and template selection.
- Provides a preview button for the selected template.

**Usage Context**:
Triggered when the user clicks the "Add" button in the interface.

**Example**:
```php
// User clicks "Add" under a directory entry with index 123.
// The interface displays a form to input the new page's name, visibility, and template.
```

---

#### `case "add_target"`

**Purpose**:
Displays the target selection interface for placing a new content page.

**Parameters**:
None (uses `$ifc_param` from context).

**Return Values**:
None (renders the interface).

**Inner Mechanisms**:
- Uses `flexview` to display the directory hierarchy for target selection.
- Allows the user to choose where to insert or append the new page.

**Usage Context**:
Follows the "add" message to let the user specify the new page's location.

---

#### `case "add_insert" / "add_append"`

**Purpose**:
Creates a new content page at the specified location (insert or append).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Comma-separated string: `"<new_page_index>,<target_index>"`. |

**Return Values**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
1. Extracts the new page and target indices from `$ifc_param`.
2. Validates that the target is not a child of the new page (prevents circular references).
3. Uses the `directory` module to cut and paste the new page into the target location.
4. Updates the `$object` context to the new page's index.

**Usage Context**:
Triggered when the user drags a new page to a target location in the hierarchy.

**Example**:
```php
// User drags a new page to append it under directory entry 456.
// The system creates the page and appends it to the target.
$ifc_param = "789,456"; // 789 is the new page, 456 is the target.
```

---

#### `case "save"`

**Purpose**:
Saves metadata changes for the selected content page.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New name for the page. |
| `$ifc_param2` | `bool` | Whether the page should be hidden. |
| `$ifc_param3` | `string` | New title for the page. |
| `$ifc_param4` | `string` | New description for the page. |
| `$ifc_param5` | `string` | New keywords for the page. |

**Return Values**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
1. Updates the directory entry's name and hidden status.
2. If the directory entry links to a content item (`content://`), updates the linked content's metadata in the database.

**Usage Context**:
Triggered when the user clicks the "Save" button after editing a page's metadata.

**Example**:
```php
// User edits the title, description, and keywords of a page linked to content item 123.
// The system updates both the directory entry and the linked content.
$ifc_param1 = "New Page Name";
$ifc_param3 = "New Title";
$ifc_param4 = "New description for the page.";
$ifc_param5 = "keyword1, keyword2";
```

---

#### `case "delete"`

**Purpose**:
Deletes a directory entry and its associated content.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `mixed` | The directory entry index to delete. |

**Return Values**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
1. Collects all content items linked to the directory branch being deleted.
2. Deletes the directory branch.
3. Deletes all linked content items from the database.
4. Removes any scheduled publish/withdraw actions for the deleted content.

**Usage Context**:
Triggered when the user drags a directory entry to the trash bin.

**Example**:
```php
// User drags a directory entry with index 789 to the trash bin.
// The system deletes the entry and all its linked content.
$ifc_param = 789;
```

---

#### `case "insert" / "append"`

**Purpose**:
Moves a directory entry to a new location (insert before or append after a target).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Comma-separated string: `"<source_index>,<target_index>"`. |

**Return Values**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
1. Validates that the target is not a child of the source (prevents circular references).
2. Uses the `directory` module to cut the source entry and insert/append it under the target.
3. Updates the `$object` context to the moved entry's new index.

**Usage Context**:
Triggered when the user drags a directory entry to a new location in the hierarchy.

**Example**:
```php
// User drags directory entry 123 to insert it before entry 456.
// The system moves the entry and updates the hierarchy.
$ifc_param = "123,456";
```

---

#### `case "template_preview"`

**Purpose**:
Displays a preview of the selected template.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `mixed` | The template identifier to preview. |

**Return Values**:
None (exits after rendering the preview).

**Inner Mechanisms**:
- Calls the `template_preview` function to render the template.

**Usage Context**:
Triggered when the user clicks the "Show" button in the template selection interface.

---

### Main Display

The main display renders the directory hierarchy and the metadata editor for the selected object.

#### Directory Hierarchy

**Purpose**:
Displays the directory structure as an interactive hierarchy.

**Inner Mechanisms**:
- Uses `flexview` to render the hierarchy with drag-and-drop support.
- Registers JavaScript event handlers for drag-and-drop operations (`directory_easypage_event`).
- Provides a trash bin for deleting entries.

**Usage Context**:
Always displayed to the left of the interface.

**Example**:
```javascript
// User drags a directory entry to the trash bin.
// The `directory_easypage_event` function triggers the "delete" message.
function directory_easypage_event(event, source, target) {
    if (target.id === "trashbin") {
        ifc_post("delete", source.getAttribute("data-fv-hir-index"));
    }
}
```

---

#### Metadata Editor

**Purpose**:
Displays and allows editing of metadata for the selected directory entry.

**Fields**:
| Field | Type | Description |
|-------|------|-------------|
| Name | `text` | The display name of the directory entry. |
| Hidden | `checkbox` | Whether the entry is hidden from navigation. |
| Page Title | `text` | The title of the linked content (if applicable). |
| Description | `texteditor` | The description of the linked content. |
| Keywords | `text` | The keywords for the linked content (if applicable). |

**Inner Mechanisms**:
- Retrieves metadata from the directory entry or linked content.
- Provides "Edit" and "Save" buttons for modifying the content.

**Usage Context**:
Displayed to the right of the directory hierarchy when an object is selected.

**Example**:
```php
// User selects a directory entry linked to content item 123.
// The editor displays the content's title, description, and keywords.
$url = $data->get($object, "url"); // "content://123"
$index = substr($url, 10); // "123"
```

---

### Helper Functions

#### `directory_flexview_display_function`

**Purpose**:
Custom display function for rendering directory entries in the flexview.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$key` | `mixed` | The directory entry index. |
| `$data` | `data` | The data object containing directory entries. |

**Return Values**:
- `string`: HTML representation of the directory entry.

**Inner Mechanisms**:
- Retrieves the entry's name, URL, and type.
- Applies custom formatting based on the entry's properties (e.g., hidden entries are grayed out).

**Usage Context**:
Used by `flexview` to render each directory entry in the hierarchy.

---

#### `directory_get_type`

**Purpose**:
Determines the icon type for a directory entry.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$key` | `mixed` | The directory entry index. |
| `$data` | `data` | The data object containing directory entries. |

**Return Values**:
- `string`: Icon identifier for the entry.

**Inner Mechanisms**:
- Checks the entry's URL to determine if it links to content, media, or another resource.
- Returns an appropriate icon identifier (e.g., `page`, `media`).

**Usage Context**:
Used by `flexview` to display icons next to directory entries.

---

### Usage Example

**Scenario**: Adding a new content page to the directory.

1. **User Action**: Clicks the "Add" button in the interface.
   - **Code Path**: `case "add"` initializes the form for the new page.

2. **User Input**: Enters the page name, selects a template, and chooses a target location.
   - **Code Path**: `case "add_target"` displays the directory hierarchy for target selection.

3. **User Action**: Drags the new page to the desired location and drops it.
   - **Code Path**: `case "add_insert"` or `case "add_append"` creates the page at the target location.

4. **Result**: The new page appears in the hierarchy, and the user can edit its metadata.

```php
// Example of the interface flow:
ifc_post("add", 123); // User clicks "Add" under directory entry 123.
ifc_post("add_target", "New Page"); // User names the page and selects a template.
ifc_post("add_append", "456,789"); // User appends the new page (456) under entry 789.
```


<!-- HASH:a5226c4b6753ef8edfe685f905bc8c45 -->
