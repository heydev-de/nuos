# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.permission.inc)

- **Version:** `26.9.21.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# Permission System

The `sys.permission.inc` file implements the complete permission and authentication system for the PWNC Web Platform. It provides user and group management, access control evaluation, token-based authentication, and agent configuration. The system uses a hierarchical permission model where access strings follow a dot-separated path notation (e.g., `content.create.page`), and permissions can be granted or excluded at both user and group levels.

## Core Concepts

- **Access strings** are dot-separated paths representing resources or actions (e.g., `content.create`, `desktop.file.read`)
- **Permissions** grant access; **exclusions** deny access
- **Wildcard `*`** grants or denies all access at a given level
- **Superordinate checks** allow parent-level permissions to apply to child paths (e.g., `content.*` covers `content.create`)
- **Operator-level** permissions (`*.operator`) provide a special administrative tier
- Users can belong to multiple groups, and group permissions are aggregated

## Standalone Functions

### permission_delete

Removes all data associated with a user from the system, including instruction files, tokens, IMS data, desktop data, and content ownership.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) to delete |

**Return:** void

**Inner mechanisms:**
1. Deletes the agent instruction file from `#permission/instruction/`
2. Removes all agent tokens from `#system/permission.token`
3. If `core_resource` is available, removes IMS (Instant Messaging System) data owned by the user
4. If `desktop` is available, removes the user's desktop directory
5. Reassigns all content owned by the user to `admin`

**Usage example:**
```php
// Completely remove a user and all their data
permission_delete("john_doe");
```

### permission_match

Evaluates whether a given access request matches the provided permission and exclusion sets, implementing hierarchical matching with wildcard and operator support.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$access` | string | The access string to check (e.g., `content.create.page`) |
| `$permission` | string | Newline-separated list of permitted access strings |
| `$exclusion` | string | Newline-separated list of excluded access strings |

**Return:** `bool\|NULL` — `TRUE` if permitted, `FALSE` if excluded, `NULL` if no match (inconclusive)

**Inner mechanisms:**
1. If the access is in `CMS_PERMISSION_ALWAYS`, returns `TRUE` immediately
2. If no permissions are set, returns `NULL`
3. Splits the access string into parts for hierarchical traversal
4. Converts exclusion and permission strings into flipped arrays for O(1) lookup
5. Checks for global (`*`) or operator-level exclusions at the top level
6. Checks for global or operator-level permissions
7. Traverses the access path element by element, checking for superordinate exclusions (`.*` and `.operator`) and permissions at each level
8. Returns the final permission state

**Usage example:**
```php
// Check if "content.create.page" is permitted
$permitted = permission_match(
    "content.create.page",
    "content.create\ncontent.*",
    "content.delete"
);
// Returns TRUE because "content.*" covers "content.create.page"
```

### permission_merge

Merges two permission strings and removes duplicates.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value1` | string | First permission string (newline-separated) |
| `$value2` | string | Second permission string (newline-separated) |

**Return:** string — Merged, deduplicated, newline-separated permission string

**Inner mechanisms:**
1. Concatenates both values with a newline separator
2. Splits on whitespace into an array
3. Flips the array to remove duplicates
4. Returns the keys as a newline-separated string

**Usage example:**
```php
$merged = permission_merge("content.create\ncontent.read", "content.read\ncontent.update");
// Result: "content.create\ncontent.read\ncontent.update"
```

### permission_is_user

Checks if a key represents a user entry.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The key to check |

**Return:** bool — `TRUE` if the key starts with `user.`

**Usage example:**
```php
permission_is_user("user.john");  // TRUE
permission_is_user("group.admins"); // FALSE
```

### permission_is_group

Checks if a key represents a group entry.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The key to check |

**Return:** bool — `TRUE` if the key starts with `group.`

**Usage example:**
```php
permission_is_group("group.admins"); // TRUE
permission_is_group("user.john");    // FALSE
```

### permission_remove_prefix

Removes the `user.` or `group.` prefix from a key.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The key to process |

**Return:** string — The key without its prefix, or the original key if no prefix exists

**Usage example:**
```php
permission_remove_prefix("user.john");   // "john"
permission_remove_prefix("group.admins"); // "admins"
```

### permission_get_name

Retrieves the display name or email of a user, with caching and fallback support.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username to look up |
| `$fallback` | string | `CMS_L_USER_UNKNOWN` | Fallback name if user is not found |
| `$email` | bool | `FALSE` | If `TRUE`, returns email instead of name |

**Return:** string — The user's name or email, or the fallback value

**Inner mechanisms:**
1. Uses a static cache to avoid repeated lookups
2. Checks the basic permission data store (`#system/permission`)
3. If not found and the `profile` module is available, checks extended user profiles
4. Caches and returns the result

**Usage example:**
```php
$name = permission_get_name("john_doe", "Unknown User");
$email = permission_get_email("john_doe");
```

### permission_get_email

Convenience wrapper for `permission_get_name` that retrieves a user's email.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username to look up |

**Return:** string — The user's email address, or `NULL` if not found

**Usage example:**
```php
$email = permission_get_email("john_doe");
```

### permission_get_group

Retrieves all groups that a user belongs to, with group names.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username to look up |
| `$data` | data\|NULL | `NULL` | Optional data instance for the permission store |

**Return:** array — Associative array of `group_key => group_name`

**Inner mechanisms:**
1. Gets the user's group list from the permission data
2. For each group, retrieves the group's display name
3. Returns the mapping

**Usage example:**
```php
$groups = permission_get_group("john_doe");
// e.g., ["group.admins" => "Administrators", "group.editors" => "Editors"]
```

### permission_get_member

Retrieves all users that belong to a specific group, with user names.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$group` | string | — | The group name (without `group.` prefix) |
| `$data` | data\|NULL | `NULL` | Optional data instance for the permission store |

**Return:** array — Associative array of `user_key => user_name`

**Inner mechanisms:**
1. Iterates through all entries in the permission data
2. For each user entry, checks if the group is in their group list
3. Collects matching users with their display names

**Usage example:**
```php
$members = permission_get_member("admins");
// e.g., ["user.john" => "John Doe", "user.jane" => "Jane Smith"]
```

## permission Class

The main permission management class that handles user/group CRUD operations, access verification, and token management.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `$data` | data | Data instance for `#system/permission` storage |

### __construct

Initializes the permission system by ensuring default system users exist with proper configuration.

**Inner mechanisms:**
1. Creates a data instance for `#system/permission`
2. Ensures the `admin` user has: no disabled flag, a name, a default password hash, wildcard permissions, and no exclusions
3. Ensures the `anonymous` user has a name
4. Ensures the `profile` user has a name and a random password
5. Ensures the `daemon` user has a name, a random password, and the `daemon` permission

**Usage example:**
```php
$perm = new permission();
// System is now initialized with default users
```

### user

Creates or updates a user account with full configuration.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | Username (max 40 chars) |
| `$disabled` | bool\|NULL | `NULL` | Whether the account is disabled |
| `$name` | string\|NULL | `NULL` | Display name |
| `$password` | string\|NULL | `NULL` | Password (will be hashed) |
| `$group` | string\|NULL | `NULL` | Newline-separated group list |
| `$permission` | string\|NULL | `NULL` | Newline-separated permission list |
| `$exclusion` | string\|NULL | `NULL` | Newline-separated exclusion list |
| `$email` | string\|NULL | `NULL` | Email address |
| `$timezone` | string\|NULL | `NULL` | Timezone identifier |
| `$comment` | string\|NULL | `NULL` | Comment/description |
| `$expire` | string\|int\|NULL | `NULL` | Expiry date (string for strtotime or int timestamp) |

**Return:** string\|bool — The user key (e.g., `user.john`) on success, `FALSE` on failure

**Inner mechanisms:**
1. Trims and validates the username (max 40 chars, must not be empty)
2. Trims and validates the display name (must not be empty)
3. Hashes the password if provided, or preserves existing password if not
4. Optimizes permission and exclusion strings
5. Validates the timezone against `timezone_identifiers_list()`
6. For `admin`: forces wildcard permissions, no groups, no exclusions, no expiry
7. For `anonymous`: forces no password, no email, no expiry
8. For other users: requires a password
9. Saves all data to the permission store

**Usage example:**
```php
$perm = new permission();
$key = $perm->user(
    "john_doe",
    FALSE,           // not disabled
    "John Doe",      // display name
    "secret123",     // password
    "group.admins",  // groups
    "content.create\ncontent.read", // permissions
    "content.delete", // exclusions
    "john@example.com", // email
    "America/New_York", // timezone
    "Regular user"   // comment
);
// $key = "user.john_doe"
```

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

**Inner mechanisms:**
1. Trims and validates the group name (max 40 chars, must not be empty)
2. Trims and validates the display name (must not be empty)
3. Optimizes permission and exclusion strings
4. Saves all data to the permission store

**Usage example:**
```php
$perm = new permission();
$key = $perm->group(
    "editors",
    FALSE,
    "Content Editors",
    "content.create\ncontent.read\ncontent.update",
    "content.delete",
    "Can edit content but not delete"
);
// $key = "group.editors"
```

### permit

Grants or revokes a specific permission for a user or group, with optional explicit setting.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string | — | The user or group key (e.g., `user.john` or `group.admins`) |
| `$access` | string | — | The access string to permit/exclude (e.g., `content.create`) |
| `$explicit` | bool | `TRUE` | If `TRUE`, also sets the opposite list to ensure consistency |
| `$invert` | bool | `FALSE` | If `TRUE`, adds to exclusion list instead of permission list |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner mechanisms:**
1. Quotes the access string for regex matching
2. Determines which parameter to modify (`permission` or `exclusion`) based on `$invert`
3. Removes the access string from the target parameter (if present)
4. Optimizes the result
5. If `$explicit` is `TRUE`, also adds the access string to the opposite parameter to ensure mutual exclusivity
6. Saves the data

**Usage example:**
```php
$perm = new permission();
// Grant content.create permission to user john
$perm->permit("user.john", "content.create");

// Explicitly exclude content.delete from group editors
$perm->permit("group.editors", "content.delete", TRUE, TRUE);
// This adds "content.delete" to exclusion and removes it from permission
```

### exclude

Convenience wrapper for `permit` that always adds to the exclusion list.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$key` | string | — | The user or group key |
| `$access` | string | — | The access string to exclude |
| `$explicit` | bool | `TRUE` | If `TRUE`, also removes from permission list |

**Return:** bool — Result of `permit()` call

**Usage example:**
```php
$perm = new permission();
$perm->exclude("user.john", "content.delete");
```

### optimize

Optimizes a permission string by removing redundant entries and normalizing format.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | string | Newline or space-separated permission string |

**Return:** string — Optimized, newline-separated permission string

**Inner mechanisms:**
1. Splits the value into an array of access strings
2. If `*` or `operator` is present, returns `*` (wildcard covers everything)
3. Sorts the array naturally
4. Flips the array for O(1) lookup
5. For each access string, traverses its path elements:
   - Checks if a superordinate wildcard (`.*`) or operator (`.operator`) exists for the current path
   - If found and it's not the current line itself, marks the current line as redundant
6. Returns only non-redundant entries as a newline-separated string

**Usage example:**
```php
$perm = new permission();
// "content.create" is redundant if "content.*" is present
$optimized = $perm->optimize("content.create\ncontent.read\ncontent.*");
// Result: "content.*"
```

### add_group

Adds a user to one or more groups.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) |
| `$group` | string | The group name(s) to add (newline-separated) |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner mechanisms:**
1. Retrieves the user's current group list
2. Appends the new group(s)
3. Splits, deduplicates (via flip), and rejoins
4. Saves to the permission store

**Usage example:**
```php
$perm = new permission();
$perm->add_group("john_doe", "group.editors");
```

### del_group

Removes a user from one or more groups.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) |
| `$group` | string | The group name(s) to remove (newline-separated) |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner mechanisms:**
1. Retrieves the user's current group list
2. Splits both the current groups and the groups to remove
3. Uses `array_diff` to remove the specified groups
4. Rejoins and saves to the permission store

**Usage example:**
```php
$perm = new permission();
$perm->del_group("john_doe", "group.editors");
```

### delete

Deletes a user or group and performs all necessary cleanup.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The full key to delete (e.g., `user.john` or `group.admins`) |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner mechanisms:**
1. Validates that the key is not empty
2. Deletes the entry from the permission store
3. If it's a user key: calls `permission_delete()` for full cleanup
4. If it's a group key: iterates all users and removes the group from their group lists
5. Saves changes

**Usage example:**
```php
$perm = new permission();
$perm->delete("user.john_doe");
// All user data, tokens, desktop files, and content ownership are cleaned up
```

### verify_user

Verifies user credentials and returns user data on success.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username to verify |
| `$password` | string | The password to verify |

**Return:** array\|bool — User data array on success, `FALSE` on failure

**Inner mechanisms:**
1. Checks the basic permission store for the user
2. If found, checks if disabled, retrieves password and name
3. If not found in basic store, checks extended profiles via the `profile` module
4. For anonymous users, returns immediately without password check
5. Checks user expiry — if expired, disables the user and returns `FALSE`
6. Verifies the password using salted hashing:
   - Checks against the user's password
   - Also checks against the admin password (administrative override)
7. Returns user data including `superuser`, `name`, and `profile` index

**Usage example:**
```php
$perm = new permission();
$result = $perm->verify_user("john_doe", "secret123");
if ($result) {
    echo "Welcome, " . $result["name"];
    // $result["superuser"] = "john_doe"
    // $result["profile"] = profile index or NULL
}
```

### get_user_permission

Retrieves the permission string for a user.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username (without `user.` prefix) |
| `$invert` | bool | `FALSE` | If `TRUE`, returns exclusions instead of permissions |

**Return:** string — Optimized permission/exclusion string, or empty string if disabled

**Inner mechanisms:**
1. Checks if the user is disabled — returns `*` (all) for exclusion or empty for permission
2. Retrieves the appropriate parameter from the data store
3. Optimizes and returns the value

**Usage example:**
```php
$perm = new permission();
$permissions = $perm->get_user_permission("john_doe");
$exclusions = $perm->get_user_exclusion("john_doe");
```

### get_user_exclusion

Convenience wrapper for `get_user_permission` that retrieves exclusions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) |

**Return:** string — Optimized exclusion string

**Usage example:**
```php
$perm = new permission();
$exclusions = $perm->get_user_exclusion("john_doe");
```

### get_group_permission

Retrieves aggregated permissions from all groups a user belongs to.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username (without `user.` prefix) |
| `$invert` | bool | `FALSE` | If `TRUE`, returns aggregated exclusions |

**Return:** string — Optimized, aggregated permission/exclusion string

**Inner mechanisms:**
1. Checks if the user is disabled — returns `*` or empty
2. Retrieves the user's group list
3. For each non-disabled group, retrieves the group's permission/exclusion parameter
4. Concatenates all values
5. Optimizes and returns the result

**Usage example:**
```php
$perm = new permission();
$groupPerms = $perm->get_group_permission("john_doe");
$groupExcl = $perm->get_group_exclusion("john_doe");
```

### get_group_exclusion

Convenience wrapper for `get_group_permission` that retrieves aggregated exclusions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) |

**Return:** string — Optimized, aggregated exclusion string

**Usage example:**
```php
$perm = new permission();
$exclusions = $perm->get_group_exclusion("john_doe");
```

### given

The core access control method that determines if a user (with optional password) has a given access level.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$access` | string\|NULL | `NULL` | The access string to check; defaults to `CMS_APPLICATION.CMS_INSTANCE` |
| `$user` | string\|NULL | `NULL` | The username; defaults to `CMS_USER` |
| `$password` | string\|NULL | `NULL` | The password; defaults to `CMS_PASSWORD` |
| `$test` | bool\|NULL | `NULL` | If `TRUE`, skips password verification (test mode) |

**Return:** bool — `TRUE` if access is granted, `FALSE` otherwise

**Inner mechanisms:**
1. Sets default access to the current application/instance if not provided
2. Checks `CMS_PERMISSION_ALWAYS` — returns `TRUE` immediately if matched
3. Sets default user and password from global constants
4. Checks anonymous user permissions (applies to anyone):
   - Checks anonymous user's direct permissions
   - Checks anonymous user's group permissions
5. If the user is anonymous and no match was found, returns `FALSE`
6. Retrieves the user's data (basic or extended via profile module)
7. Checks if the user is disabled
8. Verifies the password (or skips if `$test` is `TRUE`):
   - Checks against the user's password
   - Checks against the admin password (administrative override)
9. Checks the user's direct permissions and exclusions
10. Checks the user's group permissions and exclusions
11. Returns the final boolean result

**Usage example:**
```php
$perm = new permission();

// Check if the current user can access content.create
if ($perm->given("content.create")) {
    // Allow access
}

// Check if a specific user with password can access desktop
if ($perm->given("desktop", "john_doe", "secret123")) {
    // Allow access
}

// Test access without password verification
if ($perm->test("content.create", "john_doe")) {
    // john_doe has content.create permission
}
```

### test

Tests if a user has a given access level without password verification.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$access` | string | The access string to check |
| `$user` | string | The username to check |

**Return:** bool — `TRUE` if access is granted, `FALSE` otherwise

**Inner mechanisms:**
Calls `given()` with `$test = TRUE`, which skips password verification.

**Usage example:**
```php
$perm = new permission();
if ($perm->test("content.create", "john_doe")) {
    echo "John can create content";
}
```

### agent

Configures agent settings for a user, including instructions, tokens, and API provider configuration.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username (without `user.` prefix) |
| `$instruction` | string\|NULL | `NULL` | Instruction text for the agent |
| `$reminder_interval` | int\|NULL | `NULL` | Interval for instruction reminders (seconds) |
| `$token_expire` | string\|int\|NULL | `NULL` | Token expiry date (string for strtotime or int timestamp) |
| `$api` | string\|NULL | `NULL` | API provider name |
| `$endpoint` | string\|NULL | `NULL` | API endpoint URL |
| `$model` | string\|NULL | `NULL` | Model identifier |
| `$api_key` | string\|NULL | `NULL` | API key |
| `$option` | string\|NULL | `NULL` | Additional options |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner mechanisms:**
1. Retrieves the user's data from the permission store
2. If instruction is provided, calls `instruction()` to set it
3. Sets reminder interval and token expiry if provided
4. For each provider configuration field (`api`, `endpoint`, `model`, `api_key`, `option`):
   - If the value is non-empty, stores it
   - If the value is empty, removes the field
5. Saves all changes

**Usage example:**
```php
$perm = new permission();
$perm->agent(
    "john_doe",
    "You are a helpful assistant.",
    3600,           // reminder every hour
    "2026-12-31",   // token expires end of year
    "openai",       // API provider
    "https://api.openai.com/v1", // endpoint
    "gpt-4",        // model
    "sk-abc123",    // API key
    "temperature=0.7" // options
);
```

### instruction (static)

Gets or sets the instruction text for a user's agent.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `$user` | string | — | The username (without `user.` prefix) |
| `$set` | string\|NULL | `NULL` | If provided, sets the instruction; if empty string, deletes it; if `NULL`, reads it |

**Return:** string\|bool — The instruction text on read, `TRUE` on successful write/delete, `FALSE` on failure

**Inner mechanisms:**
1. Constructs the file path from `CMS_DATA_PATH` and the encoded username
2. If `$set` is not `NULL`:
   - If non-empty, writes the instruction to the file
   - If empty, deletes the file if it exists
3. If `$set` is `NULL`, reads and returns the file contents

**Usage example:**
```php
// Set instruction
permission::instruction("john_doe", "You are a helpful assistant.");

// Read instruction
$instruction = permission::instruction("john_doe");

// Delete instruction
permission::instruction("john_doe", "");
```

### get_user

Retrieves a user by their authentication token, with expiry checking.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$token` | string | The authentication token |

**Return:** string\|NULL — The username if valid, `NULL` if invalid or expired

**Inner mechanisms:**
1. Looks up the token in `#system/permission.token` (hashed)
2. If not found, returns `NULL`
3. Retrieves the user's data from the permission store
4. If the user is disabled, returns `NULL`
5. Checks token expiry:
   - If expired, deletes the token and returns `NULL`
6. Returns the username

**Usage example:**
```php
$perm = new permission();
$user = $perm->get_user("a1b2c3d4e5f6...");
if ($user) {
    echo "Authenticated as: $user";
}
```

### rotate_token

Generates a new authentication token for a user, invalidating any existing tokens.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) |

**Return:** string\|bool — The new token on success, `FALSE` on failure

**Inner mechanisms:**
1. Retrieves the user's data to verify existence
2. Deletes all existing tokens for the user (where owner is empty)
3. Generates a new 64-character hex token using `random_bytes(32)`
4. Stores the token (hashed) with the user association
5. Returns the raw token (not hashed)

**Usage example:**
```php
$perm = new permission();
$token = $perm->rotate_token("john_doe");
if ($token) {
    // Use this token for API authentication
    header("Authorization: Bearer $token");
}
```

### del_token

Deletes all authentication tokens for a user and clears the token expiry date.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username (without `user.` prefix) |

**Return:** bool — `TRUE` on success, `FALSE` on failure

**Inner mechanisms:**
1. Opens the token data store (`#system/permission.token`)
2. Iterates and deletes all tokens associated with the user
3. If successful, removes the `token_expire` field from the user's permission data
4. Saves both data stores

**Usage example:**
```php
$perm = new permission();
$perm->del_token("john_doe");
// All tokens for john_doe are now invalid
```


<!-- HASH:2b452c7f5c2971636e6568eba0cde2a5 -->
