# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.mailform.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.mailform.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Mail Form Module (`mod.mailform.inc`)

The **Mail Form Module** provides a flexible, multi-page form system for collecting and processing user input. It supports various field types (text, textarea, checkbox, radio, select, hidden, and custom code), input validation, CAPTCHA verification, and multi-channel submission (email, HTTP POST). Forms are defined in a structured data file (`#system/mailform`) and rendered dynamically based on configuration.

---

### **Global Variables**

| Name | Type | Description |
|------|------|-------------|
| `$mailform_form` | `string` | Currently selected form identifier. |
| `$mailform_form_active` | `string` | Form identifier during active submission. |
| `$mailform_page` | `int` | Current page index (0-based) during form navigation. |
| `$mailform_message` | `string` | Command submitted (`CMS_L_COMMAND_PREVIOUS` or `CMS_L_COMMAND_NEXT`). |
| `$mailform_captcha_code` | `string` | CAPTCHA verification code submitted by the user. |
| `$mailform_captcha_key` | `string` | CAPTCHA session key submitted by the user. |

---

### **Core Workflow**

1. **Form Selection**
   - If no form is selected, lists all available forms (with valid email/URL targets).
   - If multiple forms exist, displays a selection list.
   - If exactly one form exists, proceeds to render it.

2. **Form Rendering**
   - Renders the form in pages (using `pagebreak` elements).
   - Validates input on submission (regex, required fields, CAPTCHA).
   - Displays errors and retains submitted values.

3. **Form Submission**
   - Sends data via email (SMTP) and/or HTTP POST to configured endpoints.
   - Sends confirmation emails to users if configured.
   - Logs successful submissions and displays a confirmation message.

---

### **Key Functions & Logic**

#### **Form Selection & Initialization**
```php
if (stre($mailform_form)) { ... }
```
- **Purpose**: Lists available forms or selects the only one.
- **Mechanism**:
  - Iterates through `#system/mailform` data.
  - Validates forms with `email` or `url` targets.
  - Renders a list if multiple forms exist; exits if none are valid.
- **Usage**: Automatically triggered when `$mailform_form` is empty.

---

#### **Form Processing**
```php
$flag_active = streq($mailform_form, $mailform_form_active);
if ($flag_active) { ... }
```
- **Purpose**: Determines if the form is being submitted and handles page navigation.
- **Parameters**:
  - `$mailform_form`: Current form identifier.
  - `$mailform_form_active`: Form identifier during submission.
- **Mechanism**:
  - Adjusts `$_mailform_page` based on `$mailform_message` (previous/next).
  - Resets to page 0 if the form is not active.
- **Usage**: Core logic for multi-page form navigation.

---

#### **Element Processing Loop**
```php
while (($key = $data->move("next")) !== NULL) { ... }
```
- **Purpose**: Processes form elements (fields, page breaks, CAPTCHA).
- **Mechanism**:
  - Skips non-container elements.
  - Handles `pagebreak` to split forms into pages.
  - Collects submitted values from `$GLOBALS` (sanitized via `preg_replace`).
  - Validates input (regex, required fields) and flags mismatches.
  - Prepares confirmation email recipients for fields marked with `confirm`.
- **Usage**: Iterates through all elements of the selected form.

---

#### **Field Rendering**
```php
switch ($type) { ... }
```
- **Purpose**: Renders form fields based on their type.
- **Supported Types**:
  - `checkbox`/`radio`: Multi-column layouts, preselection logic.
  - `select`: Dropdown with optional preselection.
  - `text`: Single-line input with width/length constraints.
  - `textarea`: Multi-line input with width/height constraints.
  - `code`: Custom HTML/JS/PHP code (via `replace_placeholder`).
  - `hidden`: Hidden input fields.
- **Mechanism**:
  - Uses `x()` for escaping output.
  - Applies CSS classes for styling and error states (`mailform-mismatch`).
  - Handles descriptions and required-field markers (`*`).
- **Example (Text Field)**:
  ```php
  echo("<input id=\"$__id\" name=\"$_id\" type=\"text\" value=\"" . x($value) . "\">");
  ```

---

#### **CAPTCHA Integration**
```php
if ($use_captcha && cms_load("captcha")) { ... }
```
- **Purpose**: Adds CAPTCHA verification to the last page.
- **Mechanism**:
  - Creates a CAPTCHA image using the `captcha` library.
  - Renders an input field for the user to enter the code.
  - Flags mismatches if verification fails.
- **Usage**: Enabled via the `captcha` setting in the form configuration.

---

#### **Form Submission**
```php
if (($email = l($data->get($mailform_form, "email"))) || $confirm) { ... }
```
- **Purpose**: Sends form data via email (SMTP) and/or HTTP POST.
- **Mechanism**:
  - **Email**: Generates an HTML email with a table of submitted data.
    - Uses `smtp_send()` for delivery.
    - Supports `reply_to` for confirmation emails.
  - **HTTP POST**: Sends data to configured URLs via `http_post()`.
  - Logs successful submissions via `log->access()`.
- **Example (Email Template)**:
  ```php
  $buffer = CMS_DOCTYPE_HTML . "<html><head><title>%s</title>...</html>";
  $content = "<tr><td>Field</td><td>Value</td></tr>";
  smtp_send("recipient@example.com", "Subject", sprintf($buffer, "Form", $content), TRUE);
  ```

---

#### **Confirmation & Error Handling**
```php
if ($flag_success) { ... } else { ... }
```
- **Purpose**: Displays success/error messages after submission.
- **Mechanism**:
  - **Success**: Renders a confirmation message (customizable via `confirmation` field).
  - **Failure**: Displays a generic error message (`CMS_L_MOD_MAILFORM_012`).
- **Usage**: Triggered after submission attempts.

---

### **Usage Example**

#### **1. Defining a Form in `#system/mailform`**
```ini
[contact_form]
name = "Contact Us"
description = "Send us a message"
email = "support@example.com"
captcha = true
submit = "Send Message"

[contact_form/name]
#type = text
name = "Your Name"
required = true
match = "/^[a-zA-Z ]+$/"

[contact_form/email]
#type = text
name = "Your Email"
required = true
match = "/^[^@]+@[^@]+\.[^@]+$/"

[contact_form/message]
#type = textarea
name = "Message"
required = true

[contact_form/pagebreak]
#type = pagebreak

[contact_form/confirm]
#type = checkbox
name = "Send me a copy"
option = "+Yes\nNo"
confirm = true
```

#### **2. Rendering the Form in a Template**
```php
// Load the module (typically via CMS routing)
cms_load("mailform");

// Or trigger via URL:
echo u(["mailform_form" => "contact_form"]);
```
- **Output**: A multi-page form with validation, CAPTCHA, and email submission.

#### **3. Handling Submission**
- On submission, the module:
  1. Validates input (e.g., email format, required fields).
  2. Sends an email to `support@example.com`.
  3. Sends a confirmation email to the user if "Send me a copy" is checked.
  4. Displays a success message or error.

---

### **Key Features**
| Feature | Description |
|---------|-------------|
| **Multi-Page Forms** | Split forms into pages using `pagebreak` elements. |
| **Input Validation** | Regex (`match`), required fields, and CAPTCHA. |
| **Multi-Channel Submission** | Email (SMTP) and HTTP POST. |
| **Confirmation Emails** | Send copies to users via `confirm` fields. |
| **Custom Code Fields** | Embed HTML/JS/PHP via `code` type. |
| **Localization** | Uses `CMS_L_*` constants for UI text. |
| **Security** | CSRF protection via `cms_url()`, output escaping (`x()`). |

---

### **Error Handling**
| Error | Cause | Resolution |
|-------|-------|------------|
| `CMS_MSG_UNAVAILABLE` | No valid forms or SMTP not loaded. | Ensure `#system/mailform` has valid forms and `smtp` module is loaded. |
| `CMS_L_MOD_MAILFORM_008` | Input validation failed. | Check field values against `match` regex or required fields. |
| `CMS_L_MOD_MAILFORM_012` | Submission failed (SMTP/HTTP error). | Verify SMTP/HTTP endpoint configurations. |


<!-- HASH:93ec257dc5ef5bb99c2319e5ec196a11 -->
