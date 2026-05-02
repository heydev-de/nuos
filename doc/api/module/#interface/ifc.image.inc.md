# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.image.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Image Interface Module (`ifc.image.inc`)

This file implements the **Image Management Interface** for the NUOS platform, providing a complete UI for uploading, organizing, linking, editing, and processing images. It integrates with the `image` class to handle physical files, metadata, and caching, while exposing all operations through the **IFC (Interface Controller)** system.

---

### **Overview**
The module acts as a **message-driven interface** for the `image` class, handling:
- **Single/Multi Uploads** (local files)
- **Remote Image Linking** (URL-based)
- **Metadata Editing** (name, category, filename)
- **Replacement/Deletion** of images
- **Category Management** (rename, filtering)
- **Cache Clearing** (image and content cache)
- **Configuration** (preferred format, resolution, daemon processing)
- **Language Support** (per-language image selection)

All operations are **permission-aware**, requiring at least `CMS_L_ACCESS` (view) or `CMS_L_OPERATOR` (modify) privileges.

---

### **Constants & Variables**
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IMAGE_PERMISSION_OPERATOR` | (System-defined) | Permission level required for write operations (e.g., upload, delete). |
| `$object` | `NULL` | Current image identifier (e.g., `image:abc123`). Restored from cache if empty. |
| `$language` | `NULL` | Language code for multilingual image selection. |
| `$_object` | (Derived) | Language-specific image identifier (e.g., `image:abc123.en`). |

---

### **Message Handling**
The interface processes messages via `CMS_IFC_MESSAGE`, triggering specific actions. Each case is documented below.

---

### **Message Cases**

#### ### `select`
**Purpose**: Updates the selected image for a specific language.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Image identifier (e.g., `image:abc123`). |
| `$language` | `string` | Target language code. |

**Mechanism**:
- Calls `language_set()` to associate the image with the language.
- Updates the cached object for the current user.

**Usage**:
- Triggered when a user selects an image from the list or changes the language.

---

#### ### `select_language`
**Purpose**: Switches the active language for image selection.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Language code (e.g., `en`). |

**Mechanism**:
- Updates the `$language` variable directly.

**Usage**:
- Triggered via language selector UI.

---

#### ### `display`
**Purpose**: Renders a preview of the selected image in an iframe.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Image identifier (e.g., `image:abc123`). |

**Return**:
- Exits early if no image is selected.
- Outputs HTML for an `<iframe>` containing:
  - A clickable thumbnail (processed via `image_process()`).
  - Metadata (dimensions, MIME type).

**Mechanism**:
- Resolves the image URL via `translate_url()`.
- Generates a resized preview (600x600 max).
- Escapes all output with `x()` for XML safety.

**Usage**:
- Embedded in the main interface as a live preview.

---

#### ### `upload`
**Purpose**: Displays the upload form for a single image.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Base image identifier (language-neutral). |
| `$language` | `string` | Target language. |

**Mechanism**:
- Initializes an `ifc` form with fields for:
  - **Name** (text, 40 chars max).
  - **File** (file upload, restricted to GIF/JPG/PNG/SVG/WEBP).
  - **Category** (dropdown from `image_get_select()`).
  - **Filename** (text, 35 chars max, with `.ext` suffix).
- Auto-triggers the file input via JavaScript.

**Usage**:
- Triggered via the "Upload" menu option.

---

#### ### `_upload`
**Purpose**: Processes the uploaded file and saves it to the `image` class.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `string` | Temporary file path. |
| `$ifc_file1_name` | `string` | Original filename. |
| `$ifc_param1` | `string` | Image name. |
| `$ifc_param2` | `string` | Category. |
| `$ifc_param3` | `string` | Custom filename (optional). |

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Calls `image->add()` to store the file and metadata.
- Updates the `$object` with the new identifier.

**Usage**:
- Form submission handler for `upload`.

---

#### ### `upload_multi`
**Purpose**: Displays the form for bulk image uploads.
**Parameters**: Same as `upload`.

**Mechanism**:
- Uses a **multifile** input field.
- Auto-triggers the file input via JavaScript.

**Usage**:
- Triggered via the "Upload Multiple" menu option.

---

#### ### `_upload_multi`
**Purpose**: Processes multiple uploaded files.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `array` | Array of temporary file paths. |
| `$ifc_file1_name` | `array` | Array of original filenames. |
| `$ifc_param1` | `string` | Category. |

**Return**:
- `CMS_MSG_DONE` if at least one file succeeds, `CMS_MSG_ERROR` otherwise.

**Mechanism**:
- Combines files and filenames into an associative array.
- Calls `image->add()` for each file.
- Updates `$object` with the last successful upload.

**Usage**:
- Form submission handler for `upload_multi`.

---

#### ### `add` / `edit`
**Purpose**: Displays forms to add (link) or edit an image.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Image identifier (for edit mode). |
| `$language` | `string` | Target language. |

**Mechanism**:
- **Add Mode**:
  - Fields: Name, URL, Category.
- **Edit Mode**:
  - Fields: Name, Category.
  - If the image is **internal** (uploaded), shows a filename field.
  - If **external** (URL), shows the URL field.

**Usage**:
- Triggered via "Add" or "Edit" menu options.

---

#### ### `_add`
**Purpose**: Processes the addition of a remote image (URL-based).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name. |
| `$ifc_param2` | `string` | URL. |
| `$ifc_param3` | `string` | Category. |

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Calls `image->link()` to create a metadata entry.

**Usage**:
- Form submission handler for `add`.

---

#### ### `_edit`
**Purpose**: Processes edits to an image.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name. |
| `$ifc_param2` | `string` | URL (external) or Filename (internal). |
| `$ifc_param3` | `string` | Category. |
| `$ifc_param4` | `string` | Filename (internal only). |

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Calls `image->set()` with the appropriate parameters.
- For internal images, updates the filename.

**Usage**:
- Form submission handler for `edit`.

---

#### ### `replace`
**Purpose**: Displays the form to replace an internal image.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Image identifier. |

**Mechanism**:
- Validates that the image is **internal** (uploaded).
- Shows a file input field.
- Auto-submits on file selection via JavaScript.

**Usage**:
- Triggered via the "Replace" menu option.

---

#### ### `_replace`
**Purpose**: Processes the replacement file.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `string` | Temporary file path. |
| `$ifc_file1_name` | `string` | Original filename. |

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Calls `image->replace()` to overwrite the file.

**Usage**:
- Form submission handler for `replace`.

---

#### ### `delete`
**Purpose**: Deletes selected images and updates the selection.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$_object` | `array` | Array of image identifiers to delete. |

**Return**:
- `CMS_MSG_DONE` if all deletions succeed, `CMS_MSG_ERROR` otherwise.

**Mechanism**:
- Calls `image->unlink()` for each image.
- For each language, updates the selection to the first image in the same category (or clears it if none remain).

**Usage**:
- Triggered via the "Delete" menu option.

---

#### ### `category_rename`
**Purpose**: Displays the form to rename a category.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Image identifier (to infer the category). |

**Mechanism**:
- Pre-fills the current category name.

**Usage**:
- Triggered via the "Rename Category" menu option.

---

#### ### `_category_rename`
**Purpose**: Processes the category rename.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New category name. |

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Iterates through all images in the old category and updates their category field.
- Saves the changes via `image->data->save()`.

**Usage**:
- Form submission handler for `category_rename`.

---

#### ### `clear_cache`
**Purpose**: Clears the image and content caches.
**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Requires `CMS_IMAGE_PERMISSION_OPERATOR`.
- Deletes directories:
  - `CMS_DATA_PATH . "image/cache"`
  - `CMS_DATA_PATH . "#content/cache"`
- Uses `filemanager_delete()` for recursive deletion.

**Usage**:
- Triggered via the "Clear Cache" menu option.

---

#### ### `config`
**Purpose**: Displays the image configuration form.
**Mechanism**:
- Fields:
  - **Preferred Format** (WebP/JPEG).
  - **Maximum Resolution** (UHD-II, UHD-I, FHD, HD).
  - **Daemon Processing** (checkbox).

**Usage**:
- Triggered via the "Configuration" menu option.

---

#### ### `_config`
**Purpose**: Saves the configuration.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Preferred format (`webp`/`jpg`). |
| `$ifc_param2` | `string` | Resolution (e.g., `3840x2160`). |
| `$ifc_param3` | `bool` | Daemon processing enabled. |

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Uses the `system` class to store preferences.

**Usage**:
- Form submission handler for `config`.

---

### **Main Display**
**Purpose**: Renders the primary interface with:
- **Category Selector** (dropdown).
- **Image List** (thumbnails with checkboxes).
- **Language Selector** (if multilingual).
- **Preview Pane** (iframe).

**Mechanism**:
1. **Category Handling**:
   - If `$object` is set, infers its category.
   - Otherwise, restores the last selected category from cache.
2. **Menu Generation**:
   - **Insert** (if `CMS_IFC_SELECT` is enabled).
   - **Upload/Add/Edit/Delete** (operator-only).
   - **Clear Cache/Configuration** (operator-only).
3. **JavaScript**:
   - `image_select()`: Updates the preview when an image is selected.
   - Auto-activates list selection via `ifc_custom_select()`.

**Usage**:
- Loaded as the default view for the image interface.


<!-- HASH:e544b06b8f5bf3438129abb55413f077 -->
