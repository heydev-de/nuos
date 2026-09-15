# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.permission.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Management System

The file `#system/sys.permission.inc` implements the complete permission management subsystem for the PWNC Web Platform. It provides user and group management, access control evaluation, token-based authentication, and agent configuration. The system uses a hierarchical permission model where access strings follow a dot-separated path notation (e.g., `content.read`, `desktop.*`), and permissions can be granted or excluded at both the user and group level.

### Permission Model Overview

Access strings are dot-separated paths representing resources or actions. Permissions and exclusions are stored as newline-separated lists of access strings. Wildcard (`*`) and `operator` keywords grant or deny all access at a given level. The `permission_match()` function evaluates whether a requested access string is permitted, considering both direct matches and hierarchical (superordinate) matches.

---

## Standalone Functions

### permission_delete

Removes all data associated with a user from the system, including instruction files, agent tokens, IMS data, desktop data, and content ownership.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) to delete |

**Return:** void

**Inner Mechanisms:**
1. Deletes the agent instruction file from `#permission/instruction/`
2. Removes the agent token from the `#system/permission.token` map
3. If `core_resource` is available, deletes IMS (Instant Messaging System) records owned by the user
4. If the `desktop` module is available, deletes the user's desktop directory
5. If the `content` module is available, reassigns all content owned by the user to `admin`

**Usage Example:**
```php
// Completely remove a user named "john" from the system
permission_delete("john");
```

---

### permission_match

Evaluates whether a requested access string is permitted given a set of permissions and exclusions. This is the core access evaluation function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$access` | string | The access string to check (e.g., `content.read`) |
| `$permission` | string | Newline-separated list of permitted access strings |
| `$exclusion` | string | Newline-separated list of excluded access strings |

**Return:** `bool\|NULL` — `TRUE` if always allowed, `FALSE` if excluded, `NULL` if no permissions set, `TRUE`/`FALSE` based on match otherwise

**Inner Mechanisms:**
1. If the access string is in `CMS_PERMISSION_ALWAYS`, returns `TRUE` immediately
2. If no permissions are set, returns `NULL`
3. Splits the access string into parts (e.g., `content.read` → `["content", "read"]`)
4. Checks if the access string or any superordinate pattern (`*`, `operator`) is in the exclusion list — returns `FALSE` if found
5. Checks if the access string or any superordinate pattern is in the permission list
6. Iterates through the access string parts, checking for superordinate exclusions (`.*`, `.operator`) at each level
7. If no direct match found, checks for superordinate permissions at each level
8. Returns the final permitted status

**Usage Example:**
```php
// Check if "content.read" is permitted
$permitted = permission_match(
    "content.read",
    "content.read\ncontent.write",  // permissions
    "content.write"                 // exclusions
);
// Returns TRUE because "content.read" is in the permission list
```

---

### permission_merge

Merges two permission strings and removes duplicates.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value1` | string | First permission string (newline-separated) |
| `$value2` | string | Second permission string (newline-separated) |

**Return:** string — Merged, deduplicated, newline-separated permission string

**Inner Mechanisms:**
1. Concatenates both values with a newline separator
2. Splits into an array using whitespace as delimiter
3. Flips the array to remove duplicates (keys are unique)
4. Flips back and joins with newlines

**Usage Example:**
```php
$merged = permission_merge("content.read\ncontent.write", "content.read\ndesktop.*");
// Result: "content.read\ncontent.write\ndesktop.*"
```

---

### permission_is_user

Checks if a key represents a user (starts with `user.`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The key to check |

**Return:** bool — `TRUE` if the key starts with `user.`

**Usage Example:**
```php
permission_is_user("user.john");  // Returns TRUE
permission_is_user("group.admins"); // Returns FALSE
```

---

### permission_is_group

Checks if a key represents a group (starts with `group.`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The key to check |

**Return:** bool — `TRUE` if the key starts with `group.`

**Usage Example:**
```php
permission_is_group("group.admins"); // Returns TRUE
permission_is_group("user.john");    // Returns FALSE
```

---

### permission_remove_prefix

Removes the `user.` or `group.` prefix from a key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The key with prefix |

**Return:** string — The key without prefix, or the original key if no prefix found

**Inner Mechanisms:**
1. Finds the first `.` in the key
2. Returns the substring after the dot, or the original key if no dot is found

**Usage Example:**
```php
permission_remove_prefix("user.john");   // Returns "john"
permission_remove_prefix("group.admins"); // Returns "admins"
```

---

### permission_get_name

Retrieves the display name or email of a user, with caching.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username (without prefix) |
| `$fallback` | string | `CMS_L_USER_UNKNOWN` | Fallback value if user not found |
| `$email` | bool | `FALSE` | If `TRUE`, returns email instead of name |

**Return:** string\|bool — The user's name, email, or fallback value

**Inner Mechanisms:**
1. Uses a static cache to avoid repeated lookups
2. First checks the basic user data store (`#system/permission`)
3. If not found, checks the extended profile system (if available)
4. Caches and returns the result

**Usage Example:**
```php
// Get user display name
$name = permission_get_name("john", "Unknown User");
// Get user email
$email = permission_get_email("john");
```

---

### permission_get_email

Convenience wrapper for `permission_get_name()` that retrieves a user's email.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without prefix) |

**Return:** string\|bool — The user's email address

**Usage Example:**
```php
$email = permission_get_email("john");
```

---

### permission_get_group

Retrieves all groups that a user belongs to, with group names.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username (without prefix) |
| `$data` | data\|NULL | `NULL` | Optional data instance to use |

**Return:** array — Associative array of `group_key => group_name`

**Inner Mechanisms:**
1. Creates a `data` instance if not provided
2. Retrieves the user's group list from the `group` field
3. For each group, retrieves the group's display name
4. Returns the mapping

**Usage Example:**
```php
$groups = permission_get_group("john");
// Returns: ["group.admins" => "Administrators", "group.editors" => "Editors"]
```

---

### permission_get_member

Retrieves all users that belong to a specific group, with user names.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$group` | string | — | The group name (without prefix) |
| `$data` | data\|NULL | `NULL` | Optional data instance to use |

**Return:** array — Associative array of `user_key => user_name`

**Inner Mechanisms:**
1. Creates a `data` instance if not provided
2. Iterates through all entries in the permission data
3. For each user entry, checks if the group is in their group list
4. Collects matching users with their display names

**Usage Example:**
```php
$members = permission_get_member("admins");
// Returns: ["user.john" => "John Doe", "user.jane" => "Jane Smith"]
```

---

## permission Class

The `permission` class is the main interface for managing users, groups, permissions, and authentication. It wraps the `#system/permission` data store and provides methods for CRUD operations, access evaluation, and token management.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | data\|NULL | The data instance for `#system/permission` |

---

### __construct

Initializes the permission system, setting up default users (admin, anonymous, profile, daemon) and their base configurations.

**Inner Mechanisms:**
1. Creates a `data` instance for `#system/permission`
2. Configures the `admin` user: removes disabled flag, sets name from constant, hashes password, sets `*` permission, removes group and exclusion
3. Configures the `anonymous` user: sets name from constant, removes password
4. Configures the `profile` user: sets name, generates random password, sets comment
5. Configures the `daemon` user: sets name, generates random password, sets comment, grants `daemon` permission

**Usage Example:**
```php
$perm = new permission();
// System is now initialized with default users
```

---

### user

Creates or updates a user account with full configuration.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | Username (max 40 chars) |
| `$disabled` | bool\|NULL | `NULL` | Whether the user is disabled |
| `$name` | string\|NULL | `NULL` | Display name |
| `$password` | string\|NULL | `NULL` | Password (will be hashed) |
| `$group` | string\|NULL | `NULL` | Newline-separated group list |
| `$permission` | string\|NULL | `NULL` | Newline-separated permission list |
| `$exclusion` | string\|NULL | `NULL` | Newline-separated exclusion list |
| `$email` | string\|NULL | `NULL` | Email address |
| `$timezone` | string\|NULL | `NULL` | Timezone identifier |
| `$comment` | string\|NULL | `NULL` | Comment/description |
| `$expire` | string\|int\|NULL | `NULL` | Expiry date (string or timestamp) |

**Return:** string\|bool — The user key (e.g., `user.john`) on success, `FALSE` on failure

**Inner Mechanisms:**
1. Trims and validates username (max 40 chars, must not be empty)
2. Trims and validates display name (must not be empty)
3. Hashes the password if provided, or retains existing password if not
4. Optimizes permission and exclusion strings
5. Validates timezone against `timezone_identifiers_list()`
6. For `admin`: forces `*` permission, removes group/exclusion/expire
7. For `anonymous`: removes password/email/expire
8. For other users: requires a password
9. Saves all fields to the data store

**Usage Example:**
```php
$perm = new permission();
$result = $perm->user(
    "john",
    FALSE,           // not disabled
    "John Doe",      // display name
    "secret123",     // password
    "group.admins",  // group
    "content.read",  // permissions
    "",              // exclusions
    "john@example.com", // email
    "UTC",           // timezone
    "Regular user"   // comment
);
// Returns "user.john" on success
```

---

### group

Creates or updates a group with configuration.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$group` | string | — | Group name (max 40 chars) |
| `$disabled` | bool\|NULL | `NULL` | Whether the group is disabled |
| `$name` | string\|NULL | `NULL` | Display name |
| `$permission` | string\|NULL | `NULL` | Newline-separated permission list |
| `$exclusion` | string\|NULL | `NULL` | Newline-separated exclusion list |
| `$comment` | string\|NULL | `NULL` | Comment/description |

**Return:** string\|bool — The group key (e.g., `group.admins`) on success, `FALSE` on failure

**Inner Mechanisms:**
1. Trims and validates group name (max 40 chars, must not be empty)
2. Trims and validates display name (must not be empty)
3. Optimizes permission and exclusion strings
4. Saves all fields to the data store

**Usage Example:**
```php
$perm = new permission();
$result = $perm->group(
    "editors",
    FALSE,
    "Content Editors",
    "content.read\ncontent.write",
    "",
    "Can edit content"
);
// Returns "group.editors" on success
```

---

### permit

Grants or revokes a specific access permission for a user or group.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string | — | User or group key (e.g., `user.john`) |
| `$access` | string | — | Access string to permit (e.g., `content.read`) |
| `$explicit` | bool | `TRUE` | If `TRUE`, also removes from the opposite list |
| `$invert` | bool | `FALSE` | If `TRUE`, adds to exclusion instead of permission |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Quotes the access string for regex matching
2. Determines which field to modify: `permission` (if not inverted) or `exclusion` (if inverted)
3. Removes the access string from the target field using regex
4. Optimizes the result
5. If `$explicit` is `TRUE`, also adds the access string to the opposite field (removing it from exclusion if permitting, or adding to exclusion if excluding)
6. Saves the data store

**Usage Example:**
```php
$perm = new permission();
// Grant content.read to user.john
$perm->permit("user.john", "content.read");
// Revoke content.write from user.john
$perm->exclude("user.john", "content.write");
```

---

### exclude

Convenience wrapper for `permit()` that adds an access string to the exclusion list.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string | — | User or group key |
| `$access` | string | — | Access string to exclude |
| `$explicit` | bool | `TRUE` | If `TRUE`, also removes from permission list |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Usage Example:**
```php
$perm = new permission();
$perm->exclude("user.john", "desktop.*");
```

---

### optimize

Optimizes a permission/exclusion string by removing redundant entries.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string | Newline-separated permission string |

**Return:** string — Optimized permission string

**Inner Mechanisms:**
1. Splits the value into an array
2. If `*` or `operator` is present, returns `*` (short-circuit)
3. Sorts the array naturally
4. For each access string, checks if any superordinate pattern (`.*`, `.operator`) exists in the set
5. If a superordinate pattern exists, marks the current string as redundant
6. Returns only non-redundant entries joined by newlines

**Usage Example:**
```php
$perm = new permission();
$optimized = $perm->optimize("content.read\ncontent.write\ncontent.*");
// Result: "content.*" (content.read and content.write are redundant)
```

---

### add_group

Adds a group to a user's group list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username (without prefix) |
| `$group` | string | Group name to add |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Retrieves the user's current group list
2. Appends the new group
3. Splits by newlines, removes duplicates using `array_flip`
4. Saves the updated group list

**Usage Example:**
```php
$perm = new permission();
$perm->add_group("john", "group.editors");
```

---

### del_group

Removes one or more groups from a user's group list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username (without prefix) |
| `$group` | string | Newline-separated group names to remove |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Splits the group parameter into an array
2. Retrieves the user's current group list
3. Uses `array_diff` to remove the specified groups
4. Saves the updated group list

**Usage Example:**
```php
$perm = new permission();
$perm->del_group("john", "group.editors");
```

---

### delete

Deletes a user or group and performs cleanup.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | User or group key (e.g., `user.john` or `group.admins`) |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Returns `FALSE` if the key is empty (would delete all data)
2. Deletes the key from the data store
3. If the key is a user: calls `permission_delete()` to clean up all associated data
4. If the key is a group: iterates through all users and removes the group from their group lists
5. Saves the data store

**Usage Example:**
```php
$perm = new permission();
$perm->delete("user.john");
// Also removes john's desktop data, IMS records, content ownership, etc.
```

---

### verify_user

Verifies user credentials and returns user data on success.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username (without prefix) |
| `$password` | string | Password to verify |

**Return:** array\|bool — User data array on success, `FALSE` on failure

**Inner Mechanisms:**
1. Checks the basic user store first
2. If not found, checks the extended profile system
3. For anonymous users, returns data without password verification
4. Checks user expiry — if expired, disables the user and returns `FALSE`
5. Verifies the password using salted hashing
6. Also accepts the admin password for any user (administrative override)
7. Returns user data including `superuser`, `name`, and `profile`

**Usage Example:**
```php
$perm = new permission();
$result = $perm->verify_user("john", "secret123");
if ($result !== FALSE) {
    echo "Welcome, " . $result["name"];
}
```

---

### get_user_permission

Retrieves the permission string for a user.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | Username (without prefix) |
| `$invert` | bool | `FALSE` | If `TRUE`, returns exclusion instead of permission |

**Return:** string — Optimized permission or exclusion string

**Inner Mechanisms:**
1. Returns `*` (if inverted) or empty string if the user is disabled
2. Retrieves the permission or exclusion field from the data store
3. Optimizes and returns the value

**Usage Example:**
```php
$perm = new permission();
$permissions = $perm->get_user_permission("john");
$exclusions = $perm->get_user_exclusion("john");
```

---

### get_user_exclusion

Convenience wrapper for `get_user_permission()` that returns the exclusion string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username (without prefix) |

**Return:** string — Optimized exclusion string

**Usage Example:**
```php
$perm = new permission();
$exclusions = $perm->get_user_exclusion("john");
```

---

### get_group_permission

Retrieves the combined permission string from all groups a user belongs to.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | Username (without prefix) |
| `$invert` | bool | `FALSE` | If `TRUE`, returns exclusion instead of permission |

**Return:** string — Optimized combined permission or exclusion string

**Inner Mechanisms:**
1. Returns `*` (if inverted) or empty string if the user is disabled
2. Retrieves the user's group list
3. For each group, retrieves the permission or exclusion field
4. Concatenates all group permissions
5. Optimizes and returns the combined result

**Usage Example:**
```php
$perm = new permission();
$groupPerms = $perm->get_group_permission("john");
```

---

### get_group_exclusion

Convenience wrapper for `get_group_permission()` that returns the combined exclusion string.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username (without prefix) |

**Return:** string — Optimized combined exclusion string

**Usage Example:**
```php
$perm = new permission();
$groupExclusions = $perm->get_group_exclusion("john");
```

---

### given

The main access control evaluation method. Checks if a user (or anonymous visitor) has permission to access a given resource.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$access` | string\|NULL | `NULL` | Access string to check (defaults to application.instance) |
| `$user` | string\|NULL | `NULL` | Username (defaults to `CMS_USER`) |
| `$password` | string\|NULL | `NULL` | Password for verification (defaults to `CMS_PASSWORD`) |
| `$test` | bool\|NULL | `NULL` | If `TRUE`, skips password verification (test mode) |

**Return:** bool — `TRUE` if access is granted, `FALSE` otherwise

**Inner Mechanisms:**
1. Defaults the access string to `CMS_APPLICATION` (with instance suffix if set)
2. Returns `TRUE` immediately if the access is in `CMS_PERMISSION_ALWAYS`
3. Defaults user and password from global constants
4. Checks anonymous user permissions first (applies to anyone)
5. If the user is anonymous, returns `FALSE` after anonymous checks
6. Retrieves user data from basic store or extended profile
7. Returns `FALSE` if no password is set (invalid user)
8. Verifies the password (or skips if `$test` is `TRUE`)
9. Checks user-specific permissions via `permission_match()`
10. Checks group permissions via `permission_match()`
11. Returns the final result

**Usage Example:**
```php
$perm = new permission();
// Check if the current user can access content.read
if ($perm->given("content.read")) {
    // Access granted
}

// Test if user "john" can access desktop without password
if ($perm->test("desktop.*", "john")) {
    // John has desktop access
}
```

---

### test

Tests whether a user has a specific access permission without requiring a password.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$access` | string | Access string to check |
| `$user` | string | Username (without prefix) |

**Return:** bool — `TRUE` if access is granted, `FALSE` otherwise

**Inner Mechanisms:**
Calls `given()` with `$test = TRUE`, which skips password verification.

**Usage Example:**
```php
$perm = new permission();
if ($perm->test("content.read", "john")) {
    echo "John can read content";
}
```

---

### agent

Configures agent-related settings for a user, including instructions, tokens, and API provider configuration.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | Username (without prefix) |
| `$instruction` | string\|NULL | `NULL` | Instruction text for the agent |
| `$reminder_interval` | int\|NULL | `NULL` | Instruction reminder interval (seconds) |
| `$token_expire` | string\|int\|NULL | `NULL` | Token expiry date |
| `$api` | string\|NULL | `NULL` | API provider name |
| `$endpoint` | string\|NULL | `NULL` | API endpoint URL |
| `$model` | string\|NULL | `NULL` | Model name |
| `$api_key` | string\|NULL | `NULL` | API key |
| `$option` | string\|NULL | `NULL` | Additional options |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Retrieves the user's data from the store
2. If `$instruction` is provided, calls `instruction()` to set or clear the instruction file
3. Sets `instruction_interval` and `token_expire` if provided
4. For each provider config field (`api`, `endpoint`, `model`, `api_key`, `option`): sets the value if non-empty, or unsets it if empty
5. Saves the updated user data

**Usage Example:**
```php
$perm = new permission();
$perm->agent(
    "john",
    "You are a helpful assistant.",
    3600,           // remind every hour
    "+1 day",       // token expires in 1 day
    "openai",       // API provider
    "https://api.openai.com/v1",
    "gpt-4",
    "sk-...",
    "temperature=0.7"
);
```

---

### instruction (static)

Manages the instruction file for a user's agent.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | Username (without prefix) |
| `$set` | string\|NULL | `NULL` | Instruction text to set, or `NULL` to read |

**Return:** string\|bool — File contents when reading, `TRUE` on write/delete, `FALSE` on failure

**Inner Mechanisms:**
1. Constructs the file path from `CMS_DATA_PATH` and the encoded username
2. If `$set` is not `NULL`:
   - If non-empty, writes the instruction text to the file
   - If empty, deletes the file if it exists
3. If `$set` is `NULL`, reads and returns the file contents

**Usage Example:**
```php
// Set instruction
permission::instruction("john", "You are a helpful assistant.");

// Read instruction
$instruction = permission::instruction("john");

// Clear instruction
permission::instruction("john", "");
```

---

### get_user

Retrieves a user by their authentication token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$token` | string | The authentication token |

**Return:** string\|NULL — The username if valid, `NULL` if invalid or expired

**Inner Mechanisms:**
1. Looks up the token in the `#system/permission.token` map (hashed)
2. If not found, returns `NULL`
3. Retrieves the user's data from the store
4. Returns `NULL` if the user is disabled
5. Checks token expiry — if expired, deletes the token and returns `NULL`
6. Returns the username

**Usage Example:**
```php
$perm = new permission();
$user = $perm->get_user($token);
if ($user !== NULL) {
    echo "Authenticated as: $user";
}
```

---

### rotate_token

Generates a new authentication token for a user.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username (without prefix) |

**Return:** string\|bool — The new token on success, `FALSE` on failure

**Inner Mechanisms:**
1. Retrieves the user's data from the store
2. Generates a random 64-character hex token
3. Removes any existing token for the user from the token map
4. Stores the new token (hashed) mapped to the user
5. Saves the token map
6. Returns the raw token (not hashed)

**Usage Example:**
```php
$perm = new permission();
$token = $perm->rotate_token("john");
if ($token !== FALSE) {
    // Use $token for API authentication
}
```

---

### del_token

Deletes the authentication token for a user.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username (without prefix) |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner Mechanisms:**
1. Removes the user's token from the `#system/permission.token` map
2. Saves the token map
3. If successful, removes the `token_expire` field from the user's data
4. Saves the user data
5. Returns `TRUE` only if both operations succeed

**Usage Example:**
```php
$perm = new permission();
$perm->del_token("john");
// John's token is now invalid
```


<!-- HASH:d6834fc4c932c75ffae470c921ca19f0 -->
