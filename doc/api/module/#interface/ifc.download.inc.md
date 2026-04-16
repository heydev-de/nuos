# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.download.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Download Interface Module (`ifc.download.inc`)

This file implements the **Download Management Interface** for the NUOS platform. It provides a complete UI for browsing, uploading, editing, replacing, and deleting downloadable files, along with category management and multi-language support. The interface integrates with the core `download` module and leverages the NUOS IFC (Interface Controller) system for consistent UI rendering and state management.

---

### **Core Responsibilities**
- **File Management**: Upload, edit metadata, replace, and delete downloadable files.
- **Category Management**: Organize files into categories and rename categories.
- **Multi-Language Support**: Handle language-specific metadata (name, description) and object references.
- **User Interface**: Render a split-view interface with a category/file selector on the left and a preview/details iframe on the right.
- **Permission Handling**: Enforce access control via `CMS_L_ACCESS` and operator permissions (`CMS_L_OPERATOR`).
- **State Persistence**: Maintain user-specific category and object selections across sessions using `cms_cache`.

---

### **Constants & Variables**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DOWNLOAD_PERMISSION_OPERATOR` | (Platform constant) | Permission level required for upload/edit/delete operations. |
| `$object` | `NULL` or `download://<id>` | Current selected download object (URI format). Restored from cache if empty. |
| `$language` | `NULL` or language code | Current language context for multi-language metadata. |
| `$category` | `""` or category name | Current selected category. Persisted in user cache. |
| `$_object` | `NULL` or object ID | Language-resolved object ID (without `download://` prefix). |

---

### **Message Handling (IFC Dispatch)**

The interface responds to the following `CMS_IFC_MESSAGE` values:

| Message | Purpose | Typical Trigger |
|---------|---------|-----------------|
| `select` | Update the selected object (language-aware). | User selects a file from the list. |
| `select_language` | Change the active language context. | User clicks a language flag. |
| `display` | Show file details (name, size, description, download button). | Object selection or page load. |
| `download` | Trigger file download. | User clicks the download button. |
| `upload` | Show single-file upload form. | Operator clicks "Upload" in menu. |
| `_upload` | Process single-file upload. | Form submission from `upload`. |
| `upload_multi` | Show multi-file upload form. | Operator clicks "Upload Multiple". |
| `_upload_multi` | Process multi-file upload. | Form submission from `upload_multi`. |
| `edit` | Show edit form for file metadata. | Operator clicks "Edit". |
| `_edit` | Save edited metadata. | Form submission from `edit`. |
| `replace` | Show file replacement form. | Operator clicks "Replace". |
| `_replace` | Process file replacement. | Form submission from `replace`. |
| `delete` | Delete selected files. | Operator clicks "Delete". |
| `category_rename` | Show category rename form. | Operator clicks "Rename Category". |
| `_category_rename` | Process category rename. | Form submission from `category_rename`. |

---

### **Key Functions & Methods**

---

#### ### `ifc_download($url)`
**Purpose**: Trigger a file download via JavaScript.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL to the download endpoint (generated via `cms_url`). |

**Return**: None (triggers browser download).
**Mechanism**: Uses `javascript:ifc_download()` to navigate to the download URL.
**Usage**: Called when the user clicks the download button in the details view.

---

#### ### `download_get_select()`
**Purpose**: Generate a `<select>` element for category selection.
**Parameters**: None.
**Return**: `string` – HTML `<select>` element with all available categories.
**Mechanism**:
- Retrieves all categories from `download_get_array()`.
- Generates `<option>` elements for each category.
- Used in upload/edit forms.

**Usage**: Called during form rendering for category selection.

---

#### ### `download_get_array()`
**Purpose**: Retrieve a structured array of all downloadable files grouped by category.
**Parameters**: None.
**Return**: `array` – Associative array: `["Category" => ["ObjectID" => "Name", ...], ...]`.
**Mechanism**:
- Iterates over all download objects in the `download` module.
- Groups objects by their `category` field.
- Returns a sorted, category-indexed array.

**Usage**: Used to populate the category selector and file list.

---

### **Class: `ifc` (Interface Controller)**
While not defined in this file, the `ifc` class is heavily used for UI rendering. Key interactions:

| Method | Purpose | Usage in This File |
|--------|---------|--------------------|
| `new ifc($response, $page, $menu, $params, $action, $title)` | Initialize a new interface panel. | Used for all forms and the main display. |
| `$ifc->set($label, $type, $value)` | Add a form field to the interface. | Used for name, description, file upload, etc. |
| `$ifc->close()` | Finalize and render the interface. | Called after all fields are added. |

---

### **Core Logic Flows**

---

#### **1. Object Selection & Language Handling**
- **Selection**: When a user selects a file, `download_select()` updates the `object` parameter and reloads the details iframe.
- **Language Resolution**: `language_get()` and `language_set()` ensure the correct language variant of an object is used.
- **Fallback**: If an object is not available in the selected language, the default language is used.

---

#### **2. File Upload & Replacement**
- **Single Upload**: `upload` → `_upload`:
  - User selects a file and provides metadata (name, description, category).
  - The `download->add()` method handles file storage and database entry.
- **Multi Upload**: `upload_multi` → `_upload_multi`:
  - User selects multiple files (no metadata per file).
  - All files are assigned the same category.
- **Replacement**: `replace` → `_replace`:
  - User selects a new file to replace the existing one.
  - The `download->replace()` method updates the file while preserving metadata.

---

#### **3. Deletion & Cleanup**
- **Deletion**: `delete`:
  - Deletes all selected objects across all languages.
  - If the deleted object was the last in its category, the selection falls back to the first object in the category.
  - Uses `download->unlink()` to remove files and database entries.

---

#### **4. Category Management**
- **Rename**: `category_rename` → `_category_rename`:
  - Renames a category and updates all objects in that category.
  - Uses `download->data->set()` to update the `category` field for all matching objects.

---

### **UI Components**

---

#### **Main Display**
- **Left Panel**:
  - **Category Selector**: Dropdown to filter files by category.
  - **File List**: Checkbox list of files in the selected category (double-click to select).
  - **Language Selector**: Flags to switch between enabled languages.
- **Right Panel**:
  - **Details Iframe**: Displays file metadata (name, size, description) and a download button.

---

#### **Forms**
- **Upload/Edit**: Text fields for name and description, file upload field, and category selector.
- **Replace**: File upload field with auto-submit on selection.
- **Category Rename**: Text field for the new category name.

---

### **Security & Validation**
- **Permissions**: All write operations (`upload`, `edit`, `delete`, etc.) require `CMS_DOWNLOAD_PERMISSION_OPERATOR`.
- **CSRF Protection**: All form submissions use `ifc_post()` which includes CSRF tokens.
- **Input Sanitization**: All user-provided values are escaped using `x()`, `q()`, or `sqlesc()` before output or database operations.
- **File Validation**: The `download` module handles file type and size validation.

---

### **Caching**
- **User-Specific Cache**:
  - `download.<USER>.object`: Persists the last selected object.
  - `download.<USER>.category`: Persists the last selected category.
- **Cache Keys**: Cleared or updated during object selection and deletion.

---

### **Usage Scenarios**

---

#### **1. Browsing Downloads**
- **User Action**: Navigate to the download interface.
- **System Behavior**:
  - Restores the last selected category and object from cache.
  - Displays the file list and details iframe.
  - User can switch categories, select files, or change languages.

---

#### **2. Uploading a File**
- **User Action**: Click "Upload", fill in metadata, and select a file.
- **System Behavior**:
  - `upload` message renders the form.
  - `_upload` processes the submission, stores the file, and creates a database entry.
  - The new file is selected and displayed.

---

#### **3. Editing Metadata**
- **User Action**: Select a file and click "Edit".
- **System Behavior**:
  - `edit` message renders the form with current metadata.
  - `_edit` saves the updated metadata.

---

#### **4. Deleting Files**
- **User Action**: Select one or more files and click "Delete".
- **System Behavior**:
  - `delete` message removes all selected files and their language variants.
  - The selection falls back to the first file in the category (or clears if the category is empty).

---

#### **5. Renaming a Category**
- **User Action**: Select a file in the category and click "Rename Category".
- **System Behavior**:
  - `category_rename` renders a form with the current category name.
  - `_category_rename` updates all files in the category with the new name.

---

### **Integration Points**
- **`download` Module**: All file operations (add, set, replace, unlink) are delegated to this module.
- **`language` Module**: Handles multi-language object resolution and metadata.
- **`ifc` System**: Provides UI rendering, form handling, and state management.
- **`cms_cache`**: Persists user-specific state (object, category).
- **`cms_url`**: Generates URLs for navigation and downloads.

---

### **Error Handling**
- **Form Submission**: If a submission fails (e.g., file upload error), the response is set to `CMS_MSG_ERROR`.
- **Deletion**: If any file fails to delete, the entire operation is marked as an error (`$flag_error`).
- **Fallbacks**: If an object is not found, the system falls back to the first object in the category or clears the selection.

---

### **JavaScript Integration**
- **`download_select(value)`**: Updates the selected object and reloads the details iframe.
- **`ifc_download(url)`**: Triggers a file download.
- **`ifc_autopost(field, action)`**: Auto-submits a form when a file is selected (used in replace).
- **`ifc_list_*`**: Utilities for managing checkbox lists (select all, invert, etc.).


<!-- HASH:9eee721dc318dcc818fa6a6fe343c128 -->
