# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.image.inc)

- **Version:** `26.8.9.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Image Interface Module (`ifc.image.inc`)

This file implements the **Image Management Interface** for the PWNC Web Platform. It provides a complete user interface for:
- Uploading, linking, editing, replacing, and deleting images
- Managing image categories
- Configuring system-wide image preferences
- Previewing images with metadata
- Multi-language support for image objects

The interface integrates with the platform's **IFC (Interface Controller)** system, handling user interactions through message-based workflows.

---

## Core Workflow

The module operates in two main phases:

1. **Message Handling** – Processes user actions (e.g., upload, edit, delete) via `CMS_IFC_MESSAGE`.
2. **Main Display** – Renders the image browser with category selection, thumbnail list, and preview pane.

---

## Key Components

### Constants & Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$object` | `NULL` | Currently selected image object (multilingual-aware). Retrieved from cache or user input. |
| `$language` | `init($language)` | Current language context for multilingual image management. |
| `$category` | `""` | Current image category filter. |
| `$_object` | `NULL` | Language-specific image identifier (e.g., `image:abc123`). |

---

## Message Handlers

### `case "select"`
**Purpose**: Updates the selected image object based on user input.
**Parameters**:
- `$ifc_param` – New object identifier.
- `$language` – Current language context.
**Mechanism**: Uses `language_set()` to update the multilingual object reference.
**Usage**: Triggered when a user selects a category or image from the list.

---

### `case "select_language"`
**Purpose**: Changes the active language for image management.
**Parameters**:
- `$ifc_param` – Target language code.
**Mechanism**: Updates the `$language` variable and refreshes the interface.
**Usage**: Called via the language selector dropdown.

---

### `case "display"`
**Purpose**: Renders a preview of the selected image in an iframe.
**Parameters**:
- `$object` – Image identifier (e.g., `image:abc123`).
**Mechanism**:
1. Retrieves the image path via `translate_url()`.
2. Generates a resized preview using `image_process()`.
3. Displays metadata (dimensions, MIME type) via `getimagesize()`.
**Return**: HTML iframe content with image and metadata.
**Usage Example**:
```php
// Displays a 600x600 preview of "image:example" with dimensions and MIME type
cms_url(["ifc_page" => "image", "ifc_message" => "display", "object" => "image:example"]);
```

---

### `case "upload"`
**Purpose**: Renders the upload form for a single image.
**Mechanism**:
1. Initializes the `image` class and retrieves the current category.
2. Uses the `ifc` class to generate a form with fields for:
   - Name (text input)
   - File (file upload)
   - Category (dropdown)
   - Filename (text input with extension display)
3. Auto-triggers the file input via JavaScript.
**Usage**: Triggered when a user clicks the "Upload" button.

---

### `case "_upload"`
**Purpose**: Processes the uploaded image.
**Parameters**:
- `$ifc_file1` – Uploaded file data.
- `$ifc_file1_name` – Original filename.
- `$ifc_param1` – User-provided name.
- `$ifc_param2` – Category.
- `$ifc_param3` – Custom filename (optional).
**Mechanism**:
1. Calls `image->add()` to store the file.
2. Updates the `$object` reference and sets the response message (`CMS_MSG_DONE` or `CMS_MSG_ERROR`).
**Usage**: Form submission handler for `upload`.

---

### `case "upload_multi"`
**Purpose**: Renders a form for uploading multiple images at once.
**Mechanism**:
1. Uses `multifile` input for batch uploads.
2. Shares the same category dropdown as the single upload form.
**Usage**: Triggered when a user clicks the "Upload Multiple" button.

---

### `case "_upload_multi"`
**Purpose**: Processes multiple uploaded images.
**Parameters**:
- `$ifc_file1` – Array of uploaded files.
- `$ifc_file1_name` – Array of original filenames.
- `$ifc_param1` – Category.
**Mechanism**:
1. Combines files and filenames into an associative array.
2. Iterates through each file, calling `image->add()`.
3. Sets `$ifc_response` to `CMS_MSG_ERROR` if any upload fails.
**Usage**: Form submission handler for `upload_multi`.

---

### `case "add"` / `case "edit"`
**Purpose**: Renders forms for adding or editing image links (external URLs).
**Mechanism**:
1. For **edits**, retrieves existing data (name, URL, category) via `image->data->get()`.
2. For **internal images**, displays the filename and extension.
3. Uses `ifc` to generate form fields:
   - Name (text input)
   - URL (text input, hidden for internal images)
   - Category (dropdown)
   - Filename (text input, internal images only).
**Usage**:
- `add`: Triggered for new external image links.
- `edit`: Triggered for existing images (internal or external).

---

### `case "_add"`
**Purpose**: Processes a new external image link.
**Parameters**:
- `$ifc_param1` – Name.
- `$ifc_param2` – URL.
- `$ifc_param3` – Category.
**Mechanism**: Calls `image->link()` and updates `$object`.
**Usage**: Form submission handler for `add`.

---

### `case "_edit"`
**Purpose**: Processes edits to an image (internal or external).
**Parameters**:
- `$ifc_param1` – Name.
- `$ifc_param2` – URL (external) or filename (internal).
- `$ifc_param3` – Category.
**Mechanism**:
1. Checks if the image is internal via `image->internal()`.
2. Calls `image->set()` with the appropriate parameters.
**Usage**: Form submission handler for `edit`.

---

### `case "replace"`
**Purpose**: Renders a form to replace an internal image file.
**Mechanism**:
1. Verifies the image is internal via `image->internal()`.
2. Uses `ifc` to generate a file upload field.
3. Auto-submits the form on file selection via JavaScript.
**Usage**: Triggered when a user clicks the "Replace" button.

---

### `case "_replace"`
**Purpose**: Processes a replacement file upload.
**Parameters**:
- `$ifc_file1` – New file data.
- `$ifc_file1_name` – Original filename.
**Mechanism**: Calls `image->replace()` and updates `$ifc_response`.
**Usage**: Form submission handler for `replace`.

---

### `case "delete"`
**Purpose**: Deletes selected images and updates the interface.
**Mechanism**:
1. Iterates through `$_object` (selected images) and calls `image->unlink()`.
2. Updates the `$object` reference to the next available image in the category.
3. Handles multilingual cleanup via `language_get()` and `language_set()`.
**Usage**: Triggered when a user clicks the "Delete" button.

---

### `case "category_rename"`
**Purpose**: Renders a form to rename a category.
**Mechanism**:
1. Retrieves the current category from the selected image.
2. Uses `ifc` to generate a text input for the new category name.
**Usage**: Triggered when a user clicks the "Rename Category" button.

---

### `case "_category_rename"`
**Purpose**: Processes the category rename.
**Parameters**:
- `$ifc_param1` – New category name.
**Mechanism**:
1. Iterates through all images in the old category.
2. Updates each image's category via `image->data->set()`.
3. Saves changes via `image->data->save()`.
**Usage**: Form submission handler for `category_rename`.

---

### `case "clear_cache"`
**Purpose**: Clears the image and content cache directories.
**Mechanism**:
1. Verifies operator permissions.
2. Deletes cache directories via `filemanager_delete()`.
3. Sets a 10-minute execution timeout.
**Usage**: Triggered when a user clicks the "Clear Cache" button.

---

### `case "config"`
**Purpose**: Renders the system-wide image configuration form.
**Mechanism**:
1. Retrieves current settings via `system->getval()`:
   - Preferred format (WebP/JPEG)
   - Maximum resolution (UHD-II, UHD-I, FHD, HD)
   - Daemon processing (checkbox).
2. Uses `ifc` to generate form fields.
**Usage**: Triggered when a user clicks the "Configuration" button.

---

### `case "_config"`
**Purpose**: Processes configuration updates.
**Parameters**:
- `$ifc_param1` – Preferred format.
- `$ifc_param2` – Maximum resolution.
- `$ifc_param3` – Daemon processing (boolean).
**Mechanism**:
1. Verifies operator permissions.
2. Updates settings via `system->setval()` and saves.
**Usage**: Form submission handler for `config`.

---

## Main Display

### Rendering Logic
1. **Category Selection**:
   - Populates a dropdown with categories from `image_get_array()`.
   - Updates the image list when changed.
2. **Image List**:
   - Displays thumbnails (100x100) via `image_process()`.
   - Supports checkbox selection for bulk operations.
   - Double-click inserts the image (if in `CMS_IFC_SELECT` mode).
3. **Preview Pane**:
   - Embeds an iframe showing the selected image via the `display` message.
4. **Menu**:
   - Dynamically generates buttons based on permissions and selection state:
     - Upload (single/multi)
     - Add/Edit
     - Replace/Delete
     - Rename Category
     - Clear Cache
     - Configuration.

### JavaScript Helpers
- `image_select(value)`: Updates the preview when an image is selected.
- `ifc_list_*` functions: Bulk selection controls (All/Invert/None).

---

## Example Usage

### Uploading an Image
```php
// Navigate to the upload form
$url = cms_url([
    "ifc_page" => "image",
    "ifc_message" => "upload"
]);
// After form submission, the "_upload" handler processes the file
```

### Inserting an Image into Content
```php
// In a content editor, double-click an image in the list to insert:
$image_url = translate_url("image:abc123");
// Returns: "https://pwnc.it/data/image/abc123.jpg"
```

### Clearing the Cache
```php
// Programmatically clear the cache (operator-only)
cms_url([
    "ifc_page" => "image",
    "ifc_message" => "clear_cache"
]);
```

### Configuring Image Settings
```php
// Set WebP as the preferred format and enable daemon processing
$system = new system();
$system->setval("webp", "image", "preference");
$system->setval(TRUE, "image", "daemon");
$system->save();
```


<!-- HASH:dc0ad45b62c22672309a04a9531ff86c -->
