# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.template.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.template.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template Interface

The `ifc.template.inc` file is the interface controller for the **Template** module in PWNC. It handles all user interactions related to managing templates within the CMS, including creating, editing, deleting, exporting, and viewing template source code. This interface supports multilingual templates and integrates with other modules like Content, Directory, Image, Media, Download, and Token.

### Key Responsibilities

- **Template Management**: Add, edit, delete, and rename template categories.
- **Source Code Editing**: View and edit HTML, CSS (stylesheet), and JavaScript code for templates.
- **Multilingual Support**: Manage translations across enabled languages.
- **Content Export**: Export content structures as reusable templates.
- **UI Integration**: Provides a rich interface with tabs, buttons, and dynamic selects for inserting CMS tags.

---

## Constants and Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IFC_MESSAGE` | Dynamic | Current action/message being processed (e.g., `select`, `display`, `add`, `edit`, `delete`). |
| `CMS_USER` | Dynamic | Current user identifier used in cache keys. |
| `CMS_LANGUAGE` | Dynamic | Default system language. |
| `CMS_LANGUAGE_ENABLED` | Comma-separated list | List of enabled languages for multilingual support. |
| `CMS_L_ACCESS` | String | Access level constant for basic access. |
| `CMS_TEMPLATE_PERMISSION_OPERATOR` | String | Permission level required for operators. |
| `CMS_L_OPERATOR` | String | Operator role label. |
| `CMS_L_COMMAND_ADD` | String | Label for "Add" command. |
| `CMS_L_COMMAND_EDIT` | String | Label for "Edit" command. |
| `CMS_L_COMMAND_DELETE` | String | Label for "Delete" command. |
| `CMS_L_COMMAND_EXPORT` | String | Label for "Export" command. |
| `CMS_L_COMMAND_CONFIRM` | String | Label for "Confirm" command. |
| `CMS_L_COMMAND_SAVE` | String | Label for "Save" command. |
| `CMS_L_COMMAND_CANCEL` | String | Label for "Cancel" command. |
| `CMS_L_COMMAND_INSERT` | String | Label for "Insert" command. |
| `CMS_L_NAME` | String | Label for "Name" field. |
| `CMS_L_ALL` | String | Label for "Select All" button. |
| `CMS_L_INVERT` | String | Label for "Invert Selection" button. |
| `CMS_L_NONE` | String | Label for "Clear Selection" button. |
| `CMS_L_IFC_TEMPLATE_*` | Various | Localized labels specific to the template interface. |
| `CMS_MSG_DONE` | String | Success message constant. |
| `CMS_MSG_ERROR` | String | Error message constant. |
| `CMS_DOCTYPE_HTML` | String | HTML doctype declaration. |
| `CMS_DATA_PATH` | Path | Filesystem path to data directory. |
| `CMS_DATA_URL` | URL | Web-accessible URL to data directory. |
| `CMS_ROOT_PATH` | Path | Root filesystem path of the application. |

---

## Initialization

### Library Loading and Permissions

```php
if (! cms_load("template")) ifc_inactive($ifc_page);
```

Loads the `template` library. If loading fails, marks the interface as inactive.

```php
ifc_permission(["" => CMS_L_ACCESS, CMS_TEMPLATE_PERMISSION_OPERATOR => CMS_L_OPERATOR]);
```

Sets up permission levels for accessing the interface.

```php
cms_cache_init($object, "template." . CMS_USER . ".object");
```

Initializes the selected object from permanent cache using the current user's context.

```php
init($language);
```

Initializes the language variable based on global state or defaults.

---

## Message Handling

The main logic is driven by a `switch` statement on `CMS_IFC_MESSAGE`, which determines the current operation.

### Case: `select`

Handles selection of a template object or language.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$object` | String | Selected template object name. |
| `$ifc_param` | Mixed | New value for object or language. |
| `$language` | String | Current language context. |

#### Mechanism

Calls `language_set()` to update the selected object or language in the multilingual mapping.

#### Usage Example

When a user selects a different template from the list, this case updates the active object reference.

---

### Case: `select_language`

Updates the current working language.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$ifc_param` | String | New language identifier. |

#### Mechanism

Directly assigns the new language to `$language`.

#### Usage Example

User switches from English to French via the language selector dropdown.

---

### Case: `display`

Renders a live preview of the selected template.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$object` | String | Template object to display. |

#### Mechanism

Instantiates a `template` instance, caches the object, calls `template_preview()`, retrieves category info, and exits after rendering.

#### Usage Example

Used when an iframe needs to show a real-time preview of a template during editing.

---

### Case: `_display`

Internal handler for rendering previews without caching.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$object` | String | Template object to render. |

#### Mechanism

Calls `template_preview()` with flags to disable caching and exit immediately.

#### Usage Example

Called internally by AJAX requests to refresh the preview pane.

---

### Cases: `add`, `edit`

Displays the form for adding or editing a template.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$object` | String | Existing template object (for edit). |
| `$language` | String | Current language context. |
| `$ifc_param1` | String | Template name input. |
| `$ifc_param2` | Boolean | Whether it's a page template. |
| `$ifc_param3` | Integer | Source option (1=keep, 2=URL, 3=upload). |
| `$ifc_param4` | String | URL or uploaded file content. |
| `$ifc_param5` | String | Category selection. |

#### Mechanism

Creates an `ifc` form instance, sets fields for name, page flag, code source options, and category dropdown. Uses `language_get()` to resolve the correct object per language.

#### Usage Example

User clicks "Edit" on a template row; this case renders the edit form pre-filled with existing values.

---

### Cases: `_add`, `_edit`

Processes submitted forms for adding or editing templates.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$ifc_param1` | String | Template name. |
| `$ifc_param2` | String | Category. |
| `$ifc_param3` | Integer | Source option (1=keep, 2=URL, 3=upload). |
| `$ifc_param4` | String | URL or file content. |
| `$ifc_param5` | String | Category. |
| `$ifc_file1` | File | Uploaded file (if applicable). |

#### Mechanism

Depending on the source option:
- Keeps existing code
- Reads from a remote URL
- Reads from an uploaded file

Then either adds a new template or updates an existing one using `template->add()` or `template->set()` + `template->set_code()`.

#### Usage Example

After submitting the add/edit form, this case processes the inputs and saves the template.

---

### Case: `export`

Displays the export form for converting content into a template.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$object` | String | Target template object. |
| `$language` | String | Current language. |
| `$content_index` | String | Index of content to export. |
| `$content_range` | Optional | Range within content. |

#### Mechanism

Checks if the `content` module is available, creates an `ifc` form with name and category fields, and prepares for submission.

#### Usage Example

User wants to convert a structured content block into a reusable template.

---

### Case: `_export`

Executes the actual export process.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$ifc_param1` | String | New template name. |
| `$ifc_param2` | String | Category. |
| `$content_index` | String | Content index to export. |
| `$content_range` | Optional | Range within content. |

#### Mechanism

Loads the `content` library, calls `content_template_export()` to generate code, stylesheets, and scripts, then adds a new template using `template->add()`.

#### Usage Example

After submitting the export form, this case generates and stores the new template.

---

### Cases: `source`, `_source`, `__source`, `___source`

Manages the source code editor for templates.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$object` | String | Template object to edit. |
| `$language` | String | Current language. |
| `$ifc_param1` | String | HTML code. |
| `$ifc_param2` | String | Stylesheet code. |
| `$ifc_param3` | String | JavaScript code. |

#### Mechanism

- For `_source`, `__source`, `___source`: Saves the provided code snippets.
- For `source`: Renders the full source editor UI with tabs for HTML/CSS/JS, prefabs dropdown, and integration buttons for other modules.

#### Usage Example

User opens the source editor to modify the raw HTML/CSS/JS of a template.

---

### Case: `delete`

Deletes one or more selected templates.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$_object` | Array | List of template objects to delete. |

#### Mechanism

Iterates through selected objects, deletes each using `template->delete()`, and cleans up language mappings. Updates the active object if necessary.

#### Usage Example

User selects multiple templates and clicks "Delete".

---

### Case: `category_rename`

Renames the category of a selected template.

#### Parameters

| Parameter | Type | Description |
|----------|------|-------------|
| `$_object` | String | Selected template object. |
| `$category` | String | Current category. |
| `$ifc_param1` | String | New category name. |

#### Mechanism

Displays a form to enter a new category name, then applies it to all templates in the same category.

#### Usage Example

User renames a group of templates from "Blog" to "News".

---

## Main Display

After processing any message, the main display section renders the template management UI.

### Components

- **Category Selector**: Dropdown to filter templates by category.
- **Template List**: Checkbox list of templates in the selected category.
- **Language Switcher**: If multilingual is enabled, shows flags for switching languages.
- **Preview Pane**: An iframe showing a live preview of the selected template.
- **Action Menu**: Buttons for Add, Edit, Delete, Export, etc., depending on permissions.

### JavaScript Functions

#### `template_select(value)`

Updates the selected template object and refreshes the preview iframe.

#### `template_source_insert(value)`

Inserts predefined CMS tag snippets into the source editor based on user selection from the prefabs dropdown.

#### `template_source_save(message)`

Saves the current state of the source editor, either closing, saving, or showing the final result.

#### `template_source_save_callback(value)`

Handles the response after saving and optionally reloads the preview.

---

## Usage Examples

### Adding a New Template

1. User navigates to the Template interface.
2. Clicks "Add".
3. Fills in the name, selects a category, chooses a source method (keep empty, from URL, or upload).
4. Submits the form.
5. The `_add` case processes the input and creates the template.

### Editing Source Code

1. User selects a template.
2. Clicks "Code" (source editor).
3. Edits HTML/CSS/JS in respective tabs.
4. Uses the prefabs dropdown to insert common CMS tags.
5. Saves changes using the save button.

### Exporting Content as Template

1. User selects a content item.
2. Clicks "Export".
3. Enters a name and selects a category.
4. Submits the form.
5. The `_export` case converts the content structure into a template.

---

## Summary

This interface file orchestrates the entire lifecycle of template management in PWNC, from creation to deletion, with strong support for multilingual environments and seamless integration with other modules. Its modular design allows for extensibility while maintaining a clean separation between presentation and business logic.


<!-- HASH:44a8f78d8dda3dcced3d1a3a57cd74b5 -->
