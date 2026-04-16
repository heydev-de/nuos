# NUOS API Documentation

[← Index](../README.md) | [`module/content.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Content Module (`module/content.php`)

The **Content Module** is the core entry point for serving, displaying, and editing content in the NUOS platform. It handles:

- **Content resolution** (URL-to-content mapping via directory references)
- **Permission checks** (read/write access)
- **Caching** (ETag-based HTTP caching for published content)
- **Edit mode** (interactive content manipulation via JavaScript)
- **Command processing** (apply/revert/undo/redo operations)
- **Dynamic partial updates** (AJAX-based buffer extraction)

---

### **Constants & Global Variables**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$content_display` | Global | Determines if content is displayed as a directory or single item. |
| `$content_index` | Global | Unique identifier of the content being accessed. |
| `$content_directory_index` | Global | Unique identifier of the directory entry (used for URL resolution). |
| `$content_option` | Global | Bitmask of template options (e.g., `CMS_TEMPLATE_OPTION_TEXT`, `CMS_TEMPLATE_OPTION_IMAGE`). |
| `$content_user` | Global | User identifier for permission checks. |
| `$content_message` | Global | Command to execute (e.g., `"apply"`, `"revert"`). |
| `$range` | Global | Path identifier for partial content updates. |
| `$type` | Global | Type of edit operation (e.g., `"value"`, `"image"`, `"#buffer"`). |
| `$value` | Global | New value for the edit operation. |
| `$left` / `$top` | Global | Scroll position (percentage) for edit mode. |
| `$id` | Global | DOM element identifier for JavaScript interactions. |

---

### **Core Logic Flow**

#### **1. Custom 404 Handling**
- Checks for `REDIRECT_STATUS=404` and skips custom error pages for static assets (e.g., images, CSS, JS).
- **Usage**: Prevents unnecessary 404 pages for missing static files.

#### **2. Library Loading & Content Initialization**
- Loads `content` and `template` libraries.
- Instantiates the `content` object and checks if the module is enabled.
- **Failure**: Returns HTTP 500 if libraries fail to load or the module is disabled.

#### **3. Directory Reference Resolution**
- Resolves logical URLs (e.g., `directory://123`) into physical content or external URLs.
- **Redirects**:
  - **Language Gate**: Redirects to the correct language version if `CMS_LANGUAGE_ENABLED` is set.
  - **Homepage**: Redirects to the first directory entry if no `content_directory_index` is provided.
  - **External References**: Redirects to absolute URLs (e.g., `https://example.com`).
- **Loops**: Detects circular references (e.g., `directory://1` → `directory://2` → `directory://1`) and returns HTTP 508.

#### **4. Partial Content Buffer Extraction**
- Triggered by JavaScript when `$type = "#buffer"`.
- Extracts a specific range from the content buffer and caches it permanently.
- **Usage**: Enables dynamic updates of content fragments without full page reloads.

#### **5. Permission Verification**
- Checks if the user has read access (`CMS_CONTENT_PERMISSION_READER . ".$content_index`).
- **Failure**: Redirects to the login page (`identification.php`) with a `location` parameter.

#### **6. Content Status Check**
- Retrieves the content status from the database.
- **Failure**: Returns HTTP 404 if the content does not exist.
- **Unpublished Content**: Returns HTTP 410 if the content is not in `CMS_CONTENT_STATUS_PUBLICATION` state.

#### **7. Read Mode (Published Content)**
- Logs access via the `log` object.
- Generates output using `content_parse()` and checks for dynamic parts.
- **Caching**:
  - Sends `ETag` and `Last-Modified` headers for cache validation.
  - Returns HTTP 304 if the content has not been modified.
- **Performance Metrics**: Outputs generation time in milliseconds.

#### **8. Edit Mode (Interactive Editing)**
- **Command Processing**: Handles `apply`, `revert`, `undo`, and `redo` commands.
- **Range Updates**: Calls `content_set_range()` to update specific content fragments.
- **Action Setup**: Configures JavaScript functions for template controls (e.g., copy/paste, drag-and-drop).
- **Module Integration**: Adds edit functions for supported modules (e.g., `image`, `media`, `download`).

---

### **Key Functions & Methods**

#### **`content_parse()`**
- **Purpose**: Renders content into HTML, applying template actions and controls.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$content` | `content` | Content object instance. |
  | `$content_index` | `int` | Content identifier. |
  | `$action` | `array\|NULL` | Template actions and controls. |
  | `$header` | `string` | Additional HTML headers (e.g., JavaScript). |
- **Return**: `string` (Rendered HTML output).
- **Usage**: Called in both read and edit modes to generate the final output.

#### **`content_set_range()`**
- **Purpose**: Updates a specific range of content with a new value.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$content` | `content` | Content object instance. |
  | `$content_index` | `int` | Content identifier. |
  | `$range` | `string` | Path to the content fragment. |
  | `$type` | `string` | Type of update (e.g., `"value"`, `"image"`). |
  | `$value` | `mixed` | New value for the fragment. |
- **Usage**: Called in edit mode to apply changes from user interactions.

#### **`analyze_url()`**
- **Purpose**: Parses a URL into its components (scheme, host, query, etc.).
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$url` | `string` | URL to analyze. |
- **Return**: `array\|FALSE` (URL components or `FALSE` if invalid).
- **Usage**: Used during directory reference resolution to handle logical URLs (e.g., `content://123`).

---

### **Template Actions & Controls**

The `$action` array configures JavaScript functions for interactive editing. It is structured as follows:

| Key | Sub-Key | Description |
|-----|---------|-------------|
| `CMS_TEMPLATE_CONTROL` | `CMS_TEMPLATE_SWITCH` | Option selector (e.g., template switching). |
| `CMS_TEMPLATE_CONTROL` | `CMS_TEMPLATE_COMMAND` | Global commands (e.g., undo, redo, logout). |
| `CMS_TEMPLATE_ACTION` | `CMS_TEMPLATE_TYPE_*` | Type-specific actions (e.g., text, image, download). |

#### **Example Actions**
| Action | Description |
|--------|-------------|
| `CMS_TEMPLATE_COMMAND_PASTE` | Pastes copied content into a new location. |
| `CMS_TEMPLATE_ACTION[CMS_TEMPLATE_TYPE_IMAGE]` | Opens the image editor for a specific fragment. |
| `CMS_TEMPLATE_COMMAND[CMS_L_MOD_CONTENT_015]` | Logs the user out. |

---

### **JavaScript Integration**

#### **`content.js`**
- **Purpose**: Handles interactive editing (e.g., drag-and-drop, copy/paste, undo/redo).
- **Key Functions**:
  - `content_edit_apply()`: Applies pending changes.
  - `content_edit_revert()`: Reverts pending changes.
  - `content_edit_copy()`: Copies a content fragment to the buffer.
  - `content_edit_paste()`: Pastes the buffer into a new location.
  - `content_edit_switch()`: Toggles a switch-type fragment (e.g., conditional blocks).

#### **`asr.js`**
- **Purpose**: Provides drag-and-drop functionality for template elements.
- **Usage**: Registered for elements with `data-tp-dd-type` and `data-tp-dd-accept` attributes.

---

### **Usage Scenarios**

#### **1. Serving Published Content**
- **URL**: `https://example.com/content.php?content_index=123`
- **Behavior**:
  - Checks read permissions.
  - Validates content status.
  - Generates HTML with caching headers.
  - Logs access.

#### **2. Editing Content**
- **URL**: `https://example.com/content.php?content_index=123&content_option=15`
- **Behavior**:
  - Verifies write permissions.
  - Sets up edit controls (e.g., text/image editors, undo/redo buttons).
  - Processes commands (e.g., `apply`, `revert`).

#### **3. Directory Resolution**
- **URL**: `https://example.com/content.php?content_directory_index=home`
- **Behavior**:
  - Resolves `directory://home` to a physical URL or content index.
  - Redirects to the resolved URL (e.g., `content://123` or `https://example.com`).

#### **4. Partial Updates (AJAX)**
- **URL**: `https://example.com/content.php?type=#buffer&range=section1`
- **Behavior**:
  - Extracts `section1` from the content buffer.
  - Returns the fragment as a cached response.

---

### **Error Handling**

| HTTP Code | Scenario |
|-----------|----------|
| **404** | Content not found or invalid directory reference. |
| **410** | Content exists but is not published. |
| **500** | Module or library loading failed. |
| **508** | Circular directory reference detected. |
| **301/302** | Redirects for language gates, homepage, or external references. |
| **303** | Redirect after command processing (e.g., `apply`). |
| **304** | Cached content has not been modified. |

---


<!-- HASH:cdb05fc64c1f4ec58798baaada37110d -->
