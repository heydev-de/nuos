# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.forum.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.forum.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Forum Module

The `mod.forum.inc` file is the core controller for the forum module in the PWNC Web Platform. It handles all aspects of forum interaction including viewing topics and posts, creating and editing messages, searching content, browsing user contributions, and managing hierarchical navigation through threaded discussions.

The module operates as a state machine driven by the `$forum_message` variable, which determines the current action (e.g., add, edit, preview, save, delete, move). It integrates with the `forum` class (loaded via `cms_load("forum")`) for data operations and uses global variables for state management across requests.

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$forum_message` | string | Current action/state (add, edit, _add, __add, ___add, etc.) |
| `$forum_index` | int | Index of the current forum post/topic |
| `$forum_buffer` | int | Index of a post being moved (cut/paste buffer) |
| `$forum_search` | string | Search query string |
| `$forum_user` | string | User identifier for filtering posts by author |
| `$forum_page` | int | Current page number for pagination |
| `$forum_edit_message` | string | Submit button value from edit form |
| `$forum_edit_title` | string | Title input from edit form |
| `$forum_edit_text` | string | Text content from edit form |
| `$forum_edit_email` | bool | Email notification checkbox value |

### Initialization

The module begins by loading the forum library and validating access permissions. If the forum is disabled or the user lacks reader/writer/operator permissions, it exits early.

```php
// Typical usage context:
// Accessed via URL like: /forum.php?forum_index=123&forum_message=add
// The module automatically loads the forum library and checks permissions
```

### Message Handling State Machine

The module implements a multi-stage workflow for adding and editing posts:

1. **Initial action** (`add`, `edit`): Prepares data for editing
2. **Editing stage** (`_add`, `_edit`): Displays the edit form
3. **Preview stage** (`__add`, `__edit`): Shows a preview of the post
4. **Saving stage** (`___add`, `___edit`): Performs the actual database write
5. **Post-save** (`____add`): Displays success message after redirect

#### `add` Case

When `$forum_message` is `"add"`, the module prepares to create a new post. If replying to an existing post, it automatically quotes the parent post's content.

**Mechanism:**
- Checks if the user can add posts via `$forum->test_add()`
- If `$forum_index` is set, retrieves the parent post data
- Formats a quote block with the original author's name, date, and content
- Sets `$forum_message` to `"_add"` to proceed to the edit form

```php
// Triggered when user clicks "Reply" on a post
// URL: ?forum_index=45&forum_message=add
// Result: Edit form pre-filled with quoted content from post #45
```

#### `edit` Case

When `$forum_message` is `"edit"`, the module loads existing post data into the edit form.

**Mechanism:**
- Verifies edit permissions via `$forum->test_edit()`
- Retrieves the post's title, text, and email notification setting
- Populates `$forum_edit_*` variables
- Sets `$forum_message` to `"_edit"` to proceed to the edit form

```php
// Triggered when user clicks "Edit" on their own post
// URL: ?forum_index=45&forum_message=edit
// Result: Edit form pre-filled with current post content
```

#### `_add` / `_edit` Cases (Edit Form Display)

These cases render the HTML edit form with title and text inputs, extended editing buttons (for images, links, tokens), and command buttons (cancel, preview).

**Mechanism:**
- Sets template title to indicate editing mode
- Outputs a form with hidden fields preserving state
- Includes extended editing buttons if interface modules are available
- Initializes a JavaScript text control for the textarea
- Shows email notification checkbox for anonymous users
- Provides cancel and preview submit buttons

```php
// Displayed when user is composing/editing a post
// Form action points back to the same URL with updated forum_message
// Extended buttons allow inserting images, links, and tokens
```

#### `__add` / `__edit` Cases (Preview Display)

These cases show a preview of the post before final submission.

**Mechanism:**
- Renders the post title and formatted text using `parse_text()`
- Preserves all input data in hidden form fields
- Provides three command buttons: cancel, edit (back to form), and save

```php
// Shown after user clicks "Preview" in the edit form
// Allows user to review formatting before final save
// All input is preserved in hidden fields for round-trip
```

#### `___add` Case (Save New Post)

Performs the actual insertion of a new post into the database.

**Mechanism:**
- Calls `$forum->add()` with parent index, title, text, and email flag
- On success, redirects to prevent form resubmission (HTTP 303)
- On failure, sets an error message

```php
// Called when user clicks "Save" in the preview
// Redirects to ?forum_index=[new_id]&forum_message=____add
// The redirect prevents duplicate submissions on page refresh
```

#### `___edit` Case (Save Edited Post)

Updates an existing post in the database.

**Mechanism:**
- Calls `$forum->edit()` with post index, title, text, and email flag
- Sets success or error message accordingly

```php
// Called when user clicks "Save" while editing an existing post
// Updates the post in-place without redirect
```

#### `____add` Case (Post-Save Success)

Displays a success message after a new post is created.

**Mechanism:**
- Sets `$success` to a localized success message
- Falls through to the main forum display

```php
// Shown after redirect from ___add
// Displays "Post created successfully" message
```

#### `insert` Case (Move Post)

Moves a buffered (cut) post to a new parent location.

**Mechanism:**
- Calls `$forum->move()` with buffer index and target index
- Clears the buffer on success
- Sets success or error message

```php
// Triggered when user clicks "Paste" after cutting a post
// URL: ?forum_index=123&forum_buffer=45&forum_message=insert
// Moves post #45 to be a child of post #123
```

#### `delete` Case (Delete Post)

Removes a post from the forum.

**Mechanism:**
- Retrieves the parent container index before deletion
- Calls `$forum->delete()` to remove the post
- Updates `$forum_index` to point to the parent (for navigation)
- Sets success or error message

```php
// Triggered when user clicks "Delete" on a post
// URL: ?forum_index=45&forum_message=delete
// After deletion, navigates to the parent post
```

### Command Function

A closure that generates contextual action buttons (cut, paste, delete, edit, add) based on the current user's permissions and the post's state.

**Parameters:**
- `$index` (int): The post index for which to generate commands

**Mechanism:**
- Checks operator status for cut/paste functionality
- Uses permission test methods to determine available actions
- Generates appropriate URLs with CSRF protection via `cms_url()`
- Includes JavaScript confirmation for destructive actions

```php
// Used throughout the display logic to show action buttons
// Example output: [Cut] [Paste] [Delete] [Edit] [Reply]
// Buttons are conditionally displayed based on permissions
```

### Path Generation

Builds a breadcrumb navigation path by traversing parent containers from the current post up to the root.

**Mechanism:**
- Iteratively queries each post's container field
- Stores the path in an associative array
- Reverses the array to get root-to-leaf order
- Used for breadcrumb navigation and depth detection

```php
// For a post structure like: Root → Topic → Post → Reply
// Generates path: [1 => 0, 5 => 1, 12 => 5]
// path_depth = 3 (indicating we're viewing a reply)
```

### Main Forum Display

The module renders different views based on the current state:

#### Search Results

When `$forum_search` is set, displays paginated search results with highlighted query terms.

**Mechanism:**
- Calls `$forum->search()` to execute the search
- Paginates results (10 per page)
- Highlights search terms using `quote_text()`
- Falls back to first words if no match found in text

```php
// URL: ?forum_search=keyword
// Shows matching posts with highlighted search terms
// Includes pagination controls
```

#### User Posts

When `$forum_user` is set, displays all posts by a specific user.

**Mechanism:**
- Queries posts where `CMS_DB_FORUM_USER` matches
- Orders by time descending
- Paginates results (10 per page)
- Shows post title, date, and reply count

```php
// URL: ?forum_user=author_name
// Displays all posts by the specified user
// Useful for viewing a user's contribution history
```

#### Overview (Root Level)

When no specific post is selected, displays the forum overview with topics and their latest posts.

**Mechanism:**
- Retrieves root-level topics (container = 0)
- Uses GROUP_CONCAT to fetch latest 3 posts per topic
- Displays topic title, teaser text, and latest post summaries
- Shows command buttons for each topic

```php
// URL: (no forum_index parameter)
// Shows all top-level topics with previews
// Each topic shows its 3 most recent replies
```

#### 410 Gone (Invalid Post)

When `$path_depth` is 0 but a forum_index was provided, indicates the post no longer exists.

**Mechanism:**
- Sets HTTP 410 status code
- Updates template metadata
- Displays a "content not found" message

```php
// URL: ?forum_index=999 (where post 999 doesn't exist)
// Returns 410 Gone status for SEO purposes
```

#### Topic View

When `$path_depth` is 1, displays a single topic with its replies.

**Mechanism:**
- Logs access via `$forum->log_access()`
- Retrieves the topic post
- Fetches and displays replies (paginated, 10 per page)
- Shows reply count, dates, and author information

```php
// URL: ?forum_index=123 (where 123 is a topic)
// Displays the topic content followed by its replies
// Includes pagination for topics with many replies
```

#### Post View

When `$path_depth` is greater than 1, displays a single post with its replies.

**Mechanism:**
- Similar to topic view but for individual posts
- Shows the post content and any direct replies
- Includes command buttons for the post

```php
// URL: ?forum_index=456 (where 456 is a reply/post)
// Displays the post content followed by its direct replies
```

### Integration Points

The module integrates with several PWNC subsystems:

- **Template system**: Updates `$template->title` and `$template->description`
- **Permission system**: Uses `permission_get_name()` and `permission()` for access control
- **Interface modules**: Conditionally loads image, content, and token interfaces
- **Text processing**: Uses `parse_text()`, `remove_format()`, `first_words()`, `quote_text()`
- **URL generation**: Uses `cms_url()` for all links with CSRF protection
- **Pagination**: Uses `pagination()` helper for paginated content
- **Caching**: Relies on `cms_cache()` indirectly through the forum class


<!-- HASH:f8945808cf6a27286c1a4325cf7b821b -->
