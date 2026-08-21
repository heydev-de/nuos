# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.mailform.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.mailform.inc)

- **Version:** `26.8.11.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Mailform Interface Module (`ifc.mailform.inc`)

This file implements the **Mailform Interface** for the PWNC Web Platform, providing a user interface for creating, managing, and configuring mail forms. It allows operators to define form structures (containers, fields, and elements), set validation rules, and configure email delivery settings.

The interface supports hierarchical form structures with drag-and-drop functionality, template integration, and multi-language field naming. It integrates with the platform's `data`, `flexview`, `template`, and `ifc` modules to provide a seamless editing experience.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_MAILFORM_PERMISSION_OPERATOR` | `"operator"` | Permission level required for advanced operations (e.g., configuration, testing). |

---

## Interface Workflow

The interface operates via **message-driven cases**, triggered by `CMS_IFC_MESSAGE`. Each case handles a specific action (e.g., `select`, `add_receiver`, `save`, `del`).

### Key Concepts
- **Form Structure**: Stored in `#system/mailform` as a hierarchical `data` object.
- **Containers**: Top-level forms (e.g., contact forms) that group elements.
- **Elements**: Individual fields (e.g., text, checkbox, select) with validation rules.
- **Templates**: Optional `template` objects for rendering forms in the frontend.

---

## Message Handling

### `case "select"`
**Purpose**: Selects a form or element for editing.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Index of the object to select. |

**Mechanism**:
- Updates the `$object` variable to the selected index.
- Caches the selection for persistence.

**Usage**:
```php
// Triggered when a user clicks on a form/element in the hierarchy.
ifc_post("select", "123"); // Selects object with index "123".
```

---

### `case "display"`
**Purpose**: Renders a preview of the selected form in a new window.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Index of the form to preview. |

**Mechanism**:
- Loads the `template` module.
- Generates a preview using `template_preview()` with embedded PHP code to render the form.

**Usage**:
```php
// Displays a preview of the form with index "123".
ifc_post("display", "123");
```

---

### `case "add_receiver"` / `case "add_element"`
**Purpose**: Initiates the addition of a new form (receiver) or element.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Target parent index (where the new object will be added). |
| `$ifc_param1` | `string` | Default name for the new object (localized). |
| `$ifc_param2` | `string` | Subtype for elements (e.g., `"text"`, `"checkbox"`). |

**Mechanism**:
- Sets default names (e.g., `"New Form"` or `"New Element"`) if none is provided.
- Redirects to `add_receiver_target` or `add_element_target` for further configuration.

**Usage**:
```php
// Adds a new text element under parent index "123".
ifc_post("add_element", "123", "New Text Field", "text");
```

---

### `case "add_receiver_target"` / `case "add_element_target"`
**Purpose**: Displays a target selection interface for adding a new object.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Target parent index. |
| `$ifc_param1` | `string` | Name of the new object. |
| `$ifc_param2` | `string` | Subtype (for elements). |

**Mechanism**:
- Uses `flexview` to display a hierarchical list of valid targets.
- Supports **insertion** (before/after) or **appending** (as a child).

**Usage**:
```php
// Shows a list of targets to insert/append a new form.
ifc_post("add_receiver_target", "123", "Contact Form");
```

---

### `case "add_receiver_insert"` / `case "add_receiver_append"`
**Purpose**: Inserts or appends a new form (container) to the hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Target index. |
| `$ifc_param1` | `string` | Name of the new form. |

**Mechanism**:
- Creates a new container in the `#system/mailform` data structure.
- Optionally generates a `template` for the form if the `template` module is loaded.

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Usage**:
```php
// Appends a new form named "Feedback" under index "123".
ifc_post("add_receiver_append", "123", "Feedback");
```

---

### `case "add_element_insert"` / `case "add_element_append"`
**Purpose**: Inserts or appends a new element (field) to the hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Target index. |
| `$ifc_param1` | `string` | Name of the element. |
| `$ifc_param2` | `string` | Subtype (e.g., `"text"`, `"checkbox"`). |

**Mechanism**:
- Creates a new element with default values (e.g., placeholder code for `"code"` subtype).
- Saves the updated data structure.

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Usage**:
```php
// Inserts a new checkbox element named "Subscribe" before index "123".
ifc_post("add_element_insert", "123", "Subscribe", "checkbox");
```

---

### `case "save"`
**Purpose**: Saves the configuration of a selected form or element.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Index of the object to save. |
| `$ifc_param1` | `string` | Name of the object. |
| `$ifc_param2`–`$ifc_param11` | `mixed` | Field-specific values (e.g., description, validation rules). |

**Mechanism**:
- Updates the object's properties in the `#system/mailform` data structure.
- Handles type-specific fields (e.g., `width` for `text`, `option` for `select`).
- Updates associated templates if applicable.

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Usage**:
```php
// Saves a text field with validation rules.
ifc_post("save", "123", "Email", "email_field", "Enter your email", 50, 256, "", "/^.+@.+\..+$/", TRUE);
```

---

### `case "copy_insert"` / `case "copy_append"` / `case "cut_insert"` / `case "cut_append"`
**Purpose**: Copies or moves an object within the hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Comma-separated source and target indices (e.g., `"456,123"`). |

**Mechanism**:
- **Copy**: Duplicates the object and its children.
- **Cut**: Moves the object and its children.
- Validates container constraints (e.g., containers cannot be inserted into non-containers).
- Updates templates for copied containers.

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Usage**:
```php
// Moves object "456" to be a child of "123".
ifc_post("cut_append", "456,123");
```

---

### `case "del"`
**Purpose**: Deletes an object and its children.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Index of the object to delete. |

**Mechanism**:
- Removes the object from the `#system/mailform` data structure.
- Deletes associated templates.
- Selects the nearest existing parent object.

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Usage**:
```php
// Deletes object "123" and its children.
ifc_post("del", "123");
```

---

### `case "config"`
**Purpose**: Displays the email configuration interface.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| (None) | - | Uses system-wide email settings. |

**Mechanism**:
- Loads current email settings (e.g., SMTP, sender address) from the `system` module.
- Renders an `ifc` form for editing.

**Usage**:
```php
// Opens the email configuration interface.
ifc_post("config");
```

---

### `case "_config"`
**Purpose**: Saves email configuration settings.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Sender email address. |
| `$ifc_param2` | `string` | Reply-to address. |
| `$ifc_param3` | `string` | Email method (`"mail"` or custom SMTP). |
| `$ifc_param4` | `string` | SMTP server address. |
| `$ifc_param5` | `string` | SMTP username. |
| `$ifc_param6` | `string` | SMTP password. |

**Mechanism**:
- Updates the `system` module with new email settings.
- Validates operator permissions.

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.

**Usage**:
```php
// Saves SMTP configuration.
ifc_post("_config", "noreply@example.com", "reply@example.com", "smtp", "smtp.example.com", "user", "pass");
```

---

### `case "test"`
**Purpose**: Sends a test email to verify configuration.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| (None) | - | Uses system-wide email settings. |

**Mechanism**:
- Loads the `smtp` and `mime` modules.
- Sends a test email to the configured reply-to address.
- Displays success/error messages.

**Return**:
- `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` with details on failure.

**Usage**:
```php
// Sends a test email.
ifc_post("test");
```

---

## Main Display

### Hierarchy Rendering
- Uses `flexview` to display the form hierarchy with drag-and-drop support.
- Icons indicate object types (e.g., form, checkbox, text field).
- **Drag-and-Drop Events**:
  - `dropon`: Moves (`cut`) an object.
  - `dropon_alt`: Copies an object.

### Data Panel
- Displays editable fields for the selected object.
- **Field Types**:
  - **Container**: Form settings (e.g., email, URL, captcha).
  - **Checkbox/Radio/Select**: Option lists, validation flags.
  - **Text/Textarea**: Width, length, default values, regex validation.
  - **Code**: Custom HTML/JS code for advanced fields.
  - **Hidden**: Default values, confirmation flags.

**Example**:
```php
// Renders a text field with validation for a phone number.
$ifc->set(CMS_L_IFC_MAILFORM_022, "text 50 b", "/^[ \/\(\)0-9\+\-]*$/");
```

---

## Helper Functions

### `qr()`
**Purpose**: Escapes strings for HTML/JS output (used by `flexview`).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$s` | `string` | Input string. |

**Return**: `string` – Escaped string.

**Usage**:
```php
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
```

---

## Usage Example

### Creating a Contact Form
1. **Add a Form**:
   ```php
   ifc_post("add_receiver", "0", "Contact Us");
   ```
2. **Add Fields**:
   ```php
   ifc_post("add_element_append", "123", "Name", "text");
   ifc_post("add_element_append", "123", "Email", "text");
   ifc_post("add_element_append", "123", "Message", "textarea");
   ```
3. **Configure Fields**:
   ```php
   // Save the "Email" field with validation.
   ifc_post("save", "456", "Email", "email_field", "Your Email", 50, 256, "", "/^.+@.+\..+$/", TRUE);
   ```
4. **Set Email Configuration**:
   ```php
   ifc_post("_config", "noreply@example.com", "contact@example.com", "mail");
   ```
5. **Test the Form**:
   ```php
   ifc_post("test");
   ```

---

## Dependencies
- **`flexview`**: Hierarchical data display and drag-and-drop.
- **`data`**: Storage for form structures.
- **`template`**: Optional form rendering templates.
- **`ifc`**: Interface construction and form handling.
- **`smtp`/`mime`**: Email sending (for testing).


<!-- HASH:36cd3180f8ec507a713aa1de603cf750 -->
