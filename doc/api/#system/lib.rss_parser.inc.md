# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.rss_parser.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.rss_parser.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## rss_parser_attribute_list

A container class that holds a collection of `rss_parser_attribute` objects, accessible by name as dynamic properties.

### Properties

| Name | Type | Description |
|------|------|-------------|
| *(dynamic)* | `rss_parser_attribute` | Dynamically added attributes, keyed by name |

### add($name, $value)

Adds a new attribute to the list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | The attribute name |
| `$value` | `string` | The attribute value |

**Inner mechanism:** Creates a new `rss_parser_attribute` instance and assigns it to the property named `$name`.

**Usage example:**
```php
$list = new rss_parser_attribute_list();
$list->add("version", "2.0");
echo $list->version; // Outputs: 2.0
```

### __get($name)

Magic getter to retrieve an attribute by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | The attribute name |

**Return value:** `rss_parser_attribute` if found, otherwise `NULL`.

**Usage example:**
```php
$attr = $list->version; // Returns rss_parser_attribute object or NULL
```

### __toString()

Returns an empty string when the object is cast to string.

**Return value:** `string` (always `""`)

---

## rss_parser_attribute

Represents a single XML/RSS attribute with a value.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `string` | `""` | The attribute value |

### __construct($value)

Initializes the attribute with a value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | The attribute value |

**Usage example:**
```php
$attr = new rss_parser_attribute("2.0");
echo $attr; // Outputs: 2.0
```

### __get($name)

Always returns the attribute's value regardless of the requested property name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Ignored property name |

**Return value:** `string` — the attribute value.

### __toString()

Returns the attribute value as a string.

**Return value:** `string`

---

## rss_parser_node

Represents a node in the parsed RSS/XML tree structure.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$_parent` | `rss_parser_node\|NULL` | `NULL` | Parent node reference |
| `$_name` | `string` | `""` | Tag name of this node |
| `$_data` | `string` | `""` | Text content of this node |
| `$_attrib` | `rss_parser_attribute_list` | `NULL` | List of attributes |

### __construct($name = "")

Creates a new node with the given tag name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Tag name |

**Usage example:**
```php
$node = new rss_parser_node("title");
```

### add_attribute($name, $value)

Adds an attribute to this node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Attribute name |
| `$value` | `string` | Attribute value |

### add_node($name)

Creates and appends a child node. If a node with the same name already exists, it converts the property into an array.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Child tag name |

**Return value:** `rss_parser_node` — the newly created node.

**Inner mechanism:** Checks if a property with the same name exists. If so, converts it to an array (or appends to existing array). Sets parent reference and returns the new node.

**Usage example:**
```php
$parent = new rss_parser_node("channel");
$child = $parent->add_node("title");
$child->_data = "My Feed";
```

### get_parent()

Returns the parent node.

**Return value:** `rss_parser_node\|NULL`

### get_path()

Builds a colon-separated path from root to this node.

**Return value:** `string` — e.g., `"rss:channel:title"`

**Inner mechanism:** Traverses up the parent chain, prepending each node's name.

### __get($name)

Magic getter for child nodes or attributes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Property name |

**Return value:** Mixed — the property value if set, otherwise `NULL`.

### __toString()

Returns the node's text data.

**Return value:** `string`

---

## rss_parser

Main RSS parser class that fetches, parses, and displays RSS feeds using PHP's XML parser.

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enable_html_filter` | `bool` | `FALSE` | Whether to filter HTML in descriptions |
| `$max_text_length` | `int` | `100` | Max length for text fields |
| `$max_description_length` | `int` | `400` | Max length for descriptions (when HTML filter enabled) |
| `$max_item_number` | `int` | `20` | Maximum number of items to display |
| `$show_channel` | `bool` | `TRUE` | Show channel section |
| `$show_channel_link` | `bool` | `TRUE` | Show channel link |
| `$show_channel_image` | `bool` | `TRUE` | Show channel image |
| `$channel_image_max_width` | `int` | `120` | Max width for channel image |
| `$channel_image_max_height` | `int` | `120` | Max height for channel image |
| `$show_channel_title` | `bool` | `TRUE` | Show channel title |
| `$show_channel_category` | `bool` | `TRUE` | Show channel category |
| `$show_channel_description` | `bool` | `TRUE` | Show channel description |
| `$show_channel_pub_date` | `bool` | `TRUE` | Show channel publication date |
| `$show_channel_last_build_date` | `bool` | `TRUE` | Show channel last build date |
| `$show_channel_copyright` | `bool` | `TRUE` | Show channel copyright |
| `$show_channel_managing_editor` | `bool` | `TRUE` | Show managing editor |
| `$show_channel_web_master` | `bool` | `TRUE` | Show web master |
| `$show_channel_generator` | `bool` | `TRUE` | Show generator |
| `$show_item` | `bool` | `TRUE` | Show items section |
| `$show_item_link` | `bool` | `TRUE` | Show item link |
| `$show_item_title` | `bool` | `TRUE` | Show item title |
| `$show_item_category` | `bool` | `TRUE` | Show item category |
| `$show_item_enclosure` | `bool` | `TRUE` | Show item enclosure |
| `$show_item_description` | `bool` | `TRUE` | Show item description |
| `$show_item_pub_date` | `bool` | `TRUE` | Show item publication date |
| `$show_item_author` | `bool` | `TRUE` | Show item author |
| `$show_item_source` | `bool` | `TRUE` | Show item source |
| `$show_item_comments` | `bool` | `TRUE` | Show item comments |
| `$show_item_guid` | `bool` | `TRUE` | Show item GUID |
| `$parser` | `resource` | `NULL` | XML parser resource |
| `$data` | `rss_parser_node` | `NULL` | Root parsed data node |
| `$node` | `rss_parser_node` | `NULL` | Current node being processed |
| `$stack` | `array` | `[]` | Stack of open tags |
| `$structure` | `array` | *(see below)* | Valid RSS structure paths |

### __construct()

Initializes the XML parser with UTF-8 encoding and registers element/data handlers.

**Inner mechanism:** Creates an XML parser, sets case-folding off, and binds `_start`, `_end`, and `_data` methods as handlers.

**Usage example:**
```php
$parser = new rss_parser();
```

### parse($source)

Fetches and parses an RSS feed from a URL.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source` | `string` | URL of the RSS feed |

**Return value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanism:**
1. Loads HTTP library via `cms_load("http")`.
2. Checks cache for recent parse (60-second delay).
3. Initializes root node and stack.
4. Opens remote source with `http_fopen`.
5. Parses data in chunks using `xml_parse`.
6. Caches result permanently.

**Usage example:**
```php
$parser = new rss_parser();
if ($parser->parse("https://example.com/feed.xml")) {
    $parser->display();
}
```

### display()

Outputs the parsed RSS feed as HTML.

**Return value:** `bool` — `TRUE` if output was generated, `FALSE` if invalid RSS version.

**Inner mechanism:**
1. Validates RSS version is "2.0".
2. Outputs channel section (title, link, image, description, dates, etc.).
3. Outputs item list (title, link, category, enclosure, description, dates, author, source, comments, GUID).
4. Uses `fd()` and `ft()` for filtering text content.
5. Applies HTML escaping via `x()` and URL generation via `cms_url()`.

**Usage example:**
```php
$parser = new rss_parser();
$parser->parse("https://example.com/feed.xml");
$parser->display();
```

### fd($value)

Filters description content.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | Raw description text |

**Return value:** `string` — filtered and escaped description.

**Inner mechanism:**
1. If HTML filter disabled, returns value unchanged.
2. Converts HTML to plain text via `htmltoplain`.
3. Truncates to `max_description_length` with ellipsis.
4. Escapes XML special characters via `x()`.

### ft($value)

Filters text content (titles, categories, etc.).

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | Raw text |

**Return value:** `string` — filtered and escaped text.

**Inner mechanism:**
1. Decodes HTML entities.
2. Truncates to `max_text_length` with ellipsis.
3. Escapes XML special characters via `x()`.

### _start($parser, $tag, $attribute)

XML start element handler.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser |
| `$tag` | `string` | Element tag name |
| `$attribute` | `array` | Element attributes |

**Inner mechanism:**
1. Builds current path from node hierarchy.
2. Ignores tags not in `$structure`.
3. Creates new node and sets as current.
4. Adds valid attributes to node.
5. Pushes tag onto stack.

### _data($parser, $data)

XML character data handler.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser |
| `$data` | `string` | Text content |

**Inner mechanism:** Appends data to current node's `_data` property.

### _end($parser, $tag)

XML end element handler.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser |
| `$tag` | `string` | Element tag name |

**Inner mechanism:**
1. Ignores mismatched closing tags.
2. Moves to parent node.
3. Pops tag from stack.


<!-- HASH:124451e25c4fe3a464b0b2c25b2734d9 -->
