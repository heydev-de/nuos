# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.desktop.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.desktop.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

#system/lib.desktop.inc

## Overview

The `desktop` class provides a personalized workspace management system for PWNC users. It manages a hierarchical data structure stored per-user under `#desktop/{user}/desktop`, representing a desktop-like environment containing containers, links, notes, appointments, addresses, and mailboxes.

The class ensures that every user's desktop contains three default containers (`mailbox`, `appointment`, `address`) upon initialization. These containers serve as organizational roots for related content types.

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DESKTOP_TYPE_NONE` | 0 | No specific type |
| `CMS_DESKTOP_TYPE_LINK` | 1 | Represents a link object |
| `CMS_DESKTOP_TYPE_NOTE` | 2 | Represents a note object |
| `CMS_DESKTOP_TYPE_APPOINTMENT` | 4 | Represents an appointment object |
| `CMS_DESKTOP_TYPE_ADDRESS` | 8 | Represents an address object |
| `CMS_DESKTOP_TYPE_CONTAINER` | 16 | Represents a container object |
| `CMS_DESKTOP_TYPE_MAILBOX` | 32 | Represents a mailbox object |
| `CMS_DESKTOP_TYPE_ALL` | 255 | All types combined |

## Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | string | NULL | The user identifier associated with this desktop instance |
| `$data` | data | NULL | A `data` object managing the desktop's hierarchical structure |

## Methods

### `__construct($user = NULL)`

Initializes a new desktop instance for the specified user.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$user` | string | NULL | User identifier; defaults to `CMS_SUPERUSER` if not provided |

**Return Value:** None

**Inner Mechanisms:**
1. Sets the user property, defaulting to `CMS_SUPERUSER`.
2. Creates a `data` object pointing to `#desktop/{user}/desktop`.
3. Iterates through existing desktop items to detect presence of mailbox, appointment, and address containers using a flag system.
4. If any of these containers are missing, creates them with appropriate names from language constants (`CMS_L_DESKTOP_001`, `CMS_L_DESKTOP_002`, `CMS_L_DESKTOP_003`) and marks them as quick-access.
5. Saves changes only if new containers were added.

**Usage Example:**
```php
// Create a desktop for user "john"
$desktop = new desktop("john");

// This will ensure john's desktop has mailbox, appointment, and address containers
```

### `object_get($index, $property)`

Retrieves a property value from a desktop object.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | string | The index/key of the object in the data structure |
| `$property` | string | The property name to retrieve |

**Return Value:** Mixed – the value of the requested property, or NULL if not found

**Inner Mechanisms:** Delegates directly to the underlying `data` object's `get()` method.

**Usage Example:**
```php
// Get the name of object at index "item1"
$name = $desktop->object_get("item1", "name");
```

### `object_set($index, $property, $value = NULL)`

Sets a property value on a desktop object.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | string | – | The index/key of the object |
| `$property` | string | – | The property name to set |
| `$value` | mixed | NULL | The value to assign |

**Return Value:** Boolean – TRUE on success, FALSE if the object doesn't exist

**Inner Mechanisms:**
1. Checks if the object exists at the given index.
2. If it exists, sets the property using the `data` object's `set()` method.
3. Returns FALSE if the object doesn't exist.

**Usage Example:**
```php
// Update the name of an existing object
$desktop->object_set("item1", "name", "Updated Name");
```

### `save()`

Persists all pending changes to the desktop data structure.

**Parameters:** None

**Return Value:** Boolean – TRUE on successful save, FALSE otherwise

**Inner Mechanisms:** Delegates to the underlying `data` object's `save()` method.

**Usage Example:**
```php
// After making multiple changes, save them
$desktop->object_set("item1", "color", "#FF0000");
$desktop->save();
```

### `create_object($index, $type, $name)`

Creates a new object in the desktop structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | string | The position/index where the object should be inserted |
| `$type` | string | The type of object (e.g., "container", "link", "note") |
| `$name` | string | The display name for the new object |

**Return Value:** Mixed – the return value from `data->insert()` on success, FALSE on failure

**Inner Mechanisms:**
1. Generates a color from the name using `strtocolor()`.
2. For containers, creates a buffer with opening and closing tags.
3. For other types, creates a simple buffer entry.
4. Inserts the buffer at the specified index.
5. Saves the data structure.
6. Returns the insertion result or FALSE on failure.

**Usage Example:**
```php
// Create a new note at the root level
$result = $desktop->create_object("root", "note", "My Important Note");

// Create a new container
$result = $desktop->create_object("root", "container", "Project Files");
```

### `move_object($source, $target)`

Moves an object from one location to another within the desktop structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$source` | string | The index of the object to move |
| `$target` | string | The destination index |

**Return Value:** Mixed – the return value from `data->insert()` on success, FALSE on failure

**Inner Mechanisms:**
1. Cuts the object from the source location.
2. Inserts it at the target location.
3. Saves the data structure.
4. Returns the insertion result or FALSE on failure.

**Usage Example:**
```php
// Move an object from one container to another
$desktop->move_object("item1", "container2");
```

### `delete_object($index)`

Deletes an object from the desktop structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | string | The index of the object to delete |

**Return Value:** Boolean – TRUE on successful deletion, FALSE on failure

**Inner Mechanisms:**
1. Retrieves the object's type before deletion.
2. Deletes the object from the data structure.
3. Saves the data structure.
4. Performs cleanup based on object type:
   - For mailboxes: loads the filemanager library and deletes the associated filesystem path.
5. Returns TRUE on success, FALSE on failure.

**Usage Example:**
```php
// Delete an object and clean up its resources
$desktop->delete_object("old_item");
```

### `object_type($index)`

Retrieves the type of a desktop object.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | string | The index of the object |

**Return Value:** String – the type of the object (e.g., "container", "link", "note")

**Inner Mechanisms:** Delegates to the underlying `data` object's `get()` method to retrieve the `#type` property.

**Usage Example:**
```php
// Check if an object is a container
if ($desktop->object_type("item1") === "container") {
    // Handle container-specific logic
}
```

### `get_parent($index)`

Retrieves the parent index of a given object.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | string | The index of the object whose parent is requested |

**Return Value:** String – the index of the parent object

**Inner Mechanisms:** Delegates to the underlying `data` object's `move()` method with the "parent" direction.

**Usage Example:**
```php
// Find the parent container of an object
$parent = $desktop->get_parent("item1");
```


<!-- HASH:564af47af8d8f89c63f95b52d11d75f9 -->
