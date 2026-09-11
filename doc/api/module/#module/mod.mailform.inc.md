# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.mailform.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.mailform.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mod.mailform.inc

The `mod.mailform.inc` file is a PWNC Web Platform module that implements a dynamic, multi-page mail form system. It allows administrators to define forms via configuration data stored under `#system/mailform`, which can include various field types (text, textarea, checkbox, radio, select, code, hidden), page breaks, CAPTCHA support, email notifications, HTTP POST submissions, and confirmation messages.

This module handles both the rendering of the form and its processing upon submission. It supports:

- **Form selection**: If multiple forms are defined, it displays a list for the user to choose from.
- **Multi-page navigation**: Forms can be split into pages using `pagebreak` elements.
- **Input validation**: Each field can have a regex match rule and required flag.
- **CAPTCHA verification**: Optional CAPTCHA integration for spam protection.
- **Email delivery**: Sends form data via SMTP to specified recipients.
- **HTTP POST forwarding**: Optionally forwards form data to external URLs.
- **Confirmation display**: Shows success or error feedback after submission.

---

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `$mailform_form` | `NULL` | Identifier of the selected form to display/process. |
| `$mailform_form_active` | `NULL` | Identifier of the currently active (submitted) form. |
| `$mailform_page` | `NULL` | Current page number during multi-step form navigation. |
| `$mailform_message` | `NULL` | Navigation command (`next`, `previous`) or submit action. |
| `$mailform_captcha_code` | `NULL` | Hidden CAPTCHA code used for verification. |
| `$mailform_captcha_key` | `NULL` | User-entered CAPTCHA key for validation. |

---

### Form Selection Logic

When no specific form is selected (`$mailform_form` is empty), the module lists all available forms that have either an `email` or `url` target. If exactly one such form exists, it proceeds directly to render that form. Otherwise, it shows a list of available forms for the user to pick from.

#### Example Usage Scenario:
If two forms are configured — one sending emails and another posting to an API — users will see a list to choose which form they want to interact with.

---

### Multi-Page Navigation & Validation

Once a form is selected, the module processes submitted values based on the current page index (`$_mailform_page`). It validates each input against optional regex patterns and required flags. Mismatches cause the form to redisplay with highlighted errors.

#### Key Concepts:
- Page breaks divide long forms into steps.
- Input values are preserved across pages using hidden fields.
- Regex matching ensures format compliance (e.g., email addresses).
- Required fields enforce mandatory entry.

---

### CAPTCHA Integration

If enabled in the form configuration, a CAPTCHA challenge appears on the final page before submission. The module verifies the user’s input against a generated code.

#### Dependencies:
- Requires the `captcha` library loaded via `cms_load("captcha")`.

---

### Email Delivery

Upon successful validation, the module sends the collected form data via SMTP to the configured recipient(s). A confirmation email may also be sent to the submitter if their address was captured.

#### Features:
- HTML-formatted emails with embedded CSS styling.
- Reference codes included for tracking purposes.
- Reply-to headers set when confirmations are involved.

---

### HTTP POST Forwarding

In addition to email, form data can be forwarded to external endpoints via HTTP POST requests. This enables integration with third-party services like CRMs or analytics platforms.

#### Behavior:
- URLs are parsed and translated using `translate_url()`.
- Data is sent as POST parameters.
- Success depends on receiving a status code below 300.

---

### Confirmation Display

After processing, the module displays either a success message (with optional confirmation text) or an error notice indicating failure.

#### Output Sections:
- `<section class="mailform-success">`: Shown on successful submission.
- `<section class="mailform-error">`: Shown if any part of the process fails.

---

### Field Types Supported

Each form element is defined by its `#type` property. Below are the supported types and how they're rendered:

#### checkbox / radio

Renders selectable options laid out in columns. Supports preselection using `+` prefix in option strings.

##### Parameters:
| Name | Type | Description |
|------|------|-------------|
| `option` | string | Newline-separated list of options. |
| `column` | int | Number of columns for layout. |
| `required` | mixed | Marks field as required. |
| `match` | string | Regex pattern for validation. |
| `description` | string | Optional help text. |

##### Example:
```php
// Configuration snippet
$key = "gender";
$data->set($key, "#type", "radio");
$data->set($key, "name", "Gender");
$data->set($key, "option", "+Male\nFemale");
$data->set($key, "required", true);
```

#### select

Displays a dropdown menu populated from newline-separated options.

##### Parameters:
Same as checkbox/radio but rendered as `<select>`.

##### Example:
```php
$key = "country";
$data->set($key, "#type", "select");
$data->set($key, "name", "Country");
$data->set($key, "option", "USA\nCanada\nUK");
```

#### text

Renders a single-line text input with configurable width and length constraints.

##### Parameters:
| Name | Type | Description |
|------|------|-------------|
| `width` | int | Visual width in characters. |
| `length` | int | Maximum allowed character count. |
| `default` | string | Preset value when not yet submitted. |

##### Example:
```php
$key = "username";
$data->set($key, "#type", "text");
$data->set($key, "name", "Username");
$data->set($key, "width", 30);
$data->set($key, "length", 50);
```

#### textarea

Renders a multi-line text area with adjustable dimensions.

##### Parameters:
| Name | Type | Description |
|------|------|-------------|
| `width` | int | Column width in characters. |
| `height` | int | Row height in lines. |
| `default` | string | Initial content. |

##### Example:
```php
$key = "message";
$data->set($key, "#type", "textarea");
$data->set($key, "name", "Message");
$data->set($key, "width", 60);
$data->set($key, "height", 10);
```

#### code

Allows custom HTML/PHP code injection using placeholder replacement.

##### Parameters:
| Name | Type | Description |
|------|------|-------------|
| `code` | string | Raw HTML/code template. |
| `codeonly` | bool | Whether to wrap in `<div>` instead of `<fieldset>`. |
| `default` | string | Fallback value. |

##### Placeholders Available:
| Placeholder | Replaced With |
|------------|---------------|
| `{label}` | Field name |
| `{id}` | Unique DOM ID |
| `{name}` | Input name attribute |
| `{value}` | Submitted/default value |

##### Example:
```php
$key = "custom_html";
$data->set($key, "#type", "code");
$data->set($key, "name", "Custom Section");
$data->set($key, "code", '<div class="custom">{label}: {value}</div>');
```

#### hidden

Outputs a hidden input field whose value persists between pages.

##### Parameters:
| Name | Type | Description |
|------|------|-------------|
| `default` | string | Value used if none submitted. |

##### Example:
```php
$key = "token";
$data->set($key, "#type", "hidden");
$data->set($key, "default", generate_token());
```

---

### Internal Functions Used

| Function | Purpose |
|---------|----------|
| `cms_load()` | Loads required libraries (SMTP, CAPTCHA). |
| `l()` | Retrieves localized string from data store. |
| `x()` | Escapes string for safe HTML output. |
| `rx()` | Encodes string for use in URLs. |
| `cms_url()` | Generates URLs preserving state. |
| `parse_text()` | Parses BBCode-like markup in descriptions. |
| `replace_placeholder()` | Replaces tokens in custom code blocks. |
| `smtp_send()` | Sends email through SMTP backend. |
| `http_post()` | Performs HTTP POST request to external URL. |
| `translate_url()` | Converts logical identifiers to real URLs. |
| `insert()` | Inserts theme hooks at designated positions. |
| `permission()` | Enforces access control rules. |

---

### Typical Workflow Summary

1. **Load Dependencies**: Ensure SMTP and optionally CAPTCHA libraries are available.
2. **Select Form**: Display list if multiple forms exist; proceed with chosen one.
3. **Navigate Pages**: Track current page and validate inputs accordingly.
4. **Validate Inputs**: Check required fields and regex matches.
5. **Verify CAPTCHA**: On final page, ensure human interaction.
6. **Send Emails/Posts**: Deliver data via SMTP and/or HTTP POST.
7. **Show Result**: Display success or error message.

---

### Sample Configuration Snippet

```php
// Define a simple contact form
$form_key = "contact";
$data->set($form_key, "name", "Contact Us");
$data->set($form_key, "email", "admin@example.com");
$data->set($form_key, "captcha", true);

// Add fields
$data->set("name_field", "#type", "text");
$data->set("name_field", "name", "Your Name");
$data->set("name_field", "required", true);

$data->set("email_field", "#type", "text");
$data->set("email_field", "name", "Email Address");
$data->set("email_field", "match", "/^[^@]+@[^@]+\.[^@]+$/");
$data->set("email_field", "required", true);

$data->set("message_field", "#type", "textarea");
$data->set("message_field", "name", "Message");
$data->set("message_field", "required", true);

$data->set("submit_button", "#type", "pagebreak");
```

This would produce a two-page form: first collecting name/email/message, then showing a CAPTCHA before submission.


<!-- HASH:04f59855f8d3633f334cb203f244cc11 -->
