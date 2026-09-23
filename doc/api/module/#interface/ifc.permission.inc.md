# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.permission.inc)

- **Version:** `26.9.21.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Interface Controller

The `ifc.permission.inc` file is a PWNC Web Platform interface controller responsible for managing user and group permissions within the CMS. It handles all CRUD operations for users and groups, permission assignment and exclusion, activation/deactivation, deletion, and token rotation for API access.

This controller operates through a message-based system where `CMS_IFC_MESSAGE` determines the action to perform. It uses the `permission` class for data manipulation and the `ifc` class for form rendering.

### Key Components

| Component | Description |
|-----------|-------------|
| `permission` class | Core permission management (users, groups, permissions) |
| `ifc` class | Form/interface rendering |
| `CMS_IFC_MESSAGE` | Determines which action to execute |
| `$object` | Currently selected user or group identifier |
| `$type` | Either "user" or "group" |
| `$ifc_param1-19` | Form parameters passed from the interface |

## Message Handling

### select

Sets the currently selected object based on the interface parameter.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | The object identifier to select |

**Usage:**
```php
// Selects a user or group for editing
// Triggered when clicking on a user/group in the list
```

### select_user

Clears the current selection and sets the type to "user".

**Usage:**
```php
// Switches the interface to user management mode
// Clears any previously selected object
```

### select_group

Clears the current selection and sets the type to "group".

**Usage:**
```php
// Switches the interface to group management mode
// Clears any previously selected object
```

### _add_user

Creates a new user after validating input.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | User's full name |
| `$ifc_param2` | string | Username (user ID) |
| `$ifc_param3` | string | Password |
| `$ifc_param4` | string | Password confirmation |

**Validation:**
- Passwords must match
- Username must not already exist

**Usage:**
```php
// Creates a new user with the given credentials
// User is created in disabled state initially
$permission = new permission();
$_object = $permission->user("newuser", TRUE, "John Doe", "password123");
```

### add_user

Displays the user creation form.

**Usage:**
```php
// Renders the "Add User" form with fields for:
// - Name
// - Username
// - Password
// - Password confirmation
```

### _add_group

Creates a new group after validating input.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Group's full name |
| `$ifc_param2` | string | Group ID |

**Validation:**
- Group ID must not already exist

**Usage:**
```php
// Creates a new group
$permission = new permission();
$_object = $permission->group("editors", TRUE, "Content Editors");
```

### add_group

Displays the group creation form.

**Usage:**
```php
// Renders the "Add Group" form with fields for:
// - Name
// - Group ID
```

### save_user

Updates an existing user's properties.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | User's full name |
| `$ifc_param2` | string | Username (user ID) |
| `$ifc_param3` | string | New password (optional) |
| `$ifc_param4` | string | Password confirmation |
| `$ifc_param5` | array | Group assignments |
| `$ifc_param6` | string | Permissions |
| `$ifc_param7` | string | Exclusions |
| `$ifc_param8` | string | Comment |
| `$ifc_param9` | string | Email |
| `$ifc_param10` | string | Timezone |
| `$ifc_param11` | string | Expiry date |
| `$ifc_param12` | string | Agent instruction |
| `$ifc_param13` | string | Reminder interval |
| `$ifc_param14` | string | Token expiry |
| `$ifc_param15` | string | API provider |
| `$ifc_param16` | string | API endpoint |
| `$ifc_param17` | string | API model |
| `$ifc_param18` | string | API key |
| `$ifc_param19` | string | API options |

**Usage:**
```php
// Updates user properties including:
// - Name, password, groups
// - Permissions and exclusions
// - Email, timezone, expiry
// - API agent settings
```

### token_rotate

Generates a new API token for a user.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username to rotate token for |

**Usage:**
```php
// Rotates the API token for a user
// Displays the new token in a form
$permission = new permission();
$token = $permission->rotate_token("john_doe");
```

### save_group

Updates an existing group's properties.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Group's full name |
| `$ifc_param2` | string | Group ID |
| `$ifc_param3` | array | User assignments |
| `$ifc_param4` | string | Permissions |
| `$ifc_param5` | string | Exclusions |
| `$ifc_param6` | string | Comment |

**Usage:**
```php
// Updates group properties and manages user assignments
// Users in $ifc_param3 are added to the group
// Users not in $ifc_param3 are removed from the group
```

### activate

Activates (enables) selected users or groups.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | array | Array of object identifiers to activate |

**Usage:**
```php
// Removes the "disabled" flag from selected objects
foreach ($list AS $value) {
    $permission->data->del($value, "disabled");
}
```

### deactivate

Deactivates (disables) selected users or groups.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | array | Array of object identifiers to deactivate |

**Usage:**
```php
// Sets the "disabled" flag on selected objects
foreach ($list AS $value) {
    $permission->data->set(TRUE, $value, "disabled");
}
```

### delete

Deletes selected users or groups.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | array | Array of object identifiers to delete |

**Usage:**
```php
// Permanently removes users or groups
// Groups are automatically removed from users
foreach ($list AS $value) {
    $permission->delete($value);
}
```

### set / _set / set_add / set_add_ex / set_del / set_del_ex

Manages permission assignments and exclusions for users and groups.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Access level/permission to assign |
| `$ifc_param2` | array | Users/groups to permit |
| `$ifc_param3` | array | Users/groups to exclude |

**Sub-actions:**
- `set_add`: Grant permission to selected users/groups
- `set_add_ex`: Grant explicit permission
- `set_del`: Revoke permission from selected users/groups
- `set_del_ex`: Revoke explicit permission

**Usage:**
```php
// Grants or revokes permissions for users and groups
// Builds a visual interface showing permitted vs excluded entities
$permission->permit("user.john", "cms_access", TRUE);  // Explicit permit
$permission->exclude("group.editors", "admin_access"); // Exclude
```

## Main Display

The main display section renders the complete permission management interface:

1. **Menu Bar**: Create user, create group, enable, disable, delete, permission management
2. **Type Selection**: Toggle between user and group views
3. **Object List**: Alphabetically sorted list of users/groups with:
   - Selection checkboxes
   - Icons and names
   - Comments/descriptions
4. **Properties Panel**: Context-sensitive editing panel that appears when an object is selected

### Group Properties

When a group is selected, the properties panel shows:
- Group ID and name
- Disabled flag
- User membership management
- Permission and exclusion text areas
- Comment field

### User Properties

When a user is selected, the properties panel shows:
- Username and name
- Disabled flag
- Password fields
- Group membership
- Permission and exclusion text areas
- Email and timezone settings
- Expiry date
- API agent settings (instruction, reminder, token expiry)
- API provider configuration (api, endpoint, model, key, options)

## Helper Functions

### permission_remove_prefix

Removes the "user." or "group." prefix from an object identifier.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | The object identifier |

**Returns:** string - The identifier without prefix

### permission_is_user

Checks if an object identifier represents a user.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The object identifier |

**Returns:** bool - TRUE if user, FALSE otherwise

### permission_is_group

Checks if an object identifier represents a group.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | string | The object identifier |

**Returns:** bool - TRUE if group, FALSE otherwise

### permission_match

Checks if a permission string matches given permissions and exclusions.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$access` | string | The access level to check |
| `$permission` | string | The permission string |
| `$exclusion` | string | The exclusion string |

**Returns:** bool|null - TRUE if explicitly permitted, FALSE if explicitly excluded, NULL if implicit

### permission_get_name

Retrieves the display name for a user.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | The username |

**Returns:** string - The user's display name


<!-- HASH:8ec38a7df38ff5cfa19f362e6c07c16a -->
