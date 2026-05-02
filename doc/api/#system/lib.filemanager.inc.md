# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.filemanager.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## File Manager Utility Functions

This file provides core file management utilities for the NUOS platform, including directory traversal, file operations (copy, delete, compress), and hierarchical data structures for UI representation. These functions are designed to work with the platform's security model, path validation, and multibyte-safe string handling.

---

## Constants and Dependencies

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_ROOT_PATH` | System-defined | Root directory path for the NUOS installation. |
| `CMS_PATH` | System-defined | Current working directory path for the CMS. |
| `CMS_USER` | System-defined | Current authenticated user (e.g., `"admin"`). |
| `flexview` | Class | Internal class for hierarchical data storage and UI representation. |

---

## Functions

### `filemanager_flexview_compare`

**Purpose:**
Comparison function for sorting file and directory names in a user-friendly manner. Directories are prioritized over files, and items are sorted alphabetically by extension and name.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | First file/directory name to compare. |
| `$value2` | `string` | Second file/directory name to compare. |

**Return Values:**

| Type | Description |
|------|-------------|
| `int` | `-1` if `$value1` should come before `$value2`, `1` if after, `0` if equal. |

**Inner Mechanisms:**
- Checks if either value is a directory (ends with `/`).
- If one is a directory and the other is not, the directory is prioritized.
- Extracts file extensions using `strrchr()`.
- Compares extensions lexicographically.
- Falls back to case-insensitive UTF-8 string comparison (`utf8_strcasecmp`) if extensions match.

**Usage Context:**
Used as a callback for `uasort()` and `uksort()` in `filemanager_sort()` and flexview functions to ensure consistent, user-friendly sorting in file listings.

---

### `filemanager_sort`

**Purpose:**
Sorts an array of file/directory entries using `filemanager_flexview_compare`.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$array` | `array` | Reference to the array to be sorted (modified in-place). |

**Return Values:**
None (modifies input array by reference).

**Inner Mechanisms:**
- Uses `uasort()` to maintain key-value associations while sorting.
- Delegates comparison logic to `filemanager_flexview_compare`.

**Usage Context:**
Used to sort file listings before rendering in the UI.

---

### `filemanager_flexview`

**Purpose:**
Recursively scans a directory and builds a hierarchical `flexview` object representing files and subdirectories. Includes security checks to hide sensitive paths (e.g., `CMS_PATH`) from non-admin users.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$root` | `string` | `NULL` (falls back to `CMS_ROOT_PATH`) | Root directory to start scanning. |
| `$path` | `string` | `NULL` (falls back to `$root`) | Current path being viewed (used for UI state). |

**Return Values:**

| Type | Description |
|------|-------------|
| `flexview` | Hierarchical object containing file/directory metadata. |
| `FALSE` | If the root directory cannot be opened. |

**Inner Mechanisms:**
- Uses `opendir()`/`readdir()` to traverse directories.
- Skips `.` and `..` entries.
- Hides `CMS_PATH` from non-admin users.
- For directories, recursively scans subdirectories and marks them as `"#type" => "container"`.
- For files, extracts extensions and marks them as `"#type" => "file"` with `"#subtype"` set to the extension.
- Uses `filemanager_flexview_compare` to sort entries.
- Maintains parent-child relationships in the `flexview` object.

**Usage Context:**
Primary function for generating file manager UIs, enabling navigation and selection of files/directories.

---

### `filemanager_flexview_directory`

**Purpose:**
Similar to `filemanager_flexview`, but **only includes directories** (excludes files). Used for directory selection UIs.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$root` | `string` | `NULL` (falls back to `CMS_ROOT_PATH`) | Root directory to start scanning. |
| `$path` | `string` | `NULL` (falls back to `$root`) | Current path being viewed. |

**Return Values:**

| Type | Description |
|------|-------------|
| `flexview` | Hierarchical object containing directory metadata. |
| `FALSE` | If the root directory cannot be opened. |

**Inner Mechanisms:**
- Identical to `filemanager_flexview`, but skips non-directory entries.
- Still hides `CMS_PATH` from non-admin users.

**Usage Context:**
Used in UIs where only directory selection is required (e.g., setting upload paths).

---

### `filemanager_get_select`

**Purpose:**
Generates a flat associative array of directory paths for use in `<select>` dropdowns. Indents subdirectories with Unicode whitespace for visual hierarchy.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$root` | `string` | `NULL` (falls back to `CMS_ROOT_PATH`) | Root directory to start scanning. |

**Return Values:**

| Type | Description |
|------|-------------|
| `array` | Associative array: `["Display Name" => "path/to/directory/"]`. |

**Inner Mechanisms:**
- Uses `opendir()`/`readdir()` to traverse directories.
- Skips `.`, `..`, and non-directory entries.
- Hides `CMS_PATH` from non-admin users.
- Indents subdirectories with `\xE2\x80\x83` (Unicode "em space") based on depth.
- Handles duplicate display names by appending `(1)`, `(2)`, etc.

**Usage Context:**
Used to populate `<select>` elements in forms where directory selection is required.

---

### `filemanager_copy`

**Purpose:**
Recursively copies a file or directory (including all contents) to a new location. Preserves file permissions.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$source_path` | `string` | Path to the source file/directory. |
| `$target_path` | `string` | Path to the target location. |

**Return Values:**

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

**Inner Mechanisms:**
- Uses a static `$flag` to prevent infinite recursion when copying a directory into itself.
- Validates paths with `ice_check_path()`.
- For directories:
  - Creates the target directory with the same permissions as the source.
  - Uses a stack to process files/directories in reverse order (depth-first).
  - Recursively calls `filemanager_copy` for each entry.
- For files:
  - Uses `stream_copy_to_stream()` to copy contents.
  - Locks files during copy (`flock`).
  - Preserves permissions with `chmod()`.
  - Deletes the target file if the copy fails.

**Usage Context:**
Used for duplicating files/directories, e.g., during asset management or backups.

---

### `filemanager_delete`

**Purpose:**
Recursively deletes a file or directory (including all contents).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path to the file/directory to delete. |

**Return Values:**

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

**Inner Mechanisms:**
- For directories:
  - Uses `opendir()`/`readdir()` to traverse contents.
  - Recursively calls `filemanager_delete` for each entry.
  - Deletes the empty directory with `rmdir()`.
- For files:
  - Uses `unlink()` to delete the file.

**Usage Context:**
Used for permanent deletion of files/directories, e.g., during cleanup or user-initiated removal.

---

### `filemanager_gzcompress`

**Purpose:**
Compresses a file using gzip (level 9) and saves it to the target path. If no target is specified, appends `.gz` to the source filename.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$source` | `string` | | Path to the source file. |
| `$target` | `string` | `NULL` | Path to the compressed output file. |

**Return Values:**

| Type | Description |
|------|-------------|
| `string` | Path to the compressed file on success. |
| `FALSE` | On failure. |

**Inner Mechanisms:**
- Validates that the source is a file.
- Opens the source in binary read mode (`"rb"`).
- Opens the target in gzip write mode (`"wb9"`).
- Uses `flock` to lock files during operation.
- Reads the source in 64KB chunks and writes to the gzip stream.

**Usage Context:**
Used for compressing files (e.g., logs, backups) to save disk space.

---

### `filemanager_gzdecompress`

**Purpose:**
Decompresses a gzip-compressed file. If no target is specified, removes `.gz` from the source filename.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$source` | `string` | | Path to the compressed file (must end with `.gz`). |
| `$target` | `string` | `NULL` | Path to the decompressed output file. |

**Return Values:**

| Type | Description |
|------|-------------|
| `string` | Path to the decompressed file on success. |
| `FALSE` | On failure. |

**Inner Mechanisms:**
- Validates that the source is a `.gz` file.
- Opens the source in gzip read mode (`"rb"`).
- Opens the target in binary write mode (`"wb"`).
- Uses `flock` to lock files during operation.
- Reads the gzip stream in 64KB chunks and writes to the target.

**Usage Context:**
Used to restore compressed files (e.g., backups, archives) to their original state.


<!-- HASH:991c67fafd18f1c88a98b8e9c313774c -->
