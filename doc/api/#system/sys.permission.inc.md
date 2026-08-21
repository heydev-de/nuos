# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.permission.inc)

- **Version:** `26.8.13.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Management System

This file provides the core permission management functionality for the PWNC Web Platform. It includes functions and a class to handle user/group permissions, access control, and user data management.

---

## Functions

### `permission_delete($user)`

Removes all data associated with a user, including IMS (Instant Messaging System) data, desktop data, and reassigns content ownership.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username to be deleted          |

**Inner Mechanisms:**
1. Removes IMS data if the `core_resource` module is loaded
2. Deletes desktop data if the `desktop` module is available
3. Reassigns content ownership to the administrator

**Usage Example:**
```php
permission_delete("john_doe");
// Removes all data associated with user "john_doe"
```

---

### `permission_match($access, $permission, $exclusion)`

Checks if a specific access is permitted based on permission and exclusion rules.

| Parameter    | Type   | Description                     |
|--------------|--------|---------------------------------|
| `$access`    | string | Access string to check          |
| `$permission`| string | Permission rules                |
| `$exclusion` | string | Exclusion rules                 |

**Return Values:**
- `TRUE` if access is permitted
- `FALSE` if access is denied

**Inner Mechanisms:**
1. Checks for always-allowed permissions
2. Verifies specific exclusions
3. Checks global and specific permissions
4. Processes hierarchical access strings

**Usage Example:**
```php
$hasAccess = permission_match(
    "content.edit",
    "*\ncontent.*\ncontent.edit",
    "admin.*"
);
// Returns TRUE if "content.edit" is permitted
```

---

### `permission_merge($value1, $value2)`

Merges two permission/exclusion strings while removing duplicates.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value1` | string | First permission/exclusion set  |
| `$value2` | string | Second permission/exclusion set |

**Return Values:**
- Merged string with unique entries

**Usage Example:**
```php
$merged = permission_merge(
    "content.view\ncontent.edit",
    "content.edit\ncontent.delete"
);
// Returns "content.view\ncontent.edit\ncontent.delete"
```

---

### `permission_is_user($key)`

Checks if a permission key represents a user.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | Permission key to check         |

**Return Values:**
- `TRUE` if key represents a user
- `FALSE` otherwise

**Usage Example:**
```php
$isUser = permission_is_user("user.john_doe");
// Returns TRUE
```

---

### `permission_is_group($key)`

Checks if a permission key represents a group.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | Permission key to check         |

**Return Values:**
- `TRUE` if key represents a group
- `FALSE` otherwise

**Usage Example:**
```php
$isGroup = permission_is_group("group.editors");
// Returns TRUE
```

---

### `permission_get_name($user, $email = FALSE)`

Retrieves a user's name or email address.

| Parameter | Type    | Description                     |
|-----------|---------|---------------------------------|
| `$user`   | string  | Username                        |
| `$email`  | boolean | If TRUE, returns email instead  |

**Return Values:**
- User's name or email address
- `NULL` if not found

**Usage Example:**
```php
$name = permission_get_name("john_doe");
// Returns "John Doe"

$email = permission_get_name("john_doe", TRUE);
// Returns "john@example.com"
```

---

### `permission_get_email($user)`

Convenience function to get a user's email address.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- User's email address
- `NULL` if not found

**Usage Example:**
```php
$email = permission_get_email("john_doe");
// Returns "john@example.com"
```

---

## Permission Class

The main class for managing permissions, users, and groups.

### Properties

| Name     | Type  | Description                     |
|----------|-------|---------------------------------|
| `data`   | object| Data storage handler            |
| `buffer` | mixed | Temporary storage buffer        |

---

### `__construct()`

Initializes the permission system and sets up default users (admin, anonymous, profile, daemon).

**Inner Mechanisms:**
1. Creates data storage instance
2. Configures default administrator account
3. Sets up anonymous user
4. Configures profile and daemon users

**Usage Example:**
```php
$permission = new permission();
// Initializes the permission system
```

---

### `user($user, $disabled = NULL, $name = NULL, $password = NULL, $group = NULL, $permission = NULL, $exclusion = NULL, $email = NULL, $timezone = NULL, $comment = NULL, $expire = NULL)`

Creates or updates a user account.

| Parameter    | Type    | Description                     |
|--------------|---------|---------------------------------|
| `$user`      | string  | Username                        |
| `$disabled`  | boolean | Whether user is disabled        |
| `$name`      | string  | Display name                    |
| `$password`  | string  | Password (plaintext)            |
| `$group`     | string  | Group memberships               |
| `$permission`| string  | Permission rules                |
| `$exclusion` | string  | Exclusion rules                 |
| `$email`     | string  | Email address                   |
| `$timezone`  | string  | Timezone identifier             |
| `$comment`   | string  | Comment/description             |
| `$expire`    | integer | Days until expiration           |

**Return Values:**
- User key on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Validates and sanitizes input
2. Handles password hashing
3. Manages special users (admin, anonymous)
4. Sets expiration if specified
5. Validates timezone

**Usage Example:**
```php
$permission->user(
    "john_doe",
    FALSE,
    "John Doe",
    "secure123",
    "editors\nreviewers",
    "content.view\ncontent.edit",
    "",
    "john@example.com",
    "America/New_York",
    "Content editor",
    30
);
// Creates/updates user "john_doe" with specified properties
```

---

### `group($group, $disabled = NULL, $name = NULL, $permission = NULL, $exclusion = NULL, $comment = NULL)`

Creates or updates a group.

| Parameter    | Type    | Description                     |
|--------------|---------|---------------------------------|
| `$group`     | string  | Group name                      |
| `$disabled`  | boolean | Whether group is disabled       |
| `$name`      | string  | Display name                    |
| `$permission`| string  | Permission rules                |
| `$exclusion` | string  | Exclusion rules                 |
| `$comment`   | string  | Comment/description             |

**Return Values:**
- Group key on success
- `FALSE` on failure

**Usage Example:**
```php
$permission->group(
    "editors",
    FALSE,
    "Content Editors",
    "content.view\ncontent.edit",
    "",
    "Can edit content"
);
// Creates/updates group "editors"
```

---

### `permit($key, $access, $explicit = TRUE)`

Grants a permission to a user or group.

| Parameter  | Type    | Description                     |
|------------|---------|---------------------------------|
| `$key`     | string  | User/group key                  |
| `$access`  | string  | Access string to grant          |
| `$explicit`| boolean | Whether to explicitly add       |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->permit("user.john_doe", "content.edit");
// Grants "content.edit" permission to user "john_doe"
```

---

### `exclude($key, $access, $explicit = TRUE)`

Adds an exclusion to a user or group.

| Parameter  | Type    | Description                     |
|------------|---------|---------------------------------|
| `$key`     | string  | User/group key                  |
| `$access`  | string  | Access string to exclude        |
| `$explicit`| boolean | Whether to explicitly add       |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->exclude("group.editors", "content.delete");
// Excludes "content.delete" from group "editors"
```

---

### `optimize($value)`

Optimizes a permission/exclusion string by removing redundant entries.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value`  | string | Permission/exclusion string     |

**Return Values:**
- Optimized string

**Usage Example:**
```php
$optimized = $permission->optimize("content.view\ncontent.*\ncontent.edit");
// Returns "content.*" (more specific rules are redundant)
```

---

### `add_group($user, $group)`

Adds a user to one or more groups.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |
| `$group`  | string | Group(s) to add (newline-separated) |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->add_group("john_doe", "editors\nreviewers");
// Adds user "john_doe" to groups "editors" and "reviewers"
```

---

### `del_group($user, $group)`

Removes a user from one or more groups.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |
| `$group`  | string | Group(s) to remove (newline-separated) |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->del_group("john_doe", "reviewers");
// Removes user "john_doe" from group "reviewers"
```

---

### `delete($key)`

Deletes a user or group.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | User/group key to delete        |

**Return Values:**
- `TRUE` on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Handles user-specific cleanup
2. Removes group references from users
3. Calls `permission_delete()` for users

**Usage Example:**
```php
$permission->delete("user.john_doe");
// Deletes user "john_doe" and associated data
```

---

### `save()`

Optimizes and saves all permission data.

**Return Values:**
- Result of data save operation

**Usage Example:**
```php
$permission->save();
// Saves all permission data
```

---

### `verify_user($user, $password)`

Verifies user credentials and returns user data.

| Parameter  | Type   | Description                     |
|------------|--------|---------------------------------|
| `$user`    | string | Username                        |
| `$password`| string | Password (plaintext)            |

**Return Values:**
- Array with user data on success
- `FALSE` on failure

**Return Array:**
| Key         | Type   | Description                     |
|-------------|--------|---------------------------------|
| `superuser` | string | Superuser identifier            |
| `name`      | string | Display name                    |
| `profile`   | mixed  | Profile data                    |

**Usage Example:**
```php
$userData = $permission->verify_user("john_doe", "secure123");
// Returns user data if credentials are valid
```

---

### `get_user_permission($user)`

Retrieves all permissions for a user (including group permissions).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- Permission string

**Usage Example:**
```php
$permissions = $permission->get_user_permission("john_doe");
// Returns all permissions for user "john_doe"
```

---

### `get_user_exclusion($user)`

Retrieves all exclusions for a user (including group exclusions).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- Exclusion string

**Usage Example:**
```php
$exclusions = $permission->get_user_exclusion("john_doe");
// Returns all exclusions for user "john_doe"
```

---

### `given($access = NULL, $user = NULL, $password = NULL, $test = NULL)`

Checks if a specific access is granted to a user.

| Parameter  | Type    | Description                     |
|------------|---------|---------------------------------|
| `$access`  | string  | Access string to check          |
| `$user`    | string  | Username                        |
| `$password`| string  | Password (plaintext)            |
| `$test`    | boolean | Whether to skip password check  |

**Return Values:**
- `TRUE` if access is granted
- `FALSE` if access is denied

**Inner Mechanisms:**
1. Handles default access (current application)
2. Checks anonymous permissions
3. Verifies user credentials
4. Evaluates permissions and exclusions

**Usage Example:**
```php
$hasAccess = $permission->given("content.edit", "john_doe", "secure123");
// Returns TRUE if user "john_doe" has "content.edit" permission
```

---

### `test($access, $user)`

Tests if a user has a specific access (skips password verification).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$access` | string | Access string to check          |
| `$user`   | string | Username                        |

**Return Values:**
- `TRUE` if access is granted
- `FALSE` if access is denied

**Usage Example:**
```php
$hasAccess = $permission->test("content.edit", "john_doe");
// Returns TRUE if user "john_doe" has "content.edit" permission
```


<!-- HASH:afca657c8ccc165622a912ac35ec2e56 -->
