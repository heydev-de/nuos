# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.download.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Download Module (`lib.download.inc`)

The `lib.download.inc` file provides functionality for managing downloadable files within the NUOS platform. It includes utility functions for retrieving download metadata and a `download` class for handling file uploads, updates, replacements, and deletions. This module integrates with the platform's permission system and supports multilingual naming.

---

## Utility Functions

### `download_get_array()`

Retrieves all downloadable files organized by category in a nested associative array.

#### Parameters
None.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Nested associative array where top-level keys are categories and sub-arrays contain file names mapped to their unique identifiers. Empty categories are included. |

#### Inner Mechanisms
1. Initializes a `data` object targeting the `#system/download` dataset.
2. Iterates through all entries, extracting `category` and localized `name`.
3. Generates unique display names by appending incrementing numbers if duplicates exist.
4. Sorts categories and file names alphabetically before returning.

#### Usage Context
- Used in administrative interfaces to populate dropdowns or lists of available downloads.
- Enables frontend modules to display categorized file listings.

---

### `download_get_select()`

Retrieves a flat associative array of all unique download categories.

#### Parameters
None.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array where keys and values are category names, sorted alphabetically. The first entry is an empty string. |

#### Inner Mechanisms
1. Initializes a `data` object targeting the `#system/download` dataset.
2. Iterates through all entries, collecting unique `category` values.
3. Sorts categories alphabetically before returning.

#### Usage Context
- Used in forms to populate category selection dropdowns.
- Simplifies UI filtering by category.

---

## `download` Class

Manages the lifecycle of downloadable files, including upload, update, replacement, and deletion. Requires operator-level permissions.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the `data` class targeting the `#system/download` dataset. |
| `operator` | `bool` | Indicates whether the current user has operator permissions (`CMS_DOWNLOAD_PERMISSION_OPERATOR`). |

---

### Constructor: `__construct()`

Initializes the download manager.

#### Parameters
None.

#### Return Values
None.

#### Inner Mechanisms
1. Instantiates a `data` object for the `#system/download` dataset.
2. Checks operator permissions using `cms_permission()`.
3. Ensures the download storage directory (`#download`) exists using `mkpath()`.

#### Usage Context
- Called automatically when instantiating the `download` class.
- Ensures filesystem and dataset readiness.

---

### `add($uploaded_file, $uploaded_filename, $name = "", $description = "", $category = "")`

Uploads a new file and registers it in the download system.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path to the uploaded file. |
| `$uploaded_filename` | `string` | Original filename of the uploaded file. |
| `$name` | `string` | Multilingual key or default name for the file. Default: `""`. |
| `$description` | `string` | Description of the file. Default: `""`. |
| `$category` | `string` | Category under which the file is grouped. Default: `""`. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|bool` | Unique identifier (`download://...`) on success; `FALSE` on failure. |

#### Inner Mechanisms
1. Validates operator permissions and file existence.
2. Extracts the file extension from `$uploaded_filename`.
3. Falls back to the uploaded filename if no default name is provided via `language_get()`.
4. Generates a unique filename using `unique_id()` and the original extension.
5. Moves the file to the `#download` directory.
6. Registers metadata (name, description, category, filename) in the dataset.
7. Returns the generated index on successful save.

#### Usage Context
- Used in file upload forms to register new downloads.
- Supports multilingual naming via `language_set()` and `language_get()`.

---

### `set($index, $name, $description, $category)`

Updates metadata for an existing download.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Unique identifier of the download (e.g., `download://...`). |
| `$name` | `string` | Updated multilingual key or default name. |
| `$description` | `string` | Updated description. |
| `$category` | `string` | Updated category. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|bool` | The provided `$index` on success; `FALSE` on failure. |

#### Inner Mechanisms
1. Validates operator permissions and index existence.
2. Falls back to the existing default name if no new default is provided.
3. Updates the dataset with new metadata.
4. Returns the index on successful save.

#### Usage Context
- Used in administrative interfaces to edit download metadata.
- Preserves multilingual naming consistency.

---

### `replace($index, $uploaded_file, $uploaded_filename)`

Replaces the physical file associated with a download while preserving metadata.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Unique identifier of the download. |
| `$uploaded_file` | `string` | Temporary path to the new uploaded file. |
| `$uploaded_filename` | `string` | Original filename of the new uploaded file. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success; `FALSE` on failure. |

#### Inner Mechanisms
1. Validates operator permissions and file existence.
2. Extracts the new file extension and retrieves the old filename from the dataset.
3. If extensions match, directly replaces the file.
4. If extensions differ, deletes the old file, generates a new filename, and updates the dataset.
5. Returns `TRUE` only if the file is replaced and the dataset is saved.

#### Usage Context
- Used to update the file content without altering metadata.
- Ensures file integrity by validating extensions.

---

### `unlink($index)`

Deletes a download and its associated file.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Unique identifier of the download. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success; `FALSE` on failure. |

#### Inner Mechanisms
1. Validates operator permissions and index existence.
2. Retrieves the filename from the dataset (with fallback for legacy entries).
3. Deletes the physical file if it exists.
4. Removes the dataset entry and saves changes.

#### Usage Context
- Used in administrative interfaces to remove downloads.
- Ensures both file and metadata are cleaned up.


<!-- HASH:af7a9c58bbeb7d8c8346087e052255ef -->
