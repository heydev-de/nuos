# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.image.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Image Management Module (`lib.image.inc`)

This file provides the core image management functionality for the NUOS platform, including:
- **Storage and retrieval** of image metadata in the system database
- **Upload and file handling** for local images
- **Linking to external images** (URL-based)
- **Permission-based operations** (operator-only actions)
- **Categorization and naming** of images with multilingual support

The module follows NUOS's zero-dependency approach, using the platform's built-in `data` class for storage and `cms_permission()` for access control.

---

## Global Functions

### `image_get_array()`
Returns a structured associative array of all images, grouped by category, for use in selection interfaces (e.g., dropdowns).

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `array`    | Nested associative array: `["Category" => ["Display Name" => "image://ID"]]` |

**Mechanisms:**
1. Loads all image records from `#system/image` dataset
2. Groups by `category` field
3. Uses localized `name` values (falling back to record key if empty)
4. Handles duplicate display names by appending `(1)`, `(2)`, etc.
5. Sorts categories and names alphabetically

**Usage:**
```php
$image_options = image_get_array();
// Used in form builders or image pickers
```

---

### `image_get_select()`
Returns a flat associative array of all unique image categories for filtering purposes.

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `array`    | Associative array: `["Category" => "Category"]` (sorted alphabetically)     |

**Mechanisms:**
1. Loads all image records
2. Extracts unique `category` values
3. Returns sorted list with empty default option

**Usage:**
```php
$categories = image_get_select();
// Used in category filters or form selectors
```

---

## `image` Class

Core class for image operations with permission-based access control.

### Properties

| Name       | Type       | Description                                                                 |
|------------|------------|-----------------------------------------------------------------------------|
| `data`     | `data`     | Instance of NUOS `data` class for `#system/image` dataset                   |
| `operator` | `bool`     | Permission flag (TRUE if current user has `CMS_IMAGE_PERMISSION_OPERATOR`)  |

---

### `__construct()`
Initializes the image manager and ensures the image storage directory exists.

**Mechanisms:**
1. Instantiates `data` class for `#system/image` dataset
2. Checks operator permissions via `cms_permission()`
3. Creates image storage directory (`CMS_DATA_PATH . "image"`) if missing

---

### `add()`
Handles image uploads, storing the file and creating a database record.

| Parameter           | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$uploaded_file`    | `string`   | Temporary file path from upload form                                        |
| `$uploaded_filename`| `string`   | Original filename from upload form                                          |
| `$name`             | `string`   | Localized name (optional)                                                   |
| `$category`         | `string`   | Category identifier (optional)                                              |
| `$filename`         | `string`   | Desired filename (optional)                                                 |

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `string`   | Image identifier (`image://ID`) on success                                  |
|              | `bool`     | `FALSE` on failure                                                          |

**Mechanisms:**
1. **Validation:** Checks permissions, file existence, and format (GIF/JPG/PNG/SVG/WEBP)
2. **Naming:**
   - Uses `$name` if provided (localized)
   - Falls back to `$filename` or original upload name
   - Generates random name if all else fails
3. **Filename Generation:**
   - Cleans filename via `stringtofilename()`
   - Appends incrementing number if filename exists
4. **Storage:**
   - Moves file to `CMS_DATA_PATH . "image/"`
   - Creates database record with metadata
5. **Compatibility:** Converts `jpeg` to `jpg` extension

**Usage:**
```php
$image_id = $image->add($_FILES['upload']['tmp_name'], $_FILES['upload']['name'], "Profile Picture");
```

---

### `set()`
Updates metadata for an existing image (both internal and external).

| Parameter           | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$index`            | `string`   | Image identifier (`image://ID`)                                             |
| `$name`             | `string`   | New localized name                                                          |
| `$category`         | `string`   | New category                                                                |
| `$filename_or_url`  | `string`   | New filename (internal) or URL (external)                                   |

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `string`   | Image identifier on success                                                 |
|              | `bool`     | `FALSE` on failure                                                          |

**Mechanisms:**
1. **Internal Images:**
   - Updates filename if provided (with collision handling)
   - Renames physical file if filename changes
   - Preserves old name if new name is empty
2. **External Images:**
   - Updates URL and metadata only
3. **Localization:** Handles empty names by preserving existing localized values

**Usage:**
```php
$image->set("image://123", "Updated Name", "Logos", "new-logo.png");
```

---

### `replace()`
Replaces the physical file for an existing internal image while preserving metadata.

| Parameter           | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$index`            | `string`   | Image identifier (`image://ID`)                                             |
| `$uploaded_file`    | `string`   | Temporary file path from upload form                                        |
| `$uploaded_filename`| `string`   | Original filename from upload form                                          |

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `bool`     | `TRUE` on success, `FALSE` on failure                                       |

**Mechanisms:**
1. **Validation:** Checks permissions, image type (internal), and file format
2. **Extension Handling:**
   - Preserves existing file if extensions match
   - Deletes old file and creates new one if extensions differ
3. **Collision Handling:** Appends incrementing number to filename if needed

**Usage:**
```php
$image->replace("image://123", $_FILES['upload']['tmp_name'], $_FILES['upload']['name']);
```

---

### `link()`
Creates a record for an external image (URL-based) without file storage.

| Parameter   | Type       | Description                                                                 |
|-------------|------------|-----------------------------------------------------------------------------|
| `$url`      | `string`   | Full URL to external image                                                  |
| `$name`     | `string`   | Localized name (optional)                                                   |
| `$category` | `string`   | Category identifier (optional)                                              |

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `string`   | Image identifier (`image://ID`) on success                                  |
|              | `bool`     | `FALSE` on failure                                                          |

**Mechanisms:**
1. **Naming:** Uses filename from URL if no name provided
2. **Storage:** Creates database record with URL and metadata

**Usage:**
```php
$image_id = $image->link("https://example.com/logo.png", "Company Logo");
```

---

### `unlink()`
Removes an image record and deletes the physical file (if internal).

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| `$index`  | `string`   | Image identifier (`image://ID`)                                             |

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `bool`     | `TRUE` on success, `FALSE` on failure                                       |

**Mechanisms:**
1. **Internal Images:** Deletes physical file from `CMS_DATA_PATH . "image/"`
2. **Database:** Removes record from `#system/image` dataset

**Usage:**
```php
$image->unlink("image://123");
```

---

### `internal()`
Checks if an image is stored locally (vs. external URL).

| Parameter | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| `$index`  | `string`   | Image identifier (`image://ID`)                                             |

| Return Value | Type       | Description                                                                 |
|--------------|------------|-----------------------------------------------------------------------------|
|              | `bool`     | `TRUE` if internal, `FALSE` if external                                     |

**Mechanisms:**
1. Checks for `url` field in database record
2. Verifies physical file existence for internal images

**Usage:**
```php
if ($image->internal("image://123")) { /* Local file operations */ }
```


<!-- HASH:305bb51e418649d905a89b08dacd201d -->
