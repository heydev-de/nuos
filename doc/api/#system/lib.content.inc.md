# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.content.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.content.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Management System (CMS) - Content Module

The `lib.content.inc` file provides the core content management functionality for the PWNC Web Platform. It defines constants, utility functions, and the `content` class for handling content creation, modification, versioning, publishing, and access control.

---

## Constants

### Content Types
| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_TYPE_ORIGINAL` | `1` | Original content created by a user. |
| `CMS_CONTENT_TYPE_DUPLICATE` | `2` | Duplicate of an original content item. |
| `CMS_CONTENT_TYPE_COPY` | `3` | Copy of an original content item. |

### Content Statuses
| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_STATUS_DRAFT` | `1` | Content is in draft state. |
| `CMS_CONTENT_STATUS_DOCUMENT` | `2` | Content is authorized but not published. |
| `CMS_CONTENT_STATUS_PUBLICATION` | `3` | Content is published. |
| `CMS_CONTENT_STATUS_MAIL` | `4` | Reserved for future use. |
| `CMS_CONTENT_STATUS_POOL` | `5` | Reserved for future use. |

### Content Flags
| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_FLAG_NONE` | `0` | No flags set. |
| `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE` | `1` | Exclude content from sitemap. |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX` | `2` | Set `noindex` meta robots tag. |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW` | `4` | Set `nofollow` meta robots tag. |
| `CMS_CONTENT_FLAG_ALL` | `4294967295` | All flags set. |

### Content Actions
| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_ACTION_NONE` | `0` | No action. |
| `CMS_CONTENT_ACTION_CREATE` | `1` | Create new content. |
| `CMS_CONTENT_ACTION_UPDATE` | `2` | Update existing content. |
| `CMS_CONTENT_ACTION_AUTHORIZE` | `3` | Authorize content for publication. |
| `CMS_CONTENT_ACTION_DERIVE_DRAFT` | `4` | Derive a draft from existing content. |
| `CMS_CONTENT_ACTION_PUBLISH` | `5` | Publish content. |
| `CMS_CONTENT_ACTION_WITHDRAW` | `6` | Withdraw published content. |
| `CMS_CONTENT_ACTION_DUPLICATE` | `7` | Duplicate content. |
| `CMS_CONTENT_ACTION_COPY` | `8` | Copy content. |
| `CMS_CONTENT_ACTION_DELETE` | `9` | Delete content. |
| `CMS_CONTENT_ACTION_RECEIVE` | `10` | Receive content from another user. |
| `CMS_CONTENT_ACTION_CHANNEL` | `11` | Set content channel. |
| `CMS_CONTENT_ACTION_FLAG` | `12` | Set content flags. |
| `CMS_CONTENT_ACTION_EXTRA` | `13` | Set extra content metadata. |

### Content Roles
| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_ROLE_NONE` | `0` | No role. |
| `CMS_CONTENT_ROLE_WRITER` | `1` | Writer role. |
| `CMS_CONTENT_ROLE_EDITOR` | `2` | Editor role. |
| `CMS_CONTENT_ROLE_PUBLISHER` | `4` | Publisher role. |
| `CMS_CONTENT_ROLE_ALL` | `7` | All roles combined. |
| `CMS_CONTENT_ROLE_WRITER_EDITOR` | `8` | Writer and editor roles. |
| `CMS_CONTENT_ROLE_WRITER_PUBLISHER` | `16` | Writer and publisher roles. |
| `CMS_CONTENT_ROLE_EDITOR_PUBLISHER` | `32` | Editor and publisher roles. |
| `CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER` | `64` | All roles combined. |

### Schedule Types
| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_SCHEDULE_TYPE_APPLY` | `1` | Apply buffered changes to content. |
| `CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE` | `2` | Retrieve a content version. |
| `CMS_CONTENT_SCHEDULE_TYPE_PUBLISH` | `3` | Publish content. |
| `CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW` | `4` | Withdraw published content. |

### Content Permissions
| Name | Value | Description |
|------|-------|-------------|
| `CMS_CONTENT_PERMISSION_READER` | `"reader"` | Read-only access. |
| `CMS_CONTENT_PERMISSION_WRITER` | `"writer"` | Write access. |
| `CMS_CONTENT_PERMISSION_EDITOR` | `"editor"` | Edit access. |
| `CMS_CONTENT_PERMISSION_PUBLISHER` | `"publisher"` | Publish access. |
| `CMS_CONTENT_PERMISSION_OPERATOR` | `"operator"` | Full access. |

### Database Tables and Fields
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_CONTENT` | `CMS_DB_PREFIX . "content"` | Main content table. |
| `CMS_DB_CONTENT_INDEX` | `"id"` | Primary key. |
| `CMS_DB_CONTENT_OWNER` | `"owner"` | Content owner. |
| `CMS_DB_CONTENT_TYPE` | `"type"` | Content type (original, duplicate, copy). |
| `CMS_DB_CONTENT_STATUS` | `"status"` | Content status (draft, document, publication). |
| `CMS_DB_CONTENT_FLAG` | `"flag"` | Content flags. |
| `CMS_DB_CONTENT_CHANNEL` | `"channel"` | Content channel. |
| `CMS_DB_CONTENT_WRITER` | `"writer"` | Writer username. |
| `CMS_DB_CONTENT_WRITER_TIME` | `"writer_time"` | Last write timestamp. |
| `CMS_DB_CONTENT_WRITER_COMMENT` | `"writer_comment"` | Writer comment. |
| `CMS_DB_CONTENT_EDITOR` | `"editor"` | Editor username. |
| `CMS_DB_CONTENT_EDITOR_TIME` | `"editor_time"` | Last edit timestamp. |
| `CMS_DB_CONTENT_EDITOR_COMMENT` | `"editor_comment"` | Editor comment. |
| `CMS_DB_CONTENT_PUBLISHER` | `"publisher"` | Publisher username. |
| `CMS_DB_CONTENT_PUBLISHER_TIME` | `"publisher_time"` | Last publish timestamp. |
| `CMS_DB_CONTENT_PUBLISHER_COMMENT` | `"publisher_comment"` | Publisher comment. |
| `CMS_DB_CONTENT_TIME` | `"time"` | Last modification timestamp. |
| `CMS_DB_CONTENT_TITLE` | `"title"` | Content title. |
| `CMS_DB_CONTENT_AUTHOR` | `"author"` | Content author. |
| `CMS_DB_CONTENT_DESCRIPTION` | `"description"` | Content description. |
| `CMS_DB_CONTENT_KEYWORD` | `"keyword"` | Content keywords. |
| `CMS_DB_CONTENT_IMAGE` | `"image"` | Content image. |
| `CMS_DB_CONTENT_TEXT` | `"text"` | Content text. |
| `CMS_DB_CONTENT_TEMPLATE` | `"template"` | Content template. |
| `CMS_DB_CONTENT_BUFFER_*` | Various | Buffered content fields. |
| `CMS_DB_CONTENT_SENDER` | `"sender"` | Content sender. |
| `CMS_DB_CONTENT_SENDER_TIME` | `"sender_time"` | Send timestamp. |
| `CMS_DB_CONTENT_SENDER_COMMENT` | `"sender_comment"` | Sender comment. |
| `CMS_DB_CONTENT_EXTRA_VALUE` | `"extra_value"` | Extra metadata value. |
| `CMS_DB_CONTENT_EXTRA_TYPE` | `"extra_type"` | Extra metadata type. |
| `CMS_DB_CONTENT_EXTRA_COLOR` | `"extra_color"` | Extra metadata color. |
| `CMS_DB_CONTENT_VERSION` | `CMS_DB_PREFIX . "content_version"` | Content version table. |
| `CMS_DB_CONTENT_VERSION_INDEX` | `"id"` | Version primary key. |
| `CMS_DB_CONTENT_VERSION_CONTENT` | `"content"` | Content ID. |
| `CMS_DB_CONTENT_VERSION_TIME` | `"time"` | Version timestamp. |
| `CMS_DB_CONTENT_VERSION_*` | Various | Versioned content fields. |
| `CMS_DB_CONTENT_VERSION_HASH` | `"hash"` | Version hash. |
| `CMS_DB_CONTENT_SCHEDULE` | `CMS_DB_PREFIX . "content_schedule"` | Content schedule table. |
| `CMS_DB_CONTENT_SCHEDULE_TIME` | `"time"` | Scheduled time. |
| `CMS_DB_CONTENT_SCHEDULE_TYPE` | `"type"` | Schedule type. |
| `CMS_DB_CONTENT_SCHEDULE_CONTENT` | `"content"` | Content ID. |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_1` | `"value1"` | Schedule value 1. |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_2` | `"value2"` | Schedule value 2. |
| `CMS_DB_CONTENT_SCHEDULE_HASH` | `"hash"` | Schedule hash. |

---

## Utility Functions

### `content_get_range`

Retrieves a specific range of content from a document.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object (passed by reference). |
| `$index` | `int` | Content ID. |
| `$range` | `string` | Document range to retrieve. |
| `$type` | `string` | Document type (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `mixed` | Retrieved content or `NULL` if not found. |

#### Inner Mechanisms
- Checks if the content object is enabled.
- Attempts to retrieve content from cache first.
- Falls back to database retrieval if not cached.
- Uses the `document` class to parse and retrieve the specified range.

#### Usage Example
```php
$content = new content();
$text = content_get_range($content, 123, "body");
if ($text !== NULL) {
    echo "Retrieved content: " . q($text);
}
```

---

### `content_set_range`

Updates a specific range of content in a document.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object (passed by reference). |
| `$index` | `int` | Content ID. |
| `$range` | `string` | Document range to update. |
| `$type` | `string` | Update type (e.g., `#paste`, `#swap`). |
| `$text` | `string` | New content. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
- Checks if the content object is enabled.
- Retrieves the document from cache or database.
- Applies the specified update type to the document.
- Updates the content in the database.

#### Usage Example
```php
$content = new content();
if (content_set_range($content, 123, "body", "#paste", "New content")) {
    echo "Content updated successfully.";
}
```

---

### `content_get_directory_index`

Finds the directory index linked to a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$content_index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Directory index or `0` if not found. |

#### Inner Mechanisms
- Iterates through the directory data to find a URL matching `content://<ID>`.
- Returns the first matching directory index.

#### Usage Example
```php
$directory_index = content_get_directory_index(123);
if ($directory_index > 0) {
    echo "Content is linked to directory: " . $directory_index;
}
```

---

### `content_parse`

Parses content and generates output using a template.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object (passed by reference). |
| `$index` | `int` | Content ID. |
| `$action` | `array` | Template actions (optional). |
| `$header` | `array` | HTTP headers (optional). |
| `$is_dynamic` | `bool` | Outputs whether content is dynamic (passed by reference). |
| `$mod_time` | `int` | Outputs last modification time (passed by reference). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Parsed content output. |

#### Inner Mechanisms
- Checks if the content object is enabled.
- Loads required libraries (`document`, `template`).
- Retrieves content from the database.
- Sets meta robots tags based on content flags.
- Uses caching to optimize performance.
- Processes the content using the `template` class.

#### Usage Example
```php
$content = new content();
$is_dynamic = FALSE;
$mod_time = 0;
$output = content_parse($content, 123, NULL, NULL, $is_dynamic, $mod_time);
echo $output;
```

---

### `content_template_export`

Exports a content template.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object (passed by reference). |
| `$index` | `int` | Content ID. |
| `$range` | `string` | Document range to export (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Exported template or `FALSE` on failure. |

#### Inner Mechanisms
- Retrieves the content document and template from the database.
- Uses the `template` class to export the specified range.

#### Usage Example
```php
$content = new content();
$template = content_template_export($content, 123);
if ($template !== FALSE) {
    file_put_contents("template.html", $template);
}
```

---

### `content_template_select`

Generates a list of available templates for selection.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array of template names and keys. |

#### Inner Mechanisms
- Loads template data.
- Filters templates with a `page` attribute.
- Organizes templates by category.

#### Usage Example
```php
$templates = content_template_select();
foreach ($templates as $name => $key) {
    echo "<option value='" . q($key) . "'>" . q($name) . "</option>";
}
```

---

### `content_get_receiver`

Retrieves a list of users who can receive content.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object (passed by reference). |
| `$type` | `int` | Content type. |
| `$status` | `int` | Content status. |

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array of receivers by content type. |

#### Inner Mechanisms
- Checks if the content object is enabled.
- Iterates through users to determine who can receive the content.
- Uses permission checks to validate actions.

#### Usage Example
```php
$content = new content();
$receivers = content_get_receiver($content, CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT);
foreach ($receivers[CMS_CONTENT_TYPE_ORIGINAL] as $user => $flag) {
    echo "User " . q($user) . " can receive content.<br>";
}
```

---

## `content` Class

The `content` class manages content lifecycle, permissions, and database operations.

### Properties
| Name | Type | Description |
|------|------|-------------|
| `$action` | `array` | Permission matrix for actions. |
| `$user` | `string` | Current user. |
| `$writer` | `bool` | Writer permission flag. |
| `$editor` | `bool` | Editor permission flag. |
| `$publisher` | `bool` | Publisher permission flag. |
| `$operator` | `bool` | Operator permission flag. |
| `$enabled` | `bool` | Whether the content module is enabled. |

---

### `__construct`

Initializes the content module and sets up permissions.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | User to initialize permissions for (optional). |

#### Inner Mechanisms
- Sets up the permission matrix for content actions.
- Verifies and creates database tables if necessary.
- Processes scheduled content actions.
- Sets user permissions based on the provided user.

#### Usage Example
```php
$content = new content("admin");
if ($content->enabled) {
    echo "Content module initialized for user: " . q($content->user);
}
```

---

### `test_create`

Tests if a new content item can be created.

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if creation is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_create()) {
    echo "User can create content.";
}
```

---

### `create`

Creates a new content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$title` | `string` | Content title. |
| `$template` | `string` | Template key (optional). |
| `$comment` | `string` | Writer comment (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Content ID on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates creation permissions.
- Sets default values for new content.
- Inserts a new record into the database.

#### Usage Example
```php
$content = new content();
$content_id = $content->create("New Article", "default_template", "Initial draft");
if ($content_id !== FALSE) {
    echo "Content created with ID: " . $content_id;
}
```

---

### `test_update`

Tests if a content item can be updated.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if update is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_update(123)) {
    echo "User can update content.";
}
```

---

### `update`

Updates a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$title` | `string` | New title (optional). |
| `$description` | `string` | New description (optional). |
| `$keyword` | `string` | New keywords (optional). |
| `$image` | `string` | New image (optional). |
| `$text` | `string` | New text (optional). |
| `$comment` | `string` | Writer comment (optional). |
| `$template` | `string` | New template (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates update permissions.
- Stores the current state in the step buffer.
- Updates the content in the database.

#### Usage Example
```php
$content = new content();
if ($content->update(123, "Updated Title", "New description", "keywords", NULL, "Updated text")) {
    echo "Content updated successfully.";
}
```

---

### `test_copy`

Tests if a content item can be copied.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if copy is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_copy(123)) {
    echo "User can copy content.";
}
```

---

### `copy`

Copies a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$duplicate` | `bool` | Whether to create a duplicate (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | New content ID on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates copy permissions.
- Creates a new content item with the same data.
- Optionally duplicates version history.

#### Usage Example
```php
$content = new content();
$new_id = $content->copy(123, TRUE);
if ($new_id !== FALSE) {
    echo "Content duplicated with ID: " . $new_id;
}
```

---

### `test_duplicate`

Tests if a content item can be duplicated.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if duplication is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_duplicate(123)) {
    echo "User can duplicate content.";
}
```

---

### `duplicate`

Duplicates a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | New content ID on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
$new_id = $content->duplicate(123);
if ($new_id !== FALSE) {
    echo "Content duplicated with ID: " . $new_id;
}
```

---

### `test_authorize`

Tests if a content item can be authorized.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if authorization is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_authorize(123)) {
    echo "User can authorize content.";
}
```

---

### `authorize`

Authorizes a content item for publication.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$comment` | `string` | Editor comment (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->authorize(123, "Approved for publication")) {
    echo "Content authorized successfully.";
}
```

---

### `test_derive_draft`

Tests if a draft can be derived from a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if derivation is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_derive_draft(123)) {
    echo "User can derive a draft.";
}
```

---

### `derive_draft`

Derives a draft from a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->derive_draft(123)) {
    echo "Draft derived successfully.";
}
```

---

### `test_publish`

Tests if a content item can be published.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if publication is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_publish(123)) {
    echo "User can publish content.";
}
```

---

### `publish`

Publishes a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$comment` | `string` | Publisher comment (optional). |
| `$time_publish` | `int` | Scheduled publish time (optional). |
| `$time_withdraw` | `int` | Scheduled withdraw time (optional). |
| `$directory_index` | `int` | Directory index (optional). |
| `$directory_action` | `string` | Directory action (`replace`, `insert`, `append`) (optional). |
| `$directory_title` | `string` | Directory title (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Directory index on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates publication permissions.
- Handles immediate or scheduled publication.
- Links content to a directory if specified.

#### Usage Example
```php
$content = new content();
if ($content->publish(123, "Published", 0, 0, 456, "replace")) {
    echo "Content published successfully.";
}
```

---

### `test_apply`

Tests if buffered changes can be applied to a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if application is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_apply(123)) {
    echo "User can apply changes.";
}
```

---

### `apply`

Applies buffered changes to a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$time` | `int` | Scheduled apply time (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates apply permissions.
- Handles immediate or scheduled application.
- Stores a version of the content before applying changes.

#### Usage Example
```php
$content = new content();
if ($content->apply(123)) {
    echo "Changes applied successfully.";
}
```

---

### `test_revert`

Tests if a content item can be reverted to its last applied state.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if reversion is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_revert(123)) {
    echo "User can revert content.";
}
```

---

### `revert`

Reverts a content item to its last applied state.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->revert(123)) {
    echo "Content reverted successfully.";
}
```

---

### `test_version_store`

Tests if a version of a content item can be stored.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if version storage is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_version_store(123)) {
    echo "User can store a version.";
}
```

---

### `version_store`

Stores a version of a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->version_store(123)) {
    echo "Version stored successfully.";
}
```

---

### `_version_store`

Internal method to store a content version in the database.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$version_index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

---

### `test_version_retrieve`

Tests if a content version can be retrieved.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$version_index` | `int` | Version ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if retrieval is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_version_retrieve(456)) {
    echo "User can retrieve version.";
}
```

---

### `version_retrieve`

Retrieves a content version.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$version_index` | `int` | Version ID. |
| `$time` | `int` | Scheduled retrieval time (optional). |
| `$apply` | `bool` | Whether to apply the version immediately (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates retrieval permissions.
- Handles immediate or scheduled retrieval.
- Optionally applies the version to the content.

#### Usage Example
```php
$content = new content();
if ($content->version_retrieve(456, 0, TRUE)) {
    echo "Version retrieved and applied successfully.";
}
```

---

### `schedule_add`

Adds a scheduled content action.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$time` | `int` | Scheduled time. |
| `$type` | `int` | Schedule type. |
| `$content_index` | `int` | Content ID. |
| `$value1` | `string` | Additional value 1 (optional). |
| `$value2` | `string` | Additional value 2 (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->schedule_add(time() + 3600, CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, 123)) {
    echo "Content scheduled for publication.";
}
```

---

### `schedule_delete`

Deletes a scheduled content action.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Schedule type. |
| `$content_index` | `int` | Content ID. |
| `$value1` | `string` | Additional value 1 (optional). |
| `$value2` | `string` | Additional value 2 (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->schedule_delete(CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, 123)) {
    echo "Scheduled publication deleted.";
}
```

---

### `_schedule_delete`

Internal method to delete a scheduled action by hash.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$hash` | `string` | Schedule hash. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

---

### `test_withdraw`

Tests if a content item can be withdrawn.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if withdrawal is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_withdraw(123)) {
    echo "User can withdraw content.";
}
```

---

### `withdraw`

Withdraws a published content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$directory_index` | `int` | Directory index (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates withdrawal permissions.
- Removes content from the directory if specified.

#### Usage Example
```php
$content = new content();
if ($content->withdraw(123)) {
    echo "Content withdrawn successfully.";
}
```

---

### `_withdraw`

Internal method to withdraw content and remove directory links.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$directory_index` | `int` | Directory index (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

---

### `test_send`

Tests if a content item can be sent to another user.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if sending is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_send(123)) {
    echo "User can send content.";
}
```

---

### `send`

Sends a content item to another user.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$receiver` | `string` | Receiver username. |
| `$comment` | `string` | Sender comment (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->send(123, "editor_user", "Please review this content")) {
    echo "Content sent successfully.";
}
```

---

### `test_delete`

Tests if a content item can be deleted.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if deletion is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_delete(123)) {
    echo "User can delete content.";
}
```

---

### `delete`

Deletes a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$ignore_directory` | `bool` | Whether to ignore directory links (optional). |
| `$override_owner` | `bool` | Whether to override owner check (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
- Validates deletion permissions.
- Optionally removes directory links.
- Deletes content, versions, and schedules from the database.

#### Usage Example
```php
$content = new content();
if ($content->delete(123)) {
    echo "Content deleted successfully.";
}
```

---

### `test_flag_set`

Tests if content flags can be set.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if flag setting is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_flag_set(123)) {
    echo "User can set flags.";
}
```

---

### `flag_set`

Sets content flags.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$flag` | `int` | Flag value (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->flag_set(123, CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX)) {
    echo "Flag set successfully.";
}
```

---

### `test_channel_set`

Tests if a content channel can be set.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if channel setting is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_channel_set(123)) {
    echo "User can set channel.";
}
```

---

### `channel_set`

Sets a content channel.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$channel` | `string` | Channel name (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->channel_set(123, "news")) {
    echo "Channel set successfully.";
}
```

---

### `step_store`

Stores the current state of a content item in the step buffer.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->step_store(123)) {
    echo "State stored successfully.";
}
```

---

### `test_step_undo`

Tests if an undo operation can be performed.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if undo is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_step_undo(123)) {
    echo "User can undo changes.";
}
```

---

### `step_undo`

Reverts to the previous state of a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->step_undo(123)) {
    echo "Undo successful.";
}
```

---

### `step_undo_depth`

Returns the number of available undo steps.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Number of available undo steps. |

#### Usage Example
```php
$content = new content();
$depth = $content->step_undo_depth(123);
echo "Available undo steps: " . $depth;
```

---

### `test_step_redo`

Tests if a redo operation can be performed.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if redo is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_step_redo(123)) {
    echo "User can redo changes.";
}
```

---

### `step_redo`

Redoes the next state of a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->step_redo(123)) {
    echo "Redo successful.";
}
```

---

### `step_redo_depth`

Returns the number of available redo steps.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | Number of available redo steps. |

#### Usage Example
```php
$content = new content();
$depth = $content->step_redo_depth(123);
echo "Available redo steps: " . $depth;
```

---

### `step_clear`

Clears the step buffer for a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->step_clear(123)) {
    echo "Step buffer cleared.";
}
```

---

### `action`

Checks if a user has permission to perform an action on a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Content type. |
| `$status` | `int` | Content status. |
| `$action` | `int` | Action to check. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the action is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_UPDATE)) {
    echo "User can update draft content.";
}
```

---

### `refresh_extra`

Refreshes extra metadata from the content document.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->refresh_extra(123)) {
    echo "Extra metadata refreshed.";
}
```

---

### `test_set_extra`

Tests if extra metadata can be set.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if setting is allowed, `FALSE` otherwise. |

#### Usage Example
```php
$content = new content();
if ($content->test_set_extra(123)) {
    echo "User can set extra metadata.";
}
```

---

### `set_extra`

Sets extra metadata for a content item.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Content ID. |
| `$value` | `string` | Extra value (optional). |
| `$type` | `string` | Extra type (optional). |
| `$color` | `string` | Extra color (optional). |
| `$test` | `bool` | Test mode (optional). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$content = new content();
if ($content->set_extra(123, "highlight", "tag", "#FF0000")) {
    echo "Extra metadata set successfully.";
}
```


<!-- HASH:c5f01c5d2524e9df8b4b4868fee32777 -->
