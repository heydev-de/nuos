# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.media_type.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Media Type Management Module

This file provides the `media_type` class and related utility functions for managing and rendering different media types in the NUOS platform. It handles:

- **Media Type Definitions**: Storing and retrieving custom HTML templates for different file types (e.g., video, audio, PDF).
- **Type Association**: Mapping file extensions to specific media type templates.
- **Dynamic Rendering**: Generating HTML markup for media files with support for placeholders (e.g., `src`, `width`, `alt`).
- **Permission Control**: Restricting write operations to users with the `operator` permission.

---

### Constants

| Name                          | Value/Default       | Description                                                                 |
|-------------------------------|---------------------|-----------------------------------------------------------------------------|
| `CMS_MEDIA_TYPE_PERMISSION_OPERATOR` | `"operator"`        | Permission key required to modify media types.                             |

---

### Utility Function: `media_type_get_select()`

#### Purpose
Generates an associative array of media type names keyed by their database IDs, suitable for use in HTML `<select>` elements.

#### Parameters
None.

#### Return Values
- **Type**: `array`
- **Description**: Associative array where keys are media type names (or a space `" "` if unnamed) and values are their corresponding database IDs. The array is sorted alphabetically by name.

#### Inner Mechanisms
1. Initializes a `data` object targeting the `#system/media.type` table.
2. Adds a default entry (`CMS_L_MEDIA_TYPE_001`) with a `NULL` value.
3. Iterates through all media type entries, populating the array with their names and IDs.
4. Sorts the array by keys (names) before returning.

#### Usage Context
- Used in administrative interfaces to populate dropdown menus for media type selection.

---

## Class: `media_type`

Manages media type definitions, including storage, retrieval, and rendering of custom HTML templates for different file types.

---

### Properties

| Name       | Value/Default       | Description                                                                 |
|------------|---------------------|-----------------------------------------------------------------------------|
| `data`     | `data` object       | Instance of the `data` class for interacting with the `#system/media.type` table. |
| `operator` | `bool`              | `TRUE` if the current user has the `operator` permission; otherwise `FALSE`. |
| `type`     | `array`             | Associative array mapping file extensions to media type IDs.                |

---

### Constructor: `__construct()`

#### Purpose
Initializes the `media_type` object, loads media type data, and ensures the required filesystem directory exists.

#### Parameters
None.

#### Return Values
None.

#### Inner Mechanisms
1. Initializes the `data` property to interact with the `#system/media.type` table.
2. Checks the user's `operator` permission.
3. Creates the `#media.type` directory in `CMS_DATA_PATH` if it does not exist.
4. Attempts to load the media type mapping from cache (`media_type.type`). If not found, calls `update_type()` to generate it.

#### Usage Context
- Automatically invoked when creating a new `media_type` object.

---

### Method: `add()`

#### Purpose
Creates a new media type entry with a unique ID, name, type associations, and HTML template code.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `name` | `string` | Display name of the media type.                                             |
| `type` | `string` | Comma-separated list of file extensions associated with this media type.    |
| `code` | `string` | HTML template code for rendering the media type.                            |

#### Return Values
- **Type**: `string|bool`
- **Description**: The unique ID of the newly created media type on success; `FALSE` on failure (e.g., lack of permissions).

#### Inner Mechanisms
1. Checks for `operator` permission. Returns `FALSE` if not granted.
2. Generates a unique ID for the new media type.
3. Delegates to the `set()` method to store the data.

#### Usage Context
- Used in administrative interfaces to add new media types.

---

### Method: `set()`

#### Purpose
Updates or creates a media type entry with the specified ID, name, type associations, and HTML template code.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `index` | `string` | Unique ID of the media type.                                                |
| `name`  | `string` | Display name of the media type.                                             |
| `type`  | `string` | Comma-separated list of file extensions associated with this media type.    |
| `code`  | `string` | HTML template code for rendering the media type.                            |

#### Return Values
- **Type**: `bool`
- **Description**: `TRUE` on success; `FALSE` on failure (e.g., lack of permissions or file write errors).

#### Inner Mechanisms
1. Checks for `operator` permission. Returns `FALSE` if not granted.
2. Writes the HTML template code to a file named `#media.type/{index}.htm` in `CMS_DATA_PATH`.
3. Updates the media type entry in the database with the provided `name` and `type`.
4. Calls `save()` to persist changes and update the type mapping.

#### Usage Context
- Used to create or update media type definitions.

---

### Method: `get_index()`

#### Purpose
Retrieves the media type ID associated with a given file extension.

#### Parameters

| Name   | Type     | Description                                                                 |
|--------|----------|-----------------------------------------------------------------------------|
| `type` | `string` | File extension (e.g., `"mp4"`, `"pdf"`).                                    |

#### Return Values
- **Type**: `string|bool`
- **Description**: The media type ID if found; `FALSE` otherwise.

#### Inner Mechanisms
1. Converts the input `type` to lowercase using `utf8_strtolower()`.
2. Checks if the `type` exists in the `type` property array.
3. Returns the associated media type ID or `FALSE`.

#### Usage Context
- Used to determine which HTML template to use for a given file extension.

---

### Method: `get_code()`

#### Purpose
Retrieves the HTML template code for a given media type ID.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `index` | `string` | Unique ID of the media type.                                                |

#### Return Values
- **Type**: `string|bool`
- **Description**: The HTML template code if found; `FALSE` otherwise.

#### Inner Mechanisms
1. Reads the content of the file `#media.type/{index}.htm` in `CMS_DATA_PATH`.
2. Returns the content or `FALSE` if the file does not exist or cannot be read.

#### Usage Context
- Used to fetch the HTML template for rendering a specific media type.

---

### Method: `delete()`

#### Purpose
Deletes one or more media type entries and their associated template files.

#### Parameters

| Name    | Type            | Description                                                                 |
|---------|-----------------|-----------------------------------------------------------------------------|
| `index` | `string|array` | Unique ID(s) of the media type(s) to delete.                                |

#### Return Values
- **Type**: `bool`
- **Description**: `TRUE` if all deletions were successful; `FALSE` otherwise (e.g., lack of permissions).

#### Inner Mechanisms
1. Checks for `operator` permission. Returns `FALSE` if not granted.
2. Converts a single `index` into an array for uniform processing.
3. Iterates through each `index`:
   - Deletes the media type entry from the database.
   - Deletes the associated template file if it exists.
4. Calls `save()` to persist changes and update the type mapping.

#### Usage Context
- Used in administrative interfaces to remove media types.

---

### Method: `parse()`

#### Purpose
Generates HTML markup for a media file by applying a template and replacing placeholders with provided values.

#### Parameters

| Name     | Type      | Description                                                                 |
|----------|-----------|-----------------------------------------------------------------------------|
| `index`  | `string`  | Unique ID of the media type.                                                |
| `url`    | `string`  | URL of the media file.                                                      |
| `id`     | `string`  | (Optional) HTML `id` attribute for the media element.                       |
| `type`   | `string`  | (Optional) MIME type of the media file.                                     |
| `width`  | `string`  | (Optional) Width of the media element.                                      |
| `height` | `string`  | (Optional) Height of the media element.                                     |
| `alt`    | `string`  | (Optional) Alternative text for the media element.                          |
| `title`  | `string`  | (Optional) Title attribute for the media element.                           |
| `class`  | `string`  | (Optional) CSS class(es) for the media element.                             |
| `style`  | `string`  | (Optional) Inline CSS styles for the media element.                         |

#### Return Values
- **Type**: `string`
- **Description**: The generated HTML markup for the media file.

#### Inner Mechanisms
1. Attempts to load the HTML template code for the given `index`. If not found:
   - Falls back to identifying the media type by file extension.
   - If no template is found, uses a default `<object>` template.
2. Determines the MIME type of the media file if not provided.
3. Replaces placeholders in the template (e.g., `%src%`, `%width%`) with the provided values. Placeholders wrapped in square brackets (e.g., `[ width="%width%"]`) are conditionally included only if the corresponding value is non-empty.
4. Returns the processed HTML markup.

#### Usage Context
- Used to render media files (e.g., videos, audio, PDFs) in frontend views.

---

### Method: `update_type()`

#### Purpose
Rebuilds the `type` property array, which maps file extensions to media type IDs.

#### Parameters
None.

#### Return Values
None.

#### Inner Mechanisms
1. Resets the `type` property to an empty array.
2. Sorts the media type entries by name.
3. Iterates through each media type entry:
   - Splits the `type` field (comma-separated extensions) into an array.
   - Trims and converts each extension to lowercase.
   - Maps each extension to the media type ID.
4. Caches the updated `type` array using `cms_cache()`.

#### Usage Context
- Called automatically during initialization if the cache is empty.
- Called after modifications to media type entries (e.g., `add()`, `set()`, `delete()`).

---

### Method: `save()`

#### Purpose
Persists changes to the media type database and updates the type mapping cache.

#### Parameters
None.

#### Return Values
- **Type**: `bool`
- **Description**: `TRUE` if changes were saved successfully; `FALSE` otherwise (e.g., lack of permissions).

#### Inner Mechanisms
1. Checks for `operator` permission. Returns `FALSE` if not granted.
2. Calls `update_type()` to rebuild the type mapping.
3. Saves the `data` object to persist changes to the database.

#### Usage Context
- Called after modifications to media type entries to ensure changes are persisted.


<!-- HASH:9d2de18e951ef8a59d35251ad2388e79 -->
