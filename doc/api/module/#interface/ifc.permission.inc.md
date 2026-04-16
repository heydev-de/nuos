# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.permission.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Permission Interface Module (`ifc.permission.inc`)

This file implements the user and group permission management interface for the NUOS web platform. It provides a complete UI for creating, modifying, deleting, and assigning permissions to users and groups, including access control management.

---

### Overview

The interface handles:
- **User Management**: Creation, modification, and deletion of user accounts.
- **Group Management**: Creation, modification, and deletion of groups.
- **Permission Assignment**: Granting or revoking access permissions to users and groups.
- **State Management**: Selection, activation, deactivation, and display of users/groups.

The interface responds to various messages (commands) sent via `CMS_IFC_MESSAGE` to perform specific actions, such as adding a user, saving changes, or setting permissions.

---

## Constants and Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$object` | `NULL` | Currently selected user or group (e.g., `"user.john"`, `"group.admins"`). |
| `$type` | `"user"` | Type of object being managed: `"user"` or `"group"`. |
| `$list` | `NULL` | Array of selected objects (used in bulk operations like activate/deactivate/delete). |
| `$ifc_param` | Varies | Input parameters passed to the interface (e.g., user details, group assignments). |
| `$ifc_response` | `NULL` | Response message returned to the caller (e.g., success/error messages). |

---

## Interface Message Handlers

The interface processes messages via a `switch` on `CMS_IFC_MESSAGE`. Each case corresponds to a specific action.

---

### `case "select"`

**Purpose**:
Selects an object (user or group) for editing.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | The object identifier (e.g., `"user.john"`). |

**Return Values**:
None (updates `$object`).

**Inner Mechanisms**:
- Sets `$object` to the value of `$ifc_param`.

**Usage Context**:
Triggered when a user or group is clicked in the list. Updates the interface to display the selected object's properties.

---

### `case "select_user"`

**Purpose**:
Switches the interface to display users only.

**Parameters**:
None.

**Return Values**:
None (updates `$type` and clears `$object`).

**Inner Mechanisms**:
- Sets `$type` to `"user"`.
- Clears `$object` if the current type is not `"user"`.

**Usage Context**:
Triggered when the "Users" button is clicked. Resets the interface to show the user list.

---

### `case "select_group"`

**Purpose**:
Switches the interface to display groups only.

**Parameters**:
None.

**Return Values**:
None (updates `$type` and clears `$object`).

**Inner Mechanisms**:
- Sets `$type` to `"group"`.
- Clears `$object` if the current type is not `"group"`.

**Usage Context**:
Triggered when the "Groups" button is clicked. Resets the interface to show the group list.

---

### `case "add_user"`

**Purpose**:
Displays a form to add a new user.

**Parameters**:
None.

**Return Values**:
None (renders an `ifc` form).

**Inner Mechanisms**:
- Creates a new `ifc` (interface control) object with fields for:
  - Name (`CMS_L_NAME`)
  - User ID (`CMS_L_IFC_PERMISSION_002`)
  - Password (`CMS_L_PASSWORD`)
  - Password confirmation (`CMS_L_IFC_PERMISSION_003`)

**Usage Context**:
Triggered when the "Create User" button is clicked. Displays a modal form for user creation.

---

### `case "_add_user"`

**Purpose**:
Processes the submission of the "add user" form.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | User's display name. |
| `$ifc_param2` | `string` | User ID. |
| `$ifc_param3` | `string` | Password. |
| `$ifc_param4` | `string` | Password confirmation. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- Validates that the password matches the confirmation.
- Checks if the user ID already exists.
- Creates a new user using the `permission` class.
- Saves changes and updates `$object` and `$type` on success.

**Usage Context**:
Triggered when the "add user" form is submitted. Creates a new user if validation passes.

---

### `case "add_group"`

**Purpose**:
Displays a form to add a new group.

**Parameters**:
None.

**Return Values**:
None (renders an `ifc` form).

**Inner Mechanisms**:
- Creates a new `ifc` object with fields for:
  - Name (`CMS_L_NAME`)
  - Group ID (`CMS_L_IFC_PERMISSION_005`)

**Usage Context**:
Triggered when the "Create Group" button is clicked. Displays a modal form for group creation.

---

### `case "_add_group"`

**Purpose**:
Processes the submission of the "add group" form.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Group's display name. |
| `$ifc_param2` | `string` | Group ID. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- Checks if the group ID already exists.
- Creates a new group using the `permission` class.
- Saves changes and updates `$object` and `$type` on success.

**Usage Context**:
Triggered when the "add group" form is submitted. Creates a new group if validation passes.

---

### `case "save_user"`

**Purpose**:
Saves changes to a user's properties.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | User's display name. |
| `$ifc_param2` | `bool` | Disabled flag. |
| `$ifc_param3` | `string` | New password. |
| `$ifc_param4` | `string` | Password confirmation. |
| `$ifc_param5` | `array` | Array of group assignments. |
| `$ifc_param6` | `string` | Permissions. |
| `$ifc_param7` | `string` | Exclusions. |
| `$ifc_param8` | `string` | Email. |
| `$ifc_param9` | `string` | Timezone. |
| `$ifc_param10` | `string` | Comment. |
| `$ifc_param11` | `int` | Expiration in seconds (converted from days). |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` (with optional password message) on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- Validates password match (if provided).
- Converts group assignments from array to newline-separated string.
- Updates user properties using the `permission` class.
- Saves changes to the data store.

**Usage Context**:
Triggered when the "Save" button is clicked on a user's property form.

---

### `case "save_group"`

**Purpose**:
Saves changes to a group's properties.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Group's display name. |
| `$ifc_param2` | `bool` | Disabled flag. |
| `$ifc_param3` | `array` | Array of user assignments. |
| `$ifc_param4` | `string` | Permissions. |
| `$ifc_param5` | `string` | Exclusions. |
| `$ifc_param6` | `string` | Comment. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- Updates group properties using the `permission` class.
- Iterates through all users and updates their group assignments based on `$ifc_param3`.
- Saves changes to the data store.

**Usage Context**:
Triggered when the "Save" button is clicked on a group's property form.

---

### `case "activate"`

**Purpose**:
Activates selected users or groups (removes the `disabled` flag).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of object identifiers to activate. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- Iterates through `$list` and removes the `disabled` flag for each object.
- Saves changes to the data store.

**Usage Context**:
Triggered when the "Enable" button is clicked after selecting objects.

---

### `case "deactivate"`

**Purpose**:
Deactivates selected users or groups (sets the `disabled` flag).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of object identifiers to deactivate. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- Iterates through `$list` and sets the `disabled` flag for each object.
- Saves changes to the data store.

**Usage Context**:
Triggered when the "Disable" button is clicked after selecting objects.

---

### `case "delete"`

**Purpose**:
Deletes selected users or groups.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of object identifiers to delete. |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- Iterates through `$list` and deletes each object using the `permission` class.
- Clears `$object` if the deleted object was currently selected.
- Saves changes to the data store.

**Usage Context**:
Triggered when the "Delete" button is clicked after selecting objects.

---

### `case "set"`, `"_set"`, `"set_add"`, `"set_add_ex"`, `"set_del"`, `"set_del_ex"`

**Purpose**:
Manages permission assignments for users and groups.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Access permission string (e.g., `"module.access"`). |
| `$ifc_param2` | `array` | Array of users/groups to exclude (for `set_del`/`set_del_ex`). |
| `$ifc_param3` | `array` | Array of users/groups to permit (for `set_add`/`set_add_ex`). |

**Return Values**:
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms**:
- **`set_add`/`set_add_ex`**: Grants permission to selected users/groups. `set_add_ex` marks the permission as explicit.
- **`set_del`/`set_del_ex`**: Revokes permission from selected users/groups. `set_del_ex` removes explicit permissions.
- Caches the last accessed permission for the current user.
- Generates a UI to display permitted and excluded users/groups, with controls to move objects between lists.

**Usage Context**:
Triggered when managing permissions via the "Permission" button or explicit permit/exclude controls.

---

## Main Display Logic

### Overview

The main display logic renders:
1. A list of users or groups (based on `$type`).
2. A property form for the selected object (if any).
3. A menu for actions (e.g., create user/group, enable/disable, delete, set permissions).

### Key Components

#### Type Selection
- Buttons to switch between users and groups (`select_user`, `select_group`).

#### Object List
- Displays a sortable list of users or groups with:
  - Selection checkboxes.
  - Icons indicating disabled status.
  - Names and comments.

#### Property Form
- **User Properties**:
  - Name, disabled flag, password, group assignments, permissions, exclusions, email, timezone, comment, expiration.
- **Group Properties**:
  - Name, disabled flag, user assignments, permissions, exclusions, comment.

#### Menu
- Actions for creating, enabling, disabling, deleting, and setting permissions.

---

## Helper Functions and Utilities

### `permission_is_user($key)`
**Purpose**:
Checks if a key represents a user (e.g., `"user.john"`).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Object key to check. |

**Return Values**:
- `bool`: `TRUE` if the key represents a user, `FALSE` otherwise.

---

### `permission_is_group($key)`
**Purpose**:
Checks if a key represents a group (e.g., `"group.admins"`).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$key` | `string` | Object key to check. |

**Return Values**:
- `bool`: `TRUE` if the key represents a group, `FALSE` otherwise.

---

### `permission_match($access, $permission, $exclusion)`
**Purpose**:
Determines if a given access string matches the permission/exclusion rules.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$access` | `string` | Access string to check (e.g., `"module.access"`). |
| `$permission` | `string` | Permission string (newline-separated). |
| `$exclusion` | `string` | Exclusion string (newline-separated). |

**Return Values**:
- `bool`: `TRUE` if access is granted, `FALSE` otherwise.

**Inner Mechanisms**:
- Checks if `$access` is explicitly permitted or not excluded.

---

## Usage Examples

### Creating a User
1. Click "Create User" in the menu.
2. Fill in the name, user ID, and password (with confirmation).
3. Submit the form. The user is created and selected for editing.

### Setting Permissions
1. Select users/groups from the list.
2. Click "Permission" in the menu.
3. Enter the access string (e.g., `"module.access"`).
4. Use the permit/exclude controls to assign permissions.
5. Click "Save" to apply changes.

### Deleting a Group
1. Select one or more groups from the list.
2. Click "Delete Selected" in the menu.
3. Confirm the action. The groups are deleted.


<!-- HASH:f872298c8f42fb72fb64b352bee790cd -->
