# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.directory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.directory.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Directory Management Module

The `lib.directory.inc` file provides utilities and a class for managing the PWNC Web Platform's directory structure. It handles canonical URL resolution, directory visualization, filesystem creation, and entry manipulation.

---

## Functions

### `directory_get_canonical`

**Purpose:**
Retrieves a canonical URL for a given directory index from a language-specific map.

**Parameters:**

| Name    | Type   | Default | Description                          |
|---------|--------|---------|--------------------------------------|
| `$index` | int    | `NULL`  | Directory index. If empty, defaults to `0`. |

**Return Values:**
- `string`: Canonical URL for the given index.

**Inner Mechanisms:**
1. Uses `CMS_LANGUAGE` to determine the language-specific map file.
2. Loads the map file (`#system/[language.]directory.canonical`).
3. Returns the value for the given index.

**Usage Context:**
Used to resolve logical directory indices into canonical URLs, typically for navigation or redirection.

**Example:**
```php
$canonical_url = directory_get_canonical(5); // Returns the canonical URL for directory index 5
```

---

### `directory_flexview_display_function`

**Purpose:**
Renders a directory entry in a `flexview` UI component, handling icons, styling, and visibility based on entry properties.

**Parameters:**

| Name        | Type       | Default | Description                                                                 |
|-------------|------------|---------|-----------------------------------------------------------------------------|
| `$flexview` | `flexview` | -       | The `flexview` instance rendering the directory.                            |
| `$index`    | string     | -       | The directory index being rendered.                                        |
| `$open`     | bool       | -       | Whether the entry is expanded in the UI.                                   |

**Return Values:**
- `void`: Outputs HTML directly via `echo`.

**Inner Mechanisms:**
1. Determines if the entry is the currently active one (`$flag`).
2. Applies styling based on entry properties (`hidden`, `placeholder`, `used`).
3. Resolves icons based on entry type (`#subtype`) or fallback to default icons.
4. Constructs a display string by replacing placeholders (`%key%`) in the `flexview`'s template.

**Usage Context:**
Used as a callback for `flexview` to render directory entries in a hierarchical UI.

**Example:**
```php
$flexview = new flexview();
$flexview->set_display_function("cms\\directory_flexview_display_function");
$flexview->render(); // Renders the directory with custom display logic
```

---

### `directory_get_select`

**Purpose:**
Generates an associative array of directory entries for use in `<select>` form elements, with indentation for hierarchical display.

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array where keys are display names (indented for hierarchy) and values are directory indices.

**Inner Mechanisms:**
1. Loads directory data from `#system/directory`.
2. Iterates through entries, applying indentation for nested containers.
3. Skips entries with empty names, replacing them with `CMS_L_UNKNOWN`.

**Usage Context:**
Used to populate dropdown menus for directory selection.

**Example:**
```php
$options = directory_get_select();
echo "<select>";
foreach ($options as $name => $index) {
    echo "<option value='$index'>$name</option>";
}
echo "</select>";
```

---

### `directory_get_type`

**Purpose:**
Retrieves URLs for directory entry type icons (both default and expanded states).

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array where keys are type identifiers (e.g., `type` or `+type`) and values are icon URLs.

**Inner Mechanisms:**
1. Loads type data from `#system/directory.type`.
2. Constructs URLs for icons using `CMS_DATA_URL`.

**Usage Context:**
Used to fetch icons for directory entries based on their type.

**Example:**
```php
$icons = directory_get_type();
echo "<img src='" . $icons["article"] . "' alt='Article Icon'>";
```

---

### `directory_get_type_select`

**Purpose:**
Generates an associative array of directory entry types for use in `<select>` form elements, with icons.

**Parameters:**
- None.

**Return Values:**
- `array`: Associative array where keys are display names (with optional icon paths) and values are type identifiers.

**Inner Mechanisms:**
1. Loads type data from `#system/directory.type`.
2. Resolves duplicate names by appending incrementing numbers.
3. Appends icon paths to display names for visual feedback.

**Usage Context:**
Used to populate dropdown menus for selecting directory entry types.

**Example:**
```php
$types = directory_get_type_select();
echo "<select>";
foreach ($types as $name => $type) {
    echo "<option value='$type'>$name</option>";
}
echo "</select>";
```

---

### `directory_get_visible`

**Purpose:**
Finds the nearest visible ancestor of a directory entry, accounting for hidden containers.

**Parameters:**

| Name    | Type   | Default | Description                          |
|---------|--------|---------|--------------------------------------|
| `$index` | string | `NULL`  | Directory index. If `NULL`, uses `CMS_CONTENT_DIRECTORY_INDEX`. |

**Return Values:**
- `string`: Index of the nearest visible ancestor, or `0` if none exists.

**Inner Mechanisms:**
1. Loads directory data and checks if the entry exists.
2. Traverses the directory hierarchy, tracking hidden states.
3. Returns the last visible ancestor before encountering the target index.

**Usage Context:**
Used to resolve the correct entry to display when the requested entry is hidden.

**Example:**
```php
$visible_index = directory_get_visible("hidden_entry"); // Returns the nearest visible ancestor
```

---

### `directory_value`

**Purpose:**
Resolves a directory index into its canonical URL, optionally appending query parameters.

**Parameters:**

| Name    | Type   | Default | Description                          |
|---------|--------|---------|--------------------------------------|
| `$index` | string | -       | Directory index.                     |

**Return Values:**
- `string`: Canonical URL with optional query parameters.

**Inner Mechanisms:**
1. Loads the language-specific canonical map.
2. Appends query parameters from `cms_param()` if present.

**Usage Context:**
Used to generate URLs for directory entries, e.g., in navigation menus.

**Example:**
```php
$url = directory_value("about"); // Returns "https://example.com/about"
```

---

### `directory_get_flexview`

**Purpose:**
Creates a `flexview` instance for rendering the directory hierarchy, optionally filtering hidden or unused entries.

**Parameters:**

| Name              | Type | Default | Description                          |
|-------------------|------|---------|--------------------------------------|
| `$remove_hidden`  | bool | `TRUE`  | Whether to exclude hidden entries.   |

**Return Values:**
- `flexview|bool`: A configured `flexview` instance, or `FALSE` if `flexview` is not loaded.

**Inner Mechanisms:**
1. Loads the `flexview` library.
2. Filters entries based on `hidden`, `placeholder`, and `used` properties.
3. Configures the `flexview` to use `directory_value` for URL resolution.

**Usage Context:**
Used to render the directory in a hierarchical UI.

**Example:**
```php
$flexview = directory_get_flexview();
if ($flexview) {
    $flexview->render();
}
```

---

### `directory_create_filesystem`

**Purpose:**
Generates the physical filesystem structure for the directory, including PHP files, directories, and auxiliary files.

**Parameters:**

| Name        | Type   | Default               | Description                          |
|-------------|--------|-----------------------|--------------------------------------|
| `$language` | string | `CMS_LANGUAGE_ENABLED` | Comma-separated list of languages.   |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Clears the stat cache and reverts any logged filesystem changes.
2. Initializes paths and maps for each language.
3. Processes directory entries, creating files and directories as needed.
4. Handles dereferencing of `directory://` URLs.
5. Saves path and canonical maps for each language.
6. Logs all filesystem changes to `#system/directory.log`.

**Usage Context:**
Used during deployment or when the directory structure changes.

**Example:**
```php
if (directory_create_filesystem()) {
    echo "Filesystem created successfully.";
}
```

---

### `directory_remove_filesystem`

**Purpose:**
Removes all files and directories created by `directory_create_filesystem` using the log file.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Migrates legacy log files if present.
2. Locks the log file to prevent concurrent execution.
3. Reads the log file and deletes all listed files and directories in reverse order.

**Usage Context:**
Used to clean up the filesystem before regeneration.

**Example:**
```php
if (directory_remove_filesystem()) {
    echo "Filesystem removed successfully.";
}
```

---

## Class: `directory`

**Purpose:**
Provides an object-oriented interface for managing directory entries, including creation, modification, and deletion.

### Properties

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| `data` | `data` | Instance of the `data` class for directory storage. |

---

### Constructor

**Purpose:**
Initializes the `directory` instance with the directory data.

**Example:**
```php
$directory = new directory();
```

---

### `append`

**Purpose:**
Appends a new directory entry after the specified key.

**Parameters:**

| Name             | Type    | Default | Description                          |
|------------------|---------|---------|--------------------------------------|
| `$key`           | string  | -       | Key after which to append the entry. |
| `$name`          | string  | -       | Entry name.                          |
| `$description`   | string  | `""`    | Entry description.                   |
| `$url`           | string  | `""`    | Entry URL.                           |
| `$subtype`       | string  | `""`    | Entry subtype.                       |
| `$hidden`        | bool    | `FALSE` | Whether the entry is hidden.         |
| `$placeholder`   | bool    | `FALSE` | Whether the entry is a placeholder.  |
| `$dynamic`       | bool    | `TRUE`  | Whether the entry is dynamic.        |
| `$image_button`  | string  | `""`    | Button image path.                   |
| `$image_hover`   | string  | `""`    | Hover image path.                    |
| `$image_active`  | string  | `""`    | Active image path.                   |
| `$path`          | string  | `""`    | Custom path override.                |
| `$canonical`     | string  | `""`    | Custom canonical URL.                |

**Return Values:**
- `string|bool`: The key of the new entry, or `FALSE` on failure.

**Example:**
```php
$directory->append("parent", "About", "About our company", "content://about");
```

---

### `insert`

**Purpose:**
Inserts a new directory entry before the specified key.

**Parameters:**
- Same as `append`.

**Return Values:**
- `string|bool`: The key of the new entry, or `FALSE` on failure.

**Example:**
```php
$directory->insert("sibling", "Team", "Our team members", "content://team");
```

---

### `set`

**Purpose:**
Updates properties of an existing directory entry.

**Parameters:**

| Name             | Type    | Default | Description                          |
|------------------|---------|---------|--------------------------------------|
| `$key`           | string  | -       | Entry key to update.                 |
| `$name`          | string  | `NULL`  | New name.                            |
| `$description`   | string  | `NULL`  | New description.                     |
| `$url`           | string  | `NULL`  | New URL.                             |
| `$subtype`       | string  | `NULL`  | New subtype.                         |
| `$hidden`        | bool    | `NULL`  | New hidden state.                    |
| `$placeholder`   | bool    | `NULL`  | New placeholder state.               |
| `$dynamic`       | bool    | `NULL`  | New dynamic state.                   |
| `$image_button`  | string  | `NULL`  | New button image path.               |
| `$image_hover`   | string  | `NULL`  | New hover image path.                |
| `$image_active`  | string  | `NULL`  | New active image path.               |
| `$path`          | string  | `NULL`  | New path override.                   |
| `$canonical`     | string  | `NULL`  | New canonical URL.                   |

**Return Values:**
- `string|bool`: The entry key, or `FALSE` on failure.

**Example:**
```php
$directory->set("about", $name: "About Us", $hidden: TRUE);
```

---

### `del`

**Purpose:**
Deletes a directory entry and its children.

**Parameters:**

| Name   | Type   | Default | Description                          |
|--------|--------|---------|--------------------------------------|
| `$key` | string | -       | Entry key to delete.                 |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Example:**
```php
$directory->del("obsolete_entry");
```

---

### `parse_placeholder`

**Purpose:**
Marks placeholder entries as "used" if they contain non-placeholder children, otherwise marks them as unused.

**Parameters:**
- None.

**Return Values:**
- `void`.

**Inner Mechanisms:**
1. Traverses the directory hierarchy.
2. Tracks the depth of non-placeholder entries (`$limit`).
3. Marks placeholders as "used" if they contain non-placeholder children.

**Usage Context:**
Called internally before saving the directory.

**Example:**
```php
$directory->parse_placeholder();
```

---

### `save`

**Purpose:**
Saves the directory data and regenerates the filesystem.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Calls `parse_placeholder` to update placeholder states.
2. Saves the directory data and regenerates the filesystem.

**Example:**
```php
if ($directory->save()) {
    echo "Directory saved successfully.";
}
```


<!-- HASH:b55c8f5b89244e4d5e9761632cc505fc -->
