# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.download.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.download.inc)

- **Version:** `26.8.7.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Download Interface Module (`ifc.download.inc`)

This file implements the **Download Management Interface** for the PWNC Web Platform. It provides a complete user interface for:
- Uploading, editing, replacing, and deleting downloadable files
- Organizing downloads into categories
- Displaying download metadata (name, description, size, format)
- Handling multilingual support for download metadata
- Generating secure download links

The interface integrates with the platform's **IFC (Interface Controller)** system, handling messages (`CMS_IFC_MESSAGE`) to perform actions and render appropriate UI components.

---

## Core Components

### Constants & Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$object` | `NULL` | Current download object identifier (e.g., `download://filename`). Retrieved from cache or set via interface. |
| `$language` | `init($language)` | Current language code for multilingual metadata. |
| `$category` | `""` | Current category filter for downloads. Persisted in cache. |
| `$download` | `new download()` | Instance of the `download` class handling data operations. |

---

## Message Handlers

The interface responds to various `CMS_IFC_MESSAGE` values, each triggering a specific action or UI state.

---

### `select`

**Purpose:**
Updates the selected download object and refreshes the display.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | New object identifier (e.g., `download://file.pdf`). |
| `$ifc_param` | `string` | Raw parameter from IFC (used for language-aware object setting). |
| `$language` | `string` | Current language code. |

**Inner Mechanism:**
- Uses `language_set()` to update the object identifier in the current language context.
- The updated object is stored in cache and used to refresh the display.

**Usage Example:**
```php
// Triggered when user selects a download from the list
cms_url(["ifc_page" => "download", "ifc_message" => "select", "object" => "download://manual.pdf"]);
```

---

### `select_language`

**Purpose:**
Changes the active language for metadata display and editing.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | New language code (e.g., `en`, `de`). |

**Inner Mechanism:**
- Updates `$language` and triggers a UI refresh.

**Usage Example:**
```php
// User clicks a language flag in the UI
cms_url(["ifc_page" => "download", "ifc_message" => "select_language", "param" => "es"]);
```

---

### `display`

**Purpose:**
Renders a detailed view of a download, including metadata and a download button (if permitted).

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Download object identifier. |

**Return/Output:**
- HTML table with download metadata (name, format, size, description).
- Download button (if user has permission).

**Inner Mechanism:**
- Retrieves metadata using `$download->data->get()`.
- Falls back to filename if no name is set.
- Uses `ifc_table_open()`/`close()` for consistent UI styling.
- Escapes all output with `x()` for XML safety.

**Usage Example:**
```php
// Display details for a specific download
cms_url(["ifc_page" => "download", "ifc_message" => "display", "object" => "download://report.pdf"]);
```

---

### `download`

**Purpose:**
Initiates a file download for the selected object.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Download object identifier. |

**Inner Mechanism:**
- Resolves the physical file path from the object identifier.
- Uses the platform's `download()` function to send the file to the client.
- Terminates script execution after download.

**Usage Example:**
```php
// Direct download link (e.g., in an email)
cms_url(["ifc_page" => "download", "ifc_message" => "download", "object" => "download://invoice.pdf"]);
```

---

### `upload`

**Purpose:**
Displays a form for uploading a new downloadable file.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Base object identifier (for multilingual support). |
| `$language` | `string` | Current language. |

**Output:**
- IFC form with fields for:
  - Name (text input)
  - Description (rich text editor)
  - File (file upload)
  - Category (dropdown)

**Inner Mechanism:**
- Uses `ifc->set()` to define form fields.
- Auto-triggers file input click via JavaScript.

**Usage Example:**
```php
// Open upload form
cms_url(["ifc_page" => "download", "ifc_message" => "upload"]);
```

---

### `_upload` (Handler for `upload` submission)

**Purpose:**
Processes the uploaded file and saves metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `string` | Temporary file path. |
| `$ifc_file1_name` | `string` | Original filename. |
| `$ifc_param1` | `string` | Name. |
| `$ifc_param2` | `string` | Description. |
| `$ifc_param3` | `string` | Category. |

**Return:**
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Inner Mechanism:**
- Calls `$download->add()` to store the file and metadata.
- Updates the object identifier for multilingual support.

**Usage Context:**
- Triggered automatically after form submission.

---

### `upload_multi`

**Purpose:**
Displays a form for uploading multiple files at once.

**Output:**
- IFC form with:
  - Info message
  - Multi-file upload field
  - Category dropdown

**Usage Example:**
```php
// Open multi-upload form
cms_url(["ifc_page" => "download", "ifc_message" => "upload_multi"]);
```

---

### `_upload_multi` (Handler for `upload_multi` submission)

**Purpose:**
Processes multiple uploaded files.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `array` | Array of temporary file paths. |
| `$ifc_file1_name` | `array` | Array of original filenames. |
| `$ifc_param1` | `string` | Category. |

**Return:**
- `CMS_MSG_DONE` if at least one file succeeds.
- `CMS_MSG_ERROR` if all files fail.

**Inner Mechanism:**
- Uses `array_combine()` to pair files with filenames.
- Processes each file via `$download->add()`.

---

### `edit`

**Purpose:**
Displays a form to edit metadata for an existing download.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Download object identifier. |
| `$language` | `string` | Current language. |

**Output:**
- IFC form with pre-filled fields for:
  - Name
  - Description
  - Category

**Usage Example:**
```php
// Edit metadata for a download
cms_url(["ifc_page" => "download", "ifc_message" => "edit", "object" => "download://guide.pdf"]);
```

---

### `_edit` (Handler for `edit` submission)

**Purpose:**
Saves edited metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name. |
| `$ifc_param2` | `string` | Description. |
| `$ifc_param3` | `string` | Category. |

**Return:**
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Inner Mechanism:**
- Calls `$download->set()` to update metadata.

---

### `replace`

**Purpose:**
Displays a form to replace the file for an existing download.

**Output:**
- IFC form with a file upload field.
- Auto-submits on file selection via JavaScript.

**Usage Example:**
```php
// Replace the file for a download
cms_url(["ifc_page" => "download", "ifc_message" => "replace", "object" => "download://old.pdf"]);
```

---

### `_replace` (Handler for `replace` submission)

**Purpose:**
Processes the file replacement.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `string` | Temporary file path. |
| `$ifc_file1_name` | `string` | Original filename. |

**Return:**
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Inner Mechanism:**
- Calls `$download->replace()` to update the file.

---

### `delete`

**Purpose:**
Deletes selected downloads and updates the UI.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Comma-separated list of object identifiers. |

**Return:**
- `CMS_MSG_DONE` if all deletions succeed.
- `CMS_MSG_ERROR` if any deletion fails.

**Inner Mechanism:**
- Splits multilingual objects using `language_get()`.
- Calls `$download->unlink()` for each object.
- Updates the selection to the next available download in the same category.

---

### `category_rename`

**Purpose:**
Displays a form to rename a category.

**Output:**
- IFC form with a text input for the new category name.

**Usage Example:**
```php
// Rename a category
cms_url(["ifc_page" => "download", "ifc_message" => "category_rename", "object" => "download://file.pdf"]);
```

---

### `_category_rename` (Handler for `category_rename` submission)

**Purpose:**
Updates all downloads in the category with the new name.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New category name. |

**Return:**
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Inner Mechanism:**
- Iterates through all downloads and updates the `category` field if it matches the old name.
- Calls `$download->data->save()` to persist changes.

---

## Main Display Logic

### Overview
The main display renders a two-pane interface:
1. **Left Pane:** Category filter and download list.
2. **Right Pane:** Embedded iframe showing the selected download's details.

### Key Components

#### Category Filter
- Dropdown populated by `download_get_array()`.
- On change, triggers `ifc_post('select', value)` to update the category.

#### Download List
- Checkbox list of downloads in the current category.
- Supports select-all, invert, and clear actions.
- Double-click or "Apply" button to select a download.

#### Language Selector
- Only shown if `CMS_LANGUAGE_ENABLED` is `TRUE`.
- Updates the language context for metadata.

#### Download Display
- Embedded iframe showing the `display` message for the selected download.

### JavaScript Helpers

#### `download_select(value)`
Updates the selected download and refreshes the iframe.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `value` | `string` | Download object identifier. |

**Usage Example:**
```javascript
// Called when a download is selected from the list
download_select("download://example.pdf");
```

---

## Helper Functions

### `download_get_array()`

**Purpose:**
Retrieves a structured array of all downloads, grouped by category.

**Return:**
- `array`: Associative array where keys are category names and values are arrays of download objects.

**Usage Example:**
```php
$categories = download_get_array();
// Output: ["Documents" => ["download://file1.pdf", "download://file2.pdf"], ...]
```

### `download_get_select()`

**Purpose:**
Generates a dropdown list of all categories for use in forms.

**Return:**
- `string`: HTML `<select>` element with category options.

**Usage Example:**
```php
$categoryDropdown = download_get_select();
```

---

## Class: `download`

While not defined in this file, the `download` class is central to the interface's functionality. Key methods used:

| Method | Purpose |
|--------|---------|
| `$download->data->get($object, $key)` | Retrieves metadata (e.g., name, description) for a download. |
| `$download->data->set($value, $object, $key)` | Updates metadata. |
| `$download->add($file, $filename, $name, $description, $category)` | Adds a new download. |
| `$download->replace($object, $file, $filename)` | Replaces the file for an existing download. |
| `$download->unlink($object)` | Deletes a download. |
| `$download->data->save()` | Persists metadata changes to storage. |

---

## Usage Scenarios

### 1. Uploading a New Download
```php
// Step 1: Open the upload form
cms_url(["ifc_page" => "download", "ifc_message" => "upload"]);

// Step 2: User fills the form and submits
// Handled by `_upload` message
```

### 2. Editing Download Metadata
```php
// Step 1: Select a download and open the edit form
cms_url(["ifc_page" => "download", "ifc_message" => "edit", "object" => "download://manual.pdf"]);

// Step 2: User updates fields and submits
// Handled by `_edit` message
```

### 3. Downloading a File
```php
// Direct link for users
cms_url(["ifc_page" => "download", "ifc_message" => "download", "object" => "download://report.pdf"]);
```

### 4. Renaming a Category
```php
// Step 1: Select a download in the category and open the rename form
cms_url(["ifc_page" => "download", "ifc_message" => "category_rename", "object" => "download://file1.pdf"]);

// Step 2: User enters new name and submits
// Handled by `_category_rename` message
```

### 5. Embedding a Download Link in Content
```php
// Generate a URL for a download
$url = translate_url("download://brochure.pdf");
echo('<a href="' . x($url) . '">Download Brochure</a>');
```

---

## Security Considerations

1. **Permission Checks:**
   - `ifc_permission()` ensures users have `CMS_L_ACCESS` or `CMS_L_OPERATOR` rights.
   - `cms_permission("download")` checks for download rights before showing the download button.

2. **Input Sanitization:**
   - All user input is escaped using `x()`, `q()`, or `sqlesc()` before output or database operations.

3. **CSRF Protection:**
   - The IFC system includes CSRF tokens in forms and URL generation.

4. **File Access:**
   - Files are stored outside the web root (`CMS_DATA_PATH . "#download/"`).
   - Direct access is prevented; downloads are served via the `download()` function.

---

## Multilingual Support

- **Object Identifiers:** Use `language_get()` and `language_set()` to handle per-language metadata.
- **Fallbacks:** If no language-specific metadata exists, the system falls back to the base object.
- **UI:** Language selector allows switching between enabled languages (`CMS_LANGUAGE_ENABLED`).

**Example:**
```php
// Set metadata for English and German
$object = "download://manual.pdf";
language_set($object, "Manual (EN)", "en");
language_set($object, "Handbuch (DE)", "de");
```

---

## Performance Optimizations

1. **Caching:**
   - Selected object and category are cached in `cms_cache()` to avoid repeated lookups.
   - Cache keys are user-specific (`download.{CMS_USER}.object`).

2. **Lazy Loading:**
   - The download display iframe is only loaded when a download is selected.

3. **Efficient Data Access:**
   - The `download` class uses optimized data structures for metadata retrieval.


<!-- HASH:02d0aaa400a610968d4b62d17ae45781 -->
