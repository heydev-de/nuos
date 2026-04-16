# NUOS API Documentation

[← Index](../README.md) | [`module/desktop.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Module (`desktop.php`)

The `desktop.php` module serves as the core interface for the NUOS web platform's desktop environment. It provides a graphical user interface for managing objects (links, notes, appointments, addresses, containers, and mailboxes) with drag-and-drop functionality, user-specific configurations, and real-time interactions. The module handles object lifecycle operations (create, read, update, delete), user switching, background customization, and integrates with other system components like the interface module and permission system.

---

### Constants and Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `DESKTOP_USER` | `CMS_SUPERUSER` or authenticated user | Current user identifier for the desktop session. |
| `DESKTOP_PATH` | `CMS_DATA_PATH . "#desktop/" . safe_filename(DESKTOP_USER) . "/"` | Filesystem path to the user's desktop data directory. |
| `$desktop_display` | `NULL`, `"interface"`, or `"link"` | Determines the display mode of the desktop. |
| `$ifc_option` | `"external"` if `$desktop_display` is `"interface"` or `"link"` | Interface option flag for external handling. |

---

### Initialization and Setup

#### Libraries and Dependencies
- **`ifc`**: Interface controller for handling UI interactions and messaging.
- **`desktop`**: Core desktop management class for object manipulation.
- **Permission Data**: Loaded from `#system/permission` to validate user access.

#### User Validation
- If no user is specified or the user lacks permissions, defaults to `CMS_SUPERUSER`.
- Validates permissions for the current user to access the desktop.

#### Cache Synchronization
- Synchronizes user-specific desktop settings from cache (`desktop.{user}.user`).

---

### Class: `desktop`

*(Note: The `desktop` class is loaded via `cms_load("desktop", TRUE)` and is documented separately. This section focuses on its usage within `desktop.php`.)*

---

### Main Logic Flow

The module operates in three primary modes, determined by `$desktop_display`:

1. **Default Mode**: Renders the interactive desktop UI with objects, controls, and menus.
2. **Interface Mode**: Delegates to specialized interface handlers (e.g., `desktop.mailbox.inc`).
3. **Background Mode**: Serves the user's custom background image.

---

### Message Handling

The desktop processes messages via `CMS_IFC_MESSAGE` to perform actions on objects. Each message triggers a specific operation:

| Message | Parameters | Purpose | Return/Response |
|---------|------------|---------|-----------------|
| `activate` | `$ifc_param` (object ID) | Activates an object or container. If the object is a container, sets it as the active parent. Otherwise, loads the object for editing. | Updates `$parent` or `$object` and `$load` flags. |
| `select` | `$ifc_param` (object ID) | Selects an object (e.g., for highlighting). | Updates `$object`. |
| `create` | `$ifc_param` (type;name) | Creates a new object of the specified type with a given name. | Returns the new object ID or `CMS_MSG_ERROR` on failure. |
| `quickaccess` | `$object` | Toggles the "quickaccess" flag for the object (adds/removes from favorites). | Updates the object and saves the desktop. Returns `CMS_MSG_ERROR` on failure. |
| `rename` | `$ifc_param` (new name) | Renames the currently selected object. | Updates the object and saves the desktop. Returns `CMS_MSG_ERROR` on failure. |
| `drop` | `$ifc_param` (objectID,x,y) | Updates the position of an object on the desktop. | Updates the object's `x` and `y` coordinates and saves the desktop. Returns `CMS_MSG_ERROR` on failure. |
| `move` | `$ifc_param` (objectID,parentID) | Moves an object to a new parent container. | Updates the object's parent and resets its position. Returns `CMS_MSG_ERROR` on failure. |
| `delete` | `$ifc_param` (object ID) | Deletes an object from the desktop. | Removes the object and updates `$object` if it was the selected object. Returns `CMS_MSG_ERROR` on failure. |
| `send` | `$ifc_param` (object ID) | Initiates the process to send an object to other users. | Displays a user selection interface. |
| `_send` | `$ifc_param1` (array of user IDs) | Completes the send operation by copying the object to selected users' desktops. | Returns `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure. |
| `config` | `$ifc_param1` to `$ifc_param6` | Displays the configuration interface for password, timezone, and background image. | Renders the configuration form. |
| `_config` | `$ifc_param1` to `$ifc_param6` | Processes configuration changes (password, timezone, background image). | Updates user settings, saves data, and redirects. Returns `CMS_MSG_ERROR` on failure. |
| `__config` | None | Placeholder for post-configuration handling. | Returns `CMS_MSG_DONE`. |

---

### Desktop Rendering (Default Mode)

#### Key Components
1. **Background Image**:
   - Checks for `background.{jpg,png,webp}` in `DESKTOP_PATH`.
   - Renders a `<div>` with the background image if found.

2. **Desktop Control Panel**:
   - **Logo**: Links to the homepage.
   - **Interface Links**: Access to the interface module and website.
   - **User Selection**: Dropdown to switch between users (if multiple are available).
   - **Favorites**: Quick-access links for objects marked with `quickaccess`.
   - **Create Menu**: Buttons to create new objects (link, note, appointment, address, container, mailbox).
   - **Appointments**: Displays upcoming appointments.
   - **Receiver/Trashbin**: Drop targets for sending or deleting objects.
   - **Logout**: Logs the user out.

3. **Object Rendering**:
   - Objects are rendered as draggable `<div>` elements with icons and names.
   - Containers can be nested; the current path is displayed.
   - Objects are positioned using `x` and `y` coordinates (rasterized to a 105x60 grid).

4. **JavaScript Callbacks**:
   - `desktop_event`: Handles drag-and-drop events (activate, select, drop, dropon).
   - `desktop_activate`: Opens objects in a popup or loads them into the interface.
   - `desktop_rename`: Prompts the user to rename an object.
   - `desktop_create`: Prompts the user to create a new object of a specified type.

5. **Drag-and-Drop System**:
   - Uses the `dd_*` functions to register draggable objects and drop targets.
   - Objects can be moved, deleted (dropped on trashbin), or sent to other users (dropped on receiver).

---

### Interface Mode (`$desktop_display = "interface"`)

Delegates to specialized interface handlers based on the object type (`$desktop_interface`):

| Interface | File | Purpose |
|-----------|------|---------|
| `mailbox` | `desktop.mailbox.inc` | Manages email-like messages. |
| `address` | `desktop.address.inc` | Manages contact information. |
| `note` | `desktop.note.inc` | Edits text notes. |
| `appointment` | `desktop.appointment.inc` | Manages calendar appointments. |
| `ims` | `desktop.ims.inc` | Instant messaging interface. |
| `link` | `desktop.link.inc` | Edits web links. |

If no valid interface is specified, calls `ifc_close_external()` to close the interface.

---

### Background Mode (`$desktop_display = "background"`)

Serves the user's custom background image:

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$extension` | `string` | File extension (`jpg`, `png`, or `webp`). |
| `$time` | `int` | Cache-busting timestamp (file modification time). |

#### Behavior
- Validates the extension and checks if the file exists.
- Sends appropriate `Cache-Control` and `Content-Type` headers.
- Outputs the file contents using `readfile()`.
- Returns a `404` response if the file is not found.

---

### Helper Arrays

| Array | Purpose | Key-Value Pairs |
|-------|---------|-----------------|
| `$desktop_icon` | Maps object types to icon paths. | `"link" => "desktop/icon_webpage"`, `"note" => "desktop/icon_note"`, etc. |
| `$desktop_type` | Maps object types to numeric constants. | `"link" => CMS_DESKTOP_TYPE_LINK`, `"note" => CMS_DESKTOP_TYPE_NOTE`, etc. |
| `$desktop_accept` | Defines which object types can be dropped into containers. | `"container" => CMS_DESKTOP_TYPE_ALL`, others `CMS_DESKTOP_TYPE_NONE`. |

---

### Caching
- Selected object and parent are cached permanently using `cms_cache` to persist state across sessions.
- Cache keys: `desktop.{DESKTOP_USER}.object` and `desktop.{DESKTOP_USER}.parent`.

---

### Usage Scenarios

1. **User Interaction**:
   - Users can create, rename, move, and delete objects via the UI.
   - Objects can be dragged to new positions or dropped on the trashbin/receiver.
   - Favorites provide quick access to frequently used objects.

2. **Configuration**:
   - Users can change their password, timezone, and background image via the config interface.

3. **Multi-User Support**:
   - Superusers can switch between users to manage their desktops.
   - Objects can be sent to other users (e.g., sharing a note or link).

4. **Integration**:
   - The desktop integrates with the interface module for editing objects.
   - Background images are served dynamically with cache control.

---

### Security Considerations
- **Permissions**: All actions are validated against the user's permissions.
- **Input Sanitization**: Uses `x()`, `q()`, and `qrx()` for escaping output in HTML, JavaScript, and URLs.
- **CSRF Protection**: Relies on `cms_url` and `ifc_post` for secure form submissions.
- **File Uploads**: Background images are validated for allowed extensions (`jpg`, `png`, `webp`).

---

### Performance Optimizations
- **Rasterization**: Object positions are snapped to a grid to prevent overlap and improve visual consistency.
- **Caching**: User-specific settings and object states are cached to reduce database load.
- **Lazy Loading**: Objects are loaded on-demand (e.g., appointments are only fetched for the current time window).


<!-- HASH:6527daedfa9c6858f24b396070c85cfc -->
