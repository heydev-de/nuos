# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.desktop.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Class

The `desktop` class in NUOS provides a structured way to manage user-specific desktop objects such as links, notes, appointments, addresses, containers, and mailboxes. It acts as a personal workspace for users, storing and organizing these objects in a hierarchical data structure. The class ensures default containers (mailbox, appointments, addresses) exist for each user upon initialization.

---

### Constants

| Name                     | Value | Description                                                                 |
|--------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_DESKTOP_TYPE_NONE`  | 0     | Represents no desktop object type.                                          |
| `CMS_DESKTOP_TYPE_LINK`  | 1     | Represents a link object.                                                   |
| `CMS_DESKTOP_TYPE_NOTE`  | 2     | Represents a note object.                                                   |
| `CMS_DESKTOP_TYPE_APPOINTMENT` | 4 | Represents an appointment object.                                           |
| `CMS_DESKTOP_TYPE_ADDRESS`     | 8 | Represents an address object.                                               |
| `CMS_DESKTOP_TYPE_CONTAINER`   | 16 | Represents a container object (can hold other objects).                     |
| `CMS_DESKTOP_TYPE_MAILBOX`     | 32 | Represents a mailbox object.                                                |
| `CMS_DESKTOP_TYPE_ALL`   | 255   | Represents all desktop object types (bitmask).                              |

---

### Properties

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `user` | `NULL`        | Stores the user identifier for whom the desktop is managed.                |
| `data` | `NULL`        | Instance of the `data` class, handling the storage and retrieval of desktop objects. |

---

### Constructor

#### `desktop::__construct($user = NULL)`

**Purpose:**
Initializes a desktop instance for a specified user. If no user is provided, it defaults to `CMS_SUPERUSER`. Ensures default containers (mailbox, appointments, addresses) exist.

**Parameters:**

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| `$user` | string | (Optional) User identifier. Defaults to `CMS_SUPERUSER` if not provided.    |

**Return Values:**
- None (Constructor).

**Inner Mechanisms:**
1. Sets the `user` property and initializes the `data` property to manage desktop objects stored under `#desktop/{safe_filename($user)}/desktop`.
2. Checks for existing default containers (mailbox, appointments, addresses) by iterating through the data.
3. If any default container is missing, it creates and inserts the missing container(s) into the data structure.
4. Saves the data if any changes were made.

**Usage Context:**
- Called when initializing a user's desktop environment.
- Ensures a consistent starting state with default containers.

---

### Methods

#### `desktop::object_get($index, $property)`

**Purpose:**
Retrieves the value of a specified property for a desktop object identified by `$index`.

**Parameters:**

| Name       | Type   | Description                                                                 |
|------------|--------|-----------------------------------------------------------------------------|
| `$index`   | mixed  | Identifier of the desktop object.                                           |
| `$property`| string | Property name to retrieve.                                                  |

**Return Values:**
- `mixed`: The value of the property if it exists; otherwise, `NULL` or `FALSE`.

**Inner Mechanisms:**
- Delegates the retrieval to the `data->get()` method.

**Usage Context:**
- Used to fetch properties (e.g., name, color, type) of a desktop object.

---

#### `desktop::object_set($index, $property, $value = NULL)`

**Purpose:**
Sets the value of a specified property for a desktop object identified by `$index`.

**Parameters:**

| Name       | Type   | Description                                                                 |
|------------|--------|-----------------------------------------------------------------------------|
| `$index`   | mixed  | Identifier of the desktop object.                                           |
| `$property`| string | Property name to set.                                                       |
| `$value`   | mixed  | (Optional) Value to set. Defaults to `NULL`.                                |

**Return Values:**
- `bool`: `TRUE` if the property was set successfully; `FALSE` if the object does not exist.

**Inner Mechanisms:**
- Checks if the object exists using `data->get($index)`.
- If the object exists, sets the property using `data->set()`.

**Usage Context:**
- Used to update properties of an existing desktop object.

---

#### `desktop::save()`

**Purpose:**
Saves the current state of the desktop data to persistent storage.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` if the save operation was successful; otherwise, `FALSE`.

**Inner Mechanisms:**
- Delegates the save operation to the `data->save()` method.

**Usage Context:**
- Called after making changes to the desktop objects to persist them.

---

#### `desktop::create_object($index, $type, $name)`

**Purpose:**
Creates a new desktop object of a specified type and name at a given position in the hierarchy.

**Parameters:**

| Name    | Type   | Description                                                                 |
|---------|--------|-----------------------------------------------------------------------------|
| `$index`| mixed  | Position in the hierarchy where the object will be inserted.                |
| `$type` | string | Type of the object (e.g., "link", "note", "container").                     |
| `$name` | string | Name of the object.                                                         |

**Return Values:**
- `mixed`: The identifier of the newly created object if successful; `FALSE` otherwise.

**Inner Mechanisms:**
1. Generates a color for the object based on its name using `strtocolor()`.
2. Constructs a buffer array for the object:
   - For containers, includes opening and closing tags (`#type` and `/container`).
   - For other types, includes only the object definition.
3. Inserts the object into the data structure at the specified index.
4. Saves the data if the insertion was successful.

**Usage Context:**
- Used to add new objects (e.g., links, notes, containers) to the desktop.

---

#### `desktop::move_object($source, $target)`

**Purpose:**
Moves a desktop object from one position (`$source`) to another (`$target`) in the hierarchy.

**Parameters:**

| Name     | Type   | Description                                                                 |
|----------|--------|-----------------------------------------------------------------------------|
| `$source`| mixed  | Identifier of the object to move.                                           |
| `$target`| mixed  | Target position in the hierarchy.                                           |

**Return Values:**
- `mixed`: The new identifier of the moved object if successful; `FALSE` otherwise.

**Inner Mechanisms:**
1. Cuts the object from its current position using `data->cut()`.
2. Inserts the object at the target position using `data->insert()`.
3. Saves the data if the operation was successful.

**Usage Context:**
- Used to reorganize the desktop hierarchy (e.g., moving objects between containers).

---

#### `desktop::delete_object($index)`

**Purpose:**
Deletes a desktop object identified by `$index` and performs cleanup based on the object type.

**Parameters:**

| Name    | Type   | Description                                                                 |
|---------|--------|-----------------------------------------------------------------------------|
| `$index`| mixed  | Identifier of the object to delete.                                         |

**Return Values:**
- `bool`: `TRUE` if the deletion was successful; `FALSE` otherwise.

**Inner Mechanisms:**
1. Retrieves the type of the object before deletion.
2. Deletes the object using `data->del()`.
3. Saves the data.
4. Performs type-specific cleanup:
   - For mailboxes, deletes the associated directory using `filemanager_delete()` if the `filemanager` library is loaded.

**Usage Context:**
- Used to remove objects from the desktop and clean up associated resources.

---

#### `desktop::object_type($index)`

**Purpose:**
Retrieves the type of a desktop object identified by `$index`.

**Parameters:**

| Name    | Type   | Description                                                                 |
|---------|--------|-----------------------------------------------------------------------------|
| `$index`| mixed  | Identifier of the desktop object.                                           |

**Return Values:**
- `string`: The type of the object (e.g., "link", "note", "container").

**Inner Mechanisms:**
- Delegates the retrieval to the `data->get()` method with the `#type` property.

**Usage Context:**
- Used to determine the type of an object for conditional logic (e.g., rendering, validation).

---

#### `desktop::get_parent($index)`

**Purpose:**
Retrieves the parent identifier of a desktop object identified by `$index`.

**Parameters:**

| Name    | Type   | Description                                                                 |
|---------|--------|-----------------------------------------------------------------------------|
| `$index`| mixed  | Identifier of the desktop object.                                           |

**Return Values:**
- `mixed`: The identifier of the parent object if it exists; otherwise, `FALSE`.

**Inner Mechanisms:**
- Delegates the retrieval to the `data->move("parent", $index)` method.

**Usage Context:**
- Used to navigate the desktop hierarchy (e.g., finding the container of an object).


<!-- HASH:d097a50d17f1c6e8bc570eaf0dc3737e -->
