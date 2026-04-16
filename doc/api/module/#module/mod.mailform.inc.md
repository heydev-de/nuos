# NUOS API Documentation

[← Index](../../README.md) | [`module/#module/mod.mailform.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: Mail Form Handler (`mod.mailform.inc`)

This module provides a dynamic, multi-page form system for the NUOS platform. It handles form display, validation, submission, and confirmation, supporting various field types (text, textarea, checkbox, radio, select, code, hidden), CAPTCHA verification, and multi-page navigation. Forms are defined in the `#system/mailform` data structure and can be submitted via email or HTTP POST.

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$mailform_form` | `string` | Current form identifier. |
| `$mailform_form_active` | `string` | Form identifier during active submission. |
| `$mailform_page` | `int` | Current page index (0-based). |
| `$mailform_message` | `string` | Submission command (`CMS_L_COMMAND_PREVIOUS` or `CMS_L_COMMAND_NEXT`). |
| `$mailform_captcha_code` | `string` | CAPTCHA verification code. |
| `$mailform_captcha_key` | `string` | CAPTCHA user input. |

---

### Core Logic Flow

1. **Form Selection**: If no form is specified, lists all available forms.
2. **Form Display**: Renders the current page of the specified form, including fields, validation errors, and navigation buttons.
3. **Submission Handling**: Validates input, checks CAPTCHA, and processes submission (email or HTTP POST).
4. **Confirmation**: Displays success/error messages and sends confirmation emails if configured.

---

### Key Functions and Logic

#### **Form Selection and Initialization**
```php
if (stre($mailform_form)) { ... }
```
- **Purpose**: Lists all valid forms if none is specified or displays the single available form.
- **Mechanism**:
  - Iterates through `#system/mailform` data to find containers with valid `email` or `url` targets.
  - Renders a list of forms if multiple exist; exits if none are found.
- **Usage**: Triggered when `$mailform_form` is empty.

---

#### **Submission State Check**
```php
$flag_active = streq($mailform_form, $mailform_form_active);
```
- **Purpose**: Determines if the form is being submitted.
- **Mechanism**:
  - Compares `$mailform_form` with `$mailform_form_active` to detect submission.
  - Adjusts the current page index (`$_mailform_page`) based on navigation commands (`CMS_L_COMMAND_PREVIOUS`/`NEXT`).
- **Usage**: Used to differentiate between form display and submission processing.

---

#### **Form Data Processing**
```php
$data->move("to", $mailform_form);
while (($key = $data->move("next")) !== NULL) { ... }
```
- **Purpose**: Processes form elements, validates input, and prepares data for submission.
- **Parameters**:
  - `$data`: `data` object pointing to `#system/mailform`.
- **Mechanism**:
  - Iterates through form elements, skipping page breaks and hidden fields.
  - Validates input against `match` regex patterns and `required` flags.
  - Collects confirmation email recipients if `confirm` is enabled.
  - Handles CAPTCHA verification on the final page.
- **Return**: Populates `$array` with processed field data (name, ID, value, mismatch status).
- **Usage**: Core logic for form validation and data preparation.

---

#### **Field Rendering**
```php
switch ($type) { ... }
```
- **Purpose**: Renders form fields based on their type.
- **Parameters**:
  - `$type`: Field type (`checkbox`, `radio`, `select`, `text`, `textarea`, `code`, `hidden`).
  - `$value`: Field data (name, ID, value, mismatch status).
- **Mechanism**:
  - **Checkbox/Radio**: Renders multi-column layouts with preselected options.
  - **Select**: Renders dropdowns with optional preselection.
  - **Text/Textarea**: Renders input fields with size/length constraints.
  - **Code**: Renders custom HTML/JS code with placeholder substitution.
  - **Hidden**: Renders hidden inputs with preset values.
- **Usage**: Called during form display to render each field dynamically.

---

#### **CAPTCHA Handling**
```php
if ((($captcha = $data->get($mailform_form, "captcha")) !== NULL) { ... }
```
- **Purpose**: Validates CAPTCHA on the final form page.
- **Mechanism**:
  - Loads the `captcha` library and verifies user input against the generated code.
  - Sets mismatch flags if verification fails.
- **Usage**: Triggered on the last page if CAPTCHA is enabled.

---

#### **Form Submission**
```php
if (($email = l($data->get($mailform_form, "email"))) || $confirm) { ... }
```
- **Purpose**: Sends form data via email or HTTP POST.
- **Mechanism**:
  - **Email**: Constructs an HTML email with form data and sends via `smtp_send()`.
  - **HTTP POST**: Sends data to configured URLs using `http_post()`.
  - Logs successful submissions via `log->access()`.
- **Usage**: Triggered after successful validation on the final page.

---

#### **Confirmation Display**
```php
if ($flag_success) { ... }
```
- **Purpose**: Displays success/error messages after submission.
- **Mechanism**:
  - Renders a confirmation message if submission succeeds.
  - Displays an error message if submission fails.
- **Usage**: Final step in the submission process.

---

### Helper Functions

| Function | Purpose | Usage |
|----------|---------|-------|
| `l($value)` | Localizes strings. | Used for form labels, descriptions, and messages. |
| `x($value)` | Escapes strings for HTML output. | Used for dynamic attribute values. |
| `parse_text($value)` | Parses text with NUOS formatting. | Used for field descriptions. |
| `image($src, ...)` | Renders an image tag. | Used for CAPTCHA display. |
| `permission($rules)` | Checks user permissions. | Used to restrict form access. |

---

### Data Structure (`#system/mailform`)

Form definitions are stored in a hierarchical data structure with the following keys:

| Key | Type | Description |
|-----|------|-------------|
| `name` | `string` | Form name (displayed in title). |
| `description` | `string` | Form description (displayed in overview). |
| `email` | `string` | Recipient email address. |
| `url` | `string` | Target URL for HTTP POST. |
| `confirmation` | `string` | Confirmation message (supports NUOS formatting). |
| `captcha` | `bool` | Enables CAPTCHA on the final page. |
| `submit` | `string` | Custom submit button label. |
| **Field Keys** | | |
| `#type` | `string` | Field type (`checkbox`, `radio`, `select`, `text`, `textarea`, `code`, `hidden`, `pagebreak`). |
| `name` | `string` | Field label. |
| `id` | `string` | Field identifier (auto-generated if empty). |
| `required` | `bool` | Marks field as required. |
| `match` | `string` | Regex pattern for input validation. |
| `confirm` | `bool` | Includes field value in confirmation emails. |
| `secret` | `bool` | Excludes field from confirmation emails. |
| `option` | `string` | Options for `checkbox`, `radio`, or `select` (newline-separated). |
| `default` | `string` | Default value for `text`, `textarea`, or `hidden`. |
| `width`/`height` | `int` | Dimensions for `text` or `textarea`. |
| `column` | `int` | Number of columns for `checkbox`/`radio`. |
| `code` | `string` | Custom HTML/JS for `code` fields. |
| `codeonly` | `bool` | Renders `code` fields without a fieldset. |

---

### Example Usage

#### **Form Definition**
```ini
[contact_form]
name = "Contact Us"
email = "contact@example.com"
confirmation = "Thank you for your message!"
captcha = true

[contact_form/name]
#type = text
name = "Name"
required = true
match = "/^.+$/"

[contact_form/email]
#type = text
name = "Email"
required = true
match = "/^.+@.+\..+$/"
confirm = true

[contact_form/message]
#type = textarea
name = "Message"
required = true
```

#### **Template Integration**
```php
// Display the form in a template
insert("mailform", ["mailform_form" => "contact_form"]);
```

#### **HTTP POST Target**
```php
// Process submitted data in a custom script
if (nstre($mailform_form = cms_param("mailform_form"))) {
    $name = cms_param("mailform_value1");
    $email = cms_param("mailform_value2");
    // ...
}
```


<!-- HASH:3fd93b43c70329f9abc392d595294693 -->
