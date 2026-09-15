# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.permission.inc)

- **Version:** `26.9.14.11`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Interface Controller

The `ifc.permission.inc` file is a PWNC Web Platform interface controller responsible for managing user and group permissions within the CMS. It handles the full lifecycle of permission management including:

- **User and Group Creation**: Adding new users and groups with configurable properties
- **Permission Assignment**: Granting or denying access permissions to users and groups
- **Object Management**: Activating, deactivating, and deleting users/groups
- **Token Management**: Rotating API tokens for users
- **UI Rendering**: Displaying interactive forms and tables for permission management

The controller operates through a message-based system where `CMS_IFC_MESSAGE` determines the current action, and uses the `permission` class for data operations and the `ifc` class for form rendering.

### Key Components

| Component | Description |
|-----------|-------------|
| `permission` class | Core data model for user/group operations |
| `ifc` class | Form and interface rendering engine |
| `CMS_IFC_MESSAGE` | Current action/message identifier |
| `$object` | Currently selected user or group identifier |
| `$type` | Current selection type ("user" or "group") |

## Message Handling

### select

Sets the currently selected object based on the interface parameter.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | string | The object identifier to select |

**Usage:**
```php
// Selects a user or group for editing
// Triggered when user clicks on an item in the list
```

### select_user

Clears the current selection and sets the type to "user".

**Usage:**
```php
// Switches the interface to user selection mode
// Clears any previously selected object
```

### select_group

Clears the current selection and sets the type to "group".

**Usage:**
```php
// Switches the interface to group selection mode
// Clears any previously selected object
```

### _add_user

Handles the backend logic for creating a new user.

**Process:**
1. Validates that passwords match
2. Checks if username already exists
3. Creates the user with disabled flag set
4. Sets response message based on success/failure

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | User's full name |
| `$ifc_param2` | string | Username (user ID) |
| `$ifc_param3` | string | Password |
| `$ifc_param4` | string | Password confirmation |

**Usage:**
```php
// Called internally when the add_user form is submitted
// Creates a new disabled user account
```

### add_user

Renders the form for adding a new user.

**Form Fields:**
- Name (text input)
- Username (text input)
- Password (password input)
- Password confirmation (password input)

**Usage:**
```php
// Displays the user creation form
// Shows validation errors if any
```

### _add_group

Handles the backend logic for creating a new group.

**Process:**
1. Checks if group already exists
2. Creates the group with disabled flag set
3. Sets response message based on success/failure

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | Group's full name |
| `$ifc_param2` | string | Group identifier |

**Usage:**
```php
// Called internally when the add_group form is submitted
// Creates a new disabled group
```

### add_group

Renders the form for adding a new group.

**Form Fields:**
- Name (text input)
- Group ID (text input)

**Usage:**
```php
// Displays the group creation form
// Shows validation errors if any
```

### save_user

Handles saving modifications to an existing user.

**Process:**
1. Validates password if provided
2. Updates user properties (name, disabled state, password, groups, permissions)
3. Updates agent configuration (API settings, tokens)
4. Sets response message based on success/failure

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | User identifier |
| `$ifc_param1` | string | Full name |
| `$ifc_param2` | string | Disabled flag |
| `$ifc_param3` | string | Password |
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
| `$ifc_param15` | string | API type |
| `$ifc_param16` | string | API endpoint |
| `$ifc_param17` | string | API model |
| `$ifc_param18` | string | API key |
| `$ifc_param19` | string | API options |

**Usage:**
```php
// Called when the save_user button is clicked
// Updates all user properties and agent configuration
```

### token_rotate

Rotates the API token for a user.

**Process:**
1. Validates user exists
2. Generates new token
3. Displays token in a form for copying

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$user` | string | Username to rotate token for |

**Usage:**
```php
// Called when the "Rotate API Key" button is clicked
// Displays the new token for the user to copy
```

### save_group

Handles saving modifications to an existing group.

**Process:**
1. Updates group properties (name, disabled state, permissions)
2. Updates group membership for all users
3. Sets response message based on success/failure

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | Group identifier |
| `$ifc_param1` | string | Full name |
| `$ifc_param2` | string | Disabled flag |
| `$ifc_param3` | array | User assignments |
| `$ifc_param4` | string | Permissions |
| `$ifc_param5` | string | Exclusions |
| `$ifc_param6` | string | Comment |

**Usage:**
```php
// Called when the save_group button is clicked
// Updates group properties and user memberships
```

### activate

Activates (enables) selected users or groups.

**Process:**
1. Removes disabled flags from selected objects
2. Saves changes to data store

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | array | Array of object identifiers to activate |

**Usage:**
```php
// Called when the "Enable" button is clicked
// Enables multiple selected users or groups
```

### deactivate

Deactivates (disables) selected users or groups.

**Process:**
1. Sets disabled flags on selected objects
2. Saves changes to data store

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | array | Array of object identifiers to deactivate |

**Usage:**
```php
// Called when the "Disable" button is clicked
// Disables multiple selected users or groups
```

### delete

Deletes selected users or groups.

**Process:**
1. Deletes each selected object
2. Deselects current object if deleted
3. Saves changes to data store

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | array | Array of object identifiers to delete |

**Usage:**
```php
// Called when the "Delete Selected" button is clicked
// Permanently removes users or groups
```

### set

Initializes the permission setting interface.

**Process:**
1. Retrieves or caches the access level
2. Falls through to `_set` for rendering

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$access` | string | Access level to set permissions for |

**Usage:**
```php
// Entry point for the permission assignment interface
// Caches the access level for subsequent operations
```

### _set, set_add, set_add_ex, set_del, set_del_ex

Handles permission assignment and exclusion operations.

**Process:**
1. Validates access level
2. Permits or excludes selected users/groups
3. Builds lists of permitted and excluded objects
4. Renders the permission assignment interface

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | Access level |
| `$ifc_param2` | array | Selected users/groups for permission |
| `$ifc_param3` | array | Selected users/groups for exclusion |

**Usage:**
```php
// Called when assigning or removing permissions
// Renders the dual-list interface for permission management
```

## Main Display

The main display section renders the primary permission management interface with:

1. **Menu Bar**: Buttons for creating users/groups, enabling/disabling, deleting, and setting permissions
2. **Type Selection**: Toggle between user and group views
3. **Object List**: Alphabetically sorted list of users or groups with selection checkboxes
4. **Properties Panel**: Detailed editing form for the selected object

### Object List Features

- Alphabetical grouping with initial letter separators
- Icons indicating disabled state
- Click-to-select functionality
- Bulk selection controls (select all, invert, clear)

### Properties Panel

For **groups**, displays:
- Group ID and name
- Disabled flag
- User membership list
- Permission and exclusion text areas
- Comment field

For **users**, displays:
- Username and name
- Disabled flag
- Password fields
- Group membership list
- Permission and exclusion text areas
- Email and timezone settings
- Expiry date
- Agent configuration (API settings, token management)
- Comment field

**Usage Example:**
```php
// The entire interface is rendered automatically based on CMS_IFC_MESSAGE
// Developers interact with it through the permission class methods
$permission = new permission();
$user = $permission->user("john_doe", false, "John Doe", "password123");
```


<!-- HASH:49c7f4caec98f2cacd248475c6fddbf4 -->
