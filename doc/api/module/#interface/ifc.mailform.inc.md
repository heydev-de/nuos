# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.mailform.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.mailform.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Mailform Interface

The `ifc.mailform.inc` file is the interface controller for the **Mailform** module in the PWNC Web Platform. It provides a backend interface for managing mailform structures, including form containers (receivers), individual form elements (fields), and email configuration. The interface allows operators to create, edit, delete, and organize form components in a hierarchical structure using a drag-and-drop flexview tree.

### Key Responsibilities:
- Manage form containers (mailform receivers) and their associated templates.
- Add, edit, and remove form elements (text, textarea, checkbox, radio, select, code, hidden, pagebreak).
- Configure email settings (SMTP, sender, reply-to, etc.).
- Test email delivery.
- Provide a visual hierarchy of form elements with drag-and-drop reordering.

---

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MAILFORM_PERMISSION_OPERATOR` | `"operator"` | Permission level required for advanced operations like configuration and testing. |

---

## Message Handling

The interface uses a `switch` statement on `CMS_IFC_MESSAGE` to determine the current action. Each case handles a specific operation related to form management.

### `select`

Sets the currently selected object for editing.

**Parameters:**
- `$ifc_param` (`string`): The index of the selected object.

**Usage Example:**
```php
// Triggered when a user clicks on a form element in the hierarchy
CMS_IFC_MESSAGE = "select";
$ifc_param = "5"; // Selects object with index 5
```

---

### `display`

Renders a live preview of the selected mailform.

**Parameters:**
- `$object` (`string`): The index of the form container to preview.

**Usage Example:**
```php
// Displays a preview of the mailform with index 3
CMS_IFC_MESSAGE = "display";
$object = "3";
```

---

### `add_receiver` / `add_element`

Displays a form to add a new receiver (container) or element to the form structure.

**Parameters:**
- `$ifc_param` (`string`): Target object index where the new item will be added.
- `$ifc_param1` (`string`): Default name for the new item.
- `$ifc_param2` (`string`): Default type for new elements (e.g., `"text"`).

**Usage Example:**
```php
// Shows a form to add a new text element under object 2
CMS_IFC_MESSAGE = "add_element";
$ifc_param = "2";
$ifc_param1 = "New Field";
$ifc_param2 = "text";
```

---

### `add_receiver_target` / `add_element_target`

Handles the actual insertion or appending of a new receiver or element after the user submits the add form.

**Parameters:**
- `$ifc_param` (`string`): Target object index.
- `$ifc_param1` (`string`): Name of the new item.
- `$ifc_param2` (`string`): Type of the new element (for elements only).

**Usage Example:**
```php
// Inserts a new container named "Contact Form" under object 0
CMS_IFC_MESSAGE = "add_receiver_insert";
$ifc_param = "0";
$ifc_param1 = "Contact Form";
```

---

### `add_receiver_insert` / `add_receiver_append`

Inserts or appends a new receiver (container) into the data structure and creates a corresponding template.

**Parameters:**
- `$ifc_param` (`string`): Target object index.
- `$ifc_param1` (`string`): Name of the new receiver.

**Usage Example:**
```php
// Appends a new receiver named "Newsletter Signup" to object 1
CMS_IFC_MESSAGE = "add_receiver_append";
$ifc_param = "1";
$ifc_param1 = "Newsletter Signup";
```

---

### `add_element_insert` / `add_element_append`

Inserts or appends a new form element into the data structure.

**Parameters:**
- `$ifc_param` (`string`): Target object index.
- `$ifc_param1` (`string`): Name of the new element.
- `$ifc_param2` (`string`): Type of the element (e.g., `"text"`, `"checkbox"`).

**Usage Example:**
```php
// Inserts a new checkbox element named "Agree to Terms" under object 4
CMS_IFC_MESSAGE = "add_element_insert";
$ifc_param = "4";
$ifc_param1 = "Agree to Terms";
$ifc_param2 = "checkbox";
```

---

### `save`

Saves the configuration of the currently selected object based on its type.

**Parameters:**
- `$object` (`string`): Index of the object being saved.
- `$ifc_param1` through `$ifc_param11`: Type-specific parameters (name, description, email, etc.).

**Usage Example:**
```php
// Saves a text field with name, description, width, length, default value, etc.
CMS_IFC_MESSAGE = "save";
$object = "7";
$ifc_param1 = "Email Address";
$ifc_param2 = "email_field";
$ifc_param3 = "Enter your email";
$ifc_param4 = "50"; // width
$ifc_param5 = "254"; // length
$ifc_param6 = ""; // default
$ifc_param7 = "/^.+@.+\..+$/"; // match pattern
$ifc_param8 = TRUE; // required
$ifc_param10 = TRUE; // confirm
$ifc_param11 = FALSE; // secret
```

---

### `copy_insert` / `copy_append` / `cut_insert` / `cut_append`

Handles copying or cutting of form elements and inserting/appending them to a new location.

**Parameters:**
- `$ifc_param` (`string`): Comma-separated string of source and target indices (e.g., `"source,target"`).

**Usage Example:**
```php
// Copies element 5 and appends it to container 3
CMS_IFC_MESSAGE = "copy_append";
$ifc_param = "5,3";
```

---

### `del`

Deletes the specified object and cleans up associated templates.

**Parameters:**
- `$ifc_param` (`string`): Index of the object to delete.

**Usage Example:**
```php
// Deletes object with index 9
CMS_IFC_MESSAGE = "del";
$ifc_param = "9";
```

---

### `config`

Displays the email configuration form for the mailform system.

**Parameters:**
- `$ifc_param1` through `$ifc_param6`: Email configuration values (address, reply-to, method, SMTP server, username, password).

**Usage Example:**
```php
// Displays the configuration form with current system values
CMS_IFC_MESSAGE = "config";
$ifc_param1 = "admin@example.com";
$ifc_param2 = "noreply@example.com";
$ifc_param3 = "smtp";
$ifc_param4 = "smtp.example.com";
$ifc_param5 = "user";
$ifc_param6 = "pass";
```

---

### `_config`

Saves the email configuration values to the system.

**Parameters:**
- `$ifc_param1` through `$ifc_param6`: Email configuration values to save.

**Usage Example:**
```php
// Saves new SMTP configuration
CMS_IFC_MESSAGE = "_config";
$ifc_param1 = "admin@example.com";
$ifc_param2 = "noreply@example.com";
$ifc_param3 = "smtp";
$ifc_param4 = "smtp.example.com";
$ifc_param5 = "user";
$ifc_param6 = "newpass";
```

---

### `test`

Sends a test email to verify the current email configuration.

**Usage Example:**
```php
// Sends a test email using current SMTP settings
CMS_IFC_MESSAGE = "test";
```

---

## Main Display

After processing the message, the interface renders the main mailform management UI:

1. **Hierarchy Tree**: A flexview tree showing all form containers and elements, supporting drag-and-drop reordering.
2. **Trash Bin**: A designated drop zone for deleting elements.
3. **Detail Panel**: When a non-pagebreak element is selected, displays a form to edit its properties based on its type.

### Supported Element Types:

| Type | Description |
|------|-------------|
| `container` | A mailform receiver with email settings and confirmation messages. |
| `checkbox` | A checkbox input with options and column layout. |
| `radio` | A radio button group with options and column layout. |
| `select` | A dropdown select with options. |
| `text` | A single-line text input with width, length, and validation. |
| `textarea` | A multi-line text input with width and height. |
| `code` | A custom HTML/code input field. |
| `hidden` | A hidden input field. |
| `pagebreak` | A visual separator between form sections. |

### JavaScript Integration

The interface includes a JavaScript function `mailform_flexview_event` that handles drag-and-drop events:
- **Drop on Trash Bin**: Deletes the element.
- **Drop on Another Element**: Moves (cut) or copies the element.

**Usage Example:**
```javascript
// User drags element 5 onto element 3
// Triggers: ifc_post("cut_append", "5,3");
```

---

## Data Structure

The mailform data is stored in `#system/mailform` using the `data` class. Each object has:
- `#type`: The element type (container, text, etc.).
- `name`: The display name.
- Type-specific properties (e.g., `email`, `url`, `description`, `option`, `width`, `height`, etc.).
- `template`: Reference to an associated template (for containers).

---

## Permissions

| Permission | Level | Description |
|------------|-------|-------------|
| `""` (default) | `CMS_L_ACCESS` | Basic access to view and manage forms. |
| `CMS_MAILFORM_PERMISSION_OPERATOR` | `CMS_L_OPERATOR` | Advanced access to configure email settings and test delivery. |

---

## Caching

The selected object is cached per user:
```php
cms_cache("mailform." . CMS_USER . ".object", $object, TRUE);
```

This ensures the user's last selection persists across requests.

---

## Libraries Used

- `flexview`: For rendering the hierarchical form structure.
- `template`: For creating and managing form templates.
- `smtp`: For sending test emails.
- `mime`: For generating MIME-compliant test messages.
- `system`: For storing and retrieving global configuration values.

---

## Example Workflow

1. **Create a Form Container**:
   - Navigate to the mailform interface.
   - Click "Add Form" to create a new container.
   - Enter a name and save.

2. **Add Form Elements**:
   - Select the container in the hierarchy.
   - Click "Add Element" and choose a type (e.g., text).
   - Configure the element's properties and save.

3. **Configure Email Settings**:
   - Click "Configuration" in the menu.
   - Set SMTP details and save.

4. **Test Email Delivery**:
   - Click "Test" to send a test email and verify configuration.

5. **Preview the Form**:
   - Select a container and click "Show" to preview the live form.


<!-- HASH:36cd3180f8ec507a713aa1de603cf750 -->
