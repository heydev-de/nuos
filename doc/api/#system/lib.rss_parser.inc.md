# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.rss_parser.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## RSS Parser Module

This file implements a robust RSS feed parser for the NUOS web platform. It provides classes to parse, process, and display RSS 2.0 feeds with configurable output options. The module handles XML parsing, data extraction, HTML filtering, and rendering of feed content.

---

## Classes

### `rss_parser_attribute_list`

Manages a collection of RSS feed attributes as key-value pairs.

#### Properties

| Name | Type | Description |
|------|------|-------------|
| (dynamic) | `rss_parser_attribute` | Attributes are stored as dynamic properties, each holding an `rss_parser_attribute` object. |

#### Methods

##### `add($name, $value)`

Adds a new attribute to the list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the attribute. |
| `$value` | `mixed` | Value of the attribute. |

**Inner Mechanisms:**
- Creates a new `rss_parser_attribute` object with the provided value and assigns it to the property named `$name`.

**Usage:**
- Used internally when parsing XML attributes in RSS feeds.

---

##### `__get($name)`

Retrieves an attribute by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the attribute to retrieve. |

**Return Values:**
- `rss_parser_attribute` | `NULL`: The attribute object if found; otherwise, `NULL`.

**Inner Mechanisms:**
- Checks if the requested attribute exists and returns it; otherwise, returns `NULL`.

**Usage:**
- Allows safe access to attributes without throwing errors if the attribute does not exist.

---

##### `__toString()`

Converts the attribute list to a string.

**Return Values:**
- `string`: Always returns an empty string.

**Usage:**
- Placeholder for string conversion; not intended for meaningful output.

---

---

### `rss_parser_attribute`

Represents a single RSS feed attribute.

#### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `mixed` | `""` | The value of the attribute. |

#### Constructor

##### `__construct($value)`

Initializes the attribute with a value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `mixed` | The value to assign to the attribute. |

---

#### Methods

##### `__get($name)`

Retrieves the attribute's value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Ignored; included for compatibility. |

**Return Values:**
- `mixed`: The value of the attribute.

**Usage:**
- Allows the attribute to be accessed as if it were a property.

---

##### `__toString()`

Converts the attribute's value to a string.

**Return Values:**
- `string`: The string representation of the attribute's value.

**Usage:**
- Enables implicit string conversion in string contexts.

---

---

### `rss_parser_node`

Represents a node in the parsed RSS feed XML structure.

#### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$_parent` | `rss_parser_node` | `NULL` | Parent node in the XML tree. |
| `$_name` | `string` | `""` | Name of the node. |
| `$_data` | `string` | `""` | Text content of the node. |
| `$_attrib` | `rss_parser_attribute_list` | `NULL` | List of attributes for the node. |

#### Constructor

##### `__construct($name = "")`

Initializes a new node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the node. |

**Inner Mechanisms:**
- Sets the node's name and initializes an empty attribute list.

---

#### Methods

##### `add_attribute($name, $value)`

Adds an attribute to the node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the attribute. |
| `$value` | `mixed` | Value of the attribute. |

**Inner Mechanisms:**
- Delegates to the attribute list's `add` method.

**Usage:**
- Used during XML parsing to attach attributes to nodes.

---

##### `add_node($name)`

Adds a child node to the current node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the child node. |

**Return Values:**
- `rss_parser_node`: The newly created child node.

**Inner Mechanisms:**
- Creates a new node, sets its parent to the current node, and adds it as a child.
- Handles both single and multiple child nodes of the same name by converting single nodes into arrays when necessary.

**Usage:**
- Core method for building the XML tree during parsing.

---

##### `get_parent()`

Retrieves the parent node.

**Return Values:**
- `rss_parser_node` | `NULL`: The parent node if it exists; otherwise, `NULL`.

**Usage:**
- Used during parsing to navigate back up the XML tree.

---

##### `get_path()`

Generates the full path of the node in the XML tree.

**Return Values:**
- `string`: The path from the root to the current node, using colons (`:`) as separators.

**Inner Mechanisms:**
- Traverses up the parent chain to construct the path string.

**Usage:**
- Used to validate node paths against the expected RSS structure.

---

##### `__get($name)`

Retrieves a child node by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Name of the child node to retrieve. |

**Return Values:**
- `rss_parser_node` | `array` | `NULL`: The child node, an array of child nodes, or `NULL` if not found.

**Usage:**
- Provides safe access to child nodes.

---

##### `__toString()`

Converts the node's data to a string.

**Return Values:**
- `string`: The text content of the node.

**Usage:**
- Enables implicit string conversion in string contexts.

---

---

### `rss_parser`

Main class for parsing and displaying RSS 2.0 feeds.

#### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enable_html_filter` | `bool` | `FALSE` | Enables HTML filtering for descriptions. |
| `$max_text_length` | `int` | `100` | Maximum length for text fields (e.g., titles). |
| `$max_description_length` | `int` | `400` | Maximum length for descriptions when HTML filtering is enabled. |
| `$max_item_number` | `int` | `20` | Maximum number of items to display. |
| `$show_channel` | `bool` | `TRUE` | Whether to display the channel section. |
| `$show_channel_link` | `bool` | `TRUE` | Whether to display the channel link. |
| `$show_channel_image` | `bool` | `TRUE` | Whether to display the channel image. |
| `$channel_image_max_width` | `int` | `120` | Maximum width for the channel image. |
| `$channel_image_max_height` | `int` | `120` | Maximum height for the channel image. |
| `$show_channel_title` | `bool` | `TRUE` | Whether to display the channel title. |
| `$show_channel_category` | `bool` | `TRUE` | Whether to display the channel category. |
| `$show_channel_description` | `bool` | `TRUE` | Whether to display the channel description. |
| `$show_channel_pub_date` | `bool` | `TRUE` | Whether to display the channel publication date. |
| `$show_channel_last_build_date` | `bool` | `TRUE` | Whether to display the channel last build date. |
| `$show_channel_copyright` | `bool` | `TRUE` | Whether to display the channel copyright. |
| `$show_channel_managing_editor` | `bool` | `TRUE` | Whether to display the channel managing editor. |
| `$show_channel_web_master` | `bool` | `TRUE` | Whether to display the channel web master. |
| `$show_channel_generator` | `bool` | `TRUE` | Whether to display the channel generator. |
| `$show_item` | `bool` | `TRUE` | Whether to display feed items. |
| `$show_item_link` | `bool` | `TRUE` | Whether to display item links. |
| `$show_item_title` | `bool` | `TRUE` | Whether to display item titles. |
| `$show_item_category` | `bool` | `TRUE` | Whether to display item categories. |
| `$show_item_enclosure` | `bool` | `TRUE` | Whether to display item enclosures. |
| `$show_item_description` | `bool` | `TRUE` | Whether to display item descriptions. |
| `$show_item_pub_date` | `bool` | `TRUE` | Whether to display item publication dates. |
| `$show_item_author` | `bool` | `TRUE` | Whether to display item authors. |
| `$show_item_source` | `bool` | `TRUE` | Whether to display item sources. |
| `$show_item_comments` | `bool` | `TRUE` | Whether to display item comments. |
| `$show_item_guid` | `bool` | `TRUE` | Whether to display item GUIDs. |
| `$parser` | `resource` | `NULL` | Internal XML parser resource. |
| `$data` | `rss_parser_node` | `NULL` | Root node of the parsed RSS feed. |
| `$node` | `rss_parser_node` | `NULL` | Current node during parsing. |
| `$stack` | `array` | `[]` | Stack of open tags during parsing. |
| `$structure` | `array` | (see code) | List of valid RSS 2.0 paths for validation. |

#### Constructor

##### `__construct()`

Initializes the RSS parser.

**Inner Mechanisms:**
- Creates an XML parser with UTF-8 encoding.
- Sets up element and character data handlers.
- Disables case folding to preserve tag case.

---

#### Methods

##### `parse($source)`

Parses an RSS feed from a given source.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source` | `string` | URL of the RSS feed to parse. |

**Return Values:**
- `bool`: `TRUE` if parsing succeeded; `FALSE` otherwise.

**Inner Mechanisms:**
- Checks for cached data (60-second cache lifetime).
- Loads the HTTP library for remote fetching.
- Opens the source URL and streams data in chunks.
- Parses each chunk with the XML parser.
- Caches the parsed data permanently if successful.

**Usage:**
- Primary method to parse an RSS feed from a URL.

---

##### `display()`

Renders the parsed RSS feed as HTML.

**Return Values:**
- `bool`: `TRUE` if rendering succeeded; `FALSE` if the feed is invalid or not RSS 2.0.

**Inner Mechanisms:**
- Validates that the feed is RSS 2.0.
- Outputs HTML markup for the channel and items based on display settings.
- Applies text and description filtering.
- Uses NUOS utility functions for URL generation, escaping, and media type handling.

**Usage:**
- Called after `parse()` to output the feed content.

---

##### `fd($value)`

Filters and truncates a description field.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | The description text to filter. |

**Return Values:**
- `string`: The filtered and escaped description.

**Inner Mechanisms:**
- If HTML filtering is enabled, converts HTML to plain text.
- Truncates the text to the configured maximum length.
- Escapes XML special characters.

**Usage:**
- Used internally to process description fields before display.

---

##### `ft($value)`

Filters and truncates a text field.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | The text to filter. |

**Return Values:**
- `string`: The filtered and escaped text.

**Inner Mechanisms:**
- Decodes HTML entities.
- Truncates the text to the configured maximum length.
- Escapes XML special characters.

**Usage:**
- Used internally to process text fields (e.g., titles, authors) before display.

---

##### `_start($parser, $tag, $attribute)`

Internal XML parser handler for opening tags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser resource. |
| `$tag` | `string` | Name of the opening tag. |
| `$attribute` | `array` | Attributes of the tag. |

**Inner Mechanisms:**
- Validates the tag path against the expected RSS structure.
- Creates a new node and adds it to the tree.
- Attaches valid attributes to the node.
- Pushes the tag onto the stack.

**Usage:**
- Called automatically by the XML parser during parsing.

---

##### `_data($parser, $data)`

Internal XML parser handler for character data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser resource. |
| `$data` | `string` | Character data within a tag. |

**Inner Mechanisms:**
- Appends the data to the current node's content.

**Usage:**
- Called automatically by the XML parser during parsing.

---

##### `_end($parser, $tag)`

Internal XML parser handler for closing tags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser resource. |
| `$tag` | `string` | Name of the closing tag. |

**Inner Mechanisms:**
- Validates that the closing tag matches the most recent opening tag.
- Moves back to the parent node.
- Pops the tag from the stack.

**Usage:**
- Called automatically by the XML parser during parsing.


<!-- HASH:a1aeb3f5265d36212260b38ef4ac7a88 -->
