# PWNC API Documentation

[← Index](../README.md) | [`module/content.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/content.php)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Module (`module/content.php`)

The **Content Module** is the core component of the PWNC Web Platform responsible for handling content delivery, rendering, and editing. It manages:

- **Content retrieval** (published and draft states)
- **Permission checks** (read/write access)
- **URL resolution** (directory redirection, internal/external references)
- **Editing interface** (inline editing, command processing, and template interactions)
- **Caching** (ETag-based HTTP caching for performance)
- **Dynamic content updates** (via JavaScript-driven partial updates)

This module acts as the primary entry point for content requests, whether for viewing or editing, and integrates with other modules (e.g., `template`, `interface`, `download`, `image`, `media`) to provide a unified editing experience.

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$content_display` | `string` | Determines the display mode (e.g., `"directory"`). |
| `$content_index` | `int` | Unique identifier of the content being accessed. |
| `$content_directory_index` | `string` | Identifier for directory-based content routing. |
| `$content_option` | `int` | Bitmask of editing options (e.g., `CMS_TEMPLATE_OPTION_TEXT`, `CMS_TEMPLATE_OPTION_IMAGE`). |
| `$content_user` | `int` | User ID for permission checks. |
| `$content_message` | `string` | Command to execute (e.g., `"apply"`, `"revert"`, `"undo"`, `"redo"`). |
| `$range` | `string` | Path to the specific content range being edited. |
| `$type` | `string` | Type of edit operation (e.g., `"text"`, `"image"`, `"#buffer"`). |
| `$value` | `mixed` | New value for the edited range. |
| `$left` / `$top` | `float` | Scroll position percentages for the editing interface. |
| `$id` | `string` | DOM element ID for targeted updates. |

---

### Key Workflows

#### 1. **404 Handling**
- Checks for `REDIRECT_STATUS=404` and skips custom error pages for static assets (e.g., images, CSS, JS).
- Static asset extensions are hardcoded in an array (e.g., `"avif"`, `"css"`, `"js"`).

#### 2. **Directory Resolution**
- If no `$content_index` is provided, resolves the request to a directory entry (e.g., homepage or language-specific redirect).
- Handles internal references (e.g., `content://123`) and external URLs (e.g., `https://example.com`).
- Detects circular references (e.g., `directory://A` → `directory://B` → `directory://A`) and returns HTTP 508 (Loop Detected).

#### 3. **Permission Checks**
- Validates read/write permissions using `cms_permission()`.
- Redirects unauthorized users to the login page (`identification.php`) with a `302` redirect.

#### 4. **Content Status Validation**
- Checks if content is published (`CMS_CONTENT_STATUS_PUBLICATION`) or draft.
- Returns HTTP 410 (Gone) for unpublished content in read mode.

#### 5. **Caching**
- Generates an `ETag` based on the last modification time (`$mod_time`).
- Returns HTTP 304 (Not Modified) if the client’s `If-None-Match` header matches the `ETag`.

#### 6. **Editing Mode**
- Processes commands (`apply`, `revert`, `undo`, `redo`) via `$content_message`.
- Generates a dynamic editing interface with action buttons (e.g., copy/paste, drag-and-drop, template edits).
- Uses `content_parse()` to render content with inline editing controls.

---

### Core Functions

#### `content_parse()`
*(Implicitly used; defined in `content.inc`)*
**Purpose**: Renders content (view or edit mode) by parsing the document structure and applying templates.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object instance. |
| `$content_index` | `int` | Content ID to render. |
| `$action` | `array\|null` | Editing actions (e.g., buttons, commands). |
| `$header` | `string` | Additional HTML/JS headers for edit mode. |
**Return**: `string` – Rendered HTML output.
**Usage Context**:
- Called in both read and edit modes to generate the final output.
- In edit mode, injects JavaScript event handlers and controls.

**Example**:
```php
// Render content in read mode
$output = content_parse($content, $content_index, NULL, CMS_BOT_CHECK, $is_dynamic, $mod_time);
echo($output);
```

---

### Helper Functions

#### `analyze_url()`
*(Implicitly used; defined in `pwnc.inc`)*
**Purpose**: Parses a URL into components (scheme, host, path, query).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL to parse. |
**Return**: `array\|false` – Associative array of URL components or `false` on failure.

**Example**:
```php
$_url = analyze_url("content://123?param=value");
/*
$_url = [
    "scheme" => "content",
    "host"   => "123",
    "query"  => "param=value"
]
*/
```

---

#### `import_querystring()`
*(Implicitly used; defined in `pwnc.inc`)*
**Purpose**: Merges a query string into the global `$_GET` state.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$query` | `string` | Query string (e.g., `"?key=value"`). |
**Return**: `void`

**Example**:
```php
import_querystring("?content_index=42&range=body");
```

---

#### `content_set_range()`
*(Implicitly used; defined in `content.inc`)*
**Purpose**: Updates a specific range of content (e.g., text, image, template).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object. |
| `$content_index` | `int` | Content ID. |
| `$range` | `string` | Path to the range. |
| `$type` | `string` | Type of update (e.g., `"text"`, `"image"`). |
| `$value` | `mixed` | New value for the range. |
**Return**: `void`

**Example**:
```php
content_set_range($content, 123, "body.paragraph1", "text", "New text content");
```

---

### Editing Actions

The module dynamically generates JavaScript actions for the editing interface. These are stored in the `$action` array and injected into the template. Key action types:

| Action Type | Description | Example |
|-------------|-------------|---------|
| `CMS_TEMPLATE_ACTION` | Inline edit buttons (e.g., text, image, template). | `content_edit_open()` for text edits. |
| `CMS_TEMPLATE_COMMAND` | Global commands (e.g., undo, redo, copy/paste). | `content_edit_apply()` for saving changes. |
| `CMS_TEMPLATE_SWITCH` | Dropdown selectors (e.g., template options). | `tp_ctrl_opt_apply()` for template switching. |

**Example Action Definition**:
```php
// Set up a text edit button
$action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_TEXT] = [
    CMS_TEMPLATE_CODE => "content_edit_open('" . q(cms_url($interface_url, [
        "ifc_page" => "content",
        "ifc_message" => "edit_range",
        "object" => $content_index,
        "range" => "\x1B%index%",
        "id" => "\x1B%id%"
    ], TRUE)) . "');",
    CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_text.svg"
];
```

---

### JavaScript Integration

The module outputs JavaScript to:
1. **Restore scroll position** (`tp_flp_restore`).
2. **Register drag-and-drop handlers** (`dd_register`).
3. **Highlight edited elements** (adds `tp-edited` class).
4. **Handle dynamic updates** (e.g., partial content buffers via `#buffer`).

**Example**:
```javascript
// Highlight edited elements on page load
var list = document.querySelectorAll("DIV[data-tp-path=\"" + q($value) + "\"]");
for (var i = 0; i < list.length; i++) list[i].className += " tp-edited";
```

---

### Usage Scenarios

#### 1. **Viewing Published Content**
- **URL**: `https://example.com/content.php?content_index=123`
- **Flow**:
  1. Validates permissions and content status.
  2. Logs access via `log->access()`.
  3. Renders content with `content_parse()`.
  4. Sends `ETag` and `Last-Modified` headers for caching.

#### 2. **Editing Content**
- **URL**: `https://example.com/content.php?content_index=123&content_option=15` (bitmask for text/image/template edits)
- **Flow**:
  1. Validates write permissions.
  2. Processes commands (e.g., `apply`, `undo`).
  3. Generates an editing interface with action buttons.
  4. Injects JavaScript for drag-and-drop, copy/paste, etc.

#### 3. **Directory Redirection**
- **URL**: `https://example.com/` (no `content_index`)
- **Flow**:
  1. Resolves the directory entry (e.g., homepage).
  2. Redirects to the canonical URL (e.g., `https://example.com/home`) with HTTP 301.

#### 4. **Partial Content Updates**
- **URL**: `https://example.com/content.php?content_index=123&type=#buffer&range=body.paragraph1`
- **Flow**:
  1. Fetches the buffer text and template from the database.
  2. Extracts the specified range using `document->extract()`.
  3. Returns the partial content for JavaScript updates.

---

### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_CONTENT_PERMISSION_READER` | `"content.reader"` | Permission prefix for read access. |
| `CMS_CONTENT_STATUS_PUBLICATION` | `2` | Status code for published content. |
| `CMS_TEMPLATE_OPTION_TEXT` | `1 << 0` | Bitmask for text editing. |
| `CMS_TEMPLATE_OPTION_IMAGE` | `1 << 1` | Bitmask for image editing. |
| `CMS_TEMPLATE_OPTION_TEMPLATE` | `1 << 2` | Bitmask for template editing. |
| `CMS_DB_CONTENT_INDEX` | `"content_index"` | Database column for content ID. |
| `CMS_DB_CONTENT_BUFFER_TEXT` | `"buffer_text"` | Database column for buffer text. |
| `CMS_DB_CONTENT_BUFFER_TEMPLATE` | `"buffer_template"` | Database column for buffer template. |

---

### Error Handling

| HTTP Code | Scenario |
|-----------|----------|
| `404` | Content not found or invalid directory reference. |
| `410` | Content exists but is not published. |
| `500` | Failed to load required libraries or instantiate the content object. |
| `508` | Circular reference detected in directory resolution. |
| `301` | Permanent redirect (e.g., directory resolution). |
| `302` | Temporary redirect (e.g., language gate or login redirect). |
| `303` | Redirect after command processing (e.g., `apply`, `undo`). |
| `304` | Not Modified (ETag match). |

---

### Dependencies

| Module | Purpose |
|--------|---------|
| `content.inc` | Core content logic (e.g., `content_parse()`, `content_set_range()`). |
| `template.inc` | Template rendering and editing. |
| `interface.php` | Modal dialogs for editing (e.g., text, images, templates). |
| `download.php` | File download management. |
| `image.php` | Image processing and editing. |
| `media.php` | Media asset management. |
| `asr.js` / `content.js` | JavaScript for editing actions (e.g., drag-and-drop, copy/paste). |


<!-- HASH:c2ba81f701cb833e1a0e9f5aaf4bb6ad -->
