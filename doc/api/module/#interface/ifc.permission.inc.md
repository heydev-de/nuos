# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.permission.inc)

- **Version:** `26.8.14.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Interface Module (`ifc.permission.inc`)

This file implements the **Permission Management Interface** for the PWNC Web Platform. It provides a user interface for managing users, groups, and their access permissions within the system. The interface handles creation, modification, activation, deactivation, and deletion of users and groups, as well as setting explicit permissions and exclusions.

The module operates as an **Interactive Form Controller (IFC)** and responds to various messages (commands) to perform actions on the permission system. It integrates with the `permission` class to manipulate user/group data and access control lists.

---

## Core Interface Logic

The file is structured as a **message-driven interface** that responds to predefined commands (e.g., `select`, `add_user`, `save_group`, `set_add`, etc.). Each command triggers a specific workflow, such as displaying forms, validating input, or applying changes to the permission system.

The interface maintains state via:
- `$object`: Currently selected user or group (e.g., `user.jdoe`, `group.admins`)
- `$type`: Current view type (`user` or `group`)
- `$list`: Array of selected objects for batch operations

---

## Key Functions & Workflows

### Message Handling (`CMS_IFC_MESSAGE` Switch)

The main logic is a `switch` statement over `CMS_IFC_MESSAGE`, which routes incoming commands to the appropriate handler.

#### ### `select`
**Purpose**: Updates the currently selected object.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Object identifier (e.g., `user.jdoe`) |

**Usage Context**:
Triggered when a user clicks on a user or group in the list. Updates the UI to show details of the selected object.

---

#### ### `select_user` / `select_group`
**Purpose**: Switches the view to display only users or groups, and clears the selected object if the type changes.
**Inner Mechanism**:
- Sets `$type` to `user` or `group`
- Clears `$object` if the current type does not match the new one

**Usage Context**:
Used when the user clicks the "Users" or "Groups" tab in the UI.

---

#### ### `add_user`
**Purpose**: Displays a form to create a new user.
**Form Fields**:
| Label | Field Type | Description |
|-------|------------|-------------|
| Name | `text 40 40 b` | Full name of the user |
| User ID | `text 40 40 b` | Unique login identifier |
| Password | `password 40 40 b` | Initial password |
| Repeat Password | `password 40 40` | Password confirmation |

**Usage Context**:
Triggered via the "Create User" button in the UI.

---

#### ### `_add_user`
**Purpose**: Processes the submitted user creation form.
**Parameters** (via `$ifc_param*`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name |
| `$ifc_param2` | `string` | User ID |
| `$ifc_param3` | `string` | Password |
| `$ifc_param4` | `string` | Password repetition |

**Return/Response**:
- `CMS_MSG_DONE`: User created successfully
- `CMS_MSG_ERROR`: User already exists or passwords do not match

**Inner Mechanism**:
- Validates password match
- Checks if user ID already exists
- Creates user via `permission->user()`
- Saves changes via `permission->save()`

**Example**:
```php
// After form submission, the system calls _add_user with:
// $ifc_param1 = "John Doe"
// $ifc_param2 = "jdoe"
// $ifc_param3 = "secret123"
// $ifc_param4 = "secret123"
$permission = new permission();
if ($permission->user("jdoe", TRUE, "John Doe", "secret123")) {
    $permission->save();
}
```

---

#### ### `add_group` / `_add_group`
**Purpose**: Displays and processes the group creation form.
**Form Fields**:
| Label | Field Type | Description |
|-------|------------|-------------|
| Name | `text 40 40 b` | Group name |
| Group ID | `text 40 40` | Unique group identifier |

**Parameters** (for `_add_group`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name |
| `$ifc_param2` | `string` | Group ID |

**Inner Mechanism**:
- Checks if group ID exists
- Creates group via `permission->group()`
- Saves changes

---

#### ### `save_user`
**Purpose**: Saves changes to an existing user.
**Parameters** (via `$ifc_param*`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name |
| `$ifc_param2` | `bool` | Disabled flag |
| `$ifc_param3` | `string` | New password (optional) |
| `$ifc_param4` | `string` | Password repetition |
| `$ifc_param5` | `array` | Selected groups |
| `$ifc_param6` | `string` | Permission string |
| `$ifc_param7` | `string` | Exclusion string |
| `$ifc_param8` | `string` | Email |
| `$ifc_param9` | `string` | Timezone |
| `$ifc_param10` | `string` | Comment |
| `$ifc_param11` | `int` | Expiration in seconds |

**Inner Mechanism**:
- Validates password (if provided)
- Converts group array to newline-separated string
- Updates user via `permission->user()`
- Saves changes

**Example**:
```php
$permission = new permission();
$permission->user("jdoe", FALSE, "John Doe", NULL, "group.admins\ngroup.editors", "content.edit", "admin.*", "john@example.com", "America/New_York", "Lead Editor", 86400 * 30);
$permission->save();
```

---

#### ### `save_group`
**Purpose**: Saves changes to an existing group.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name |
| `$ifc_param2` | `bool` | Disabled flag |
| `$ifc_param3` | `array` | Users to assign to this group |
| `$ifc_param4` | `string` | Permission string |
| `$ifc_param5` | `string` | Exclusion string |
| `$ifc_param6` | `string` | Comment |

**Inner Mechanism**:
- Updates group via `permission->group()`
- Iterates through all users and updates group membership
- Saves changes

---

#### ### `activate` / `deactivate`
**Purpose**: Enables or disables selected users/groups.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of object keys (e.g., `["user.jdoe", "group.admins"]`) |

**Inner Mechanism**:
- For each object in `$list`, removes or sets the `disabled` flag
- Saves changes

**Usage Context**:
Batch operations via the "Enable" or "Disable" buttons.

---

#### ### `delete`
**Purpose**: Deletes selected users or groups.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Objects to delete |

**Inner Mechanism**:
- Calls `permission->delete()` for each object
- Deselects `$object` if it was deleted
- Saves changes

---

#### ### `set`, `_set`, `set_add`, `set_add_ex`, `set_del`, `set_del_ex`
**Purpose**: Manages access permissions for users and groups.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Access string (e.g., `content.edit`) |
| `$ifc_param2` | `array` | Users/groups to exclude (for `set_del`) |
| `$ifc_param3` | `array` | Users/groups to permit (for `set_add`) |

**Inner Mechanism**:
- Uses `permission->permit()` or `permission->exclude()`
- Supports explicit mode (e.g., `set_add_ex`) for fine-grained control
- Caches the last accessed permission for the current user
- Displays a dual-list interface (permitted vs. excluded)
- Uses regex to highlight explicit permissions/exclusions

**Example**:
```php
$permission = new permission();
$permission->permit("user.jdoe", "content.edit", TRUE); // explicit permit
$permission->exclude("group.guests", "admin.*", FALSE); // implicit deny
$permission->save();
```

**UI Output**:
- Two multiselect lists: "Permitted" and "Excluded"
- Buttons to move items between lists
- Info text explaining explicit (`⁺`) and exclusion (`⁻`) markers

---

## Main Display Logic

The main display renders:
1. A left panel with a list of users or groups
2. A right panel with detailed properties of the selected object

### Data Preparation
- `$permission = new permission()`
- `$data = &$permission->data` (reference to internal data store)
- `data_sort($data, "name")` – sorts users/groups by name
- Groups are displayed in `[UPPERCASE]`
- Users are indented under their groups

### UI Components
| Component | Description |
|---------|-------------|
| Type Tabs | Toggle between "Users" and "Groups" |
| List with Checkboxes | Select multiple objects for batch operations |
| Initial Separators | A-Z headers for better navigation |
| Selection Controls | "All", "Invert", "None" buttons |
| Property Form | Editable fields for the selected object |

---

## Usage Example: Full Workflow

**Scenario**: Create a new user, assign to a group, and grant permission.

1. **Create User**:
   ```php
   // User submits form with:
   // Name: "Alice Smith"
   // User ID: "asmith"
   // Password: "alice123"
   // Repeat: "alice123"
   $permission = new permission();
   $permission->user("asmith", FALSE, "Alice Smith", "alice123");
   $permission->save();
   ```

2. **Assign to Group**:
   ```php
   // In save_user handler:
   $permission->user("asmith", FALSE, "Alice Smith", NULL, "group.editors");
   $permission->save();
   ```

3. **Grant Permission**:
   ```php
   // In set_add handler:
   $permission->permit("user.asmith", "content.publish", FALSE);
   $permission->save();
   ```

---

## Dependencies & Integration

- **`permission` class**: Core logic for user/group management and access control
- **`ifc` class**: UI form generator and message handler
- **`data` class**: Internal data storage and manipulation
- **`cms_cache()`**: Caches user access permissions
- **`image()`**: Renders icons (e.g., `permission/icon_user`)
- **`ifc_table_open()`, `ifc_table_close()`**: UI table helpers
- **`x()`, `qx()`**: Escaping utilities for HTML and JavaScript

---

## Security Considerations

- All user input is implicitly escaped via `x()`, `qx()`, and `ifc` form controls
- Passwords are handled via `password` input type (masked)
- CSRF protection is handled at the IFC level
- Disabled users/groups are skipped in permission evaluation
- Explicit permissions override group-level settings

---

## Performance Notes

- Uses buffered permission lookups (`$array`) to avoid repeated calls to `get_user_permission()`
- Sorts data once (`data_sort`) for consistent display
- Uses `preg_quote()` and regex for efficient string matching
- Caches user access permissions to avoid repeated computation


<!-- HASH:de1aba96862f8d5e472963f7fb16529c -->
