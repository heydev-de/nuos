# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.permission.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Interface Module

This file implements the administrative interface for managing **users and groups** within the PWNC permission system. It provides a full CRUD (Create, Read, Update, Delete) interface for user and group management, including:

- Creating new users and groups
- Editing existing user/group properties (name, permissions, exclusions, comments, etc.)
- Activating/deactivating (enabling/disabling) users and groups
- Deleting users and groups
- Managing access permissions through explicit permit/exclude operations
- Rotating API tokens for users
- Displaying hierarchical permission structures with visual indicators

The interface uses the PWNC `ifc` (interface controller) system for form rendering and message handling, and interacts with the `permission` class for data operations.

### Key Concepts

| Concept | Description |
|---------|-------------|
| `$object` | The currently selected user or group identifier (e.g., `user.admin`, `group.editors`) |
| `$type` | The current view type: `"user"` or `"group"` |
| `$list` | Array of selected items from checkboxes for bulk operations |
| `$ifc_param1`–`$ifc_param14` | Form field values submitted via the interface |
| `CMS_IFC_MESSAGE` | The current action/message being processed (e.g., `add_user`, `save_group`) |

---

## Message Handling

### `select`
Sets the currently selected object based on `$ifc_param`.

### `select_user`
Switches the view to users and clears any selected object if the type changes.

### `select_group`
Switches the view to groups and clears any selected object if the type changes.

### `_add_user`
Handles the backend logic for creating a new user:

1. Validates that passwords match
2. Checks if the username already exists
3. Creates the user via `permission->user()` with disabled flag set to `TRUE`
4. Sets success/error response

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | User's full name |
| `$ifc_param2` | string | Username (user ID) |
| `$ifc_param3` | string | Password |
| `$ifc_param4` | string | Password confirmation |

### `add_user`
Renders the "Add User" form with fields for name, username, password, and password confirmation.

### `_add_group`
Handles the backend logic for creating a new group:

1. Checks if the group already exists
2. Creates the group via `permission->group()` with disabled flag set to `TRUE`
3. Sets success/error response

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Group's full name |
| `$ifc_param2` | string | Group ID |

### `add_group`
Renders the "Add Group" form with fields for name and group ID.

### `save_user`
Handles saving modifications to an existing user:

1. Validates password match if a new password is provided
2. Updates user properties via `permission->user()`
3. Updates agent settings (instructions, reminders, token expiry) via `permission->agent()`
4. Sets success/error response

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Name |
| `$ifc_param2` | string | Username |
| `$ifc_param3` | string | Password |
| `$ifc_param4` | string | Password confirmation |
| `$ifc_param5` | array | Groups (newline-separated) |
| `$ifc_param6` | string | Permissions |
| `$ifc_param7` | string | Exclusions |
| `$ifc_param8` | string | Comment |
| `$ifc_param9` | string | Email |
| `$ifc_param10` | string | Timezone |
| `$ifc_param11` | string | Expiry date |
| `$ifc_param12` | string | Agent instruction |
| `$ifc_param13` | string | Reminder interval |
| `$ifc_param14` | string | Token expiry |

### `token_rotate`
Rotates the API token for a specified user and displays the new token.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$user` | string | Username to rotate token for |

### `save_group`
Handles saving modifications to an existing group:

1. Updates group properties via `permission->group()`
2. Synchronizes group memberships for all users
3. Sets success/error response

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Name |
| `$ifc_param2` | string | Group ID |
| `$ifc_param3` | array | Assigned users |
| `$ifc_param4` | string | Permissions |
| `$ifc_param5` | string | Exclusions |
| `$ifc_param6` | string | Comment |

### `activate`
Enables (removes disabled flag from) all selected users/groups.

### `deactivate`
Disables (sets disabled flag on) all selected users/groups.

### `delete`
Deletes all selected users/groups. Groups are automatically removed from users.

### `set` / `_set` / `set_add` / `set_add_ex` / `set_del` / `set_del_ex`
Manages access permissions for selected users/groups:

- `set_add`: Explicitly permit access
- `set_add_ex`: Explicitly permit access (with explicit flag)
- `set_del`: Exclude (deny) access
- `set_del_ex`: Exclude (deny) access (with explicit flag)

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Access path/identifier |
| `$ifc_param2` | array | Selected groups |
| `$ifc_param3` | array | Selected users |

The interface also displays a visual representation of all users and groups with their permission status:
- **⁺** = Explicitly permitted
- **⁻** = Explicitly excluded
- No marker = Implicitly permitted or denied based on group membership

---

## Main Display

The main display renders a two-column layout:

1. **Left column**: List of all users or groups (based on `$type`) with:
   - Alphabetical grouping by first letter
   - Checkboxes for selection
   - Icons indicating disabled status
   - Inline editing links

2. **Right column** (when an object is selected): Properties panel with tabs for:
   - **Basic**: Name, disabled flag, save button
   - **Access**: Permissions and exclusions textareas
   - **Extended**: Comment, email, timezone, expiry date
   - **Agent** (users only): Instructions, reminder interval, API token management

### Usage Example

```php
// The interface is automatically loaded when navigating to the permission module
// No direct instantiation required - handled by PWNC's module system

// Typical URL pattern:
// /permission/
// /permission/?ifc_message=add_user
// /permission/?ifc_message=select&ifc_param=user.admin
```

The interface integrates with PWNC's standard form processing pipeline, where:
1. User actions trigger `ifc_post()` calls
2. Messages are routed to appropriate handlers (`_add_user`, `save_group`, etc.)
3. Data is persisted via the `permission` class
4. Responses are rendered using the `ifc` interface controller


<!-- HASH:fc37b399eeb9bbfaeae73d4d56c8fd17 -->
