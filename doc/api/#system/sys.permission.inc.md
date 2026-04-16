# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.permission.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Permission Management System

This file provides the core permission management functionality for the NUOS platform. It includes functions and a class to handle user/group permissions, access control, and user data management.

---

## Functions

### `permission_delete($user)`

Removes all user-related data from the system when a user is deleted.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username to be deleted          |

**Inner Mechanisms:**
1. Removes Instant Messaging System (IMS) data associated with the user
2. Deletes desktop data if the desktop module is available
3. Transfers ownership of content from the deleted user to the administrator

**Usage Context:**
Called when a user account is permanently removed from the system.

---

### `permission_match($access, $permission, $exclusion)`

Checks if a specific access is permitted based on permission and exclusion rules.

| Parameter    | Type   | Description                                      |
|--------------|--------|--------------------------------------------------|
| `$access`    | string | Access string to check (e.g., "content.edit")    |
| `$permission`| string | Newline-separated list of permitted access rules |
| `$exclusion` | string | Newline-separated list of excluded access rules  |

**Return Values:**
- `TRUE` if access is permitted
- `FALSE` if access is denied

**Inner Mechanisms:**
1. Checks for always-allowed permissions (CMS_PERMISSION_ALWAYS)
2. Verifies if the access is specifically excluded
3. Checks for direct permission matches
4. Evaluates hierarchical permission structures (e.g., "content.*" permits "content.edit")

**Usage Context:**
Used to determine if a user has permission to perform specific actions.

---

### `permission_merge($value1, $value2)`

Merges two permission/exclusion strings while removing duplicates.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value1` | string | First permission/exclusion set  |
| `$value2` | string | Second permission/exclusion set |

**Return Values:**
- string: Merged and deduplicated permission/exclusion set

**Usage Context:**
Used when combining permissions from multiple sources (e.g., user and group permissions).

---

### `permission_is_user($key)`

Determines if a permission key represents a user.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | Permission key to check         |

**Return Values:**
- `TRUE` if the key represents a user
- `FALSE` otherwise

**Usage Context:**
Used to distinguish between user and group permission keys.

---

### `permission_is_group($key)`

Determines if a permission key represents a group.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | Permission key to check         |

**Return Values:**
- `TRUE` if the key represents a group
- `FALSE` otherwise

**Usage Context:**
Used to distinguish between user and group permission keys.

---

### `permission_get_name($user, $email = FALSE)`

Retrieves the name or email of a user.

| Parameter | Type    | Description                                      |
|-----------|---------|--------------------------------------------------|
| `$user`   | string  | Username                                         |
| `$email`  | boolean | If TRUE, returns email instead of name (default: FALSE) |

**Return Values:**
- string: User's name or email
- NULL: If email is requested but not available

**Inner Mechanisms:**
1. Uses static caching to improve performance
2. Checks basic user data first
3. Falls back to profile data if available

**Usage Context:**
Used to display user information throughout the system.

---

### `permission_get_email($user)`

Convenience function to retrieve a user's email address.

| Parameter | Type   | Description     |
|-----------|--------|-----------------|
| `$user`   | string | Username        |

**Return Values:**
- string: User's email address
- NULL: If no email is available

**Usage Context:**
Used when email information is needed for a specific user.

---

## Permission Class

The `permission` class provides comprehensive permission management functionality.

### Properties

| Name     | Type   | Description                          |
|----------|--------|--------------------------------------|
| `data`   | object | Data storage object for permissions  |
| `mysql`  | object | MySQL connection object              |
| `buffer` | mixed  | Internal buffer (currently unused)   |

---

### `__construct()`

Initializes the permission system and sets up default users.

**Inner Mechanisms:**
1. Creates data storage for permissions
2. Configures default users (admin, anonymous, profile, daemon)
3. Sets default permissions for these users

**Usage Context:**
Automatically called when creating a new permission object.

---

### `user($user, $disabled = NULL, $name = NULL, $password = NULL, $group = NULL, $permission = NULL, $exclusion = NULL, $email = NULL, $timezone = NULL, $comment = NULL, $expire = NULL)`

Creates or updates a user account.

| Parameter    | Type    | Description                                      |
|--------------|---------|--------------------------------------------------|
| `$user`      | string  | Username (max 40 chars)                          |
| `$disabled`  | boolean | Whether the user is disabled (default: NULL)     |
| `$name`      | string  | User's display name                              |
| `$password`  | string  | User's password (plaintext)                      |
| `$group`     | string  | Newline-separated list of groups                 |
| `$permission`| string  | Newline-separated list of permissions            |
| `$exclusion` | string  | Newline-separated list of exclusions             |
| `$email`     | string  | User's email address                             |
| `$timezone`  | string  | User's timezone (default: CMS_TIMEZONE)          |
| `$comment`   | string  | Administrative comment                           |
| `$expire`    | integer | Number of days until account expires (default: NULL) |

**Return Values:**
- string: The user's permission key ("user.$user")
- FALSE: If required fields are missing or invalid

**Inner Mechanisms:**
1. Validates and sanitizes input data
2. Handles special cases for admin and anonymous users
3. Sets expiration time if specified
4. Hashes passwords before storage

**Usage Context:**
Used to create new users or update existing user accounts.

---

### `group($group, $disabled = NULL, $name = NULL, $permission = NULL, $exclusion = NULL, $comment = NULL)`

Creates or updates a user group.

| Parameter    | Type    | Description                                      |
|--------------|---------|--------------------------------------------------|
| `$group`     | string  | Group name (max 40 chars)                        |
| `$disabled`  | boolean | Whether the group is disabled (default: NULL)    |
| `$name`      | string  | Group's display name                             |
| `$permission`| string  | Newline-separated list of permissions            |
| `$exclusion` | string  | Newline-separated list of exclusions             |
| `$comment`   | string  | Administrative comment                           |

**Return Values:**
- string: The group's permission key ("group.$group")

**Usage Context:**
Used to create new groups or update existing group settings.

---

### `permit($key, $access, $explicit = TRUE)`

Grants a specific permission to a user or group.

| Parameter  | Type    | Description                                      |
|------------|---------|--------------------------------------------------|
| `$key`     | string  | Permission key (user or group)                   |
| `$access`  | string  | Access string to grant                           |
| `$explicit`| boolean | Whether to explicitly add the permission (default: TRUE) |

**Return Values:**
- TRUE: On success

**Inner Mechanisms:**
1. Removes any exclusion for the specified access
2. Adds the permission if explicitly requested

**Usage Context:**
Used to grant specific permissions to users or groups.

---

### `exclude($key, $access, $explicit = TRUE)`

Explicitly denies a specific permission to a user or group.

| Parameter  | Type    | Description                                      |
|------------|---------|--------------------------------------------------|
| `$key`     | string  | Permission key (user or group)                   |
| `$access`  | string  | Access string to deny                            |
| `$explicit`| boolean | Whether to explicitly add the exclusion (default: TRUE) |

**Return Values:**
- TRUE: On success

**Inner Mechanisms:**
1. Removes any permission for the specified access
2. Adds the exclusion if explicitly requested

**Usage Context:**
Used to explicitly deny specific permissions to users or groups.

---

### `optimize($value)`

Optimizes a permission or exclusion string by removing redundant entries.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value`  | string | Permission/exclusion string     |

**Return Values:**
- string: Optimized permission/exclusion string

**Inner Mechanisms:**
1. Sorts access strings naturally
2. Removes redundant access strings that are covered by broader permissions

**Usage Context:**
Used to clean up permission/exclusion strings before saving.

---

### `add_group($user, $group)`

Adds a user to one or more groups.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |
| `$group`  | string | Newline-separated list of groups|

**Return Values:**
- TRUE: On success

**Usage Context:**
Used to assign users to groups.

---

### `del_group($user, $group)`

Removes a user from one or more groups.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |
| `$group`  | string | Newline-separated list of groups|

**Return Values:**
- TRUE: On success

**Usage Context:**
Used to remove users from groups.

---

### `delete($key)`

Deletes a user or group and performs necessary cleanup.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | Permission key to delete        |

**Return Values:**
- TRUE: On success
- FALSE: If the key is empty

**Inner Mechanisms:**
1. Deletes the specified user or group
2. For users: calls permission_delete() to clean up related data
3. For groups: removes the group from all users

**Usage Context:**
Used to permanently remove users or groups from the system.

---

### `save()`

Optimizes and saves all permission data.

**Return Values:**
- Result of the data object's save operation

**Inner Mechanisms:**
1. Optimizes all permission and exclusion strings
2. Saves the data to persistent storage

**Usage Context:**
Called after making multiple changes to permission data.

---

### `verify_user($user, $password)`

Verifies a user's credentials and returns user data if valid.

| Parameter  | Type   | Description                     |
|------------|--------|---------------------------------|
| `$user`    | string | Username                        |
| `$password`| string | Password (plaintext)            |

**Return Values:**
- array: User data (superuser, name, profile) if credentials are valid
- FALSE: If credentials are invalid or user is disabled

**Inner Mechanisms:**
1. Checks basic user data first
2. Falls back to profile data if available
3. Verifies password against stored hash

**Usage Context:**
Used during authentication to verify user credentials.

---

### `get_user_permission($user)`

Retrieves the combined permissions for a user (including group permissions).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- string: Newline-separated list of permissions
- NULL: If the user is disabled

**Inner Mechanisms:**
1. Gets direct user permissions
2. Adds permissions from all groups the user belongs to

**Usage Context:**
Used to determine what a user is allowed to do.

---

### `get_user_exclusion($user)`

Retrieves the combined exclusions for a user (including group exclusions).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- string: Newline-separated list of exclusions
- "*": If the user is disabled

**Inner Mechanisms:**
1. Gets direct user exclusions
2. Adds exclusions from all groups the user belongs to

**Usage Context:**
Used to determine what a user is explicitly denied.

---

### `given($access = NULL, $user = NULL, $password = NULL, $test = NULL)`

Checks if a specific access is permitted for a user.

| Parameter  | Type    | Description                                      |
|------------|---------|--------------------------------------------------|
| `$access`  | string  | Access string to check (default: CMS_APPLICATION)|
| `$user`    | string  | Username (default: CMS_USER)                     |
| `$password`| string  | Password (default: CMS_PASSWORD)                 |
| `$test`    | boolean | If TRUE, skips password verification (default: NULL) |

**Return Values:**
- TRUE: If access is permitted
- FALSE: If access is denied

**Inner Mechanisms:**
1. Checks anonymous permissions first (which override user permissions)
2. Verifies user credentials if not anonymous
3. Evaluates user permissions and exclusions

**Usage Context:**
The primary function for checking if a user has permission to perform an action.

---

### `test($access, $user)`

Convenience function to test if a user has a specific permission (skips password verification).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$access` | string | Access string to check          |
| `$user`   | string | Username                        |

**Return Values:**
- TRUE: If access is permitted
- FALSE: If access is denied

**Usage Context:**
Used when you need to check permissions without verifying credentials.


<!-- HASH:d756c60a48912247b54ed0ea5f45a18b -->
