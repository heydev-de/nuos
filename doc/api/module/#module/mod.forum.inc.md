# NUOS API Documentation

[← Index](../../README.md) | [`module/#module/mod.forum.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Forum Module (`mod.forum.inc`)

This file implements the **Forum** module for the NUOS web platform, providing a hierarchical discussion system with topics, posts, and replies. It handles:

- **Displaying** forum content (overview, topics, posts, search results, user contributions)
- **Editing** posts (add, edit, delete, move)
- **Navigation** through forum hierarchy
- **User interaction** (search, pagination, access control)

The module integrates with the platform's template system, permission management, and utility functions to deliver a secure, multilingual, and extensible forum experience.

---

### Global Variables

| Name                  | Type     | Description                                                                                     |
|-----------------------|----------|-------------------------------------------------------------------------------------------------|
| `$forum_message`      | string   | Current action being performed (e.g., `"add"`, `"edit"`, `"delete"`).                          |
| `$forum_index`        | string   | Unique identifier of the current forum post/topic.                                             |
| `$forum_buffer`       | string   | Temporary storage for post IDs during move operations (cut/paste).                            |
| `$forum_search`       | string   | Search query for filtering posts.                                                              |
| `$forum_user`         | string   | User identifier for displaying contributions by a specific user.                               |
| `$forum_page`         | string   | Current pagination page number.                                                                |
| `$forum_edit_*`       | mixed    | Edit form fields (`title`, `text`, `email`).                                                   |

---

### Initialization

#### Module Setup
1. **Library Loading**: Checks if the `forum` library is available. If not, displays an "unavailable" message.
2. **Template Integration**: Merges forum data into the active template's `query_data`.
3. **Instance Handling**: Resolves the forum instance (defaults to `CMS_CONTENT_INDEX` if not specified).
4. **Access Control**: Validates user permissions (`reader`, `writer`, `operator`) before proceeding.

---

### Class: `forum`

The `forum` class (defined in the `forum` library) encapsulates all forum-related logic, including:

- **Database operations** (CRUD for posts)
- **Permission checks** (`test_add`, `test_edit`, `test_delete`, `test_move`)
- **Search functionality**
- **Access logging**

---

### Message Handling

The module processes user actions via the `$forum_message` variable, which triggers state transitions:

| State          | Action                                                                                     |
|----------------|--------------------------------------------------------------------------------------------|
| `add`          | Prepares a new post (quotes parent post if replying). Transitions to `_add` (edit form).   |
| `edit`         | Loads an existing post for editing. Transitions to `_edit` (edit form).                   |
| `_add`/`_edit` | Displays the edit form with validation.                                                    |
| `__add`/`__edit`| Shows a preview of the post before saving.                                                 |
| `___add`       | Saves a new post and redirects to prevent duplicate submissions.                           |
| `____add`      | Confirms successful post creation.                                                         |
| `___edit`      | Saves edits to an existing post.                                                           |
| `insert`       | Moves a post (cut/paste operation).                                                        |
| `delete`       | Removes a post and redirects to its parent.                                                |

---

### Key Functions

#### `command($index)`
**Purpose**: Renders action buttons (cut, paste, delete, edit, add) for a post/topic.

**Parameters**:
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| string | Post/topic ID to generate commands for. |

**Inner Mechanisms**:
- Checks user permissions (`$forum->operator`, `test_*` methods) to determine available actions.
- Uses `cms_url()` to generate secure, CSRF-protected links for each action.

**Usage**:
```php
$command($post_id); // Renders buttons for the specified post.
```

---

#### Path Generation
**Purpose**: Builds a breadcrumb trail for hierarchical navigation.

**Inner Mechanisms**:
1. Traverses the post hierarchy from `$forum_index` to the root topic.
2. Reverses the path to display from root to current post.
3. Uses `mysql_query` to fetch parent post titles.

**Usage**:
- Automatically called when displaying a post/topic to show the navigation path.

---

### Display Logic

#### Search Results (`$forum_search`)
- **Title**: Displays "Search Results" with the query.
- **Query**: Searches post titles/texts using `forum->search()`.
- **Pagination**: Shows 10 results per page with navigation controls.
- **Highlighting**: Uses `quote_text()` to highlight search terms in results.

#### User Contributions (`$forum_user`)
- **Title**: Displays the user's name.
- **Query**: Fetches posts by the user, ordered by date.
- **Pagination**: Similar to search results.

#### Overview (`! $forum_index`)
- **Query**: Lists root-level topics with their latest replies.
- **Layout**: Displays topics with images (extracted via `get_first_image()`), titles, and teasers.

#### Topic/Post Display
- **Topic** (`$path_depth === 1`): Shows the topic and its replies.
- **Post** (`$path_depth > 1`): Shows the post and its nested replies.
- **Meta Data**: Updates the template's `title` and `description` with post content.

#### Not Found (`$path_depth === 0`)
- **HTTP Status**: Sets `410 Gone` for deleted/missing posts.
- **Message**: Displays a user-friendly "not found" message.

---

### Form Handling

#### Edit Form (`_add`/`_edit`)
- **Fields**:
  - `forum_edit_title`: Post title (max 80 chars).
  - `forum_edit_text`: Post content (supports BBCode via `textcontrol()`).
  - `forum_edit_email`: Checkbox for anonymous users to request notifications.
- **Extensions**: Loads optional buttons for images, links, and tokens if the respective modules are available.
- **Validation**: Checks for empty fields using `stre()`.

#### Preview Form (`__add`/`__edit`)
- **Purpose**: Shows a read-only preview of the post before saving.
- **Actions**: Allows canceling, editing, or saving.

---

### Utility Functions

| Function               | Purpose                                                                                     |
|------------------------|---------------------------------------------------------------------------------------------|
| `forum_quote($text)`   | Formats text for quoting (e.g., `[quote]...[/quote]`).                                     |
| `remove_format($text)` | Strips BBCode/formatting from text.                                                         |
| `parse_text($text)`    | Renders BBCode/formatted text as HTML.                                                     |
| `first_words($text)`   | Truncates text to the first few words (for teasers).                                       |
| `friendly_date($time)` | Converts a timestamp to a human-readable format (e.g., "Yesterday at 14:30").              |

---

### Security

1. **SQL Injection Protection**: All database queries use `sqlesc()` for escaping.
2. **CSRF Protection**: URLs generated via `cms_url()` include CSRF tokens.
3. **Permission Checks**: Every action validates user permissions via `forum->test_*` methods.
4. **Output Escaping**: Uses `x()` for HTML escaping and `q()` for JavaScript/JSON encoding.

---

### Integration Points

1. **Templates**: Merges forum data into `$template->query_data` for dynamic content.
2. **Permissions**: Uses `permission()` to display available actions based on user roles.
3. **Modules**: Extends editing functionality with `image`, `content`, and `token` modules if available.
4. **Pagination**: Uses the platform's `pagination()` function for consistent navigation.

---

### Error Handling

- **Validation Errors**: Displays messages like `CMS_L_MOD_FORUM_017` ("Title and text are required").
- **Database Errors**: Falls back to generic messages (e.g., `CMS_L_MOD_FORUM_023` for failed post creation).
- **HTTP Status**: Sets `410 Gone` for deleted posts.

---

### Typical Usage Scenarios

1. **Viewing a Topic**:
   ```php
   // URL: /forum?forum_index=123
   $forum_index = "123";
   include "mod.forum.inc";
   ```

2. **Adding a Post**:
   ```php
   // URL: /forum?forum_index=123&forum_message=add
   $forum_index = "123";
   $forum_message = "add";
   include "mod.forum.inc";
   ```

3. **Searching Posts**:
   ```php
   // URL: /forum?forum_search=query
   $forum_search = "query";
   include "mod.forum.inc";
   ```

4. **Moving a Post** (Operator Only):
   ```php
   // Step 1: Cut (URL: /forum?forum_index=123&forum_buffer=123)
   // Step 2: Paste (URL: /forum?forum_index=456&forum_buffer=123&forum_message=insert)
   $forum_buffer = "123";
   $forum_message = "insert";
   include "mod.forum.inc";
   ```


<!-- HASH:b66a5b865d3108beb1a681c5a8d099df -->
