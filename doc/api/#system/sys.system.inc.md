# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.system.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.system.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## system

The `system` class is a core component of the PWNC Web Platform's system layer. It provides a high-level interface for managing system-wide configuration values and settings through a persistent data store. The class acts as a wrapper around the `data` class, abstracting the complexity of file-based data persistence and retrieval.

### Properties

| Name | Type | Default | Description |
|-------|------|---------|-------------|
| `$data` | `data` | `NULL` | Instance of the `data` class used for storing and retrieving system configuration values |

### __construct

#### Description
Initializes the `system` class by creating a new `data` instance. The constructor determines which data file to use based on whether a development configuration file exists. If `system.dev.dat` is found in the `CMS_DATA_PATH` directory, it uses the development configuration; otherwise, it defaults to the production configuration.

#### Parameters
None

#### Return Values
None

#### Inner Mechanisms
1. Uses a static variable `$file` to cache the determined file path across multiple instantiations
2. Checks for the existence of a development data file using `is_file()`
3. Constructs the appropriate file path based on the check result
4. Creates a new `data` instance with the determined file path

#### Usage Context
This constructor is automatically called when a new `system` object is instantiated. It's typically used during application initialization to load system configuration.

#### Example
```php
// Create a system instance (automatically loads appropriate config)
$system = new \cms\system();
// The system will automatically use development config if available,
// otherwise falls back to production config
```

### getval

#### Description
Retrieves a system configuration value by property name. This method delegates to the underlying `data` object's `get` method, providing a simple interface for accessing system settings.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$property` | `string` | Required | The name of the system property to retrieve |
| `$option` | `string` | `"value"` | The option type to retrieve (e.g., "value", "default", etc.) |

#### Return Values
Mixed - Returns the value of the requested property, or `NULL` if the property doesn't exist

#### Inner Mechanisms
1. Delegates directly to `$this->data->get($property, $option)`
2. The `data` class handles the actual retrieval from the persistent storage
3. The `$option` parameter allows for different retrieval modes (value, default, etc.)

#### Usage Context
Used throughout the application to retrieve system configuration values such as site settings, feature flags, or other global parameters.

#### Example
```php
$system = new \cms\system();
// Get the site title
$siteTitle = $system->getval('site_title');
// Get a property with a specific option
$propertyValue = $system->getval('maintenance_mode', 'value');
```

### setval

#### Description
Sets a system configuration value for a given property. This method delegates to the underlying `data` object's `set` method, allowing for modification of system settings.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `mixed` | `NULL` | The value to set for the property |
| `$property` | `string` | `NULL` | The name of the system property to set |
| `$option` | `string` | `"value"` | The option type to set (e.g., "value", "default", etc.) |

#### Return Values
Mixed - Returns the result of the underlying `data` object's `set` method

#### Inner Mechanisms
1. Delegates directly to `$this->data->set($value, $property, $option)`
2. The `data` class handles the actual storage of the value
3. Changes are not persisted until `save()` is called

#### Usage Context
Used when modifying system configuration values, such as updating site settings or changing feature flags.

#### Example
```php
$system = new \cms\system();
// Set a new site title
$system->setval('My New Site Title', 'site_title');
// Set a boolean flag
$system->setval(true, 'maintenance_mode');
// Persist the changes
$system->save();
```

### save

#### Description
Persists any changes made to the system configuration to permanent storage. This method delegates to the underlying `data` object's `save` method.

#### Parameters
None

#### Return Values
Boolean - Returns `TRUE` on successful save, `FALSE` on failure

#### Inner Mechanisms
1. Delegates directly to `$this->data->save()`
2. The `data` class handles the actual file writing operation
3. Ensures all pending changes are written to disk

#### Usage Context
Called after making changes with `setval()` to ensure configuration changes are persisted across requests.

#### Example
```php
$system = new \cms\system();
// Modify a system setting
$system->setval('https://example.com', 'base_url');
// Save the changes to persistent storage
if ($system->save()) {
    echo "System configuration updated successfully";
} else {
    echo "Failed to save system configuration";
}
```


<!-- HASH:066a7b00119e905867263a071fdd46cb -->
