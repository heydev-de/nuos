# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.filemanager.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.filemanager.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Filemanager Functions

The `lib.filemanager.inc` file provides a comprehensive set of file management utilities for the PWNC Web Platform. These functions handle directory traversal, file operations (copy, move, delete), and archive creation/extraction (zip/unzip). They work with the platform's root path (`CMS_ROOT_PATH`) and utilize custom utility functions for path handling, uniqueness, and error management.

### filemanager_flexview_compare

Compares two file/directory names for sorting purposes, prioritizing directories over files and grouping by file extension.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value1` | string | First file/directory name to compare |
| `$value2` | string | Second file/directory name to compare |

**Return Value:**
- `-1` if `$value1` should come before `$value2`
- `1` if `$value1` should come after `$value2`
- `0` if they are considered equal

**Inner Mechanisms:**
1. Checks if each value represents a directory (ends with `/`)
2. Directories are sorted before files
3. If both are files or both are directories, compares by file extension using natural case-insensitive comparison
4. Falls back to natural case-insensitive comparison of the full names

**Usage Example:**
```php
// Sort an array of file/directory names
$files = ["document.txt", "images/", "config.php", "assets/"];
usort($files, 'cms\filemanager_flexview_compare');
// Result: ["assets/", "images/", "config.php", "document.txt"]
```

### filemanager_sort

Sorts an array of file/directory names using the flexview comparison algorithm.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$array` | array | Array of file/directory names to sort (passed by reference) |

**Return Value:** None (modifies the array in place)

**Inner Mechanisms:**
Uses `uasort` with `filemanager_flexview_compare` to sort the array while maintaining key associations.

**Usage Example:**
```php
$files = ["document.txt", "images/", "config.php", "assets/"];
cms\filemanager_sort($files);
// $files is now sorted with directories first, then by extension
```

### filemanager_flexview

Creates a hierarchical view of files and directories starting from a root path, optionally focusing on a specific path.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$root` | string | `CMS_ROOT_PATH` | Root directory to start scanning from |
| `$path` | string | `CMS_ROOT_PATH` | Specific path to focus on within the root |

**Return Value:**
- `flexview` object containing the hierarchical structure
- `FALSE` on failure (e.g., unable to open directory)

**Inner Mechanisms:**
1. Opens the root directory and iterates through its contents
2. For directories, recursively opens and processes them
3. For files, records their extension as subtype
4. Builds a tree structure using the `flexview` class
5. Sorts directory contents using the flexview comparison algorithm
6. Handles path navigation and parent-child relationships

**Usage Example:**
```php
$view = cms\filemanager_flexview(CMS_ROOT_PATH, CMS_ROOT_PATH . "assets/images/");
if ($view !== FALSE) {
    // Process the hierarchical view
    foreach ($view->object as $path => $info) {
        echo $info['name'] . " (" . $info['#type'] . ")\n";
    }
}
```

### filemanager_flexview_directory

Creates a hierarchical view containing only directories, starting from a root path.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$root` | string | `CMS_ROOT_PATH` | Root directory to start scanning from |
| `$path` | string | `CMS_ROOT_PATH` | Specific path to focus on within the root |

**Return Value:**
- `flexview` object containing only directory entries
- `FALSE` on failure

**Inner Mechanisms:**
Similar to `filemanager_flexview` but filters out files, only processing directories. This is useful for directory selection interfaces.

**Usage Example:**
```php
$dirView = cms\filemanager_flexview_directory(CMS_ROOT_PATH, CMS_ROOT_PATH . "assets/");
if ($dirView !== FALSE) {
    // Display directory tree for selection
    foreach ($dirView->object as $path => $info) {
        echo $info['name'] . "\n";
    }
}
```

### filemanager_get_select

Generates a flat array of all directories within a root path, formatted for use in select dropdowns.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$root` | string | `CMS_ROOT_PATH` | Root directory to scan |

**Return Value:**
- Array with directory names as keys (indented with visual hierarchy) and paths as values
- `FALSE` on failure

**Inner Mechanisms:**
1. Traverses all directories recursively
2. Creates visual indentation using Unicode thin spaces (U+2003)
3. Handles duplicate directory names by appending "(1)", "(2)", etc.
4. Returns relative paths from `CMS_ROOT_PATH`

**Usage Example:**
```php
$directories = cms\filemanager_get_select(CMS_ROOT_PATH . "assets/");
if ($directories !== FALSE) {
    foreach ($directories as $name => $path) {
        echo "<option value=\"$path\">$name</option>\n";
    }
}
```

### filemanager_collect_recursive

Recursively collects all files and directories within specified source paths, returning relative paths.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$source` | string/array | Required | File/directory path(s) to collect |
| `$base` | string | `CMS_ROOT_PATH` | Base path to calculate relative paths from |

**Return Value:**
- Array with relative paths as keys and boolean values (`TRUE` for directories, `FALSE` for files)
- Empty array if no valid paths found

**Inner Mechanisms:**
1. Normalizes input to an array
2. Validates that paths are within the base directory
3. Uses PHP's `RecursiveDirectoryIterator` for efficient traversal
4. Orders results by path length to ensure directories come before their contents
5. Returns relative paths from the base

**Usage Example:**
```php
$files = cms\filemanager_collect_recursive(
    CMS_ROOT_PATH . "assets/images/",
    CMS_ROOT_PATH
);
// Returns: ["assets/images/" => TRUE, "assets/images/logo.png" => FALSE, ...]
```

### filemanager_copy

Copies files and directories from source to target, handling name conflicts and preserving permissions.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$source` | string/array | Required | Source file/directory path(s) |
| `$target` | string | Required | Target directory path |
| `$base` | string | `CMS_ROOT_PATH` | Base path for relative path calculation |

**Return Value:**
- Array of successfully copied file/directory paths
- `FALSE` if nothing was copied

**Inner Mechanisms:**
1. Collects all source paths recursively
2. Handles name conflicts using `unique_filename`
3. Preserves file permissions using `fileperms`
4. Uses stream copying with file locking for safe file operations
5. Tracks replacements to handle nested conflicts
6. Creates directories with original permissions

**Usage Example:**
```php
$copied = cms\filemanager_copy(
    CMS_ROOT_PATH . "assets/images/",
    CMS_ROOT_PATH . "backup/images/",
    CMS_ROOT_PATH
);
if ($copied !== FALSE) {
    echo "Copied " . count($copied) . " items\n";
}
```

### filemanager_move

Moves files and directories from source to target, handling name conflicts.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$source` | string/array | Required | Source file/directory path(s) |
| `$target` | string | Required | Target directory path |
| `$base` | string | `CMS_ROOT_PATH` | Base path for relative path calculation |

**Return Value:**
- Array of successfully moved file/directory paths
- `FALSE` if nothing was moved

**Inner Mechanisms:**
1. Normalizes source to an array
2. Sorts paths to ensure directories are processed before their contents
3. Skips paths that are within previously processed paths (avoiding duplicates)
4. Uses `unique_filename` to handle name conflicts
5. Uses `rename` for atomic move operations

**Usage Example:**
```php
$moved = cms\filemanager_move(
    [CMS_ROOT_PATH . "temp/file1.txt", CMS_ROOT_PATH . "temp/file2.txt"],
    CMS_ROOT_PATH . "documents/",
    CMS_ROOT_PATH
);
if ($moved !== FALSE) {
    echo "Moved " . count($moved) . " files\n";
}
```

### filemanager_delete

Deletes files and directories recursively, processing in reverse order to handle dependencies.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | string/array | File/directory path(s) to delete |

**Return Value:**
- Array of successfully deleted file/directory paths
- `TRUE` if nothing to delete
- `FALSE` if nothing was deleted

**Inner Mechanisms:**
1. Collects all paths recursively using `filemanager_collect_recursive`
2. Reverses the order to delete contents before parent directories
3. Uses `unlink` for files and `rmdir` for directories
4. Returns absolute paths of deleted items

**Usage Example:**
```php
$deleted = cms\filemanager_delete(CMS_ROOT_PATH . "temp/old_files/");
if ($deleted !== FALSE && $deleted !== TRUE) {
    echo "Deleted " . count($deleted) . " items\n";
}
```

### filemanager_zip

Creates a ZIP archive from specified source files/directories.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$source` | string/array | Required | Source file/directory path(s) |
| `$target` | string | Required | Target ZIP file path |
| `$base` | string | `CMS_ROOT_PATH` | Base path for relative path calculation |

**Return Value:**
- Path to the created ZIP file on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Collects all source paths recursively
2. Creates a temporary file to avoid partial archives
3. Uses `ZipArchive` to create the archive
4. Adds directories as empty directories and files with their content
5. Renames the temporary file to the final target using `unique_filename`
6. Cleans up temporary files on failure

**Usage Example:**
```php
$zipPath = cms\filemanager_zip(
    CMS_ROOT_PATH . "assets/images/",
    CMS_ROOT_PATH . "backups/images.zip",
    CMS_ROOT_PATH
);
if ($zipPath !== FALSE) {
    echo "Archive created at: $zipPath\n";
}
```

### filemanager_unzip

Extracts a ZIP archive to a target directory, with security checks for path traversal.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$source` | string | Required | Path to the ZIP file |
| `$target` | string | Auto-generated | Target directory for extraction |

**Return Value:**
- Path to the extraction directory on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Generates a unique target directory if not specified
2. Creates the target directory using `mkpath`
3. Opens the ZIP archive using `ZipArchive`
4. Validates each entry against security regex patterns:
   - Windows drive letters (e.g., `C:`)
   - Network paths (e.g., `//`)
   - Protocol wrappers (e.g., `http://`)
   - Path traversal attempts (`../`)
5. Extracts files using streams for error tolerance
6. Creates necessary directories before extracting files

**Usage Example:**
```php
$extractPath = cms\filemanager_unzip(CMS_ROOT_PATH . "uploads/archive.zip");
if ($extractPath !== FALSE) {
    echo "Extracted to: $extractPath\n";
} else {
    echo "Extraction failed or contained invalid paths\n";
}
```


<!-- HASH:3cc965f7b35b4d839f68c63afb12b621 -->
