# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.token.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.token.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Token Management System

The `lib.token.inc` file provides a **Token Management System** for the PWNC Web Platform. It enables the creation, storage, retrieval, and replacement of tokens within text content. Tokens are placeholders (e.g., `%%token_name param1,param2%%`) that can be dynamically replaced with predefined or user-defined content.

This system is particularly useful for:
- **Multilingual content** – Tokens can represent language-specific strings.
- **Dynamic content injection** – Tokens can be replaced with values from a database or user input.
- **Template systems** – Tokens allow for reusable, parameterized content blocks.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TOKEN_PERMISSION_OPERATOR` | `"operator"` | Permission identifier required to modify tokens. |
| `CMS_DB_TOKEN` | `CMS_DB_PREFIX . "token"` | Database table name for storing tokens. |
| `CMS_DB_TOKEN_INDEX` | `"id"` | Column name for the token identifier (primary key). |
| `CMS_DB_TOKEN_VALUE` | `"value"` | Column name for the token's parameter names (comma-separated). |
| `CMS_DB_TOKEN_CATEGORY` | `"category"` | Column name for the token's category (grouping). |
| `CMS_DB_TOKEN_TITLE` | `"title"` | Column name for the token's title (human-readable). |
| `CMS_DB_TOKEN_TEXT` | `"text"` | Column name for the token's content (replacement text). |

---

## Standalone Functions

### `token_get_index(&$token, $category)`

**Purpose:**
Retrieves the first token index (ID) from a given category.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$token` | `token` (object, by reference) | Token class instance. |
| `$category` | `string` | Category name to search for. |

**Return Values:**
- `string` – The token index (ID) if found.
- `FALSE` – If no token exists in the category or the token system is disabled.

**Inner Mechanisms:**
- Checks if the token system is enabled.
- Executes a SQL query to fetch the first token index from the specified category.
- Uses `sqlesc()` for SQL escaping to prevent injection.

**Usage Context:**
Used to find a token by its category, typically when iterating over tokens in a specific group.

**Example:**
```php
$token = new token();
$index = token_get_index($token, "notifications");
if ($index) {
    echo "First token in 'notifications': " . $index;
}
```

---

### `token_get_category(&$token, $index)`

**Purpose:**
Retrieves the category of a token given its index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$token` | `token` (object, by reference) | Token class instance. |
| `$index` | `string` | Token index (ID) to look up. |

**Return Values:**
- `string` – The category name if the token exists.
- `FALSE` – If the token does not exist or the system is disabled.

**Inner Mechanisms:**
- Validates that `$index` is not empty.
- Executes a SQL query to fetch the category of the specified token.

**Usage Context:**
Used to determine the category of a known token, useful for filtering or grouping.

**Example:**
```php
$token = new token();
$category = token_get_category($token, "welcome_message");
if ($category) {
    echo "Token 'welcome_message' belongs to category: " . $category;
}
```

---

### `token_get_select(&$token)`

**Purpose:**
Retrieves a list of all distinct, non-empty token categories for use in a `<select>` input.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$token` | `token` (object, by reference) | Token class instance. |

**Return Values:**
- `array` – Associative array of categories (`["" => "", "category1" => "category1", ...]`).
- `FALSE` – If the token system is disabled or no categories exist.

**Inner Mechanisms:**
- Executes a SQL query to fetch distinct categories.
- Builds an array suitable for HTML `<select>` elements.

**Usage Context:**
Used in administrative interfaces to allow users to select a token category.

**Example:**
```php
$token = new token();
$categories = token_get_select($token);
if ($categories) {
    echo '<select name="category">';
    foreach ($categories as $value => $label) {
        echo '<option value="' . q($value) . '">' . q($label) . '</option>';
    }
    echo '</select>';
}
```

---

### `token_override($index, $text = NULL)`

**Purpose:**
Temporarily overrides the content of a token for the current request.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index (ID) to override. |
| `$text` | `string` \| `NULL` | New content for the token. If `NULL`, removes the override. |

**Return Values:**
- None (void).

**Inner Mechanisms:**
- Modifies the static `token::$override` array.
- Overrides are checked before database lookups in `token::apply()`.

**Usage Context:**
Used for testing, debugging, or temporarily changing token content without modifying the database.

**Example:**
```php
token_override("welcome_message", "Welcome back, valued user!");
$token = new token();
echo $token->apply("Hello, %%welcome_message%%!");
// Output: "Hello, Welcome back, valued user!"
```

---

## Class: `token`

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$operator` | `bool` | Whether the current user has operator permissions. |
| `$enabled` | `bool` | Whether the token system is enabled (database table exists). |
| `$override` (static) | `array` | Temporary overrides for token content (key: index, value: text). |

---

### `__construct()`

**Purpose:**
Initializes the token system and verifies the database table.

**Parameters:**
- None.

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
- Creates a `mysql` instance.
- Uses `mysql::verify_table()` to ensure the `token` table exists with the correct schema.
- Sets `$operator` based on `cms_permission()`.
- Sets `$enabled` to `TRUE` if the table exists.

**Usage Context:**
Automatically called when instantiating the `token` class.

**Example:**
```php
$token = new token();
if ($token->enabled) {
    echo "Token system is ready.";
}
```

---

### `add($index, $value, $category, $title, $text)`

**Purpose:**
Adds a new token to the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token identifier (ID). If empty, derived from `$title`. |
| `$value` | `string` | Comma-separated parameter names (e.g., `"name,age"`). |
| `$category` | `string` | Category for grouping tokens. |
| `$title` | `string` | Human-readable title. If empty, defaults to `$index`. |
| `$text` | `string` | Content to replace the token (supports placeholders like `{name}`). |

**Return Values:**
- `string` – The token index if successful.
- `FALSE` – If the operation fails or the user lacks permissions.

**Inner Mechanisms:**
- Validates permissions and system status.
- Derives `$index` from `$title` if empty (using `language_get()`).
- Sanitizes `$index` and `$value` to remove invalid characters.
- Inserts the token into the database.
- Clears the cache for the new token.

**Usage Context:**
Used in administrative interfaces to create new tokens.

**Example:**
```php
$token = new token();
$result = $token->add(
    "greet_user",
    "name,time",
    "greetings",
    "Greet User",
    "Hello, {name}! It's {time}."
);
if ($result) {
    echo "Token created with ID: " . $result;
}
```

---

### `get($index)`

**Purpose:**
Retrieves a token's data from the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index (ID) to retrieve. |

**Return Values:**
- `array` – Associative array with keys: `CMS_DB_TOKEN_INDEX`, `CMS_DB_TOKEN_VALUE`, `CMS_DB_TOKEN_CATEGORY`, `CMS_DB_TOKEN_TITLE`, `CMS_DB_TOKEN_TEXT`.
- `FALSE` – If the token does not exist or the system is disabled.

**Inner Mechanisms:**
- Executes a SQL query to fetch the token's data.
- Returns the result as an associative array.

**Usage Context:**
Used to inspect token details, e.g., in an edit form.

**Example:**
```php
$token = new token();
$data = $token->get("greet_user");
if ($data) {
    echo "Token Title: " . $data[CMS_DB_TOKEN_TITLE];
    echo "Parameters: " . $data[CMS_DB_TOKEN_VALUE];
}
```

---

### `update($index, $_index, $value, $category, $title, $text)`

**Purpose:**
Updates an existing token in the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Current token index (ID). |
| `$_index` | `string` | New token index (ID). If empty, derived from `$title`. |
| `$value` | `string` | Updated comma-separated parameter names. |
| `$category` | `string` | Updated category. |
| `$title` | `string` | Updated human-readable title. |
| `$text` | `string` | Updated replacement content. |

**Return Values:**
- `string` – The new token index if successful.
- `FALSE` – If the operation fails or the user lacks permissions.

**Inner Mechanisms:**
- Validates permissions and system status.
- Derives `$_index` from `$title` if empty.
- Sanitizes `$_index` and `$value`.
- Updates the token in the database.
- Clears the cache for both the old and new token indices.

**Usage Context:**
Used in administrative interfaces to modify existing tokens.

**Example:**
```php
$token = new token();
$result = $token->update(
    "greet_user",
    "greet_user_updated",
    "name,time,location",
    "greetings",
    "Updated Greet User",
    "Hello, {name}! It's {time} in {location}."
);
if ($result) {
    echo "Token updated with new ID: " . $result;
}
```

---

### `delete($index)`

**Purpose:**
Deletes a token from the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index (ID) to delete. |

**Return Values:**
- `TRUE` – If the token was deleted.
- `FALSE` – If the operation fails or the user lacks permissions.

**Inner Mechanisms:**
- Validates permissions and system status.
- Executes a SQL query to delete the token.
- Clears the cache for the deleted token.

**Usage Context:**
Used in administrative interfaces to remove tokens.

**Example:**
```php
$token = new token();
if ($token->delete("greet_user")) {
    echo "Token deleted successfully.";
}
```

---

### `apply($text)`

**Purpose:**
Replaces all tokens in a given text with their corresponding content.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text containing tokens (e.g., `"Hello, %%greet_user name=John%%!"`). |

**Return Values:**
- `string` – Text with tokens replaced by their content.
- `FALSE` – If the system is disabled.

**Inner Mechanisms:**
1. **Token Detection:**
   - Scans for `%%` delimiters, ignoring escaped sequences (`\%%`).
   - Extracts token indices and parameters (e.g., `greet_user name=John`).
2. **Token Resolution:**
   - Checks the cache for token content.
   - If not cached, fetches from the database and caches the result.
   - Applies overrides from `token::$override` if present.
3. **Replacement:**
   - Uses `replace_placeholder()` to substitute parameters in the token content.
   - Handles missing tokens by leaving them unchanged.

**Usage Context:**
Used to render dynamic content in templates, emails, or user-facing text.

**Example:**
```php
$token = new token();
$text = "Welcome, %%greet_user name=Alice,time=morning%%!";
echo $token->apply($text);
// Output (if token content is "Hello, {name}! It's {time}."):
// "Welcome, Hello, Alice! It's morning.!"
```

---

### `cache_set($index, $value, $text)`

**Purpose:**
Stores a token's data in the file-based cache.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index (ID). |
| `$value` | `string` \| `NULL` | Token's parameter names (comma-separated). `NULL` if the token does not exist. |
| `$text` | `string` \| `NULL` | Token's replacement content. `NULL` if the token does not exist. |

**Return Values:**
- `bool` – Result of `write_file()` (success/failure).

**Inner Mechanisms:**
- Generates a filesystem path using `hash32($index)`.
- Serializes the data and writes it to a file in `CMS_DATA_PATH . "#token/cache/"`.

**Usage Context:**
Internal use by `apply()`, `add()`, `update()`, and `delete()`.

---

### `cache_get($index)`

**Purpose:**
Retrieves a token's data from the cache.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index (ID). |

**Return Values:**
- `array` – Unserialized data: `[value, text]`.
- `FALSE` – If the cache file does not exist.

**Inner Mechanisms:**
- Generates the same filesystem path as `cache_set()`.
- Reads and unserializes the file.

**Usage Context:**
Internal use by `apply()`.

---

### `cache_del($index)`

**Purpose:**
Deletes a token's cache file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index (ID). |

**Return Values:**
- `bool` – Result of `unlink()` (success/failure).

**Inner Mechanisms:**
- Generates the filesystem path and deletes the file if it exists.

**Usage Context:**
Internal use by `add()`, `update()`, and `delete()`.

---

### `cache_clean()`

**Purpose:**
Deletes all cached token data.

**Parameters:**
- None.

**Return Values:**
- `bool` – Result of `filemanager_delete()` (success/failure).

**Inner Mechanisms:**
- Requires the `filemanager` library.
- Deletes the entire cache directory (`CMS_DATA_PATH . "#token/cache/"`).

**Usage Context:**
Used during system maintenance or when clearing stale cache.

**Example:**
```php
$token = new token();
if ($token->cache_clean()) {
    echo "Token cache cleared.";
}
```


<!-- HASH:078e71177206a0bc581258a7c59b6f6a -->
