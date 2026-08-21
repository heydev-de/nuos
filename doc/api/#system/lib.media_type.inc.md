# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.media_type.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.media_type.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Type Management (`lib.media_type.inc`)

This file provides a **media type management system** for the PWNC Web Platform. It handles:
- **Registration** of custom media types (MIME types, file extensions, and associated HTML embedding code).
- **Storage** of media type definitions in a structured data file (`#system/media.type`).
- **Dynamic HTML generation** for embedding media files (e.g., `<object>`, `<video>`, `<audio>`) with proper attributes.
- **Caching** of type-to-index mappings for performance.
- **Permission control** (operator-only modifications).

---

### **Global Function: `media_type_get_select()`**

#### **Purpose**
Generates an associative array of all registered media types for use in `<select>` form elements. Keys are human-readable names, and values are internal IDs.

#### **Parameters**
None.

#### **Return Values**
| Type | Description |
|------|-------------|
| `array` | Associative array: `["Display Name" => "internal_id", ...]`. Sorted naturally (case-insensitive). |

#### **Inner Mechanisms**
1. Loads the `#system/media.type` data file.
2. Iterates through all entries, extracting the `name` field.
3. Sorts the array naturally (`SORT_NATURAL | SORT_FLAG_CASE`).
4. Prepends a "None" option (`CMS_L_MEDIA_TYPE_001`).

#### **Usage Example**
```php
$mediaTypes = media_type_get_select();
echo '<select name="media_type">';
foreach ($mediaTypes as $name => $id) {
    echo "<option value=\"$id\">" . x($name) . "</option>";
}
echo '</select>';
```
**Context**: Used in admin forms to select a media type for uploads or embeds.

---

## **Class: `media_type`**

### **Properties**
| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Instance of the `#system/media.type` data file. |
| `$operator` | `bool` | `TRUE` if the current user has operator permissions. |
| `$type` | `array` | Cached mapping of file extensions/MIME types to media type IDs. |

---

### **Constructor: `__construct()`**

#### **Purpose**
Initializes the media type system, loads data, and ensures the media type directory exists.

#### **Parameters**
None.

#### **Return Values**
None (constructor).

#### **Inner Mechanisms**
1. Loads the `#system/media.type` data file into `$this->data`.
2. Checks operator permissions via `cms_permission()`.
3. Attempts to load cached type mappings (`media_type.type`). If missing, calls `update_type()`.
4. Creates the media type storage directory (`#media.type`) if it doesn’t exist.

---

### **Method: `add()`**

#### **Purpose**
Creates a new media type entry with a unique ID, name, type (extensions/MIME), and HTML embedding code.

#### **Parameters**
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$name` | `string` | - | Human-readable name (e.g., "MP4 Video"). |
| `$type` | `string` | `NULL` | Comma-separated file extensions/MIME types (e.g., "mp4,video/mp4"). |
| `$code` | `string` | `NULL` | HTML embedding code (e.g., `<video src="%src%">%alt%</video>`). |

#### **Return Values**
| Type | Description |
|------|-------------|
| `string` | The generated unique ID on success. |
| `bool` | `FALSE` if the user lacks permissions or the operation fails. |

#### **Inner Mechanisms**
1. Generates a unique ID via `unique_id()`.
2. Delegates to `set()` for storage.
3. Returns the ID if successful.

#### **Usage Example**
```php
$media = new media_type();
$id = $media->add(
    "MP4 Video",
    "mp4,video/mp4",
    '<video src="%src%" controls>%alt%</video>'
);
if ($id) {
    echo "Media type added with ID: $id";
}
```
**Context**: Used in admin panels to register new media types.

---

### **Method: `set()`**

#### **Purpose**
Updates an existing media type or creates a new one with the given ID.

#### **Parameters**
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Unique ID of the media type. |
| `$name` | `string` | - | Human-readable name. |
| `$type` | `string` | `NULL` | Comma-separated extensions/MIME types. |
| `$code` | `string` | `NULL` | HTML embedding code. |

#### **Return Values**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### **Inner Mechanisms**
1. Writes the HTML code to a file (`#media.type/[id].htm`).
2. Updates the data file with the name and type.
3. Calls `save()` to persist changes. Rolls back file creation on failure.

#### **Usage Example**
```php
$media = new media_type();
if ($media->set(
    "mp4_video",
    "MP4 Video (Updated)",
    "mp4,video/mp4,m4v",
    '<video src="%src%" controls width="%width%">%alt%</video>'
)) {
    echo "Media type updated!";
}
```

---

### **Method: `get_index()`**

#### **Purpose**
Retrieves the media type ID associated with a file extension or MIME type.

#### **Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$type` | `string` | File extension (e.g., "mp4") or MIME type (e.g., "video/mp4"). |

#### **Return Values**
| Type | Description |
|------|-------------|
| `string` | Media type ID if found. |
| `bool` | `FALSE` if no match exists. |

#### **Inner Mechanisms**
1. Converts `$type` to lowercase and trims whitespace.
2. Looks up the type in the cached `$this->type` array.

#### **Usage Example**
```php
$media = new media_type();
$index = $media->get_index("mp4");
if ($index) {
    echo "MP4 files use media type ID: $index";
}
```
**Context**: Used to determine how to embed a file based on its extension/MIME type.

---

### **Method: `get_code()`**

#### **Purpose**
Retrieves the HTML embedding code for a media type.

#### **Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media type ID. |

#### **Return Values**
| Type | Description |
|------|-------------|
| `string` | The HTML embedding code. |
| `bool` | `FALSE` if the file doesn’t exist. |

#### **Inner Mechanisms**
Reads the code from `#media.type/[id].htm`.

#### **Usage Example**
```php
$media = new media_type();
$code = $media->get_code("mp4_video");
if ($code) {
    echo "Embed code: " . x($code);
}
```

---

### **Method: `delete()`**

#### **Purpose**
Deletes one or more media types and their associated code files.

#### **Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string|array` | Single ID or array of IDs to delete. |

#### **Return Values**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### **Inner Mechanisms**
1. Deletes entries from the data file.
2. Deletes the associated `.htm` files.
3. Calls `save()` to persist changes.

#### **Usage Example**
```php
$media = new media_type();
if ($media->delete("old_media_type")) {
    echo "Media type deleted!";
}
```

---

### **Method: `parse()`**

#### **Purpose**
Generates HTML to embed a media file using the appropriate media type.

#### **Parameters**
| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Media type ID. |
| `$url` | `string` | - | URL of the media file. |
| `$id` | `string` | `NULL` | HTML `id` attribute. |
| `$type` | `string` | `NULL` | MIME type (auto-detected if `NULL`). |
| `$width` | `int` | `NULL` | Width attribute. |
| `$height` | `int` | `NULL` | Height attribute. |
| `$alt` | `string` | `NULL` | Alternative text. |
| `$title` | `string` | `NULL` | Title attribute. |
| `$class` | `string` | `NULL` | CSS class. |
| `$style` | `string` | `NULL` | Inline style. |

#### **Return Values**
| Type | Description |
|------|-------------|
| `string` | Generated HTML for embedding the media. |

#### **Inner Mechanisms**
1. Retrieves the embedding code for `$index`. Falls back to a generic `<object>` if the code is empty or the type is unknown.
2. Replaces placeholders (`%src%`, `%alt%`, etc.) with provided values.
3. Auto-detects MIME type if not provided.

#### **Usage Example**
```php
$media = new media_type();
$html = $media->parse(
    "mp4_video",
    "/media/sample.mp4",
    "video_player",
    NULL,
    640,
    360,
    "Sample Video",
    "Sample Video",
    "media-embed",
    "max-width: 100%;"
);
echo $html;
```
**Output**:
```html
<video id="video_player" src="/media/sample.mp4" controls width="640" height="360" class="media-embed" style="max-width: 100%;">Sample Video</video>
```

---

### **Method: `update_type()`**

#### **Purpose**
Rebuilds the cached mapping of file extensions/MIME types to media type IDs.

#### **Parameters**
None.

#### **Return Values**
None.

#### **Inner Mechanisms**
1. Clears the existing `$this->type` cache.
2. Iterates through all media types, splitting their `type` field by commas.
3. Maps each extension/MIME type to its media type ID.
4. Updates the cache via `cms_cache()`.

#### **Usage Example**
```php
$media = new media_type();
$media->update_type(); // Force cache refresh
```

---

### **Method: `save()`**

#### **Purpose**
Persists changes to the data file and updates the type cache.

#### **Parameters**
None.

#### **Return Values**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### **Inner Mechanisms**
1. Calls `update_type()` to refresh the cache.
2. Saves the data file via `$this->data->save()`.

#### **Usage Example**
```php
$media = new media_type();
$media->data->set(["name" => "New Type"], "new_id");
if ($media->save()) {
    echo "Changes saved!";
}
```


<!-- HASH:bd43b977433af8399872cdd2841176ea -->
