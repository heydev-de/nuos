# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.permission.inc)

- **Version:** `26.9.9.7`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## permission

The `permission` class is the central access control mechanism of the PWNC Web Platform. It manages users, groups, permissions, exclusions, tokens, and agent instructions. It provides methods to create, modify, and delete users and groups, assign permissions and exclusions, verify credentials, and evaluate access requests against defined rules.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Internal data store instance for `#system/permission` |

---

### Methods

#### `__construct()`

Initializes the permission system by setting up the internal data store and ensuring default users (admin, anonymous, profile, daemon) exist with appropriate settings.

**Parameters:** None  
**Returns:** `void`

**Inner Mechanisms:**
- Loads the `#system/permission` data store.
- Ensures the admin user has full permissions (`*`) and no exclusions.
- Sets default names for anonymous, profile, and daemon users using language constants.
- Generates random passwords for profile and daemon users if they exist but have no password.
- Grants the daemon user the `daemon` permission.

**Usage Example:**
```php
$permission = new permission();
// The system is now initialized with default users
```

---

#### `user($user, $disabled = NULL, $name = NULL, $password = NULL, $group = NULL, $permission = NULL, $exclusion = NULL, $email = NULL, $timezone = NULL, $comment = NULL, $expire = NULL)`

Creates or updates a user with the specified attributes.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username (max 40 chars) |
| `$disabled` | `bool\|NULL` | Whether the user is disabled |
| `$name` | `string\|NULL` | Display name |
| `$password` | `string\|NULL` | Password (will be hashed) |
| `$group` | `string\|NULL` | Newline-separated group memberships |
| `$permission` | `string\|NULL` | Newline-separated permissions |
| `$exclusion` | `string\|NULL` | Newline-separated exclusions |
| `$email` | `string\|NULL` | Email address |
| `$timezone` | `string\|NULL` | Timezone identifier |
| `$comment` | `string\|NULL` | Comment |
| `$expire` | `string\|int\|NULL` | Expiry date (string or timestamp) |

**Returns:** `string\|FALSE` — Returns `"user.$user"` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates and trims input values.
- Applies special rules for `admin` and `anonymous` users.
- Hashes passwords using `hash64()`.
- Optimizes permission and exclusion strings.
- Validates timezone against PHP's timezone list.
- Stores all data in the internal data store.

**Usage Example:**
```php
$permission = new permission();
$result = $permission->user(
    "john",
    FALSE,
    "John Doe",
    "secret123",
    "group.editors",
    "content.edit",
    "",
    "john@example.com",
    "UTC",
    "Editor account"
);
// $result === "user.john"
```

---

#### `group($group, $disabled = NULL, $name = NULL, $permission = NULL, $exclusion = NULL, $comment = NULL)`

Creates or updates a group with the specified attributes.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$group` | `string` | Group name (max 40 chars) |
| `$disabled` | `bool\|NULL` | Whether the group is disabled |
| `$name` | `string\|NULL` | Display name |
| `$permission` | `string\|NULL` | Newline-separated permissions |
| `$exclusion` | `string\|NULL` | Newline-separated exclusions |
| `$comment` | `string\|NULL` | Comment |

**Returns:** `string\|FALSE` — Returns `"group.$group"` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates and trims input values.
- Optimizes permission and exclusion strings.
- Stores all data in the internal data store.

**Usage Example:**
```php
$permission = new permission();
$result = $permission->group(
    "editors",
    FALSE,
    "Content Editors",
    "content.edit\ncontent.create",
    "",
    "Standard editor group"
);
// $result === "group.editors"
```

---

#### `permit($key, $access, $explicit = TRUE, $invert = FALSE)`

Adds or removes a permission or exclusion for a user or group.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | User or group key (e.g., `"user.john"` or `"group.editors"`) |
| `$access` | `string` | Access string to permit/exclude (e.g., `"content.edit"`) |
| `$explicit` | `bool` | If `TRUE`, also removes from the opposite list |
| `$invert` | `bool` | If `TRUE`, modifies exclusion instead of permission |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Uses regex to remove the access string from the target list.
- If `$explicit` is `TRUE`, ensures the access string is not present in the opposite list.
- Optimizes the resulting permission/exclusion string.

**Usage Example:**
```php
$permission = new permission();
$permission->permit("user.john", "content.publish");
// John can now publish content
```

---

#### `exclude($key, $access, $explicit = TRUE)`

Excludes a user or group from a specific access right.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | User or group key |
| `$access` | `string` | Access string to exclude |
| `$explicit` | `bool` | If `TRUE`, also removes from permission list |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Calls `permit()` with `$invert = TRUE`.

**Usage Example:**
```php
$permission = new permission();
$permission->exclude("user.john", "content.delete");
// John is now excluded from deleting content
```

---

#### `optimize($value)`

Optimizes a permission or exclusion string by removing redundant entries.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Newline-separated access strings |

**Returns:** `string` — Optimized newline-separated access strings.

**Inner Mechanisms:**
- Splits the input into individual access strings.
- If `*` or `operator` is present, returns `"*"`.
- Sorts entries naturally.
- Removes entries that are subsumed by wildcard entries (e.g., `content.*` makes `content.edit` redundant).

**Usage Example:**
```php
$permission = new permission();
$optimized = $permission->optimize("content.edit\ncontent.*");
// $optimized === "content.*"
```

---

#### `add_group($user, $group)`

Adds a group membership to a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$group` | `string` | Group name to add |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Retrieves current groups for the user.
- Appends the new group.
- Removes duplicates.
- Saves the updated group list.

**Usage Example:**
```php
$permission = new permission();
$permission->add_group("john", "group.editors");
// John is now a member of the editors group
```

---

#### `del_group($user, $group)`

Removes a group membership from a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$group` | `string` | Group name to remove |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Retrieves current groups for the user.
- Removes the specified group(s).
- Saves the updated group list.

**Usage Example:**
```php
$permission = new permission();
$permission->del_group("john", "group.editors");
// John is no longer a member of the editors group
```

---

#### `delete($key)`

Deletes a user or group and performs cleanup.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | User or group key to delete |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Removes the key from the data store.
- If deleting a user, calls `permission_delete()` to clean up related data.
- If deleting a group, removes the group from all users.

**Usage Example:**
```php
$permission = new permission();
$permission->delete("user.john");
// John's account and all related data are removed
```

---

#### `verify_user($user, $password)`

Verifies a user's credentials and returns user data if valid.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$password` | `string` | Password to verify |

**Returns:** `array\|FALSE` — User data array on success, `FALSE` on failure.

**Inner Mechanisms:**
- Checks if the user exists in the basic store or extended profile.
- Verifies the user is not disabled.
- Checks expiry date and disables expired users.
- Verifies the password using salted hashing.
- Returns user data including superuser, name, and profile index.

**Usage Example:**
```php
$permission = new permission();
$result = $permission->verify_user("john", "secret123");
if ($result !== FALSE) {
    echo "Welcome, " . $result["name"];
}
```

---

#### `get_user_permission($user, $invert = FALSE)`

Retrieves the permission or exclusion string for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$invert` | `bool` | If `TRUE`, returns exclusion instead of permission |

**Returns:** `string` — Optimized permission/exclusion string.

**Inner Mechanisms:**
- Returns `"*"` (or `""`) if the user is disabled.
- Retrieves the permission or exclusion value from the data store.
- Optimizes the result.

**Usage Example:**
```php
$permission = new permission();
$perms = $permission->get_user_permission("john");
// $perms might be "content.edit\ncontent.create"
```

---

#### `get_user_exclusion($user)`

Retrieves the exclusion string for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |

**Returns:** `string` — Optimized exclusion string.

**Inner Mechanisms:**
- Calls `get_user_permission()` with `$invert = TRUE`.

**Usage Example:**
```php
$permission = new permission();
$exclusions = $permission->get_user_exclusion("john");
// $exclusions might be "content.delete"
```

---

#### `get_group_permission($user, $invert = FALSE)`

Retrieves the combined permission or exclusion string from all groups a user belongs to.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$invert` | `bool` | If `TRUE`, returns exclusion instead of permission |

**Returns:** `string` — Optimized combined permission/exclusion string.

**Inner Mechanisms:**
- Returns `"*"` (or `""`) if the user is disabled.
- Retrieves all groups for the user.
- Iterates through each group, skipping disabled ones.
- Combines all permission/exclusion values.
- Optimizes the result.

**Usage Example:**
```php
$permission = new permission();
$groupPerms = $permission->get_group_permission("john");
// $groupPerms might be "content.edit\ncontent.create"
```

---

#### `get_group_exclusion($user)`

Retrieves the combined exclusion string from all groups a user belongs to.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |

**Returns:** `string` — Optimized combined exclusion string.

**Inner Mechanisms:**
- Calls `get_group_permission()` with `$invert = TRUE`.

**Usage Example:**
```php
$permission = new permission();
$groupExclusions = $permission->get_group_exclusion("john");
// $groupExclusions might be "content.delete"
```

---

#### `given($access = NULL, $user = NULL, $password = NULL, $test = NULL)`

Evaluates whether a user has access to a specific resource.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$access` | `string\|NULL` | Access string to check (defaults to application context) |
| `$user` | `string\|NULL` | Username (defaults to current user) |
| `$password` | `string\|NULL` | Password for verification |
| `$test` | `bool\|NULL` | If `TRUE`, skips password verification |

**Returns:** `bool` — `TRUE` if access is granted, `FALSE` otherwise.

**Inner Mechanisms:**
- Defaults access to the current application context.
- Checks if access is always allowed via `CMS_PERMISSION_ALWAYS`.
- Checks anonymous user permissions.
- Verifies user credentials if password is provided.
- Checks user-specific permissions and exclusions.
- Checks group-based permissions and exclusions.

**Usage Example:**
```php
$permission = new permission();
if ($permission->given("content.edit", "john", "secret123")) {
    echo "Access granted";
}
```

---

#### `test($access, $user)`

Tests whether a user has a specific access right without password verification.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$access` | `string` | Access string to check |
| `$user` | `string` | Username |

**Returns:** `bool` — `TRUE` if access is granted, `FALSE` otherwise.

**Inner Mechanisms:**
- Calls `given()` with `$test = TRUE` to skip password verification.

**Usage Example:**
```php
$permission = new permission();
if ($permission->test("content.edit", "john")) {
    echo "John can edit content";
}
```

---

#### `agent($user, $instruction = NULL, $reminder_interval = NULL, $token_expire = NULL)`

Configures agent-related settings for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$instruction` | `string\|NULL` | Instruction text for the agent |
| `$reminder_interval` | `int\|NULL` | Interval (seconds) for instruction reminders |
| `$token_expire` | `string\|int\|NULL` | Token expiry date |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Retrieves user data.
- Sets instruction text via `instruction()`.
- Updates reminder interval and token expiry if provided.
- Saves the updated user data.

**Usage Example:**
```php
$permission = new permission();
$permission->agent("john", "Process incoming requests", 3600, "+1 day");
```

---

#### `instruction($user, $set = NULL)`

Manages instruction text for a user's agent.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$set` | `string\|NULL` | Instruction text to set, or `NULL` to read |

**Returns:** `string\|bool` — Instruction text when reading, `TRUE`/`FALSE` on write/delete.

**Inner Mechanisms:**
- Constructs a file path based on the username.
- If `$set` is provided, writes or deletes the instruction file.
- If `$set` is `NULL`, reads the instruction file.

**Usage Example:**
```php
$permission = new permission();
$permission->instruction("john", "Monitor system logs");
$text = $permission->instruction("john"); // Returns "Monitor system logs"
```

---

#### `get_user($token)`

Retrieves a user by their authentication token.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$token` | `string` | Authentication token |

**Returns:** `string\|NULL` — Username if valid, `NULL` otherwise.

**Inner Mechanisms:**
- Looks up the token in the token map.
- Retrieves user data.
- Checks if the user is disabled.
- Checks token expiry and deletes expired tokens.

**Usage Example:**
```php
$permission = new permission();
$user = $permission->get_user($authToken);
if ($user !== NULL) {
    echo "Authenticated user: $user";
}
```

---

#### `rotate_token($user)`

Generates and stores a new authentication token for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |

**Returns:** `string\|FALSE` — New token on success, `FALSE` on failure.

**Inner Mechanisms:**
- Retrieves user data.
- Generates a random 64-character hex token.
- Stores the token in the token map.
- Returns the new token.

**Usage Example:**
```php
$permission = new permission();
$newToken = $permission->rotate_token("john");
// Use $newToken for subsequent authenticated requests
```

---

#### `del_token($user)`

Deletes the authentication token for a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |

**Returns:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Removes the token from the token map.
- Removes the token expiry date from user data.

**Usage Example:**
```php
$permission = new permission();
$permission->del_token("john");
// John's token is now invalid
```

---

## Related Functions

### `permission_delete($user)`

Removes all data associated with a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username to delete |

**Returns:** `void`

**Inner Mechanisms:**
- Removes the agent instruction file.
- Removes the agent token from the token map.
- Removes IMS (Instant Messaging System) data owned by the user.
- Removes desktop data directory.
- Changes content ownership to administrator.

**Usage Example:**
```php
permission_delete("john");
// All data for user "john" is removed
```

---

### `permission_match($access, $permission, $exclusion)`

Evaluates whether an access request matches the given permissions and exclusions.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$access` | `string` | Access string to check (e.g., `"content.edit"`) |
| `$permission` | `string` | Newline-separated permissions |
| `$exclusion` | `string` | Newline-separated exclusions |

**Returns:** `bool\|NULL` — `TRUE` if permitted, `FALSE` if excluded, `NULL` if undetermined.

**Inner Mechanisms:**
- Checks if access is always allowed via `CMS_PERMISSION_ALWAYS`.
- Returns `NULL` if no permissions are defined.
- Splits access string into parts.
- Checks global and specific exclusions.
- Checks global and specific permissions.
- Iterates through access string parts to check superordinate exclusions and permissions.

**Usage Example:**
```php
$result = permission_match("content.edit", "content.*\ncontent.create", "content.delete");
// $result === TRUE (permitted via content.*)
```

---

### `permission_merge($value1, $value2)`

Merges two permission/exclusion strings and removes duplicates.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | First permission/exclusion string |
| `$value2` | `string` | Second permission/exclusion string |

**Returns:** `string` — Merged newline-separated string.

**Inner Mechanisms:**
- Splits both inputs by whitespace.
- Flips to remove duplicates.
- Rejoins into a newline-separated string.

**Usage Example:**
```php
$merged = permission_merge("content.edit\ncontent.create", "content.delete\ncontent.edit");
// $merged === "content.edit\ncontent.create\ncontent.delete"
```

---

### `permission_is_user($key)`

Checks if a key represents a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Key to check |

**Returns:** `bool` — `TRUE` if the key starts with `"user."`.

**Usage Example:**
```php
if (permission_is_user("user.john")) {
    echo "This is a user key";
}
```

---

### `permission_is_group($key)`

Checks if a key represents a group.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Key to check |

**Returns:** `bool` — `TRUE` if the key starts with `"group."`.

**Usage Example:**
```php
if (permission_is_group("group.editors")) {
    echo "This is a group key";
}
```

---

### `permission_remove_prefix($key)`

Removes the `user.` or `group.` prefix from a key.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Key to process |

**Returns:** `string` — Key without prefix.

**Usage Example:**
```php
$name = permission_remove_prefix("user.john");
// $name === "john"
```

---

### `permission_get_name($user, $email = FALSE)`

Retrieves the display name or email of a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$email` | `bool` | If `TRUE`, returns email instead of name |

**Returns:** `string` — User's name or email.

**Inner Mechanisms:**
- Uses a static cache to avoid repeated lookups.
- Checks the basic permission store first.
- Falls back to the profile module for extended users.
- Returns default values for unknown users.

**Usage Example:**
```php
$name = permission_get_name("john");
// $name === "John Doe"
```

---

### `permission_get_email($user)`

Retrieves the email address of a user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |

**Returns:** `string\|NULL` — User's email or `NULL`.

**Inner Mechanisms:**
- Calls `permission_get_name()` with `$email = TRUE`.

**Usage Example:**
```php
$email = permission_get_email("john");
// $email === "john@example.com"
```

---

### `permission_get_group($user, $data = NULL)`

Retrieves all groups a user belongs to with their names.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username |
| `$data` | `data\|NULL` | Optional data store instance |

**Returns:** `array` — Associative array of group keys to names.

**Inner Mechanisms:**
- Retrieves the user's group list.
- Looks up each group's name.
- Returns an associative array.

**Usage Example:**
```php
$groups = permission_get_group("john");
// $groups === ["group.editors" => "Content Editors", ...]
```

---

### `permission_get_member($group, $data = NULL)`

Retrieves all users that belong to a specific group.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$group` | `string` | Group name |
| `$data` | `data\|NULL` | Optional data store instance |

**Returns:** `array` — Associative array of user keys to names.

**Inner Mechanisms:**
- Iterates through all users in the data store.
- Checks if each user belongs to the specified group.
- Returns an associative array of matching users.

**Usage Example:**
```php
$members = permission_get_member("editors");
// $members === ["user.john" => "John Doe", ...]
```


<!-- HASH:153fdf0bffcf0d106ef94f225e40d5e8 -->
