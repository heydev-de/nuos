# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.content.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Content Management System (CMS) - Content Module

This file provides the core content management functionality for the NUOS web platform. It includes constants for content types, statuses, flags, actions, roles, and database schema definitions, as well as utility functions and the primary `content` class for managing website content.

---

## Constants

### Content Types
| Name                     | Value | Description                          |
|--------------------------|-------|--------------------------------------|
| `CMS_CONTENT_TYPE_ORIGINAL` | 1     | Original content created by a user   |
| `CMS_CONTENT_TYPE_DUPLICATE` | 2     | Duplicate of existing content        |
| `CMS_CONTENT_TYPE_COPY`      | 3     | Copy of existing content             |

### Content Statuses
| Name                          | Value | Description                          |
|-------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_STATUS_DRAFT`      | 1     | Content is in draft state            |
| `CMS_CONTENT_STATUS_DOCUMENT`   | 2     | Content is authorized for publication|
| `CMS_CONTENT_STATUS_PUBLICATION`| 3     | Content is published                 |
| `CMS_CONTENT_STATUS_MAIL`       | 4     | Reserved for future use              |
| `CMS_CONTENT_STATUS_POOL`       | 5     | Reserved for future use              |

### Content Flags
| Name                                  | Value          | Description                          |
|---------------------------------------|----------------|--------------------------------------|
| `CMS_CONTENT_FLAG_NONE`               | 0              | No flags set                         |
| `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE`    | 1              | Exclude from sitemap                 |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX`| 2              | Set meta robots to noindex           |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW`| 4             | Set meta robots to nofollow          |
| `CMS_CONTENT_FLAG_ALL`                | 4294967295     | All flags set (bitmask)              |

### Content Actions
| Name                                 | Value | Description                          |
|--------------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_ACTION_NONE`            | 0     | No action                            |
| `CMS_CONTENT_ACTION_CREATE`          | 1     | Create new content                   |
| `CMS_CONTENT_ACTION_UPDATE`          | 2     | Update existing content              |
| `CMS_CONTENT_ACTION_AUTHORIZE`       | 3     | Authorize content for publication    |
| `CMS_CONTENT_ACTION_DERIVE_DRAFT`    | 4     | Derive a draft from existing content |
| `CMS_CONTENT_ACTION_PUBLISH`         | 5     | Publish content                      |
| `CMS_CONTENT_ACTION_WITHDRAW`        | 6     | Withdraw published content           |
| `CMS_CONTENT_ACTION_DUPLICATE`       | 7     | Duplicate content                    |
| `CMS_CONTENT_ACTION_COPY`            | 8     | Copy content                         |
| `CMS_CONTENT_ACTION_DELETE`          | 9     | Delete content                       |
| `CMS_CONTENT_ACTION_RECEIVE`         | 10    | Receive content from another user    |
| `CMS_CONTENT_ACTION_CHANNEL`         | 11    | Set content channel                  |
| `CMS_CONTENT_ACTION_FLAG`            | 12    | Set content flags                    |
| `CMS_CONTENT_ACTION_EXTRA`           | 13    | Set extra content properties         |

### Content Roles
| Name                                      | Value | Description                          |
|-------------------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_ROLE_NONE`                   | 0     | No role                              |
| `CMS_CONTENT_ROLE_WRITER`                 | 1     | Writer role                          |
| `CMS_CONTENT_ROLE_EDITOR`                 | 2     | Editor role                          |
| `CMS_CONTENT_ROLE_PUBLISHER`              | 4     | Publisher role                       |
| `CMS_CONTENT_ROLE_ALL`                    | 7     | All roles (bitmask)                  |
| `CMS_CONTENT_ROLE_WRITER_EDITOR`          | 8     | Writer and Editor roles              |
| `CMS_CONTENT_ROLE_WRITER_PUBLISHER`       | 16    | Writer and Publisher roles           |
| `CMS_CONTENT_ROLE_EDITOR_PUBLISHER`       | 32    | Editor and Publisher roles           |
| `CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER`| 64    | Writer, Editor, and Publisher roles  |

### Schedule Types
| Name                                | Value | Description                          |
|-------------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_SCHEDULE_TYPE_APPLY`   | 1     | Apply content changes                |
| `CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE`| 2     | Retrieve content version             |
| `CMS_CONTENT_SCHEDULE_TYPE_PUBLISH` | 3     | Publish content                      |
| `CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW`| 4     | Withdraw content                     |

### Content Permissions
| Name                              | Value      | Description                          |
|-----------------------------------|------------|--------------------------------------|
| `CMS_CONTENT_PERMISSION_READER`   | "reader"   | Read-only permission                 |
| `CMS_CONTENT_PERMISSION_WRITER`   | "writer"   | Write permission                     |
| `CMS_CONTENT_PERMISSION_EDITOR`   | "editor"   | Edit permission                      |
| `CMS_CONTENT_PERMISSION_PUBLISHER`| "publisher"| Publish permission                   |
| `CMS_CONTENT_PERMISSION_OPERATOR` | "operator" | Operator permission                  |

### Database Schema
| Name                                      | Value                                      | Description                          |
|-------------------------------------------|--------------------------------------------|--------------------------------------|
| `CMS_DB_CONTENT`                          | `CMS_DB_PREFIX . "content"`                | Main content table                   |
| `CMS_DB_CONTENT_INDEX`                    | "id"                                       | Primary key                          |
| `CMS_DB_CONTENT_OWNER`                    | "owner"                                    | Content owner                        |
| `CMS_DB_CONTENT_TYPE`                     | "type"                                     | Content type                         |
| `CMS_DB_CONTENT_STATUS`                   | "status"                                   | Content status                       |
| `CMS_DB_CONTENT_FLAG`                     | "flag"                                     | Content flags                        |
| `CMS_DB_CONTENT_CHANNEL`                  | "channel"                                  | Content channel                      |
| `CMS_DB_CONTENT_WRITER`                   | "writer"                                   | Writer of content                    |
| `CMS_DB_CONTENT_WRITER_TIME`              | "writer_time"                              | Time of last write                   |
| `CMS_DB_CONTENT_WRITER_COMMENT`           | "writer_comment"                           | Writer comment                       |
| `CMS_DB_CONTENT_EDITOR`                   | "editor"                                   | Editor of content                    |
| `CMS_DB_CONTENT_EDITOR_TIME`              | "editor_time"                              | Time of last edit                    |
| `CMS_DB_CONTENT_EDITOR_COMMENT`           | "editor_comment"                           | Editor comment                       |
| `CMS_DB_CONTENT_PUBLISHER`                | "publisher"                                | Publisher of content                 |
| `CMS_DB_CONTENT_PUBLISHER_TIME`           | "publisher_time"                           | Time of last publish                 |
| `CMS_DB_CONTENT_PUBLISHER_COMMENT`        | "publisher_comment"                        | Publisher comment                    |
| `CMS_DB_CONTENT_TIME`                     | "time"                                     | Last modification time               |
| `CMS_DB_CONTENT_TITLE`                    | "title"                                    | Content title                        |
| `CMS_DB_CONTENT_AUTHOR`                   | "author"                                   | Content author                       |
| `CMS_DB_CONTENT_DESCRIPTION`              | "description"                              | Content description                  |
| `CMS_DB_CONTENT_KEYWORD`                  | "keyword"                                  | Content keywords                     |
| `CMS_DB_CONTENT_IMAGE`                    | "image"                                    | Content image                        |
| `CMS_DB_CONTENT_TEXT`                     | "text"                                     | Content text                         |
| `CMS_DB_CONTENT_TEMPLATE`                 | "template"                                 | Content template                     |
| `CMS_DB_CONTENT_BUFFER_*`                 | Various                                    | Buffered content properties          |
| `CMS_DB_CONTENT_SENDER`                   | "sender"                                   | Content sender                       |
| `CMS_DB_CONTENT_SENDER_TIME`              | "sender_time"                              | Time of send                         |
| `CMS_DB_CONTENT_SENDER_COMMENT`           | "sender_comment"                           | Sender comment                       |
| `CMS_DB_CONTENT_EXTRA_VALUE`              | "extra_value"                              | Extra value                          |
| `CMS_DB_CONTENT_EXTRA_TYPE`               | "extra_type"                               | Extra type                           |
| `CMS_DB_CONTENT_EXTRA_COLOR`              | "extra_color"                              | Extra color                          |
| `CMS_DB_CONTENT_VERSION`                  | `CMS_DB_PREFIX . "content_version"`        | Content version table                |
| `CMS_DB_CONTENT_VERSION_INDEX`            | "id"                                       | Version primary key                  |
| `CMS_DB_CONTENT_VERSION_CONTENT`          | "content"                                  | Content ID                           |
| `CMS_DB_CONTENT_VERSION_TIME`             | "time"                                     | Version time                         |
| `CMS_DB_CONTENT_VERSION_*`                | Various                                    | Version properties                   |
| `CMS_DB_CONTENT_VERSION_HASH`             | "hash"                                     | Version hash                         |
| `CMS_DB_CONTENT_SCHEDULE`                 | `CMS_DB_PREFIX . "content_schedule"`       | Content schedule table               |
| `CMS_DB_CONTENT_SCHEDULE_TIME`            | "time"                                     | Schedule time                        |
| `CMS_DB_CONTENT_SCHEDULE_TYPE`            | "type"                                     | Schedule type                        |
| `CMS_DB_CONTENT_SCHEDULE_CONTENT`         | "content"                                  | Content ID                           |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_1`         | "value1"                                   | Schedule value 1                     |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_2`         | "value2"                                   | Schedule value 2                     |
| `CMS_DB_CONTENT_SCHEDULE_HASH`            | "hash"                                     | Schedule hash                        |

---

## Utility Functions

### `content_get_range`
Retrieves a specific range of content from a document.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$content`| `content`| Content object (passed by reference) |
| `$index`  | `int`    | Content index                        |
| `$range`  | `string` | Range identifier                     |
| `$type`   | `string` | Type of range (optional)             |

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `mixed`  | Requested range data or `NULL`       |

#### Inner Mechanisms
- Checks if content is enabled.
- Attempts to retrieve from cache first.
- Falls back to database retrieval if not cached.
- Uses the `document` class to parse and retrieve the range.

#### Usage
```php
$rangeData = content_get_range($content, 123, "body");
```

---

### `content_set_range`
Sets or modifies a specific range of content in a document.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$content`| `content`| Content object (passed by reference) |
| `$index`  | `int`    | Content index                        |
| `$range`  | `string` | Range identifier                     |
| `$type`   | `string` | Operation type                       |
| `$text`   | `string` | Text to set                          |

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `bool`   | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Checks if content is enabled.
- Retrieves document from cache or database.
- Performs the specified operation (`#paste`, `#swap`, `#copy`, `#kick1`, `#kick2`, `#drop1`, `#drop2`, `#clear`, `shift`, or default set).
- Updates the content in the database.

#### Usage
```php
content_set_range($content, 123, "body", "#paste", $pastedText);
```

---

### `content_get_directory_index`
Finds the directory index linked to a specific content index.

#### Parameters
| Name             | Type  | Description                          |
|------------------|-------|--------------------------------------|
| `$content_index` | `int` | Content index                        |

#### Return Values
| Type  | Description                          |
|-------|--------------------------------------|
| `int` | Directory index or `0` if not found  |

#### Inner Mechanisms
- Iterates through the directory data to find a URL linking to the content.
- Uses regex to match content URLs.

#### Usage
```php
$directoryIndex = content_get_directory_index(123);
```

---

### `content_parse`
Parses content and generates output based on its template.

#### Parameters
| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$content`     | `content` | Content object (passed by reference) |
| `$index`       | `int`     | Content index                        |
| `$action`      | `array`   | Template actions (optional)          |
| `$header`      | `array`   | HTTP headers (optional)              |
| `$is_dynamic`  | `bool`    | Dynamic flag (passed by reference)   |
| `$mod_time`    | `int`     | Modification time (passed by reference)|

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `string` | Parsed content or `FALSE` on failure |

#### Inner Mechanisms
- Checks if content is enabled and loads required libraries.
- Retrieves content from database or cache.
- Handles caching of parsed content.
- Uses `template` class to process content.

#### Usage
```php
$output = content_parse($content, 123, [], [], $isDynamic, $modTime);
```

---

### `content_template_export`
Exports a content template for a given content index.

#### Parameters
| Name      | Type      | Description                          |
|-----------|-----------|--------------------------------------|
| `$content`| `content` | Content object (passed by reference) |
| `$index`  | `int`     | Content index                        |
| `$range`  | `string`  | Range identifier (optional)          |

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `string` | Exported template or `FALSE`         |

#### Inner Mechanisms
- Retrieves content and template from database.
- Uses `template` class to export the template.

#### Usage
```php
$template = content_template_export($content, 123);
```

---

### `content_template_select`
Generates a list of available templates for selection.

#### Return Values
| Type    | Description                          |
|---------|--------------------------------------|
| `array` | Associative array of templates       |

#### Inner Mechanisms
- Loads template data and filters for page templates.
- Generates unique names for templates.

#### Usage
```php
$templates = content_template_select();
```

---

### `content_get_receiver`
Retrieves a list of users who can receive content of a specific type and status.

#### Parameters
| Name      | Type      | Description                          |
|-----------|-----------|--------------------------------------|
| `$content`| `content` | Content object (passed by reference) |
| `$type`   | `int`     | Content type                         |
| `$status` | `int`     | Content status                       |

#### Return Values
| Type    | Description                          |
|---------|--------------------------------------|
| `array` | Array of receivers by content type   |

#### Inner Mechanisms
- Checks permissions for each user to receive content.
- Returns users grouped by content type.

#### Usage
```php
$receivers = content_get_receiver($content, CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT);
```

---

## `content` Class

### Overview
The `content` class manages content creation, modification, publication, and versioning. It handles permissions, scheduling, and content state transitions.

### Properties
| Name         | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `action`     | `array`  | Permission action matrix             |
| `user`       | `string` | Current user                         |
| `writer`     | `bool`   | Writer permission flag               |
| `editor`     | `bool`   | Editor permission flag               |
| `publisher`  | `bool`   | Publisher permission flag            |
| `operator`   | `bool`   | Operator permission flag             |
| `enabled`    | `bool`   | Content module enabled flag          |

---

### Constructor
Initializes the content module and sets up permissions.

#### Parameters
| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| `$user`| `string` | User identifier (optional)           |

#### Inner Mechanisms
- Defines the permission matrix for content actions.
- Verifies database tables.
- Processes scheduled actions.
- Sets user permissions.

#### Usage
```php
$content = new content("user123");
```

---

### `test_create`
Tests if content creation is permitted.

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_create()) { /* ... */ }
```

---

### `create`
Creates new content.

#### Parameters
| Name        | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$title`    | `string` | Content title                        |
| `$template` | `string` | Template identifier (optional)       |
| `$comment`  | `string` | Writer comment (optional)            |
| `$test`     | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `int`  | Content index or `FALSE` on failure  |

#### Usage
```php
$contentIndex = $content->create("New Article", "default_template", "Initial draft");
```

---

### `test_update`
Tests if content update is permitted.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_update(123)) { /* ... */ }
```

---

### `update`
Updates existing content.

#### Parameters
| Name           | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$index`       | `int`    | Content index                        |
| `$title`       | `string` | Title (optional)                     |
| `$description` | `string` | Description (optional)               |
| `$keyword`     | `string` | Keywords (optional)                  |
| `$image`       | `string` | Image URL (optional)                 |
| `$text`        | `string` | Content text (optional)              |
| `$comment`     | `string` | Writer comment (optional)            |
| `$template`    | `string` | Template (optional)                  |
| `$test`        | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->update(123, "Updated Title", "New description", "keywords", "image.jpg", "<p>New text</p>");
```

---

### `test_copy`
Tests if content can be copied.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_copy(123)) { /* ... */ }
```

---

### `copy`
Copies content (duplicate or regular copy).

#### Parameters
| Name        | Type   | Description                          |
|-------------|--------|--------------------------------------|
| `$index`    | `int`  | Content index                        |
| `$duplicate`| `bool` | Duplicate flag (optional)            |
| `$test`     | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `int`  | New content index or `FALSE`         |

#### Usage
```php
$newIndex = $content->copy(123, true); // Duplicate
```

---

### `test_duplicate`
Tests if content can be duplicated.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_duplicate(123)) { /* ... */ }
```

---

### `duplicate`
Duplicates content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `int`  | New content index or `FALSE`         |

#### Usage
```php
$newIndex = $content->duplicate(123);
```

---

### `test_authorize`
Tests if content can be authorized for publication.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_authorize(123)) { /* ... */ }
```

---

### `authorize`
Authorizes content for publication.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$index`  | `int`    | Content index                        |
| `$comment`| `string` | Editor comment (optional)            |
| `$test`   | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->authorize(123, "Approved for publication");
```

---

### `test_derive_draft`
Tests if a draft can be derived from content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_derive_draft(123)) { /* ... */ }
```

---

### `derive_draft`
Derives a draft from existing content.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->derive_draft(123);
```

---

### `test_publish`
Tests if content can be published.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_publish(123)) { /* ... */ }
```

---

### `publish`
Publishes content immediately or schedules publication.

#### Parameters
| Name               | Type     | Description                          |
|--------------------|----------|--------------------------------------|
| `$index`           | `int`    | Content index                        |
| `$comment`         | `string` | Publisher comment (optional)         |
| `$time_publish`    | `int`    | Scheduled publish time (optional)    |
| `$time_withdraw`   | `int`    | Scheduled withdraw time (optional)   |
| `$directory_index` | `int`    | Directory index (optional)           |
| `$directory_action`| `string` | Directory action (optional)          |
| `$directory_title` | `string` | Directory title (optional)           |
| `$test`            | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->publish(123, "Published", 0, 0, 456, "replace", "New Article");
```

---

### `test_apply`
Tests if content changes can be applied.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_apply(123)) { /* ... */ }
```

---

### `apply`
Applies buffered content changes immediately or schedules them.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |
| `$time` | `int` | Scheduled apply time (optional)      |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->apply(123);
```

---

### `test_revert`
Tests if content can be reverted to its last applied state.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_revert(123)) { /* ... */ }
```

---

### `revert`
Reverts content to its last applied state.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->revert(123);
```

---

### `test_version_store`
Tests if a content version can be stored.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_version_store(123)) { /* ... */ }
```

---

### `version_store`
Stores a version of the content.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->version_store(123);
```

---

### `_version_store`
Internal method to store a content version in the database.

#### Parameters
| Name           | Type  | Description                          |
|----------------|-------|--------------------------------------|
| `$version_index`| `int`| Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->_version_store(123);
```

---

### `test_version_retrieve`
Tests if a content version can be retrieved.

#### Parameters
| Name           | Type  | Description                          |
|----------------|-------|--------------------------------------|
| `$version_index`| `int`| Version index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_version_retrieve(456)) { /* ... */ }
```

---

### `version_retrieve`
Retrieves a content version immediately or schedules retrieval.

#### Parameters
| Name           | Type   | Description                          |
|----------------|--------|--------------------------------------|
| `$version_index`| `int` | Version index                        |
| `$time`        | `int`  | Scheduled retrieval time (optional)  |
| `$apply`       | `bool` | Apply immediately flag (optional)    |
| `$test`        | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->version_retrieve(456, 0, true);
```

---

### `schedule_add`
Adds a scheduled action for content.

#### Parameters
| Name           | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$time`        | `int`    | Scheduled time                       |
| `$type`        | `int`    | Schedule type                        |
| `$content_index`| `int`   | Content index                        |
| `$value1`      | `string` | Additional value 1 (optional)        |
| `$value2`      | `string` | Additional value 2 (optional)        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->schedule_add(time() + 3600, CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, 123);
```

---

### `schedule_delete`
Deletes a scheduled action.

#### Parameters
| Name           | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$type`        | `int`    | Schedule type                        |
| `$content_index`| `int`   | Content index                        |
| `$value1`      | `string` | Additional value 1 (optional)        |
| `$value2`      | `string` | Additional value 2 (optional)        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->schedule_delete(CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, 123);
```

---

### `_schedule_delete`
Internal method to delete a scheduled action by hash.

#### Parameters
| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| `$hash`| `string` | Schedule hash                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->_schedule_delete("abc123");
```

---

### `test_withdraw`
Tests if content can be withdrawn from publication.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_withdraw(123)) { /* ... */ }
```

---

### `withdraw`
Withdraws content from publication.

#### Parameters
| Name             | Type     | Description                          |
|------------------|----------|--------------------------------------|
| `$index`         | `int`    | Content index                        |
| `$directory_index`| `int`   | Directory index (optional)           |
| `$test`          | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->withdraw(123, 456);
```

---

### `_withdraw`
Internal method to withdraw content and remove directory links.

#### Parameters
| Name             | Type  | Description                          |
|------------------|-------|--------------------------------------|
| `$index`         | `int` | Content index                        |
| `$directory_index`| `int`| Directory index (optional)           |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->_withdraw(123, 456);
```

---

### `test_send`
Tests if content can be sent to another user.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_send(123)) { /* ... */ }
```

---

### `send`
Sends content to another user.

#### Parameters
| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$index`   | `int`    | Content index                        |
| `$receiver`| `string` | Receiver user identifier             |
| `$comment` | `string` | Sender comment (optional)            |
| `$test`    | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->send(123, "user456", "Please review this content");
```

---

### `test_delete`
Tests if content can be deleted.

#### Parameters
| Name               | Type   | Description                          |
|--------------------|--------|--------------------------------------|
| `$index`           | `int`  | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_delete(123)) { /* ... */ }
```

---

### `delete`
Deletes content.

#### Parameters
| Name                | Type   | Description                          |
|---------------------|--------|--------------------------------------|
| `$index`            | `int`  | Content index                        |
| `$ignore_directory` | `bool` | Ignore directory links (optional)    |
| `$override_owner`   | `bool` | Override owner check (optional)      |
| `$test`             | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->delete(123);
```

---

### `test_flag_set`
Tests if content flags can be set.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_flag_set(123)) { /* ... */ }
```

---

### `flag_set`
Sets content flags.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |
| `$flag` | `int` | Flag value (optional)                |
| `$test` | `bool`| Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->flag_set(123, CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX);
```

---

### `test_channel_set`
Tests if content channel can be set.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_channel_set(123)) { /* ... */ }
```

---

### `channel_set`
Sets content channel.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$index`  | `int`    | Content index                        |
| `$channel`| `string` | Channel identifier (optional)        |
| `$test`   | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->channel_set(123, "news");
```

---

### `step_store`
Stores the current state of content for undo/redo functionality.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->step_store(123);
```

---

### `test_step_undo`
Tests if an undo operation is possible.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_step_undo(123)) { /* ... */ }
```

---

### `step_undo`
Reverts content to the previous state.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->step_undo(123);
```

---

### `step_undo_depth`
Returns the number of available undo steps.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type  | Description                          |
|-------|--------------------------------------|
| `int` | Number of undo steps                 |

#### Usage
```php
$depth = $content->step_undo_depth(123);
```

---

### `test_step_redo`
Tests if a redo operation is possible.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_step_redo(123)) { /* ... */ }
```

---

### `step_redo`
Redoes the last undone content state.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->step_redo(123);
```

---

### `step_redo_depth`
Returns the number of available redo steps.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type  | Description                          |
|-------|--------------------------------------|
| `int` | Number of redo steps                 |

#### Usage
```php
$depth = $content->step_redo_depth(123);
```

---

### `step_clear`
Clears the undo/redo buffer for content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->step_clear(123);
```

---

### `action`
Checks if a user has permission to perform an action on content.

#### Parameters
| Name      | Type  | Description                          |
|-----------|-------|--------------------------------------|
| `$type`   | `int` | Content type                         |
| `$status` | `int` | Content status                       |
| `$action` | `int` | Action to perform                    |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_UPDATE)) { /* ... */ }
```

---

### `refresh_extra`
Refreshes extra content properties from the document.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->refresh_extra(123);
```

---

### `test_set_extra`
Tests if extra content properties can be set.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if permitted, `FALSE` otherwise|

#### Usage
```php
if ($content->test_set_extra(123)) { /* ... */ }
```

---

### `set_extra`
Sets extra content properties.

#### Parameters
| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$index`| `int`    | Content index                        |
| `$value`| `string` | Extra value (optional)               |
| `$type` | `string` | Extra type (optional)                |
| `$color`| `string` | Extra color (optional)               |
| `$test` | `bool`   | Test mode flag (optional)            |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage
```php
$content->set_extra(123, "extra_value", "value", "#ff0000");
```


<!-- HASH:f9011e08ff5f8b46c01710182539f0e6 -->
