# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.media_type.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.media_type.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Type Management System

The `media_type` class and its associated functions provide a flexible system for managing and rendering different media types within the PWNC Web Platform. This system allows administrators to define custom media types with associated HTML rendering templates, enabling consistent presentation of various media formats (images, videos, audio, documents, etc.) across the platform.

The system stores media type definitions in a data structure (`#system/media.type`) and their corresponding HTML templates in individual files within the `#media.type` directory. Each media type can have a custom HTML template that defines how content of that type should be rendered, with support for placeholder substitution for dynamic values.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MEDIA_TYPE_PERMISSION_OPERATOR` | `"operator"` | Permission level required to manage media types |

## Functions

### media_type_get_select()

Generates a sorted array of media types suitable for use in dropdown selection interfaces.

**Return Value:**
- **Type:** `array`
- **Description:** An associative array where keys are media type names and values are their corresponding indices. The array is sorted naturally and case-insensitively.

**Inner Mechanisms:**
1. Loads all media type definitions from the `#system/media.type` data store
2. Creates an array mapping media type names to their indices
3. Sorts the array using natural order sorting with case insensitivity
4. Returns the sorted array for use in UI components

**Usage Example:**
```php
// Generate options for a media type selection dropdown
$options = media_type_get_select();
foreach ($options as $name => $index) {
    echo "<option value='{$index}'>{$name}</option>";
}
```

## Class: media_type

The main class for managing media type definitions and rendering media content.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `$data` | `data` | Data handler instance for `#system/media.type` storage |
| `$operator` | `bool` | Whether the current user has operator permissions |
| `$type` | `array\|NULL` | Cached mapping of file extensions to media type indices |

### __construct()

Initializes the media type manager by setting up data storage, checking permissions, and loading cached type mappings.

**Inner Mechanisms:**
1. Creates a data handler for the `#system/media.type` storage
2. Checks if the current user has operator permissions
3. Attempts to load cached type mappings from the cache
4. If no cache exists, calls `update_type()` to build the mapping
5. Ensures the `#media.type` directory exists for storing HTML templates

**Usage Example:**
```php
$mediaType = new media_type();
// The object is now ready to manage media types
```

### add($name, $type = NULL, $code = NULL)

Creates a new media type with a unique index.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Display name for the media type |
| `$type` | `string\|NULL` | File extensions associated with this type (comma-separated) |
| `$code` | `string\|NULL` | HTML template code for rendering this media type |

**Return Value:**
- **Type:** `string\|bool`
- **Description:** Returns the unique index of the newly created media type on success, or `FALSE` if the operation fails (due to insufficient permissions or save failure).

**Inner Mechanisms:**
1. Checks operator permissions
2. Generates a unique ID for the new media type
3. Delegates to `set()` method to store the media type
4. Returns the index on success or `FALSE` on failure

**Usage Example:**
```php
$mediaType = new media_type();
$index = $mediaType->add(
    "Video Player",
    "mp4,mov,avi",
    "<video src=\"%src%\" controls>[ width=\"%width%\"]>[ height=\"%height%\"]></video>"
);
if ($index !== FALSE) {
    echo "Created media type with index: {$index}";
}
```

### set($index, $name, $type = NULL, $code = NULL)

Sets or updates a media type definition with the specified index.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | Unique identifier for the media type |
| `$name` | `string` | Display name for the media type |
| `$type` | `string\|NULL` | File extensions associated with this type (comma-separated) |
| `$code` | `string\|NULL` | HTML template code for rendering this media type |

**Return Value:**
- **Type:** `bool`
- **Description:** `TRUE` on success, `FALSE` on failure (insufficient permissions, file write failure, or save failure).

**Inner Mechanisms:**
1. Verifies operator permissions
2. Writes the HTML template code to a file named `{index}.htm` in the `#media.type` directory
3. Stores the media type metadata (name and type) in the data store
4. Attempts to save the data store
5. If save fails, removes the written template file and returns `FALSE`

**Usage Example:**
```php
$mediaType = new media_type();
$success = $mediaType->set(
    "custom_video",
    "Custom Video Type",
    "mp4,webm",
    "<div class='video-wrapper'><video src='%src%' controls></video></div>"
);
if ($success) {
    echo "Media type updated successfully";
}
```

### get_index($type)

Retrieves the media type index for a given file extension or MIME type.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$type` | `string` | File extension or MIME type to look up |

**Return Value:**
- **Type:** `string\|bool`
- **Description:** The media type index if found, or `FALSE` if not found.

**Inner Mechanisms:**
1. Converts the input type to lowercase for case-insensitive matching
2. Looks up the type in the cached `$type` mapping
3. Returns the corresponding index or `FALSE` if not found

**Usage Example:**
```php
$mediaType = new media_type();
$index = $mediaType->get_index("mp4");
if ($index !== FALSE) {
    echo "MP4 files use media type index: {$index}";
}
```

### get_code($index)

Retrieves the HTML template code for a specific media type.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | The media type index |

**Return Value:**
- **Type:** `string`
- **Description:** The HTML template code for the specified media type.

**Inner Mechanisms:**
1. Constructs the file path for the media type's HTML template
2. Reads and returns the file contents

**Usage Example:**
```php
$mediaType = new media_type();
$template = $mediaType->get_code("custom_video");
echo "Template: " . $template;
```

### delete($index)

Removes one or more media types and their associated template files.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string\|array` | Single media type index or array of indices to delete |

**Return Value:**
- **Type:** `bool`
- **Description:** `TRUE` on success, `FALSE` on failure (insufficient permissions or save failure).

**Inner Mechanisms:**
1. Checks operator permissions
2. Normalizes the input to an array if a single index is provided
3. Removes all specified entries from the data store
4. Saves the data store
5. If save succeeds, deletes the corresponding HTML template files
6. Returns `TRUE` on success or `FALSE` on failure

**Usage Example:**
```php
$mediaType = new media_type();
$success = $mediaType->delete(["old_type1", "old_type2"]);
if ($success) {
    echo "Media types deleted successfully";
}
```

### parse($index, $url, $id = NULL, $type = NULL, $width = NULL, $height = NULL, $alt = NULL, $title = NULL, $class = NULL, $style = NULL)

Renders media content using the appropriate HTML template with placeholder substitution.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `$index` | `string` | Media type index |
| `$url` | `string` | URL of the media content |
| `$id` | `string\|NULL` | HTML element ID |
| `$type` | `string\|NULL` | MIME type of the content |
| `$width` | `int\|string\|NULL` | Width of the media element |
| `$height` | `int\|string\|NULL` | Height of the media element |
| `$alt` | `string\|NULL` | Alternative text |
| `$title` | `string\|NULL` | Title attribute |
| `$class` | `string\|NULL` | CSS class |
| `$style` | `string\|NULL` | Inline CSS styles |

**Return Value:**
- **Type:** `string`
- **Description:** The rendered HTML with all placeholders replaced by actual values.

**Inner Mechanisms:**
1. Retrieves the HTML template code for the specified media type
2. If no template exists, attempts fallback type identification based on file extension
3. If fallback also fails, uses a default `<object>` tag template
4. Determines the MIME type if not provided
5. Creates a replacement array with all provided parameters
6. Performs placeholder substitution in the template
7. Returns the rendered HTML

**Usage Example:**
```php
$mediaType = new media_type();
$html = $mediaType->parse(
    "custom_video",
    "https://example.com/video.mp4",
    "my-video",
    "video/mp4",
    640,
    480,
    "My Video",
    "Video Title",
    "video-player",
    "border: 1px solid #ccc;"
);
echo $html;
// Outputs: <div class='video-wrapper'><video src='https://example.com/video.mp4' controls id="my-video" width="640" height="480" title="Video Title" class="video-player" style="border: 1px solid #ccc;"></video></div>
```

### update_type()

Rebuilds the internal type mapping cache from the data store.

**Inner Mechanisms:**
1. Clears the existing type mapping
2. Sorts the data store by name
3. Iterates through all media type definitions
4. For each type, extracts associated file extensions
5. Normalizes extensions (trimming whitespace, converting to lowercase)
6. Maps each extension to its media type index
7. Stores the updated mapping in the cache

**Usage Example:**
```php
$mediaType = new media_type();
// After adding new media types, update the cache
$mediaType->update_type();
```

### save()

Persists the current media type definitions to permanent storage.

**Return Value:**
- **Type:** `bool`
- **Description:** `TRUE` on success, `FALSE` on failure (insufficient permissions).

**Inner Mechanisms:**
1. Checks operator permissions
2. Updates the type mapping cache by calling `update_type()`
3. Saves the data store to permanent storage
4. Returns the result of the save operation

**Usage Example:**
```php
$mediaType = new media_type();
$mediaType->add("New Type", "ext", "<p>%src%</p>");
$success = $mediaType->save();
if ($success) {
    echo "Changes saved successfully";
}
```


<!-- HASH:bd43b977433af8399872cdd2841176ea -->
