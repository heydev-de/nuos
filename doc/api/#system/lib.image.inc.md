# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.image.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## image

The `image` class provides a comprehensive interface for managing images within the PWNC Web Platform. It supports uploading, linking, replacing, and deleting images, with support for both internal (file-based) and external (URL-based) images. Images are stored in the `#system/image` data store and physically saved in the `CMS_DATA_PATH/image/` directory.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_IMAGE_PERMISSION_OPERATOR` | `"operator"` | Permission level required to perform image operations |

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Data handler instance for the `#system/image` store |
| `$operator` | `bool` | Whether the current user has operator permissions |

### Constructor

#### `__construct()`

Initializes the image manager by setting up the data handler, checking operator permissions, and ensuring the image storage directory exists.

**Parameters:** None

**Return Value:** None

**Inner Mechanisms:**
- Creates a `data` instance pointing to `#system/image`
- Checks if the current user has `CMS_IMAGE_PERMISSION_OPERATOR` permission
- Ensures the physical image directory (`CMS_DATA_PATH/image/`) exists via `mkpath()`

**Usage Example:**
```php
$img = new image();
// Now ready to manage images
```

### Methods

#### `add($uploaded_file, $uploaded_filename, $name = NULL, $category = NULL, $filename = NULL)`

Uploads and registers a new internal image.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path of the uploaded file (from `$_FILES`) |
| `$uploaded_filename` | `string` | Original filename of the uploaded file |
| `$name` | `array\|NULL` | Multilingual name for the image |
| `$category` | `string\|NULL` | Category to assign to the image |
| `$filename` | `string\|NULL` | Custom filename for storage |

**Return Value:**
- `string` (image index) on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permission and file existence
2. Validates file extension (gif, jpg, png, svg, webp)
3. Resolves default name using language functions
4. Generates a unique filename and moves the uploaded file
5. Stores metadata in the data handler and saves

**Usage Example:**
```php
$img = new image();
$result = $img->add(
    $_FILES['image']['tmp_name'],
    $_FILES['image']['name'],
    ['en' => 'My Image'],
    'gallery'
);
if ($result !== FALSE) {
    echo "Image added with index: " . $result;
}
```

#### `set($index, $name, $category, $filename_or_url)`

Updates an existing image's metadata. Handles both internal (file rename) and external (URL) images.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index to update |
| `$name` | `array` | New multilingual name |
| `$category` | `string` | New category |
| `$filename_or_url` | `string` | New filename (internal) or URL (external) |

**Return Value:**
- `string` (image index) on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permission and index validity
2. For internal images: renames the physical file if needed
3. For external images: stores the URL directly
4. Updates metadata and saves

**Usage Example:**
```php
$img = new image();
$img->set('image://abc123', ['en' => 'Updated Name'], 'new_category', 'new_filename.jpg');
```

#### `replace($index, $uploaded_file, $uploaded_filename)`

Replaces the physical file of an internal image.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index to replace |
| `$uploaded_file` | `string` | Temporary path of the new uploaded file |
| `$uploaded_filename` | `string` | Original filename of the new upload |

**Return Value:**
- `TRUE` on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permission and internal image status
2. Validates new file extension
3. If extension matches: moves file directly
4. If extension differs: generates new filename, moves file, deletes old file
5. Updates metadata and saves

**Usage Example:**
```php
$img = new image();
$img->replace('image://abc123', $_FILES['new_image']['tmp_name'], $_FILES['new_image']['name']);
```

#### `link($url, $name = NULL, $category = NULL)`

Creates a reference to an external image via URL.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL of the external image |
| `$name` | `array\|NULL` | Multilingual name for the image |
| `$category` | `string\|NULL` | Category to assign |

**Return Value:**
- `string` (image index) on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permission
2. Resolves default name from URL if not provided
3. Generates a unique index
4. Stores metadata with URL reference and saves

**Usage Example:**
```php
$img = new image();
$index = $img->link('https://example.com/image.jpg', ['en' => 'External Image'], 'links');
```

#### `unlink($index)`

Deletes an image and its associated file (if internal).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index to delete |

**Return Value:**
- `TRUE` on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permission and index validity
2. For internal images: deletes the physical file
3. Removes the dataset entry
4. Saves changes

**Usage Example:**
```php
$img = new image();
$img->unlink('image://abc123');
```

#### `internal($index)`

Checks whether an image is stored internally (as a file) rather than externally (as a URL).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index to check |

**Return Value:**
- `TRUE` if the image is internal and its file exists
- `FALSE` otherwise

**Inner Mechanisms:**
1. Checks if the image has a URL (external) — returns `FALSE`
2. Retrieves the filename
3. Checks if the physical file exists in the image directory

**Usage Example:**
```php
$img = new image();
if ($img->internal('image://abc123')) {
    echo "This is an internal image file.";
}
```

## Helper Functions

### `image_get_array()`

Retrieves all images organized by category as an associative array.

**Parameters:** None

**Return Value:** `array` — Images grouped by category with names as keys and indices as values

**Inner Mechanisms:**
1. Loads all entries from `#system/image`
2. Groups by category
3. Handles duplicate names by appending "(1)", "(2)", etc.
4. Sorts recursively using natural case-insensitive order

**Usage Example:**
```php
$images = image_get_array();
foreach ($images as $category => $imageList) {
    foreach ($imageList as $name => $index) {
        echo "$name: $index";
    }
}
```

### `image_get_select()`

Retrieves all image categories for use in selection interfaces.

**Parameters:** None

**Return Value:** `array` — Categories with category names as both keys and values

**Inner Mechanisms:**
1. Loads all entries from `#system/image`
2. Extracts unique categories
3. Sorts using natural case-insensitive order

**Usage Example:**
```php
$categories = image_get_select();
// Use in a dropdown:
foreach ($categories as $value => $label) {
    echo "<option value='$value'>$label</option>";
}
```


<!-- HASH:0be857726ca94d5eacad731ab44aa0e0 -->
