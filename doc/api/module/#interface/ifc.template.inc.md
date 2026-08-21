# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.template.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.template.inc)

- **Version:** `26.8.8.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template Interface (`ifc.template.inc`)

This file implements the **Template Management Interface** for the PWNC Web Platform. It provides a user interface for creating, editing, previewing, exporting, and deleting templates, as well as managing template categories and source code. The interface supports multilingual templates and integrates with other modules (e.g., content, image, media) for seamless asset embedding.

---

### **Overview**
The interface handles the following key operations:
- **Template Selection & Preview**: Browse and preview templates by category.
- **Template Management**: Add, edit, delete, and rename templates/categories.
- **Source Code Editing**: Modify template code, stylesheets, and JavaScript with built-in prefabs and asset insertion tools.
- **Content Export**: Export content as reusable templates.
- **Multilingual Support**: Manage templates across multiple languages.

---

### **Key Functions & Workflows**

#### **1. Initialization & Permissions**
- **Purpose**: Loads required libraries, checks user permissions, and initializes the interface state.
- **Mechanism**:
  - Verifies the `template` library is loaded; otherwise, deactivates the interface.
  - Checks user permissions (`CMS_L_ACCESS` for basic access, `CMS_L_OPERATOR` for editing).
  - Retrieves the selected template from the cache (`template.{USER}.object`).
  - Initializes language settings.

---

#### **2. Message Handling (`CMS_IFC_MESSAGE` Switch)**
The interface processes user actions via the `CMS_IFC_MESSAGE` switch, which routes to specific workflows.

---

##### **`select`**
- **Purpose**: Updates the selected template based on user input.
- **Parameters**:
  | Name          | Type     | Description                          |
  |---------------|----------|--------------------------------------|
  | `$object`     | `string` | Template identifier (multilingual).  |
  | `$ifc_param`  | `string` | New template identifier.             |
  | `$language`   | `string` | Current language.                    |
- **Mechanism**: Calls `language_set()` to update the selected template in the current language.

**Example**:
```php
// User selects a template from a dropdown; the interface updates the selection.
ifc_post("select", "homepage_en");
```

---

##### **`select_language`**
- **Purpose**: Switches the active language for template management.
- **Parameters**:
  | Name        | Type     | Description               |
  |-------------|----------|---------------------------|
  | `$language` | `string` | New language identifier.  |
- **Mechanism**: Updates the `$language` variable and refreshes the interface.

**Example**:
```php
// User clicks a language flag; the interface switches to French.
ifc_post("select_language", "fr");
```

---

##### **`display` / `_display`**
- **Purpose**: Renders a template preview in an iframe.
- **Parameters**:
  | Name      | Type      | Description                          |
  |-----------|-----------|--------------------------------------|
  | `$object` | `string`  | Template identifier.                 |
  | `TRUE`    | `bool`    | (For `_display`) Forces raw preview. |
- **Mechanism**:
  - Caches the selected template and category.
  - Calls `template_preview()` to render the template.
  - For `_display`, skips UI elements and outputs raw HTML.

**Example**:
```php
// Preview the "homepage" template.
cms_url(["ifc_page" => "template", "ifc_message" => "display", "object" => "homepage_en"]);
```

---

##### **`add` / `edit` / `_add` / `_edit`**
- **Purpose**: Adds or edits a template.
- **Parameters**:
  | Name          | Type      | Description                          |
  |---------------|-----------|--------------------------------------|
  | `$ifc_param1` | `string`  | Template name.                       |
  | `$ifc_param2` | `bool`    | Whether the template is a page.      |
  | `$ifc_param3` | `int`     | Code source (1: empty, 2: URL, 3: file). |
  | `$ifc_param4` | `string`  | URL (if `$ifc_param3 = 2`).          |
  | `$ifc_param5` | `string`  | Category.                            |
  | `$ifc_file1`  | `string`  | Uploaded file path (if `$ifc_param3 = 3`). |
- **Mechanism**:
  - For `edit`, pre-fills form fields with existing data.
  - For `_add`/`_edit`, processes the code source (URL/file/upload) and saves the template.
  - Uses the `template` class to persist changes.

**Example**:
```php
// Add a new template from a URL.
$template = new template();
$template->add("about_us", "pages", true, template_read_plugin("https://example.com/template.htm"));
```

---

##### **`export` / `_export`**
- **Purpose**: Exports content as a reusable template.
- **Parameters**:
  | Name              | Type      | Description                          |
  |-------------------|-----------|--------------------------------------|
  | `$content_index`  | `string`  | Content identifier.                  |
  | `$content_range`  | `string`  | Range of content items (optional).   |
  | `$ifc_param1`     | `string`  | Template name.                       |
  | `$ifc_param2`     | `string`  | Category.                            |
- **Mechanism**:
  - Uses the `content` module to generate template code, stylesheets, and JavaScript.
  - Saves the exported data as a new template.

**Example**:
```php
// Export a content block as a template.
content_template_export($content, "news_articles", "1-5");
```

---

##### **`source` / `_source` / `__source` / `___source`**
- **Purpose**: Edits template source code, stylesheets, and JavaScript.
- **Parameters**:
  | Name          | Type     | Description                          |
  |---------------|----------|--------------------------------------|
  | `$ifc_param1` | `string` | Template code.                       |
  | `$ifc_param2` | `string` | Stylesheet.                          |
  | `$ifc_param3` | `string` | JavaScript.                          |
- **Mechanism**:
  - Provides a code editor with prefabs (e.g., `<CMS:menu>`, `<CMS:text>`).
  - Integrates with other modules (e.g., image, media) to insert assets via JavaScript callbacks.
  - For `_source`, saves changes and exits; for `__source`/`___source`, saves and continues editing.

**Example**:
```javascript
// Insert a media asset into the template code.
textcontrol_set("#l_ifc_param1", "#insert", "<CMS:media default=\"video.mp4\"/>");
```

---

##### **`delete`**
- **Purpose**: Deletes selected templates.
- **Parameters**:
  | Name      | Type     | Description                          |
  |-----------|----------|--------------------------------------|
  | `$object` | `string` | Comma-separated template identifiers. |
- **Mechanism**:
  - Deletes templates across all languages.
  - Updates the selection to the next available template in the same category.

**Example**:
```php
// Delete the "old_homepage" template.
$template->delete("old_homepage_en");
```

---

##### **`category_rename` / `_category_rename`**
- **Purpose**: Renames a template category.
- **Parameters**:
  | Name          | Type     | Description                          |
  |---------------|----------|--------------------------------------|
  | `$ifc_param1` | `string` | New category name.                   |
- **Mechanism**:
  - Updates the category for all templates in the current category.

**Example**:
```php
// Rename category "old" to "legacy".
$template->data->set("legacy", $key, "category");
```

---

### **Main Display Workflow**
- **Purpose**: Renders the primary interface for template management.
- **Mechanism**:
  1. **Category/Template Selection**:
     - Displays a dropdown for categories and a list of templates.
     - Supports multilingual selection via language flags.
  2. **Preview Pane**:
     - Shows the selected template in an iframe.
  3. **Menu Actions**:
     - Add/Edit/Delete templates, rename categories, or open the source editor.

**Example**:
```php
// Render the template management interface.
$ifc = new ifc($ifc_response, "template", $menu, ["object" => "homepage_en"]);
```

---

### **Helper Functions**

#### **`template_get_select()`**
- **Purpose**: Generates a dropdown of template categories.
- **Return**: `string` HTML `<select>` element.
- **Usage**:
  ```php
  $ifc->set(template_get_select(), "list 40", "pages");
  ```

#### **`template_preview($object, $raw = FALSE, $title = NULL, $sandbox = TRUE)`**
- **Purpose**: Renders a template preview.
- **Parameters**:
  | Name      | Type      | Description                          |
  |-----------|-----------|--------------------------------------|
  | `$object` | `string`  | Template identifier.                 |
  | `$raw`    | `bool`    | If `TRUE`, outputs raw HTML.         |
  | `$title`  | `string`  | Custom title (optional).             |
  | `$sandbox`| `bool`    | If `TRUE`, enables iframe sandboxing.|
- **Usage**:
  ```php
  template_preview("homepage_en", true);
  ```

#### **`template_read_plugin($url)`**
- **Purpose**: Fetches template code from a URL.
- **Parameters**:
  | Name  | Type     | Description       |
  |-------|----------|-------------------|
  | `$url`| `string` | Remote URL.       |
- **Return**: `string` Template code.
- **Usage**:
  ```php
  $code = template_read_plugin("https://example.com/template.htm");
  ```

---

### **JavaScript Functions**
- **`template_select(value)`**: Updates the selected template and refreshes the preview.
- **`template_source_save(message)`**: Saves template source code (e.g., `_source`, `__source`).
- **`template_source_insert(value)`**: Inserts prefab code snippets into the editor.

**Example**:
```javascript
// Save template code and close the editor.
template_source_save("_source");
```

---

### **Constants & Labels**
| Constant                          | Description                          |
|-----------------------------------|--------------------------------------|
| `CMS_L_IFC_TEMPLATE_001`          | "Retrieve from URL"                  |
| `CMS_L_IFC_TEMPLATE_002`          | "Empty template"                     |
| `CMS_L_IFC_TEMPLATE_003`          | "Upload file"                        |
| `CMS_L_IFC_TEMPLATE_005`          | "Category"                           |
| `CMS_L_IFC_TEMPLATE_006`          | "Edit source code"                   |
| `CMS_L_IFC_TEMPLATE_007`          | "Insert template"                    |
| `CMS_L_IFC_TEMPLATE_008`          | "Page template"                      |
| `CMS_L_IFC_TEMPLATE_009`          | "Your browser does not support iframes." |
| `CMS_L_IFC_TEMPLATE_011`          | "Insert prefab..."                   |
| `CMS_L_IFC_TEMPLATE_012`          | "Keep existing code"                 |
| `CMS_L_IFC_TEMPLATE_013`          | "Insert token"                       |
| `CMS_L_IFC_TEMPLATE_014`          | "Head"                               |
| `CMS_L_IFC_TEMPLATE_015`          | "Control"                            |
| `CMS_L_IFC_TEMPLATE_016`          | "Content block"                      |
| `CMS_L_IFC_TEMPLATE_017`          | "Content alternative"                |
| `CMS_L_IFC_TEMPLATE_018`          | "Repeat"                             |
| `CMS_L_IFC_TEMPLATE_019`          | "Template"                           |
| `CMS_L_IFC_TEMPLATE_020`          | "Text"                               |
| `CMS_L_IFC_TEMPLATE_021`          | "Link"                               |
| `CMS_L_IFC_TEMPLATE_022`          | "Image"                              |
| `CMS_L_IFC_TEMPLATE_023`          | "Thumbnail"                          |
| `CMS_L_IFC_TEMPLATE_024`          | "Media"                              |
| `CMS_L_IFC_TEMPLATE_025`          | "Download"                           |
| `CMS_L_IFC_TEMPLATE_026`          | "Plugin"                             |
| `CMS_L_IFC_TEMPLATE_027`          | "PHP"                                |
| `CMS_L_IFC_TEMPLATE_028`          | "Edit"                               |
| `CMS_L_IFC_TEMPLATE_029`          | "Comment"                            |
| `CMS_L_IFC_TEMPLATE_030`          | "Page"                               |
| `CMS_L_IFC_TEMPLATE_031`          | "Backlink"                           |
| `CMS_L_IFC_TEMPLATE_032`          | "HTML"                               |
| `CMS_L_IFC_TEMPLATE_033`          | "Include"                            |
| `CMS_L_IFC_TEMPLATE_034`          | "Rename category"                    |
| `CMS_L_IFC_TEMPLATE_035`          | "Shift"                              |
| `CMS_L_IFC_TEMPLATE_036`          | "No cache"                           |
| `CMS_L_IFC_TEMPLATE_037`          | "Group"                              |
| `CMS_L_IFC_TEMPLATE_038`          | "Namespace"                          |
| `CMS_L_IFC_TEMPLATE_039`          | "Base"                               |
| `CMS_L_IFC_TEMPLATE_040`          | "Value"                              |
| `CMS_L_IFC_TEMPLATE_041`          | "Stylesheet"                         |
| `CMS_L_IFC_TEMPLATE_042`          | "Insert directory link"              |
| `CMS_L_IFC_TEMPLATE_043`          | "Echo"                               |
| `CMS_L_IFC_TEMPLATE_044`          | "Show"                               |
| `CMS_L_IFC_TEMPLATE_045`          | "Menu"                               |
| `CMS_L_IFC_TEMPLATE_046`          | "Insert download"                    |
| `CMS_L_IFC_TEMPLATE_047`          | "Insert directory (code)"            |
| `CMS_L_IFC_TEMPLATE_048`          | "Insert image (code)"                |
| `CMS_L_IFC_TEMPLATE_049`          | "Insert media"                       |
| `CMS_L_IFC_TEMPLATE_050`          | "PHP/Code"                           |
| `CMS_L_IFC_TEMPLATE_051`          | "CMS Tags"                           |
| `CMS_L_IFC_TEMPLATE_052`          | "Switch"                             |
| `CMS_L_IFC_TEMPLATE_053`          | "No edit"                            |
| `CMS_L_IFC_TEMPLATE_054`          | "Insert stylesheet"                  |
| `CMS_L_IFC_TEMPLATE_055`          | "JavaScript"                         |
| `CMS_L_IFC_TEMPLATE_056`          | "Insert JavaScript"                  |

---

### **Usage Example**
```php
// 1. Load the template interface.
load_page("template");

// 2. Add a new template from a file upload.
$template = new template();
$code = read_file("/path/to/template.htm");
$template->add("contact_us", "pages", true, $code);

// 3. Edit the template's source code.
$ifc = new ifc($response, "template", [], ["object" => "contact_us_en"], "source");
$ifc->set(NULL, "code 80x20 fl", $template->get_code("contact_us_en"));
```

---

### **Key Integration Points**
- **Multilingual Support**: Uses `language_get()`/`language_set()` to manage templates across languages.
- **Asset Embedding**: Integrates with `image`, `media`, `download`, and `directory` modules for dynamic asset insertion.
- **Content Export**: Leverages the `content` module to convert content into templates.
- **Caching**: Uses `cms_cache()` to persist user selections (e.g., selected template/category).


<!-- HASH:44a8f78d8dda3dcced3d1a3a57cd74b5 -->
