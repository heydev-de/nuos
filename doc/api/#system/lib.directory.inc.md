# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.directory.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/%23system/lib.directory.inc)

- **Version:** `26.4.29.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Directory Management Module

This file provides core functionality for managing the NUOS platform's directory structure, which serves as the foundation for URL routing, content organization, and filesystem generation. It includes both standalone utility functions and a `directory` class for programmatic manipulation of directory entries.

---

## Functions

### `directory_get_canonical`

**Purpose:**
Retrieves the canonical URL for a given directory index from the system's canonical map.

**Parameters:**

| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| index | int    | NULL    | Directory index. If NULL, defaults to 0 (root). |

**Return Values:**
- `string`: Canonical URL for the specified index.

**Inner Mechanisms:**
1. Determines the current language context (appends language code to map path if set).
2. Instantiates a `map` object to load the canonical URL mappings.
3. Returns the URL associated with the provided index.

**Usage Context:**
Used when resolving logical directory paths into physical URLs, particularly for generating links or redirects.

---

### `directory_flexview_display_function`

**Purpose:**
Custom display function for rendering directory entries in a `flexview` UI component. Handles visual styling, icons, and state-based formatting (hidden, placeholder, used).

**Parameters:**

| Name     | Type      | Default | Description                                                                 |
|----------|-----------|---------|-----------------------------------------------------------------------------|
| flexview | flexview  | —       | The flexview instance rendering the directory.                              |
| index    | string    | —       | Current directory entry index.                                              |
| open     | bool      | —       | Whether the entry is expanded in the UI.                                    |

**Return Values:**
- None (outputs HTML directly via `echo`).

**Inner Mechanisms:**
1. Determines if the current entry is active (matches `flexview->index`).
2. Resolves the entry's name, visibility state (hidden, placeholder, used), and icon.
3. Applies conditional styling:
   - Hidden entries are wrapped in parentheses.
   - Placeholder entries are colored (green if used, red if unused).
4. Constructs a display string using `flexview->display` template, replacing placeholders (`%icon%`, `%name%`, etc.) with resolved values.
5. Outputs the final HTML.

**Usage Context:**
Used as a callback for `flexview` instances displaying directory structures in the admin UI.

---

### `directory_get_select`

**Purpose:**
Generates an associative array of directory entries suitable for use in `<select>` dropdowns. Applies indentation to reflect hierarchy.

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array where keys are display names (indented) and values are directory keys.

**Inner Mechanisms:**
1. Loads directory data from `#system/directory`.
2. Iterates through entries, applying indentation for nested containers.
3. Skips unnamed entries, replacing them with `CMS_L_UNKNOWN`.
4. Handles container start/end markers to adjust indentation level.

**Usage Context:**
Used in forms where users select a directory entry (e.g., content assignment, URL routing).

---

### `directory_get_type`

**Purpose:**
Retrieves icon URLs for directory entry types (e.g., "page", "folder") from the `#system/directory.type` data source.

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array where keys are type identifiers (e.g., "page", "+page") and values are icon URLs.

**Inner Mechanisms:**
1. Loads type definitions from `#system/directory.type`.
2. Resolves both standard (`format`) and expanded (`+format`) icon paths.
3. Constructs absolute URLs using `CMS_DATA_URL`.

**Usage Context:**
Used to fetch icons for directory entries in UIs (e.g., flexview, menus).

---

### `directory_get_type_select`

**Purpose:**
Generates an associative array of directory entry types for use in `<select>` dropdowns.

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array where keys are display names and values are type identifiers.

**Inner Mechanisms:**
1. Loads type definitions from `#system/directory.type`.
2. Resolves display names, defaulting to `CMS_L_UNKNOWN` if empty.
3. Ensures unique keys by appending suffixes (e.g., "Page (1)") if duplicates exist.
4. Sorts the array alphabetically.

**Usage Context:**
Used in forms where users select a directory entry type (e.g., during entry creation).

---

### `directory_get_visible`

**Purpose:**
Finds the nearest visible ancestor of a given directory entry, accounting for hidden containers.

**Parameters:**

| Name  | Type   | Default | Description                                                                 |
|-------|--------|---------|-----------------------------------------------------------------------------|
| index | string | NULL    | Directory index. If NULL, uses `CMS_CONTENT_DIRECTORY_INDEX`.               |

**Return Values:**
- `string`: Key of the nearest visible ancestor (or the entry itself if visible). Returns `0` if no visible ancestor exists.

**Inner Mechanisms:**
1. Validates the existence of the entry.
2. Traverses the directory hierarchy, tracking visibility state (hidden containers propagate invisibility to children).
3. Returns the last visible ancestor encountered.

**Usage Context:**
Used to resolve fallback URLs when a requested entry is hidden (e.g., redirects, breadcrumbs).

---

### `directory_value`

**Purpose:**
Resolves a directory entry's URL, optionally appending query parameters from the current request.

**Parameters:**

| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| index | string | —       | Directory index.                     |

**Return Values:**
- `string`: Absolute URL for the entry, with optional query parameters.

**Inner Mechanisms:**
1. Loads the canonical URL from the language-specific directory map.
2. Appends query parameters from `cms_param()` if present, inserting them before a `#` fragment if one exists.

**Usage Context:**
Used to generate links to directory entries, preserving the current query state.

---

### `directory_get_flexview`

**Purpose:**
Constructs a `flexview` instance for displaying the directory structure, with optional filtering of hidden/placeholder entries.

**Parameters:**

| Name           | Type | Default | Description                                      |
|----------------|------|---------|--------------------------------------------------|
| remove_hidden  | bool | TRUE    | If TRUE, omits hidden and unused placeholder entries. |

**Return Values:**
- `flexview|bool`: Configured `flexview` instance, or `FALSE` if `flexview` library fails to load.

**Inner Mechanisms:**
1. Loads the `flexview` library.
2. Initializes a `flexview` instance with `directory_value` as the value resolver.
3. Iterates through directory entries, skipping hidden or unused placeholder entries if `remove_hidden` is TRUE.
4. Builds a hierarchical structure using a stack-based approach.

**Usage Context:**
Used to render directory trees in the admin UI.

---

### `directory_create_filesystem`

**Purpose:**
Generates the physical filesystem structure (directories and PHP files) based on the logical directory configuration. Handles language-specific paths, canonical URLs, and auxiliary files.

**Parameters:**

| Name     | Type   | Default               | Description                                                                 |
|----------|--------|-----------------------|-----------------------------------------------------------------------------|
| language | string | CMS_LANGUAGE_ENABLED  | Comma-separated list of languages to generate. Empty string generates all. |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure.

**Inner Mechanisms:**
1. **Initialization:**
   - Reverts any previously generated files (via `directory_remove_filesystem`).
   - Loads auxiliary files (e.g., `.htaccess`) from `CMS_DATA_PATH/#directory/auxiliary/`.
   - Opens a log file (`directory.log`) to track generated files.
2. **Language-Specific Setup:**
   - Initializes paths, canonical URLs, and maps for each language.
   - Creates a root `index.php` file.
3. **Directory Traversal:**
   - Processes each entry, skipping unused placeholders.
   - Resolves language-specific names, paths, and canonical URLs.
   - Generates unique directory/file names to avoid conflicts.
   - Creates directories and copies auxiliary files.
   - Generates PHP files with bootstrap code to set `content_directory_index` and load the content module.
4. **URL Resolution:**
   - Handles directory references (e.g., `directory://key#fragment`) by resolving them to physical paths.
   - Tracks canonical URLs to ensure uniqueness (preferring shorter paths).
5. **Map Generation:**
   - Saves language-specific maps for paths, canonical URLs, and content associations.
6. **Cleanup:**
   - Closes the log file.

**Usage Context:**
Called during deployment or when the directory structure is modified. Typically invoked via `directory->save()`.

---

### `directory_remove_filesystem`

**Purpose:**
Reverts the physical filesystem to its pre-generation state by deleting files and directories logged in `directory.log`.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
1. Reads `directory.log` to retrieve the list of generated files/directories.
2. Deletes files and removes empty directories in reverse order of generation.

**Usage Context:**
Called before regenerating the filesystem to ensure a clean slate.

---

## Class: `directory`

**Purpose:**
Provides an object-oriented interface for manipulating the directory structure stored in `#system/directory`.

### Properties

| Name | Type  | Description                          |
|------|-------|--------------------------------------|
| data | data  | Instance of the `data` class for `#system/directory`. |

---

### Constructor: `__construct`

**Purpose:**
Initializes the `directory` instance by loading the `#system/directory` data source.

**Parameters:**
- None.

---

### Method: `append`

**Purpose:**
Appends a new directory entry after the specified key.

**Parameters:**

| Name           | Type    | Default | Description                                                                 |
|----------------|---------|---------|-----------------------------------------------------------------------------|
| key            | string  | —       | Key after which the new entry will be inserted.                             |
| name           | string  | —       | Display name of the entry.                                                  |
| description    | string  | ""      | Description of the entry.                                                   |
| url            | string  | ""      | URL or logical identifier (e.g., `content://page`).                         |
| subtype        | string  | ""      | Entry subtype (e.g., "page", "folder").                                     |
| hidden         | bool    | FALSE   | If TRUE, the entry is hidden from navigation.                               |
| placeholder    | bool    | FALSE   | If TRUE, the entry is a placeholder (e.g., for future content).             |
| dynamic        | bool    | TRUE    | If TRUE, the entry is dynamically generated (not static).                   |
| image_button   | string  | ""      | Path to the button-state icon.                                              |
| image_hover    | string  | ""      | Path to the hover-state icon.                                               |
| image_active   | string  | ""      | Path to the active-state icon.                                              |
| path           | string  | ""      | Overrides the default filesystem path.                                      |
| canonical      | string  | ""      | Overrides the default canonical URL.                                        |

**Return Values:**
- `string`: Key of the newly appended entry.

**Inner Mechanisms:**
1. Constructs a buffer with the entry data, including a closing `/container` marker.
2. Delegates to `data->append` to insert the entry.

**Usage Context:**
Used to add new entries to the directory structure programmatically.

---

### Method: `insert`

**Purpose:**
Inserts a new directory entry before the specified key.

**Parameters:**
- Same as `append`.

**Return Values:**
- `string`: Key of the newly inserted entry.

**Inner Mechanisms:**
1. Constructs a buffer with the entry data, including a closing `/container` marker.
2. Delegates to `data->insert` to insert the entry.

---

### Method: `set`

**Purpose:**
Updates one or more properties of an existing directory entry.

**Parameters:**

| Name           | Type    | Default | Description                                                                 |
|----------------|---------|---------|-----------------------------------------------------------------------------|
| key            | string  | —       | Key of the entry to update.                                                 |
| name           | string  | NULL    | New display name (NULL to skip).                                            |
| description    | string  | NULL    | New description (NULL to skip).                                             |
| url            | string  | NULL    | New URL (NULL to skip).                                                     |
| subtype        | string  | NULL    | New subtype (NULL to skip).                                                 |
| hidden         | bool    | NULL    | New hidden state (NULL to skip).                                            |
| placeholder    | bool    | NULL    | New placeholder state (NULL to skip).                                       |
| dynamic        | bool    | NULL    | New dynamic state (NULL to skip).                                           |
| image_button   | string  | NULL    | New button-state icon (NULL to skip).                                       |
| image_hover    | string  | NULL    | New hover-state icon (NULL to skip).                                        |
| image_active   | string  | NULL    | New active-state icon (NULL to skip).                                       |
| path           | string  | NULL    | New path override (NULL to skip).                                           |
| canonical      | string  | NULL    | New canonical URL override (NULL to skip).                                  |

**Return Values:**
- `string|bool`: Key of the updated entry on success, FALSE if the entry does not exist.

**Inner Mechanisms:**
1. Validates the existence of the entry.
2. Updates each non-NULL property individually via `data->set`.

---

### Method: `del`

**Purpose:**
Deletes a directory entry and its children.

**Parameters:**

| Name | Type   | Description               |
|------|--------|---------------------------|
| key  | string | Key of the entry to delete. |

**Return Values:**
- `bool`: TRUE on success, FALSE on failure.

**Inner Mechanisms:**
Delegates to `data->del`.

---

### Method: `parse_placeholder`

**Purpose:**
Marks placeholder entries as "used" if they contain non-placeholder children, or "unused" otherwise.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
1. Traverses the directory hierarchy using a stack-based approach.
2. Tracks the depth of the last non-placeholder entry (`limit`).
3. Marks entries as "used" if they are placeholders and contain non-placeholder children, or "unused" otherwise.

**Usage Context:**
Called automatically during `save()` to update placeholder states.

---

### Method: `save`

**Purpose:**
Saves the directory data and regenerates the filesystem.

**Parameters:**
- None.

**Return Values:**
- `bool`: TRUE on success, FALSE on failure.

**Inner Mechanisms:**
1. Calls `parse_placeholder` to update placeholder states.
2. Saves the directory data via `data->save`.
3. Calls `directory_create_filesystem` to regenerate the filesystem.

**Usage Context:**
Called after modifying the directory structure to persist changes and update the filesystem.


<!-- HASH:8a588fefdbb46a85be024bc064393187 -->
