# NUOS API Documentation

[← Index](../README.md) | [`#system/sys.system.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.3`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## System Class

Core system configuration manager for the NUOS platform. Provides a unified interface to read, modify, and persist system-wide settings stored in structured data files. Acts as a facade over the `data` class, abstracting file location and format details.

### Properties

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| data  | object | Instance of the `data` class responsible for low-level storage operations. |

---

### `__construct()`

Initializes the system configuration manager by loading the appropriate data file.

#### Purpose
Determines the correct configuration file path (development or production) and instantiates the underlying `data` object.

#### Parameters
None.

#### Return Values
None (constructor).

#### Inner Mechanisms
- Uses a static variable `$file` to cache the resolved file path across multiple instances.
- Checks for the existence of a development-specific file (`#system/system.dev.dat`) to decide between development and production configurations.
- Instantiates a `data` object with the resolved file path and assigns it to the `data` property.

#### Usage Context
- Automatically invoked when creating a new `system` instance.
- Should be instantiated once per request (singleton pattern recommended).
- Typical usage:
  ```php
  $system = new \cms\system();
  ```

---

### `getval()`

Retrieves a system configuration value.

#### Purpose
Fetches a specific configuration value or metadata (e.g., default, type) from the system data store.

#### Parameters

| Name      | Type   | Default | Description                                                                 |
|-----------|--------|---------|-----------------------------------------------------------------------------|
| property  | string | -       | Key or path to the desired configuration value.                            |
| option    | string | "value" | Specifies which part of the configuration entry to return (e.g., "value", "default", "type"). |

#### Return Values
- **Mixed**: The requested value or metadata. Type depends on the stored data (e.g., string, integer, array).

#### Inner Mechanisms
- Delegates the operation to the underlying `data` object's `get()` method.
- Supports nested property access via dot notation (e.g., `"database.host"`).

#### Usage Context
- Used to read system settings such as database credentials, feature flags, or paths.
- Example:
  ```php
  $dbHost = $system->getval("database.host");
  $isDebug = $system->getval("debug.enabled");
  ```

---

### `setval()`

Updates or creates a system configuration value.

#### Purpose
Modifies an existing configuration value or adds a new one to the system data store.

#### Parameters

| Name      | Type   | Default | Description                                                                 |
|-----------|--------|---------|-----------------------------------------------------------------------------|
| value     | mixed  | NULL    | The new value to store.                                                     |
| property  | string | NULL    | Key or path to the configuration entry. If `NULL`, replaces the entire data store. |
| option    | string | "value" | Specifies which part of the configuration entry to update (e.g., "value", "default"). |

#### Return Values
None.

#### Inner Mechanisms
- Delegates the operation to the underlying `data` object's `set()` method.
- Supports nested property access via dot notation.
- If `property` is `NULL`, the entire data store is replaced with the provided `value`.

#### Usage Context
- Used to dynamically update system settings (e.g., enabling maintenance mode, updating API keys).
- Example:
  ```php
  $system->setval(true, "maintenance.enabled");
  $system->setval("new_api_key", "services.api.key");
  ```

---

### `save()`

Persists the current system configuration to disk.

#### Purpose
Writes all pending changes in the system data store to the underlying configuration file.

#### Parameters
None.

#### Return Values
- **Boolean**: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
- Delegates the operation to the underlying `data` object's `save()` method.
- Handles file locking and error conditions internally.

#### Usage Context
- Must be called explicitly after one or more `setval()` operations to ensure changes are persisted.
- Example:
  ```php
  $system->setval("127.0.0.1", "database.host");
  $success = $system->save();
  ```


<!-- HASH:8a4c4b842a2512066d39d5b56fff003c -->
