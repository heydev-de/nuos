# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.content.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.content.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Module Interface (`ifc.content.inc`)

This file implements the **Content Module Interface** for the PWNC Web Platform, providing a comprehensive user interface for managing website content. It handles content creation, editing, publishing, versioning, and interaction with other modules like directories, templates, and the content pool.

---

## Overview

The interface allows users to:
- **Create, edit, and manage** content items (drafts, documents, publications).
- **Organize content** within a directory structure.
- **Control workflow** (send, authorize, publish, withdraw).
- **Manage versions** and scheduled actions.
- **Analyze content** for SEO and readability.
- **Configure RSS feeds** and content settings.
- **Use a content pool** for reusable content snippets.

The interface is **permission-aware**, showing only actions the current user is allowed to perform.

---

## Key Components

### 1. **Initialization and Permission Handling**

The file begins by loading required libraries (`content`, `content_pool`, `directory`, `document`, `flexview`, `template`) and initializing core variables. It checks user permissions to determine access levels and instantiates the `content` class for the current user.

#### **Constants and Variables**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$user` | `CMS_USER` or `CMS_SUPERUSER` | Current user ID. Falls back to superuser if access is denied. |
| `$content` | `new content($user)` | Content management instance for the current user. |
| `$icon` | Associative array | Maps content status and type to icon paths. |
| `$status` | Associative array | Maps content status codes to human-readable labels. |
| `$type` | Associative array | Maps content type codes to human-readable labels. |

---

### 2. **Message Handling (Main Switch)**

The interface processes different actions via `CMS_IFC_MESSAGE`. Each case represents a distinct operation (e.g., `select`, `edit`, `publish`, `delete`).

#### **Key Message Handlers**

| Message | Purpose |
|---------|---------|
| `select` | Selects a content object and finds its directory linkage. |
| `directory_select` | Selects a directory and resolves its linked content. |
| `info` | Displays detailed metadata for a content item. |
| `meta` / `_meta` | Edits metadata (title, description, keywords, image, template). |
| `edit_range` / `edit_value` / `edit_plugin` / `edit_href` | Edits specific parts of content (text, values, plugins, links). |
| `create` / `_create` | Creates a new content item. |
| `display` | Renders the content for display. |
| `apply` / `_apply` | Applies changes (immediately or scheduled). |
| `authorize` / `_authorize` | Authorizes content for publication. |
| `publish` / `_publish` | Publishes content to a directory. |
| `withdraw` | Withdraws published content. |
| `duplicate` / `copy` | Duplicates or copies content. |
| `send` / `_send` | Sends content to other users. |
| `delete` | Deletes selected content items. |
| `version` / `version_display` / `version_store` / `version_retrieve` | Manages content versions. |
| `schedule` / `schedule_save` / `schedule_delete` | Manages scheduled actions. |
| `pool` | Manages reusable content snippets in the content pool. |
| `rss` | Configures RSS feeds for content. |
| `configuration` | Configures module settings (e.g., extra column). |
| `flag` | Sets SEO-related flags (e.g., sitemap exclusion, meta robots). |
| `analyze` / `analyze_span` | Analyzes content for word frequency and markup. |
| `debug` | Debugs content structure (template operators only). |
| `reset` | Resets filters and search settings. |

---

## Detailed Method/Function Documentation

### `content_template_preview()`
**Purpose**: Previews the selected template in a new window.
**Parameters**: None (uses `ifc_param6` from the IFC form).
**Return Values**: None (triggers a page load).
**Inner Mechanisms**:
- Retrieves the template ID from the form.
- Constructs a URL to preview the template.
- Uses `load_page()` to open the preview.
**Usage Context**:
- Used in the metadata and creation forms to preview templates before saving.

**Example**:
```javascript
// In the metadata form, clicking "Show" triggers this function:
content_template_preview(); // Opens a preview of the selected template.
```

---

### `content_edit_range_save()`
**Purpose**: Saves edited text range back to the parent window and closes the editor.
**Parameters**: None (uses form data).
**Return Values**: None (submits form to parent window).
**Inner Mechanisms**:
- Sets the form target to the parent window.
- Posts the edited range back to the parent.
- Closes the editor window.
**Usage Context**:
- Used in the `edit_range` popup to save changes.

**Example**:
```javascript
// In the text range editor, clicking "Insert" triggers this function:
content_edit_range_save(); // Saves changes and closes the editor.
```

---

### `content_edit_href_select(value, text)`
**Purpose**: Updates the link URL and text in the `edit_href` form.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | string | The selected URL (e.g., `content://123`). |
| `text` | string | The display text for the link. |
**Return Values**: None (updates form fields).
**Inner Mechanisms**:
- Updates the multi-language URL field.
- Cleans up the display text (removes indices, trims spaces).
- Sets the link code (`_value`) and raw URL (`__value`).
**Usage Context**:
- Used in the `edit_href` popup when selecting a directory or publication.

**Example**:
```javascript
// Selecting a directory updates the link:
content_edit_href_select("directory://456", "About Us"); // Sets the link to "About Us".
```

---

### `content_pool_select(value)`
**Purpose**: Updates the selected content pool item and refreshes the preview.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | string | The ID of the selected pool item. |
**Return Values**: None (updates form and reloads preview).
**Inner Mechanisms**:
- Updates the `pool_object` field with the selected item.
- Reloads the preview iframe to display the selected item.
**Usage Context**:
- Used in the content pool interface to switch between items.

**Example**:
```javascript
// Selecting a pool item updates the preview:
content_pool_select("pool_789"); // Displays the selected pool item.
```

---

### `content_version_select(value)`
**Purpose**: Displays a selected content version in the preview iframe.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | string | The version ID to display. |
**Return Values**: None (updates iframe source).
**Inner Mechanisms**:
- Constructs a URL to display the selected version.
- Updates the iframe `src` to load the version.
**Usage Context**:
- Used in the version history interface to preview versions.

**Example**:
```javascript
// Selecting a version updates the preview:
content_version_select("version_123"); // Displays version 123.
```

---

### `directory_flexview_display_function($key, $data)`
**Purpose**: Custom display function for directory entries in the `flexview` component.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The directory entry key. |
| `$data` | `data` object | The directory data object. |
**Return Values**: string (HTML for the directory entry).
**Inner Mechanisms**:
- Retrieves the entry name and URL.
- Handles directory dereferencing (resolves `directory://` links).
- Displays the entry with appropriate icons and styling.
**Usage Context**:
- Used in the directory popover to render directory entries.

**Example**:
```php
// Renders a directory entry:
echo directory_flexview_display_function("dir_123", $directory_data);
// Output: <a href="javascript:...">Home</a>
```

---

## Helper Functions

### `content_get_range($content, $object, $range, $type)`
**Purpose**: Retrieves a specific range of content (text, value, plugin, or href).
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$content` | `content` object | The content instance. |
| `$object` | string | The content object ID. |
| `$range` | string | The range identifier (e.g., `tp-dd-abc123`). |
| `$type` | string | The type of range (`text`, `value`, `plugin`, `href`). |
**Return Values**: string (the retrieved content).
**Usage Context**:
- Used in editing popups to fetch the current value of a range.

**Example**:
```php
// Retrieve a text range for editing:
$text = content_get_range($content, "123", "tp-dd-abc123", "text");
```

---

### `content_get_receiver($content, $type, $status)`
**Purpose**: Retrieves a list of users who can receive content of a given type and status.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$content` | `content` object | The content instance. |
| `$type` | int | The content type (e.g., `CMS_CONTENT_TYPE_ORIGINAL`). |
| `$status` | int | The content status (e.g., `CMS_CONTENT_STATUS_DRAFT`). |
**Return Values**: array (list of users grouped by content type).
**Usage Context**:
- Used in the `send` interface to populate the receiver list.

**Example**:
```php
// Get receivers for a draft:
$receivers = content_get_receiver($content, CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT);
```

---

### `content_get_directory_index($content_index)`
**Purpose**: Retrieves the directory index linked to a content item.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$content_index` | string | The content item ID. |
**Return Values**: string (the directory index) or `NULL` if not found.
**Usage Context**:
- Used to find the directory entry for a content item.

**Example**:
```php
// Get the directory index for content item 123:
$directory_index = content_get_directory_index("123");
```

---

### `content_pool_get_select()`
**Purpose**: Retrieves a list of categories from the content pool.
**Parameters**: None.
**Return Values**: array (list of categories).
**Usage Context**:
- Used in the content pool interface to populate the category dropdown.

**Example**:
```php
// Get categories for the content pool:
$categories = content_pool_get_select();
```

---

### `content_pool_get_array($type = NULL)`
**Purpose**: Retrieves an array of content pool items, optionally filtered by type.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$type` | string | Optional. The template type to filter by. |
**Return Values**: array (pool items grouped by category).
**Usage Context**:
- Used in the content pool interface to display items.

**Example**:
```php
// Get all pool items:
$pool_items = content_pool_get_array();
// Get only "text" type items:
$text_items = content_pool_get_array("text");
```

---

### `content_template_select()`
**Purpose**: Generates a dropdown of available templates.
**Parameters**: None.
**Return Values**: string (HTML for the template dropdown).
**Usage Context**:
- Used in the metadata and creation forms to select a template.

**Example**:
```php
// Display a template dropdown:
echo content_template_select();
```

---

### `template_preview($object, $exit = TRUE, $document = NULL)`
**Purpose**: Previews a template or content item.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | The content or template ID. |
| `$exit` | bool | Whether to exit after rendering. Default: `TRUE`. |
| `$document` | `document` object | Optional. A document object to use for preview. |
**Return Values**: None (outputs HTML or exits).
**Usage Context**:
- Used to preview templates or content items.

**Example**:
```php
// Preview a template:
template_preview("template_123");
// Preview a content item with a custom document:
template_preview("123", TRUE, $custom_document);
```

---

## Main Display Logic

The main display section renders the content management interface, including:
- **User selection** (if multiple users are accessible).
- **Filtering** (by status, sender, or author).
- **Search** (by index, title, description, keywords, or extra fields).
- **Directory popover** (to navigate linked directories).
- **Content table** (listing content items with actions).

### Key Features:
1. **Pagination**: Handles large result sets with pagination controls.
2. **Sorting**: Allows sorting by index, title, author, or date.
3. **Action Buttons**: Provides context-sensitive actions (edit, publish, send, etc.).
4. **Extra Column**: Displays custom data (e.g., links, images, plugins) if configured.

### Example Workflow:
1. **Select a User**: Choose a user whose content you want to manage.
2. **Apply a Filter**: Filter by drafts, documents, or publications.
3. **Search**: Search for content by title or keywords.
4. **Select an Item**: Click on a content item to view or edit it.
5. **Perform Actions**: Use the action buttons to edit, publish, or send the content.

---

## Usage Examples

### Example 1: Creating a New Content Item
1. Click **"Create"** in the interface.
2. Fill in the **title**, select a **template**, and add a **comment**.
3. Click **"Confirm"** to create the item.
4. The new item appears in the content table as a draft.

### Example 2: Publishing Content
1. Select a **draft** or **document** from the content table.
2. Click the **"Publish"** button.
3. Choose a **directory** to link the content to.
4. Set a **publication time** (immediate or scheduled).
5. Add a **comment** and click **"Confirm"**.
6. The content is published and linked to the selected directory.

### Example 3: Analyzing Content
1. Select a content item from the table.
2. Click the **"Analyze"** button.
3. View the **plain text**, **word frequency**, and **source code** tabs.
4. Adjust the **word span** to analyze phrases.

### Example 4: Managing the Content Pool
1. Open the **content pool** interface.
2. Select a **category** and **item** to preview.
3. Click **"Edit"** to modify the item or **"Insert"** to use it in content.
4. Use **"Synchronize"** to update all pool items.

---


<!-- HASH:324b66b85e117efccf86ae8678afb761 -->
