# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.rss.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.rss.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## RSS Module (`lib.rss.inc`)

The RSS module provides functionality for managing RSS feed channels within the PWNC Web Platform. It allows creation, modification, and deletion of RSS channels stored in a structured data format. The module leverages the platform's `data` class for persistent storage and retrieval.

---

## Related Functions

### `rss_get_default()`

Retrieves the default RSS channel path(s) from the system's RSS configuration.

#### Parameters
None.

#### Return Values
| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Path string of default channel(s) (e.g., `/channel1/`), or `NULL` if none. |

#### Inner Mechanisms
- Initializes a `data` object targeting `#system/rss`.
- Iterates through all stored channels.
- Concatenates paths of channels marked as default (`default = TRUE`).
- Returns `NULL` if no default channels exist (checked via `stre()`).

#### Usage Context
- Used to determine which RSS channel(s) should be served by default.
- Typically called during feed generation to resolve the default feed path.

#### Example
```php
$defaultFeed = rss_get_default();
if ($defaultFeed) {
    header("Location: /rss$defaultFeed");
    exit;
}
```

---

## `rss` Class

Manages RSS channel configurations, including creation, modification, and persistence.

### Properties

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| `data` | object | Instance of `data` class for storage.|

---

### `__construct()`

Initializes the RSS manager with a `data` object targeting `#system/rss`.

#### Parameters
None.

#### Return Values
None.

#### Inner Mechanisms
- Instantiates a `data` object bound to the `#system/rss` storage key.
- Assigns the object to the `data` property.

#### Usage Context
- Called when creating a new `rss` instance to manage channels.

#### Example
```php
$rssManager = new \cms\rss();
```

---

### `add_channel($name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`

Adds a new RSS channel to the system.

#### Parameters

| Name          | Type    | Description                                      |
|---------------|---------|--------------------------------------------------|
| `$name`       | string  | Display name of the channel.                     |
| `$description`| string  | Description of the channel.                      |
| `$link`       | string  | Base URL of the channel (e.g., site homepage).   |
| `$image`      | string  | Optional URL of a channel image/logo.            |
| `$category`   | string  | Optional category or tag for the channel.        |
| `$default`    | bool    | Whether this channel is the default (default: `NULL`). |

#### Return Values
| Type   | Description                                      |
|--------|--------------------------------------------------|
| int    | The index (ID) of the newly inserted channel.    |

#### Inner Mechanisms
- Uses `data->set_buffer()` to prepare channel data as an associative array.
- Calls `data->insert()` to persist the new channel.
- Returns the auto-generated index.

#### Usage Context
- Used during initial setup or when adding new RSS feeds to the platform.

#### Example
```php
$rss = new \cms\rss();
$channelId = $rss->add_channel(
    "News Feed",
    "Latest updates from our platform",
    "https://pwnc.it",
    "https://pwnc.it/logo.png",
    "Technology",
    true
);
```

---

### `set_channel($index, $name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`

Updates an existing RSS channel.

#### Parameters

| Name          | Type    | Description                                      |
|---------------|---------|--------------------------------------------------|
| `$index`      | int     | ID/index of the channel to update.               |
| `$name`       | string  | New display name.                                |
| `$description`| string  | New description.                                 |
| `$link`       | string  | New base URL.                                    |
| `$image`      | string  | New image URL (optional).                        |
| `$category`   | string  | New category (optional).                         |
| `$default`    | bool    | Whether to set as default (default: `NULL`).     |

#### Return Values
| Type   | Description                                      |
|--------|--------------------------------------------------|
| int    | The provided `$index` (unchanged).               |

#### Inner Mechanisms
- Updates each field individually using `data->set()`.
- Converts `$default` to boolean for consistency.

#### Usage Context
- Used to modify existing channels (e.g., after rebranding or URL changes).

#### Example
```php
$rss = new \cms\rss();
$rss->set_channel(
    1,
    "Updated News",
    "Fresh updates and announcements",
    "https://pwnc.it/news",
    "https://pwnc.it/new-logo.png",
    "Tech News",
    true
);
```

---

### `del_channel($index)`

Deletes an RSS channel by its index.

#### Parameters

| Name     | Type | Description                          |
|----------|------|--------------------------------------|
| `$index` | int  | ID/index of the channel to delete.   |

#### Return Values
| Type    | Description                                      |
|---------|--------------------------------------------------|
| bool    | `TRUE` on success, `FALSE` on failure.           |

#### Inner Mechanisms
- Delegates deletion to `data->del()`.

#### Usage Context
- Used to remove obsolete or deprecated channels.

#### Example
```php
$rss = new \cms\rss();
$rss->del_channel(3); // Remove channel with ID 3
```

---

### `save()`

Persists all changes to the RSS channel data.

#### Parameters
None.

#### Return Values
| Type    | Description                                      |
|---------|--------------------------------------------------|
| bool    | `TRUE` on success, `FALSE` on failure.           |

#### Inner Mechanisms
- Calls `data->save()` to write buffered changes to storage.

#### Usage Context
- Must be called after modifications to ensure persistence.
- Typically used in administrative workflows.

#### Example
```php
$rss = new \cms\rss();
$rss->set_channel(1, "New Name", "Description", "https://pwnc.it");
$rss->save(); // Commit changes
```


<!-- HASH:26db33b175e931cdd0f2fb77c3edd95e -->
