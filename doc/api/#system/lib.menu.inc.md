# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.menu.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.menu.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## menu

The `menu` class generates hierarchical navigation menus from the PWNC directory structure. It renders nested `<ul>`/`<li>` HTML lists with configurable filtering, depth limits, exclusion rules, and optional icons/images.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MENU_FILTER_NORMAL` | `0` | Show all entries |
| `CMS_MENU_FILTER_OPEN` | `1` | Show only open (expanded) branches |
| `CMS_MENU_FILTER_ACTIVE` | `2` | Show only the active path |
| `CMS_MENU_FILTER_ACTIVE_OPEN` | `3` | Show active path and open branches |

### Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$flexview` | `object` | `NULL` | Directory flexview display object |
| `$level` | `int` | `0` | Starting indentation level |
| `$depth` | `int\|NULL` | `NULL` | Maximum depth of menu levels |
| `$filter` | `int` | `CMS_MENU_FILTER_NORMAL` | Entry visibility filter |
| `$show_images` | `bool\|NULL` | `NULL` | Whether to show menu images |
| `$exclude` | `array\|NULL` | `NULL` | Array of excluded entry indices |
| `$start` | `int` | `0` | Start offset for entries |
| `$end` | `int\|NULL` | `NULL` | End offset for entries |

### __construct

Creates a menu instance and triggers rendering.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | `0` | Currently selected directory index |
| `$base` | `int` | `0` | Base entry for the menu branch |
| `$level` | `int\|string` | `0` | Starting level (absolute or relative with `+`/`-`) |
| `$depth` | `int\|NULL` | `NULL` | Maximum depth to display |
| `$filter` | `string\|int` | `CMS_MENU_FILTER_NORMAL` | Filter type (`"open"`, `"active"`, `"active-open"`, or constant) |
| `$show_icons` | `bool` | `FALSE` | Show icons |
| `$show_images` | `bool` | `FALSE` | Show images |
| `$show_description` | `bool` | `FALSE` | Show descriptions |
| `$show_hidden` | `bool` | `FALSE` | Include hidden entries |
| `$exclude` | `string\|array\|NULL` | `NULL` | Entries to exclude (space-separated string or array) |
| `$start` | `int` | `0` | Start offset |
| `$length` | `int\|NULL` | `NULL` | Number of entries to show |

#### Usage Example

```php
// Render a menu starting from the root, showing 3 levels deep
$menu = new menu(0, 0, 0, 3, CMS_MENU_FILTER_NORMAL, true, false, true);
```

### show

Callback method invoked by the flexview display system for each directory entry. Handles HTML output generation.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$flexview_entry` | `object` | Entry structure with `type`, `indentation`, `open`, `index` properties |

#### Return Values

- `TRUE` — Skip subsequent entries (used for filtering)
- `void` — Continue processing

#### Usage Example

```php
// Automatically called during construction; no direct invocation needed
$menu = new menu();
```

### _ul_li

Outputs opening `<ul><li>` tags with an ID attribute.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `int` | Menu instance counter |
| `$index` | `int` | Entry index |

### _li

Outputs opening `<li>` tag with an ID attribute.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `int` | Menu instance counter |
| `$index` | `int` | Entry index |


<!-- HASH:c8f5de8651d9c3294263f683196de9bb -->
