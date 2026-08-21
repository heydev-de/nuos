# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.forum.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.forum.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Forum Module (`mod.forum.inc`)

The **Forum Module** provides a hierarchical, thread-based discussion system with support for topics, posts, and replies. It includes features for creating, editing, moving, and deleting messages, as well as searching and user-specific post filtering. The module integrates with PWNC's permission system, template engine, and core utilities (e.g., URL generation, text parsing, and database access).

---

### **Global Variables**
The module uses the following global variables to manage state and user input:

| Name                  | Type     | Description                                                                                     |
|-----------------------|----------|-------------------------------------------------------------------------------------------------|
| `$forum_message`      | `string` | Current action being processed (e.g., `"add"`, `"edit"`, `"delete"`).                          |
| `$forum_index`        | `string` | Unique identifier of the current forum post/topic.                                             |
| `$forum_buffer`       | `string` | Temporary storage for a post being moved (cut/copy/paste operations).                         |
| `$forum_search`       | `string` | Search query for filtering posts.                                                              |
| `$forum_user`         | `string` | User identifier for filtering posts by author.                                                 |
| `$forum_page`         | `string` | Current pagination page for search/user results.                                               |
| `$forum_edit_*`       | `string` | Temporary storage for post editing (title, text, email notification preference).                |

---

### **Initialization**
The module initializes by:
1. Loading the `forum` library (aborts if unavailable).
2. Merging forum data into the active template (if one exists).
3. Instantiating the `forum` class with the current content index.
4. Validating user permissions (reader/writer/operator).

---

### **Class: `forum`**
The `forum` class encapsulates all forum logic, including database operations, permission checks, and state management.

#### **Properties**
| Name          | Type      | Description                                                                                     |
|---------------|-----------|-------------------------------------------------------------------------------------------------|
| `enabled`     | `bool`    | Whether the forum is enabled for the current instance.                                         |
| `table`       | `string`  | Database table name for forum posts.                                                           |
| `reader`      | `bool`    | User has read permissions.                                                                     |
| `writer`      | `bool`    | User has write permissions.                                                                    |
| `operator`    | `bool`    | User has operator (admin) permissions.                                                         |
| `mysql`       | `object`  | Instance of the database wrapper class.                                                        |

---

### **Core Methods**

#### `test_add($container)`
**Purpose**: Checks if the user can add a post to the specified container (topic or reply).

| Parameter   | Type     | Description                                                                                     |
|-------------|----------|-------------------------------------------------------------------------------------------------|
| `$container`| `string` | Parent post index (0 for new topics).                                                           |

**Return Value**: `bool` – `TRUE` if allowed, `FALSE` otherwise.

**Inner Mechanisms**:
- Validates write permissions.
- Ensures the container exists (if not root).

**Usage Example**:
```php
if ($forum->test_add("123")) {
    echo '<a href="' . x(cms_url(["forum_index" => "123", "forum_message" => "add"])) . '">Reply</a>';
}
```

---

#### `test_edit($index)`
**Purpose**: Checks if the user can edit the specified post.

| Parameter | Type     | Description                                                                                     |
|-----------|----------|-------------------------------------------------------------------------------------------------|
| `$index`  | `string` | Post index to edit.                                                                             |

**Return Value**: `bool` – `TRUE` if allowed, `FALSE` otherwise.

**Inner Mechanisms**:
- Validates operator permissions **or** ownership of the post.

**Usage Example**:
```php
if ($forum->test_edit("456")) {
    echo '<a href="' . x(cms_url(["forum_index" => "456", "forum_message" => "edit"])) . '">Edit</a>';
}
```

---

#### `add($container, $title, $text, $email)`
**Purpose**: Creates a new post (topic or reply).

| Parameter   | Type     | Description                                                                                     |
|-------------|----------|-------------------------------------------------------------------------------------------------|
| `$container`| `string` | Parent post index (0 for new topics).                                                           |
| `$title`    | `string` | Post title.                                                                                     |
| `$text`     | `string` | Post content (supports BBCode).                                                                 |
| `$email`    | `bool`   | Whether to notify the user via email (for anonymous users).                                     |

**Return Value**: `string|FALSE` – New post index on success, `FALSE` on failure.

**Inner Mechanisms**:
- Escapes input data for SQL.
- Sets the current user as the author.
- Generates a unique index for the post.

**Usage Example**:
```php
$new_index = $forum->add("0", "Hello World", "This is my first post!", FALSE);
if ($new_index) {
    header("Location: " . cms_url(["forum_index" => $new_index]));
    exit();
}
```

---

#### `edit($index, $title, $text, $email)`
**Purpose**: Updates an existing post.

| Parameter | Type     | Description                                                                                     |
|-----------|----------|-------------------------------------------------------------------------------------------------|
| `$index`  | `string` | Post index to edit.                                                                             |
| `$title`  | `string` | New title.                                                                                      |
| `$text`   | `string` | New content.                                                                                    |
| `$email`  | `bool`   | Updated email notification preference.                                                          |

**Return Value**: `bool` – `TRUE` on success, `FALSE` on failure.

**Usage Example**:
```php
if ($forum->edit("456", "Updated Title", "New content...", TRUE)) {
    echo "Post updated!";
}
```

---

#### `delete($index)`
**Purpose**: Deletes a post and its replies (recursively).

| Parameter | Type     | Description                                                                                     |
|-----------|----------|-------------------------------------------------------------------------------------------------|
| `$index`  | `string` | Post index to delete.                                                                           |

**Return Value**: `bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**:
- Uses a recursive query to delete all child posts.
- Logs the deletion for auditing.

**Usage Example**:
```php
if ($forum->delete("789")) {
    echo "Post deleted!";
}
```

---

#### `move($index, $container)`
**Purpose**: Moves a post to a new parent (rethreading).

| Parameter   | Type     | Description                                                                                     |
|-------------|----------|-------------------------------------------------------------------------------------------------|
| `$index`    | `string` | Post index to move.                                                                             |
| `$container`| `string` | New parent post index (0 for root/topics).                                                      |

**Return Value**: `bool` – `TRUE` on success, `FALSE` on failure.

**Usage Example**:
```php
if ($forum->move("123", "456")) {
    echo "Post moved!";
}
```

---

#### `search($query)`
**Purpose**: Searches posts by title or content.

| Parameter | Type     | Description                                                                                     |
|-----------|----------|-------------------------------------------------------------------------------------------------|
| `$query`  | `string` | Search term.                                                                                    |

**Return Value**: `resource|FALSE` – MySQL result resource on success, `FALSE` on failure.

**Inner Mechanisms**:
- Uses `LIKE` with wildcards for partial matches.
- Excludes container posts (topics only).

**Usage Example**:
```php
$result = $forum->search("hello");
if ($result) {
    while ($row = mysql_fetch_assoc($result)) {
        echo x($row[CMS_DB_FORUM_TITLE]);
    }
}
```

---

#### `reply_count($indexes)`
**Purpose**: Counts replies for one or more posts.

| Parameter  | Type       | Description                                                                                     |
|------------|------------|-------------------------------------------------------------------------------------------------|
| `$indexes` | `array`    | Array of post indexes.                                                                          |

**Return Value**: `array` – Associative array of `[index => count]`.

**Usage Example**:
```php
$counts = $forum->reply_count(["123", "456"]);
echo "Post 123 has " . ($counts["123"] ?? 0) . " replies.";
```

---

### **Helper Functions**

#### `forum_quote($text)`
**Purpose**: Formats text for quoting in replies.

| Parameter | Type     | Description                                                                                     |
|-----------|----------|-------------------------------------------------------------------------------------------------|
| `$text`   | `string` | Text to quote.                                                                                  |

**Return Value**: `string` – Quoted text with `[quote]` BBCode tags.

**Usage Example**:
```php
$quoted = forum_quote("Original post content.");
echo parse_text($quoted); // Renders as a quoted block.
```

---

### **Workflow**
1. **Initialization**: Loads the forum library, checks permissions, and instantiates the `forum` class.
2. **Message Handling**: Processes actions (`add`, `edit`, `delete`, etc.) via a state machine (`$forum_message`).
3. **Rendering**: Displays the forum interface based on the current view (overview, topic, post, search, or user posts).
4. **Pagination**: Handles large result sets with 10 items per page.

---

### **Usage Example: Adding a Post**
```php
// Simulate user input (e.g., from a form submission)
$forum_message = "add";
$forum_index = "0"; // New topic
$forum_edit_title = "New Topic";
$forum_edit_text = "This is the content of my new topic.";

// Process the request
switch ($forum_message) {
    case "add":
        if ($forum->test_add($forum_index)) {
            $forum_message = "_add"; // Proceed to editing
        }
        break;
    case "_add":
        if (nstre($forum_edit_message) && $forum_edit_message === CMS_L_COMMAND_SHOW) {
            $forum_message = "__add"; // Proceed to preview
        }
        break;
    case "__add":
        if (nstre($forum_edit_message) && $forum_edit_message === CMS_L_COMMAND_SAVE) {
            $index = $forum->add($forum_index, $forum_edit_title, $forum_edit_text, FALSE);
            if ($index) {
                header("Location: " . cms_url(["forum_index" => $index]));
                exit();
            }
        }
        break;
}
```

---

### **Key Features**
- **Hierarchical Structure**: Topics and nested replies.
- **Permissions**: Role-based access control (reader/writer/operator).
- **Search**: Full-text search with pagination.
- **User Filtering**: View all posts by a specific user.
- **BBCode Support**: Rich text formatting via `parse_text()`.
- **Email Notifications**: Optional for anonymous users.
- **Cut/Copy/Paste**: Move posts between threads.


<!-- HASH:f8945808cf6a27286c1a4325cf7b821b -->
