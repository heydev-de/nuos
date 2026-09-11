# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.yaml.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.yaml.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

The file `#system/lib.yaml.inc` provides a pure-PHP implementation of a YAML 1.2 parser for the PWNC Web Platform. It implements the YAML 1.2 specification's core schema, supporting block and flow collections, quoted and plain scalars, block scalars (literal and folded), anchors, aliases, tags, directives, and multi-document streams.

The parser is implemented as a static class `yaml_parser` with a companion function `yaml_parse()` that serves as a convenience wrapper. The parser operates on a normalized string input (line breaks converted to `\n`, BOM stripped) and maintains its position via static properties, making it a single-pass, stateful parser.

### Key Design Patterns

- **Static state**: All parser state (position, source, registries) is stored in static properties, allowing method calls without object instantiation.
- **Recursive descent**: The parser uses recursive method calls to handle nested structures (mappings, sequences, flow collections).
- **Error propagation**: Errors are set via `self::$error` and `self::$errmsg`, with early returns of `FALSE` to propagate failures up the call stack.
- **Context flags**: Parsing context (BLOCK, FLOW, NO_INDENT) is passed as bitmask flags to control behavior in shared methods.

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `TAG_PREFIX` | `"tag:yaml.org,2002:"` | Core schema tag prefix used for standard YAML types |
| `CHOMP_CLIP` | `0` | Chomping mode: keep the final line break |
| `CHOMP_STRIP` | `1` | Chomping mode: remove all trailing line breaks |
| `CHOMP_KEEP` | `2` | Chomping mode: keep all trailing line breaks |
| `BLOCK` | `1` | Context flag: parser is in a block collection context |
| `FLOW` | `2` | Context flag: parser is inside a flow collection (`[]` or `{}`) |
| `NO_INDENT` | `4` | Context flag: sequence entries may share the parent node's indent |

## Static Properties

| Property | Type | Description |
|----------|------|-------------|
| `$source` | `string` | The normalized input document being parsed |
| `$length` | `int` | Total byte length of the source string |
| `$pos` | `int` | Current byte position within the source |
| `$tag` | `array` | Tag handle registry (maps handles like `!!` to full prefixes) |
| `$anchor` | `array` | Anchor registry (maps anchor names to parsed values) |
| `$error` | `bool` | Error flag; set to `TRUE` when a parse error occurs |
| `$errmsg` | `string` | Human-readable error message with line number |

## yaml_parse

### `yaml_parse($input)`

A convenience function that delegates to `yaml_parser::parse()`.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$input` | `string` | The YAML document string to parse |

**Return Values**

| Type | Description |
|------|-------------|
| `array` | Parsed result as a PHP array (or scalar/null for single-value documents) |
| `FALSE` | Parse error occurred; check `yaml_parser::$errmsg` for details |

**Inner Mechanisms**

Normalizes line endings and strips BOM before delegating to the static `parse()` method. This is the primary entry point for consumers of the library.

**Usage Example**

```php
$yaml = "name: John\nage: 30\nactive: true";
$result = yaml_parse($yaml);
// $result = ["name" => "John", "age" => 30, "active" => true]
```

## yaml_parser

### `parse($input)`

The main entry point for parsing a YAML stream. Handles document directives, multi-document streams, and orchestrates the recursive descent parsing.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$input` | `string` | Raw YAML input string |

**Return Values**

| Type | Description |
|------|-------------|
| `array` | Array of parsed documents in stream order |
| `FALSE` | Parse error; `self::$errmsg` contains the reason |

**Inner Mechanisms**

1. **Normalization**: Converts `\r\n` and `\r` to `\n`, strips UTF-8 BOM if present.
2. **State reset**: Initializes `$source`, `$length`, `$pos`, `$error`, and `$errmsg`.
3. **Document loop**: Iterates over documents in the stream. Each document may have its own `%YAML` and `%TAG` directives.
4. **Directive parsing**: Processes `%YAML` (version) and `%TAG` (handle-to-prefix mapping) directives. Directives require an explicit document start marker (`---`).
5. **Document start**: Handles `---` (document start), `...` (document end), and implicit document starts.
6. **Node parsing**: Delegates to `parse_node()` with appropriate context flags.
7. **Stream end**: Detects `...` or end-of-input to terminate the document loop.

**Usage Example**

```php
// Single document
$result = yaml_parser::parse("key: value");
// $result = [["key" => "value"]]

// Multi-document stream
$stream = "---\nname: doc1\n---\nname: doc2";
$result = yaml_parser::parse($stream);
// $result = [["name" => "doc1"], ["name" => "doc2"]]
```

### `parse_node($indent, $context, $inherit)`

Parses a single YAML node, which can be a scalar, sequence, mapping, alias, or empty value. This is the central dispatch method for all node types.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$indent` | `int` | — | The indentation level of the parent context |
| `$context` | `int` | `0` | Bitmask of context flags (BLOCK, FLOW, NO_INDENT) |
| `$inherit` | `mixed` | `NULL` | Tag inherited from parent context (for block scalars) |

**Return Values**

| Type | Description |
|------|-------------|
| `mixed` | The parsed PHP value (array, string, int, float, bool, null) |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. **Property processing**: Reads node properties (`!` tags and `&` anchors) in a loop, ensuring no duplicates.
2. **Flow context**: If inside a flow collection, checks for empty nodes and delegates to `parse_flow()`.
3. **Block context**: If at line end, reads content from below via `parse_below()`. Otherwise, checks for block collection indicators (`-`, `?`, `:`).
4. **Indicator-based nodes**: Dispatches based on the first character:
   - `*` → alias (resolves from `$anchor` registry)
   - `[` or `{` → flow collection
   - `"` or `'` → quoted scalar
   - `|` or `>` → block scalar
   - default → plain scalar
5. **Resolution**: Passes the parsed text through `resolve()` for type conversion.

**Usage Example**

```php
// Called internally by parse() and other methods
// Not typically called directly by consumers
```

### `parse_property()`

Parses a YAML node property — either a tag (`!tag`) or an anchor (`&anchor`).

**Parameters**

None.

**Return Values**

| Type | Description |
|------|-------------|
| `string` | The parsed tag or anchor name |
| `FALSE` | Parse error |

**Inner Mechanisms**

- **Anchor (`&`)**: Reads a token (alphanumeric name) following the `&` character.
- **Verbatim tag (`!<...>`)**: Reads the content between `<` and `>`, validating it contains no spaces.
- **Shorthand tag (`!tag` or `!!tag`)**: Resolves the handle prefix from the `$tag` registry. If the tag starts with the core schema prefix, returns the full tag URI; otherwise returns empty string (non-specific tag).

**Usage Example**

```php
// Called internally when parsing node properties
// e.g., for input "&myanchor value", returns "myanchor"
```

### `register_anchor($anchor, $value)`

Registers an anchor name to its parsed value in the `$anchor` registry, enabling alias resolution.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$anchor` | `string\|NULL` | The anchor name, or `NULL` if no anchor |
| `$value` | `mixed` | The parsed value to associate with the anchor |

**Return Values**

| Type | Description |
|------|-------------|
| `mixed` | The `$value` parameter (unchanged) |

**Inner Mechanisms**

If `$anchor` is not `NULL`, stores `$value` in `self::$anchor[$anchor]`. Always returns `$value` to allow method chaining in the calling code.

**Usage Example**

```php
// Called internally after parsing a node with an anchor
// e.g., for input "&a [1, 2, 3]", registers "a" => [1, 2, 3]
```

### `empty_node($tag)`

Creates an empty node value based on the tag type.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$tag` | `string\|NULL` | The resolved tag URI, or `NULL`/empty for untagged nodes |

**Return Values**

| Type | Description |
|------|-------------|
| `array` | Empty array `[]` for `seq` or `map` tags |
| `mixed` | Resolved empty scalar (NULL, bool, etc.) via `resolve()` |

**Inner Mechanisms**

If the tag indicates a collection type (`seq` or `map`), returns an empty array. Otherwise, delegates to `resolve("", $tag, TRUE)` to produce the appropriate empty scalar value (typically `NULL`).

**Usage Example**

```php
// Called internally for nodes with no content
// e.g., for input "key:" (empty value), returns NULL
```

### `parse_below($indent, $anchor, $tag)`

Attempts to parse a node from content on lines below the current position.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$indent` | `int` | — | Parent indentation level |
| `$anchor` | `string\|NULL` | `NULL` | Anchor name from current node (if any) |
| `$tag` | `string\|NULL` | `NULL` | Tag from current node (if any) |

**Return Values**

| Type | Description |
|------|-------------|
| `mixed` | Parsed value from below, or `NULL` if no deeper content |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. Advances to the next line; returns `NULL` if no more content.
2. Checks if the next line is deeper than `$indent`; returns `NULL` if not (different node).
3. Handles the case where anchor or tag properties are repeated on the next line (only allowed for keys).
4. Delegates to `parse_node()` with the inherited tag.

**Usage Example**

```php
// Called internally when a node's content appears on subsequent lines
// e.g., for multi-line plain scalars or block collections
```

### `parse_sequence($indent)`

Parses a YAML block sequence (list of items prefixed with `-`).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$indent` | `int` | The indentation level of the sequence entries |

**Return Values**

| Type | Description |
|------|-------------|
| `array` | Sequential array of parsed entries |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. Loops over sequence entries, each starting with `-`.
2. For each entry, skips the `-` indicator and whitespace, then parses the node content.
3. If the entry is at line end, delegates to `parse_below()` to read content from subsequent lines.
4. Uses `next_entry()` to advance to the next entry and check if it belongs to the same sequence.
5. Stops when the next entry doesn't match the current indent or isn't a sequence indicator.

**Usage Example**

```php
// Called internally for block sequences
// Input: "- one\n- two\n- three"
// Returns: ["one", "two", "three"]
```

### `parse_mapping($indent)`

Parses a YAML block mapping (key-value pairs).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$indent` | `int` | The indentation level of the mapping entries |

**Return Values**

| Type | Description |
|------|-------------|
| `array` | Associative array of key-value pairs |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. Loops over mapping entries.
2. **Explicit key** (`? key`): Parses the key as a full node, then optionally reads the value after `:`.
3. **Implicit key**: Locates the `:` separator using `key_end()`, parses the key as a node, then reads the value.
4. Converts keys to PHP array keys via `to_array_key()`.
5. Checks for duplicate keys.
6. Uses `next_entry()` to advance between entries.

**Usage Example**

```php
// Called internally for block mappings
// Input: "name: John\nage: 30"
// Returns: ["name" => "John", "age" => 30]
```

### `parse_mapping_value($indent, $context)`

Parses the value portion of a mapping entry, handling both inline and block-style values.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$indent` | `int` | The indentation level of the mapping |
| `$context` | `int` | Context flags (BLOCK, NO_INDENT) |

**Return Values**

| Type | Description |
|------|-------------|
| `mixed` | The parsed value, or `NULL` if empty |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. If content exists on the current line, parses it as a node with `NO_INDENT` context.
2. If at line end, advances to the next line and checks indentation:
   - If the next line is a sequence at the same indent, parses it as a block sequence.
   - If deeper, parses as a block node.
   - If not deeper, returns `NULL` (empty value).

**Usage Example**

```php
// Called internally after parsing a mapping key
// For input "key:\n  nested: value", parses the nested mapping
```

### `parse_flow($indent)`

Parses a YAML flow collection (inline `[]` or `{}`).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$indent` | `int` | The indentation level of the flow collection |

**Return Values**

| Type | Description |
|------|-------------|
| `array` | Parsed flow collection (sequential or associative) |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. Determines whether it's a mapping (`{}`) or sequence (`[]`).
2. Loops over entries separated by commas.
3. **Explicit key** (`? key`): Parses key as a full node, then optionally reads value after `:`.
4. **Implicit key**: Parses key node, checks for `:` to determine if it's a key-value pair.
5. Handles JSON-like syntax within flow collections (quoted strings, nested collections).
6. Validates that content is properly indented relative to the flow collection.
7. Converts keys to PHP array keys and checks for duplicates.

**Usage Example**

```php
// Called internally for flow collections
// Input: "[1, 2, 3]" → [1, 2, 3]
// Input: "{name: John, age: 30}" → ["name" => "John", "age" => 30]
```

### `parse_plain($indent, $context)`

Parses a YAML plain scalar (unquoted text).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$indent` | `int` | The indentation level of the parent context |
| `$context` | `int` | Context flags (BLOCK, FLOW) |

**Return Values**

| Type | Description |
|------|-------------|
| `string` | The parsed plain scalar text |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. Processes the current line character by character, stopping at:
   - Comments (`#`)
   - Flow indicators (`,`, `[`, `]`, `{`, `}`) in flow context
   - Colons followed by space (mapping separator)
   - Line breaks
2. Continues to subsequent lines if they are deeper indented (block context) or not flow indicators (flow context).
3. Folds single line breaks into spaces; preserves blank lines as multiple newlines.
4. Strips trailing whitespace from each line segment.

**Usage Example**

```php
// Called internally for plain scalars
// Input: "Hello world" → "Hello world"
// Input: "multi\n  line" → "multi line"
```

### `parse_quoted($char, $indent)`

Parses a YAML quoted scalar (double or single quotes).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$char` | `string` | The quote character (`"` or `'`) |
| `$indent` | `int` | The indentation level of the parent context |

**Return Values**

| Type | Description |
|------|-------------|
| `string` | The parsed and unescaped scalar text |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. **Double-quoted strings**: Processes escape sequences (`\n`, `\t`, `\xNN`, `\uNNNN`, `\UNNNNNNNN`, named escapes like `\e`).
2. **Single-quoted strings**: Handles doubled-quote escaping (`''` → `'`).
3. **Line folding**: When a quoted string spans multiple lines, folds line breaks:
   - Single line break → space
   - Blank lines → preserved newlines
4. **Escaped line breaks**: A backslash at end of line removes the line break.
5. Validates that continuation lines are properly indented and don't cross document markers.

**Usage Example**

```php
// Called internally for quoted scalars
// Input: '"Hello\nWorld"' → "Hello\nWorld" (with actual newline)
// Input: "'It''s here'" → "It's here"
```

### `parse_block_scalar($indent, $style)`

Parses a YAML block scalar (`|` literal or `>` folded).

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$indent` | `int` | The indentation level of the parent context |
| `$style` | `string` | The block scalar indicator (`|` or `>`) |

**Return Values**

| Type | Description |
|------|-------------|
| `string` | The parsed block scalar text |
| `FALSE` | Parse error |

**Inner Mechanisms**

1. **Header parsing**: Reads optional chomping indicator (`+`, `-`) and explicit indentation indicator (digit `1`-`9`).
2. **Content collection**: Iterates over lines, collecting content at the determined indentation level.
3. **Blank line handling**: Tracks blank lines and their width relative to content indentation.
4. **Chomping**:
   - `CHOMP_CLIP` (default): Keeps one final line break.
   - `CHOMP_STRIP` (`-`): Removes all trailing line breaks.
   - `CHOMP_KEEP` (`+`): Preserves all trailing line breaks.
5. **Folding**: For `>` style, joins lines via `fold_lines()`; for `|` style, joins with `\n`.

**Usage Example**

```php
// Called internally for block scalars
// Input: "|2\n  Hello\n  World" → "Hello\nWorld\n"
// Input: ">2\n  Hello\n  World" → "Hello World\n"
```

### `fold_lines($array)`

Folds an array of block scalar lines into a single string, applying YAML's line folding rules.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array` | Array of string lines from a folded block scalar |

**Return Values**

| Type | Description |
|------|-------------|
| `string` | The folded text |

**Inner Mechanisms**

1. Iterates over lines, tracking blank line counts and indentation state.
2. **Single line break** → space (when neither line is indented).
3. **Blank lines** → preserved as newlines.
4. **Indented lines** → prevent folding (newlines preserved).
5. Leading blank lines are treated as content.

**Usage Example**

```php
// Called internally by parse_block_scalar for ">" style
// Input: ["Hello", "", "World"] → "Hello\n\nWorld"
```

### `resolve($value, $tag, $is_plain)`

Resolves a scalar value to its appropriate PHP type based on tag and plain scalar rules.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The raw scalar text |
| `$tag` | `string\|NULL` | The resolved tag URI, or `NULL`/empty |
| `$is_plain` | `bool` | Whether the scalar was plain (unquoted) |

**Return Values**

| Type | Description |
|------|-------------|
| `NULL` | For null tags or null-like plain values (`~`, `null`, `NULL`) |
| `bool` | For bool tags or bool-like plain values (`true`, `false`, etc.) |
| `int` | For int tags or integer-like plain values |
| `float` | For float tags or float-like plain values (including `.inf`, `.nan`) |
| `string` | For string tags, quoted scalars, or non-numeric plain scalars |
| `FALSE` | Parse error (invalid tagged value) |

**Inner Mechanisms**

1. **Explicit tag**: If a tag is present, resolves based on the tag's type suffix:
   - `null` → `NULL`
   - `bool` → `TRUE`/`FALSE` (validates value)
   - `int`/`float` → numeric conversion via `to_number()`
   - `seq`/`map` → error (scalar tag on collection)
   - Other → returns string as-is
2. **Quoted scalars**: Always returned as strings.
3. **Plain scalars**: Applies default YAML 1.2 core schema resolution:
   - Null-like: `""`, `~`, `null`, `Null`, `NULL` → `NULL`
   - Bool-like: `true`, `True`, `TRUE`, `false`, `False`, `FALSE` → `bool`
   - Numeric: via `to_number()`
   - Otherwise: string

**Usage Example**

```php
// Called internally after parsing any scalar
// resolve("30", NULL, TRUE) → 30 (int)
// resolve("true", NULL, TRUE) → TRUE (bool)
// resolve("hello", NULL, TRUE) → "hello" (string)
// resolve("3.14", "tag:yaml.org,2002:float", FALSE) → 3.14 (float)
```

### `check_tag($tag, $kind)`

Validates that a tag is compatible with the expected collection kind.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$tag` | `string\|NULL` | The resolved tag URI, or `NULL`/empty |
| `$kind` | `string` | Expected collection kind: `"seq"` or `"map"` |

**Return Values**

| Type | Description |
|------|-------------|
| `TRUE` | Tag is valid for the given collection kind |
| `FALSE` | Parse error (tag mismatch) |

**Inner Mechanisms**

1. Extracts the type suffix from the tag (e.g., `seq` from `tag:yaml.org,2002:seq`).
2. If the tag is a scalar type (`null`, `bool`, `int`, `float`, `str`), returns an error (scalar tag on collection).
3. If the tag is a collection type (`seq` or `map`), checks that it matches `$kind`.
4. Non-core-schema tags (empty or non-matching prefix) are always accepted.

**Usage Example**

```php
// Called internally before parsing block/flow collections
// check_tag("tag:yaml.org,2002:seq", "seq") → TRUE
// check_tag("tag:yaml.org,2002:map", "seq") → FALSE (error)
```

### `to_number($value, $type)`

Converts a string to a PHP number (int or float) following YAML 1.2 core schema rules.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The numeric string to convert |
| `$type` | `string\|NULL` | Expected type: `"int"`, `"float"`, or `NULL` for auto-detection |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | For integer values (decimal, octal `0o`, hexadecimal `0x`) |
| `float` | For float values, or when `$type` is `"float"` |
| `INF` | For `.inf` / `+.inf` / `-.inf` (float context only) |
| `NAN` | For `.nan` (float context only) |
| `NULL` | If the value doesn't match any number pattern |

**Inner Mechanisms**

1. Rejects empty strings or values not starting with a numeric character.
2. **Float specials** (only when `$type !== "int"`): `.inf`, `.Inf`, `.INF`, `+.inf`, `-.inf`, `.nan`, `.NaN`, `.NAN`.
3. **Decimal integers**: Matches `^[+-]?[0-9]+$`; uses `+` prefix to widen past `PHP_INT_MAX`.
4. **Octal**: Matches `^0o[0-7]+$`; uses `octdec()`.
5. **Hexadecimal**: Matches `^0x[0-9a-fA-F]+$`; uses `hexdec()`.
6. **Floats**: Matches standard float regex with optional exponent.
7. Returns `NULL` if no pattern matches.

**Usage Example**

```php
// Called internally by resolve()
// to_number("42", NULL) → 42 (int)
// to_number("3.14", NULL) → 3.14 (float)
// to_number(".inf", "float") → INF
// to_number("0xFF", NULL) → 255 (int)
// to_number("0o17", NULL) → 15 (int)
```

### `to_base_number($value, $base, $type)`

Converts a base-prefixed numeric string (octal or hexadecimal) to a PHP number.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The numeric string (e.g., `"0o17"` or `"0xFF"`) |
| `$base` | `int` | The numeric base: `8` for octal, `16` for hexadecimal |
| `$type` | `string\|NULL` | Expected type: `"int"`, `"float"`, or `NULL` |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | For integer results |
| `float` | For float results or when overflow occurs |

**Inner Mechanisms**

1. Strips the base prefix (`0o` or `0x`) to get the digit string.
2. Uses `octdec()` for base 8 or `hexdec()` for base 16.
3. Returns as `float` if `$type` is `"float"` or if the result overflows to float.

**Usage Example**

```php
// Called internally by to_number()
// to_base_number("0o17", 8, NULL) → 15 (int)
// to_base_number("0xFF", 16, NULL) → 255 (int)
```

### `to_array_key($value)`

Converts a parsed key value to a valid PHP array key.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `mixed` | The parsed key value |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | For integer keys |
| `string` | For string, boolean, float, or array keys |
| `string` | Empty string `""` for `NULL` keys |

**Inner Mechanisms**

1. `NULL` → `""` (empty string)
2. `integer` → returned as-is
3. `boolean` → `"true"` or `"false"`
4. `double` → string representation
5. `array` → JSON-encoded string
6. `string` → returned as-is

**Usage Example**

```php
// Called internally by parse_mapping() and parse_flow()
// to_array_key(NULL) → ""
// to_array_key(true) → "true"
// to_array_key(42) → 42
```

## Position and Utility Methods

### `char($offset)`

Returns the character at the current position plus an optional offset.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$offset` | `int` | `0` | Byte offset from current position |

**Return Values**

| Type | Description |
|------|-------------|
| `string` | The character at the position, or `""` if out of bounds |

**Inner Mechanisms**

Calculates `self::$pos + $offset` and returns the character at that position in `self::$source`, or `""` if the position is outside the valid range.

**Usage Example**

```php
// Called internally throughout the parser
// self::$pos = 5; self::char() → source[5]
// self::char(1) → source[6]
```

### `token()`

Reads a token (sequence of non-delimiter characters) from the current position.

**Parameters**

None.

**Return Values**

| Type | Description |
|------|-------------|
| `string` | The token text |

**Inner Mechanisms**

Advances `self::$pos` past characters that are not in the delimiter set (`\t\n ,[]{}`) and returns the substring. Used for reading anchor names, tag handles, and other identifiers.

**Usage Example**

```php
// Called internally by parse_property()
// For input "&my_anchor value", after consuming "&", returns "my_anchor"
```

### `eol($pos)`

Finds the end-of-line position (next newline) from a given position.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$pos` | `int\|NULL` | `NULL` | Starting position, defaults to `self::$pos` |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | Position of the next `\n`, or `self::$length` if not found |

**Inner Mechanisms**

Uses `strpos()` to find the next newline character from the given position. Returns the source length if no newline is found.

**Usage Example**

```php
// Called internally to determine line boundaries
// self::$source = "hello\nworld"; self::eol(0) → 5
```

### `indent_end($pos)`

Finds the end of leading spaces (indentation) from a given position.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$pos` | `int\|NULL` | `NULL` | Starting position, defaults to `self::$pos` |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | Position after all leading spaces |

**Inner Mechanisms**

Uses `strspn()` to skip space characters (` `) from the given position. Note: tabs are not counted as indentation.

**Usage Example**

```php
// Called internally to skip indentation
// For "  hello", self::indent_end(0) → 2
```

### `space_end($pos)`

Finds the end of all whitespace (spaces and tabs) from a given position.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$pos` | `int\|NULL` | `NULL` | Starting position, defaults to `self::$pos` |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | Position after all leading whitespace |

**Inner Mechanisms**

Uses `strspn()` to skip both space and tab characters from the given position.

**Usage Example**

```php
// Called internally to skip whitespace
// For " \t hello", self::space_end(0) → 3
```

### `line_indent($pos)`

Calculates the indentation level of the line containing a given position.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$pos` | `int\|NULL` | `NULL` | Position on the line, defaults to `self::$pos` |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | Number of leading spaces on the line |

**Inner Mechanisms**

1. Backtracks from `$pos` to the start of the line (previous `\n`).
2. Counts leading space characters using `strcspn()` (stops at tab, which is not valid indentation).

**Usage Example**

```php
// Called internally to determine nesting levels
// For "  hello", self::line_indent(5) → 2
```

### `skip_space()`

Advances the current position past all whitespace (spaces and tabs).

**Parameters**

None.

**Return Values**

None (modifies `self::$pos`).

**Inner Mechanisms**

Sets `self::$pos` to the result of `self::space_end()`.

**Usage Example**

```php
// Called internally to skip whitespace between tokens
```

### `skip_blank()`

Advances the current position past all blank lines, whitespace, and comments.

**Parameters**

None.

**Return Values**

None (modifies `self::$pos`).

**Inner Mechanisms**

1. Loops while within the source length.
2. Skips whitespace characters (`\t`, `\n`, ` `).
3. Skips comments (`#`) only if they are properly separated (via `at_comment()`).
4. Stops at the first content character.

**Usage Example**

```php
// Called internally to skip blank lines and comments
// For "  \n  # comment\n  value", advances to "value"
```

### `next_line()`

Advances to the next non-blank, non-comment line.

**Parameters**

None.

**Return Values**

| Type | Description |
|------|-------------|
| `TRUE` | Successfully advanced to a new line |
| `FALSE` | No more lines (end of input) |

**Inner Mechanisms**

1. Loops while within the source length.
2. Skips indentation (spaces).
3. Handles tabs: if a tab follows indentation, checks if the line is blank or a comment; otherwise returns `TRUE` (tab is content).
4. Handles newlines: skips to the next line.
5. Handles comments: skips if properly separated.
6. Returns `TRUE` when content is found.

**Usage Example**

```php
// Called internally to advance between lines
// For "key: value\n  nested: val", advances from "key" line to "nested" line
```

### `next_entry()`

Advances to the next entry in a block collection and returns its indentation.

**Parameters**

None.

**Return Values**

| Type | Description |
|------|-------------|
| `int` | Indentation level of the next entry |
| `FALSE` | No more entries (end of input, document marker, or tab indentation) |

**Inner Mechanisms**

1. Calls `next_line()` to advance; returns `FALSE` if no more lines.
2. Returns `FALSE` if a document marker (`---` or `...`) is found.
3. Returns `FALSE` if the previous character is a tab (invalid indentation).
4. Returns the line indentation of the next entry.

**Usage Example**

```php
// Called internally by parse_sequence() and parse_mapping()
// to determine if the next entry belongs to the same collection
```

### `has_separator($pos)`

Checks if the character following the current position is a valid YAML separator.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$pos` | `int\|NULL` | `NULL` | Position to check from, defaults to `self::$pos` |

**Return Values**

| Type | Description |
|------|-------------|
| `TRUE` | A separator (space, tab, or newline) follows |
| `FALSE` | No separator follows |

**Inner Mechanisms**

Checks if the character at `$pos + 1` is in the set `\t\n ` (tab, newline, space). Returns `TRUE` if at end of input.

**Usage Example**

```php
// Called internally to validate indicators
// For "- value", after "-", has_separator() → TRUE (space follows)
// For "-value", after "-", has_separator() → FALSE
```

### `at_value()`

Checks if the current position is at a value indicator (`:` followed by a separator).

**Parameters**

None.

**Return Values**

| Type | Description |
|------|-------------|
| `TRUE` | Current position is at a value indicator |
| `FALSE` | Not at a value indicator |

**Inner Mechanisms**

1. Checks if the current character is `:`.
2. If so, checks if a separator follows (via `has_separator()`).
3. Also returns `TRUE` if a flow indicator (`,[]{}`) follows the colon (for flow context).

**Usage Example**

```php
// Called internally to detect empty values in flow context
// For "key:", at_value() → TRUE
// For "key: value", at_value() → TRUE (colon followed by space)
```

### `at_comment()`

Checks if the current position is at a properly separated comment.

**Parameters**

None.

**Return Values**

| Type | Description |
|------|-------------|
| `TRUE` | Current position is at a comment (`#`) preceded by whitespace or at input start |
| `FALSE` | Not at a comment |

**Inner Mechanisms**

1. Checks if the current character is `#`.
2. Checks if the preceding character is whitespace (`\t`, `\n`, ` `) or if at input start.

**Usage Example**

```php
// Called internally by skip_blank() and next_line()
// For "value # comment", at_comment() at "#" → TRUE
// For "value#comment", at_comment() at "#" → FALSE
```

### `at_line_end()`

Checks if the current position is at the end of a line (or end of input).

**Parameters**

None.

**Return Values**

| Type | Description |
|------|-------------|
| `TRUE` | At end of line, end of input, or at a comment |
| `FALSE` | Content follows on the current line |

**Inner Mechanisms**

Returns `TRUE` if the current character is `""` (end of input), `\n` (newline), or if `at_comment()` returns `TRUE`.

**Usage Example**

```php
// Called internally to detect empty nodes
// For "key:\n  value", at_line_end() after ":" → TRUE
```

### `get_marker($pos)`

Checks if the position is at a YAML document marker (`---` or `...`).

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$pos` | `int\|NULL` | `NULL` | Position to check, defaults to `self::$pos` |

**Return Values**

| Type | Description |
|------|-------------|
| `string` | `"-"` for document start marker, `"."` for document end marker |
| `string` | `""` if not at a marker |

**Inner Mechanisms**

1. Checks that the position is at column zero (preceded by `\n` or at input start).
2. Checks that there are at least 3 characters remaining.
3. Checks that the first character is `-` or `.`.
4. Validates that all three characters are the same.
5. Checks that a separator follows the marker.

**Usage Example**

```php
// Called internally to detect document boundaries
// For "---\nkey: value", get_marker() → "-"
// For "...\n", get_marker() → "."
```

### `key_end($eol)`

Finds the position of the `:` separator that terminates a mapping key.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$eol` | `int` | End-of-line position for the current line |

**Return Values**

| Type | Description |
|------|-------------|
| `int` | Position of the `:` separator |
| `FALSE` | No valid separator found |

**Inner Mechanisms**

1. Iterates through characters from `self::$pos` to `$eol`.
2. Tracks quote state (`"` and `'`) to skip colons inside quoted strings.
3. Tracks flow depth (`[` and `{` increase, `]` and `}` decrease) to skip colons inside flow collections.
4. Handles property tokens (`&`, `!`, `*`) by skipping to their end.
5. Returns the position of a `:` that is:
   - At flow depth 0
   - Followed by a space/tab or at end of line
6. Returns `FALSE` if a `#` comment is found without proper separation.

**Usage Example**

```php
// Called internally by parse_mapping() to find the key-value separator
// For "key: value", key_end() returns position of ":"
```

### `error($errmsg)`

Sets the error state and records an error message with line number.

**Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$errmsg` | `string` | The error description |

**Return Values**

| Type | Description |
|------|-------------|
| `FALSE` | Always returns `FALSE` to propagate errors |

**Inner Mechanisms**

1. Sets `self::$error` to `TRUE`.
2. If no error message has been set yet, calculates the line number from `self::$pos` (counting newlines) and stores `"line N: $errmsg"` in `self::$errmsg`.
3. Returns `FALSE` to allow inline error propagation (e.g., `return self::error("...")`).

**Usage Example**

```php
// Called internally throughout the parser
// self::error("Invalid scalar.") sets the error state and returns FALSE
```

## Complete Usage Example

```php
// Parse a complex YAML document
$yaml = <<<YAML
---
name: "John Doe"
age: 30
active: true
tags:
  - php
  - yaml
  - parser
address:
  street: 123 Main St
  city: "New York"
  zip: 10001
metadata: !!map
  created: 2024-01-15
  version: 1.0
aliases: &person
  name: Jane
  age: 25
friend: *person
empty_value:
block_text: |
  This is a
  multi-line
  literal scalar.
folded_text: >
  This is a
  folded scalar
  that becomes one line.
YAML;

$result = yaml_parse($yaml);

// Result:
// [
//   "name" => "John Doe",
//   "age" => 30,
//   "active" => true,
//   "tags" => ["php", "yaml", "parser"],
//   "address" => ["street" => "123 Main St", "city" => "New York", "zip" => 10001],
//   "metadata" => ["created" => "2024-01-15", "version" => 1.0],
//   "empty_value" => null,
//   "block_text" => "This is a\nmulti-line\nliteral scalar.\n",
//   "folded_text" => "This is a folded scalar that becomes one line.\n",
//   "friend" => ["name" => "Jane", "age" => 25]
// ]
```


<!-- HASH:dced2111751e6b69b9ea40e548e3edb4 -->
