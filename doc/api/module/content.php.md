# NUOS API Documentation

[← Index](../README.md) | [`module/content.php`](https://github.com/heydev-de/nuos/blob/main/nuos/module/content.php)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Content Module (`module/content.php`)

The `content.php` module is the core entry point for serving, displaying, and editing content in the NUOS platform. It handles:

- **Content resolution**: Translates directory-based URLs into specific content items.
- **Permission checks**: Validates user access rights before rendering content.
- **Dynamic vs. static content**: Differentiates between cached (static) and editable (dynamic) content.
- **Edit mode**: Provides a full in-place editing interface with actions for modifying content structure, templates, and metadata.
- **HTTP optimizations**: Implements `ETag` and `Last-Modified` headers for efficient caching.
- **Error handling**: Manages 404 (Not Found), 410 (Gone), 500 (Server Error), and 508 (Loop Detected) responses.

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$content_display` | `string` | Controls the display mode (e.g., `"directory"`). |
| `$content_index` | `int` | Unique identifier of the content item being accessed. |
| `$content_directory_index` | `string` | Identifier of the directory entry (logical grouping). |
| `$content_option` | `int` | Bitmask of enabled editing options (e.g., `CMS_TEMPLATE_OPTION_TEXT`). |
| `$content_user` | `string` | User identifier for permission checks. |
| `$content_message` | `string` | Command issued (e.g., `"apply"`, `"revert"`). |
| `$range` | `string` | Path to the specific element within the content structure. |
| `$type` | `string` | Type of edit operation (e.g., `"value"`, `"image"`, `"#buffer"`). |
| `$value` | `mixed` | New value for the edited element. |
| `$left` / `$top` | `float` | Scroll position (percentage) for maintaining viewport state. |
| `$id` | `string` | Unique DOM identifier for the edited element. |

---

### Key Logic Flow

1. **404 Handling**
   - Skips custom 404 pages for static assets (images, fonts, CSS, JS).
   - Falls back to the platform’s default 404 if no custom handler is defined.

2. **Directory Resolution**
   - Converts directory-based URLs (e.g., `directory://home`) into specific content items.
   - Detects and prevents infinite loops in directory references.

3. **Permission Validation**
   - Redirects unauthorized users to the login page (`identification.php`).

4. **Content Status Check**
   - Returns `410 Gone` if the content is not published.

5. **Edit vs. Read Mode**
   - **Read Mode**: Renders static content with caching headers.
   - **Edit Mode**: Injects JavaScript for in-place editing and constructs an action menu.

6. **Action Dispatch**
   - Processes commands (`apply`, `revert`, `undo`, `redo`) or applies direct edits.

---

### Functions and Methods

#### `content_parse()`
*(Called internally; not defined in this file.)*

| Parameter | Type | Description |
|-----------|------|-------------|
| `$content` | `content` | Content object instance. |
| `$content_index` | `int` | Content identifier. |
| `$action` | `array\|null` | Action menu configuration for edit mode. |
| `$header` | `string` | Additional HTML/JS to inject into `<head>`. |
| `$is_dynamic` | `bool` (out) | Set to `TRUE` if the output contains uncached parts. |
| `$mod_time` | `int` (out) | Last modification timestamp (Unix epoch). |

**Return Value**: `string` – Rendered HTML output.

**Mechanism**:
- Parses the content’s template and data.
- Injects edit controls if in edit mode.
- Computes `ETag` and `Last-Modified` headers for static content.

**Usage**:
```php
$output = content_parse($content, $content_index, $action, $header, $is_dynamic, $mod_time);
```

---

#### `content_set_range()`
*(Called internally; not defined in this file.)*

| Parameter | Type | Description |
|-----------|------|-------------|
| `$content` | `content` | Content object instance. |
| `$content_index` | `int` | Content identifier. |
| `$range` | `string` | Path to the edited element. |
| `$type` | `string` | Type of edit (e.g., `"value"`, `"image"`). |
| `$value` | `mixed` | New value for the element. |

**Mechanism**:
- Applies the edit to the specified range in the content structure.
- Triggers re-rendering of the affected area.

**Usage**:
```php
content_set_range($content, $content_index, $range, $type, $value);
```

---

### Action Menu Configuration

The `$action` array defines the edit interface. It is structured as:

```php
$action = [
    CMS_TEMPLATE_CONTROL => [
        CMS_TEMPLATE_SWITCH => [CMS_TEMPLATE_CODE => "..."], // Option selector
        CMS_TEMPLATE_COMMAND => ["Label" => "JS function call"], // Global commands
    ],
    CMS_TEMPLATE_ACTION => [
        CMS_TEMPLATE_TYPE_TEXT => [ // Per-element actions
            CMS_TEMPLATE_CODE => "JS function call",
            CMS_TEMPLATE_IMAGE => "icon.svg",
        ],
    ],
];
```

#### Key Action Types

| Constant | Description |
|----------|-------------|
| `CMS_TEMPLATE_OPTION_HREF` | Edit hyperlinks. |
| `CMS_TEMPLATE_OPTION_TEXT` | Edit text content. |
| `CMS_TEMPLATE_OPTION_IMAGE` | Edit images. |
| `CMS_TEMPLATE_OPTION_TEMPLATE` | Edit templates. |
| `CMS_TEMPLATE_OPTION_GROUP` | Group elements. |
| `CMS_TEMPLATE_OPTION_REPEAT` | Repeat elements. |
| `CMS_TEMPLATE_OPTION_SHIFT` | Reorder elements. |
| `CMS_TEMPLATE_OPTION_DEBUG` | Debug mode. |

---

### Special Edit Types

| Type | Description |
|------|-------------|
| `#buffer` | Retrieves a partial content fragment (used for copy/paste). |
| `#paste` | Pastes a copied fragment into the specified range. |
| `#swap` | Swaps two elements. |
| `#kick1` / `#kick2` | Moves an element up/down. |
| `#drop1` / `#drop2` | Adjusts nesting level. |
| `#clear` | Removes an element. |
| `#dragdrop1` / `#dragdrop2` | Drag-and-drop reordering. |
| `#reference` | Inserts a reference to a content pool item. |

---

### JavaScript Integration

The module injects the following scripts:
- `asr.js`: Core event and DOM utilities.
- `content.js`: Edit mode logic (copy/paste, drag-and-drop, etc.).

Key functions exposed to JavaScript:
- `content_edit_apply()` / `content_edit_revert()`: Save or discard changes.
- `content_edit_open()`: Open a modal interface (e.g., for image selection).
- `content_edit_switch()`: Toggle a boolean value.
- `content_edit_copy()` / `content_edit_paste()`: Clipboard operations.
- `tp_flp_restore()`: Restores scroll position and edit state.

---

### Caching and Performance

- **Static Content**: Uses `ETag` and `Last-Modified` headers to enable browser caching.
- **Dynamic Content**: Bypasses caching for logged-in users with edit permissions.
- **Processing Time**: Outputs generation time in a comment (`<!-- generated in X ms -->`).

---

### Error Handling

| HTTP Code | Scenario |
|-----------|----------|
| `404` | Content or directory not found. |
| `410` | Content exists but is not published. |
| `500` | Content module failed to load. |
| `508` | Infinite loop detected in directory references. |

---

### Usage Scenarios

1. **Viewing Content**
   - Access a URL like `https://example.com/content.php?content_index=123`.
   - The module checks permissions, retrieves the content, and renders it.

2. **Editing Content**
   - Append `&content_user=admin` to enable edit mode.
   - Use the action menu to modify text, images, templates, etc.

3. **Directory Resolution**
   - Access `https://example.com/directory/home`.
   - The module resolves `directory://home` to a specific `content_index` and redirects.

4. **Partial Updates**
   - JavaScript calls `content.php?type=#buffer&range=...` to fetch a fragment for copy/paste.

5. **Command Execution**
   - Submit a form or click a button to trigger `content.php?content_message=apply`.


<!-- HASH:042500e4659a9136ad4b53b9222f3968 -->
