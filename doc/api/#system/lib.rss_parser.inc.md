# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.rss_parser.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.rss_parser.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## RSS Parser Module

The `lib.rss_parser.inc` file provides a comprehensive RSS feed parser and renderer for the PWNC Web Platform. It consists of three main classes:

1. **`rss_parser_attribute_list`** – Manages collections of RSS attributes.
2. **`rss_parser_attribute`** – Represents a single RSS attribute.
3. **`rss_parser_node`** – Models an RSS node (element) with hierarchical relationships.
4. **`rss_parser`** – Core class for parsing, caching, and rendering RSS feeds.

This module enables developers to fetch, parse, and display RSS 2.0 feeds with fine-grained control over output formatting and content filtering.

---

## `rss_parser_attribute_list` Class

Manages dynamic collections of RSS attributes as key-value pairs.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| (Dynamic) | `rss_parser_attribute` | Dynamically created properties representing RSS attributes. |

### Methods

#### `add($name, $value)`

Adds a new attribute to the list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the attribute. |
| `$value` | `mixed` | Value of the attribute. |

**Inner Mechanisms:**
- Creates a new `rss_parser_attribute` instance and assigns it to a dynamic property named `$name`.

**Usage Example:**
```php
$attrList = new rss_parser_attribute_list();
$attrList->add("domain", "https://example.com");
// Access via: $attrList->domain->value
```

---

#### `__get($name)`

Retrieves an attribute by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the attribute to retrieve. |

**Return Value:**
- `rss_parser_attribute|null` – The attribute object or `NULL` if not found.

**Usage Example:**
```php
$domain = $attrList->domain; // Returns rss_parser_attribute
```

---

#### `__toString()`

Converts the attribute list to a string (empty).

**Return Value:**
- `string` – Always returns an empty string.

---

## `rss_parser_attribute` Class

Represents a single RSS attribute with a value.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$value` | `""` | The stored attribute value. |

### Constructor

#### `__construct($value)`

Initializes the attribute with a value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `mixed` | The value to store. |

---

### Methods

#### `__get($name)`

Returns the stored value regardless of the property name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Ignored; any name returns the value. |

**Return Value:**
- `mixed` – The stored value.

---

#### `__toString()`

Converts the attribute value to a string.

**Return Value:**
- `string` – The string representation of `$this->value`.

---

## `rss_parser_node` Class

Models an RSS node (element) with support for hierarchical relationships, attributes, and data.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$_parent` | `NULL` | Parent node reference. |
| `$_name` | `""` | Name of the node. |
| `$_data` | `""` | Text content of the node. |
| `$_attrib` | `NULL` | `rss_parser_attribute_list` instance for attributes. |

### Constructor

#### `__construct($name = "")`

Initializes a new node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the node. |

---

### Methods

#### `add_attribute($name, $value)`

Adds an attribute to the node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Attribute name. |
| `$value` | `mixed` | Attribute value. |

---

#### `add_node($name)`

Creates and adds a child node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the child node. |

**Return Value:**
- `rss_parser_node` – The newly created child node.

**Inner Mechanisms:**
- If a node with the same name already exists, it converts it into an array and appends the new node.

**Usage Example:**
```php
$channel = new rss_parser_node("channel");
$item = $channel->add_node("item");
```

---

#### `get_parent()`

Returns the parent node.

**Return Value:**
- `rss_parser_node|null` – Parent node or `NULL` if root.

---

#### `get_path()`

Returns the full hierarchical path of the node (e.g., `rss:channel:item`).

**Return Value:**
- `string` – Colon-separated path.

---

#### `__get($name)`

Retrieves a child node by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the child node. |

**Return Value:**
- `rss_parser_node|array|null` – Child node, array of nodes, or `NULL`.

---

#### `__toString()`

Returns the node's data as a string.

**Return Value:**
- `string` – The content of `$this->_data`.

---

## `rss_parser` Class

Core class for parsing, caching, and rendering RSS 2.0 feeds.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$enable_html_filter` | `FALSE` | Enables HTML filtering in descriptions. |
| `$max_text_length` | `100` | Maximum length for text fields (e.g., titles). |
| `$max_description_length` | `400` | Maximum length for descriptions when HTML filtering is on. |
| `$max_item_number` | `20` | Maximum number of items to display. |
| `$show_channel` | `TRUE` | Whether to display channel metadata. |
| `$show_channel_link` | `TRUE` | Whether to display channel link. |
| `$show_channel_image` | `TRUE` | Whether to display channel image. |
| `$channel_image_max_width` | `120` | Maximum width for channel image. |
| `$channel_image_max_height` | `120` | Maximum height for channel image. |
| `$show_channel_title` | `TRUE` | Whether to display channel title. |
| `$show_channel_category` | `TRUE` | Whether to display channel categories. |
| `$show_channel_description` | `TRUE` | Whether to display channel description. |
| `$show_channel_pub_date` | `TRUE` | Whether to display channel publication date. |
| `$show_channel_last_build_date` | `TRUE` | Whether to display channel last build date. |
| `$show_channel_copyright` | `TRUE` | Whether to display channel copyright. |
| `$show_channel_managing_editor` | `TRUE` | Whether to display managing editor. |
| `$show_channel_web_master` | `TRUE` | Whether to display web master. |
| `$show_channel_generator` | `TRUE` | Whether to display generator. |
| `$show_item` | `TRUE` | Whether to display items. |
| `$show_item_link` | `TRUE` | Whether to display item links. |
| `$show_item_title` | `TRUE` | Whether to display item titles. |
| `$show_item_category` | `TRUE` | Whether to display item categories. |
| `$show_item_enclosure` | `TRUE` | Whether to display item enclosures (media). |
| `$show_item_description` | `TRUE` | Whether to display item descriptions. |
| `$show_item_pub_date` | `TRUE` | Whether to display item publication date. |
| `$show_item_author` | `TRUE` | Whether to display item author. |
| `$show_item_source` | `TRUE` | Whether to display item source. |
| `$show_item_comments` | `TRUE` | Whether to display item comments link. |
| `$show_item_guid` | `TRUE` | Whether to display item GUID. |
| `$parser` | `NULL` | Internal XML parser resource. |
| `$data` | `NULL` | Root `rss_parser_node` after parsing. |
| `$node` | `NULL` | Current node during parsing. |
| `$stack` | `[]` | Stack for tracking open tags. |
| `$structure` | `array` | Valid RSS 2.0 structure paths. |

---

### Constructor

#### `__construct()`

Initializes the XML parser and sets up event handlers.

**Inner Mechanisms:**
- Creates a UTF-8 XML parser.
- Registers `_start`, `_end`, and `_data` as event handlers.
- Disables case folding for tag names.

---

### Methods

#### `parse($source)`

Parses an RSS feed from a URL.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source` | `string` | URL of the RSS feed. |

**Return Value:**
- `bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Uses `http_fopen()` to fetch the feed.
- Streams data in chunks (`CMS_HTTP_SIZE_CHUNK`).
- Validates against RSS 2.0 structure.
- Caches results for 60 seconds to reduce network load.

**Usage Example:**
```php
$parser = new rss_parser();
if ($parser->parse("https://example.com/feed.rss")) {
    $parser->display();
}
```

---

#### `display()`

Renders the parsed RSS feed as HTML.

**Return Value:**
- `bool` – `TRUE` if rendered, `FALSE` if invalid or no data.

**Inner Mechanisms:**
- Outputs semantic HTML5 with classes for styling.
- Respects all `show_*` properties to control visibility.
- Uses `cms_url()`, `x()`, and `image()` for safe output.
- Applies text and description filters (`ft()`, `fd()`).

**Usage Context:**
- Call after successful `parse()`.
- Designed for direct output in templates.

---

#### `fd($value)` – Filter Description

Filters and truncates description text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | Raw description text. |

**Return Value:**
- `string` – Filtered and escaped text.

**Inner Mechanisms:**
- If `$enable_html_filter` is `TRUE`, converts HTML to plain text.
- Truncates to `$max_description_length`.
- Escapes XML special characters.

---

#### `ft($value)` – Filter Text

Filters and truncates text fields (e.g., titles).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | Raw text. |

**Return Value:**
- `string` – Filtered and escaped text.

**Inner Mechanisms:**
- Decodes HTML entities.
- Truncates to `$max_text_length`.
- Escapes XML special characters.

---

#### `_start($parser, $tag, $attribute)`

Internal XML parser handler for opening tags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser resource. |
| `$tag` | `string` | Tag name. |
| `$attribute` | `array` | Tag attributes. |

**Inner Mechanisms:**
- Validates tag path against `$structure`.
- Creates new node and adds valid attributes.
- Pushes tag onto stack.

---

#### `_data($parser, $data)`

Internal XML parser handler for character data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser resource. |
| `$data` | `string` | Text content. |

**Inner Mechanisms:**
- Appends data to current node.

---

#### `_end($parser, $tag)`

Internal XML parser handler for closing tags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser resource. |
| `$tag` | `string` | Tag name. |

**Inner Mechanisms:**
- Validates tag matches stack.
- Moves back to parent node.
- Pops stack.

---

## Usage Example: Full RSS Feed Integration

```php
// In a PWNC module or template
$parser = new \cms\rss_parser();
$parser->max_item_number = 5;
$parser->show_channel_image = FALSE;
$parser->enable_html_filter = TRUE;

if ($parser->parse("https://news.example.com/feed")) {
    $parser->display();
} else {
    echo "<p>Could not load RSS feed.</p>";
}
```

**Explanation:**
- Creates parser instance.
- Configures to show only 5 items and hide channel image.
- Enables HTML filtering for descriptions.
- Parses and displays feed if successful.


<!-- HASH:747f5175ef4b3957dfad53d0b821e623 -->
