# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.mailform.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Mailform Interface Module (`ifc.mailform.inc`)

This file implements the **Mailform Interface** for the NUOS web platform, providing a visual editor for creating and managing hierarchical mail form structures. It enables operators to design complex forms with various field types (containers, text inputs, checkboxes, radio buttons, etc.), configure email submission settings, and test SMTP functionality.

The interface leverages the **FlexView** component for drag-and-drop hierarchy management and the **Data** class for persistent storage of form configurations in the `#system/mailform` data structure.

---

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MAILFORM_PERMISSION_OPERATOR` | `"operator"` | Permission level required for advanced operations (e.g., SMTP configuration). |

---

## Core Workflow

### Initialization
1. **Library Loading**: Ensures `flexview` is loaded; deactivates the interface if unavailable.
2. **Permission Check**: Validates user permissions (`CMS_L_ACCESS` for basic access, `CMS_L_OPERATOR` for advanced operations).
3. **State Management**: Caches the currently selected form object per user.

---

## Message Handling (`CMS_IFC_MESSAGE` Switch)

Handles interface actions triggered by user interactions (e.g., adding fields, saving configurations).

### `select`
**Purpose**: Updates the currently selected form object.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Index of the selected object in the data structure. |
**Mechanism**: Sets `$object` to the provided index and updates the user-specific cache.

---

### `display`
**Purpose**: Renders a preview of the selected form.
**Mechanism**:
1. Generates a PHP snippet to set the global `mailform_form` variable.
2. Invokes the `mailform` application via `cms_application()`.
3. Uses `template_preview()` to display the form in an isolated context.

---

### `add_receiver` / `add_element`
**Purpose**: Initiates the addition of a new form container (receiver) or field element.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Target parent object index. |
| `$ifc_param1` | `string` | Default name for the new element (localized). |
| `$ifc_param2` | `string` | Field subtype (e.g., `"text"`, `"checkbox"`). Defaults to `"text"` for elements. |
**Mechanism**:
1. Sets default names using language constants (`CMS_L_IFC_MAILFORM_001` for receivers, `CMS_L_IFC_MAILFORM_009` for elements).
2. Delegates to `add_receiver_target`/`add_element_target` for further processing.

---

### `add_receiver_target` / `add_element_target`
**Purpose**: Displays a dialog to select the insertion target for a new receiver/element.
**UI Components**:
- **Data Column**: Input field for the element name.
- **Target Column**: FlexView hierarchy showing valid insertion points.
**Mechanism**:
- Uses `flexview->show_target()` to render a draggable hierarchy with insertion/append options.
- Validates target types (e.g., containers can only append to other containers).

---

### `add_receiver_insert` / `add_receiver_append`
**Purpose**: Inserts/appends a new container (receiver) to the form hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Target parent index. |
| `$ifc_param1` | `string` | Container name (localized if empty). |
**Mechanism**:
1. Creates a new container in the `#system/mailform` data structure.
2. Generates a **template** for the container (if `template` library is loaded) to enable standalone form rendering.
3. Saves the data and updates `$object` to the new container’s index.

---

### `add_element_insert` / `add_element_append`
**Purpose**: Inserts/appends a new field element to the form hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Target parent index. |
| `$ifc_param1` | `string` | Element name (localized if empty). |
| `$ifc_param2` | `string` | Field type (e.g., `"text"`, `"checkbox"`). |
**Mechanism**:
1. Sets default values (e.g., HTML input code for `"code"` type).
2. Creates the element in the data structure and saves it.
3. Updates `$object` to the new element’s index.

---

### `save`
**Purpose**: Saves configuration changes for the selected form object.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Name (localized if empty). |
| `$ifc_param2`–`$ifc_param11` | `mixed` | Field-specific values (e.g., description, email, validation regex). |
**Mechanism**:
- **Container Fields**: Saves metadata (description, email, URL, confirmation text, CAPTCHA flag).
- **Field Types**: Saves type-specific properties (e.g., `id`, `description`, `required` flag).
- Updates associated templates if the object is a container.

---

### `copy_insert` / `copy_append` / `cut_insert` / `cut_append`
**Purpose**: Copies or moves elements within the form hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Comma-separated source and target indices (e.g., `"source_index,target_index"`). |
**Mechanism**:
1. **Validation**: Ensures containers are only appended to other containers.
2. **Cloning**: Uses a cloned `data` object to buffer the source element.
3. **Paste**: Inserts/appends the buffered element to the target.
4. **Template Handling**: Generates a new template for copied containers.
5. **State Update**: Adjusts `$object` to reflect the new hierarchy.

---

### `del`
**Purpose**: Deletes the selected form object.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Index of the object to delete. |
**Mechanism**:
1. Traverses the hierarchy to find the parent path.
2. Deletes the object and its associated template (if any).
3. Selects the nearest existing parent object post-deletion.

---

### `config` / `_config`
**Purpose**: Manages global email submission settings (SMTP/mail).
**Parameters** (`_config`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Sender email address. |
| `$ifc_param2` | `string` | Email method (`"mail"` or custom). |
| `$ifc_param3` | `string` | SMTP server address. |
| `$ifc_param4` | `string` | SMTP username. |
| `$ifc_param5` | `string` | SMTP password. |
**Mechanism**:
- **`config`**: Displays a form with current system email settings.
- **`_config`**: Saves settings to the `system` data structure.

---

### `test`
**Purpose**: Tests SMTP connectivity and email sending.
**Mechanism**:
1. Validates SMTP availability.
2. Sends a test email using the `mime` and `smtp` libraries.
3. Returns success/error feedback.

---

## Main Display

### Hierarchy Visualization
- **FlexView Integration**: Renders the form hierarchy with drag-and-drop support.
- **Icons**: Type-specific icons for containers, fields, and page breaks.
- **Event Handling**: JavaScript callbacks for drag-and-drop operations (`mailform_flexview_event`).

### Field Configuration Panel
- **Dynamic UI**: Renders type-specific fields (e.g., validation regex for `text`, options for `checkbox`).
- **Localization**: Supports multilingual field names and descriptions.
- **Preview Button**: Enables real-time form previews.

### Trash Bin
- **Deletion**: Drag elements to the trash bin to delete them.

---

## Key Classes and Dependencies

| Class | Purpose |
|-------|---------|
| `data` | Manages hierarchical form data stored in `#system/mailform`. |
| `flexview` | Provides drag-and-drop hierarchy visualization. |
| `ifc` | Generates interface controls (buttons, inputs, etc.). |
| `template` | Manages form templates for standalone rendering. |
| `system` | Stores global email settings. |
| `smtp` / `mime` | Handles email sending and MIME message construction. |

---

## Usage Scenarios

1. **Form Creation**:
   - Add a container (`add_receiver`), then append fields (`add_element`).
   - Configure field properties (e.g., validation, default values).

2. **Hierarchy Management**:
   - Drag-and-drop to reorder elements or nest containers.
   - Use "Copy" or "Cut" to duplicate/move elements.

3. **Email Configuration**:
   - Set SMTP credentials via `config` (operator-only).
   - Test email delivery with `test`.

4. **Template Integration**:
   - Containers automatically generate templates for standalone use.
   - Preview forms via the "Show" button.

---

## Error Handling
- **Data Persistence**: Rolls back changes if `data->save()` fails.
- **Template Cleanup**: Deletes orphaned templates on failed operations.
- **Permission Checks**: Blocks operator-only actions for non-operator users.


<!-- HASH:d96d550a342afc270fdeb8f26445823a -->
