# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.token.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.token.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Token Interface

The `ifc.token.inc` file is the interface controller for managing **tokens** within the PWNC Web Platform. Tokens are reusable content placeholders that can be inserted into pages, templates, or other content areas using a `%%token_name%%` syntax. This interface allows users to create, edit, delete, and organize tokens by category, as well as preview their rendered output in real-time.

The interface supports several operations:
- **Display**: Preview a token's rendered value.
- **Add/Edit**: Create or modify a token's properties (name, index, value, text, category).
- **Delete**: Remove one or more tokens.
- **Category Rename**: Rename the category of a token.
- **Clear Cache**: Clear cached token and content data.

---

### Initialization & Setup

At the top of the file, the interface initializes the token library, checks permissions, and prepares the token object:

| Variable | Description |
|---------|-------------|
| `$token` | Instance of the `token` class used for all token operations. |
| `$object` | The currently selected token index. If blank, it is loaded from cache. |

#### Key Behaviors:
- Loads the `token` library via `cms_load("token")`.
- Checks user permissions using `ifc_permission()`.
- Instantiates the `token` class and verifies if it is enabled.
- Retrieves or sanitizes the current token object identifier.

---

### Message Handling (`switch (CMS_IFC_MESSAGE)`)

This section handles different actions based on the `CMS_IFC_MESSAGE` constant, which determines what action the interface should perform.

#### `select`

##### Purpose
Sets the currently selected token based on the provided parameter.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | string | The token index to select. |

##### Mechanism
Calls `token_get_index()` to resolve the token index from the given parameter.

##### Usage Example
When a user selects a token from the list, this case sets the active token for further operations like editing or displaying.

---

#### `display`

##### Purpose
Displays a live preview of the selected token's rendered content.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | The token index to display. |

##### Mechanism
- Caches the selected token object.
- Fetches the token data using `$token->get($object)`.
- Constructs a placeholder string (`%%token%%` or `%%token value%%`) and renders it using `$token->apply()`.
- Outputs the result via `preview()`.

##### Usage Example
Used when a user wants to see how a token will appear when rendered in content.

---

#### `add` / `edit`

##### Purpose
Renders a form to add a new token or edit an existing one.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | Token index (empty for add, filled for edit). |
| `$ifc_param1` | string | Title of the token. |
| `$ifc_param2` | string | Index of the token. |
| `$ifc_param3` | string | Value(s) associated with the token. |
| `$ifc_param4` | string | Text content of the token. |
| `$ifc_param5` | string | Category of the token. |

##### Mechanism
- For **edit**, loads existing token data into form fields.
- Creates an `ifc` form instance with appropriate labels and input types.
- Includes JavaScript functions to standardize token index and value formatting.
- Renders a dropdown of available token values for insertion.
- Displays a rich text editor for the token's body content.
- Shows a category selection dropdown.

##### Usage Example
When a user clicks "Add" or "Edit", this case generates the full form UI for creating or modifying a token.

---

#### `_add`

##### Purpose
Processes the submission of a new token.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | Title of the token. |
| `$ifc_param2` | string | Index of the token. |
| `$ifc_param3` | string | Value(s) associated with the token. |
| `$ifc_param4` | string | Text content of the token. |
| `$ifc_param5` | string | Category of the token. |

##### Mechanism
Calls `$token->add()` with the submitted parameters. On success, updates the response status and sets the new token as the current object.

##### Usage Example
Triggered when the user submits the "Add Token" form.

---

#### `_edit`

##### Purpose
Processes the submission of an edited token.

##### Parameters
Same as `_add`, plus:
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | Original token index being edited. |

##### Mechanism
Calls `$token->update()` with the original index and new values. On success, updates the response status and sets the updated token as the current object.

##### Usage Example
Triggered when the user submits the "Edit Token" form.

---

#### `delete`

##### Purpose
Deletes one or more selected tokens.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$_object` | array | Array of token indices to delete. |

##### Mechanism
Iterates over selected tokens and calls `$token->delete()` for each. Sets the response status based on whether all deletions succeeded.

##### Usage Example
When a user selects multiple tokens and clicks "Delete".

---

#### `category_rename`

##### Purpose
Renders a form to rename the category of the current token.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | Current token index. |
| `$category` | string | Current category name. |

##### Mechanism
Creates an `ifc` form with a single text field pre-filled with the current category name.

##### Usage Example
When a user wants to rename the category of a token.

---

#### `_category_rename`

##### Purpose
Processes the submission of a renamed category.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | New category name. |
| `$object` | string | Current token index. |

##### Mechanism
Executes a direct SQL `UPDATE` query to rename the category across all tokens in that category.

##### Usage Example
Triggered when the user submits the "Rename Category" form.

---

#### `clear_cache`

##### Purpose
Clears cached token and content data from disk.

##### Mechanism
- Verifies operator permission.
- Loads the `filemanager` library.
- Deletes the `#token/cache/` and `#content/cache/` directories if they exist.
- Sets the response status based on success.

##### Usage Example
When an operator needs to force-refresh cached token outputs.

---

### Main Display Section

After processing the message, the interface renders the main token management view.

#### Category Resolution
- If a token object is set, its category is determined and cached.
- If no object is set, the category is retrieved from cache, and a default token is selected.

#### Menu Construction
Builds a contextual menu based on user permissions and current state:

| Condition | Menu Item |
|----------|-----------|
| Selection made | Insert |
| Operator access | Add, Edit, Delete, Rename Category, Clear Cache |

#### UI Rendering
- Opens a table layout with two columns:
  - Left: Category selector and token list.
  - Right: Live preview iframe.
- Category dropdown populated from database.
- Token list rendered as checkboxes with labels.
- Token preview shown in an iframe that loads the display view.
- Includes JavaScript for selecting tokens and updating the preview.

---

### JavaScript Functions

#### `token_select(value)`
##### Purpose
Updates the selected token and refreshes the preview iframe.

##### Parameters
| Name | Type | Description |
|------|------|-------------|
| `value` | string | The token index to select. |

##### Mechanism
- Sets the hidden `object` and `token` form fields.
- Updates the iframe source URL with the new token index.

##### Usage Example
Called when a user clicks on a token in the list.

---

### Helper Functions Used

| Function | Description |
|---------|-------------|
| `token_get_index()` | Resolves a token index from a category or parameter. |
| `token_get_category()` | Gets the category of a given token. |
| `token_get_select()` | Generates a category selection dropdown. |
| `cms_cache()` | Manages caching of token objects and categories. |
| `cms_url()` | Generates URLs for interface navigation. |
| `sqlesc()` | Escapes strings for safe SQL queries. |
| `x()` | Escapes strings for safe HTML/XML output. |
| `preview()` | Outputs rendered content for preview. |
| `ifc_table_open/close()` | Wraps the main display in a styled table. |

---

### Summary

The `ifc.token.inc` file provides a complete interface for managing tokens in the PWNC Web Platform. It supports CRUD operations, category management, real-time previews, and cache control, all while maintaining security through proper escaping and permission checks. The interface is designed for both content editors and operators, offering a streamlined workflow for token creation and maintenance.


<!-- HASH:d2e599d430d50d3f4c64d983300ba30d -->
