# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.media.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.media.inc)

- **Version:** `26.8.9.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Interface Module (`ifc.media.inc`)

This file implements the **Media Interface** for the PWNC Web Platform, providing a user interface for managing media assets (upload, edit, delete, categorize, and preview). It integrates with the `media` and `media_type` libraries to handle media storage, retrieval, and type-specific operations.

The interface supports:
- **Single/Multi-file uploads**
- **Media linking (external URLs)**
- **Media replacement**
- **Category management**
- **Media type configuration**
- **Language-aware media selection**

---

## Core Functionality

### Message Handling (`switch (CMS_IFC_MESSAGE)`)
The interface processes user actions via `CMS_IFC_MESSAGE`, a system-defined variable that determines the current operation. Each case handles a specific media-related task.

---

### `case "select"`
**Purpose:**
Updates the selected media object based on user input and refreshes the interface.

**Parameters:**
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$object`     | `string` | Current media object identifier.     |
| `$ifc_param`  | `string` | New media object identifier.         |
| `$language`   | `string` | Current language context.            |

**Inner Mechanisms:**
- Uses `language_set()` to update the selected object in the current language context.
- Relies on `cms_cache()` to persist the selection.

**Usage Example:**
```php
// User selects a new media object from a dropdown
$object = language_set($object, "media_123", "en");
```

---

### `case "select_language"`
**Purpose:**
Switches the active language context for media selection.

**Parameters:**
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$ifc_param`  | `string` | Language code (e.g., "en", "de").    |

**Inner Mechanisms:**
- Updates the `$language` variable, which cascades to all subsequent operations.

**Usage Example:**
```php
// User switches from English to German
$language = "de";
```

---

### `case "display"`
**Purpose:**
Renders a preview of the selected media object in an iframe.

**Parameters:**
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$object` | `string` | Media object identifier.             |

**Return Values:**
- **Output:** HTML iframe with media preview and metadata (size, MIME type).

**Inner Mechanisms:**
1. Validates the object exists.
2. Caches the object for persistence.
3. Uses `media->parse()` to generate a preview (resized to 600x400).
4. Displays metadata (file size, MIME type) via `format_bytesize()` and `get_mime_type()`.

**Usage Example:**
```php
// Display a preview of "media_123"
$media = new media();
$preview = $media->parse("media_123", NULL, "600", "400", "Sample Image", "Sample Image");
echo $preview;
```

---

### `case "upload"` / `case "_upload"`
**Purpose:**
Handles single-file uploads via a form and processes the uploaded file.

**Parameters (Form Fields):**
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$ifc_file1`    | `file`   | Uploaded file data.                  |
| `$ifc_file1_name` | `string` | Original filename.                   |
| `$ifc_param1`   | `string` | Media name.                          |
| `$ifc_param2`   | `string` | Media type.                          |
| `$ifc_param3`   | `string` | Category.                            |
| `$ifc_param4`   | `string` | Custom filename (optional).          |

**Return Values:**
- **Success:** `CMS_MSG_DONE`
- **Failure:** `CMS_MSG_ERROR`

**Inner Mechanisms:**
1. **Form Display (`upload`):**
   - Renders a form with fields for name, file, type, and category.
   - Auto-triggers the file input via JavaScript.
2. **Processing (`_upload`):**
   - Uses `media->add()` to store the file.
   - Updates the object selection via `language_set()`.

**Usage Example:**
```php
// Process an uploaded file
$media = new media();
$result = $media->add(
    "/tmp/uploaded_file.jpg",
    "uploaded_file.jpg",
    "My Image",
    "image/jpeg",
    "gallery"
);
```

---

### `case "upload_multi"` / `case "_upload_multi"`
**Purpose:**
Handles multi-file uploads via a form and processes all uploaded files.

**Parameters (Form Fields):**
| Name            | Type       | Description                          |
|-----------------|------------|--------------------------------------|
| `$ifc_file1[]`  | `file[]`   | Array of uploaded files.             |
| `$ifc_file1_name[]` | `string[]` | Array of original filenames.         |
| `$ifc_param1`   | `string`   | Media type.                          |
| `$ifc_param2`   | `string`   | Category.                            |

**Return Values:**
- **Partial Success:** `CMS_MSG_DONE` (if at least one file succeeds)
- **Failure:** `CMS_MSG_ERROR` (if all files fail)

**Inner Mechanisms:**
1. **Form Display (`upload_multi`):**
   - Renders a multi-file input and category selector.
2. **Processing (`_upload_multi`):**
   - Combines filenames with file data using `array_combine()`.
   - Processes each file via `media->add()`.
   - Returns success if at least one file is uploaded.

**Usage Example:**
```php
// Process multiple uploaded files
$files = ["/tmp/file1.jpg" => "file1.jpg", "/tmp/file2.png" => "file2.png"];
$media = new media();
foreach ($files as $path => $name) {
    $media->add($path, $name, NULL, "image", "gallery");
}
```

---

### `case "add"` / `case "_add"`
**Purpose:**
Adds a media object by linking to an external URL (instead of uploading a file).

**Parameters (Form Fields):**
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$ifc_param1` | `string` | Media name.                          |
| `$ifc_param2` | `string` | External URL.                        |
| `$ifc_param3` | `string` | Media type.                          |
| `$ifc_param4` | `string` | Category.                            |

**Return Values:**
- **Success:** `CMS_MSG_DONE`
- **Failure:** `CMS_MSG_ERROR`

**Inner Mechanisms:**
- Uses `media->link()` to create a reference to the external URL.

**Usage Example:**
```php
// Add an external media link
$media = new media();
$result = $media->link(
    "https://example.com/image.jpg",
    "External Image",
    "image/jpeg",
    "external"
);
```

---

### `case "edit"` / `case "_edit"`
**Purpose:**
Edits an existing media object (name, URL, type, category, or filename).

**Parameters (Form Fields):**
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$ifc_param1` | `string` | Media name.                          |
| `$ifc_param2` | `string` | URL (external) or filename (internal). |
| `$ifc_param3` | `string` | Media type.                          |
| `$ifc_param4` | `string` | Category.                            |
| `$ifc_param5` | `string` | Filename (internal only).            |

**Return Values:**
- **Success:** `CMS_MSG_DONE`
- **Failure:** `CMS_MSG_ERROR`

**Inner Mechanisms:**
1. **Form Display (`edit`):**
   - Pre-fills form fields with existing data.
   - Hides the URL field for internal media (replaced with filename input).
2. **Processing (`_edit`):**
   - Uses `media->set()` to update the object.
   - Handles internal/external media differently.

**Usage Example:**
```php
// Edit an internal media object
$media = new media();
$result = $media->set(
    "media_123",
    "Updated Name",
    "image/jpeg",
    "updated_category",
    "new_filename.jpg"
);
```

---

### `case "replace"` / `case "_replace"`
**Purpose:**
Replaces the file of an internal media object while preserving metadata.

**Parameters (Form Fields):**
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$ifc_file1`    | `file`   | New file data.                       |
| `$ifc_file1_name` | `string` | Original filename.                   |

**Return Values:**
- **Success:** `CMS_MSG_DONE`
- **Failure:** `CMS_MSG_ERROR`

**Inner Mechanisms:**
1. **Validation:**
   - Checks if the media is internal (external media cannot be replaced).
2. **Processing (`_replace`):**
   - Uses `media->replace()` to swap the file.

**Usage Example:**
```php
// Replace an internal media file
$media = new media();
$result = $media->replace("media_123", "/tmp/new_file.jpg", "new_file.jpg");
```

---

### `case "delete"`
**Purpose:**
Deletes selected media objects and updates the interface.

**Parameters:**
| Name          | Type       | Description                          |
|---------------|------------|--------------------------------------|
| `$_object`    | `string[]` | Array of media object identifiers.   |

**Return Values:**
- **Success:** `CMS_MSG_DONE`
- **Failure:** `CMS_MSG_ERROR`

**Inner Mechanisms:**
1. **Deletion:**
   - Uses `media->unlink()` to delete each object.
2. **Selection Update:**
   - Removes deleted objects from the selection.
   - Falls back to the first object in the category if the active object is deleted.

**Usage Example:**
```php
// Delete multiple media objects
$media = new media();
foreach (["media_123", "media_456"] as $id) {
    $media->unlink($id);
}
```

---

### `case "category_rename"` / `case "_category_rename"`
**Purpose:**
Renames a media category and updates all objects in that category.

**Parameters (Form Fields):**
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$ifc_param1` | `string` | New category name.                   |

**Return Values:**
- **Success:** `CMS_MSG_DONE`
- **Failure:** `CMS_MSG_ERROR`

**Inner Mechanisms:**
1. **Form Display (`category_rename`):**
   - Pre-fills the current category name.
2. **Processing (`_category_rename`):**
   - Iterates through all media objects and updates their category via `media->data->set()`.

**Usage Example:**
```php
// Rename a category from "old" to "new"
$media = new media();
$media->data->move("first");
while ($key = $media->data->move("next")) {
    if ($media->data->get($key, "category") === "old") {
        $media->data->set("new", $key, "category");
    }
}
$media->data->save();
```

---

### `case "type"` / `case "type_select"` / `case "type_add"` / `case "type_set"` / `case "type_delete"`
**Purpose:**
Manages media types (e.g., "image/jpeg", "video/mp4") and their associated settings.

**Parameters (Form Fields):**
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$ifc_param`  | `string` | Media type identifier.               |
| `$ifc_param1` | `string` | Type name.                           |
| `$ifc_param2` | `string` | File extensions (e.g., "jpg,png").   |
| `$ifc_param3` | `string` | HTML embedding code.                 |
| `$list[]`     | `string[]` | Array of types to delete.            |

**Return Values:**
- **Success:** `CMS_MSG_DONE`
- **Failure:** `CMS_MSG_ERROR`

**Inner Mechanisms:**
1. **Type Selection (`type_select`):**
   - Updates the active type for editing.
2. **Type Addition (`type_add`):**
   - Uses `media_type->add()` to create a new type.
3. **Type Update (`type_set`):**
   - Uses `media_type->set()` to update name, extensions, or code.
4. **Type Deletion (`type_delete`):**
   - Uses `media_type->delete()` to remove selected types.

**Usage Example:**
```php
// Add a new media type
$media_type = new media_type();
$new_type = $media_type->add("Custom Video");
$media_type->set($new_type, "Custom Video", "mp4,webm", "<video src=\"%src%\">%alt%</video>");
```

---

## Main Display Logic

### Category and Object Selection
**Purpose:**
Renders the primary interface for browsing and selecting media.

**Inner Mechanisms:**
1. **Category Dropdown:**
   - Populated via `media_get_array()`.
   - Triggers `ifc_post("select")` on change.
2. **Object List:**
   - Displays media objects in the selected category.
   - Supports checkbox selection and double-click to preview.
3. **Language Selector:**
   - Shown if `CMS_LANGUAGE_ENABLED` is `TRUE`.
4. **Preview Iframe:**
   - Displays the selected media via `case "display"`.

**Usage Example:**
```php
// Render the media interface
$media = new media();
$categories = media_get_array();
echo "<select onchange='ifc_post(\"select\", this.value)'>";
foreach ($categories as $category => $objects) {
    echo "<option value='" . x(current($objects)) . "'>" . x($category) . "</option>";
}
echo "</select>";
```

---

## Helper Functions

### `media_get_array()`
**Purpose:**
Retrieves a structured array of all media objects grouped by category.

**Return Values:**
- **Array:** `["category" => ["object_id" => "object_name"]]`

**Usage Example:**
```php
$media_array = media_get_array();
foreach ($media_array as $category => $objects) {
    echo "Category: $category";
    foreach ($objects as $id => $name) {
        echo "- $name ($id)";
    }
}
```

---

### `media_get_select()`
**Purpose:**
Generates a `<select>` element for media categories.

**Return Values:**
- **String:** HTML `<select>` element.

**Usage Example:**
```php
echo media_get_select();
```

---

### `media_type_get_select()`
**Purpose:**
Generates a `<select>` element for media types.

**Return Values:**
- **String:** HTML `<select>` element.

**Usage Example:**
```php
echo media_type_get_select();
```

---

## JavaScript Integration

### `media_select(value)`
**Purpose:**
Updates the media preview when a new object is selected.

**Parameters:**
| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `value` | `string` | Media object identifier.             |

**Inner Mechanisms:**
- Updates the `object` parameter in the current language context.
- Reloads the preview iframe with the new object.

**Usage Example:**
```javascript
// Select a new media object
media_select("media_123");
```

---

### `ifc_autopost(field, message)`
**Purpose:**
Auto-submits a form when a file input changes (used for replacements).

**Parameters:**
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `field`   | `string` | Input field ID.                      |
| `message` | `string` | Message to trigger (e.g., "_replace"). |

**Usage Example:**
```javascript
ifc_autopost("ifc_file1", "_replace");
```


<!-- HASH:291129f31e52031e18999d4acb99bee3 -->
