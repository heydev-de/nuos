# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.filemanager.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## File Manager Interface (`ifc.filemanager.inc`)

This file implements the **File Manager** interface for the NUOS web platform, providing a comprehensive file and directory management system. It enables users to navigate, create, edit, upload, download, copy, move, delete, and manipulate files and directories within the CMS root path. The interface integrates with other NUOS modules (e.g., `flexview`, `plist`) and follows the platform's no-bloat, high-performance design principles.

---

## Overview

### Purpose
The File Manager interface allows users to:
- Browse the filesystem starting from `CMS_ROOT_PATH`.
- Perform CRUD operations on files and directories (Create, Read, Update, Delete).
- Manage file permissions, compress/decompress files, and transfer files from remote URLs.
- Edit files directly in the browser with syntax highlighting based on MIME type.
- Track recently edited files for quick access.
- Copy, cut, and paste files/directories to different locations.

### Key Features
- **State Management**: Persists the current directory (`$object`) in the user's cache.
- **Permission Handling**: Uses `ifc_permission()` to enforce access control.
- **Flexible UI**: Leverages `flexview` for dynamic directory listings and `ifc` for interactive forms.
- **MIME-Aware**: Detects file types and applies appropriate editors (e.g., HTML, PHP, CSS, JavaScript).
- **Shortcuts**: Provides quick access to common directories (e.g., `design/`, `data/`).
- **Recent Files**: Displays a list of recently edited files for convenience.

### Dependencies
- **Libraries**: Requires `filemanager`, `flexview`, and `plist` modules.
- **Permissions**: Users must have `CMS_L_ACCESS` permission to access the interface.

---

## Constants and Variables

### Global Variables
| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$object`  | `string` | Current file/directory path. Defaults to `CMS_ROOT_PATH` if not set.       |
| `$list`    | `array`  | List of selected files/directories. Defaults to `[$object]` if empty.      |
| `$target`  | `string` | Target directory for copy/cut operations. Defaults to `NULL`.              |
| `$status`  | `string` | Tracks the current operation state (`"copy"`, `"cut"`). Defaults to `NULL`.|

---

## Message Handling

The interface processes user actions via `CMS_IFC_MESSAGE`, which triggers specific operations. Each case in the `switch` statement corresponds to a file management command.

---

### `select`
**Purpose**: Navigates to a specified directory or resets to `CMS_ROOT_PATH`.

**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param`  | `string` | Target directory path. If empty, resets to `CMS_ROOT_PATH`.                |

**Return/Output**: Updates `$object` and `$list` to reflect the new directory.

**Usage**:
- Triggered when a user clicks on a directory in the file listing.
- Example: `ifc_post('select', '/path/to/directory')`.

---

### `save`
**Purpose**: Renames a file/directory and updates its permissions.

**Parameters**:
| Name           | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$ifc_param1`  | `string`  | New name/path for the file/directory.                                      |
| `$ifc_param2`  | `bool`    | Owner read permission.                                                     |
| `$ifc_param3`  | `bool`    | Owner write permission.                                                    |
| `$ifc_param4`  | `bool`    | Owner execute permission.                                                  |
| `$ifc_param5`  | `bool`    | Group read permission.                                                     |
| `$ifc_param6`  | `bool`    | Group write permission.                                                    |
| `$ifc_param7`  | `bool`    | Group execute permission.                                                  |
| `$ifc_param8`  | `bool`    | Others read permission.                                                    |
| `$ifc_param9`  | `bool`    | Others write permission.                                                   |
| `$ifc_param10` | `bool`    | Others execute permission.                                                 |

**Return/Output**:
- Updates `$object` and `$list` on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Constructs the new path by combining the parent directory of `$object` with the new basename.
2. Calculates the new permissions by bitwise OR-ing the provided flags (e.g., `0400` for owner read).
3. Uses `chmod()` and `rename()` to apply changes.

**Usage**:
- Triggered when a user saves changes to a file/directory name or permissions.

---

### `upload`
**Purpose**: Handles file uploads to the current directory.

**Parameters**:
| Name           | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$ifc_file1`   | `array`   | Array of temporary file paths (from `$_FILES`).                            |
| `$ifc_file1_name` | `array` | Array of original filenames.                                               |

**Return/Output**:
- Updates `$object` to the first uploaded file.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR` based on success.

**Inner Mechanisms**:
1. Combines temporary paths and original filenames into an associative array.
2. Uses `move_uploaded_file()` to place files in the target directory.
3. Handles multiple files and tracks errors.

**Usage**:
- Triggered when a user uploads files via the file input field.

---

### `mkdir`
**Purpose**: Displays a form to create a new directory.

**Parameters**: None (uses `$object` to determine the parent directory).

**Return/Output**: Renders an `ifc` form with a text input for the directory name.

**Usage**:
- Triggered when a user clicks the "Create Directory" button.

---

### `_mkdir`
**Purpose**: Processes the directory creation form submission.

**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param1` | `string` | Name of the new directory.                                                  |

**Return/Output**:
- Updates `$object` and `$list` to the new directory on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Constructs the full path by appending the new directory name to the parent directory.
2. Uses `mkpath()` (a custom function) to create the directory.

**Usage**:
- Triggered when the user submits the "Create Directory" form.

---

### `mkfile`
**Purpose**: Displays a form to create a new file.

**Parameters**: None (uses `$object` to determine the parent directory).

**Return/Output**: Renders an `ifc` form with a text input for the filename and a textarea for content.

**Usage**:
- Triggered when a user clicks the "Create File" button.

---

### `_mkfile`
**Purpose**: Processes the file creation form submission.

**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param1` | `string` | Name of the new file.                                                       |
| `$ifc_param2` | `string` | Content of the new file.                                                    |

**Return/Output**:
- Updates `$object` and `$list` to the new file on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Constructs the full path by appending the filename to the parent directory.
2. Uses `fopen()`, `flock()`, and `fwrite()` to create and write to the file.

**Usage**:
- Triggered when the user submits the "Create File" form.

---

### `_edit`
**Purpose**: Saves changes to an edited file.

**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param1` | `string` | New content for the file.                                                   |

**Return/Output**:
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR` and exits.

**Inner Mechanisms**:
1. Opens the file in write mode with an exclusive lock (`LOCK_EX`).
2. Writes the new content and closes the file.

**Usage**:
- Triggered via AJAX when saving an edited file.

---

### `edit`
**Purpose**: Displays an editor for the selected file.

**Parameters**: None (uses `$object` to determine the file to edit).

**Return/Output**: Renders an `ifc` form with a syntax-highlighted editor based on the file's MIME type.

**Inner Mechanisms**:
1. Determines the editor type (`code_html`, `code_php`, `code_style`, `code_script`, or `textarea`) based on the file's MIME type.
2. Loads the file content into the editor.
3. Adds the file to the "recently edited" list (`#system/filemanager.recent`).

**Usage**:
- Triggered when a user clicks the "Edit" button.

---

### `compress`
**Purpose**: Compresses selected files/directories into `.gz` archives.

**Parameters**: None (uses `$list` to determine targets).

**Return/Output**:
- Updates `$list` to the compressed files on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Iterates over `$list` and compresses each item using `filemanager_gzcompress()`.
2. Tracks errors and updates the response accordingly.

**Usage**:
- Triggered when a user selects files and clicks the "Compress" button.

---

### `decompress`
**Purpose**: Decompresses selected `.gz` archives.

**Parameters**: None (uses `$list` to determine targets).

**Return/Output**:
- Updates `$list` to the decompressed files on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Iterates over `$list` and decompresses each item using `filemanager_gzdecompress()`.
2. Tracks errors and updates the response accordingly.

**Usage**:
- Triggered when a user selects `.gz` files and clicks the "Decompress" button.

---

### `copy` / `cut`
**Purpose**: Prepares files/directories for copying or moving.

**Parameters**: None (uses `$object` to determine the source).

**Return/Output**:
- Sets `$target` to `$object` and `$status` to `"copy"` or `"cut"`.

**Usage**:
- Triggered when a user clicks the "Copy" or "Cut" button.

---

### `target`
**Purpose**: Sets the target directory for copy/cut operations.

**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param`  | `string` | Target directory path.                                                      |

**Return/Output**: Updates `$target` to the specified path.

**Usage**:
- Triggered when a user selects a target directory in the copy/cut interface.

---

### `copy_insert` / `cut_insert`
**Purpose**: Pastes copied/cut files/directories into the target directory.

**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param`  | `string` | Target directory path. Defaults to `CMS_ROOT_PATH` if empty.               |

**Return/Output**:
- Updates `$object` and `$list` to the new paths on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Handles name conflicts by appending underscores (e.g., `file_`, `file__`).
2. Uses `filemanager_copy()` for copying or `rename()` for moving.
3. Tracks errors and updates the response accordingly.

**Usage**:
- Triggered when a user clicks the "Paste" button in the target directory.

---

### `download`
**Purpose**: Downloads the selected file.

**Parameters**: None (uses `$object` to determine the file).

**Return/Output**: Calls `download()` (a custom function) and exits.

**Usage**:
- Triggered when a user clicks the "Download" button.

---

### `delete`
**Purpose**: Deletes selected files/directories.

**Parameters**: None (uses `$list` to determine targets).

**Return/Output**:
- Updates `$list` to `[$object]` on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Iterates over `$list` and deletes each item using `filemanager_delete()`.
2. Tracks errors and updates the response accordingly.

**Usage**:
- Triggered when a user selects files and clicks the "Delete" button.

---

### `transfer`
**Purpose**: Displays a form to transfer a file from a remote URL.

**Parameters**: None (uses `$object` to determine the target directory).

**Return/Output**: Renders an `ifc` form with inputs for the remote URL and filename.

**Usage**:
- Triggered when a user clicks the "Transfer" button.

---

### `_transfer`
**Purpose**: Processes the file transfer form submission.

**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param1` | `string` | Remote URL of the file to transfer.                                        |
| `$ifc_param2` | `string` | Filename for the transferred file. Defaults to the basename of the URL.    |

**Return/Output**:
- Updates `$object` and `$list` to the transferred file on success.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms**:
1. Uses `retrieve_file()` (a custom function) to download the file from the remote URL.
2. Saves the file to the target directory.

**Usage**:
- Triggered when the user submits the "Transfer" form.

---

## Main Display

### Overview
The main display renders the File Manager interface, including:
- A directory listing with checkboxes for selection.
- A menu of available actions (e.g., create, edit, copy, delete).
- Detailed information about the selected file/directory.
- Shortcuts to common directories and recently edited files.

### Key Components
1. **Directory Listing**:
   - Uses `filemanager_flexview()` to display files and directories.
   - Supports checkbox selection for batch operations.
   - Encodes paths using `qr()` for safe JavaScript transmission.

2. **Menu**:
   - Dynamically generated based on the selected file/directory and user permissions.
   - Includes actions like "Edit", "Copy", "Cut", "Delete", "Compress", etc.

3. **File Information**:
   - Displays metadata (e.g., size, MIME type, permissions, timestamps).
   - Allows modification of file permissions via checkboxes.

4. **File Upload**:
   - Provides a multi-file upload form with auto-submit via `ifc_autopost()`.

5. **Target Panel**:
   - Shown when `$status` is `"copy"` or `"cut"`.
   - Uses `filemanager_flexview_directory()` to display target directories.

---

### Helper Functions
| Function                     | Description                                                                 |
|------------------------------|-----------------------------------------------------------------------------|
| `filemanager_flexview()`     | Generates a `flexview` instance for directory listings.                     |
| `filemanager_flexview_directory()` | Generates a `flexview` instance for target directory selection.       |
| `get_mime_type()`            | Returns the MIME type of a file.                                            |
| `get_mime_list()`            | Returns an associative array of file extensions to icon paths.             |
| `file_extension()`           | Extracts the file extension from a path.                                    |
| `format_bytesize()`          | Formats a byte size into a human-readable string (e.g., "1.2 MB").          |
| `friendly_date()`            | Converts a timestamp into a user-friendly date string.                     |
| `filemanager_gzcompress()`   | Compresses a file/directory into a `.gz` archive.                          |
| `filemanager_gzdecompress()` | Decompresses a `.gz` archive.                                               |
| `filemanager_copy()`         | Copies a file/directory to a new location.                                  |
| `filemanager_delete()`       | Deletes a file/directory.                                                   |
| `retrieve_file()`            | Downloads a file from a remote URL.                                         |
| `download()`                 | Forces a file download in the browser.                                      |
| `mkpath()`                   | Creates a directory and its parent directories if they don't exist.        |

---

## Usage Examples

### Navigating to a Directory
```javascript
// JavaScript
ifc_post('select', '/path/to/directory');
```

### Uploading a File
1. User selects files via the multi-file input.
2. The form auto-submits via `ifc_autopost()`.
3. The `upload` message processes the files.

### Editing a File
1. User clicks the "Edit" button for a file.
2. The `edit` message displays the editor.
3. User saves changes, triggering the `_edit` message.

### Copying and Pasting Files
1. User selects files and clicks "Copy".
2. User navigates to the target directory and clicks "Paste".
3. The `copy_insert` message processes the operation.

### Transferring a File from a URL
1. User clicks the "Transfer" button.
2. User enters the remote URL and filename.
3. The `_transfer` message downloads and saves the file.


<!-- HASH:9e5c5949e0c62f5c292ec0ebe4344551 -->
