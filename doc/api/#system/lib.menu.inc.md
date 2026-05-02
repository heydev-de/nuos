# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.menu.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Menu Class Overview

The `menu` class in NUOS provides a flexible and configurable way to render hierarchical navigation menus from directory structures. It leverages the `directory` and `flexview` components to generate HTML lists (`<ul>`/`<li>`) with customizable depth, filtering, and styling options. This class is typically used for site navigation, sitemaps, or any context requiring structured directory-based menus.

---

## Constants

| Name                          | Value | Description                                                                 |
|-------------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_MENU_FILTER_NORMAL`      | `0`   | Show all menu entries without filtering.                                   |
| `CMS_MENU_FILTER_OPEN`        | `1`   | Show only entries in open branches (expanded paths).                       |
| `CMS_MENU_FILTER_ACTIVE`      | `2`   | Show only entries in the active path (current selection).                  |
| `CMS_MENU_FILTER_ACTIVE_OPEN` | `3`   | Show entries in the active path and all open branches.                     |

---

## Class: `menu`

### Properties

| Name            | Default/Type       | Description                                                                                     |
|-----------------|--------------------|-------------------------------------------------------------------------------------------------|
| `$flexview`     | `NULL`             | Instance of `flexview` used to traverse and render the directory structure.                     |
| `$level`        | `0` (int)          | Starting indentation level (0 = root). Can be relative (`+1`, `-1`) or absolute.               |
| `$depth`        | `NULL` (int)       | Maximum depth to display. `NULL` means no limit.                                                |
| `$filter`       | `CMS_MENU_FILTER_NORMAL` | Filter mode for menu entries (see constants).                                           |
| `$show_images`  | `NULL` (bool)      | Whether to display images (background or inline) for menu items.                                |
| `$exclude`      | `NULL` (array)     | Array of directory indices to exclude from the menu.                                            |
| `$start`        | `0` (int)          | Index of the first entry to display (pagination-like offset).                                   |
| `$end`          | `NULL` (int)       | Index of the last entry to display (calculated from `$start` + `$length`).                     |

---

### `__construct`

#### Purpose
Initializes a `menu` instance with configuration options for rendering a directory-based navigation menu. Loads the `directory` library and sets up the `flexview` object with display templates, filters, and visibility rules.

#### Parameters

| Name                | Type               | Default                     | Description                                                                                     |
|---------------------|--------------------|-----------------------------|-------------------------------------------------------------------------------------------------|
| `$index`            | `int`              | `0`                         | Directory index of the currently selected entry.                                               |
| `$base`             | `int`              | `0`                         | Directory index of the root entry for the menu branch.                                         |
| `$level`            | `int`/`string`     | `0`                         | Starting level (absolute or relative, e.g., `"+1"`, `"-1"`).                                   |
| `$depth`            | `int`/`NULL`       | `NULL`                      | Maximum depth to display. `NULL` for no limit.                                                 |
| `$filter`           | `int`/`string`     | `CMS_MENU_FILTER_NORMAL`    | Filter mode (see constants) or string (`"open"`, `"active"`, `"active-open"`).                |
| `$show_icons`       | `bool`             | `FALSE`                     | Whether to display icons next to menu items.                                                   |
| `$show_images`      | `bool`             | `FALSE`                     | Whether to display images (background or inline) for menu items.                               |
| `$show_description` | `bool`             | `FALSE`                     | Whether to include descriptions below menu items.                                              |
| `$show_hidden`      | `bool`             | `FALSE`                     | Whether to include hidden directory entries.                                                   |
| `$exclude`          | `array`/`string`   | `NULL`                      | Array of indices or space-separated string of indices to exclude.                              |
| `$start`            | `int`              | `0`                         | Index of the first entry to display (pagination-like offset).                                   |
| `$length`           | `int`/`NULL`       | `NULL`                      | Number of entries to display. `NULL` for no limit.                                             |

#### Return Values
- **None**: Constructor initializes the object and triggers menu rendering via `flexview->show_custom()`.

#### Inner Mechanisms
1. **Library Loading**: Checks for the `directory` library and aborts if unavailable.
2. **Flexview Setup**:
   - Retrieves a `flexview` object for the directory tree, optionally excluding hidden entries.
   - Sets the current index (adjusted for visibility if `$show_hidden` is `FALSE`).
   - Sets the base directory for the menu branch.
3. **Display Template**: Configures the HTML template for menu items, including:
   - Links (`<a>`) with optional `title`, `class`, and `href`.
   - Images (hover and static) if `$show_images` is `TRUE`.
   - Icons if `$show_icons` is `TRUE`.
   - Descriptions if `$show_description` is `TRUE`.
4. **Visibility Parameters**:
   - **Level**: Calculates absolute level from relative values (e.g., `"+1"`).
   - **Depth**: Ensures non-negative values.
   - **Filter**: Converts string filters to constants.
   - **Exclusion**: Converts string exclusions to arrays.
   - **Pagination**: Calculates `$end` from `$start` and `$length`.
5. **Rendering**: Invokes `flexview->show_custom()` with the `show()` method as the callback.

#### Usage Context
- **Typical Scenarios**:
  - Site navigation menus (main, footer, sidebar).
  - Sitemaps or hierarchical content listings.
  - Contextual menus (e.g., breadcrumbs, sub-navigation).
- **Example**:
  ```php
  $menu = new \cms\menu(
      $index = 5,          // Current page index
      $base = 0,           // Root of the menu
      $level = 0,          // Start at root level
      $depth = 2,          // Show 2 levels deep
      $filter = "active",  // Only show active path
      $show_images = true  // Enable images
  );
  ```

---

### `show`

#### Purpose
Callback method for `flexview->show_custom()` that processes each directory entry and renders the menu structure. Handles indentation, filtering, exclusion, and pagination.

#### Parameters

| Name              | Type               | Description                                                                                     |
|-------------------|--------------------|-------------------------------------------------------------------------------------------------|
| `$flexview_entry` | `object`           | A `flexview` entry object containing properties like `type`, `indentation`, `index`, and `open`. |

#### Return Values
- **`void`**: Outputs HTML directly.
- **`bool`**: Returns `TRUE` to skip subsequent entries (e.g., for filtering).

#### Inner Mechanisms
1. **Static Variables**:
   - `$count`: Tracks the number of entries processed (for pagination).
   - `$indentation`: Tracks the current indentation level (for nested `<ul>`/`<li>`).
   - `$open`: Array of open/closed states for each indentation level.
   - `$instance`: Unique identifier for the menu instance (used in HTML `id` attributes).
2. **Event Handling**:
   - **`CMS_FLEXVIEW_ENTRY_TYPE_BASE`**: Resets static variables before processing entries.
   - **`CMS_FLEXVIEW_ENTRY_TYPE_ENTRY`**:
     - **Filtering**:
       - Skips entries if `$filter` is `CMS_MENU_FILTER_ACTIVE_OPEN` and the entry is not in an open branch.
       - Skips excluded entries.
     - **Visibility**:
       - Checks if the entry is within the specified `$level` and `$depth`.
       - Applies pagination (`$start` and `$end`).
     - **HTML Structure**:
       - Opens new `<ul>`/`<li>` for deeper indentation.
       - Closes nested lists for shallower indentation.
       - Renders the entry using the `flexview` display template.
     - **Early Termination**: Skips subsequent entries if `$filter` is `CMS_MENU_FILTER_OPEN` and the current branch is closed.
   - **`CMS_FLEXVIEW_ENTRY_TYPE_END`**: Closes all remaining open lists.

#### Usage Context
- **Internal Use**: Called by `flexview->show_custom()` during menu rendering.
- **Customization**: Override this method to modify menu structure or add custom logic (e.g., badges, counters).

---

### `_ul_li`

#### Purpose
Outputs the opening tags for a new nested list (`<ul><li>`) and applies CSS styling for the list item.

#### Parameters

| Name       | Type     | Description                                                                                     |
|------------|----------|-------------------------------------------------------------------------------------------------|
| `$instance`| `int`    | Unique identifier for the menu instance.                                                        |
| `$index`   | `int`    | Directory index of the current entry.                                                           |
| `$open`    | `bool`   | Whether the current entry is "open" (expanded).                                                 |

#### Return Values
- **`void`**: Outputs HTML directly.

#### Inner Mechanisms
1. Generates a unique `id` for the `<li>` element (e.g., `"menu1-5"`).
2. Calls `_style()` to inject CSS for background images (if enabled).
3. Outputs `<ul><li id="...">`.

#### Usage Context
- **Internal Use**: Called by `show()` when increasing indentation.

---

### `_li`

#### Purpose
Outputs the opening tag for a new list item (`<li>`) and applies CSS styling.

#### Parameters

| Name       | Type     | Description                                                                                     |
|------------|----------|-------------------------------------------------------------------------------------------------|
| `$instance`| `int`    | Unique identifier for the menu instance.                                                        |
| `$index`   | `int`    | Directory index of the current entry.                                                           |
| `$open`    | `bool`   | Whether the current entry is "open" (expanded).                                                 |

#### Return Values
- **`void`**: Outputs HTML directly.

#### Inner Mechanisms
1. Generates a unique `id` for the `<li>` element.
2. Calls `_style()` to inject CSS for background images (if enabled).
3. Outputs `<li id="...">`.

#### Usage Context
- **Internal Use**: Called by `show()` when continuing at the same indentation level.

---

### `_style`

#### Purpose
Generates CSS rules for background images (static and hover) for a menu item. **Note**: The method is currently disabled (returns early) but retains logic for future use.

#### Parameters

| Name     | Type     | Description                                                                                     |
|----------|----------|-------------------------------------------------------------------------------------------------|
| `$id`    | `string` | HTML `id` attribute of the `<li>` element.                                                      |
| `$index` | `int`    | Directory index of the current entry.                                                           |
| `$open`  | `bool`   | Whether the current entry is "open" (expanded).                                                 |

#### Return Values
- **`void`**: Outputs CSS directly (if enabled).

#### Inner Mechanisms
1. **Early Return**: Exits immediately (disabled by default).
2. **Image Handling**:
   - Retrieves the static image URL for the entry.
   - Retrieves the hover image URL (if available).
3. **CSS Generation**:
   - Outputs `<style>` tags with rules for `background-image` (static and hover).

#### Usage Context
- **Internal Use**: Called by `_ul_li()` and `_li()` to style menu items.
- **Customization**: Enable by removing the early `return` to support image-based menus.


<!-- HASH:e7ac8035864cd3d6e4c74adb712f121d -->
