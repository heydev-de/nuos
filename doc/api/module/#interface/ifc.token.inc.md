# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.token.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Token Interface Module (`ifc.token.inc`)

This file implements the **Token Management Interface** for the NUOS platform, providing a user interface for creating, editing, deleting, and organizing tokens. Tokens are reusable placeholders (e.g., `%%token_name%%`) that can be inserted into content, templates, or code, and are replaced with predefined values or dynamic content at runtime.

The interface integrates with the **Token API** (`token` class) and the **IFC (Interface Controller)** system to deliver a full CRUD (Create, Read, Update, Delete) experience with real-time previews, category-based organization, and secure parameter handling.

---

## Core Dependencies & Initialization

### Libraries & Permissions
- **`cms_load("token")`**: Loads the core `token` library. If loading fails, the interface is deactivated.
- **`ifc_permission()`**: Ensures the current user has at least **access-level permissions** (`CMS_L_ACCESS`). Operators (`CMS_L_OPERATOR`) gain additional controls (add/edit/delete).
- **`$token = new token()`**: Instantiates the token manager. If tokens are disabled (`$token->enabled === FALSE`), the interface is deactivated.

### Object & State Management
- **`$object`**: Represents the currently selected token index (e.g., `site_title`). If empty, it is restored from user-specific cache: `token.{CMS_USER}.object`.
- **Sanitization**: `$object` is truncated at the first whitespace or newline to prevent injection.

---

## Message Handling (Sub-Display Logic)

The interface responds to **IFC messages** (`CMS_IFC_MESSAGE`) to perform specific actions. Each case corresponds to a user interaction (e.g., add, edit, delete).

---

### `case "select"`
**Purpose**: Updates the current token selection based on user input (e.g., category change).

**Parameters**:
| Name         | Type   | Description |
|--------------|--------|-------------|
| `$ifc_param` | string | New category or token index passed from the interface. |

**Mechanism**:
- Calls `token_get_index($token, $ifc_param)` to resolve the first token in the given category.
- Updates the current `$object` and implicitly refreshes the display.

**Usage**: Triggered when the user selects a category from the dropdown.

---

### `case "display"`
**Purpose**: Renders a real-time preview of the selected token's resolved value.

**Mechanism**:
1. Caches the current `$object` for the user.
2. Retrieves the token data via `$token->get($object)`.
3. Constructs the token syntax:
   - If no value: `%%$object%%`
   - With value: `%%$object value1,value2%%`
4. Applies the token (resolves placeholders) using `$token->apply()`.
5. Outputs the result via `preview()` and exits to prevent further rendering.

**Usage**: Used in the right-hand iframe to show the token's live output.

---

### `case "add"` / `case "edit"`
**Purpose**: Displays a form to add a new token or edit an existing one.

**Parameters (for edit)**:
| Name         | Type   | Description |
|--------------|--------|-------------|
| `$object`    | string | Token index to edit. |

**Mechanism**:
- For **edit**, retrieves the token data via `$token->get($object)` and populates form fields (`$ifc_param1`–`$ifc_param4`).
- Creates an IFC form with fields for:
  - **Name** (title, 40 chars max)
  - **Index** (token identifier, 40 chars max, sanitized to underscores)
  - **Value** (comma-separated list, sanitized to alphanumeric + underscores)
  - **Text** (HTML code, 65536 chars max)
- Includes JavaScript (`token_index()`, `token_value()`) to sanitize inputs in real time.
- Renders a dropdown of value placeholders for easy insertion into the text field.

**Usage**: Triggered via the "Add" or "Edit" menu commands.

---

### `case "_add"`
**Purpose**: Processes the submitted "Add Token" form.

**Parameters**:
| Name         | Type   | Description |
|--------------|--------|-------------|
| `$ifc_param1` | string | Token title (name). |
| `$ifc_param2` | string | Token index (identifier). |
| `$ifc_param3` | string | Token value (comma-separated). |
| `$ifc_param4` | string | Token text (HTML content). |
| `$ifc_param5` | string | Token category. |

**Mechanism**:
- Calls `$token->add()` with the submitted data.
- On success, updates `$object` to the new token index and sets `$ifc_response = CMS_MSG_DONE`.
- On failure, sets `$ifc_response = CMS_MSG_ERROR`.

**Return**: Boolean-like via `$ifc_response`.

---

### `case "_edit"`
**Purpose**: Processes the submitted "Edit Token" form.

**Parameters**: Same as `_add`, plus:
| Name         | Type   | Description |
|--------------|--------|-------------|
| `$object`    | string | Original token index. |

**Mechanism**:
- Calls `$token->update()` with the original index and new data.
- On success, updates `$object` to the new index (if changed) and sets `$ifc_response = CMS_MSG_DONE`.
- On failure, sets `$ifc_response = CMS_MSG_ERROR`.

---

### `case "delete"`
**Purpose**: Deletes one or more selected tokens.

**Parameters**:
| Name         | Type   | Description |
|--------------|--------|-------------|
| `$_object`   | array  | Array of token indices to delete. |

**Mechanism**:
- Iterates over `$_object`, sanitizes each value, and calls `$token->delete()`.
- Uses bitwise AND (`&=`) to track errors across all deletions.
- Refreshes the token list for the current category.
- Sets `$ifc_response` to `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).

**Usage**: Triggered via the "Delete" menu command after selecting tokens.

---

### `case "category_rename"` / `case "_category_rename"`
**Purpose**: Renames a token category.

**Parameters**:
| Name         | Type   | Description |
|--------------|--------|-------------|
| `$object`    | string | Token index (used to resolve current category). |
| `$ifc_param1` | string | New category name. |

**Mechanism**:
- **Display**: Shows a form with the current category name.
- **Process**: Executes a direct SQL `UPDATE` on `CMS_DB_TOKEN` to rename all tokens in the category.
- Uses `sqlesc()` for security.

**Note**: Response is always `CMS_MSG_DONE` (even on failure, which is a bug — should check affected rows).

---

## Main Display Logic

### Category & Object Resolution
- If `$object` is set, resolves its category via `token_get_category()` and caches it.
- If no `$object`, restores the last used category from cache and fetches the first token in that category.

### Menu Construction
| Menu Item | Condition | Action |
|---------|-----------|--------|
| Insert | `CMS_IFC_SELECT && $object != ""` | Returns the selected token to the calling interface. |
| Add | `$token->operator` | Triggers "add" form. |
| Edit | `$token->operator && $object != ""` | Triggers "edit" form. |
| Delete | `$token->operator && $object != ""` | Triggers "delete" flow. |
| Rename Category | `$token->operator && $object != ""` | Triggers "category_rename" form. |

### UI Components
- **Category Dropdown**: Lists all distinct categories from the database. On change, triggers `ifc_post('select', value)`.
- **Token List**: Displays tokens in the current category as checkboxes. Supports multi-select for deletion.
  - Uses `ifc_custom_select()` for enhanced UI (e.g., "Select All", "Invert").
- **Token Syntax Display**: Shows the current token in `%%index%%` or `%%index value%%` format.
- **Preview Iframe**: Embeds the "display" message output to show the token's resolved content.

### JavaScript: `token_select(value)`
**Purpose**: Updates the preview when a token is selected.

**Mechanism**:
- Sets the `object` and `token` fields.
- Dynamically updates the iframe `src` to reload the preview with the new token.

---

## Helper Functions (Used but Not Defined Here)

| Function | Purpose |
|--------|---------|
| `token_get_index($token, $category)` | Returns the first token index in a given category. |
| `token_get_category($token, $object)` | Returns the category of a given token. |
| `token_get_select($token)` | Returns a dropdown of categories for use in forms. |
| `strabridge($str, $len)` | Truncates a string to `$len` chars with ellipsis. |
| `l($str)` | Localization helper. |
| `jscript($code)` | Outputs JavaScript safely. |
| `ifc_table_open()` / `ifc_table_close()` | Wraps content in a styled table. |

---

## Security & Sanitization
- All user inputs are sanitized:
  - `$object` truncated at first whitespace.
  - Token index/value sanitized via JavaScript regex: `[^0-9a-zA-Z_,]` → `_`.
  - SQL values escaped via `sqlesc()`.
  - Output escaped via `x()` (XML/HTML escaping).
- CSRF protection is handled by the IFC system (`ifc_post()`).

---

## Usage Scenarios

### 1. Adding a New Token
1. User clicks "Add".
2. Fills in name, index, value, and text.
3. Submits form → `_add` processes it.
4. Token appears in the list and can be previewed.

### 2. Editing a Token
1. User selects a token and clicks "Edit".
2. Form is pre-populated with current data.
3. User modifies fields and submits → `_edit` updates the token.

### 3. Deleting Tokens
1. User selects one or more tokens.
2. Clicks "Delete" → `delete` case removes them.

### 4. Renaming a Category
1. User selects a token in the category.
2. Clicks "Rename Category".
3. Enters new name → `_category_rename` updates all tokens in the category.

### 5. Inserting a Token into Content
1. User selects a token.
2. Clicks "Insert" → IFC returns the token syntax (e.g., `%%site_title%%`) to the calling interface.

---

## Database Schema (Implied)
| Column | Type | Description |
|-------|------|-------------|
| `CMS_DB_TOKEN_INDEX` | string | Token identifier (e.g., `site_title`). |
| `CMS_DB_TOKEN_VALUE` | string | Optional comma-separated values. |
| `CMS_DB_TOKEN_TITLE` | string | Human-readable name. |
| `CMS_DB_TOKEN_TEXT` | text | HTML content to replace the token. |
| `CMS_DB_TOKEN_CATEGORY` | string | Optional grouping label. |

---

## Notes
- The interface is **operator-only** for write operations.
- Real-time preview enhances usability.
- Category management is built-in but optional.
- The code assumes UTF-8 and uses multibyte-safe functions where needed.
- All UI strings are localized via `CMS_L_*` constants.


<!-- HASH:ff55bdf3dcb068a8936d605d38412800 -->
