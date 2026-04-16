# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.template.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Template Interface (`ifc.template.inc`)

This file provides the interface for managing templates within the NUOS platform. It handles template selection, creation, editing, deletion, and source code management. The interface supports multilingual templates, categorization, and integration with other modules (e.g., content, image, media) for dynamic code insertion.

---

### **Constants and Variables**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$object` | `NULL` | Current template identifier. Persisted in cache. |
| `$language` | `NULL` | Language context for multilingual templates. |
| `$category` | `NULL` | Template category for grouping. |
| `$template` | `new template()` | Instance of the `template` class for operations. |

---

### **Message Handling / Sub-Display**

The interface processes messages via `CMS_IFC_MESSAGE` to perform specific actions.

#### **`case "select"`**
**Purpose:**
Updates the current template object based on user selection and language context.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Selected template identifier. |
| `$language` | `string` | Language code (e.g., `en`, `de`). |

**Mechanism:**
- Uses `language_set()` to associate the selected template with the current language.
- Updates the cached object for the current user.

**Usage:**
Triggered when a user selects a template from the category list.

---

#### **`case "select_language"`**
**Purpose:**
Switches the active language context for template operations.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Language code (e.g., `en`, `de`). |

**Mechanism:**
- Updates the `$language` variable to the selected language.

**Usage:**
Triggered when a user clicks a language selector.

---

#### **`case "display"`**
**Purpose:**
Renders a preview of the selected template in an iframe.

**Mechanism:**
- Instantiates the `template` class.
- Caches the current template object and category for the user.
- Calls `template_preview()` to generate the preview.
- Exits to prevent further processing.

**Usage:**
Triggered when a template is selected for viewing.

---

#### **`case "add"` / `case "edit"`**
**Purpose:**
Displays forms for adding or editing a template.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Template name (pre-filled for edit). |
| `$ifc_param2` | `bool` | Page flag (pre-filled for edit). |
| `$flag` | `bool` | `TRUE` if editing, `FALSE` if adding. |

**Mechanism:**
- Initializes the `ifc` (Interface Control) class for form rendering.
- Pre-fills form fields for editing (name, page flag).
- Provides options for:
  - Keeping existing code (edit only).
  - Retrieving code from a URL.
  - Uploading code from a file.

**Usage:**
Triggered when a user initiates template creation or modification.

---

#### **`case "_add"` / `case "_edit"`**
**Purpose:**
Processes form submissions for adding or editing templates.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Template name. |
| `$ifc_param2` | `bool` | Page flag. |
| `$ifc_param3` | `int` | Code source (1: keep, 2: URL, 3: file). |
| `$ifc_param4` | `string` | URL for code retrieval. |
| `$ifc_param5` | `string` | Template category. |
| `$ifc_file1` | `string` | Uploaded file path. |
| `$ifc_file1_name` | `string` | Uploaded file name. |

**Mechanism:**
- Determines the source of the template code (keep, URL, or file).
- For URLs, extracts the basename for the template name if not provided.
- For files, reads the content and deletes the temporary file.
- Calls `template->add()` or `template->set()` to save the template.
- Updates the response message (`CMS_MSG_DONE` or `CMS_MSG_ERROR`).

**Usage:**
Triggered on form submission for template creation/modification.

---

#### **`case "export"` / `case "_export"`**
**Purpose:**
Exports content as a template.

**Parameters (Export Form):**
| Name | Type | Description |
|------|------|-------------|
| `$content_index` | `string` | Content identifier. |
| `$content_range` | `string` | Range of content items (optional). |

**Parameters (Export Submission):**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Template name. |
| `$ifc_param2` | `string` | Template category. |

**Mechanism:**
- **Form Display:** Renders a form for selecting a name and category.
- **Submission:** Uses `content_template_export()` to generate template code and stylesheet from content.
- Saves the template using `template->add()`.
- Updates the response message.

**Usage:**
Triggered when exporting content (e.g., articles) as reusable templates.

---

#### **`case "source"` / `case "_source"` / `case "__source"` / `case "___source"`**
**Purpose:**
Manages the template source code editor.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Template code. |
| `$ifc_param2` | `string` | Stylesheet code. |

**Mechanism:**
- **Display:** Renders a code editor with tabs for template and stylesheet.
- **Tools:** Provides buttons for inserting dynamic elements (e.g., images, media, CMS tags).
- **JavaScript:** Handles saving (`template_source_save()`) and inserting prefabs (`template_source_insert()`).
- **Prefabs:** Dropdown menu for inserting common CMS tags (e.g., `<CMS:menu>`, `<CMS:image>`).
- **Saving:** Updates the template code and stylesheet via `template->set_code()` and `template->set_stylesheet()`.

**Usage:**
Triggered when editing template source code.

---

#### **`case "delete"`**
**Purpose:**
Deletes selected templates.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$_object` | `array` | Array of template identifiers to delete. |

**Mechanism:**
- Deletes each template using `template->delete()`.
- Updates the selection to the next available template in the same category.
- Removes invalid templates from multilingual selections.
- Updates the response message.

**Usage:**
Triggered when a user confirms template deletion.

---

#### **`case "category_rename"` / `case "_category_rename"`**
**Purpose:**
Renames a template category.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New category name. |

**Mechanism:**
- **Form Display:** Renders a form for entering the new category name.
- **Submission:** Updates all templates in the category with the new name.
- Saves changes using `template->data->save()`.

**Usage:**
Triggered when renaming a category.

---

### **Main Display**

**Purpose:**
Renders the primary interface for template management.

**Mechanism:**
- **Category Selection:** Dropdown for selecting a template category.
- **Template List:** Checkbox list of templates in the selected category.
- **Language Selector:** Buttons for switching languages (if multilingual support is enabled).
- **Preview:** Iframe for displaying the selected template.
- **Menu:** Contextual actions (add, edit, delete, rename, source code).

**JavaScript:**
- `template_select()`: Updates the preview when a template is selected.

**Usage:**
Default view for the template interface.

---

### **Helper Functions**

#### **`template_get_select()`**
**Purpose:**
Generates a dropdown list of template categories.

**Return Value:**
`string` – HTML `<select>` element.

**Usage:**
Used in forms for selecting a template category.

#### **`template_get_array()`**
**Purpose:**
Retrieves an associative array of templates grouped by category.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$filter` | `bool` | `FALSE` to include all templates, `NULL` to exclude pages. |

**Return Value:**
`array` – Associative array of templates (e.g., `["Category" => ["template1", "template2"]]`).

**Usage:**
Populates the category and template lists.

---

### **Integration with Other Modules**
The interface provides buttons for inserting dynamic elements from other modules:
- **Directory:** Inserts links to directory items.
- **Image:** Inserts `<CMS:image>` tags.
- **Media:** Inserts `<CMS:media>` tags.
- **Download:** Inserts `<CMS:download>` tags.
- **Token:** Inserts tokens for form handling.

Each button opens a modal for selecting the item and inserts the corresponding code into the editor.


<!-- HASH:fa5c55a6922b8adc25602d91a246ad4f -->
