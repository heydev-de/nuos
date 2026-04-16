# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.content.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Overview

This file (`ifc.content.inc`) implements the **Content Management Interface** for the NUOS web platform. It provides a comprehensive user interface for managing website content, including creation, editing, publishing, versioning, and distribution. The interface integrates with several core NUOS modules (`content`, `content_pool`, `directory`, `document`, `flexview`, `template`) to offer a unified content management experience.

The interface handles:
- **Content lifecycle management** (drafts, documents, publications)
- **User permissions and roles** (writer, editor, publisher, operator)
- **Content metadata editing** (titles, descriptions, keywords, templates)
- **Version control and scheduling** (apply, revert, publish, withdraw)
- **Content distribution** (sending, duplicating, copying)
- **Content analysis** (text analysis, SEO metrics, debug information)
- **RSS feed management**
- **Content pool management** (reusable content snippets)
- **Directory linking and navigation**

---

## Key Components

### 1. **Initialization and Permission Handling**

#### **Library Loading and User Validation**
- Loads required libraries (`content`, `content_pool`, `directory`, `document`, `flexview`, `template`).
- Validates user permissions and initializes the `content` object.
- Falls back to `CMS_SUPERUSER` if no valid user is detected.

#### **Permission Mapping**
| Permission Key | Description |
|----------------|-------------|
| `""` | General access |
| `CMS_CONTENT_PERMISSION_WRITER` | Writer access |
| `CMS_CONTENT_PERMISSION_EDITOR` | Editor access |
| `CMS_CONTENT_PERMISSION_PUBLISHER` | Publisher access |
| `{user}` | User-specific access |
| `CMS_CONTENT_POOL_PERMISSION_OPERATOR` | Content pool operator |
| `CMS_CONTENT_PERMISSION_OPERATOR` | Content operator |

---

### 2. **Message Handling / Sub-Display**

The interface processes various actions via `CMS_IFC_MESSAGE`. Each case represents a distinct operation (e.g., `select`, `meta`, `edit_range`, `publish`).

#### **Key Message Handlers**

##### ### `select`
**Purpose**: Selects a content object and locates its topmost directory linkage.
**Parameters**:
- `$ifc_param`: Content index to select.
**Mechanism**:
- Searches the directory for a URL matching `content://{object}`.
- Sets `$directory_object` to the topmost directory entry containing the content link.

---

##### ### `meta` / `_meta`
**Purpose**: Displays/updates metadata (title, description, keywords, image, comment, template) for a content object.
**Parameters**:
- `$ifc_param`: Content index.
- `$ifc_param1`–`$ifc_param6`: Updated metadata values (title, description, keyword, image, comment, template).
**Mechanism**:
- Retrieves current metadata from the database.
- Displays an interface form for editing.
- On submission (`_meta`), updates the database with new values.
**Usage**: Used when editing content metadata without modifying the content body.

---

##### ### `edit_range`, `edit_value`, `edit_plugin`, `edit_href`
**Purpose**: Edits specific parts of a content object (text ranges, values, plugins, hyperlinks).
**Parameters**:
- `$object`: Content index.
- `$range`: Range identifier within the content.
- `$type`: Type of edit (`text`, `value`, `plugin`, `href`).
- `$value`, `$_value`, `$__value`: Current and parsed values for hyperlinks.
- `$language`: Language code for multilingual hyperlinks.
**Mechanism**:
- Retrieves the current value using `content_get_range()`.
- Displays a modal editor for the specified range.
- On save, updates the parent window and closes the modal.
**Usage**: Used for inline editing of content elements (e.g., hyperlinks, embedded plugins).

---

##### ### `create` / `_create`
**Purpose**: Creates a new content object.
**Parameters**:
- `$ifc_param1`: Title.
- `$ifc_param2`: Template.
- `$ifc_param3`: Comment.
**Mechanism**:
- Calls `$content->create()` to generate a new content object.
- Updates `$object` with the new index and caches the selected template.
**Usage**: Initiates the content creation workflow.

---

##### ### `apply` / `_apply`
**Purpose**: Applies changes from the buffer to the live content (immediately or scheduled).
**Parameters**:
- `$object`: Content index.
- `$ifc_param1`: Action type (`0`: immediate, `1`: scheduled, `2`: remove schedule).
- `$ifc_param2`–`$ifc_param6`: Scheduled time components (day, month, year, hour, minute).
**Mechanism**:
- Checks for existing schedules.
- Displays a form for scheduling or immediate application.
- Updates the database and applies changes via `$content->apply()`.
**Usage**: Used to finalize edits and make them live.

---

##### ### `publish` / `__publish` / `publish_replace` / `publish_insert` / `publish_append`
**Purpose**: Publishes content to a directory (immediately or scheduled).
**Parameters**:
- `$object`: Content index.
- `$ifc_param`: Directory index.
- `$ifc_param1`: Title.
- `$ifc_param2`: Publication type (`0`: immediate, `1`: scheduled).
- `$ifc_param3`–`$ifc_param14`: Scheduled times, withdrawal times, and comments.
- `$action`: Directory action (`replace`, `insert`, `append`).
**Mechanism**:
- Retrieves publication metadata (title, comment).
- Displays a form for scheduling publication/withdrawal and selecting a directory target.
- Calls `$content->publish()` to update the database and directory.
**Usage**: Used to publish content to the frontend.

---

##### ### `pool` / `pool_select` / `pool_display` / `pool_add` / `pool_edit` / `pool_delete`
**Purpose**: Manages reusable content snippets (content pool).
**Parameters**:
- `$pool_object`: Selected pool object index.
- `$pool_type`: Type of content pool item.
- `$pool_language`: Language code for multilingual pool items.
- `$content_index`, `$content_range`: Content and range identifiers for pool items.
**Mechanism**:
- Displays a list of pool items filtered by category and type.
- Supports adding, editing, and deleting pool items.
- Synchronizes pool items with their source content.
**Usage**: Used for managing reusable content components (e.g., templates, images, text snippets).

---

##### ### `rss` / `rss_select` / `rss_add` / `rss_save` / `rss_delete` / `rss_assign`
**Purpose**: Manages RSS channels and assigns content to them.
**Parameters**:
- `$rss_object`: Selected RSS channel index.
- `$object`: Content index for assignment.
- `$_list`: List of selected RSS channels.
**Mechanism**:
- Displays a list of RSS channels with options to add, edit, or delete.
- Assigns content to selected channels.
- Updates the database and regenerates RSS feeds.
**Usage**: Used for RSS feed management and content syndication.

---

##### ### `analyze` / `analyze_span`
**Purpose**: Analyzes content for SEO metrics (word frequency, markup ratio, keyword density).
**Parameters**:
- `$object`: Content index.
- `$span`: Word span size for analysis (default: `0`).
**Mechanism**:
- Retrieves content text and metadata.
- Converts HTML to plain text and tokenizes it.
- Calculates word frequency, markup ratio, and keyword density.
- Displays results in a tabbed interface.
**Usage**: Used for SEO optimization and content analysis.

---

##### ### `debug`
**Purpose**: Displays debug information for a content object (document structure, element hierarchy).
**Parameters**:
- `$object`: Content index.
**Mechanism**:
- Parses the document structure and displays a tree view of elements.
- Shows element IDs, paths, types, and values.
**Usage**: Used for debugging template and document structure issues.

---

### 3. **Main Display**

#### **Directory and Content Listing**
- Displays a directory tree for navigation.
- Lists content objects with status icons, metadata, and action buttons.
- Supports filtering, sorting, and pagination.

#### **Key UI Elements**
| Element | Description |
|---------|-------------|
| **User Box Selection** | Switches between users (if permitted). |
| **Object Type Selection** | Filters content by type (e.g., drafts, documents, publications). |
| **Object Filter** | Filters content by field (e.g., title, description, keyword). |
| **Row Display Limit** | Sets the number of rows per page. |
| **Directory Tree** | Navigates linked content directories. |
| **Content Information** | Displays metadata for the selected content object. |
| **Content List** | Lists content objects with action buttons (edit, apply, publish, etc.). |
| **Pagination** | Navigates through large content lists. |

#### **Action Buttons**
| Button | Action | Description |
|--------|--------|-------------|
| ![Display](content/button_display) | `d1()` | Displays the content. |
| ![Edit](content/button_edit) | `e()` | Edits the content. |
| ![Meta](content/button_meta) | `m()` | Edits metadata. |
| ![Apply](content/button_apply) | `a1()` | Applies changes. |
| ![Revert](content/button_revert) | `r1()` | Reverts changes. |
| ![RSS](content/button_rss) | `r2()` | Assigns to RSS channels. |
| ![Flag](content/button_flag) | `f()` | Sets SEO flags. |
| ![Version Store](content/button_version_store) | `vs()` | Stores a version. |
| ![Version](content/button_version) | `v()` | Views versions. |
| ![Authorize](content/button_authorize) | `a2()` | Authorizes content. |
| ![Publish](content/button_publish) | `p1()` | Publishes content. |
| ![Withdraw](content/button_withdraw) | `w()` | Withdraws content. |
| ![Schedule](content/button_schedule) | `s2()` | Manages schedules. |
| ![Duplicate](content/button_duplicate) | `d2()` | Duplicates content. |
| ![Copy](content/button_copy) | `c()` | Copies content. |
| ![Send](content/button_send) | `s3()` | Sends content to other users. |

---

## Helper Functions

### `content_template_select()`
**Purpose**: Generates a dropdown list of available templates.
**Returns**: Array of template options.
**Usage**: Used in metadata and creation forms.

### `content_get_range($content, $object, $range, $type)`
**Purpose**: Retrieves a specific range of content (e.g., text, value, plugin, href).
**Parameters**:
- `$content`: `content` object.
- `$object`: Content index.
- `$range`: Range identifier.
- `$type`: Type of range (`text`, `value`, `plugin`, `href`).
**Returns**: The requested range value.
**Usage**: Used for inline editing of content elements.

### `content_get_directory_index($content_index)`
**Purpose**: Retrieves the directory index for a content object.
**Parameters**:
- `$content_index`: Content index.
**Returns**: Directory index or `NULL` if not found.
**Usage**: Used to locate the directory entry for a content object.

### `directory_flexview_display_function($data, $key)`
**Purpose**: Custom display function for directory entries in `flexview`.
**Parameters**:
- `$data`: `data` object.
- `$key`: Directory index.
**Returns**: Formatted HTML for the directory entry.
**Usage**: Used to render directory entries in the interface.

### `content_pool_get_select()`
**Purpose**: Generates a list of content pool categories.
**Returns**: Array of category options.
**Usage**: Used in pool management forms.

### `content_pool_get_array($type = NULL)`
**Purpose**: Retrieves an array of pool items filtered by type.
**Parameters**:
- `$type`: Optional type filter.
**Returns**: Associative array of pool items grouped by category.
**Usage**: Used to populate the pool list in the interface.

### `rss_get_default()`
**Purpose**: Retrieves the default RSS channel configuration.
**Returns**: String representing the default channel configuration.
**Usage**: Used to determine default RSS channels for content.

---

## Constants and Variables

### **Status and Type Icons**
| Status | Type | Icon |
|--------|------|------|
| `CMS_CONTENT_STATUS_DRAFT` | `CMS_CONTENT_TYPE_ORIGINAL` | `content/icon_draft` |
| `CMS_CONTENT_STATUS_DRAFT` | `CMS_CONTENT_TYPE_DUPLICATE` | `content/icon_draft_duplicate` |
| `CMS_CONTENT_STATUS_DRAFT` | `CMS_CONTENT_TYPE_COPY` | `content/icon_draft_copy` |
| `CMS_CONTENT_STATUS_DOCUMENT` | `CMS_CONTENT_TYPE_ORIGINAL` | `content/icon_document` |
| `CMS_CONTENT_STATUS_DOCUMENT` | `CMS_CONTENT_TYPE_DUPLICATE` | `content/icon_document_duplicate` |
| `CMS_CONTENT_STATUS_DOCUMENT` | `CMS_CONTENT_TYPE_COPY` | `content/icon_document_copy` |
| `CMS_CONTENT_STATUS_PUBLICATION` | `CMS_CONTENT_TYPE_ORIGINAL` | `content/icon_publication` |

### **Status and Type Labels**
| Constant | Label |
|----------|-------|
| `CMS_CONTENT_STATUS_DRAFT` | `CMS_L_IFC_CONTENT_031` (Draft) |
| `CMS_CONTENT_STATUS_DOCUMENT` | `CMS_L_IFC_CONTENT_032` (Document) |
| `CMS_CONTENT_STATUS_PUBLICATION` | `CMS_L_IFC_CONTENT_033` (Publication) |
| `CMS_CONTENT_TYPE_ORIGINAL` | `CMS_L_IFC_CONTENT_034` (Original) |
| `CMS_CONTENT_TYPE_DUPLICATE` | `CMS_L_IFC_CONTENT_035` (Duplicate) |
| `CMS_CONTENT_TYPE_COPY` | `CMS_L_IFC_CONTENT_036` (Copy) |

### **Caching Keys**
| Key | Description |
|-----|-------------|
| `content.{user}.user` | Current user. |
| `content.{user}.object` | Selected content object. |
| `directory.{user}.object` | Selected directory object. |
| `content.{user}.sql_select` | Current SQL select filter. |
| `content.{user}.sql_filter_field` | Current SQL filter field. |
| `content.{user}.sql_filter_option` | Current SQL filter option. |
| `content.{user}.sql_filter_value` | Current SQL filter value. |
| `content.{user}.sql_order` | Current SQL sort order. |
| `content.{user}.page` | Current page number. |
| `content.{user}.limit` | Current row limit per page. |
| `template.{user}.page` | Last used template for page creation. |
| `content_pool.{user}.category` | Last selected pool category. |

---

## Usage Scenarios

### **Creating Content**
1. Click **"Create"** in the interface.
2. Enter a title, select a template, and add a comment.
3. Click **"Confirm"** to create the content object.
4. Edit the content using the inline editor.

### **Publishing Content**
1. Select a content object from the list.
2. Click **"Publish"**.
3. Enter a title, select a directory, and choose publication/withdrawal times.
4. Click **"Confirm"** to publish.

### **Managing Content Pool**
1. Navigate to the **"Pool"** tab.
2. Select a category and pool item.
3. Use the action buttons to edit, delete, or synchronize pool items.

### **Analyzing Content**
1. Select a content object.
2. Click **"Analyze"**.
3. View SEO metrics (word frequency, markup ratio, keyword density).
4. Adjust the word span size for more detailed analysis.

### **Debugging Content**
1. Select a content object.
2. Click **"Debug"**.
3. View the document structure and element hierarchy.

---

## Dependencies

- **Database**: Uses `mysql_*` wrappers for database operations.
- **Content Module**: Relies on the `content` class for core operations.
- **Content Pool Module**: Manages reusable content snippets.
- **Directory Module**: Handles directory navigation and linking.
- **Document Module**: Parses and manipulates document structures.
- **Flexview Module**: Renders directory trees and lists.
- **Template Module**: Manages templates and content rendering.
- **RSS Module**: Manages RSS channels and feeds (optional).
- **System Module**: Retrieves system configuration values.


<!-- HASH:de77da7350c26384b6f7230e276a5b1f -->
