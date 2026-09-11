# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.media.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.media.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Class

The `media` class provides a comprehensive interface for managing media files within the PWNC Web Platform. It handles both internal (uploaded) and external (linked) media assets, supporting operations such as adding, replacing, linking, updating, deleting, and parsing media entries.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MEDIA_PERMISSION_OPERATOR` | `"operator"` | Permission level required to perform media management operations |

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `$data` | `data` | Data handler instance for the `#system/media` dataset |
| `$operator` | `bool` | Whether the current user has operator permissions for media management |

### Constructor

#### `__construct()`

Initializes the media manager by setting up the data handler and checking operator permissions.

**Parameters:** None

**Return Value:** None

**Inner Mechanisms:**
- Creates a new `data` instance pointing to the `#system/media` dataset
- Checks if the current user has `CMS_MEDIA_PERMISSION_OPERATOR` permissions
- Ensures the media storage directory exists via `mkpath()`

**Usage Example:**
```php
$media = new media();
if ($media->operator) {
    // User can manage media
}
```

### Methods

#### `add($uploaded_file, $uploaded_filename, $name = NULL, $type = NULL, $category = NULL, $filename = NULL)`

Adds a new internal media file (uploaded file) to the system.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$uploaded_file` | `string` | Temporary path of the uploaded file |
| `$uploaded_filename` | `string` | Original filename of the uploaded file |
| `$name` | `array\|NULL` | Multilingual name for the media entry |
| `$type` | `string\|NULL` | Media type identifier |
| `$category` | `string\|NULL` | Category for organizing media |
| `$filename` | `string\|NULL` | Custom filename for storage |

**Return Value:**
- `string` (media index) on success
- `FALSE` on failure (no permission, invalid file, or save error)

**Inner Mechanisms:**
1. Checks operator permissions and validates the uploaded file
2. Extracts file extension and base name from the uploaded filename
3. Resolves default names using language helpers (`language_get`/`language_set`)
4. Generates a unique filename in the media directory
5. Moves the uploaded file to the media storage path
6. Creates a new dataset entry with metadata and saves it

**Usage Example:**
```php
$media = new media();
$result = $media->add(
    $_FILES['file']['tmp_name'],
    $_FILES['file']['name'],
    ['en' => 'My Image', 'de' => 'Mein Bild'],
    'image',
    'photos'
);
if ($result !== FALSE) {
    echo "Media added with index: " . $result;
}
```

#### `set($index, $name, $type, $category, $filename_or_url)`

Updates an existing media entry, handling both internal files and external URLs.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | Media index identifier |
| `$name` | `array` | Updated multilingual name |
| `$type` | `string` | Updated media type |
| `$category` | `string` | Updated category |
| `$filename_or_url` | `string` | New filename (internal) or URL (external) |

**Return Value:**
- `string` (media index) on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Validates permissions and index
2. For internal media:
   - Preserves old filename if new one is empty
   - Generates unique filename with proper extension
   - Renames the physical file if needed
3. For external media:
   - Updates the URL directly
4. Updates the dataset and saves changes

**Usage Example:**
```php
$media = new media();
$media->set(
    'media://abc123',
    ['en' => 'Updated Name'],
    'image',
    'updated_category',
    'new_filename.jpg'
);
```

#### `replace($index, $uploaded_file, $uploaded_filename)`

Replaces the physical file of an internal media entry.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | Media index identifier |
| `$uploaded_file` | `string` | Temporary path of the replacement file |
| `$uploaded_filename` | `string` | Original filename of the replacement file |

**Return Value:**
- `TRUE` on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Validates permissions and ensures the media is internal
2. Compares file extensions between old and new files
3. If extensions match, directly replaces the file
4. If extensions differ, generates a new unique filename, moves the new file, updates the dataset, and deletes the old file

**Usage Example:**
```php
$media = new media();
$media->replace(
    'media://abc123',
    $_FILES['replacement']['tmp_name'],
    $_FILES['replacement']['name']
);
```

#### `link($url, $name = NULL, $type = NULL, $category = NULL)`

Creates a new external media entry pointing to a URL.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$url` | `string` | External URL for the media |
| `$name` | `array\|NULL` | Multilingual name for the media |
| `$type` | `string\|NULL` | Media type identifier |
| `$category` | `string\|NULL` | Category for organizing media |

**Return Value:**
- `string` (media index) on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Checks operator permissions
2. Sets default name from URL filename if none provided
3. Creates a new dataset entry with the URL and metadata
4. Saves and returns the new media index

**Usage Example:**
```php
$media = new media();
$index = $media->link(
    'https://example.com/image.jpg',
    ['en' => 'External Image'],
    'image',
    'external'
);
```

#### `unlink($index)`

Deletes a media entry and its associated physical file (if internal).

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | Media index identifier |

**Return Value:**
- `TRUE` on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Validates permissions and index
2. For internal media, deletes the physical file from storage
3. Removes the dataset entry
4. Saves changes

**Usage Example:**
```php
$media = new media();
$media->unlink('media://abc123');
```

#### `internal($index)`

Checks whether a media entry refers to an internal (uploaded) file.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | Media index identifier |

**Return Value:**
- `TRUE` if the media is internal and the file exists
- `FALSE` otherwise

**Inner Mechanisms:**
1. Returns `FALSE` if the entry has a URL (external media)
2. Retrieves the filename from the dataset
3. Checks if the physical file exists in the media directory

**Usage Example:**
```php
$media = new media();
if ($media->internal('media://abc123')) {
    echo "This is an internal file";
}
```

#### `parse($index, $id = NULL, $width = NULL, $height = NULL, $alt = NULL, $title = NULL, $class = NULL, $style = NULL)`

Parses a media entry into an HTML representation using the appropriate media type handler.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | Media index identifier |
| `$id` | `string\|NULL` | HTML element ID |
| `$width` | `int\|NULL` | Display width |
| `$height` | `int\|NULL` | Display height |
| `$alt` | `array\|NULL` | Multilingual alt text |
| `$title` | `array\|NULL` | Multilingual title text |
| `$class` | `string\|NULL` | CSS class for the element |
| `$style` | `string\|NULL` | Inline CSS styles |

**Return Value:**
- Parsed HTML output from the media type handler
- `FALSE` if the media type library cannot be loaded

**Inner Mechanisms:**
1. Loads the `media_type` library
2. Generates a unique ID if none provided
3. Instantiates a `media_type` handler
4. Delegates parsing to the handler with resolved URL and parameters

**Usage Example:**
```php
$media = new media();
$html = $media->parse(
    'media://abc123',
    NULL,
    300,
    200,
    ['en' => 'Alt text'],
    ['en' => 'Title'],
    'img-responsive',
    'border: 1px solid #ccc;'
);
echo $html;
```

### Related Functions

#### `media_get_array()`

Retrieves all media entries organized by category as an associative array.

**Parameters:** None

**Return Value:** `array` - Nested array with categories as keys and media names as sub-keys mapping to media indices

**Inner Mechanisms:**
1. Loads all entries from the `#system/media` dataset
2. Organizes entries by category
3. Handles duplicate names by appending sequence numbers
4. Sorts the result recursively using natural case-insensitive ordering

**Usage Example:**
```php
$media_list = media_get_array();
foreach ($media_list as $category => $items) {
    foreach ($items as $name => $index) {
        echo "$name: $index";
    }
}
```

#### `media_get_select()`

Retrieves all media categories for use in selection interfaces.

**Parameters:** None

**Return Value:** `array` - Associative array with category names as both keys and values, sorted naturally

**Inner Mechanisms:**
1. Loads all entries from the `#system/media` dataset
2. Extracts unique category names
3. Sorts categories using natural case-insensitive ordering

**Usage Example:**
```php
$categories = media_get_select();
foreach ($categories as $value => $label) {
    echo "<option value='$value'>$label</option>";
}
```


<!-- HASH:1212a41ffec4cfc5fec85acef7b3f0d6 -->
