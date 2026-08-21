# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.download.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.download.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Download Management Module (`lib.download.inc`)

This module provides functionality for managing file downloads in the PWNC Web Platform. It includes:
- A `download` class for handling file uploads, updates, replacements, and deletions
- Utility functions for generating download-related UI elements (arrays for selection lists)
- Permission-based access control for operator-level operations

The module stores downloadable files in `#system/download` and maintains metadata in a structured data store.

---

## Utility Functions

### `download_get_array()`

Generates a nested associative array of all available downloads, organized by category.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Nested associative array with categories as top-level keys and download names as second-level keys. Values are download identifiers. |

#### Inner Mechanisms
1. Loads download data from `#system/download`
2. Iterates through all entries
3. Organizes entries by category
4. Handles naming collisions by appending `(1)`, `(2)`, etc.
5. Sorts recursively using natural case-insensitive order

#### Usage Example
```php
$downloads = download_get_array();
/*
Returns:
[
    "Documents" => [
        "User Manual" => "download://abc123",
        "Technical Specs" => "download://def456"
    ],
    "Software" => [
        "Installer" => "download://ghi789"
    ]
]
*/
```

---

### `download_get_select()`

Generates a flat associative array of all download categories for use in `<select>` elements.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array with categories as keys and values (empty string as first option). |

#### Inner Mechanisms
1. Loads download data from `#system/download`
2. Extracts unique categories
3. Sorts categories naturally (case-insensitive)

#### Usage Example
```php
$categories = download_get_select();
/*
Returns:
[
    "" => "",
    "Documents" => "Documents",
    "Software" => "Software"
]
*/
```

---

## `download` Class

Manages the lifecycle of downloadable files with operator permission control.

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `data` | `NULL` | Instance of `data` class for managing download metadata |
| `operator` | `FALSE` | Boolean indicating if current user has operator permissions |

### Constructor

```php
function __construct()
```

#### Inner Mechanisms
1. Initializes data store connection to `#system/download`
2. Checks operator permissions via `cms_permission()`
3. Ensures download directory exists via `mkpath()`

---

### `add()`

Uploads a new downloadable file and creates metadata.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path to uploaded file |
| `$uploaded_filename` | `string` | Original filename from upload |
| `$name` | `string` | (Optional) Display name (supports language placeholders) |
| `$description` | `string` | (Optional) Description text |
| `$category` | `string` | (Optional) Category name |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Download identifier on success |
| `FALSE` | On failure |

#### Inner Mechanisms
1. Validates operator permissions and file existence
2. Generates unique filename with original extension
3. Moves file to permanent location
4. Creates metadata record with provided information
5. Generates unique download identifier

#### Usage Example
```php
$download = new download();
$index = $download->add(
    $_FILES['file']['tmp_name'],
    $_FILES['file']['name'],
    "User Manual",
    "Complete product documentation",
    "Documents"
);
if ($index) {
    echo "Uploaded as: " . translate_url($index);
}
```

---

### `set()`

Updates metadata for an existing download.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Download identifier |
| `$name` | `string` | New display name (supports language placeholders) |
| `$description` | `string` | New description text |
| `$category` | `string` | New category name |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Download identifier on success |
| `FALSE` | On failure |

#### Inner Mechanisms
1. Validates operator permissions and index
2. Preserves existing default name if new name is empty
3. Updates all metadata fields
4. Persists changes to data store

#### Usage Example
```php
$download = new download();
$result = $download->set(
    "download://abc123",
    "Updated User Manual",
    "Revised documentation with new features",
    "Updated Documents"
);
if ($result) {
    echo "Download updated successfully";
}
```

---

### `replace()`

Replaces the physical file for an existing download.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Download identifier |
| `$uploaded_file` | `string` | Temporary path to new uploaded file |
| `$uploaded_filename` | `string` | Original filename of new upload |

#### Return Values
| Type | Description |
|------|-------------|
| `TRUE` | On success |
| `FALSE` | On failure |

#### Inner Mechanisms
1. Validates operator permissions and file existence
2. Checks if new file has same extension as original
3. If extensions match: replaces file directly
4. If extensions differ:
   - Generates new filename
   - Moves new file
   - Updates metadata
   - Deletes old file
5. Handles cleanup on failure

#### Usage Example
```php
$download = new download();
$result = $download->replace(
    "download://abc123",
    $_FILES['new_file']['tmp_name'],
    $_FILES['new_file']['name']
);
if ($result) {
    echo "File replaced successfully";
}
```

---

### `unlink()`

Deletes a download and its associated file.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Download identifier |

#### Return Values
| Type | Description |
|------|-------------|
| `TRUE` | On success |
| `FALSE` | On failure |

#### Inner Mechanisms
1. Validates operator permissions and index
2. Retrieves filename from metadata
3. Deletes physical file
4. Removes metadata record
5. Persists changes to data store

#### Usage Example
```php
$download = new download();
$result = $download->unlink("download://abc123");
if ($result) {
    echo "Download removed successfully";
}
```


<!-- HASH:805b1d39f6bdd190a7e75cc3798e2d7e -->
