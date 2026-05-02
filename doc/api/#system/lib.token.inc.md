# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.token.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Token Management System

This file provides the `token` class and related utility functions for managing dynamic placeholders (tokens) within the NUOS platform. Tokens are reusable text fragments that can be inserted into content and replaced with dynamic values at runtime. They support parameterized values and conditional text blocks, enabling flexible content templating.

---

## Constants

| Name                          | Value/Default               | Description                                                                 |
|-------------------------------|-----------------------------|-----------------------------------------------------------------------------|
| `CMS_TOKEN_PERMISSION_OPERATOR` | `"operator"`                | Permission identifier required to manage tokens.                           |
| `CMS_DB_TOKEN`                | `CMS_DB_PREFIX . "token"`   | Database table name for storing tokens.                                    |
| `CMS_DB_TOKEN_INDEX`          | `"id"`                      | Column name for the token identifier (primary key).                        |
| `CMS_DB_TOKEN_VALUE`          | `"value"`                   | Column name for the token's default values (comma-separated list).         |
| `CMS_DB_TOKEN_CATEGORY`       | `"category"`                | Column name for the token's category (used for grouping).                  |
| `CMS_DB_TOKEN_TITLE`          | `"title"`                   | Column name for the token's title (human-readable name).                   |
| `CMS_DB_TOKEN_TEXT`           | `"text"`                    | Column name for the token's content (supports placeholders and conditions).|

---

## Utility Functions

### `token_get_index(&$token, $category)`

**Purpose:**
Retrieves the first token index (ID) from the database for a given category.

**Parameters:**

| Name       | Type       | Description                                      |
|------------|------------|--------------------------------------------------|
| `$token`   | `token`    | Token class instance (passed by reference).      |
| `$category`| `string`   | Category name to filter tokens.                  |

**Return Values:**
- `string|FALSE`: The token index if found; `FALSE` otherwise.

**Inner Mechanisms:**
- Checks if the token system is enabled and the category is non-empty.
- Executes a SQL query to fetch the first token index for the given category.
- Uses `sqlesc()` for SQL escaping to prevent injection.

**Usage Context:**
- Used to quickly check if a category exists or to fetch a representative token from a category.

---

### `token_get_category(&$token, $index)`

**Purpose:**
Retrieves the category of a token given its index.

**Parameters:**

| Name     | Type     | Description                                      |
|----------|----------|--------------------------------------------------|
| `$token` | `token`  | Token class instance (passed by reference).      |
| `$index` | `string` | Token index (ID) to look up.                     |

**Return Values:**
- `string|FALSE`: The token's category if found; `FALSE` otherwise.

**Inner Mechanisms:**
- Validates that the index is non-empty and the token system is enabled.
- Executes a SQL query to fetch the category for the given token index.

**Usage Context:**
- Used to categorize or group tokens dynamically.

---

### `token_get_select(&$token)`

**Purpose:**
Generates an associative array of all distinct token categories for use in form select elements.

**Parameters:**

| Name     | Type    | Description                                      |
|----------|---------|--------------------------------------------------|
| `$token` | `token` | Token class instance (passed by reference).      |

**Return Values:**
- `array|FALSE`: Associative array of categories (`["" => "", "category" => "category"]`) or `FALSE` if disabled.

**Inner Mechanisms:**
- Fetches distinct non-empty categories from the database.
- Constructs an array suitable for HTML `<select>` elements.

**Usage Context:**
- Used in admin interfaces to populate dropdowns for token categorization.

---

### `token_override($index, $text = NULL)`

**Purpose:**
Temporarily overrides the text of a token for the current request.

**Parameters:**

| Name    | Type     | Description                                      |
|---------|----------|--------------------------------------------------|
| `$index`| `string` | Token index to override.                         |
| `$text` | `string` | New text to use; if `NULL`, removes the override.|

**Return Values:**
- `void`

**Inner Mechanisms:**
- Modifies the static `$override` property of the `token` class.
- Overrides are checked during token application (`apply()`).

**Usage Context:**
- Used for testing, debugging, or runtime customization without modifying the database.

---

## `token` Class

### Properties

| Name         | Type     | Description                                      |
|--------------|----------|--------------------------------------------------|
| `$mysql`     | `mysql`  | Database connection handler.                     |
| `$operator`  | `bool`   | Whether the current user has operator permission.|
| `$enabled`   | `bool`   | Whether the token system is enabled.             |
| `$override`  | `array`  | Static array of token text overrides.            |

---

### `__construct()`

**Purpose:**
Initializes the token system and ensures the database table exists.

**Parameters:**
- None

**Return Values:**
- None

**Inner Mechanisms:**
- Initializes a new `mysql` instance.
- Verifies the token table exists using `mysql->verify_table()`.
- Sets `$operator` based on `cms_permission()`.
- Enables the system only if the table is verified.

**Usage Context:**
- Called automatically when a `token` object is instantiated.

---

### `add($index, $value, $category, $title, $text)`

**Purpose:**
Adds a new token to the database.

**Parameters:**

| Name       | Type     | Description                                      |
|------------|----------|--------------------------------------------------|
| `$index`   | `string` | Token identifier (auto-generated from title if empty). |
| `$value`   | `string` | Comma-separated list of default values.          |
| `$category`| `string` | Token category.                                  |
| `$title`   | `string` | Human-readable title (used for auto-indexing).   |
| `$text`    | `string` | Token content with placeholders (e.g., `%value%`).|

**Return Values:**
- `string|FALSE`: The token index if successful; `FALSE` otherwise.

**Inner Mechanisms:**
- Validates system and operator permissions.
- Auto-generates the index from the title if empty.
- Sanitizes the index and value using regex to ensure valid characters.
- Inserts the token into the database.

**Usage Context:**
- Used in admin interfaces to create new tokens.

---

### `get($index)`

**Purpose:**
Retrieves a token's full data by its index.

**Parameters:**

| Name    | Type     | Description                                      |
|---------|----------|--------------------------------------------------|
| `$index`| `string` | Token index to fetch.                            |

**Return Values:**
- `array|FALSE`: Associative array of token data (`index`, `value`, `category`, `title`, `text`) or `FALSE`.

**Inner Mechanisms:**
- Executes a SQL query to fetch all fields for the given index.
- Returns the result as an associative array.

**Usage Context:**
- Used to display or edit token details.

---

### `update($index, $_index, $value, $category, $title, $text)`

**Purpose:**
Updates an existing token.

**Parameters:**

| Name       | Type     | Description                                      |
|------------|----------|--------------------------------------------------|
| `$index`   | `string` | Current token index.                             |
| `$_index`  | `string` | New token index (auto-generated from title if empty). |
| `$value`   | `string` | Updated comma-separated list of default values.  |
| `$category`| `string` | Updated category.                                |
| `$title`   | `string` | Updated title.                                   |
| `$text`    | `string` | Updated token content.                           |

**Return Values:**
- `string|FALSE`: The new token index if successful; `FALSE` otherwise.

**Inner Mechanisms:**
- Validates system and operator permissions.
- Auto-generates the new index from the title if empty.
- Sanitizes the index and value using regex.
- Updates the token in the database.

**Usage Context:**
- Used in admin interfaces to modify existing tokens.

---

### `delete($index)`

**Purpose:**
Deletes a token from the database.

**Parameters:**

| Name    | Type     | Description                                      |
|---------|----------|--------------------------------------------------|
| `$index`| `string` | Token index to delete.                           |

**Return Values:**
- `bool`: `TRUE` if the query succeeded; `FALSE` otherwise.

**Inner Mechanisms:**
- Validates system and operator permissions.
- Executes a SQL `DELETE` query.

**Usage Context:**
- Used in admin interfaces to remove tokens.

---

### `apply($text)`

**Purpose:**
Replaces all token placeholders in a text with their dynamic values.

**Parameters:**

| Name   | Type     | Description                                      |
|--------|----------|--------------------------------------------------|
| `$text`| `string` | Input text containing token placeholders.        |

**Return Values:**
- `string|FALSE`: The processed text with tokens replaced, or `FALSE` if disabled.

**Inner Mechanisms:**
1. **Token Detection:**
   - Scans the text for `%%token_name%%` patterns.
   - Skips escaped placeholders (`\%\%`).
   - Extracts token indices and user-provided values.

2. **Database Lookup:**
   - Fetches all detected tokens in a single query for efficiency.
   - Applies overrides if present.

3. **Replacement:**
   - For each token, replaces placeholders (`%value%`) with user values.
   - Supports conditional blocks (`[text%value%more]`), which are removed if the value is empty.
   - Handles escaped commas in user values (`\,`).

4. **Text Reconstruction:**
   - Replaces each token occurrence with its processed text.
   - Adjusts offsets to handle dynamic text length changes.

**Usage Context:**
- Used to render dynamic content in pages, emails, or templates.
- Example: `%%welcome_user%name%%` → `Hello, John!` (if `name` is provided).
- Supports complex logic: `%%discount%code,amount%%[Use code %code% for %amount% off!]%%`.


<!-- HASH:435aa8008baf3e3098ac432bef8e3d8b -->
