# PWNC API Documentation

[← Index](../README.md) | [`module/desktop.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/desktop.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## desktop

The `desktop.php` module serves as the central controller for the PWNC desktop environment. It manages user-specific desktop layouts, object interactions (links, notes, appointments, addresses, containers, mailboxes), and provides an interface for creating, moving, deleting, and configuring desktop objects. It also handles user preferences such as password changes, timezone settings, and background images.

### Module Initialization

The module begins by initializing the desktop display mode, loading required libraries (`ifc`, `desktop`), syncing user data from cache, verifying permissions, and defining constants for the current user and their desktop path.

#### Constants

| Name | Value | Description |
|------|-------|-------------|
| `DESKTOP_USER` | `$user` | The username of the currently logged-in desktop user. |
| `DESKTOP_PATH` | `CMS_DATA_PATH . "#desktop/" . encode_filename(DESKTOP_USER) . "/"` | Filesystem path to the user's desktop data directory. |

### Message Handling

The main logic is driven by `CMS_IFC_MESSAGE`, which determines the action to perform. Each case corresponds to a specific operation on the desktop.

#### `activate`

Activates a desktop object or navigates into a container.

- **Parameters**: `$ifc_param` — The ID of the object to activate.
- **Behavior**: If the parameter is empty or refers to a container, it sets the parent context. Otherwise, it selects the object and flags it for loading.
- **Usage**: Triggered when a user clicks on a desktop icon or folder.

```php
// Example: Activate object with ID "note_123"
$ifc_param = "note_123";
// Sets $object = "note_123", $load = TRUE
```

#### `select`

Selects a desktop object without activating it.

- **Parameters**: `$ifc_param` — The ID of the object to select.
- **Behavior**: Sets the selected object.
- **Usage**: Used for highlighting or preparing an object for further actions like rename or quick access toggle.

```php
// Example: Select object with ID "link_456"
$ifc_param = "link_456";
// Sets $object = "link_456"
```

#### `create`

Creates a new desktop object.

- **Parameters**: `$ifc_param` — A semicolon-separated string containing the object type and name (e.g., `"note;My Note"`).
- **Behavior**: Splits the parameter, creates the object under the current parent, and optionally loads it if it's a container.
- **Usage**: Invoked when a user creates a new link, note, appointment, etc.

```php
// Example: Create a note named "Meeting Notes"
$ifc_param = "note;Meeting Notes";
// Creates a new note object under $parent
```

#### `quickaccess`

Toggles the quick access flag on the currently selected object.

- **Parameters**: None (uses global `$object`).
- **Behavior**: Toggles the `quickaccess` property of the selected object and saves the desktop state.
- **Usage**: Allows users to pin/unpin frequently used objects to the sidebar.

```php
// Example: Toggle quick access for selected object
$object = "note_123";
// Toggles quickaccess property
```

#### `rename`

Renames the currently selected object.

- **Parameters**: `$ifc_param` — The new name for the object.
- **Behavior**: Updates the object's name and saves the desktop state.
- **Usage**: Called when a user renames an object via the rename dialog.

```php
// Example: Rename selected object to "Updated Name"
$ifc_param = "Updated Name";
// Sets object name to "Updated Name"
```

#### `drop`

Updates the position of a desktop object after dragging.

- **Parameters**: `$ifc_param` — A comma-separated string containing the object ID, X coordinate, and Y coordinate (e.g., `"note_123,100,200"`).
- **Behavior**: Parses coordinates, updates the object's position, and saves the desktop state.
- **Usage**: Triggered when a user drops a dragged object at a new location.

```php
// Example: Move object to position (150, 300)
$ifc_param = "note_123,150,300";
// Sets x=150, y=300 for note_123
```

#### `move`

Moves an object to a different parent container.

- **Parameters**: `$ifc_param` — A comma-separated string containing the object ID and target parent ID (e.g., `"note_123,container_456"`).
- **Behavior**: Moves the object to the specified parent, resets its position, and saves the desktop state.
- **Usage**: Used when a user drags an object onto a container.

```php
// Example: Move object into container
$ifc_param = "note_123,container_456";
// Moves note_123 into container_456
```

#### `delete`

Deletes a desktop object.

- **Parameters**: `$ifc_param` — The ID of the object to delete.
- **Behavior**: Removes the object from the desktop and saves the state. If the deleted object was selected, clears the selection.
- **Usage**: Triggered when a user deletes an object (e.g., via trashbin drop).

```php
// Example: Delete object with ID "note_123"
$ifc_param = "note_123";
// Removes note_123 from desktop
```

#### `send`

Sends a desktop object to another user.

- **Parameters**: `$ifc_param` — The ID of the object to send.
- **Behavior**: Displays a multi-select interface listing eligible users. On submission (`_send`), copies the object to each selected user's desktop.
- **Usage**: Allows sharing notes, links, or other objects with other users.

```php
// Example: Send object to selected users
$ifc_param = "note_123";
// Shows user selection dialog
```

#### `_send`

Handles the actual sending of objects to selected users.

- **Parameters**: `$ifc_param1` — Array of selected user keys.
- **Behavior**: Copies the object data to each recipient's desktop, wrapping it in a mailbox container.
- **Usage**: Internal handler called after user selection in the `send` flow.

```php
// Example: Send to users ["alice", "bob"]
$ifc_param1 = ["alice", "bob"];
// Copies object to alice's and bob's desktops
```

#### `__config`

Placeholder for configuration completion.

- **Behavior**: Returns a success message.
- **Usage**: Internal handler for configuration completion.

#### `_config`

Processes desktop configuration changes.

- **Parameters**:
  - `$ifc_param1` — New password (if changing).
  - `$ifc_param2` — Boolean indicating whether to use MD5 hashing.
  - `$ifc_param3` — Current password (for verification).
  - `$ifc_param4` — New password confirmation.
  - `$ifc_param5` — Timezone identifier.
  - `$ifc_param6` — Background image upload flag.
  - `$ifc_file1` — Uploaded background image file.
  - `$ifc_file1_name` — Original filename of uploaded image.
- **Behavior**: Validates and updates password, timezone, and background image. Redirects on success.
- **Usage**: Called when a user submits the desktop configuration form.

```php
// Example: Update timezone
$ifc_param5 = "Europe/Berlin";
// Sets user timezone to Europe/Berlin
```

#### `config`

Displays the desktop configuration interface.

- **Behavior**: Creates an interface form for changing password, timezone, and background image.
- **Usage**: Shown when a user clicks the configuration button.

```php
// Example: Display configuration form
// Shows password, timezone, and background image fields
```

### Desktop Rendering

After processing messages, the module renders the desktop interface.

#### Icon Mapping

Maps object types to their corresponding icon paths.

| Type | Icon Path |
|------|-----------|
| `link` | `desktop/icon_webpage` |
| `note` | `desktop/icon_note` |
| `appointment` | `desktop/icon_appointment` |
| `address` | `desktop/icon_address` |
| `container` | `desktop/icon_container` |
| `+container` | `desktop/icon_containeropen` |
| `mailbox` | `desktop/icon_mailbox` |

#### Type Mapping

Maps object types to their desktop type constants.

| Type | Constant |
|------|----------|
| `link` | `CMS_DESKTOP_TYPE_LINK` |
| `note` | `CMS_DESKTOP_TYPE_NOTE` |
| `appointment` | `CMS_DESKTOP_TYPE_APPOINTMENT` |
| `address` | `CMS_DESKTOP_TYPE_ADDRESS` |
| `container` | `CMS_DESKTOP_TYPE_CONTAINER` |
| `mailbox` | `CMS_DESKTOP_TYPE_MAILBOX` |

#### Accept Mapping

Defines what types of objects each object type can accept as drop targets.

| Type | Accepted Types |
|------|----------------|
| `link` | `CMS_DESKTOP_TYPE_NONE` |
| `note` | `CMS_DESKTOP_TYPE_NONE` |
| `appointment` | `CMS_DESKTOP_TYPE_NONE` |
| `address` | `CMS_DESKTOP_TYPE_NONE` |
| `container` | `CMS_DESKTOP_TYPE_ALL` |
| `mailbox` | `CMS_DESKTOP_TYPE_NONE` |

### Interface Display

#### `interface`

Displays the interface for a specific desktop object type.

- **Parameters**: `$object` — The ID of the object to display.
- **Behavior**: Determines the object type, loads the corresponding interface file, and renders it.
- **Usage**: Shown when a user activates a desktop object that has an associated interface.

```php
// Example: Display interface for a note object
$object = "note_123";
// Loads and displays desktop.note.inc
```

#### `background`

Serves the desktop background image.

- **Parameters**: `$extension` — Image file extension (jpg, png, webp).
- **Behavior**: Validates the extension, checks for the file's existence, sends appropriate headers, and outputs the image.
- **Usage**: Called when the browser requests the background image URL.

```php
// Example: Serve background image
$extension = "jpg";
// Outputs background.jpg with proper headers
```

### JavaScript Integration

The module includes several JavaScript functions for client-side interactivity:

- `desktop_rename()` — Prompts for a new name and posts a rename request.
- `desktop_create(type)` — Prompts for a name and posts a create request.
- `desktop_user_select(value)` — Changes the active user.
- `desktop_event(event, source, target)` — Handles drag-and-drop events.
- `desktop_activate(id, type)` — Activates an object, preopening popups for certain types.

These functions interact with the PWNC interface framework to provide a dynamic desktop experience.


<!-- HASH:6d93ccc103de12e9f607f889049c79b4 -->
