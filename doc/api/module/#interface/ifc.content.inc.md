# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.content.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.content.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Interface

The `ifc.content.inc` file serves as the primary interface controller for the Content module within the PWNC Web Platform. It manages all interactions related to content creation, editing, publishing, versioning, scheduling, and display. This includes handling user permissions, rendering UI components, processing form submissions, and coordinating with underlying libraries such as `content`, `content_pool`, `directory`, `document`, `flexview`, and `template`.

### Key Responsibilities:
- **Library Loading**: Ensures required libraries are loaded before execution.
- **User Permission Management**: Validates access rights based on roles (writer, editor, publisher, operator).
- **Message Handling**: Processes various interface messages (`select`, `info`, `meta`, `edit_range`, `create`, `apply`, `publish`, etc.) via a central switch statement.
- **UI Rendering**: Generates dynamic forms, tables, menus, and JavaScript helpers using the `ifc` class.
- **Data Operations**: Interacts with database tables for content retrieval, updates, deletions, and metadata management.

---

## Constants and Variables

| Name | Value / Default | Description |
|------|------------------|-------------|
| `$icon` | Array mapping status/type combinations to icon paths | Used for visual representation of content items |
| `$status` | Array mapping content statuses to labels | Status descriptions like Draft, Document, Publication |
| `$type` | Array mapping content types to labels | Type descriptions like Original, Duplicate, Copy |

---

## Initialization and Setup

### Library Loading
```php
if (! cms_load("content")) ifc_inactive($ifc_page);
```
Loads essential libraries (`content`, `content_pool`, `directory`, `document`, `flexview`, `template`). If any fail to load, the interface is marked inactive.

### User Context Initialization
```php
cms_cache_sync($user, "content." . CMS_USER . ".user", CMS_SUPERUSER);
init($object);
init($directory_object);
```
Initializes global variables `$object` and `$directory_object` from request parameters or cache.

### Permission Check
```php
if (nstreq($user, CMS_SUPERUSER) && (...)) $user = CMS_SUPERUSER;
```
Ensures only authorized users can interact with content. Falls back to superuser if access is denied.

### Content Instance Creation
```php
$content = new content($user);
```
Instantiates the main content handler object tied to the current user context.

### Permission Definitions
```php
$data = new data("#system/permission");
$name = $data->get("user.$user", "name");
ifc_permission([...]);
```
Sets up permission levels displayed in the interface header, including writer/editor/publisher roles.

---

## Message Handling

### `switch (CMS_IFC_MESSAGE)`
Central dispatcher for all interface actions. Each case handles a specific operation:

#### Case: `"select"`
Handles selection of an object in the directory tree.

**Parameters:**
- `$ifc_param`: Object identifier

**Mechanics:**
- Searches directory entries for matching `content://` URL prefix
- Sets `$directory_object` accordingly
- Clears cached object reference

**Usage Example:**
Triggered when navigating to a content item through the directory structure.

---

#### Case: `"directory_select"`
Selects a directory entry and resolves its target content.

**Parameters:**
- `$ifc_param`: Directory object key

**Mechanics:**
- Resolves nested directory references (`directory://`)
- Switches user context if content belongs to another accessible user
- Updates active object and clears caches

**Usage Example:**
Used when selecting a folder-like entry that points to actual content.

---

#### Case: `"info"`
Displays detailed information about a content object.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Queries database for full content record
- Renders structured table showing status, type, title, description, keywords, writer/editor/publisher details
- Uses localization constants for field labels

**Usage Example:**
View metadata summary of a selected content item.

---

#### Case: `"_meta"`
Internal handler for saving metadata changes.

**Parameters:**
- `$ifc_param1`–`$ifc_param6`: Title, description, keyword, image, text, comment, template

**Mechanics:**
- Retrieves current buffer text from DB
- Calls `$content->update()` with new values
- Returns success/failure message

**Usage Example:**
Submitted after editing meta fields in the "Meta" tab.

---

#### Case: `"meta"`
Renders the metadata editing form.

**Mechanics:**
- Loads existing metadata into editable fields
- Provides preview functionality for templates
- Includes image selector and language-aware inputs

**Usage Example:**
User clicks "Edit Meta" button on a content row.

---

#### Case: `"template_preview"`
Generates live preview of selected template.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Calls `template_preview()` function with content data
- Outputs rendered HTML directly

**Usage Example:**
Preview how content will appear using chosen template.

---

#### Case: `"edit_range"`
Edits a specific range within content text.

**Parameters:**
- `$object`: Content index
- `$range`: Range identifier
- `$id`: Element ID

**Mechanics:**
- Loads content range via `content_get_range()`
- Displays editor with save/cancel options
- Posts back to opener window upon save

**Usage Example:**
Editing inline plugin or href ranges in rich-text content.

---

#### Case: `"edit_value"`
Edits a value-type range.

**Parameters:**
- Same as `edit_range`

**Mechanics:**
- Similar to `edit_range`, but uses textarea instead of text editor
- Designed for simple text replacements

**Usage Example:**
Modifying placeholder values in content blocks.

---

#### Case: `"edit_plugin"`
Edits a plugin URL range.

**Parameters:**
- Same as `edit_range`

**Mechanics:**
- Pre-fills input with protocol prefix if empty
- Strips protocol on save if unchanged

**Usage Example:**
Setting external resource URLs in content elements.

---

#### Case: `"edit_href"` / `"edit_href_select_language"`
Manages hyperlink editing with multi-language support.

**Parameters:**
- `$range`: Range identifier
- `$language`: Language code

**Mechanics:**
- Parses link references using `template_parse_reference()`
- Provides directory/content selectors
- Supports language-specific URLs

**Usage Example:**
Creating internal links between content items across languages.

---

#### Case: `"create"`
Renders the content creation form.

**Mechanics:**
- Offers title input, template selector, and comment area
- Includes template preview capability

**Usage Example:**
Initiating new content item creation.

---

#### Case: `"_create"`
Processes content creation submission.

**Parameters:**
- `$ifc_param1`: Title
- `$ifc_param2`: Template
- `$ifc_param3`: Comment

**Mechanics:**
- Calls `$content->create()`
- Caches selected template for future use

**Usage Example:**
Finalizing new content creation after form submission.

---

#### Case: `"display"`
Outputs parsed content for public viewing.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Calls `content_parse()` to render final output
- Exits immediately after output

**Usage Example:**
Direct rendering of content for frontend display.

---

#### Case: `"apply"`
Renders apply/schedule publication form.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Offers immediate or scheduled application options
- Auto-selects schedule mode when date changes

**Usage Example:**
Applying pending edits to published content.

---

#### Case: `"_apply"`
Executes content application logic.

**Parameters:**
- `$ifc_param1`: Mode (0=immediate, 1=scheduled)
- `$ifc_param2`: Scheduled time (if applicable)

**Mechanics:**
- Calls `$content->apply()` with optional timestamp

**Usage Example:**
Processing apply request from form submission.

---

#### Case: `"apply_all"`
Applies all pending content changes.

**Mechanics:**
- Iterates over all modified content records
- Applies each one individually

**Usage Example:**
Bulk applying changes across multiple content items.

---

#### Case: `"revert"`
Reverts content to last saved version.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Calls `$content->revert()`

**Usage Example:**
Undoing recent edits to restore previous state.

---

#### Case: `"authorize"`
Displays authorization form for editors.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Loads editor comment from DB
- Shows comment field for approval/rejection

**Usage Example:**
Editor reviewing and authorizing writer submissions.

---

#### Case: `"_authorize"`
Processes authorization decision.

**Parameters:**
- `$ifc_param1`: Editor comment

**Mechanics:**
- Calls `$content->authorize()`

**Usage Example:**
Submitting authorization decision.

---

#### Case: `"derive_draft"`
Creates a draft copy from existing content.

**Parameters:**
- `$object`: Source content index

**Mechanics:**
- Calls `$content->derive_draft()`

**Usage Example:**
Branching off a new draft from approved content.

---

#### Case: `"publish"`
Renders the publishing workflow form.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Offers immediate/scheduled publish options
- Includes withdrawal scheduling
- Integrates directory targeting via FlexView

**Usage Example:**
Preparing content for public release.

---

#### Case: `"_publish"` / `"__publish"` / `"___publish"` / `"publish_replace"` / `"publish_insert"` / `"publish_append"`
Handles various stages of the publishing process.

**Parameters:**
- `$ifc_param1`: Directory title
- `$ifc_param2`: Publish mode (0=immediate, 1=scheduled)
- `$ifc_param3`: Scheduled publish time
- `$ifc_param4`: Withdrawal mode
- `$ifc_param5`: Scheduled withdrawal time
- `$ifc_param6`: Publisher comment

**Mechanics:**
- Manages directory linking actions (replace/insert/append)
- Calls `$content->publish()` with appropriate parameters

**Usage Example:**
Completing the publish workflow after setting timing and location.

---

#### Case: `"withdraw"`
Withdraws published content.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Calls `$content->withdraw()`

**Usage Example:**
Removing content from public view.

---

#### Case: `"duplicate"`
Creates a duplicate copy of content.

**Parameters:**
- `$object`: Source content index

**Mechanics:**
- Calls `$content->duplicate()`

**Usage Example:**
Creating variants of existing content.

---

#### Case: `"copy"`
Creates a copy of content.

**Parameters:**
- `$object`: Source content index

**Mechanics:**
- Calls `$content->copy()`

**Usage Example:**
Generating independent copies for reuse.

---

#### Case: `"send"`
Manages sending content to other users.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Determines eligible receivers based on permissions
- Allows sending original, duplicate, or copy versions

**Usage Example:**
Sharing content with collaborators.

---

#### Case: `"_send"`
Processes content sending operation.

**Parameters:**
- `$original`: Receiver for original content
- `$duplicate`: Array of receivers for duplicates
- `$copy`: Array of receivers for copies
- `$ifc_param1`: Sender comment

**Mechanics:**
- Iterates through selected receivers
- Creates and sends appropriate content variants

**Usage Example:**
Executing bulk content distribution.

---

#### Case: `"delete"`
Deletes selected content items.

**Parameters:**
- `$list`: Array of content indices

**Mechanics:**
- Iterates through list calling `$content->delete()`

**Usage Example:**
Removing obsolete or unwanted content.

---

#### Case: `"clear_cache"`
Clears content rendering cache.

**Mechanics:**
- Requires publisher privileges
- Cleans `#content/cache/` directory

**Usage Example:**
Forcing refresh of cached content output.

---

#### Case: `"version"`
Displays version history interface.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Lists all stored versions
- Provides restore option if updatable

**Usage Example:**
Reviewing historical changes to content.

---

#### Case: `"version_display"`
Renders a specific version for preview.

**Parameters:**
- `$version`: Version index

**Mechanics:**
- Loads version data from DB
- Parses and outputs using template engine

**Usage Example:**
Previewing a past version of content.

---

#### Case: `"version_store"`
Stores current content state as a new version.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Calls `$content->version_store()`

**Usage Example:**
Manually creating a checkpoint in content evolution.

---

#### Case: `"version_retrieve"`
Renders form to retrieve a previous version.

**Parameters:**
- `$version`: Version index

**Mechanics:**
- Offers immediate/scheduled retrieval options

**Usage Example:**
Initiating rollback to a prior version.

---

#### Case: `"_version_retrieve"`
Executes version retrieval.

**Parameters:**
- `$ifc_param1`: Mode (0=immediate, 1=scheduled)
- `$ifc_param2`: Scheduled time (if applicable)

**Mechanics:**
- Calls `$content->version_retrieve()` with optional timestamp

**Usage Example:**
Processing version rollback request.

---

#### Case: `"schedule"` / `"schedule_save"` / `"schedule_delete"`
Manages scheduled operations.

**Parameters:**
- `$object`: Content index
- `$time`: Array of scheduled times
- `$list`: List of schedule hashes to delete

**Mechanics:**
- Displays existing schedules
- Allows modification/deletion of scheduled tasks

**Usage Example:**
Managing automated content workflows.

---

#### Case: `"pool"` and related cases
Handles content pool management.

**Parameters:**
- `$pool_object`: Pool item identifier
- `$pool_type`: Type of pool content
- `$pool_language`: Language context

**Mechanics:**
- Manages reusable content snippets
- Supports categorization and multi-language handling

**Usage Example:**
Maintaining a library of reusable content blocks.

---

#### Case: `"rss"` and related cases
Manages RSS feed integration.

**Parameters:**
- `$rss_object`: RSS channel identifier
- `$_list`: Selected channels for assignment

**Mechanics:**
- Creates/edits/deletes RSS channels
- Assigns content to feeds

**Usage Example:**
Configuring syndication for published content.

---

#### Case: `"configuration"`
Renders system configuration form.

**Mechanics:**
- Allows setting extra column labels and IDs
- Requires operator privileges

**Usage Example:**
Customizing content table display fields.

---

#### Case: `"_configuration"`
Saves configuration changes.

**Parameters:**
- `$ifc_param1`: Extra label
- `$ifc_param2`: Extra ID

**Mechanics:**
- Updates system settings via `$system->setval()`

**Usage Example:**
Persisting custom field configurations.

---

#### Case: `"flag"`
Displays content flag settings.

**Parameters:**
- `$object`: Content index

**Mechanics:**
- Shows sitemap exclusion and meta robots options
- Requires appropriate permissions

**Usage Example:**
Adjusting SEO-related flags for content.

---

#### Case: `"_flag"`
Saves content flag settings.

**Parameters:**
- `$ifc_param1`: Sitemap exclude flag
- `$ifc_param2`: Noindex flag
- `$ifc_param3`: Nofollow flag

**Mechanics:**
- Combines flags into bitmask
- Calls `$content->flag_set()`

**Usage Example:**
Updating SEO flags for a content item.

---

#### Case: `"analyze"` / `"analyze_span"`
Provides content analysis tools.

**Parameters:**
- `$object`: Content index
- `$span`: Word span size for frequency analysis

**Mechanics:**
- Parses content and performs keyword density analysis
- Highlights keywords in plain text view
- Shows markup/content ratio statistics

**Usage Example:**
Optimizing content for search engines.

---

#### Case: `"debug"`
Displays document structure debug info.

**Mechanics:**
- Requires template operator privileges
- Renders hierarchical tree of document elements

**Usage Example:**
Troubleshooting complex document structures.

---

#### Case: `"reset"`
Clears user-specific content view settings.

**Mechanics:**
- Deletes cached filter, search, order, page, and limit preferences

**Usage Example:**
Restoring default content listing view.

---

## Main Display Section

### Settings Synchronization
```php
cms_cache_sync($object, "content." . CMS_USER . ".object.$user", NULL, TRUE);
```
Syncs user preferences for object, directory, filter, search, order, page, and limit settings.

### Filter and Search Setup
Builds dynamic SQL filters and search conditions based on user selections and content actions.

### Directory Tree Preparation
Uses FlexView to render interactive directory navigation with active item highlighting.

### Object Table Rendering
Generates the main content listing table with:
- Selection checkboxes
- Index numbers
- Sender/author columns (when applicable)
- Title with status/type icons
- Action buttons (edit, apply, revert, version, publish, etc.)
- Extra column data (when configured)
- Pagination controls

### JavaScript Helper Functions
Provides client-side functions for:
- Loading pages (`lp`)
- Applying content (`a1`)
- Authorizing content (`a2`)
- Copying content (`c`)
- Displaying content (`d1`)
- Duplicating content (`d2`)
- Deriving drafts (`dd`)
- Editing content (`e`)
- Handling extra data (`e2`)
- Setting flags (`f`)
- Viewing info (`i`)
- Managing metadata (`m`)
- Ordering results (`o`)
- Publishing content (`p1`)
- Navigating pages (`p2`)
- Reverting content (`r1`)
- Assigning RSS feeds (`r2`)
- Selecting content (`s1`)
- Scheduling content (`s2`)
- Sending content (`s3`)
- Managing versions (`v`, `vs`)
- Withdrawing content (`w`)

### Pagination
Implements server-side pagination with configurable items per page.

### Row Rendering
Dynamically generates table rows with conditional action buttons based on:
- Content status and type
- User permissions
- Version/schedule availability
- Extra column configuration

This comprehensive interface provides full lifecycle management of content within the PWNC platform, from initial creation through publishing, versioning, and eventual archival or deletion.


<!-- HASH:6424f304c5264ea857f9d0095240bf17 -->
