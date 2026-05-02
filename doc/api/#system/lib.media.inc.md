# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.media.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Media Management Module (`lib.media.inc`)

This file provides the core media management functionality for the NUOS platform, handling both internal (uploaded) and external (linked) media assets. It includes:

1. **Global utility functions** for retrieving media data in different formats
2. **`media` class** for CRUD operations on media assets, permission checks, and rendering

---

## Global Utility Functions

### `media_get_array()`

Returns a structured array of all media assets, grouped by category.

#### Parameters
None

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array with categories as top-level keys and media names/IDs as nested keys. Empty string (`""`) is used for uncategorized media. |

#### Inner Mechanisms
1. Loads media data using the `#system/media` dataset
2. Iterates through all media entries, organizing them by category
3. Handles duplicate names by appending incrementing numbers in parentheses
4. Sorts categories and media names alphabetically

#### Usage
```php
$media_list = media_get_array();
// Returns: ["Images" => ["Photo 1" => "media://abc123", ...], ...]
```

---

### `media_get_select()`

Returns a flat array of media categories for use in `<select>` elements.

#### Parameters
None

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array with categories as keys and values (empty string for uncategorized). |

#### Inner Mechanisms
1. Loads media data using the `#system/media` dataset
2. Extracts unique category values
3. Sorts categories alphabetically

#### Usage
```php
$category_options = media_get_select();
// Returns: ["" => "", "Images" => "Images", "Documents" => "Documents"]
```

---

## `media` Class

Core class for media asset management with permission-based operations.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the `#system/media` dataset handler |
| `operator` | `bool` | Permission flag for media operations |

---

### `__construct()`

Initializes the media handler.

#### Parameters
None

#### Return Values
None (constructor)

#### Inner Mechanisms
1. Initializes the `#system/media` dataset
2. Checks operator permissions using `CMS_MEDIA_PERMISSION_OPERATOR`
3. Ensures the media storage directory exists (`CMS_DATA_PATH . "media"`)

---

### `add()`

Uploads and registers a new media file.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary file path from upload |
| `$uploaded_filename` | `string` | Original filename from upload |
| `$name` | `string\|array\|NULL` | Display name (supports language arrays) |
| `$type` | `string\|NULL` | Media type identifier |
| `$category` | `string\|NULL` | Category name |
| `$filename` | `string\|NULL` | Custom filename (without extension) |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index (e.g., `"media://abc123"`) on success, `FALSE` on failure |

#### Inner Mechanisms
1. **Permission check**: Fails if user lacks operator permissions
2. **File validation**: Verifies the uploaded file exists
3. **Name resolution**:
   - Uses provided name or falls back to custom filename
   - Falls back to uploaded filename if no custom name provided
   - Generates a unique ID if no name is available
4. **Filename handling**:
   - Sanitizes filename using `stringtofilename()`
   - Generates unique filename if conflicts exist
5. **Storage**:
   - Moves file to `CMS_DATA_PATH . "media/"`
   - Creates dataset entry with metadata
   - Returns media index on success

#### Usage
```php
$media_index = $media->add(
    $_FILES['file']['tmp_name'],
    $_FILES['file']['name'],
    "Profile Picture",
    "image",
    "Avatars"
);
```

---

### `set()`

Updates media metadata or converts between internal/external media.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index (e.g., `"media://abc123"`) |
| `$name` | `string\|array` | New display name (supports language arrays) |
| `$type` | `string` | New media type |
| `$category` | `string` | New category |
| `$filename_or_url` | `string` | New filename (internal) or URL (external) |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index on success, `FALSE` on failure |

#### Inner Mechanisms
**For internal media:**
1. **Name resolution**: Falls back to existing name if none provided
2. **Filename handling**:
   - Preserves existing filename if none provided
   - Renames file if new filename provided
3. **Storage**: Updates dataset with new metadata

**For external media:**
1. **Name resolution**: Falls back to existing name if none provided
2. **URL handling**: Updates dataset with new URL

#### Usage
```php
// Update metadata
$media->set("media://abc123", "New Name", "video", "Videos", NULL);

// Convert to external media
$media->set("media://abc123", NULL, NULL, NULL, "https://example.com/video.mp4");
```

---

### `replace()`

Replaces the file content of an internal media asset.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index (e.g., `"media://abc123"`) |
| `$uploaded_file` | `string` | Temporary file path from upload |
| `$uploaded_filename` | `string` | Original filename from upload |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms
1. **Validation**: Checks permissions, media type, and file existence
2. **Extension handling**:
   - Preserves existing filename if extensions match
   - Generates new filename if extensions differ
3. **Storage**: Deletes old file and moves new file to storage

#### Usage
```php
$success = $media->replace(
    "media://abc123",
    $_FILES['file']['tmp_name'],
    $_FILES['file']['name']
);
```

---

### `link()`

Creates a new external media entry (URL-based).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | External media URL |
| `$name` | `string\|array\|NULL` | Display name (supports language arrays) |
| `$type` | `string\|NULL` | Media type identifier |
| `$category` | `string\|NULL` | Category name |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index on success, `FALSE` on failure |

#### Inner Mechanisms
1. **Name resolution**: Falls back to URL filename if no name provided
2. **Storage**: Creates dataset entry with URL and metadata

#### Usage
```php
$media_index = $media->link(
    "https://example.com/image.jpg",
    "Example Image",
    "image",
    "External"
);
```

---

### `unlink()`

Deletes a media asset.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index (e.g., `"media://abc123"`) |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms
1. **Validation**: Checks permissions and index validity
2. **File deletion**: Removes physical file if media is internal
3. **Dataset cleanup**: Deletes dataset entry

#### Usage
```php
$success = $media->unlink("media://abc123");
```

---

### `internal()`

Checks if a media asset is internal (uploaded) or external (linked).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index (e.g., `"media://abc123"`) |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if internal, `FALSE` if external |

#### Inner Mechanisms
1. Checks for `url` field in dataset (external if present)
2. Verifies file existence for internal media (fallback to legacy index-based paths)

#### Usage
```php
if ($media->internal("media://abc123")) {
    // Handle internal media
}
```

---

### `parse()`

Renders a media asset using its type-specific handler.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media index (e.g., `"media://abc123"`) |
| `$id` | `string\|NULL` | HTML `id` attribute |
| `$width` | `int\|string\|NULL` | Width (pixels or CSS value) |
| `$height` | `int\|string\|NULL` | Height (pixels or CSS value) |
| `$alt` | `string\|NULL` | Alternative text |
| `$title` | `string\|NULL` | Title attribute |
| `$class` | `string\|NULL` | CSS classes |
| `$style` | `string\|NULL` | Inline styles |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Rendered HTML on success, `FALSE` on failure |

#### Inner Mechanisms
1. **Dependency check**: Loads `media_type` handler
2. **URL resolution**: Converts media index to physical URL using `translate_url()`
3. **Type handling**: Delegates rendering to `media_type->parse()`

#### Usage
```php
echo $media->parse(
    "media://abc123",
    "hero_image",
    800,
    600,
    "Hero Image",
    "Welcome Banner"
);
```


<!-- HASH:a74358893fc5577161b56af31b287219 -->
