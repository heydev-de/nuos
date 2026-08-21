# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.filemanager.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.filemanager.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Manager Interface (`ifc.filemanager.inc`)

This file implements the **File Manager** interface for the PWNC Web Platform, providing a comprehensive user interface for browsing, managing, and manipulating files and directories within the CMS. It integrates with the **FlexView** and **PList** modules to deliver a dynamic, interactive experience with features such as file selection, upload, download, editing, compression, transfer, and permission management.

The interface is **context-aware**, responding to user actions (e.g., `CMS_IFC_MESSAGE`) and maintaining state across sessions using the platform’s caching system. It supports **multibyte-safe** file operations and follows the platform’s **zero-dependency**, **no-bloat** philosophy.

---

### Core Concepts

| Concept | Description |
|--------|-------------|
| **`$object`** | The currently selected file or directory path. Persisted across sessions via permanent cache. |
| **`$list`** | Array of selected file/directory paths (for batch operations like delete, copy, compress). |
| **`$status`** | Tracks whether the user is in "copy" or "cut" (move) mode. |
| **`$target`** | Target directory for copy/move operations. |
| **`CMS_IFC_MESSAGE`** | Determines the current action (e.g., `select`, `save`, `upload`, `edit`, `delete`). |
| **`ifc_param`, `ifc_param1`, etc.** | Parameters passed via interface commands. |
| **`ifc_response`** | Response message sent back to the client (e.g., `CMS_MSG_DONE`, `CMS_MSG_ERROR`). |

---

### Initialization & State Management

#### Overview
The file begins by loading required libraries (`filemanager`, `flexview`, `plist`) and verifying user permissions. It initializes the selected object from cache, sanitizes it, and sets up the initial selection list.

#### Key Operations
- **Cache Initialization**: `cms_cache_init()` retrieves the last selected object for the current user.
- **Sanitization**: Ensures `$object` exists and is within `CMS_ROOT_PATH`.
- **Selection List**: Initialized based on interface message or defaults to the current object.

---

### Message Handling (Sub-Display Logic)

The `switch (CMS_IFC_MESSAGE)` block processes user-initiated actions. Each case corresponds to a file operation.

---

#### ### `select`
**Purpose**: Selects a directory or file as the current object.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param` | `string` | Path to select. If empty, defaults to `CMS_ROOT_PATH`. |

**Return/Output**: None (updates `$object` and `$list`).

**Mechanism**:
- Resets `$object` to the provided path or root.
- Updates `$list` to contain only the selected object.
- Clears any copy/move status.

**Usage Example**:
```javascript
// User clicks a folder in the UI
ifc_post('select', '/data/images/');
```
> Selects `/data/images/` as the current directory.

---

#### ### `save`
**Purpose**: Renames the current object and updates its permissions.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param1` | `string` | New name (basename) for the object. |
| `$ifc_param2`–`$ifc_param10` | `bool` (via `isset`) | Permission flags (read/write/execute for owner/group/all). |

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Constructs new path by combining parent directory and new basename.
- If object is a directory, appends `/`.
- Calculates Unix-style permissions from checkbox states (e.g., `isset($ifc_param2)` → owner read).
- Calls `chmod()` and `rename()`.
- Updates `$object` and `$list` on success.

**Usage Example**:
```javascript
// Rename "old.txt" to "new.txt" and set 644 permissions
ifc_post('save', 'new.txt', 1, 0, 0, 1, 0, 0, 1, 0, 0);
```
> Renames file and sets permissions to `-rw-r--r--`.

---

#### ### `upload`
**Purpose**: Handles file uploads to the current directory.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_file1`, `$ifc_file1_name` | `string` or `array` | Uploaded file(s) and their original names. |

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` if all uploads succeed, `CMS_MSG_ERROR` if any fail.
- `$list`: Updated with paths of successfully uploaded files.

**Mechanism**:
- Handles single or multiple file uploads.
- Places files in the current directory (`$object` or its parent).
- Uses `move_uploaded_file()` to move files from temp location.
- Updates `$object` to the first uploaded file.

**Usage Example**:
```html
<!-- HTML form with file input -->
<input type="file" name="ifc_file1[]" multiple>
```
> Uploads all selected files to the current directory.

---

#### ### `mkdir`
**Purpose**: Displays a form to create a new directory.

**Mechanism**:
- Uses the `ifc` class to render a modal form.
- Form submits to `_mkdir` with the new directory name.

**Usage Example**:
```javascript
ifc_post('mkdir');
```
> Opens a dialog to enter a new directory name.

---

#### ### `_mkdir`
**Purpose**: Creates a new directory.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param1` | `string` | Name of the new directory. |

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Appends `/` to the name.
- Uses `mkpath()` (a recursive directory creator) to create the directory.
- Updates `$object` and `$list` to the new directory.

**Usage Example**:
```javascript
ifc_post('_mkdir', 'new_folder');
```
> Creates `/current/path/new_folder/`.

---

#### ### `mkfile`
**Purpose**: Displays a form to create a new file.

**Mechanism**:
- Uses `ifc` to render a form with name and content fields.
- Form submits to `_mkfile`.

---

#### ### `_mkfile`
**Purpose**: Creates a new file with specified content.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param1` | `string` | Name of the new file. |
| `$ifc_param2` | `string` | Initial content of the file. |

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Checks if file already exists.
- Uses `write_file()` to create the file.
- Updates `$object` and `$list` to the new file.

**Usage Example**:
```javascript
ifc_post('_mkfile', 'script.js', 'console.log("Hello");');
```
> Creates `/current/path/script.js` with the given content.

---

#### ### `edit`
**Purpose**: Displays an editor for the selected file.

**Mechanism**:
- Validates that `$object` is a file.
- Uses `plist` to track recently edited files.
- Renders an `ifc` form with a code editor or textarea based on MIME type.
- Supports HTML, PHP, CSS, JavaScript, and plain text.

**Usage Example**:
```javascript
ifc_post('edit', '/data/config.json');
```
> Opens `/data/config.json` in a JSON-aware editor.

---

#### ### `_edit`
**Purpose**: Saves edited file content (called via AJAX).

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param1` | `string` | New content of the file. |

**Return/Output**:
- Outputs `CMS_MSG_DONE` or `CMS_MSG_ERROR` directly to the client.

**Mechanism**:
- Uses `write_file()` to save content.
- Exits immediately after response.

---

#### ### `compress`
**Purpose**: Displays a form to compress selected files into a ZIP archive.

**Mechanism**:
- Requires at least one file in `$list`.
- Uses `ifc` to render a form with a name field (defaults to current directory name).

---

#### ### `_compress`
**Purpose**: Creates a ZIP archive from selected files.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param1` | `string` | Name of the ZIP file (without extension). |

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Appends `.zip` to the name.
- Uses `filemanager_zip()` to create the archive.
- Updates `$object` and `$list` to the new ZIP file.

**Usage Example**:
```javascript
ifc_post('_compress', 'backup');
```
> Creates `/current/path/backup.zip` from selected files.

---

#### ### `decompress`
**Purpose**: Extracts a ZIP archive.

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Uses `filemanager_unzip()` to extract the archive.
- Updates `$object` to the extraction directory.

**Usage Example**:
```javascript
ifc_post('decompress', '/data/archive.zip');
```
> Extracts `archive.zip` into `/data/`.

---

#### ### `copy` / `cut`
**Purpose**: Initiates copy or move (cut) mode.

**Mechanism**:
- Sets `$status` to `"copy"` or `"cut"`.
- Sets `$target` to the current directory.

**Usage Example**:
```javascript
ifc_post('copy');
```
> Enables copy mode; user can then select a target directory.

---

#### ### `target`
**Purpose**: Sets the target directory for copy/move operations.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param` | `string` | Path to the target directory. |

**Mechanism**:
- Updates `$target` to the provided path.

---

#### ### `copy_insert` / `cut_insert`
**Purpose**: Executes copy or move operation.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param` | `string` | Target directory. |

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` if all files are copied/moved, `CMS_MSG_ERROR` if some fail.

**Mechanism**:
- Uses `filemanager_copy()` or `filemanager_move()`.
- Filters results to show only files directly in the target (not subdirectories).
- Updates `$object` to the target directory.
- Clears `$status`.

**Usage Example**:
```javascript
ifc_post('copy_insert', '/data/backup/');
```
> Copies all selected files to `/data/backup/`.

---

#### ### `download`
**Purpose**: Downloads the selected file.

**Mechanism**:
- Calls `download($object)`.
- Exits immediately to send file to client.

---

#### ### `delete`
**Purpose**: Deletes selected files/directories.

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` if all are deleted, `CMS_MSG_ERROR` if some remain.

**Mechanism**:
- Uses `filemanager_delete()` to recursively delete.
- Updates `$list` to remove successfully deleted items.
- Clears `$status`.

**Usage Example**:
```javascript
ifc_post('delete');
```
> Deletes all files currently selected in `$list`.

---

#### ### `transfer`
**Purpose**: Displays a form to download a file from a URL.

**Mechanism**:
- Uses `ifc` to render a form with URL and name fields.

---

#### ### `_transfer`
**Purpose**: Downloads a file from a URL and saves it locally.

| Parameter | Type | Description |
|---------|------|-------------|
| `$ifc_param1` | `string` | URL of the file to download. |
| `$ifc_param2` | `string` | Local filename (optional; defaults to basename of URL). |

**Return/Output**:
- `$ifc_response`: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Uses `retrieve_file()` to download the file.
- Saves to current directory or parent.
- Updates `$object` and `$list`.

**Usage Example**:
```javascript
ifc_post('_transfer', 'https://example.com/image.jpg', 'logo.jpg');
```
> Downloads `image.jpg` and saves as `/current/path/logo.jpg`.

---

### Main Display Logic

#### Overview
Renders the main file manager interface, including:
- Directory tree and breadcrumb navigation.
- File list with sorting and selection controls.
- Contextual menus and popovers (shortcuts, recently edited, recently selected).
- Object details (size, type, permissions, disk space).
- File upload form.

---

#### File List Generation

**Mechanism**:
- Uses `opendir()` and `readdir()` to scan the current directory.
- Skips `.` and `..`.
- Collects metadata: path, name, type, size, modification time.
- Sorts using `usort()` based on `$order` (`name`, `type`, `size`, `time`) and `$desc` (descending flag).

**Sorting Logic**:
- Directories always appear before files.
- Secondary sort by name if primary sort is equal.

---

#### UI Components

| Component | Description |
|--------|-------------|
| **Folder Selection** | Popover showing directory tree via `filemanager_flexview_directory()`. |
| **Shortcuts** | Quick links to common directories (e.g., `design/`, `data/`). |
| **Recently Edited** | Shows last 15 edited files (from `#system/filemanager.edited` PList). |
| **Recently Selected** | Shows last 15 selected files (from `#system/filemanager.recent` PList). |
| **File Table** | Displays files with columns: select, name, type, size, time. |
| **Selection Controls** | Buttons to select all, invert, or clear selection. |
| **Copy/Move Target** | Shown only in copy/cut mode; uses FlexView for target selection. |
| **Object Details** | Tabs for general info, permissions, and upload form. |

---

#### JavaScript Helpers

| Function | Description |
|--------|-------------|
| `s(index)` | Selects a file from the list by index. |
| `o(value)` | Sets sort order; toggles descending if same column is clicked. |

```javascript
function o(value) {
    ifc_set("desc",  ((value === ifc_get("order")) && (ifc_get("desc") === "")) ? 1 : "");
    ifc_set("order", value);
    ifc_post();
}
```
> Toggles sort direction if the same column is clicked again.

---

### Key Utility Functions Used

| Function | Description |
|--------|-------------|
| `cms_cache()` | Manages in-memory and permanent cache. |
| `cms_cache_init()` | Initializes a variable from cache. |
| `cms_cache_sync()` | Syncs a variable with cache. |
| `ifc()` | Renders interface forms and controls. |
| `ifc_popover_open()` / `close()` | Creates collapsible UI sections. |
| `ifc_table_open()` / `close()` | Renders styled tables. |
| `ifc_varied()` | Applies alternating row styling. |
| `filemanager_flexview_directory()` | Renders interactive directory trees. |
| `filemanager_zip()` / `filemanager_unzip()` | Compression utilities. |
| `filemanager_copy()` / `filemanager_move()` / `filemanager_delete()` | Batch file operations. |
| `get_mime_type()` | Returns MIME type based on file extension. |
| `get_mime_list()` | Returns array of known MIME types and icons. |
| `format_bytesize()` | Formats file size in human-readable units. |
| `friendly_date()` | Converts timestamp to user-friendly format. |
| `file_name()` / `file_extension()` | Extracts name and extension from path. |
| `x()`, `q()`, `qx()`, `qr()` | Context-aware escaping functions. |
| `jscript()` | Outputs JavaScript safely. |
| `image()` | Renders an icon. |

---

### Example: Full Usage Scenario

**Scenario**: User wants to upload an image, edit its HTML wrapper, and set permissions.

```javascript
// 1. Navigate to images directory
ifc_post('select', '/design/images/');

// 2. Upload image
// (via file input in form)

// 3. Edit HTML wrapper
ifc_post('edit', '/design/images/wrapper.html');

// 4. Save changes (via AJAX)
ifc_post('_edit', '<div><img src="photo.jpg"></div>');

// 5. Set permissions to 644
ifc_post('save', 'wrapper.html', 1, 0, 0, 1, 0, 0, 1, 0, 0);
```

> Demonstrates navigation, upload, editing, and permission management.

---

### Security & Validation

- **Path Sanitization**: All paths are validated to be within `CMS_ROOT_PATH`.
- **Permission Checks**: `ifc_permission()` ensures user has `CMS_L_ACCESS`.
- **CSRF Protection**: Built into `ifc_post()` and `cms_url()`.
- **Escaping**: All dynamic output is escaped using `x()`, `q()`, `qx()`, etc.
- **File Operations**: Use platform-safe wrappers (`write_file`, `read_file`, `mkpath`).

---

### Integration with Other Modules

| Module | Role |
|-------|------|
| **FlexView** | Renders interactive directory trees. |
| **PList** | Manages recently edited and selected files. |
| **FileManager** | Provides core file operations (`zip`, `unzip`, `copy`, `move`, `delete`). |
| **System** | Used for editing `.dat` files and checking permissions. |

---

### Notes

- The interface is **fully stateful**, persisting selection, sort order, and copy/move state across requests.
- **No external dependencies**: All file operations use PHP built-ins or platform wrappers.
- **Multibyte-safe**: Uses `utf8_*` functions for string handling.
- **Responsive**: Uses CSS classes like `layout`, `nomargin`, `highlight` for consistent styling.
- **Extensible**: New actions can be added via `CMS_IFC_MESSAGE` cases.


<!-- HASH:293b9c744cbd97d4403d7726af827073 -->
