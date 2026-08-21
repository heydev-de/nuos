# PWNC API Documentation

[← Index](../README.md) | [`module/desktop.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/desktop.php)

- **Version:** `26.8.11.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Module (`module/desktop.php`)

The **Desktop Module** is the core interface of the PWNC Web Platform, providing a graphical desktop environment for users. It manages user-specific desktops, objects (e.g., links, notes, appointments), and interactions like drag-and-drop, object creation, and permissions. The module integrates with the platform's IFC (Interface Controller) for dynamic UI updates and leverages caching for performance.

---

## Overview

### Purpose
- **User-Specific Desktop**: Each user has a personalized desktop with objects (containers, links, notes, etc.).
- **Object Management**: Create, rename, move, delete, and share objects.
- **Drag-and-Drop**: Visual manipulation of objects (e.g., moving to containers or trash).
- **Background Customization**: Supports user-specific background images.
- **Permission Handling**: Ensures users only access their own desktops or shared objects.

### Key Features
| Feature               | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| **Object Types**      | Containers, links, notes, appointments, addresses, mailboxes.               |
| **Quick Access**      | Pinned objects for fast access.                                             |
| **Real-Time Updates** | Uses IFC for dynamic UI changes without full page reloads.                  |
| **Caching**           | Persists user preferences and object states.                                |
| **Security**          | Validates permissions and sanitizes inputs.                                 |

---

## Constants and Variables

### Constants
| Name            | Value/Default                     | Description                                                                 |
|-----------------|-----------------------------------|-----------------------------------------------------------------------------|
| `DESKTOP_USER`  | `$user`                           | Current user ID.                                                            |
| `DESKTOP_PATH`  | `CMS_DATA_PATH . "#desktop/..."`  | Path to user-specific desktop data.                                         |

### Variables
| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$desktop_display` | `string`   | Determines the display mode (`interface`, `link`, `background`).            |
| `$desktop`         | `desktop`  | Instance of the `desktop` class for managing objects.                       |
| `$object`          | `string`   | Current selected object ID.                                                 |
| `$parent`          | `string`   | Parent container ID (for nested objects).                                   |
| `$load`            | `bool`     | Flag to auto-load an object in a popup.                                     |
| `$desktop_icon`    | `array`    | Maps object types to their icon paths.                                      |
| `$desktop_type`    | `array`    | Maps object types to their numeric constants (e.g., `CMS_DESKTOP_TYPE_LINK`).|
| `$desktop_accept`  | `array`    | Defines which object types can be dropped into others (e.g., containers).   |

---

## Core Logic

### Initialization
1. **Load Libraries**: Imports `ifc` (Interface Controller) and `desktop` libraries.
2. **User Validation**: Syncs user data from cache and verifies permissions.
3. **Permission Check**: Ensures the user has access to the desktop.
4. **Display Mode Handling**: Routes to `interface`, `background`, or default desktop view.

---

## Functions and Methods

### `init($desktop_display)`
**Purpose**: Initializes the module based on the display mode.
**Parameters**:
| Name              | Type     | Description                                                                 |
|-------------------|----------|-----------------------------------------------------------------------------|
| `$desktop_display`| `string` | Display mode (`interface`, `link`, `background`).                           |

**Inner Mechanisms**:
- Sets `$ifc_option` to `external` for non-default modes.
- Loads required libraries and syncs user data from cache.

**Usage Context**:
- Called at the start of the script to configure the environment.

---

### Message Handling (IFC Integration)
The module processes IFC messages (e.g., `activate`, `rename`, `delete`) to update the desktop state dynamically.

#### Example: `activate` Message
**Purpose**: Activates an object or container.
**Parameters**:
| Name          | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$ifc_param`  | `string` | Object/container ID to activate.                                            |

**Logic**:
- If the target is a container, sets `$parent` to its ID.
- Otherwise, sets `$object` and flags `$load` to open it in a popup.

**Usage Example**:
```javascript
// JavaScript: Activate an object via IFC
ifc_post("activate", "obj123");
```

---

### `desktop` Class Methods
The `desktop` class (loaded via `cms_load("desktop")`) manages objects. Key methods used in this module:

#### `object_type($id)`
**Purpose**: Returns the type of an object (e.g., `link`, `container`).
**Parameters**:
| Name | Type     | Description       |
|------|----------|-------------------|
| `$id`| `string` | Object ID.        |

**Return Value**: `string` (object type).

**Usage Example**:
```php
$type = $desktop->object_type("obj123"); // Returns "link"
```

---

#### `object_get($id, $key)`
**Purpose**: Retrieves a property of an object (e.g., `name`, `x`, `y`).
**Parameters**:
| Name  | Type     | Description               |
|-------|----------|---------------------------|
| `$id` | `string` | Object ID.                |
| `$key`| `string` | Property name (e.g., `x`).|

**Return Value**: `mixed` (property value).

**Usage Example**:
```php
$name = $desktop->object_get("obj123", "name"); // Returns "My Note"
```

---

#### `object_set($id, $key, $value)`
**Purpose**: Sets a property of an object.
**Parameters**:
| Name    | Type     | Description               |
|---------|----------|---------------------------|
| `$id`   | `string` | Object ID.                |
| `$key`  | `string` | Property name.            |
| `$value`| `mixed`  | Property value.           |

**Usage Example**:
```php
$desktop->object_set("obj123", "name", "Updated Note");
```

---

#### `create_object($parent, $type, $name)`
**Purpose**: Creates a new object.
**Parameters**:
| Name     | Type     | Description               |
|----------|----------|---------------------------|
| `$parent`| `string` | Parent container ID.      |
| `$type`  | `string` | Object type (e.g., `note`).|
| `$name`  | `string` | Object name.              |

**Return Value**: `string` (new object ID) or `FALSE` on failure.

**Usage Example**:
```php
$newId = $desktop->create_object("container1", "note", "My Note");
```

---

#### `move_object($id, $parent)`
**Purpose**: Moves an object to a new parent container.
**Parameters**:
| Name     | Type     | Description               |
|----------|----------|---------------------------|
| `$id`    | `string` | Object ID.                |
| `$parent`| `string` | New parent container ID.  |

**Return Value**: `string` (object ID) or `FALSE` on failure.

**Usage Example**:
```php
$desktop->move_object("obj123", "container1");
```

---

#### `delete_object($id)`
**Purpose**: Deletes an object.
**Parameters**:
| Name | Type     | Description       |
|------|----------|-------------------|
| `$id`| `string` | Object ID.        |

**Return Value**: `bool` (success/failure).

**Usage Example**:
```php
$desktop->delete_object("obj123");
```

---

## Display Modes

### Default Mode
Renders the desktop UI with:
- **Control Panel**: User selection, quick access, object creation.
- **Objects**: Icons for containers, links, notes, etc.
- **Drag-and-Drop**: Supports moving objects to containers/trash.
- **Background**: Customizable image (JPG/PNG/WEBP).

**Key UI Elements**:
| Element               | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| **Desktop Control**   | Logo, user selection, logout, date/time.                                   |
| **Quick Access**      | Pinned objects.                                                             |
| **Object Creation**   | Buttons to create new objects (e.g., notes, containers).                   |
| **Path Navigation**   | Breadcrumbs for nested containers.                                         |
| **Appointments**      | Upcoming appointments (highlighted if imminent).                           |

**Example: Object Creation**:
```javascript
// JavaScript: Create a new note
desktop_create("note");
```

---

### `interface` Mode
Loads a specialized interface for the selected object type (e.g., note editor, link properties).

**Logic**:
1. Validates the object type.
2. Includes the corresponding interface file (e.g., `desktop.note.inc`).
3. Exits to prevent further execution.

**Example**:
```php
// URL: desktop.php?desktop_display=interface&object=obj123
$desktop_interface = $desktop->object_type("obj123"); // Returns "note"
require(CMS_DESKTOP_PATH . "desktop.note.inc");
```

---

### `background` Mode
Serves the user's background image with caching headers.

**Logic**:
1. Validates the file extension (JPG/PNG/WEBP).
2. Sends `Cache-Control` and `Content-Type` headers.
3. Outputs the file via `readfile()`.

**Example**:
```php
// URL: desktop.php?desktop_display=background&user=user1&extension=png
header("Content-Type: image/png");
readfile(DESKTOP_PATH . "background.png");
```

---

## JavaScript Integration

### Drag-and-Drop (`dd_*` Functions)
The module registers objects for drag-and-drop using the `dd_register()` function and defines a callback (`desktop_event`) for events like `activate`, `select`, and `dropon`.

**Example: Registering an Object**:
```javascript
dd_register("dd-obj123", 1, 255); // Type 1 (link), accepts all types (255)
```

**Event Handling**:
```javascript
function desktop_event(event, source, target) {
    switch (event) {
        case "dropon":
            if (target.id === "dd-trashbin") ifc_post("delete", source.id.substring(3));
            break;
    }
}
```

---

## Caching
The module uses `cms_cache()` to persist:
- Selected object (`desktop.{user}.object`).
- Parent container (`desktop.{user}.parent`).

**Example**:
```php
cms_cache("desktop.user1.object", "obj123", TRUE); // Persist to disk
```

---

## Security
- **Input Sanitization**: Uses `q()`, `qx()`, and `x()` for escaping outputs.
- **Permission Checks**: Validates user access via `cms_permission()`.
- **CSRF Protection**: Relies on `cms_url()` and `cms_param()` for secure parameter handling.

**Example: Escaping Output**:
```php
echo("<div id=\"" . x("dd-obj123") . "\">"); // XML-escaped ID
```

---

## Usage Examples

### 1. Creating a New Note
**Scenario**: User clicks the "Note" button in the desktop UI.
**Code Flow**:
1. JavaScript calls `desktop_create("note")`.
2. IFC posts a `create` message with the type and name.
3. PHP creates the object and returns its ID.

**Example**:
```javascript
desktop_create("note"); // Opens a prompt for the note name
```

---

### 2. Moving an Object to Trash
**Scenario**: User drags an object to the trash bin.
**Code Flow**:
1. JavaScript triggers `desktop_event("dropon", source, target)`.
2. IFC posts a `delete` message with the object ID.
3. PHP deletes the object and updates the UI.

**Example**:
```javascript
// Handled automatically by drag-and-drop
```

---

### 3. Changing the Background Image
**Scenario**: User uploads a new background image.
**Code Flow**:
1. IFC posts a `config` message with the uploaded file.
2. PHP deletes the old background and saves the new one.
3. The UI refreshes to show the new background.

**Example**:
```php
// Handled via the config interface
```

---

### 4. Opening an Appointment
**Scenario**: User clicks an appointment in the desktop.
**Code Flow**:
1. JavaScript calls `desktop_activate(id, CMS_DESKTOP_TYPE_APPOINTMENT)`.
2. IFC posts an `activate` message.
3. PHP loads the appointment interface in a popup.

**Example**:
```javascript
desktop_activate("obj123", <?php echo(q(CMS_DESKTOP_TYPE_APPOINTMENT)); ?>);
```

---


<!-- HASH:778220edb2c1f0e83c2ed871e75861cd -->
