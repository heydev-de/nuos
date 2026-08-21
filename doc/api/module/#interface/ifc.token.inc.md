# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.token.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.token.inc)

- **Version:** `26.8.9.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Token Interface Module (`ifc.token.inc`)

This file implements the user interface for the **Token Management System** in PWNC. Tokens are reusable placeholders (e.g., `%%token_name%%`) that can be inserted into content, templates, or code. The interface allows operators to **create, edit, delete, categorize, and preview** tokens, as well as manage their cache.

---

## Core Functionality

### Overview
- **Purpose**: Provides a full CRUD interface for token management.
- **Permissions**:
  - **Basic Access**: `CMS_L_ACCESS` (read-only).
  - **Operator Access**: `CMS_TOKEN_PERMISSION_OPERATOR` (full control: add/edit/delete/clear cache).
- **State Management**: Uses `cms_cache()` to persist the last selected **object** (token) and **category** per user.
- **Workflow**:
  1. User selects a **category** (or none).
  2. User selects a **token** from the list.
  3. Token **value** and **rendered output** are displayed in an iframe.
  4. Operators can **add/edit/delete** tokens or **rename categories**.

---

## Interface Messages & Handlers

The interface responds to predefined **messages** (`CMS_IFC_MESSAGE`) sent via the PWNC IFC (Interface Controller) system.

---

### `select`
**Purpose**: Updates the token list when a category is selected.
**Parameters**:
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param` | string | Category name (empty for uncategorized). |

**Mechanism**:
- Calls `token_get_index()` to fetch all tokens in the selected category.
- Updates the cached category and resets the object selection.

**Usage**:
- Triggered when the user changes the category dropdown.

---

### `display`
**Purpose**: Renders the selected token’s output in an iframe.
**Parameters**:
| Name      | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$object` | string | Token index (name).                  |

**Mechanism**:
1. Caches the selected token for the current user.
2. Fetches the token data via `$token->get($object)`.
3. Constructs the token placeholder (e.g., `%%token_name%%` or `%%token_name value%%`).
4. Applies the token (renders its content) via `$token->apply()`.
5. Outputs the result in a preview iframe.

**Return**: Exits immediately after rendering the preview.

**Usage Example**:
```php
// URL: ifc_page=token&ifc_message=display&object=site_title
// Output: Renders the token's content in an iframe.
```

---

### `add` / `edit`
**Purpose**: Displays a form to **add** a new token or **edit** an existing one.
**Parameters**:
| Name      | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$object` | string | Token index (for edit mode only).    |

**Mechanism**:
- **Edit Mode**:
  - Validates the token exists.
  - Pre-fills form fields with existing data (`title`, `index`, `value`, `text`).
- **Form Fields**:
  - **Name** (`CMS_L_NAME`): Human-readable title.
  - **Index** (`CMS_L_IFC_TOKEN_003`): Token identifier (e.g., `site_title`).
  - **Value** (`CMS_L_IFC_TOKEN_008`): Optional comma-separated values (e.g., `red,blue,green`).
  - **Text** (`CMS_L_IFC_TOKEN_006`): HTML/Code content (supports placeholders like `[ %token% ]`).
  - **Category** (`CMS_L_IFC_TOKEN_001`): Dropdown of existing categories.

**Client-Side Logic**:
- `token_index()`: Sanitizes the index (replaces spaces with underscores).
- `token_value()`: Sanitizes the value list (removes invalid characters).
- `token_insert()`: Inserts a placeholder (e.g., `[ %token% ]`) into the text field.

**Usage Example**:
```php
// URL: ifc_page=token&ifc_message=add
// Output: Form to create a new token.
```

---

### `_add`
**Purpose**: Processes the **add token** form submission.
**Parameters** (from form):
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | string | Title.                               |
| `$ifc_param2` | string | Index.                               |
| `$ifc_param3` | string | Value.                               |
| `$ifc_param4` | string | Text (content).                      |
| `$ifc_param5` | string | Category.                            |

**Mechanism**:
- Calls `$token->add()` to insert the new token.
- Updates the cached object if successful.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Usage**:
- Triggered when the user submits the "add" form.

---

### `_edit`
**Purpose**: Processes the **edit token** form submission.
**Parameters** (from form):
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$object`    | string | Original token index.                |
| `$ifc_param2` | string | New index (optional).                |
| `$ifc_param3` | string | New value.                           |
| `$ifc_param5` | string | New category.                        |
| `$ifc_param1` | string | New title.                           |
| `$ifc_param4` | string | New text.                            |

**Mechanism**:
- Calls `$token->update()` to modify the token.
- Updates the cached object if successful.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Usage**:
- Triggered when the user submits the "edit" form.

---

### `delete`
**Purpose**: Deletes one or more selected tokens.
**Parameters**:
| Name         | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `$_object`   | string[] | Array of token indices to delete.    |

**Mechanism**:
- Iterates over `$_object` and calls `$token->delete()` for each.
- Updates the cached object list.
- Sets `$ifc_response` to `CMS_MSG_DONE` (if all deletions succeed) or `CMS_MSG_ERROR`.

**Usage Example**:
```php
// URL: ifc_page=token&ifc_message=delete&_object[]=old_token
// Action: Deletes the token "old_token".
```

---

### `category_rename`
**Purpose**: Displays a form to rename a category.
**Parameters**:
| Name      | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$object` | string | Token index (to identify the category). |

**Mechanism**:
- Fetches the category name via `token_get_category()`.
- Displays a form with the current category name pre-filled.

**Usage**:
- Triggered when the user clicks "Rename Category".

---

### `_category_rename`
**Purpose**: Processes the **rename category** form submission.
**Parameters** (from form):
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | string | New category name.                   |
| `$object`    | string | Token index (to identify the category). |

**Mechanism**:
- Executes a SQL `UPDATE` to rename all tokens in the category.
- Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Usage**:
- Triggered when the user submits the "rename category" form.

---

### `clear_cache`
**Purpose**: Clears the token and content caches.
**Permissions**: Requires operator access (`$token->operator`).

**Mechanism**:
1. Sets a 10-minute timeout (`set_time_limit(600)`).
2. Loads the `filemanager` module.
3. Deletes the following directories (if they exist):
   - `CMS_DATA_PATH . "#token/cache/"`
   - `CMS_DATA_PATH . "#content/cache/"`
4. Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Usage Example**:
```php
// URL: ifc_page=token&ifc_message=clear_cache
// Action: Clears all token and content caches.
```

---

## Helper Functions

### `token_get_index()`
**Purpose**: Fetches all tokens in a given category.
**Parameters**:
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$token`  | `token`  | Token instance.                      |
| `$category` | string | Category name (empty for uncategorized). |

**Return**:
- `string[]`: Array of token indices.
- `FALSE`: If no tokens exist in the category.

**Mechanism**:
- Queries the database for tokens in the specified category.
- Returns an array of indices sorted naturally (case-insensitive).

**Usage Example**:
```php
$tokens = token_get_index($token, "site");
/* Returns: ["site_title", "site_description"] */
```

---

### `token_get_category()`
**Purpose**: Fetches the category of a given token.
**Parameters**:
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$token`  | `token`  | Token instance.                      |
| `$object` | string   | Token index.                         |

**Return**:
- `string`: Category name.
- `FALSE`: If the token does not exist.

**Mechanism**:
- Queries the database for the token’s category.

**Usage Example**:
```php
$category = token_get_category($token, "site_title");
/* Returns: "site" */
```

---

### `token_get_select()`
**Purpose**: Generates a dropdown of all existing categories.
**Parameters**:
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$token`  | `token`  | Token instance.                      |

**Return**: `string` (HTML `<select>` element).

**Mechanism**:
- Queries the database for distinct categories.
- Generates an `<option>` for each category.

**Usage Example**:
```php
echo token_get_select($token);
// Output: <select>...</select> with all categories.
```

---

## Main Display Logic

### Overview
- **Category Selection**: Dropdown to filter tokens by category.
- **Token List**: Checkbox list of tokens in the selected category.
- **Token Preview**: Iframe displaying the rendered token content.
- **Menu**: Contextual actions (add/edit/delete/rename/clear cache).

### Workflow
1. **Category Handling**:
   - If `$object` is set, fetches its category and caches it.
   - If no `$object` is set, uses the cached category to fetch tokens.
2. **Menu Construction**:
   - **Insert**: Available if a token is selected (returns the token placeholder to the caller).
   - **Add/Edit/Delete/Rename**: Available only for operators.
   - **Clear Cache**: Available only for operators.
3. **Token List**:
   - Displays tokens in the selected category as checkboxes.
   - Supports **select all/invert/none** actions.
4. **Token Preview**:
   - Displays the rendered output of the selected token in an iframe.

### Client-Side Logic
- `token_select(value)`: Updates the preview iframe when a token is selected.
- `ifc_list_activate()` / `ifc_list_invert()` / `ifc_list_deactivate()`: Manages checkbox states in the token list.

---

## Usage Example: Creating a Token

### Scenario
An operator wants to create a token for the site’s footer text.

### Steps
1. **Navigate to the Token Interface**:
   - URL: `ifc_page=token`
2. **Add a New Token**:
   - Click **"Add"** in the menu.
   - Fill the form:
     - **Name**: `Site Footer`
     - **Index**: `site_footer`
     - **Value**: (leave empty)
     - **Text**: `<p>© 2026 My Website. All rights reserved.</p>`
     - **Category**: `site`
   - Submit the form.
3. **Verify**:
   - The token `%%site_footer%%` now appears in the list.
   - The preview iframe shows the rendered HTML.

### Code Equivalent
```php
// Simulate form submission (operator-only)
$token->add(
    "site_footer",  // index
    "",             // value
    "site",         // category
    "Site Footer",  // title
    "<p>© 2026 My Website. All rights reserved.</p>"  // text
);
```

---

## Usage Example: Using a Token in Content

### Scenario
A content editor wants to insert the `site_footer` token into a page.

### Steps
1. **Open the Token Interface**:
   - URL: `ifc_page=token`
2. **Select the Token**:
   - Choose the `site` category.
   - Select `site_footer` from the list.
3. **Insert the Token**:
   - Click **"Insert"** in the menu.
   - The interface returns `%%site_footer%%` to the caller (e.g., a content editor).

### Code Equivalent
```php
// In a content template or editor:
echo $token->apply("%%site_footer%%");
// Output: <p>© 2026 My Website. All rights reserved.</p>
```


<!-- HASH:d2e599d430d50d3f4c64d983300ba30d -->
