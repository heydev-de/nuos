# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.download.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.download.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/lib.download.inc

## Overview

This file implements the **Download Management System** for the PWNC Web Platform. It provides functionality for uploading, managing, replacing, and deleting downloadable files within the CMS. The system stores file metadata in a structured data store (`#system/download`) and physical files in the `#download` directory under the CMS data path.

Key features include:
- File upload with automatic unique naming
- Multi-language support for file names and descriptions
- Category-based organization
- Operator permission checks
- File replacement with extension handling
- Complete cleanup on deletion

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DOWNLOAD_PERMISSION_OPERATOR` | `"operator"` | Permission level required for download management operations |

## Functions

### download_get_array

Retrieves all downloads organized by category as an associative array.

**Parameters:** None

**Return Values:**
- `array` - Associative array where keys are categories and values are arrays mapping display names to download indices

**Inner Mechanisms:**
1. Creates a `data` instance for the `#system/download` store
2. Iterates through all records using `move("next")`
3. For each record, extracts category and name
4. Handles duplicate names by appending incrementing numbers in parentheses
5. Sorts the resulting array recursively using natural, case-insensitive ordering

**Usage Example:**
```php
$downloads = download_get_array();
// Returns something like:
// [
//   "" => [],
//   "Documents" => ["Manual.pdf" => "download://abc123", "Guide.pdf" => "download://def456"],
//   "Images" => ["Logo.png" => "download://ghi789"]
// ]
```

### download_get_select

Retrieves all download categories for use in select dropdowns.

**Parameters:** None

**Return Values:**
- `array` - Associative array where both keys and values are category names, with an empty string entry for "no category"

**Inner Mechanisms:**
1. Creates a `data` instance for the `#system/download` store
2. Iterates through all records
3. Collects unique category names
4. Sorts categories using natural, case-insensitive ordering

**Usage Example:**
```php
$categories = download_get_select();
// Returns something like:
// ["", "Documents", "Images", "Software"]
```

## Class: download

Main class for managing downloadable files in the CMS.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$data` | `data` | `NULL` | Data store instance for `#system/download` |
| `$operator` | `boolean` | `FALSE` | Whether the current user has operator permissions |

### __construct

Initializes the download manager.

**Parameters:** None

**Return Values:** None

**Inner Mechanisms:**
1. Creates a `data` instance for the `#system/download` store
2. Checks if the current user has operator permissions
3. Ensures the download directory exists by calling `mkpath()`

**Usage Example:**
```php
$dl = new download();
if ($dl->operator) {
    // User can manage downloads
}
```

### add

Uploads and registers a new downloadable file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path of the uploaded file |
| `$uploaded_filename` | `string` | Original filename of the uploaded file |
| `$name` | `string` | Multi-language name for the download (default: `""`) |
| `$description` | `string` | Multi-language description (default: `""`) |
| `$category` | `string` | Category for organizing downloads (default: `""`) |

**Return Values:**
- `string` - The download index (`download://...`) on success
- `FALSE` - On failure (no permission, invalid file, or save error)

**Inner Mechanisms:**
1. Checks operator permissions
2. Verifies the uploaded file exists
3. Extracts the file extension from the uploaded filename
4. Handles multi-language naming with fallback to the uploaded filename
5. Generates a unique filename using `unique_id()`
6. Moves the uploaded file to the download directory
7. Creates a new dataset with name, description, category, and filename
8. Saves the data store and returns the index

**Usage Example:**
```php
$dl = new download();
$result = $dl->add(
    $_FILES['file']['tmp_name'],
    $_FILES['file']['name'],
    ['en' => 'User Manual', 'de' => 'Benutzerhandbuch'],
    ['en' => 'Complete guide', 'de' => 'Komplettanleitung'],
    'Documentation'
);
if ($result !== FALSE) {
    echo "Download added with index: " . $result;
}
```

### set

Updates metadata for an existing download.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The download index to update |
| `$name` | `string` | New multi-language name |
| `$description` | `string` | New multi-language description |
| `$category` | `string` | New category |

**Return Values:**
- `string` - The download index on success
- `FALSE` - On failure (no permission or invalid index)

**Inner Mechanisms:**
1. Checks operator permissions
2. Validates the index is not empty
3. Handles multi-language naming with fallback to existing default name
4. Updates name, description, and category in the dataset
5. Saves the data store and returns the index

**Usage Example:**
```php
$dl = new download();
$result = $dl->set(
    'download://abc123',
    ['en' => 'Updated Manual', 'de' => 'Aktualisierte Anleitung'],
    ['en' => 'Updated description', 'de' => 'Aktualisierte Beschreibung'],
    'Updated Category'
);
```

### replace

Replaces the physical file of an existing download.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The download index to replace the file for |
| `$uploaded_file` | `string` | Temporary path of the new uploaded file |
| `$uploaded_filename` | `string` | Original filename of the new uploaded file |

**Return Values:**
- `boolean` - `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permissions
2. Verifies the uploaded file exists
3. Retrieves the current filename from the dataset (with compatibility fallback)
4. Compares old and new file extensions:
   - If they match, simply moves the new file over the old one
   - If they differ, generates a new filename with the new extension
5. Moves the uploaded file to the download directory
6. Updates the dataset with the new filename
7. Saves the data store
8. Deletes the old file if it exists
9. Cleans up the new file if the save fails

**Usage Example:**
```php
$dl = new download();
$success = $dl->replace(
    'download://abc123',
    $_FILES['new_file']['tmp_name'],
    $_FILES['new_file']['name']
);
if ($success) {
    echo "File replaced successfully";
}
```

### unlink

Deletes a download and its associated file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | The download index to delete |

**Return Values:**
- `boolean` - `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permissions
2. Validates the index is not empty
3. Retrieves the filename from the dataset (with compatibility fallback)
4. Deletes the physical file from the download directory if it exists
5. Removes the dataset entry
6. Saves the data store

**Usage Example:**
```php
$dl = new download();
$success = $dl->unlink('download://abc123');
if ($success) {
    echo "Download deleted successfully";
}
```

## Technical Notes

- **Multi-language Support:** The `name` and `description` parameters use language arrays (e.g., `['en' => 'value', 'de' => 'wert']`) processed by `language_get()` and `language_set()` functions.
- **File Naming:** Physical files are stored with unique IDs as filenames to prevent collisions, preserving the original extension.
- **Security:** All operations require operator permissions, and indices are validated to prevent empty values.
- **Compatibility:** The `replace()` and `unlink()` methods include fallback logic using `remove_prefix()` for older data formats.


<!-- HASH:805b1d39f6bdd190a7e75cc3798e2d7e -->
