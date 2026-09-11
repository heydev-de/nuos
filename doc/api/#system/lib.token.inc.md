# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.token.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.token.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Token System

The `token` class and its associated functions provide a dynamic content substitution mechanism for the PWNC Web Platform. Tokens are placeholders embedded within text content (using `%%token_index%%` syntax) that get replaced with actual values at runtime. This system enables content personalization, localization, and dynamic data injection without requiring complex template engines.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TOKEN_PERMISSION_OPERATOR` | `"operator"` | Permission level required to manage tokens |
| `CMS_DB_TOKEN` | `CMS_DB_PREFIX . "token"` | Database table name for tokens |
| `CMS_DB_TOKEN_INDEX` | `"id"` | Column name for token identifier |
| `CMS_DB_TOKEN_VALUE` | `"value"` | Column name for token parameter values |
| `CMS_DB_TOKEN_CATEGORY` | `"category"` | Column name for token category grouping |
| `CMS_DB_TOKEN_TITLE` | `"title"` | Column name for token display title |
| `CMS_DB_TOKEN_TEXT` | `"text"` | Column name for token replacement text |

## token_get_index

Retrieves the first token index from the database for a given category.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$token` | `token` | Reference to the token instance (used for checking enabled status) |
| `$category` | `string` | Category to search for |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The token index if found |
| `FALSE` | If no token exists in the category or token system is disabled |

### Inner Mechanisms

Executes a SQL query to find the first token in the specified category, ordered by index. Uses `sqlesc()` for SQL injection prevention.

### Usage Example

```php
$token = new token();
$index = token_get_index($token, "navigation");
// Returns the first token index in the "navigation" category
```

## token_get_category

Retrieves the category of a specific token by its index.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$token` | `token` | Reference to the token instance |
| `$index` | `string` | Token index to look up |

### Return Values

| Type | Description |
|------|-------------|
| `string` | The category name if found |
| `FALSE` | If token doesn't exist or system is disabled |

### Usage Example

```php
$token = new token();
$category = token_get_category($token, "main_logo");
// Returns the category of the "main_logo" token
```

## token_get_select

Retrieves all distinct categories from the token table for use in selection interfaces.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `&$token` | `token` | Reference to the token instance |

### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array of categories (key => value pairs) |
| `FALSE` | If query fails or system is disabled |

### Usage Example

```php
$token = new token();
$categories = token_get_select($token);
// Returns ["navigation" => "navigation", "footer" => "footer", ...]
```

## token_override

Sets or removes an override value for a specific token index.

### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index to override |
| `$text` | `string\|NULL` | Override text, or NULL to remove override |

### Return Values

No return value.

### Inner Mechanisms

Manipulates the static `$override` property of the `token` class. When set, overrides take precedence over database values during token application.

### Usage Example

```php
token_override("welcome_message", "Hello, valued customer!");
// All instances of %%welcome_message%% will now display the custom text
```

## token Class

Main class implementing the token system with CRUD operations, caching, and text processing capabilities.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$operator` | `boolean\|NULL` | Whether current user has operator permissions |
| `$enabled` | `boolean\|NULL` | Whether the token system is active |
| `$override` | `array\|NULL` | Static array of token overrides |

### Constructor

Initializes the token system by verifying the database table structure and checking operator permissions.

#### Inner Mechanisms

Creates a `mysql` instance and verifies the token table schema. If successful, sets `$enabled` to TRUE and checks operator permissions via `cms_permission()`.

### add

Creates a new token in the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Unique token identifier |
| `$value` | `string` | Comma-separated parameter names |
| `$category` | `string` | Category for grouping tokens |
| `$title` | `string` | Display title (supports localization) |
| `$text` | `string` | Replacement text with placeholders |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The sanitized token index on success |
| `FALSE` | On failure (disabled, no permissions, invalid data, or database error) |

#### Inner Mechanisms

Validates permissions and input, sanitizes the index and value using regex patterns, handles localization through `language_get()` and `language_set()`, then inserts into the database. Invalidates cache for the new token.

#### Usage Example

```php
$token = new token();
$result = $token->add(
    "user_greeting",
    "name,time",
    "messages",
    "User Greeting",
    "Hello {name}, welcome at {time}!"
);
// Creates a token with parameters {name} and {time}
```

### get

Retrieves a single token record from the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index to retrieve |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array with token data (index, value, category, title, text) |
| `FALSE` | If token not found or system disabled |

#### Usage Example

```php
$token = new token();
$data = $token->get("user_greeting");
// Returns ["id" => "user_greeting", "value" => "name,time", ...]
```

### update

Modifies an existing token in the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Current token index |
| `$_index` | `string` | New token index (can be empty to keep current) |
| `$value` | `string` | New parameter values |
| `$category` | `string` | New category |
| `$title` | `string` | New title |
| `$text` | `string` | New replacement text |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The new token index on success |
| `FALSE` | On failure |

#### Inner Mechanisms

Similar to `add()` but performs an UPDATE query. Handles index renaming and cache invalidation for both old and new indices.

#### Usage Example

```php
$token = new token();
$result = $token->update(
    "user_greeting",
    "personal_greeting",
    "name,time,location",
    "messages",
    "Personal Greeting",
    "Hello {name}, welcome at {time} in {location}!"
);
// Updates and renames the token
```

### delete

Removes a token from the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index to delete |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful deletion |
| `FALSE` | On failure |

#### Inner Mechanisms

Executes a DELETE query and invalidates the cache for the deleted token.

#### Usage Example

```php
$token = new token();
$result = $token->delete("user_greeting");
// Removes the token from database and cache
```

### apply

Processes text content, replacing all `%%token_index%%` placeholders with their corresponding values.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text content containing token placeholders |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Processed text with tokens replaced |
| `FALSE` | If system is disabled |

#### Inner Mechanisms

1. Scans text for `%%...%%` delimiters, handling escape sequences (`\%%`)
2. Extracts token indices and parameter strings
3. Checks cache first, falling back to database queries for uncached tokens
4. Parses parameter assignments from the placeholder syntax
5. Replaces placeholders using `replace_placeholder()` function
6. Caches results for future use

#### Usage Example

```php
$token = new token();
$content = "Welcome %%user_greeting name=John,time=morning%% to our site!";
$result = $token->apply($content);
// Returns: "Welcome Hello John, welcome at morning! to our site!"
```

### cache_set

Stores token data in the file-based cache.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index |
| `$value` | `string\|NULL` | Token parameter values |
| `$text` | `string\|NULL` | Token replacement text |

#### Return Values

Result of `write_file()` operation.

#### Inner Mechanisms

Uses `hash32()` to generate a directory structure for cache files, storing serialized data.

### cache_get

Retrieves token data from the file-based cache.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Cached data `[$value, $text]` |
| `mixed` | Result of `unserialize()` (may be FALSE if not cached) |

### cache_del

Removes a token's cache file.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index |

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | If file was deleted |
| `FALSE` | If file didn't exist or deletion failed |

### cache_clean

Clears all token cache files.

#### Return Values

| Type | Description |
|------|-------------|
| `TRUE` | On successful cleanup |
| `FALSE` | If filemanager library cannot be loaded |

#### Inner Mechanisms

Loads the filemanager library and recursively deletes the token cache directory.

#### Usage Example

```php
$token = new token();
$token->cache_clean();
// Clears all cached token data
```


<!-- HASH:078e71177206a0bc581258a7c59b6f6a -->
