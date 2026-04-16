# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.directory.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Directory Management Module

This file provides the core directory management functionality for the NUOS web platform. It handles the creation, manipulation, and rendering of hierarchical directory structures that define the site's navigation, URL routing, and content organization.

The module includes:
- Functions for retrieving canonical paths, directory types, and visibility states
- Flexible view rendering for directory trees
- Filesystem generation based on directory definitions
- A `directory` class for programmatic directory manipulation

---

### Functions

---

#### `directory_get_canonical`

**Purpose:**
Retrieves a canonical URL path for a given directory index from the system's directory map.

**Parameters:**

| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| index | int    | NULL    | Directory index. If NULL, defaults to 0 (root). |

**Return Values:**
- `string`: The canonical path for the specified directory index.

**Inner Mechanisms:**
1. Determines the language prefix for the map file (e.g., `en.directory.canonical`).
2. Loads the map file using the `map` class.
3. Returns the value associated with the provided index.

**Usage Context:**
Used when generating absolute URLs for directory entries, especially in multilingual setups.

---

#### `directory_flexview_display_function`

**Purpose:**
Custom display function for rendering directory entries in a `flexview` component. Handles icons, styling, and visibility states (hidden, placeholder, used).

**Parameters:**

| Name      | Type      | Default | Description                                                                 |
|-----------|-----------|---------|-----------------------------------------------------------------------------|
| flexview  | flexview  | —       | The flexview instance rendering the directory.                              |
| index     | string    | —       | The current directory entry index.                                          |
| open      | bool      | —       | Whether the entry is expanded in the view.                                  |

**Return Values:**
- None. Outputs HTML directly via `echo`.

**Inner Mechanisms:**
1. Determines if the current entry is active (matches `flexview->index`).
2. Retrieves the entry name and applies XML escaping via `x()`.
3. Checks for hidden, placeholder, and used flags.
4. Resolves the appropriate icon based on entry type and state.
5. Applies conditional styling (e.g., green for used placeholders, red for unused).
6. Constructs a display string using placeholders (`%icon%`, `%name%`, etc.) and replaces them with actual values.
7. Outputs the final HTML string.

**Usage Context:**
Used as a callback in `flexview` instances to render directory trees in the admin interface.

---

#### `directory_get_select`

**Purpose:**
Generates an associative array suitable for use in HTML `<select>` elements, mapping human-readable directory paths to their internal keys.

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array of the form `["Display Name" => key, ...]`.

**Inner Mechanisms:**
1. Loads directory data using the `data` class.
2. Initializes the array with a default "Select" option.
3. Traverses the directory tree, applying indentation to reflect hierarchy.
4. Skips entries with empty names.
5. Handles container types by adding parentheses and indentation.
6. Ensures unique display names by appending `(1)`, `(2)`, etc., if duplicates exist.

**Usage Context:**
Used in forms where users need to select a directory (e.g., content assignment, menu configuration).

---

#### `directory_get_type`

**Purpose:**
Retrieves URLs for directory entry type icons (both standard and expanded states).

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array mapping type names (e.g., `article`, `+article`) to icon URLs.

**Inner Mechanisms:**
1. Loads type data from `#system/directory.type`.
2. For each type, checks for `format` and `+format` fields to determine icon filenames.
3. Constructs full URLs using `CMS_DATA_URL`.

**Usage Context:**
Used to display appropriate icons for different directory entry types in the UI.

---

#### `directory_get_type_select`

**Purpose:**
Generates an associative array of directory entry types for use in `<select>` elements.

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array of the form `["Display Name" => type_key, ...]`.

**Inner Mechanisms:**
1. Loads type data from `#system/directory.type`.
2. Maps each type to its display name, ensuring uniqueness by appending `(1)`, `(2)`, etc.
3. Sorts the array alphabetically.

**Usage Context:**
Used in forms where users can select the type of a new directory entry.

---

#### `directory_get_visible`

**Purpose:**
Finds the nearest visible ancestor of a given directory entry, respecting hidden flags.

**Parameters:**

| Name  | Type   | Default | Description                                                                 |
|-------|--------|---------|-----------------------------------------------------------------------------|
| index | string | NULL    | Directory index. If NULL, uses `CMS_CONTENT_DIRECTORY_INDEX`.              |

**Return Values:**
- `string`: The index of the nearest visible ancestor, or `0` if none exists.

**Inner Mechanisms:**
1. Validates the existence of the entry.
2. Traverses the directory tree from the root, tracking hidden state via a stack.
3. Returns the last visible ancestor before encountering the target index.

**Usage Context:**
Used to determine fallback navigation targets when a requested directory is hidden.

---

#### `directory_value`

**Purpose:**
Generates a full URL for a directory entry, optionally appending query parameters.

**Parameters:**

| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| index | string | —       | Directory index.                     |

**Return Values:**
- `string`: The full URL for the directory entry.

**Inner Mechanisms:**
1. Loads the directory map for the current language.
2. Retrieves the base path.
3. Appends current query parameters (from `cms_param()`) if a `#` anchor is present in the path.
4. Prepends `CMS_ROOT_URL`.

**Usage Context:**
Used to generate links to directory entries in navigation menus and content.

---

#### `directory_get_flexview`

**Purpose:**
Creates a `flexview` instance populated with directory data, optionally filtering out hidden or unused placeholder entries.

**Parameters:**

| Name           | Type | Default | Description                                      |
|----------------|------|---------|--------------------------------------------------|
| remove_hidden  | bool | TRUE    | Whether to exclude hidden or unused entries.     |

**Return Values:**
- `flexview|bool`: A configured `flexview` instance, or `FALSE` if `flexview` is not loaded.

**Inner Mechanisms:**
1. Loads the `flexview` library.
2. Initializes a `flexview` instance and sets its value function to `directory_value`.
3. Traverses the directory tree, skipping entries based on `remove_hidden`.
4. Populates the `flexview` with valid entries.

**Usage Context:**
Used to render interactive directory trees in the admin interface.

---

#### `directory_create_filesystem`

**Purpose:**
Generates the physical filesystem structure (directories and PHP files) based on the directory definitions. Also creates canonical URL maps.

**Parameters:**

| Name      | Type   | Default               | Description                                      |
|-----------|--------|-----------------------|--------------------------------------------------|
| language  | string | CMS_LANGUAGE_ENABLED  | Comma-separated list of languages to generate.   |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Cleans up any previously generated files via `directory_remove_filesystem`.
2. Copies auxiliary files (e.g., `.htaccess`) to each directory.
3. Initializes per-language path and canonical URL tracking.
4. Traverses the directory tree:
   - Skips unused placeholder entries.
   - Creates directories and PHP files for each entry.
   - Handles path and canonical URL overrides.
   - Ensures unique filenames via suffixes (`-1`, `-2`).
   - Writes PHP files that set the appropriate GET parameters and include the content module.
5. Resolves directory references (e.g., `directory://other_entry`).
6. Saves language-specific maps for paths, canonical URLs, and content associations.
7. Logs all generated files in `directory.log`.

**Usage Context:**
Called automatically after directory changes are saved. Can also be triggered manually to regenerate the filesystem.

---

#### `directory_remove_filesystem`

**Purpose:**
Removes all files and directories generated by `directory_create_filesystem` using the log file as a reference.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
1. Reads `directory.log` to get a list of generated files and directories.
2. Deletes files and removes empty directories in reverse order.

**Usage Context:**
Used to clean up before regenerating the filesystem or during uninstallation.

---

## `directory` Class

The `directory` class provides an object-oriented interface for manipulating the directory structure.

### Properties

| Name | Type  | Description                          |
|------|-------|--------------------------------------|
| data | data  | Instance of the `data` class holding directory data. |

### Constructor

```php
function __construct()
```

**Purpose:**
Initializes the class by loading the directory data from `#system/directory`.

---

### Methods

---

#### `append`

**Purpose:**
Appends a new directory entry after the specified key.

**Parameters:**

| Name            | Type    | Default | Description                                      |
|-----------------|---------|---------|--------------------------------------------------|
| key             | string  | —       | The key after which the new entry is inserted.   |
| name            | string  | —       | Display name of the entry.                       |
| description     | string  | ""      | Description of the entry.                        |
| url             | string  | ""      | URL or logical identifier (e.g., `content://...`). |
| subtype         | string  | ""      | Entry type (e.g., `article`).                    |
| hidden          | bool    | FALSE   | Whether the entry is hidden.                     |
| placeholder     | bool    | FALSE   | Whether the entry is a placeholder.              |
| dynamic         | bool    | TRUE    | Whether the entry is dynamic.                    |
| image_button    | string  | ""      | Image for the button state.                      |
| image_hover     | string  | ""      | Image for the hover state.                       |
| image_active    | string  | ""      | Image for the active state.                      |
| path            | string  | ""      | Override path for the entry.                     |
| canonical       | string  | ""      | Override canonical URL.                          |

**Return Values:**
- `string|bool`: The key of the new entry, or `FALSE` on failure.

---

#### `insert`

**Purpose:**
Inserts a new directory entry before the specified key.

**Parameters:**
Same as `append`.

**Return Values:**
- `string|bool`: The key of the new entry, or `FALSE` on failure.

---

#### `set`

**Purpose:**
Updates one or more properties of an existing directory entry.

**Parameters:**

| Name            | Type    | Default | Description                                      |
|-----------------|---------|---------|--------------------------------------------------|
| key             | string  | —       | The key of the entry to update.                  |
| name            | string  | NULL    | New name (NULL to leave unchanged).              |
| description     | string  | NULL    | New description.                                 |
| url             | string  | NULL    | New URL.                                         |
| subtype         | string  | NULL    | New subtype.                                     |
| hidden          | bool    | NULL    | New hidden state.                                |
| placeholder     | bool    | NULL    | New placeholder state.                           |
| dynamic         | bool    | NULL    | New dynamic state.                               |
| image_button    | string  | NULL    | New button image.                                |
| image_hover     | string  | NULL    | New hover image.                                 |
| image_active    | string  | NULL    | New active image.                                |
| path            | string  | NULL    | New path override.                               |
| canonical       | string  | NULL    | New canonical URL override.                      |

**Return Values:**
- `string|bool`: The entry key on success, `FALSE` if the entry does not exist.

---

#### `del`

**Purpose:**
Deletes a directory entry and its children.

**Parameters:**

| Name | Type   | Default | Description              |
|------|--------|---------|--------------------------|
| key  | string | —       | The key of the entry.    |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

---

#### `parse_placeholder`

**Purpose:**
Marks placeholder entries as "used" if they contain non-placeholder children, otherwise marks them as unused.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
1. Traverses the directory tree.
2. Tracks depth and the last non-placeholder entry.
3. Sets the `used` flag on placeholders that have non-placeholder descendants.

**Usage Context:**
Called automatically before saving to ensure correct state tracking.

---

#### `save`

**Purpose:**
Saves the directory data and regenerates the filesystem.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Calls `parse_placeholder()` to update placeholder states.
2. Saves the data via `data->save()`.
3. Calls `directory_create_filesystem()` to regenerate the filesystem.

**Usage Context:**
Should be called after making changes to the directory structure.


<!-- HASH:4b9f5dc85c4ea798283f0574b36286ba -->
