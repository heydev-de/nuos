# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.desktop.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.desktop.inc)

- **Version:** `26.7.23.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Class

The `desktop` class manages user-specific desktop environments in the PWNC Web Platform. It provides functionality for creating, modifying, and organizing desktop objects such as links, notes, appointments, addresses, containers, and mailboxes. The class ensures that default containers (mailbox, appointments, addresses) are automatically created for new users.

### Constants

| Name                     | Value | Description                                                                 |
|--------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_DESKTOP_TYPE_NONE`  | 0     | No desktop object type                                                      |
| `CMS_DESKTOP_TYPE_LINK`  | 1     | Link object type                                                            |
| `CMS_DESKTOP_TYPE_NOTE`  | 2     | Note object type                                                            |
| `CMS_DESKTOP_TYPE_APPOINTMENT` | 4 | Appointment object type                                                     |
| `CMS_DESKTOP_TYPE_ADDRESS`     | 8     | Address object type                                                         |
| `CMS_DESKTOP_TYPE_CONTAINER`   | 16    | Container object type (can hold other objects)                              |
| `CMS_DESKTOP_TYPE_MAILBOX`     | 32    | Mailbox object type                                                         |
| `CMS_DESKTOP_TYPE_ALL`   | 255   | All desktop object types (bitmask)                                          |

### Properties

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `user` | `NULL`        | User identifier for whom the desktop is managed. Defaults to `CMS_SUPERUSER`. |
| `data` | `NULL`        | Instance of the `data` class handling desktop object storage and retrieval.  |

---

### `__construct($user = NULL)`

**Purpose:**
Initializes the desktop for a specified user, ensuring default containers (mailbox, appointments, addresses) exist.

**Parameters:**

| Name   | Type   | Description                                                                 |
|--------|--------|-----------------------------------------------------------------------------|
| `$user`| string | User identifier. If `NULL`, defaults to `CMS_SUPERUSER`.                    |

**Return Values:**
- **void**: No return value. Initializes the desktop environment.

**Inner Mechanisms:**
1. Sets the user and initializes the `data` property to manage desktop objects stored in `#desktop/{user}/desktop`.
2. Checks for existing default containers (mailbox, appointments, addresses) using a bitmask flag.
3. Creates missing default containers with predefined names (`CMS_L_DESKTOP_001`, `CMS_L_DESKTOP_002`, `CMS_L_DESKTOP_003`).
4. Saves changes to the data store if any default containers were created.

**Usage Context:**
- Called when initializing a user's desktop environment.
- Ensures a consistent starting point for all users with default containers.

**Example:**
```php
// Initialize desktop for the current user
$desktop = new \cms\desktop();
```

---

### `object_get($index, $property)`

**Purpose:**
Retrieves a property value of a desktop object identified by `$index`.

**Parameters:**

| Name        | Type   | Description                                                                 |
|-------------|--------|-----------------------------------------------------------------------------|
| `$index`    | mixed  | Identifier of the desktop object.                                           |
| `$property` | string | Property name to retrieve (e.g., `name`, `color`, `#type`).                 |

**Return Values:**
- **mixed**: The property value if found; `NULL` otherwise.

**Inner Mechanisms:**
- Delegates the retrieval to the `data` object's `get` method.

**Usage Context:**
- Used to fetch properties of desktop objects (e.g., name, type, color).

**Example:**
```php
// Get the name of a desktop object
$name = $desktop->object_get(1, "name");
```

---

### `object_set($index, $property, $value = NULL)`

**Purpose:**
Sets a property value for a desktop object identified by `$index`.

**Parameters:**

| Name        | Type   | Description                                                                 |
|-------------|--------|-----------------------------------------------------------------------------|
| `$index`    | mixed  | Identifier of the desktop object.                                           |
| `$property` | string | Property name to set (e.g., `name`, `color`).                               |
| `$value`    | mixed  | Value to assign to the property.                                            |

**Return Values:**
- **bool**: `TRUE` if the property was set successfully; `FALSE` if the object does not exist.

**Inner Mechanisms:**
- Checks if the object exists using `data->get($index)`.
- Delegates the property assignment to the `data` object's `set` method if the object exists.

**Usage Context:**
- Used to update properties of existing desktop objects.

**Example:**
```php
// Set the color of a desktop object
$success = $desktop->object_set(1, "color", "#FF5733");
```

---

### `save()`

**Purpose:**
Persists all changes made to the desktop objects to the data store.

**Parameters:**
- None.

**Return Values:**
- **bool**: `TRUE` if changes were saved successfully; `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates the save operation to the `data` object's `save` method.

**Usage Context:**
- Called after making multiple changes to ensure persistence.

**Example:**
```php
// Save all changes to the desktop
$desktop->save();
```

---

### `create_object($index, $type, $name)`

**Purpose:**
Creates a new desktop object of a specified type and name at a given position.

**Parameters:**

| Name     | Type   | Description                                                                 |
|----------|--------|-----------------------------------------------------------------------------|
| `$index` | mixed  | Position where the object should be inserted.                               |
| `$type`  | string | Type of the object (`link`, `note`, `appointment`, `address`, `container`).  |
| `$name`  | string | Name of the object.                                                         |

**Return Values:**
- **mixed**: The identifier of the newly created object if successful; `FALSE` otherwise.

**Inner Mechanisms:**
1. Generates a color for the object using `strtocolor($name, 25, FALSE)`.
2. Prepares a buffer for the object based on its type:
   - For containers, includes opening and closing container tags.
   - For other types, includes only the object definition.
3. Inserts the object into the data store at the specified position.
4. Saves changes if the insertion is successful.

**Usage Context:**
- Used to add new objects (e.g., links, notes, containers) to the desktop.

**Example:**
```php
// Create a new note on the desktop
$noteId = $desktop->create_object(0, "note", "Meeting Notes");
```

---

### `move_object($source, $target)`

**Purpose:**
Moves a desktop object from one position (`$source`) to another (`$target`).

**Parameters:**

| Name      | Type  | Description                                                                 |
|-----------|-------|-----------------------------------------------------------------------------|
| `$source` | mixed | Identifier of the object to move.                                           |
| `$target` | mixed | Target position where the object should be moved.                           |

**Return Values:**
- **mixed**: The new identifier of the moved object if successful; `FALSE` otherwise.

**Inner Mechanisms:**
1. Cuts the object from its current position using `data->cut($source)`.
2. Inserts the object at the target position using `data->insert($target)`.
3. Saves changes if both operations succeed.

**Usage Context:**
- Used to reorganize desktop objects (e.g., dragging and dropping).

**Example:**
```php
// Move an object to a new position
$newId = $desktop->move_object(1, 5);
```

---

### `delete_object($index)`

**Purpose:**
Deletes a desktop object identified by `$index` and performs cleanup based on the object type.

**Parameters:**

| Name    | Type  | Description                                                                 |
|---------|-------|-----------------------------------------------------------------------------|
| `$index`| mixed | Identifier of the object to delete.                                         |

**Return Values:**
- **bool**: `TRUE` if the object was deleted successfully; `FALSE` otherwise.

**Inner Mechanisms:**
1. Retrieves the object type before deletion.
2. Deletes the object using `data->del($index)`.
3. Saves changes if the deletion is successful.
4. Performs type-specific cleanup:
   - For mailboxes, deletes associated files using `filemanager_delete`.

**Usage Context:**
- Used to remove objects from the desktop (e.g., user-initiated deletion).

**Example:**
```php
// Delete a desktop object
$success = $desktop->delete_object(1);
```

---

### `object_type($index)`

**Purpose:**
Retrieves the type of a desktop object identified by `$index`.

**Parameters:**

| Name    | Type  | Description                                                                 |
|---------|-------|-----------------------------------------------------------------------------|
| `$index`| mixed | Identifier of the desktop object.                                           |

**Return Values:**
- **string**: The object type (e.g., `link`, `note`, `container`).

**Inner Mechanisms:**
- Delegates the retrieval to the `data` object's `get` method with `#type` as the property.

**Usage Context:**
- Used to determine the type of an object before performing type-specific operations.

**Example:**
```php
// Get the type of a desktop object
$type = $desktop->object_type(1);
```

---

### `get_parent($index)`

**Purpose:**
Retrieves the parent identifier of a desktop object identified by `$index`.

**Parameters:**

| Name    | Type  | Description                                                                 |
|---------|-------|-----------------------------------------------------------------------------|
| `$index`| mixed | Identifier of the desktop object.                                           |

**Return Values:**
- **mixed**: The parent identifier if found; `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates the retrieval to the `data` object's `move` method with `parent` as the direction.

**Usage Context:**
- Used to navigate the desktop object hierarchy (e.g., finding the parent of a container).

**Example:**
```php
// Get the parent of a desktop object
$parentId = $desktop->get_parent(1);
```


<!-- HASH:564af47af8d8f89c63f95b52d11d75f9 -->
