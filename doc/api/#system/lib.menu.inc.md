# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.menu.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.menu.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Menu Class

The `menu` class in the PWNC Web Platform is responsible for generating hierarchical navigation menus from a directory structure. It leverages the `directory` module to fetch and traverse a tree of directory entries, then renders them as nested HTML lists (`<ul>` and `<li>` elements) with configurable visibility, depth, and filtering options.

This class is typically used to create site navigation, sitemaps, or contextual menus where hierarchical relationships between pages or sections must be visually represented.

---

### Constants

| Name                          | Value | Description                                                                 |
|-------------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_MENU_FILTER_NORMAL`      | `0`   | Show all menu entries without filtering.                                   |
| `CMS_MENU_FILTER_OPEN`        | `1`   | Show only entries in currently open branches.                              |
| `CMS_MENU_FILTER_ACTIVE`      | `2`   | Show only entries in the active branch (from root to current page).        |
| `CMS_MENU_FILTER_ACTIVE_OPEN` | `3`   | Show only entries in the active branch and any open sub-branches.          |

---

### Properties

| Name            | Default / Initial Value | Description                                                                                     |
|-----------------|-------------------------|-------------------------------------------------------------------------------------------------|
| `$flexview`     | `NULL`                  | Holds the `flexview` object from the `directory` module, used to traverse and render entries.   |
| `$level`        | `0`                     | Starting indentation level (0 = root). Can be relative (e.g., `+1`, `-1`) or absolute.         |
| `$depth`        | `NULL`                  | Maximum depth of entries to display. `NULL` means no limit.                                     |
| `$filter`       | `CMS_MENU_FILTER_NORMAL`| Filter mode (see constants above).                                                              |
| `$show_images`  | `NULL`                  | Whether to include image placeholders in menu items.                                            |
| `$exclude`      | `NULL`                  | Array of entry indices to exclude from the menu.                                                |
| `$start`        | `0`                     | Index of the first entry to display (for pagination or slicing).                                |
| `$end`          | `NULL`                  | Index of the last entry to display (exclusive). `NULL` means no limit.                          |

---

### Constructor: `__construct()`

#### Purpose
Initializes a new `menu` instance, configures the underlying `flexview` object, and sets up display parameters such as level, depth, filtering, and visual options.

#### Parameters

| Name                | Type      | Description                                                                                     |
|---------------------|-----------|-------------------------------------------------------------------------------------------------|
| `$index`            | `int`     | ID of the currently active directory entry (e.g., current page).                                |
| `$base`             | `int`     | ID of the root entry of the menu branch.                                                        |
| `$level`            | `int|string`| Starting level. Can be absolute (e.g., `1`) or relative (e.g., `+1`, `-1`).                     |
| `$depth`            | `int|null`| Maximum depth to display. `NULL` = no limit.                                                    |
| `$filter`           | `int|string`| Filter mode. Can be a constant (`CMS_MENU_FILTER_*`) or string (`"open"`, `"active"`, etc.).    |
| `$show_icons`       | `bool`    | Whether to display icons next to menu items.                                                    |
| `$show_images`      | `bool`    | Whether to include image placeholders (`image_button`, `image_hover`, `image_active`).          |
| `$show_description` | `bool`    | Whether to show the entry description below the name.                                           |
| `$show_hidden`      | `bool`    | Whether to include hidden entries in the menu.                                                  |
| `$exclude`          | `array|string|null` | Entries to exclude. Can be an array of IDs or a space-separated string.                     |
| `$start`            | `int`     | Index of the first entry to display.                                                            |
| `$length`           | `int|null`| Number of entries to display. `NULL` = no limit.                                                |

#### Return Value
None. The constructor initializes the object and triggers menu rendering via `flexview->show_custom()`.

#### Inner Mechanisms
1. Loads the `directory` library.
2. Fetches a `flexview` object representing the directory tree, optionally excluding hidden entries.
3. Resolves the visible ancestor of the active entry if hidden entries are excluded.
4. Sets the base entry to define the root of the menu branch.
5. Configures the display template for each menu item, including placeholders for links, icons, images, and descriptions.
6. Parses and applies level, depth, filter, and exclusion rules.
7. Triggers the rendering process by calling `flexview->show_custom([$this, "show"])`.

#### Usage Example
```php
// Create a menu starting at the root (base=0), showing 2 levels deep,
// only the active branch, with icons and images, excluding entry 42.
$menu = new \cms\menu(
    index: 15,                // Current page ID
    base: 0,                  // Root of the menu
    level: 0,                 // Start at root level
    depth: 2,                 // Show 2 levels
    filter: "active",         // Only show active branch
    show_icons: true,         // Show icons
    show_images: true,        // Show images
    show_description: false,  // No descriptions
    show_hidden: false,       // Exclude hidden entries
    exclude: [42],            // Exclude entry 42
    start: 0,                 // Start from first entry
    length: null              // No limit on number of entries
);
```

---

### Method: `show($flexview_entry)`

#### Purpose
Callback function used by `flexview` to render each directory entry as a menu item. It controls the flow of HTML output, manages indentation, and applies visibility and filtering rules.

#### Parameters

| Name               | Type            | Description                                                                                     |
|--------------------|-----------------|-------------------------------------------------------------------------------------------------|
| `$flexview_entry`  | `object`        | An object representing the current directory entry, with properties like `type`, `indentation`, `open`, `index`, etc. |

#### Return Value
- `void`: For entries that should be rendered.
- `TRUE`: To skip the current entry and all subsequent entries in the current branch.

#### Inner Mechanisms
1. Uses static variables to track:
   - `$count`: Number of entries processed.
   - `$indentation`: Current nesting level.
   - `$open`: Array tracking whether each indentation level is open.
   - `$instance`: Unique ID for this menu instance (used in HTML IDs).
2. Handles three event types:
   - `CMS_FLEXVIEW_ENTRY_TYPE_BASE`: Resets counters before processing starts.
   - `CMS_FLEXVIEW_ENTRY_TYPE_ENTRY`: Renders individual menu items.
     - Applies filter rules (e.g., skip non-open branches in `CMS_MENU_FILTER_OPEN`).
     - Skips excluded entries.
     - Checks visibility based on level, depth, start, and end.
     - Opens or closes `<ul>` and `<li>` tags as needed to maintain hierarchy.
     - Renders the entry using the `flexview->display()` method.
   - `CMS_FLEXVIEW_ENTRY_TYPE_END`: Closes all open `<ul>` and `<li>` tags.

#### Usage Context
This method is not called directly. It is passed to `flexview->show_custom()` and invoked automatically during tree traversal.

---

### Method: `_ul_li($instance, $index)`

#### Purpose
Outputs the opening tags for a new submenu (`<ul><li id="...">`).

#### Parameters

| Name        | Type  | Description                                                                                     |
|-------------|-------|-------------------------------------------------------------------------------------------------|
| `$instance` | `int` | Unique ID of the menu instance.                                                                |
| `$index`    | `int` | ID of the current directory entry.                                                             |

#### Return Value
None. Outputs HTML directly.

#### Inner Mechanisms
- Generates a unique HTML ID for the `<li>` element using `x()` for XML escaping.
- Outputs the opening tags for a nested list.

#### Usage Example
```php
// Output: <ul><li id="menu1-42">
$this->_ul_li(1, 42);
```

---

### Method: `_li($instance, $index)`

#### Purpose
Outputs the opening tag for a new list item (`<li id="...">`).

#### Parameters

| Name        | Type  | Description                                                                                     |
|-------------|-------|-------------------------------------------------------------------------------------------------|
| `$instance` | `int` | Unique ID of the menu instance.                                                                |
| `$index`    | `int` | ID of the current directory entry.                                                             |

#### Return Value
None. Outputs HTML directly.

#### Inner Mechanisms
- Generates a unique HTML ID for the `<li>` element using `x()` for XML escaping.
- Outputs the opening tag for a list item.

#### Usage Example
```php
// Output: <li id="menu1-42">
$this->_li(1, 42);
```


<!-- HASH:c8f5de8651d9c3294263f683196de9bb -->
