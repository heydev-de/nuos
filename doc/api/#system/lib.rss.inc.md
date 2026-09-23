# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.rss.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.rss.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## rss

The `rss` class provides a structured interface for managing RSS channel configurations within the PWNC Web Platform. It interacts with a data store located at `#system/rss` to create, modify, delete, and persist RSS channel definitions. Each channel includes metadata such as name, description, link, optional image, category, and a flag indicating whether it is the default channel.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$data` | `data` | `NULL` | Instance of the `data` class used to interact with the `#system/rss` data store. |

### Methods

#### `__construct()`

Initializes a new instance of the `rss` class by creating a `data` object bound to the `#system/rss` data store.

**Parameters:** None  
**Return Value:** None (constructor)  
**Inner Mechanism:** Instantiates the `data` class with the path `#system/rss`, which serves as the persistent storage backend for RSS channel definitions.  
**Usage Context:** Automatically called when a new `rss` object is instantiated. Used internally by other methods to access the data layer.

```php
$rss = new rss();
```

---

#### `add_channel($name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`

Adds a new RSS channel entry to the data store.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | The title or name of the RSS channel. |
| `$description` | `string` | A brief description of the channel content. |
| `$link` | `string` | The URL associated with the channel. |
| `$image` | `string\|NULL` | Optional URL to an image representing the channel. |
| `$category` | `string\|NULL` | Optional category classification for the channel. |
| `$default` | `bool\|NULL` | Whether this channel should be considered the default. |

**Return Value:**  
- **Type:** `mixed`  
- **Description:** The result of the `insert()` operation from the `data` class, typically the index or identifier of the newly added channel.

**Inner Mechanism:** Constructs an array containing all provided channel properties, sets it as the internal buffer of the `data` object, and inserts it into the data store. The `$default` parameter is cast to a boolean before being stored.

**Usage Context:** Called when programmatically registering a new RSS feed source in the system.

```php
$rss = new rss();
$index = $rss->add_channel(
    "Tech News",
    "Latest technology updates",
    "https://example.com/tech",
    "https://example.com/image.png",
    "Technology",
    true
);
echo "New channel added at index: $index";
```

---

#### `set_channel($index, $name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`

Updates an existing RSS channel identified by its index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The index of the channel to update. |
| `$name` | `string` | The updated name/title of the channel. |
| `$description` | `string` | The updated description. |
| `$link` | `string` | The updated URL. |
| `$image` | `string\|NULL` | Updated image URL or `NULL`. |
| `$category` | `string\|NULL` | Updated category or `NULL`. |
| `$default` | `bool\|NULL` | Updated default status. |

**Return Value:**  
- **Type:** `int`  
- **Description:** The index of the updated channel.

**Inner Mechanism:** Uses the `data` object's `set()` method to assign each property value to the specified index. The `$default` parameter is cast to a boolean.

**Usage Context:** Used when modifying an existing RSS channel configuration.

```php
$rss = new rss();
$rss->set_channel(
    0,
    "Updated Tech News",
    "Updated description",
    "https://example.com/updated-tech",
    NULL,
    "News",
    false
);
```

---

#### `del_channel($index)`

Deletes an RSS channel from the data store by its index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | The index of the channel to remove. |

**Return Value:**  
- **Type:** `bool`  
- **Description:** Result of the deletion operation (`TRUE` on success, `FALSE` on failure).

**Inner Mechanism:** Delegates to the `data` object's `del()` method to remove the record at the given index.

**Usage Context:** Used when removing an obsolete or unwanted RSS channel.

```php
$rss = new rss();
if ($rss->del_channel(2)) {
    echo "Channel deleted successfully.";
} else {
    echo "Failed to delete channel.";
}
```

---

#### `save()`

Persists any changes made to the RSS channel data store.

**Parameters:** None  
**Return Value:**  
- **Type:** `bool`  
- **Description:** Result of the save operation (`TRUE` on success, `FALSE` on failure).

**Inner Mechanism:** Calls the `save()` method on the underlying `data` object to write buffered changes to permanent storage.

**Usage Context:** Should be called after performing any add/update/delete operations to ensure changes are persisted.

```php
$rss = new rss();
$rss->add_channel("My Feed", "Description", "https://example.com/feed");
$rss->save();
```

---

## rss_get_default()

Retrieves the path of the default RSS channel from the data store.

**Parameters:** None  
**Return Value:**  
- **Type:** `string\|NULL`  
- **Description:** A concatenated string of keys representing the default channel path (e.g., `/news/local/`), or `NULL` if no default is set.

**Inner Mechanism:** Loads the `#system/rss` data store, iterates through records, and builds a path string from keys where the `default` field is truthy. If the resulting string is empty (checked via `stre()`), returns `NULL`; otherwise returns the constructed path.

**Usage Context:** Used to determine which RSS channel is marked as the default, often for routing or display purposes.

```php
$defaultPath = rss_get_default();
if ($defaultPath !== NULL) {
    echo "Default RSS path: $defaultPath";
} else {
    echo "No default RSS channel configured.";
}
```


<!-- HASH:26db33b175e931cdd0f2fb77c3edd95e -->
