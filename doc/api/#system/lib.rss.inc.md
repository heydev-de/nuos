# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.rss.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## RSS Module (`lib.rss.inc`)

Core RSS feed management module for the NUOS platform. Provides functionality to create, modify, and manage RSS channels and their metadata. Uses the platform's `data` class for persistent storage of channel configurations.

---

### Related Functions

#### `rss_get_default()`

**Purpose:**
Retrieves the path(s) of all RSS channels marked as default.

**Parameters:**
None

**Return Values:**
| Type | Description |
|------|-------------|
| `string` | Concatenated paths of default channels in `/key1/key2/` format. Empty string if no defaults exist. |

**Inner Mechanisms:**
- Instantiates a `data` object targeting the `#system/rss` storage.
- Iterates through all stored channels using `move("next")`.
- Checks each channel's `default` flag via `get($key, "default")`.
- Constructs a path string for each default channel.

**Usage Context:**
- Used by the platform to determine which RSS feeds should be included in global syndication.
- Typically called during feed generation or when building navigation elements that link to default feeds.

---

## `rss` Class

Primary class for RSS channel management. Encapsulates all operations related to channel creation, modification, and persistence.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the platform's `data` class, initialized to `#system/rss` storage. |

---

### Constructor

#### `__construct()`

**Purpose:**
Initializes the RSS manager with a fresh `data` instance.

**Parameters:**
None

**Return Values:**
None (constructor)

**Inner Mechanisms:**
- Instantiates a new `data` object targeting `#system/rss`.
- Assigns the instance to the `data` property.

**Usage Context:**
- Called whenever a new `rss` instance is created.
- Ensures isolation between different RSS management contexts.

---

### Methods

#### `add_channel($name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`

**Purpose:**
Creates a new RSS channel with the provided metadata.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$name` | `string` | - | Display name of the channel. |
| `$description` | `string` | - | Brief description of the channel's content. |
| `$link` | `string` | - | URL to the website or page the channel represents. |
| `$image` | `string` | `NULL` | Optional URL to an image/logo for the channel. |
| `$category` | `string` | `NULL` | Optional category or tag for the channel. |
| `$default` | `mixed` | `NULL` | If truthy, marks the channel as default. |

**Return Values:**
| Type | Description |
|------|-------------|
| `string` | The generated unique key (index) of the new channel. |

**Inner Mechanisms:**
- Constructs a single-record array with the provided metadata.
- Uses `set_buffer()` to stage the data.
- Calls `insert()` to persist the new channel and generate a key.

**Usage Context:**
- Used when setting up new content sections that require syndication.
- Called by backend modules or during site initialization.

---

#### `set_channel($index, $name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`

**Purpose:**
Updates the metadata of an existing RSS channel.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Unique key of the channel to update. |
| `$name` | `string` | - | New display name. |
| `$description` | `string` | - | New description. |
| `$link` | `string` | - | New link URL. |
| `$image` | `string` | `NULL` | New image URL. |
| `$category` | `string` | `NULL` | New category. |
| `$default` | `mixed` | `NULL` | New default flag (truthy = default). |

**Return Values:**
| Type | Description |
|------|-------------|
| `string` | The provided `$index`, confirming the update. |

**Inner Mechanisms:**
- Uses individual `set()` calls to update each field of the channel.
- Explicitly casts `$default` to boolean for consistency.

**Usage Context:**
- Used in administrative interfaces to edit channel properties.
- Called when content strategy changes require updates to syndication metadata.

---

#### `del_channel($index)`

**Purpose:**
Removes an RSS channel from the system.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Unique key of the channel to delete. |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

**Inner Mechanisms:**
- Delegates to `data->del($index)` for actual deletion.

**Usage Context:**
- Used during cleanup of deprecated content sections.
- Called from administrative interfaces when channels are no longer needed.

---

#### `save()`

**Purpose:**
Persists all staged changes to the RSS channel storage.

**Parameters:**
None

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

**Inner Mechanisms:**
- Delegates to `data->save()` to flush the buffer to permanent storage.

**Usage Context:**
- Called after a series of `add_channel`, `set_channel`, or `del_channel` operations to ensure changes are saved.
- Typically used in administrative workflows after batch updates.


<!-- HASH:8e59c2a9960917b4b989e1c6c2c81211 -->
