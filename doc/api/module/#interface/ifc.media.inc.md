# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.media.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Media Interface Module (`ifc.media.inc`)

This file implements the **Media Interface** for the NUOS platform, providing a comprehensive user interface for managing media assets (files, external URLs, and their metadata). It handles:

- **Media selection** (single/multi-file)
- **Uploading** (single/multi-file)
- **Editing** (metadata, categories, types)
- **Replacing** existing media files
- **Deleting** media assets
- **Category management** (renaming)
- **Media type configuration** (MIME types, file extensions, display templates)

The interface is **context-aware**, adapting its display based on:
- User permissions (`CMS_L_ACCESS`, `CMS_L_OPERATOR`)
- Current selection (object, category, language)
- Interface mode (`CMS_IFC_MESSAGE`)

---

### **Core Dependencies**
| Library | Purpose |
|---------|---------|
| `media` | Core media management (CRUD operations, metadata) |
| `media_type` | Media type definitions (MIME, extensions, display templates) |

---

### **Interface Flow**
The module operates via **message-driven cases** (`CMS_IFC_MESSAGE`), each handling a specific user action. The main display renders:
1. **Category selector** (left panel)
2. **Media list** (left panel, filtered by category)
3. **Preview iframe** (right panel, showing selected media)

---

## **Message Handlers**

### **`select`**
**Purpose**: Updates the selected media object for a specific language.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Current object identifier (may be language-specific). |
| `$ifc_param` | `string` | New object identifier to select. |
| `$language` | `string` | Target language code (e.g., `"en"`). |

**Mechanism**:
- Calls `language_set()` to associate the object with the current language.
- Updates the cached user-specific object (`media.{CMS_USER}.object`).

**Usage**:
Triggered when a user selects a media item from the list.

---

### **`select_language`**
**Purpose**: Switches the active language for media display/editing.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Language code (e.g., `"fr"`). |

**Mechanism**:
- Updates the `$language` variable and refreshes the preview iframe.

**Usage**:
Triggered via language selector buttons in the UI.

---

### **`display`**
**Purpose**: Renders a preview of the selected media in an iframe.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Media object identifier. |

**Return**:
- Exits if no object is selected.
- Outputs HTML with:
  - Media preview (via `media->parse()`)
  - Metadata (file size, MIME type)

**Mechanism**:
1. Caches the selected object for the current user.
2. Instantiates the `media` class and fetches metadata.
3. Generates a preview using `media->parse()` with fixed dimensions (600x400).
4. Displays file size (if available) and MIME type.

**Usage**:
Embedded in the right-panel iframe of the main interface.

---

### **`upload` / `_upload`**
**Purpose**: Handles single-file uploads.
**Parameters** (`_upload`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `string` | Temporary file path (PHP upload). |
| `$ifc_file1_name` | `string` | Original filename. |
| `$ifc_param1` | `string` | Media name. |
| `$ifc_param2` | `string` | Media type (e.g., `"image/jpeg"`). |
| `$ifc_param3` | `string` | Category. |
| `$ifc_param4` | `string` | Custom filename (optional). |

**Return** (`_upload`):
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
1. **`upload`**: Renders a form with fields for:
   - Name, file upload, type, category, and filename.
   - Auto-triggers the file input via JavaScript.
2. **`_upload`**: Processes the uploaded file via `media->add()` and updates the object cache.

**Usage**:
Triggered via the "Upload" button in the operator menu.

---

### **`upload_multi` / `_upload_multi`**
**Purpose**: Handles multi-file uploads.
**Parameters** (`_upload_multi`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `array` | Array of temporary file paths. |
| `$ifc_file1_name` | `array` | Array of original filenames. |
| `$ifc_param1` | `string` | Media type. |
| `$ifc_param2` | `string` | Category. |

**Return**:
- `CMS_MSG_DONE` if at least one file succeeds.
- `CMS_MSG_ERROR` if all files fail.

**Mechanism**:
1. **`upload_multi`**: Renders a form with:
   - Multi-file upload field.
   - Type and category selectors.
2. **`_upload_multi`**: Processes each file via `media->add()` and updates the object cache.

**Usage**:
Triggered via the "Multi-Upload" button in the operator menu.

---

### **`add` / `_add`**
**Purpose**: Adds a new media entry (external URL or file link).
**Parameters** (`_add`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Media name. |
| `$ifc_param2` | `string` | URL. |
| `$ifc_param3` | `string` | Media type. |
| `$ifc_param4` | `string` | Category. |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
1. **`add`**: Renders a form with fields for:
   - Name, URL, type, and category.
2. **`_add`**: Processes the input via `media->link()` and updates the object cache.

**Usage**:
Triggered via the "Add" button in the operator menu.

---

### **`edit` / `_edit`**
**Purpose**: Edits an existing media entry.
**Parameters** (`_edit`):
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Media object identifier. |
| `$ifc_param1` | `string` | New name. |
| `$ifc_param2` | `string` | New URL (if external). |
| `$ifc_param3` | `string` | New type. |
| `$ifc_param4` | `string` | New category. |
| `$ifc_param5` | `string` | New filename (if internal). |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
1. **`edit`**: Pre-fills the form with existing metadata.
   - For **internal files**, displays the filename (editable) and extension (read-only).
   - For **external URLs**, displays the URL field.
2. **`_edit`**: Updates the entry via `media->set()` and refreshes the object cache.

**Usage**:
Triggered via the "Edit" button in the operator menu.

---

### **`replace` / `_replace`**
**Purpose**: Replaces an internal media file with a new upload.
**Parameters** (`_replace`):
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Media object identifier. |
| `$ifc_file1` | `string` | New file path. |
| `$ifc_file1_name` | `string` | New filename. |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
1. **`replace`**: Renders a file upload form with auto-submit on selection.
2. **`_replace`**: Processes the upload via `media->replace()`.

**Usage**:
Triggered via the "Replace" button in the operator menu.

---

### **`delete`**
**Purpose**: Deletes selected media entries.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$_object` | `array` | Array of object identifiers to delete. |

**Return**:
- `CMS_MSG_DONE` if all deletions succeed.
- `CMS_MSG_ERROR` if any deletion fails.

**Mechanism**:
1. Deletes each object via `media->unlink()`.
2. Updates the selection to the next available media in the same category.
3. Clears invalid language-specific references.

**Usage**:
Triggered via the "Delete" button in the operator menu.

---

### **`category_rename` / `_category_rename`**
**Purpose**: Renames a media category.
**Parameters** (`_category_rename`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New category name. |

**Return**:
- `CMS_MSG_DONE` on success.

**Mechanism**:
1. **`category_rename`**: Renders a form with the current category name.
2. **`_category_rename`**: Updates all media entries in the category via `media->data->set()`.

**Usage**:
Triggered via the "Rename Category" button in the operator menu.

---

### **Media Type Management**
Handles cases: `type`, `type_select`, `type_add`, `type_set`, `type_delete`.

#### **`type`**
**Purpose**: Displays a list of media types with editing capabilities.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$type_object` | `string` | Currently selected type identifier. |

**Mechanism**:
1. Renders a two-panel layout:
   - **Left panel**: List of types (selectable via checkboxes).
   - **Right panel**: Edit form for the selected type (name, extensions, template code).
2. Provides buttons for:
   - Adding new types (`type_add`).
   - Deleting selected types (`type_delete`).
   - Saving changes (`type_set`).

**Usage**:
Triggered via the "Media Types" button in the operator menu.

#### **`type_add`**
**Purpose**: Adds a new media type.
**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
Calls `media_type->add()` with a default name.

#### **`type_set`**
**Purpose**: Updates a media type's properties.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$type_object` | `string` | Type identifier. |
| `$ifc_param1` | `string` | New name. |
| `$ifc_param2` | `string` | New file extensions (e.g., `"jpg,png"`). |
| `$ifc_param3` | `string` | New template code. |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
Calls `media_type->set()` to update the type.

#### **`type_delete`**
**Purpose**: Deletes selected media types.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of type identifiers to delete. |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
Calls `media_type->delete()` and deselects the deleted type if it was active.

---

## **Main Display**
**Purpose**: Renders the primary media management interface.
**Mechanism**:
1. **Category Handling**:
   - Loads the current category from cache or derives it from the selected object.
   - Falls back to the first available category if invalid.
2. **Menu Construction**:
   - **Insert**: Shown if `CMS_IFC_SELECT` is enabled (for embedding media in content).
   - **Operator Actions**: Upload, add, edit, delete, etc. (permission-dependent).
3. **Layout**:
   - **Left Panel**: Category selector, media list, language selector.
   - **Right Panel**: Preview iframe (via `display` message).

**JavaScript**:
- `media_select()`: Updates the preview iframe when a media item is selected.
- Auto-selects the first media item in a category if none is selected.

**Usage**:
Loaded as the primary interface for media management.


<!-- HASH:f84dbd23bed4df805af9a3db1f75d93d -->
