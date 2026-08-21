# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.image.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Image Management Module (`lib.image.inc`)

The `lib.image.inc` file provides a comprehensive image management system for the PWNC Web Platform. It includes:
- A **global `image` class** for handling image operations (upload, link, replace, delete, and metadata management).
- **Utility functions** (`image_get_array`, `image_get_select`) for retrieving structured image data for UI components (e.g., dropdowns, lists).
- **Permission-based access control** to restrict operations to authorized users (operators).

The module supports both **internal images** (uploaded and stored locally) and **external images** (linked via URL). It ensures unique filenames, handles multilingual naming, and maintains a structured data store for image metadata.

---

### Utility Functions

#### `image_get_array()`
Returns a nested associative array of all images, grouped by category, for use in UI components like dropdowns or galleries.

**Parameters:**
None.

**Return Values:**
| Type | Description |
|------|-------------|
| `array` | Nested associative array with categories as top-level keys and image names as second-level keys. Values are image identifiers (`image://...`). |

**Inner Mechanisms:**
1. Loads image metadata from the `#system/image` data store.
2. Iterates through all images, extracting their `category` and `name` (falling back to the image key if no name exists).
3. Handles duplicate names by appending `(1)`, `(2)`, etc.
4. Sorts the array recursively using natural case-insensitive order.

**Usage Example:**
```php
$images = image_get_array();
foreach ($images as $category => $imageList) {
    echo "<optgroup label='" . x($category) . "'>";
    foreach ($imageList as $name => $id) {
        echo "<option value='" . x($id) . "'>" . x($name) . "</option>";
    }
    echo "</optgroup>";
}
```
*Generates a `<select>` dropdown with images grouped by category.*

---

#### `image_get_select()`
Returns a flat associative array of unique image categories for use in UI filters or category selectors.

**Parameters:**
None.

**Return Values:**
| Type | Description |
|------|-------------|
| `array` | Associative array with categories as keys and values (e.g., `["Logos" => "Logos"]`). |

**Inner Mechanisms:**
1. Loads image metadata from the `#system/image` data store.
2. Extracts unique categories from all images.
3. Sorts categories using natural case-insensitive order.

**Usage Example:**
```php
$categories = image_get_select();
echo "<select name='category'>";
foreach ($categories as $category) {
    echo "<option value='" . x($category) . "'>" . x($category) . "</option>";
}
echo "</select>";
```
*Generates a dropdown to filter images by category.*

---

### `image` Class

#### Properties
| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the `#system/image` data store for image metadata. |
| `operator` | `bool` | `TRUE` if the current user has `CMS_IMAGE_PERMISSION_OPERATOR` permission. |

#### Constructor: `__construct()`
Initializes the image manager, checks operator permissions, and ensures the image storage directory exists.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Initializes the `#system/image` data store.
2. Checks operator permissions using `cms_permission()`.
3. Creates the image storage directory (`CMS_DATA_PATH . "image"`) if it doesn’t exist.

---

#### `add($uploaded_file, $uploaded_filename, $name = NULL, $category = NULL, $filename = NULL)`
Uploads a new image file to the server and stores its metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path to the uploaded file (e.g., `$_FILES['file']['tmp_name']`). |
| `$uploaded_filename` | `string` | Original filename of the uploaded file (e.g., `$_FILES['file']['name']`). |
| `$name` | `string\|array\|NULL` | Multilingual name for the image (e.g., `["en" => "Logo", "de" => "Logo"]`). If `NULL`, falls back to `$filename` or `$uploaded_filename`. |
| `$category` | `string\|NULL` | Category for the image (e.g., "Logos"). |
| `$filename` | `string\|NULL` | Desired filename (without extension). If `NULL`, falls back to `$name` or `$uploaded_filename`. |

**Return Values:**
| Type | Description |
|------|-------------|
| `string\|FALSE` | Image identifier (`image://...`) on success, `FALSE` on failure. |

**Inner Mechanisms:**
1. Validates operator permissions and file existence.
2. Extracts the file extension and validates it against supported formats (`gif`, `jpg`, `png`, `svg`, `webp`).
3. Generates a default name if none is provided (falling back to `$filename` or `$uploaded_filename`).
4. Generates a unique filename in the format `stringtofilename($name).$extension`.
5. Moves the uploaded file to the image storage directory.
6. Stores metadata (name, category, filename) in the data store and returns the image identifier.

**Usage Example:**
```php
if (isset($_FILES['file'])) {
    $image = new image();
    $id = $image->add(
        $_FILES['file']['tmp_name'],
        $_FILES['file']['name'],
        ["en" => "Company Logo", "de" => "Firmenlogo"],
        "Logos"
    );
    if ($id) {
        echo "Image uploaded with ID: " . x($id);
    } else {
        echo "Upload failed.";
    }
}
```
*Handles a file upload form and stores the image with multilingual metadata.*

---

#### `set($index, $name, $category, $filename_or_url)`
Updates the metadata of an existing image (internal or external).

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image identifier (`image://...`). |
| `$name` | `string\|array` | New multilingual name for the image. |
| `$category` | `string` | New category for the image. |
| `$filename_or_url` | `string` | For **internal images**: New filename (without extension). For **external images**: URL. |

**Return Values:**
| Type | Description |
|------|-------------|
| `string\|FALSE` | Image identifier (`image://...`) on success, `FALSE` on failure. |

**Inner Mechanisms:**
1. Validates operator permissions and the image identifier.
2. **For internal images:**
   - Generates a default name if none is provided (falling back to the existing name).
   - Updates the filename if provided, ensuring uniqueness and renaming the physical file if necessary.
   - Updates metadata (name, category, filename) in the data store.
3. **For external images:**
   - Updates metadata (name, category, URL) in the data store.

**Usage Example:**
```php
$image = new image();
$id = "image://abc123";
$result = $image->set(
    $id,
    ["en" => "Updated Logo", "de" => "Aktualisiertes Logo"],
    "Branding",
    "new_logo"
);
if ($result) {
    echo "Image metadata updated.";
}
```
*Updates the name, category, and filename of an internal image.*

---

#### `replace($index, $uploaded_file, $uploaded_filename)`
Replaces the file of an existing internal image with a new upload.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image identifier (`image://...`). |
| `$uploaded_file` | `string` | Temporary path to the new uploaded file. |
| `$uploaded_filename` | `string` | Original filename of the new uploaded file. |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

**Inner Mechanisms:**
1. Validates operator permissions and checks if the image is internal.
2. Validates the new file’s extension against supported formats.
3. **If the new file has the same extension as the old file:**
   - Overwrites the old file directly.
4. **If the new file has a different extension:**
   - Generates a new unique filename.
   - Moves the new file to the storage directory.
   - Updates the metadata in the data store.
   - Deletes the old file.

**Usage Example:**
```php
$image = new image();
$id = "image://abc123";
if (isset($_FILES['new_file'])) {
    $success = $image->replace(
        $id,
        $_FILES['new_file']['tmp_name'],
        $_FILES['new_file']['name']
    );
    if ($success) {
        echo "Image replaced successfully.";
    } else {
        echo "Replacement failed.";
    }
}
```
*Replaces an existing image with a new upload while preserving its metadata.*

---

#### `link($url, $name = NULL, $category = NULL)`
Links an external image (via URL) and stores its metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL of the external image. |
| `$name` | `string\|array\|NULL` | Multilingual name for the image. If `NULL`, falls back to the filename extracted from the URL. |
| `$category` | `string\|NULL` | Category for the image. |

**Return Values:**
| Type | Description |
|------|-------------|
| `string\|FALSE` | Image identifier (`image://...`) on success, `FALSE` on failure. |

**Inner Mechanisms:**
1. Validates operator permissions.
2. Generates a default name if none is provided (falling back to the filename extracted from the URL).
3. Stores metadata (name, category, URL) in the data store and returns the image identifier.

**Usage Example:**
```php
$image = new image();
$id = $image->link(
    "https://example.com/logo.png",
    ["en" => "External Logo", "de" => "Externes Logo"],
    "Branding"
);
if ($id) {
    echo "External image linked with ID: " . x($id);
}
```
*Links an external image and stores its metadata for later use.*

---

#### `unlink($index)`
Deletes an image (internal or external) and its metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image identifier (`image://...`). |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

**Inner Mechanisms:**
1. Validates operator permissions and the image identifier.
2. **For internal images:**
   - Deletes the physical file from the storage directory.
3. Deletes the image’s metadata from the data store.

**Usage Example:**
```php
$image = new image();
$id = "image://abc123";
$success = $image->unlink($id);
if ($success) {
    echo "Image deleted successfully.";
}
```
*Deletes an image and its associated file (if internal).*

---

#### `internal($index)`
Checks if an image is internal (stored locally) or external (linked via URL).

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image identifier (`image://...`). |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the image is internal, `FALSE` if external. |

**Inner Mechanisms:**
1. Checks if the image has a `url` field in its metadata (external).
2. For internal images, verifies the existence of the physical file.

**Usage Example:**
```php
$image = new image();
$id = "image://abc123";
if ($image->internal($id)) {
    echo "This is an internal image.";
} else {
    echo "This is an external image.";
}
```
*Determines whether an image is stored locally or linked externally.*


<!-- HASH:0be857726ca94d5eacad731ab44aa0e0 -->
