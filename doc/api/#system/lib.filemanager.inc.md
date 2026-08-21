# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.filemanager.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.filemanager.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Manager Functions

This file provides core file management utilities for the PWNC Web Platform. It includes functions for browsing, sorting, copying, moving, deleting, and archiving files and directories. The functions are designed to work recursively, handle edge cases, and maintain platform consistency.

---

## Functions

### `filemanager_flexview_compare`

**Purpose:**
Comparison function for sorting file and directory entries in a flexible view. Directories are prioritized over files, and entries are sorted by extension and name.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value1` | string | First file/directory path to compare |
| `$value2` | string | Second file/directory path to compare |

**Return Values:**
- `int`: Negative if `$value1` should come before `$value2`, positive if after, zero if equal.

**Inner Mechanisms:**
- Checks if either value is a directory (ends with `/`).
- If one is a directory and the other is not, the directory is prioritized.
- If both are files, compares their extensions using `strnatcasecmp` (case-insensitive natural order).
- If extensions are identical, compares full paths.

**Usage Context:**
Used as a callback for `uasort` in `filemanager_sort` and `filemanager_flexview`. Ensures consistent, user-friendly sorting in file listings.

**Example:**
```php
$files = [
    "image.png",
    "document.pdf",
    "folder/",
    "archive.zip"
];
uasort($files, "cms\\filemanager_flexview_compare");
// Result: ["folder/", "document.pdf", "image.png", "archive.zip"]
```

---

### `filemanager_sort`

**Purpose:**
Sorts an array of file/directory paths using `filemanager_flexview_compare`.

**Parameters:**

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `&$array` | array | Reference to array to be sorted |

**Return Values:**
- `void`: Modifies the input array in place.

**Inner Mechanisms:**
- Uses `uasort` to maintain key association while sorting.

**Usage Context:**
Used to normalize file listings before display or processing.

**Example:**
```php
$files = [
    "b.txt" => ["name" => "b.txt", "#type" => "file"],
    "a/" => ["name" => "a", "#type" => "container"]
];
filemanager_sort($files);
// $files is now ordered: ["a/", "b.txt"]
```

---

### `filemanager_flexview`

**Purpose:**
Generates a hierarchical, sorted representation of a directory tree using the `flexview` class.

**Parameters:**

| Name    | Type   | Default         | Description                                      |
|---------|--------|-----------------|--------------------------------------------------|
| `$root`  | string | `CMS_ROOT_PATH` | Root directory to scan                           |
| `$path`  | string | `$root`         | Current path to highlight (e.g., for navigation) |

**Return Values:**
- `flexview|FALSE`: `flexview` object containing the directory structure, or `FALSE` on failure.

**Inner Mechanisms:**
- Opens the root directory and recursively scans subdirectories.
- Skips `.` and `..`.
- Uses `file_extension()` to determine file types.
- Assigns metadata: `#type` (`container` or `file`), `#subtype` (extension).
- Maintains parent-child relationships in the `flexview` object.
- Sorts entries using `filemanager_flexview_compare`.

**Usage Context:**
Used in file browsers, asset pickers, and content management interfaces.

**Example:**
```php
$view = filemanager_flexview("/var/www/assets/", "/var/www/assets/images/");
if ($view) {
    // Render tree starting from "images/"
}
```

---

### `filemanager_flexview_directory`

**Purpose:**
Generates a hierarchical view of **directories only** (excludes files).

**Parameters:**

| Name    | Type   | Default         | Description                                      |
|---------|--------|-----------------|--------------------------------------------------|
| `$root`  | string | `CMS_ROOT_PATH` | Root directory to scan                           |
| `$path`  | string | `$root`         | Current path to highlight                        |

**Return Values:**
- `flexview|FALSE`: `flexview` object with directory structure, or `FALSE` on failure.

**Inner Mechanisms:**
- Similar to `filemanager_flexview`, but skips non-directory entries.
- Only includes containers (`#type => "container"`).

**Usage Context:**
Used when only directory navigation is required (e.g., selecting a target folder).

**Example:**
```php
$dirs = filemanager_flexview_directory("/var/www/uploads/");
```

---

### `filemanager_get_select`

**Purpose:**
Generates a flat associative array of directory paths suitable for HTML `<select>` elements.

**Parameters:**

| Name    | Type   | Default         | Description              |
|---------|--------|-----------------|--------------------------|
| `$root`  | string | `CMS_ROOT_PATH` | Root directory to scan   |

**Return Values:**
- `array|FALSE`: Associative array of display names (indented) => full paths, or `FALSE` on failure.

**Inner Mechanisms:**
- Recursively scans directories.
- Uses Unicode `\xE2\x80\x83` (em space) for indentation.
- Handles name collisions by appending `(1)`, `(2)`, etc.

**Usage Context:**
Used in forms where users select a directory (e.g., upload target).

**Example:**
```php
$options = filemanager_get_select("/var/www/media/");
/*
Result:
[
    "images" => "/var/www/media/images/",
    "  photos" => "/var/www/media/images/photos/",
    "documents" => "/var/www/media/documents/"
]
*/
```

---

### `filemanager_collect_recursive`

**Purpose:**
Recursively collects all files and directories under given source paths, relative to a base.

**Parameters:**

| Name      | Type          | Default         | Description                                      |
|-----------|---------------|-----------------|--------------------------------------------------|
| `$source`  | string|array | —               | Source path(s) to collect                        |
| `$base`    | string        | `CMS_ROOT_PATH` | Base path; only items under this are included    |

**Return Values:**
- `array`: Associative array of relative paths => `TRUE` (dir) or `FALSE` (file), sorted by path length.

**Inner Mechanisms:**
- Uses `RecursiveIteratorIterator` for efficient traversal.
- Skips paths outside the base.
- Sorts by path length to ensure parent directories appear before their contents.

**Usage Context:**
Used by `filemanager_copy`, `filemanager_move`, `filemanager_delete`, and `filemanager_zip` to gather file lists.

**Example:**
```php
$list = filemanager_collect_recursive("/var/www/uploads/image.jpg", "/var/www/");
// Result: ["uploads/image.jpg" => FALSE]
```

---

### `filemanager_copy`

**Purpose:**
Recursively copies files and directories from source to target, handling name collisions.

**Parameters:**

| Name      | Type          | Default         | Description                                      |
|-----------|---------------|-----------------|--------------------------------------------------|
| `$source`  | string|array | —               | Source path(s) to copy                           |
| `$target`  | string        | —               | Target directory                                 |
| `$base`    | string        | `CMS_ROOT_PATH` | Base path for relative resolution                |

**Return Values:**
- `array|FALSE`: Array of copied paths, or `FALSE` on failure.

**Inner Mechanisms:**
- Uses `filemanager_collect_recursive` to gather source items.
- Generates unique filenames using `unique_filename()`.
- Preserves file permissions (`chmod`).
- Uses file locking (`flock`) during copy.
- Handles directories and files separately.

**Usage Context:**
Used for duplicating assets, backups, or staging changes.

**Example:**
```php
$copied = filemanager_copy("/var/www/uploads/image.jpg", "/var/www/backup/");
```

---

### `filemanager_move`

**Purpose:**
Moves files and directories to a new location, handling name collisions.

**Parameters:**

| Name      | Type          | Default         | Description                                      |
|-----------|---------------|-----------------|--------------------------------------------------|
| `$source`  | string|array | —               | Source path(s) to move                           |
| `$target`  | string        | —               | Target directory                                 |
| `$base`    | string        | `CMS_ROOT_PATH` | Base path for relative resolution                |

**Return Values:**
- `array|FALSE`: Array of moved paths, or `FALSE` on failure.

**Inner Mechanisms:**
- Uses `rename()` for atomic moves.
- Generates unique target paths using `unique_filename()`.
- Skips nested paths (e.g., moving a directory also moves its contents).

**Usage Context:**
Used for reorganizing assets or relocating content.

**Example:**
```php
$moved = filemanager_move("/var/www/old/", "/var/www/new/");
```

---

### `filemanager_delete`

**Purpose:**
Recursively deletes files and directories.

**Parameters:**

| Name    | Type          | Description                     |
|---------|---------------|---------------------------------|
| `$list`  | string|array | Path(s) to delete              |

**Return Values:**
- `array|TRUE|FALSE`: Array of deleted paths, `TRUE` if nothing to delete, `FALSE` on failure.

**Inner Mechanisms:**
- Uses `filemanager_collect_recursive` to gather items.
- Processes in reverse order (deepest first) to avoid directory deletion issues.

**Usage Context:**
Used for cleanup, asset removal, or temporary file management.

**Example:**
```php
$deleted = filemanager_delete("/var/www/temp/cache/");
```

---

### `filemanager_zip`

**Purpose:**
Creates a ZIP archive from source files and directories.

**Parameters:**

| Name      | Type   | Default         | Description                                      |
|-----------|--------|-----------------|--------------------------------------------------|
| `$source`  | string | —               | Source path(s) to archive                        |
| `$target`  | string | —               | Target ZIP file path                             |
| `$base`    | string | `CMS_ROOT_PATH` | Base path for relative paths in archive          |

**Return Values:**
- `string|FALSE`: Path to created ZIP file, or `FALSE` on failure.

**Inner Mechanisms:**
- Uses `ZipArchive`.
- Creates a temporary file during creation.
- Handles directories and files separately.
- Uses `unique_filename()` to avoid collisions.

**Usage Context:**
Used for backups, exports, or bundling assets.

**Example:**
```php
$zip = filemanager_zip("/var/www/uploads/", "/var/www/backup/uploads.zip");
```

---

### `filemanager_unzip`

**Purpose:**
Extracts a ZIP archive to a target directory.

**Parameters:**

| Name      | Type   | Default                     | Description                                      |
|-----------|--------|-----------------------------|--------------------------------------------------|
| `$source`  | string | —                           | ZIP file to extract                              |
| `$target`  | string | Auto-generated subdirectory | Target directory (defaults to ZIP name + `/`)    |

**Return Values:**
- `string|FALSE`: Path to extraction directory, or `FALSE` on failure.

**Inner Mechanisms:**
- Uses `ZipArchive`.
- Validates paths to prevent directory traversal attacks.
- Uses streams for error-tolerant extraction.
- Creates target directory using `mkpath()`.

**Usage Context:**
Used for imports, restores, or asset deployment.

**Example:**
```php
$dir = filemanager_unzip("/var/www/downloads/archive.zip");
```


<!-- HASH:05f9b33e927467b497fc845ae6d61c4d -->
