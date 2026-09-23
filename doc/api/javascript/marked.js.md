# PWNC API Documentation

[← Index](../README.md) | [`javascript/marked.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/marked.js)

- **Version:** `26.9.21.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## marked.js

**File:** `javascript/marked.js`  
**Version:** marked v15.0.12  
**License:** MIT  
**Source:** https://github.com/markedjs/marked  

### Overview

This file is the bundled, minified distribution of the **marked** Markdown parser. It provides a complete, zero-dependency JavaScript implementation for parsing Markdown into HTML. The library is structured around a pipeline of **Lexer → Tokenizer → Parser → Renderer**, with support for extensions, hooks, and both synchronous and asynchronous operation.

The file is wrapped in a UMD (Universal Module Definition) pattern, making it compatible with CommonJS (`module.exports`), AMD (`define`), and browser global (`window.marked`) environments.

### Architecture

The parsing pipeline works as follows:

1. **Lexer** (`x`) — Takes raw Markdown text and produces a token tree by delegating to the **Tokenizer**.
2. **Tokenizer** (`S`) — Contains all the regex rules and methods to identify and extract Markdown constructs (headings, code blocks, lists, links, etc.) into structured token objects.
3. **Parser** (`b`) — Walks the token tree and calls the appropriate **Renderer** methods to produce output.
4. **Renderer** (`$` / `_`) — Converts individual tokens into HTML strings (or plain text for `TextRenderer`).

The **Marked** class (`E`) is the main entry point that orchestrates the entire pipeline, manages options, extensions, hooks, and error handling.

---

## Module Exports

| Export | Type | Description |
|--------|------|-------------|
| `marked` | `function` | Default parse function — `marked(src, options)` |
| `Marked` | `class` | Main orchestrator class |
| `Lexer` | `class` | Converts Markdown text into a token tree |
| `Tokenizer` | `class` | Rule-based token extraction from text |
| `Parser` | `class` | Walks tokens and delegates to Renderer |
| `Renderer` | `class` | HTML output renderer |
| `TextRenderer` | `class` | Plain-text output renderer |
| `Hooks` | `class` | Extension hook system |
| `defaults` | `object` | Default options object |
| `getDefaults` | `function` | Returns a fresh defaults object |
| `setOptions` | `function` | Sets global default options |
| `use` | `function` | Registers extensions |
| `walkTokens` | `function` | Recursively walks all tokens |
| `parse` | `function` | Alias for `marked` |
| `parseInline` | `function` | Parses inline-only Markdown |
| `lexer` | `function` | Lexes Markdown into tokens |
| `parser` | `function` | Parses tokens into output |
| `options` | `function` | Alias for `setOptions` |

---

## Default Options

The `z()` function returns the default options object:

| Property | Default | Description |
|----------|---------|-------------|
| `async` | `false` | Whether to return a Promise |
| `breaks` | `false` | Treat single newlines as `<br>` |
| `extensions` | `null` | Custom extensions array |
| `gfm` | `true` | GitHub Flavored Markdown |
| `hooks` | `null` | Custom Hooks instance |
| `pedantic` | `false` | Conform to old Markdown.pl behavior |
| `renderer` | `null` | Custom Renderer instance |
| `silent` | `false` | Suppress errors, return error HTML |
| `tokenizer` | `null` | Custom Tokenizer instance |
| `walkTokens` | `null` | Token walker callback |

---

## Tokenizer Class (`S`)

The `Tokenizer` class is responsible for identifying Markdown constructs in text using regex rules. It holds references to the active rule sets and the parent `Lexer` instance.

### Constructor

```javascript
new Tokenizer(options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `object` | Configuration options (defaults to global `w`) |

**Inner mechanism:** Stores options, initializes `rules` to `null` (set by `Lexer`), and sets `lexer` to `null` (set by `Lexer`).

### Methods

#### `space(src)`

Parses leading whitespace/newlines into a `space` token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text to tokenize |

**Returns:** `{type: "space", raw: string}` or `null`

**Usage:**
```javascript
const tok = new Tokenizer();
tok.rules = Tokenizer.rules; // set by Lexer
const token = tok.space("\n\nHello");
// { type: "space", raw: "\n\n" }
```

#### `code(src)`

Parses indented code blocks (4+ spaces or tab).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "code", raw: string, codeBlockStyle: "indented", text: string}` or `null`

**Inner mechanism:** Matches lines starting with 4 spaces or a tab. In pedantic mode, preserves original indentation; otherwise strips it.

#### `fences(src)`

Parses fenced code blocks (``` or ~~~).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "code", raw: string, lang: string, text: string}` or `null`

**Inner mechanism:** Uses `Le` regex to match opening/closing fence markers. Language identifier is extracted and trimmed. Indentation compensation is applied via `rt()`.

#### `heading(src)`

Parses ATX headings (`# Heading`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "heading", raw: string, depth: number, text: string, tokens: array}` or `null`

**Inner mechanism:** Matches 1–6 `#` characters. Trailing `#` characters are stripped (unless pedantic mode and no space before them). Inline tokens are generated for the heading text.

#### `hr(src)`

Parses horizontal rules (`---`, `***`, `___`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "hr", raw: string}` or `null`

#### `blockquote(src)`

Parses blockquote blocks (`> ...`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "blockquote", raw: string, tokens: array, text: string}` or `null`

**Inner mechanism:** Collects consecutive lines starting with `>`. Handles nested blockquotes and lists by recursively calling `Lexer.blockTokens()`. Merges adjacent blockquote tokens.

#### `list(src)`

Parses ordered and unordered lists.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "list", raw: string, ordered: boolean, start: number, loose: boolean, items: array}` or `null`

**Inner mechanism:** Determines list type (ordered/unordered) from the bullet character. Iterates through list items, handling indentation, task checkboxes (GFM), and nested content. Detects "loose" lists (items separated by blank lines).

#### `html(src)`

Parses raw HTML blocks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "html", block: true, raw: string, pre: boolean, text: string}` or `null`

**Inner mechanism:** Matches block-level HTML tags. Sets `pre` flag for `<pre>`, `<script>`, and `<style>` tags.

#### `def(src)`

Parses link reference definitions (`[label]: url "title"`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "def", tag: string, raw: string, href: string, title: string}` or `null`

**Inner mechanism:** Extracts the label (lowercased), URL (angle brackets stripped), and optional title. Stores in `Lexer.tokens.links`.

#### `table(src)`

Parses GFM tables.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "table", raw: string, header: array, align: array, rows: array}` or `null`

**Inner mechanism:** Parses header row, delimiter row (determines alignment), and data rows. Each cell is tokenized inline.

#### `lheading(src)`

Parses setext-style headings (underlined with `===` or `---`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "heading", raw: string, depth: 1|2, text: string, tokens: array}` or `null`

#### `paragraph(src)`

Parses paragraph text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "paragraph", raw: string, text: string, tokens: array}` or `null`

**Inner mechanism:** Matches any text not consumed by other block rules. Trailing newline is stripped.

#### `text(src)`

Parses remaining text as a fallback.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "text", raw: string, text: string, tokens: array}` or `null`

#### `escape(src)`

Parses backslash escapes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "escape", raw: string, text: string}` or `null`

#### `tag(src)`

Parses inline HTML tags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "html", raw: string, inLink: boolean, inRawBlock: boolean, block: false, text: string}` or `null`

**Inner mechanism:** Tracks link and raw block state for nested HTML.

#### `link(src)`

Parses inline links (`[text](url "title")`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "link", raw: string, href: string, title: string, text: string, tokens: array}` or `null`

**Inner mechanism:** Handles angle-bracket URLs, nested parentheses, and title extraction. Delegates to `me()` helper to build the token.

#### `reflink(src, links)`

Parses reference-style links (`[text][label]` or `[label]`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |
| `links` | `object` | Link reference definitions map |

**Returns:** `{type: "link"|"text", ...}` token or `null`

**Inner mechanism:** Looks up the reference label in the `links` map. If not found, returns a plain text token.

#### `emStrong(src, tokens, text)`

Parses emphasis (`*italic*`, `**bold**`, `_italic_`, `__bold__`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |
| `tokens` | `array` | Previously parsed tokens (for context) |
| `text` | `string` | Accumulated text (for delimiter tracking) |

**Returns:** `{type: "em"|"strong", raw: string, text: string, tokens: array}` or `null`

**Inner mechanism:** Uses complex delimiter-matching regexes (`je`, `Qe`, `Ke` for normal; `Fe`, `Ue` for GFM). Tracks left/right delimiter counts and applies CommonMark emphasis rules.

#### `codespan(src)`

Parses inline code spans (`` `code` ``).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "codespan", raw: string, text: string}` or `null`

**Inner mechanism:** Matches backtick-delimited spans. Strips leading/trailing spaces if both present.

#### `br(src)`

Parses hard line breaks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "br", raw: string}` or `null`

#### `del(src)`

Parses strikethrough (`~~text~~`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "del", raw: string, text: string, tokens: array}` or `null`

#### `autolink(src)`

Parses autolinked URLs and emails (`<https://example.com>`, `<user@example.com>`).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "link", raw: string, text: string, href: string, tokens: array}` or `null`

#### `url(src)`

Parses bare URLs (GFM).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "link", raw: string, text: string, href: string, tokens: array}` or `null`

**Inner mechanism:** Matches `http://`, `https://`, `www.`, or email patterns. Backpedals to avoid consuming trailing punctuation.

#### `inlineText(src)`

Parses plain inline text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |

**Returns:** `{type: "text", raw: string, text: string, escaped: boolean}` or `null`

**Inner mechanism:** Matches any text not consumed by other inline rules. Sets `escaped` flag if inside a raw HTML block.

---

## Lexer Class (`x`)

The `Lexer` class converts raw Markdown text into a structured token tree.

### Constructor

```javascript
new Lexer(options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `object` | Configuration options |

**Inner mechanism:** Initializes `tokens` array (with `links` sub-object), sets up the `Tokenizer`, selects the appropriate rule set based on `pedantic`/`gfm`/`breaks` options, and initializes state flags (`inLink`, `inRawBlock`, `top`).

### Static Properties

| Property | Type | Description |
|----------|------|-------------|
| `rules` | `object` | Contains `block` and `inline` rule sets for all modes |

### Static Methods

#### `lex(src, options)`

Creates a new `Lexer` instance and lexes the source.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Markdown source text |
| `options` | `object` | Configuration options |

**Returns:** `array` — Token tree

#### `lexInline(src, options)`

Lexes inline-only content (no block-level parsing).

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Markdown source text |
| `options` | `object` | Configuration options |

**Returns:** `array` — Inline token array

### Instance Methods

#### `lex(src)`

Main lexing entry point.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Markdown source text |

**Returns:** `array` — Token tree

**Inner mechanism:** Normalizes carriage returns, calls `blockTokens()` to parse block-level constructs, then processes the `inlineQueue` to parse inline content within each block.

#### `blockTokens(src, tokens, nested)`

Parses block-level Markdown into tokens.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Source text |
| `tokens` | `array` | Output token array |
| `nested` | `boolean` | Whether inside a nested context (e.g., list item) |

**Returns:** `array` — Token array

**Inner mechanism:** Iterates through the source, trying each block rule in order. Handles extensions, merges adjacent text/paragraph tokens, and detects infinite loops.

#### `inline(src, tokens)`

Queues inline content for later processing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Inline source text |
| `tokens` | `array` | Token array to populate |

**Returns:** `array` — The tokens array (for chaining)

**Inner mechanism:** Pushes to `inlineQueue` for deferred processing after all block tokens are collected.

#### `inlineTokens(src, tokens)`

Parses inline Markdown into tokens.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Inline source text |
| `tokens` | `array` | Output token array |

**Returns:** `array` — Inline token array

**Inner mechanism:** Pre-processes the source to handle reference links and block-level constructs that shouldn't be parsed inline. Then iterates through the source, trying each inline rule. Handles extensions and infinite loop detection.

---

## Renderer Class (`$`)

The `Renderer` class converts tokens into HTML strings.

### Constructor

```javascript
new Renderer(options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `object` | Configuration options |

**Inner mechanism:** Stores options and sets `parser` reference (set by `Parser`).

### Methods

#### `space()`

**Returns:** `""` (empty string)

#### `code({text, lang, escaped})`

Renders a code block token to HTML.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | Code content |
| `lang` | `string` | Language identifier |
| `escaped` | `boolean` | Whether text is already HTML-escaped |

**Returns:** `string` — `<pre><code>...</code></pre>`

**Inner mechanism:** Extracts language class from `lang`. If `escaped` is true, uses text as-is; otherwise applies `R()` (HTML entity encoding).

**Usage:**
```javascript
const renderer = new Renderer();
renderer.code({ text: "console.log('hi')", lang: "js", escaped: false });
// '<pre><code class="language-js">console.log(&#39;hi&#39;)</code></pre>\n'
```

#### `blockquote({tokens})`

Renders a blockquote token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Child tokens |

**Returns:** `string` — `<blockquote>...</blockquote>`

#### `html({text})`

Renders raw HTML.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | HTML content |

**Returns:** `string` — The HTML as-is

#### `heading({tokens, depth})`

Renders a heading token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline tokens for heading text |
| `depth` | `number` | Heading level (1–6) |

**Returns:** `string` — `<h1>...</h1>` through `<h6>...</h6>`

#### `hr()`

**Returns:** `"<hr>\n"`

#### `list({ordered, start, items})`

Renders a list token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `ordered` | `boolean` | Whether ordered list |
| `start` | `number` | Starting number (ordered lists) |
| `items` | `array` | List item tokens |

**Returns:** `string` — `<ul>...</ul>` or `<ol>...</ol>`

#### `listitem({task, checked, loose, tokens})`

Renders a single list item.

| Parameter | Type | Description |
|-----------|------|-------------|
| `task` | `boolean` | Whether it's a task item |
| `checked` | `boolean` | Whether checkbox is checked |
| `loose` | `boolean` | Whether list is loose |
| `tokens` | `array` | Child tokens |

**Returns:** `string` — `<li>...</li>`

**Inner mechanism:** If `task` is true, prepends a checkbox input. In loose mode, wraps content in `<p>` tags.

#### `checkbox({checked})`

Renders a checkbox input.

| Parameter | Type | Description |
|-----------|------|-------------|
| `checked` | `boolean` | Whether checked |

**Returns:** `string` — `<input ... type="checkbox">`

#### `paragraph({tokens})`

Renders a paragraph token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline tokens |

**Returns:** `string` — `<p>...</p>`

#### `table({header, rows})`

Renders a table token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `header` | `array` | Header cell tokens |
| `rows` | `array` | Data row arrays |

**Returns:** `string` — `<table>...</table>`

#### `tablerow({text})`

Renders a table row.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | Pre-rendered cell HTML |

**Returns:** `string` — `<tr>...</tr>`

#### `tablecell({tokens, header, align})`

Renders a table cell.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline tokens |
| `header` | `boolean` | Whether header cell |
| `align` | `string` | Alignment: `"left"`, `"right"`, `"center"`, or `null` |

**Returns:** `string` — `<th>...</th>` or `<td>...</td>`

#### `strong({tokens})`

Renders bold text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline tokens |

**Returns:** `string` — `<strong>...</strong>`

#### `em({tokens})`

Renders italic text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline tokens |

**Returns:** `string` — `<em>...</em>`

#### `codespan({text})`

Renders inline code.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | Code text |

**Returns:** `string` — `<code>...</code>`

#### `br()`

**Returns:** `"<br>"`

#### `del({tokens})`

Renders strikethrough text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline tokens |

**Returns:** `string` — `<del>...</del>`

#### `link({href, title, tokens})`

Renders a link.

| Parameter | Type | Description |
|-----------|------|-------------|
| `href` | `string` | URL |
| `title` | `string` | Optional title |
| `tokens` | `array` | Link text tokens |

**Returns:** `string` — `<a href="..." title="...">...</a>`

**Inner mechanism:** Validates URL via `V()`. If invalid, returns just the text.

#### `image({href, title, text, tokens})`

Renders an image.

| Parameter | Type | Description |
|-----------|------|-------------|
| `href` | `string` | Image URL |
| `title` | `string` | Optional title |
| `text` | `string` | Alt text |
| `tokens` | `array` | Optional inline tokens |

**Returns:** `string` — `<img src="..." alt="..." title="...">`

**Inner mechanism:** If `tokens` provided, renders them for alt text. Validates URL via `V()`.

#### `text({text, tokens, escaped})`

Renders plain text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` | `string` | Text content |
| `tokens` | `array` | Optional inline tokens |
| `escaped` | `boolean` | Whether already escaped |

**Returns:** `string` — Escaped or raw text

---

## TextRenderer Class (`_`)

A minimal renderer that returns plain text instead of HTML. All methods return the text content directly without wrapping tags.

### Methods

| Method | Parameter | Returns |
|--------|-----------|---------|
| `strong({text})` | `text: string` | `text` |
| `em({text})` | `text: string` | `text` |
| `codespan({text})` | `text: string` | `text` |
| `del({text})` | `text: string` | `text` |
| `html({text})` | `text: string` | `text` |
| `text({text})` | `text: string` | `text` |
| `link({text})` | `text: string` | `"" + text` |
| `image({text})` | `text: string` | `"" + text` |
| `br()` | — | `""` |

**Usage:**
```javascript
const parser = new Parser({ renderer: new TextRenderer() });
parser.parse(tokens); // Returns plain text
```

---

## Parser Class (`b`)

The `Parser` class walks the token tree and delegates to the `Renderer` to produce output.

### Constructor

```javascript
new Parser(options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `object` | Configuration options |

**Inner mechanism:** Initializes renderer (defaults to `Renderer`), sets `parser` reference on renderer, and creates a `TextRenderer` instance.

### Static Methods

#### `parse(tokens, options)`

Creates a new `Parser` and parses tokens.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Token tree from Lexer |
| `options` | `object` | Configuration options |

**Returns:** `string` — Rendered output

#### `parseInline(tokens, options)`

Parses inline-only tokens.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline token array |
| `options` | `object` | Configuration options |

**Returns:** `string` — Rendered inline output

### Instance Methods

#### `parse(tokens, output)`

Main parsing entry point.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Token tree |
| `output` | `boolean` | Whether to wrap text in `<p>` tags |

**Returns:** `string` — Rendered output

**Inner mechanism:** Iterates through tokens, calling the appropriate renderer method for each type. Handles extension renderers, merges adjacent text tokens, and wraps text in paragraphs when `output` is true.

#### `parseInline(tokens, renderer)`

Parses inline tokens only.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Inline token array |
| `renderer` | `Renderer` | Renderer instance (defaults to `this.renderer`) |

**Returns:** `string` — Rendered inline output

**Inner mechanism:** Similar to `parse()` but only handles inline token types (escape, html, link, image, strong, em, codespan, br, del, text).

---

## Hooks Class (`L`)

The `Hooks` class provides an extension point for preprocessing, postprocessing, and token manipulation.

### Constructor

```javascript
new Hooks(options)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `object` | Configuration options |

### Static Properties

| Property | Type | Description |
|----------|------|-------------|
| `passThroughHooks` | `Set` | Hook names that pass through: `"preprocess"`, `"postprocess"`, `"processAllTokens"` |

### Methods

#### `preprocess(src)`

Pre-processes the Markdown source before lexing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Markdown source |

**Returns:** `string` — Processed source

#### `postprocess(output)`

Post-processes the rendered output.

| Parameter | Type | Description |
|-----------|------|-------------|
| `output` | `string` | Rendered HTML |

**Returns:** `string` — Processed output

#### `processAllTokens(tokens)`

Processes all tokens after lexing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Token tree |

**Returns:** `array` — Processed tokens

#### `provideLexer()`

Returns the appropriate lexer function.

**Returns:** `function` — `Lexer.lex` or `Lexer.lexInline`

#### `provideParser()`

Returns the appropriate parser function.

**Returns:** `function` — `Parser.parse` or `Parser.parseInline`

---

## Marked Class (`E`)

The main orchestrator class that ties together Lexer, Parser, Renderer, and Hooks.

### Constructor

```javascript
new Marked(...extensions)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `extensions` | `...object` | Extension objects to register |

**Inner mechanism:** Initializes `defaults` with `z()`, then calls `use()` for each extension.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `defaults` | `object` | Current options |
| `options` | `function` | Alias for `setOptions` |
| `parse` | `function` | Bound `parseMarkdown(true)` |
| `parseInline` | `function` | Bound `parseMarkdown(false)` |
| `Parser` | `class` | Parser class reference |
| `Renderer` | `class` | Renderer class reference |
| `TextRenderer` | `class` | TextRenderer class reference |
| `Lexer` | `class` | Lexer class reference |
| `Tokenizer` | `class` | Tokenizer class reference |
| `Hooks` | `class` | Hooks class reference |

### Methods

#### `walkTokens(tokens, callback)`

Recursively walks all tokens in the tree, calling `callback` for each.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Token tree |
| `callback` | `function` | Called with each token |

**Returns:** `array` — All tokens collected

**Inner mechanism:** Handles special token types (`table`, `list`) by recursing into their child tokens. For other tokens, recurses into `tokens` property if present.

#### `use(...extensions)`

Registers extensions (renderers, tokenizers, hooks, walkTokens).

| Parameter | Type | Description |
|-----------|------|-------------|
| `extensions` | `...object` | Extension objects |

**Returns:** `this` (for chaining)

**Inner mechanism:** Merges extension definitions into `defaults.extensions`. For each extension:
- **Renderer:** Wraps existing renderer method, falling back to original if extension returns `false`
- **Tokenizer:** Prepends to block/inline tokenizer arrays
- **Hooks:** Wraps hook methods, supporting async
- **walkTokens:** Chains with existing walker

#### `setOptions(options)`

Merges options into `defaults`.

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `object` | Options to merge |

**Returns:** `this` (for chaining)

#### `lexer(src, options)`

Lexes Markdown using the Lexer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Markdown source |
| `options` | `object` | Options (defaults to `this.defaults`) |

**Returns:** `array` — Token tree

#### `parser(tokens, options)`

Parses tokens using the Parser.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Token tree |
| `options` | `object` | Options (defaults to `this.defaults`) |

**Returns:** `string` — Rendered output

#### `parseMarkdown(block)`

Returns a function that parses Markdown (block or inline).

| Parameter | Type | Description |
|-----------|------|-------------|
| `block` | `boolean` | `true` for full document, `false` for inline |

**Returns:** `function` — Parse function `(src, options) => string | Promise<string>`

**Inner mechanism:** Merges options, sets up hooks, selects lexer/parser, handles async mode, and wraps in error handling via `onError()`.

#### `onError(silent, async)`

Returns an error handler function.

| Parameter | Type | Description |
|-----------|------|-------------|
| `silent` | `boolean` | Whether to suppress errors |
| `async` | `boolean` | Whether in async mode |

**Returns:** `function` — Error handler `(error) => string | Promise<string> | void`

**Inner mechanism:** Appends a bug report URL to the error message. In silent mode, returns an error HTML paragraph. In async mode, returns a rejected Promise.

---

## Global Instance and API

A singleton instance `M` of `Marked` is created, and the following convenience functions are exported:

### `marked(src, options)` / `parse(src, options)`

Parses Markdown to HTML.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Markdown source text |
| `options` | `object` | Optional configuration |

**Returns:** `string` — HTML output (or `Promise<string>` if `async: true`)

**Usage:**
```javascript
const html = marked('# Hello World\n\nThis is **bold**.');
// '<h1>Hello World</h1>\n<p>This is <strong>bold</strong>.</p>\n'
```

### `marked.setOptions(options)` / `options(options)`

Sets global default options.

| Parameter | Type | Description |
|-----------|------|-------------|
| `options` | `object` | Options to merge |

**Returns:** `marked` (for chaining)

**Usage:**
```javascript
marked.setOptions({
  gfm: true,
  breaks: true,
  headerIds: false
});
```

### `marked.use(...extensions)`

Registers extensions globally.

| Parameter | Type | Description |
|-----------|------|-------------|
| `extensions` | `...object` | Extension objects |

**Returns:** `marked` (for chaining)

**Usage:**
```javascript
marked.use({
  renderer: {
    link({ href, tokens }) {
      return `<a href="${href}" target="_blank">${this.parser.parseInline(tokens)}</a>`;
    }
  }
});
```

### `marked.walkTokens(tokens, callback)`

Walks all tokens in a tree.

| Parameter | Type | Description |
|-----------|------|-------------|
| `tokens` | `array` | Token tree |
| `callback` | `function` | Called with each token |

**Returns:** `array` — All tokens

### `marked.parseInline(src, options)`

Parses inline-only Markdown.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Markdown source |
| `options` | `object` | Optional configuration |

**Returns:** `string` — HTML output

### `marked.getDefaults()`

Returns a fresh default options object.

**Returns:** `object`

### `marked.defaults`

The current global defaults object (mutable).

### `marked.Lexer`, `marked.Parser`, `marked.Renderer`, `marked.TextRenderer`, `marked.Tokenizer`, `marked.Hooks`

Class references for direct instantiation.

---

## Utility Functions

### `R(src, encode)`

HTML entity encoder.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Text to encode |
| `encode` | `boolean` | Whether to encode (default: `false`) |

**Returns:** `string` — Encoded text

**Inner mechanism:** Replaces `&`, `<`, `>`, `"`, `'` with HTML entities.

### `V(src)`

URI encoder with percent-decoding.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | URL to encode |

**Returns:** `string` or `null` — Encoded URL, or `null` if invalid

**Inner mechanism:** Calls `encodeURI()` and replaces `%25` with `%`. Returns `null` on error.

### `Y(src, output)`

Splits a table row into cells, handling escaped pipes.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Table row text |
| `output` | `number` | Expected number of cells |

**Returns:** `array` — Cell strings

**Inner mechanism:** Replaces escaped pipes (`\|`) with a placeholder, splits on `|`, trims each cell, and restores pipes. Pads or truncates to `output` length.

### `A(src, endChar, trim)`

Trims trailing occurrences of a character.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Input string |
| `endChar` | `string` | Character to trim |
| `trim` | `boolean` | If `true`, trims leading instead |

**Returns:** `string` — Trimmed string

### `de(src, delimiters)`

Finds the closing delimiter index, accounting for nesting.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Input string |
| `delimiters` | `array` | `[openChar, closeChar]` |

**Returns:** `number` — Index of closing delimiter, or `-1`/`-2`

### `me(link, href, title, raw, lexer, rules)`

Builds a link/image token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `link` | `array` | Match array from regex |
| `href` | `string` | URL |
| `title` | `string` | Title |
| `raw` | `string` | Raw text |
| `lexer` | `Lexer` | Lexer instance |
| `rules` | `object` | Rule set |

**Returns:** `object` — Link or image token

### `rt(src, indent, rules)`

Compensates for indentation in fenced code blocks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `src` | `string` | Code block text |
| `indent` | `string` | Indentation string |
| `rules` | `object` | Rule set |

**Returns:** `string` — De-indented text

---

## Rule Sets

The library defines three rule set configurations:

| Mode | Block Rules | Inline Rules |
|------|-------------|--------------|
| `normal` | `B.normal` | `P.normal` |
| `gfm` | `B.gfm` | `P.gfm` |
| `pedantic` | `B.pedantic` | `P.pedantic` |
| `breaks` | `B.gfm` | `P.breaks` |

### Block Rule Sets (`B`)

| Rule | `normal` | `gfm` | `pedantic` |
|------|----------|-------|------------|
| `blockquote` | `Ie` | `Ie` | `Ie` |
| `code` | `_e` | `_e` | `_e` |
| `def` | `Ae` | `Ae` | `Be.def` |
| `fences` | `Le` | `Le` | `I` (disabled) |
| `heading` | `ze` | `ze` | `Be.heading` |
| `hr` | `O` | `O` | `O` |
| `html` | `Ce` | `Ce` | `Be.html` |
| `lheading` | `oe` | `Me` | `Be.lheading` |
| `list` | `Ee` | `Ee` | `Ee` |
| `newline` | `$e` | `$e` | `$e` |
| `paragraph` | `le` | `Oe.paragraph` | `Be.paragraph` |
| `table` | `I` (disabled) | `re` | `I` (disabled) |
| `text` | `Pe` | `Pe` | `Pe` |

### Inline Rule Sets (`P`)

| Rule | `normal` | `gfm` | `pedantic` | `breaks` |
|------|----------|-------|------------|----------|
| `_backpedal` | `I` | `j._backpedal` | `I` | `nt._backpedal` |
| `anyPunctuation` | `Xe` | `Xe` | `Xe` | `Xe` |
| `autolink` | `We` | `We` | `We` | `We` |
| `blockSkip` | `Ne` | `Ne` | `Ne` | `Ne` |
| `br` | `ae` | `ae` | `ae` | `nt.br` |
| `code` | `ve` | `ve` | `ve` | `ve` |
| `del` | `I` | `j.del` | `I` | `nt.del` |
| `emStrongLDelim` | `je` | `Fe` | `je` | `Fe` |
| `emStrongRDelimAst` | `Qe` | `Ue` | `Qe` | `Ue` |
| `emStrongRDelimUnd` | `Ke` | `Ke` | `Ke` | `Ke` |
| `escape` | `qe` | `qe` | `qe` | `qe` |
| `link` | `Ye` | `Ye` | `Ye` | `Ye` |
| `nolink` | `ge` | `ge` | `ge` | `ge` |
| `punctuation` | `Ze` | `Ze` | `Ze` | `Ze` |
| `reflink` | `ke` | `ke` | `ke` | `ke` |
| `reflinkSearch` | `et` | `et` | `et` | `et` |
| `tag` | `Ve` | `Ve` | `Ve` | `Ve` |
| `text` | `De` | `j.text` | `De` | `nt.text` |
| `url` | `I` | `j.url` | `I` | `nt.url` |

---

## Usage Examples

### Basic Parsing

```javascript
const marked = require('marked');

const md = `
# Hello World

This is a **bold** statement and some *italic* text.

- Item 1
- Item 2
- Item 3

[Link](https://example.com)
`;

const html = marked(md);
console.log(html);
```

### Custom Renderer

```javascript
const { Marked, Renderer } = require('marked');

const renderer = new Renderer();
renderer.link = function({ href, title, tokens }) {
  const text = this.parser.parseInline(tokens);
  const titleAttr = title ? ` title="${this.options.escape(title)}"` : '';
  return `<a href="${href}" target="_blank"${titleAttr}>${text}</a>`;
};

const marked = new Marked();
marked.setOptions({ renderer });

console.log(marked.parse('[Open in new tab](https://example.com)'));
// <a href="https://example.com" target="_blank">Open in new tab</a>
```

### Async Parsing

```javascript
const marked = require('marked');

marked.setOptions({ async: true });

marked.parse('# Hello').then(html => {
  console.log(html);
});
```

### Custom Extension

```javascript
const marked = require('marked');

marked.use({
  extensions: [
    {
      name: 'spoiler',
      level: 'inline',
      start(src) { return src.indexOf('=='); },
      tokenizer: {
        spoiler(src) {
          const match = src.match(/^==([^=]+)==/);
          if (!match) return;
          return {
            type: 'spoiler',
            raw: match[0],
            text: match[1]
          };
        }
      },
      renderer: {
        spoiler(token) {
          return `<span class="spoiler">${this.parser.parseInline(token.tokens)}</span>`;
        }
      }
    }
  ]
});

console.log(marked.parse('This is a ==spoiler== text.'));
// <p>This is a <span class="spoiler">spoiler</span> text.</p>
```

### Hooks

```javascript
const { Marked, Hooks } = require('marked');

const hooks = new Hooks();
hooks.preprocess = function(src) {
  return src.replace(/{{date}}/g, new Date().toISOString());
};

const marked = new Marked();
marked.setOptions({ hooks });

console.log(marked.parse('Today is {{date}}'));
// <p>Today is 2025-01-15T12:00:00.000Z</p>
```

### Token Walking

```javascript
const marked = require('marked');

const tokens = marked.lexer('# Hello\n\n**Bold** text');

marked.walkTokens(tokens, token => {
  if (token.type === 'heading') {
    console.log(`Found heading: ${token.text}`);
  }
});
```


<!-- HASH:e04ae0bf88da292e65e1e677394f6058 -->
