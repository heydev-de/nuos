# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.directory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.directory.inc)

- **Version:** `26.9.11.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/lib.directory.inc

## Overview

The `#system/lib.directory.inc` file is a core component of the PWNC Web Platform responsible for managing the **directory structure** of a website or web application. It provides functionality to define, manipulate, and generate the physical filesystem layout based on logical directory entries stored in a data store (`#system/directory`).

This includes:

- Defining hierarchical directory entries (containers and content pages)
- Generating physical PHP files and directories on disk
- Managing canonical URLs, content mappings, and host lists
- Supporting multilingual path generation
- Providing helper functions for rendering directory-based UI components (e.g., flexviews)

The file defines several standalone utility functions and a `directory` class that encapsulates operations on the directory data structure.

---

## Functions

### `directory_get_canonical`

#### Purpose
Retrieves the canonical URL for a given directory index from a language-specific map.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | string\|int\|NULL | `NULL` | The directory index key. If empty, defaults to `0`. |

#### Return Values
- **Type:** string  
- **Description:** The canonical URL associated with the given index.

#### Inner Mechanisms
1. If `$index` is empty (`stre($index)`), it defaults to `0`.
2. Constructs a language-aware map path using `CMS_LANGUAGE`.
3. Loads the map from `#system/{language}directory.canonical`.
4. Returns the value at the specified index.

#### Usage Example
```php
$canonical = directory_get_canonical('home');
echo $canonical; // Outputs the canonical URL for the 'home' directory entry
```

---

### `directory_flexview_display_function`

#### Purpose
Renders a single entry in a directory flexview UI component, including icons, titles, checkboxes, and marks.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$flexview` | object | A `flexview` instance containing directory data. |
| `$index` | string | The current directory entry index. |
| `$open` | boolean | Whether the entry is currently expanded/open. |

#### Return Values
- **Type:** void  
- **Description:** Outputs HTML directly via `echo()`.

#### Inner Mechanisms
1. Determines if the current entry matches the active index.
2. Retrieves title, name, and metadata flags (hidden, placeholder, used).
3. Selects an appropriate icon based on state and custom icon definitions.
4. Builds an array of display variables (index, action, title, etc.).
5. Replaces placeholders in the flexview template with actual values.
6. Outputs the final rendered HTML.

#### Usage Example
```php
$flexview = directory_get_flexview();
directory_flexview_display_function($flexview, 'about', TRUE);
// Renders the 'about' directory entry as open in the flexview UI
```

---

### `directory_get_select`

#### Purpose
Generates a flat list of directory entries suitable for use in a `<select>` dropdown, with indentation for nested containers.

#### Parameters
None.

#### Return Values
- **Type:** array  
- **Description:** An associative array where keys are display names (with indentation) and values are directory keys.

#### Inner Mechanisms
1. Loads directory data from `#system/directory`.
2. Iterates through entries, tracking container depth.
3. For containers, adds em-space indentation and handles duplicate names.
4. For content entries, adds them to the result array.
5. Returns the structured list.

#### Usage Example
```php
$options = directory_get_select();
foreach ($options as $label => $value) {
    echo "<option value='$value'>$label</option>";
}
```

---

### `directory_get_type`

#### Purpose
Retrieves a mapping of directory type identifiers to their corresponding data URLs.

#### Parameters
None.

#### Return Values
- **Type:** array  
- **Description:** Associative array mapping type keys (e.g., `"container"`, `"+container"`) to data URLs.

#### Inner Mechanisms
1. Loads type definitions from `#system/directory.type`.
2. For each entry, checks for `format` and `+format` fields.
3. Constructs URLs using `CMS_DATA_URL` and rawurlencoded keys.
4. Returns the complete type-to-URL mapping.

#### Usage Example
```php
$types = directory_get_type();
foreach ($types as $key => $url) {
    echo "$key => $url\n";
}
```

---

### `directory_get_type_select`

#### Purpose
Generates a human-readable list of directory types for use in a select dropdown, with formatted labels.

#### Parameters
None.

#### Return Values
- **Type:** array  
- **Description:** Associative array with display labels as keys and type identifiers as values.

#### Inner Mechanisms
1. Loads type definitions from `#system/directory.type`.
2. Retrieves localized names for each type.
3. Handles duplicate names by appending numeric suffixes.
4. Sorts entries naturally (case-insensitive).
5. Formats labels with file paths and extensions.
6. Prepends an empty option.

#### Usage Example
```php
$typeOptions = directory_get_type_select();
foreach ($typeOptions as $label => $value) {
    echo "<option value='$value'>$label</option>";
}
```

---

### `directory_get_visible`

#### Purpose
Determines the topmost visible parent directory index for a given entry, considering hidden containers.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | string\|NULL | `NULL` | The directory entry index to check. Defaults to `CMS_CONTENT_DIRECTORY_INDEX` if not provided. |

#### Return Values
- **Type:** int\|string  
- **Description:** The index of the topmost visible parent container, or `0` if the entry doesn't exist.

#### Inner Mechanisms
1. Uses a stack to track hidden state across nested containers.
2. Iterates through directory entries until reaching the target index.
3. Tracks whether any ancestor container is hidden.
4. Returns the last non-hidden container index encountered.

#### Usage Example
```php
$visibleParent = directory_get_visible('products');
if ($visibleParent > 0) {
    echo "Visible under parent: $visibleParent";
}
```

---

### `directory_value`

#### Purpose
Generates a full URL for a directory entry by combining its base path with the current CMS parameter state.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | string | The directory entry index. |

#### Return Values
- **Type:** string  
- **Description:** The complete URL for the directory entry, including any appended parameters.

#### Inner Mechanisms
1. Loads the directory map for the current language.
2. Retrieves the base path for the given index.
3. Appends the current CMS parameter string (if any) after a `#` separator.
4. Prepends `CMS_ROOT_URL` to form the full URL.

#### Usage Example
```php
$url = directory_value('contact');
echo "<a href='$url'>Contact Us</a>";
```

---

### `directory_get_flexview`

#### Purpose
Creates and populates a `flexview` object representing the directory structure, optionally removing hidden entries.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$remove_hidden` | boolean | `TRUE` | Whether to exclude hidden directory entries. |

#### Return Values
- **Type:** object\|boolean  
- **Description:** A `flexview` object on success, or `FALSE` if the flexview library cannot be loaded.

#### Inner Mechanisms
1. Loads the `flexview` library.
2. Initializes a new `flexview` instance.
3. Sets a custom value function (`directory_value`) for URL generation.
4. Iterates through directory data, adding containers and content entries.
5. Skips hidden entries if `$remove_hidden` is `TRUE`.
6. Maintains parent-child relationships using a stack-like array.

#### Usage Example
```php
$flexview = directory_get_flexview();
if ($flexview !== FALSE) {
    $flexview->render();
}
```

---

### `directory_create_filesystem`

#### Purpose
Generates the physical filesystem structure (directories and PHP files) based on the logical directory data.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$language` | string | `CMS_LANGUAGE_ENABLED` | Comma-separated list of languages to process. |

#### Return Values
- **Type:** boolean  
- **Description:** `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Clears file status cache.
2. Reverts any previously logged filesystem changes.
3. Initializes data structures for paths, canonicals, URLs, and content mappings.
4. Processes each language variant:
   - Sets up initial paths and canonical URLs.
   - Handles auxiliary files (copied to generated directories).
5. Creates the root `index.php` file.
6. Opens a log file for tracking created files/directories.
7. Iterates through directory entries:
   - Skips unused placeholders.
   - Resolves URLs (directory://, content:// schemes).
   - Generates unique filenames and creates directories/files.
   - Maps entry paths and canonical URLs.
8. Resolves directory references (dereferences).
9. Saves generated maps (path, canonical, content) for each language.
10. Updates the host list file.
11. Closes the log file and returns success/failure status.

#### Usage Example
```php
if (directory_create_filesystem()) {
    echo "Filesystem structure generated successfully.";
} else {
    echo "Failed to generate filesystem structure.";
}
```

---

### `directory_remove_filesystem`

#### Purpose
Removes all files and directories previously created by `directory_create_filesystem`, based on the log file.

#### Parameters
None.

#### Return Values
- **Type:** boolean  
- **Description:** `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Migrates legacy log file if present.
2. Opens the log file with exclusive lock.
3. Reads all logged paths into an array.
4. Iterates backwards through the array (to remove files before parent directories).
5. Deletes files and directories as appropriate.
6. Truncates the log file on success.
7. Closes the file handle and returns status.

#### Usage Example
```php
if (directory_remove_filesystem()) {
    echo "All generated files removed.";
} else {
    echo "Failed to remove some files.";
}
```

---

## Class: `directory`

### Overview
The `directory` class provides an object-oriented interface for managing directory entries in the PWNC data store. It wraps the underlying `data` object and provides methods for appending, inserting, setting, and deleting directory entries.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | object | A `data` instance bound to `#system/directory`. |

---

### Methods

#### `__construct`

##### Purpose
Initializes the directory object by creating a `data` instance for `#system/directory`.

##### Parameters
None.

##### Return Values
None.

##### Inner Mechanisms
Instantiates a new `data` object pointing to the directory data store.

##### Usage Example
```php
$dir = new directory();
```

---

#### `append`

##### Purpose
Appends a new directory entry (container) to the end of the data store.

##### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$key` | string | — | Unique identifier for the entry. |
| `$name` | string | — | Display name of the entry. |
| `$description` | string | `""` | Optional description. |
| `$url` | string | `""` | URL or reference for the entry. |
| `$subtype` | string | `""` | Subtype identifier. |
| `$hidden` | boolean | `FALSE` | Whether the entry is hidden. |
| `$placeholder` | boolean | `FALSE` | Whether the entry is a placeholder. |
| `$dynamic` | boolean | `TRUE` | Whether the entry is dynamically generated. |
| `$image_button` | string | `""` | Button image identifier. |
| `$image_hover` | string | `""` | Hover image identifier. |
| `$image_active` | string | `""` | Active image identifier. |
| `$path` | string | `""` | Custom filesystem path. |
| `$canonical` | string | `""` | Canonical URL override. |

##### Return Values
- **Type:** mixed  
- **Description:** Result of `$this->data->append($key)`.

##### Inner Mechanisms
1. Prepares a buffer with two elements: the container definition and a closing `/container` marker.
2. Calls `$this->data->set_buffer()` to stage the data.
3. Calls `$this->data->append($key)` to add the entry.

##### Usage Example
```php
$dir = new directory();
$dir->append('about', 'About Us', '', 'content://about', '', FALSE, FALSE, TRUE, 'btn_about', 'btn_about_h', 'btn_about_a');
$dir->save();
```

---

#### `insert`

##### Purpose
Inserts a new directory entry at a specific position in the data store.

##### Parameters
Same as `append`, except `$key` specifies the insertion point.

##### Return Values
- **Type:** mixed  
- **Description:** Result of `$this->data->insert($key)`.

##### Inner Mechanisms
Same as `append`, but uses `$this->data->insert($key)` instead.

##### Usage Example
```php
$dir = new directory();
$dir->insert('home', 'Home', '', 'content://home');
$dir->save();
```

---

#### `set`

##### Purpose
Updates one or more properties of an existing directory entry.

##### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$key` | string | — | The entry key to update. |
| `$name` | string\|NULL | `NULL` | New name. |
| `$description` | string\|NULL | `NULL` | New description. |
| `$url` | string\|NULL | `NULL` | New URL. |
| `$subtype` | string\|NULL | `NULL` | New subtype. |
| `$hidden` | boolean\|NULL | `NULL` | New hidden flag. |
| `$placeholder` | boolean\|NULL | `NULL` | New placeholder flag. |
| `$dynamic` | boolean\|NULL | `NULL` | New dynamic flag. |
| `$image_button` | string\|NULL | `NULL` | New button image. |
| `$image_hover` | string\|NULL | `NULL` | New hover image. |
| `$image_active` | string\|NULL | `NULL` | New active image. |
| `$path` | string\|NULL | `NULL` | New path. |
| `$canonical` | string\|NULL | `NULL` | New canonical URL. |

##### Return Values
- **Type:** string\|boolean  
- **Description:** The entry key on success, `FALSE` if the entry doesn't exist.

##### Inner Mechanisms
1. Checks if the entry exists.
2. For each non-NULL parameter, calls `$this->data->set()` to update the corresponding field.
3. Returns the key.

##### Usage Example
```php
$dir = new directory();
$dir->set('about', 'About Our Company');
$dir->save();
```

---

#### `del`

##### Purpose
Deletes a directory entry from the data store.

##### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$key` | string | The entry key to delete. |

##### Return Values
- **Type:** mixed  
- **Description:** Result of `$this->data->del($key)`.

##### Inner Mechanisms
Delegates to the underlying `data` object's `del` method.

##### Usage Example
```php
$dir = new directory();
$dir->del('old_page');
$dir->save();
```

---

#### `parse_placeholder`

##### Purpose
Analyzes placeholder entries in the directory structure and marks them as used or unused.

##### Parameters
None.

##### Return Values
None.

##### Inner Mechanisms
1. Iterates through all directory entries.
2. Tracks container depth using a stack.
3. For each container:
   - If it has a URL, sets the limit depth.
   - If it's a placeholder and beyond the limit, marks it as unused.
   - Otherwise, marks it as used.
4. Updates the `used` field accordingly.

##### Usage Example
```php
$dir = new directory();
$dir->parse_placeholder();
$dir->save();
```

---

#### `save`

##### Purpose
Persists directory changes to the data store and regenerates the filesystem structure.

##### Parameters
None.

##### Return Values
- **Type:** boolean  
- **Description:** `TRUE` on success, `FALSE` on failure.

##### Inner Mechanisms
1. Calls `parse_placeholder()` to update placeholder usage.
2. Saves the data store via `$this->data->save()`.
3. Calls `directory_create_filesystem()` to regenerate physical files.
4. Returns the combined result.

##### Usage Example
```php
$dir = new directory();
$dir->append('blog', 'Blog');
$dir->save();
```


<!-- HASH:357292a67f1137dd8d0e876dbae1699f -->
