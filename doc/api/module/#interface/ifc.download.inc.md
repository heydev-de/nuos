# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.download.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.download.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Download Interface (`ifc.download.inc`)

This file implements the **download management interface** for the PWNC Web Platform. It provides a full-featured UI for managing downloadable files, including uploading, editing, replacing, deleting, and displaying file metadata. The interface supports multilingual content, category organization, and operator-level permissions.

The interface is driven by a message-based dispatch system (`CMS_IFC_MESSAGE`) that determines which action to perform — such as `display`, `upload`, `edit`, `delete`, etc. Each case handles a specific operation, often rendering forms or executing backend logic via the `download` library.

---

### Core Components

| Component | Description |
|----------|-------------|
| `download` class | Handles file storage, retrieval, and metadata operations |
| `ifc` class | Manages form rendering and interface output |
| `language_*` functions | Handle multilingual object mapping |
| `cms_cache` | Caches user-specific state like selected object/category |
| `CMS_IFC_MESSAGE` | Determines current interface action |

---

## Message Handling / Sub Display

### `switch (CMS_IFC_MESSAGE)`

Dispatches control to different handlers based on the current interface message.

#### Case: `"select"`

Sets the active language for the selected object.

```php
$object = language_set($object, $ifc_param, $language);
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | Current object identifier |
| `$ifc_param` | string | New language value |
| `$language` | string | Active language context |

**Usage Example:**
When a user selects a language from a dropdown, this updates the object reference accordingly.

---

#### Case: `"select_language"`

Updates the active language.

```php
$language = $ifc_param;
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | string | Selected language code |

**Usage Example:**
User switches interface language; subsequent operations use the new language context.

---

#### Case: `"display"`

Displays detailed information about a selected download item.

**Mechanism:**
1. Instantiates `ifc` without menu/response.
2. Retrieves filename and name from the `download` data store.
3. Checks if the physical file exists.
4. Renders an HTML table with:
   - File name
   - Format/extension
   - Size
   - Description
   - Download button (if permitted)

**Key Functions Used:**
- `x()` – XML escaping
- `l()` – Localization
- `format_bytesize()` – Human-readable size formatting
- `get_mime_type()` – MIME type detection
- `cms_url()` – URL generation with CSRF protection

**Example Output:**
A formatted table showing file details inside an iframe.

---

#### Case: `"download"`

Triggers actual file download.

```php
download(CMS_DATA_PATH . "#download/" . $filename, $name);
exit();
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$filename` | string | Physical file path |
| `$name` | string | Display name for download dialog |

**Usage Example:**
User clicks "Download" button → browser initiates file transfer.

---

#### Case: `"upload"`

Renders upload form for single file.

**Mechanism:**
1. Creates `ifc` instance with response/page context.
2. Sets fields:
   - Name (text input)
   - Description (text editor)
   - File selector
3. Auto-focuses file input via JavaScript.
4. Adds category selection dropdown.

**Example Form Fields:**
```html
<input type="text" name="ifc_param1"> <!-- Name -->
<textarea name="ifc_param2"></textarea> <!-- Description -->
<input type="file" name="ifc_file1"> <!-- File -->
<select name="ifc_param3">...</select> <!-- Category -->
```

---

#### Case: `"_upload"`

Processes uploaded file.

```php
$download->add($ifc_file1, $ifc_file1_name, $ifc_param1, $ifc_param2, $ifc_param3);
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | array | Uploaded file data |
| `$ifc_file1_name` | string | Original filename |
| `$ifc_param1` | string | Display name |
| `$ifc_param2` | string | Description |
| `$ifc_param3` | string | Category |

**Return Value:**
Returns new object ID on success, triggers `CMS_MSG_DONE`.

---

#### Case: `"upload_multi"`

Renders multi-file upload form.

Similar to `"upload"` but uses `multifile` field type allowing multiple selections.

---

#### Case: `"_upload_multi"`

Handles batch upload processing.

```php
foreach ($array AS $key => $value) {
    $download->add($key, $value, NULL, NULL, $ifc_param1);
}
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$array` | array | Map of temp paths to original names |
| `$ifc_param1` | string | Shared category |

**Behavior:**
Iterates over all files, adds each individually, tracks errors.

---

#### Case: `"edit"`

Renders edit form pre-filled with existing values.

**Mechanism:**
1. Loads current object data.
2. Populates:
   - Name field
   - Description editor
   - Category dropdown

**Example:**
Pre-filled form allowing modification of download metadata.

---

#### Case: `"_edit"`

Saves edited metadata.

```php
$download->set($_object, $ifc_param1, $ifc_param2, $ifc_param3);
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$_object` | string | Object key |
| `$ifc_param1` | string | Updated name |
| `$ifc_param2` | string | Updated description |
| `$ifc_param3` | string | Updated category |

---

#### Case: `"replace"`

Renders file replacement form.

Uses auto-post mechanism to submit file directly.

---

#### Case: `"_replace"`

Replaces physical file while preserving metadata.

```php
$download->replace($_object, $ifc_file1, $ifc_file1_name);
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$_object` | string | Object key |
| `$ifc_file1` | array | New uploaded file |
| `$ifc_file1_name` | string | New filename |

---

#### Case: `"delete"`

Deletes selected download items.

**Mechanism:**
1. Iterates over selected objects.
2. Calls `$download->unlink()` for each.
3. Updates language mappings.
4. Resets selection to first available item in same category.

**Error Handling:**
Tracks failures using `$flag_error`.

---

#### Case: `"category_rename"`

Renders category rename form.

Pre-populates with current category name.

---

#### Case: `"_category_rename"`

Renames category across all matching items.

```php
while ($key = $download->data->move("next")) {
    if (streq($_category, $category))
        $download->data->set($ifc_param1, $key, "category");
}
$download->data->save();
```

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | New category name |

---

## Main Display

Renders the primary download management interface.

### Initialization

```php
$download = new download();
$_object = language_get($object, $language, TRUE);
$array = download_get_array();
```

Determines current object and builds category list.

### Category/Object Resolution

If object exists in array → get its category.
Otherwise → resolve object from cached category.

### Menu Construction

Builds contextual menu based on permissions and selection state.

| Condition | Menu Items |
|----------|------------|
| `CMS_IFC_SELECT && $_object` | Insert command |
| `$download->operator` | Upload, Upload Multi, Edit, Replace, Delete, Rename |

### Interface Rendering

Outputs:
1. Category selector dropdown
2. Object list (checkboxes)
3. Language selector (if enabled)
4. Iframe preview of selected object

### JavaScript Integration

Includes dynamic scripts for:
- Object selection (`download_select`)
- Custom select behavior (`ifc_custom_select`)
- List activation/inversion/deactivation

---

## Usage Scenarios

### Uploading a Single File

1. Navigate to download interface.
2. Click "Upload" button.
3. Fill in name, description, select file.
4. Submit form → file stored with metadata.

### Editing Metadata

1. Select existing download.
2. Click "Edit".
3. Modify fields.
4. Save changes.

### Replacing a File

1. Select download.
2. Click "Replace".
3. Choose new file.
4. Auto-submission replaces file content.

### Deleting Files

1. Check boxes for items to delete.
2. Click "Delete".
3. Confirmation removes files and cleans up references.

### Managing Categories

1. Rename category through dedicated form.
2. All items in old category updated automatically.

---

## Key Dependencies

| Function/Class | Purpose |
|----------------|---------|
| `download` | Core file/data handler |
| `ifc` | Interface/form builder |
| `language_get/set` | Multilingual object mapping |
| `cms_cache` | State persistence |
| `cms_url` | Secure URL generation |
| `x`, `q`, `l` | Output escaping/localization |
| `format_bytesize` | Readable file sizes |
| `get_mime_type` | MIME detection |
| `file_extension` | Extension parsing |

---

## Security Considerations

- Uses `sqlesc()` internally for SQL safety.
- Applies `x()` for HTML output escaping.
- Leverages `cms_url()` for CSRF-safe URLs.
- Enforces permission checks before destructive actions.
- Sandboxed iframe prevents script injection in previews.

---

## Example Workflow: Full Upload Cycle

1. User navigates to download interface.
2. Clicks "Upload" → sees form with name/description/file/category fields.
3. Fills form and submits.
4. Backend calls `$download->add(...)`.
5. On success, object appears in category list.
6. User can now view, edit, replace, or delete the file.

---


<!-- HASH:02d0aaa400a610968d4b62d17ae45781 -->
