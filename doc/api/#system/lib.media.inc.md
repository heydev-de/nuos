# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.media.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.media.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Management Module

The `lib.media.inc` file provides a comprehensive media management system for the PWNC Web Platform. It handles both internal (uploaded) and external (linked) media files, offering CRUD operations, categorization, and rendering capabilities.

This module is essential for:
- Uploading and managing media files
- Linking external media resources
- Organizing media into categories
- Rendering media with appropriate HTML output based on type

---

## Functions

### `media_get_array()`

Generates a nested associative array of all media items, grouped by category.

#### Parameters
None

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Nested associative array where keys are categories and values are arrays of media items (key: display name, value: media index) |

#### Inner Mechanisms
1. Initializes a `data` object for the `#system/media` dataset
2. Iterates through all media entries
3. Groups entries by their category
4. Handles naming collisions by appending incrementing numbers
5. Sorts the result naturally and case-insensitively

#### Usage Example
```php
$media_array = media_get_array();
foreach ($media_array as $category => $items) {
    echo "<h3>" . x($category) . "</h3>";
    foreach ($items as $name => $index) {
        echo "<div>" . x($name) . "</div>";
    }
}
```

---

### `media_get_select()`

Generates a flat associative array of media categories for use in select form elements.

#### Parameters
None

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array where keys and values are category names (empty string for uncategorized) |

#### Inner Mechanisms
1. Initializes a `data` object for the `#system/media` dataset
2. Iterates through all media entries
3. Collects unique category names
4. Sorts the result naturally and case-insensitively

#### Usage Example
```php
$category_select = media_get_select();
echo '<select name="media_category">';
foreach ($category_select as $value => $label) {
    echo '<option value="' . x($value) . '">' . x($label) . '</option>';
}
echo '</select>';
```

---

## `media` Class

The core media management class providing all CRUD operations and rendering capabilities.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the data handler for media storage |
| `operator` | `bool` | Permission flag for media operations |

### Constructor

```php
function __construct()
```

#### Inner Mechanisms
1. Initializes the data handler for the `#system/media` dataset
2. Checks operator permissions
3. Ensures the media storage directory exists

---

### `add()`

Uploads and registers a new media file.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path to the uploaded file |
| `$uploaded_filename` | `string` | Original filename of the uploaded file |
| `$name` | `string\|array\|NULL` | Display name (supports language arrays) |
| `$type` | `string\|NULL` | Media type identifier |
| `$category` | `string\|NULL` | Category name |
| `$filename` | `string\|NULL` | Desired filename (without extension) |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index on success, FALSE on failure |

#### Inner Mechanisms
1. Validates permissions and file existence
2. Determines appropriate display name and filename
3. Generates a unique storage filename
4. Moves the uploaded file to permanent storage
5. Creates a new dataset in the media storage
6. Returns the generated media index

#### Usage Example
```php
if (isset($_FILES['media_file'])) {
    $media = new media();
    $index = $media->add(
        $_FILES['media_file']['tmp_name'],
        $_FILES['media_file']['name'],
        "My Image",  // name
        "image",     // type
        "Photos",    // category
        NULL         // use default filename
    );
    if ($index) {
        echo "Media uploaded with index: " . x($index);
    }
}
```

---

### `set()`

Updates an existing media item's metadata or replaces its file.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index to update |
| `$name` | `string\|array` | New display name (supports language arrays) |
| `$type` | `string` | New media type identifier |
| `$category` | `string` | New category name |
| `$filename_or_url` | `string` | For internal media: new filename (without extension). For external media: new URL |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index on success, FALSE on failure |

#### Inner Mechanisms
**For internal media:**
1. Validates permissions and index
2. Determines appropriate display name
3. Handles filename changes (including file renaming)
4. Updates the dataset

**For external media:**
1. Validates permissions and index
2. Determines appropriate display name
3. Updates the URL in the dataset

#### Usage Example
```php
$media = new media();
$success = $media->set(
    "media://abc123",  // index
    "Updated Name",    // name
    "video",           // type
    "Videos",          // category
    "new_filename"     // new filename (without extension)
);
if ($success) {
    echo "Media updated successfully";
}
```

---

### `replace()`

Replaces the file content of an existing internal media item.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index to update |
| `$uploaded_file` | `string` | Temporary path to the new uploaded file |
| `$uploaded_filename` | `string` | Original filename of the new uploaded file |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | TRUE on success, FALSE on failure |

#### Inner Mechanisms
1. Validates permissions, index, and file existence
2. Checks if the media is internal
3. Compares file extensions
4. If extensions match: directly replaces the file
5. If extensions differ: generates new filename, moves file, updates dataset, and deletes old file

#### Usage Example
```php
if (isset($_FILES['new_media_file'])) {
    $media = new media();
    $success = $media->replace(
        "media://abc123",  // index
        $_FILES['new_media_file']['tmp_name'],
        $_FILES['new_media_file']['name']
    );
    if ($success) {
        echo "Media file replaced successfully";
    }
}
```

---

### `link()`

Creates a new external media link.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL of the external media |
| `$name` | `string\|array\|NULL` | Display name (supports language arrays) |
| `$type` | `string\|NULL` | Media type identifier |
| `$category` | `string\|NULL` | Category name |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index on success, FALSE on failure |

#### Inner Mechanisms
1. Validates permissions
2. Determines appropriate display name
3. Generates a new media index
4. Creates a dataset with the URL and metadata
5. Returns the generated media index

#### Usage Example
```php
$media = new media();
$index = $media->link(
    "https://example.com/video.mp4",
    "Example Video",
    "video",
    "External Videos"
);
if ($index) {
    echo "External media linked with index: " . x($index);
}
```

---

### `unlink()`

Removes a media item (both internal file and dataset).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index to remove |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | TRUE on success, FALSE on failure |

#### Inner Mechanisms
1. Validates permissions and index
2. For internal media: deletes the associated file
3. Deletes the dataset from storage
4. Returns success status

#### Usage Example
```php
$media = new media();
$success = $media->unlink("media://abc123");
if ($success) {
    echo "Media removed successfully";
}
```

---

### `internal()`

Checks if a media item is internal (uploaded) or external (linked).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index to check |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | TRUE if internal, FALSE if external |

#### Inner Mechanisms
1. Checks for the presence of a URL (indicates external)
2. For internal media: verifies the file exists on disk
3. Returns the appropriate boolean

#### Usage Example
```php
$media = new media();
if ($media->internal("media://abc123")) {
    echo "This is an internal media file";
} else {
    echo "This is an external media link";
}
```

---

### `parse()`

Generates HTML output for a media item based on its type.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index to render |
| `$id` | `string\|NULL` | HTML id attribute |
| `$width` | `int\|string\|NULL` | Width attribute |
| `$height` | `int\|string\|NULL` | Height attribute |
| `$alt` | `string\|NULL` | Alt text for images |
| `$title` | `string\|NULL` | Title attribute |
| `$class` | `string\|NULL` | CSS class |
| `$style` | `string\|NULL` | Inline CSS style |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | HTML output on success, FALSE on failure |

#### Inner Mechanisms
1. Loads the media_type module
2. Generates a unique ID if none provided
3. Delegates rendering to the appropriate media_type handler
4. Returns the generated HTML

#### Usage Example
```php
$media = new media();
$html = $media->parse(
    "media://abc123",
    "my_media",
    640,
    480,
    "Description of the media",
    "Media Title",
    "media-class",
    "border: 1px solid #ccc;"
);
if ($html) {
    echo $html;
}
```


<!-- HASH:1212a41ffec4cfc5fec85acef7b3f0d6 -->
