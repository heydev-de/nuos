# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.profile.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mod.profile.inc

The `mod.profile.inc` module is the central controller for user profile management in the PWNC Web Platform. It handles three primary operations:

1. **Profile Activation** – Converts a pending registration into an active user profile using a unique activation code.
2. **Profile Retrieval/Editing** – Loads an existing user's profile for viewing or editing.
3. **Profile Registration** – Collects new user registration data, validates it, and stores it as a pending registration awaiting email confirmation.

The module dynamically builds form fields from system configuration, supports custom fields, validates input (email, password, CAPTCHA), and renders a responsive HTML form with inline error messaging.

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$profile_message` | `string` | Current operation mode: `activate`, `register`, `_register`, `edit`, `_edit`, `_activate` |
| `$profile_param` | `array` | Submitted form parameters keyed by field name |
| `$profile_user` | `string` | Submitted username |
| `$profile_email` | `string` | Submitted email address |
| `$profile_captcha_key` | `string` | User-entered CAPTCHA text |
| `$profile_captcha_code` | `string` | Internal CAPTCHA verification code |
| `$profile_code` | `string` | Activation code for profile activation |

---

### Library Loading and Initialization

The module begins by loading the `profile` library via `cms_load("profile")`. If the library is unavailable, it outputs `CMS_MSG_UNAVAILABLE` and returns early. It then instantiates a `profile` object and checks if the profile system is enabled. The `$profile->operator` flag is set to `TRUE`, granting operator-level permissions for certain operations.

```php
// Typical scenario: User navigates to the profile page
// The module auto-loads the profile library and checks availability
if (! cms_load("profile")) {
    echo(CMS_MSG_UNAVAILABLE);
    return;
}
$profile = new profile();
if (! $profile->enabled) {
    echo(CMS_MSG_UNAVAILABLE);
    return;
}
```

---

### Activation Handler

When `$profile_message` equals `"activate"`, the module processes a profile activation request. It retrieves registration data from the `#system/profile.registration` data store using the provided `$profile_code`. If the code is invalid or not found, an error is displayed.

If valid, it creates a `profile_data` object, populates it from the stored registration data, sets `superuser` to `"profile"` and `enabled` to `TRUE`, then calls `$profile->add()` to create the active profile. On success, it logs the registration, removes the temporary registration data, encrypts the password, sets authentication cookies, and redirects to the profile page with a success message.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$profile_code` | `string` | Unique activation code from the registration email |

```php
// Example: User clicks activation link from email
// URL: /profile?profile_message=activate&profile_code=abc123
// The module validates the code, creates the profile, and redirects
$data = new data("#system/profile.registration");
if (stre($profile_code) || ($data->get($profile_code) === NULL)) {
    echo("<div class=\"response-error\">" . CMS_L_MOD_PROFILE_047 . "</div>");
    return;
}
// ... profile creation logic ...
```

---

### Profile Retrieval and Creation

If activation is not requested, the module attempts to retrieve the current user's profile via `$profile->get(CMS_USER, CMS_DB_PROFILE_USER)`. If found, it defaults `$profile_message` to `"edit"` if empty. If no profile exists, it creates a new `profile_data` object and defaults `$profile_message` to `"register"`.

```php
// Example: Logged-in user visits /profile
// Their existing profile is loaded for editing
$profile_data = $profile->get(CMS_USER, CMS_DB_PROFILE_USER);
if ($profile_data !== FALSE) {
    if (stre($profile_message)) $profile_message = "edit";
} else {
    $profile_data = new profile_data();
    if (stre($profile_message)) $profile_message = "register";
}
```

---

### Field Configuration Builder

The module reads field configuration from the `#system/profile` data store and builds a structured field array organized into 7 categories:

| Key | Title Constant | Description |
|-----|----------------|-------------|
| 1 | `CMS_L_MOD_PROFILE_002` | System fields (code, user, password) |
| 2 | `CMS_L_MOD_PROFILE_003` | Company fields |
| 3 | `CMS_L_MOD_PROFILE_004` | Contact fields |
| 4 | `CMS_L_MOD_PROFILE_005` | Bank account fields |
| 5 | `CMS_L_MOD_PROFILE_006` | Credit card fields |
| 6 | `CMS_L_MOD_PROFILE_007` | Comment field |
| 7 | `CMS_L_MOD_PROFILE_011` | Custom fields (up to 20) |

Custom fields (1–20) are dynamically loaded from configuration, with fallback titles if not defined.

```php
// Example: System administrator configures custom fields
// in #system/profile data store
// The module automatically includes them in the form
for ($i = 1; $i <= 20; $i++) {
    if (! $_title = $data->get(CMS_DB_PROFILE_CUSTOM_FIELD . $i, "title"))
        $_title = CMS_L_MOD_PROFILE_010 . " $i";
    $field[7][CMS_DB_PROFILE_CUSTOM_FIELD . $i] = $_title;
}
```

---

### Visibility and Editability Configuration

Based on whether the current mode is registration or editing, the module collects three arrays:

- **`$visible`** – Fields visible in the form
- **`$editable`** – Fields that can be edited (input elements generated)
- **`$required`** – Fields that must be filled

During registration, system fields (category 1) and email (from category 3) are removed. Required fields are always visible and editable. During editing, visibility, editability, and required status are independently checked from configuration.

```php
// Example: Registration form shows only configured visible fields
// Required fields are always shown and editable
foreach ($field AS $key => $value) {
    foreach ($value AS $_key => $_) {
        if ($_flag = $data->get($_key, "required"))
            $required[$key][$_key] = TRUE;
        if ($_flag || $data->get($_key, "visible")) {
            $visible[$key][$_key] = TRUE;
            if ($_flag || $data->get($_key, "editable"))
                $editable[$key][$_key] = TRUE;
        }
    }
}
```

---

### Form Data Processing

When `$profile_message` is `"_register"` or `"_edit"`, the module processes submitted form data. It iterates over editable fields, applies field-specific validation, and collects mismatches.

#### Field-Specific Validation

| Field | Validation Logic |
|-------|-----------------|
| `CMS_DB_PROFILE_EMAIL` | Validates email format via `verify_email()` |
| `CMS_DB_PROFILE_PASSWORD` | Checks password/confirmation match, minimum 4 characters |
| `CMS_DB_PROFILE_CREDIT_CARD_VALIDITY` | Joins month/year with `/` separator |
| Default | Direct value assignment |

Required field validation checks if the value is empty after trimming.

```php
// Example: User submits registration form
// Password fields are validated for match and length
case CMS_DB_PROFILE_PASSWORD:
    if (stre($profile_param[$_key][0])) continue 2; // keep existing
    if (nstreq($profile_param[$_key][0], $profile_param[$_key][1])) {
        $mismatch[$key][$_key] = CMS_L_MOD_PROFILE_032; // no match
        continue 2;
    }
    if (strlen($profile_param[$_key][0]) < 4) {
        $mismatch[$key][$_key] = CMS_L_MOD_PROFILE_033; // too short
        continue 2;
    }
    $_value = $profile_param[$_key][0];
    break;
```

---

### Registration Validation

After field-level validation, the module performs registration-specific checks:

1. **Username validation** (if `$user_equals_email` is `FALSE`):
   - Strips spaces, truncates to 40 characters, trims
   - Checks for emptiness
   - Verifies uniqueness against existing profiles, permissions, and pending registrations

2. **Email validation**:
   - Checks for emptiness
   - Validates format
   - If `$user_equals_email`, uses email as username and checks uniqueness

3. **CAPTCHA verification** (if captcha library is loaded):
   - Verifies the user-entered code against the internal code

```php
// Example: New user registration with username and email
// Both are validated for uniqueness and format
if (! $user_equals_email) {
    $profile_user = stripspaces($profile_user);
    $profile_user = utf8_substr($profile_user, 0, 40);
    $profile_user = utf8_trim($profile_user);
    if (stre($profile_user)) {
        $mismatch[0]["user"] = CMS_L_MOD_PROFILE_051; // empty
    } else {
        // Check uniqueness across profile, permission, and registration stores
        $permission = new data("#system/permission");
        $registration = new data("#system/profile.registration");
        if (($profile->get($profile_user, CMS_DB_PROFILE_USER) !== FALSE) ||
            ($permission->get("user.$profile_user") !== NULL) ||
            ($registration->seek(["user" => $profile_user]) !== FALSE))
            $mismatch[0]["user"] = CMS_L_MOD_PROFILE_045; // taken
    }
}
```

---

### Registration Data Storage and Email

If all validations pass, the module:

1. Generates a unique activation code and temporary password
2. Stores registration data (including expiration timestamp) in `#system/profile.registration`
3. Sends a confirmation email with activation link via SMTP
4. Displays success or error message

| Variable | Type | Description |
|----------|------|-------------|
| `$code` | `string` | Unique registration code (via `unique_id()`) |
| `$password` | `string` | Temporary password for initial login |

```php
// Example: Successful registration triggers email
// User receives email with activation link
$code = unique_id();
$password = unique_id();
$registration->set(
    ["user" => $profile_user,
     "email" => $profile_email,
     "password" => $password,
     "#expire" => time() + 86400] +
    get_object_vars($profile_data),
    $code);
if ($registration->save() &&
    cms_load("smtp") &&
    smtp_send($profile_email,
        CMS_L_MOD_PROFILE_041,
        sprintf(CMS_L_MOD_PROFILE_042,
            CMS_ROOT_URL, $profile_user, $password,
            cms_url(["profile_message" => "activate", "profile_code" => $code])))) {
    echo("<div class=\"response-success\">" . CMS_L_MOD_PROFILE_043 . "</div>");
    return;
}
```

---

### Edit Profile Saving

When in edit mode (`_edit`) with no mismatches, the module calls `$profile->set($profile_data)` to persist changes and displays a success message.

```php
// Example: User edits their profile and clicks Save
// Changes are persisted to the database
if ((count($mismatch) === 0) && streq($profile_message, "_edit")) {
    if ($profile->set($profile_data))
        echo("<div class=\"response-success\">" . CMS_L_MOD_PROFILE_034 . "</div>");
    break;
}
```

---

### Form Rendering

The module renders an HTML form with the following structure:

1. **Activation success message** – Shown if `$profile_message` is `"_activate"`
2. **Registration header** – Shown for registration mode
3. **Mismatch summary** – Displays error summary if validation failed
4. **Username/email/captcha inputs** – Conditionally rendered based on `$user_equals_email` and captcha availability
5. **Dynamic field rendering** – Iterates over `$visible` fields, rendering appropriate input types

#### Input Type Mapping

| Field | Input Type | Special Handling |
|-------|-----------|-----------------|
| `CMS_DB_PROFILE_PASSWORD` | Two password inputs | Confirmation field, masked display |
| `CMS_DB_PROFILE_CREDIT_CARD_VALIDITY` | Month select + Year input | Split into month/year components |
| `CMS_DB_PROFILE_COMMENT` | Textarea | Multi-line text input |
| All others | Text input | Standard text field |

```php
// Example: Password field renders two inputs for confirmation
case CMS_DB_PROFILE_PASSWORD:
    if (stre($name)) {
        echo("****"); // masked display for non-editable
        break;
    }
    echo("<div class=\"p\">" .
        "<label for=\"$id\">" . x($field[$key][$_key]) . "</label><br>" .
        "<input id=\"$id\" name=\"{$name}[0]\" type=\"password\"><br>" .
        "</div>" .
        "<div class=\"p\">" .
        "<label for=\"$id-confirmation\">* " . CMS_L_MOD_PROFILE_035 . "</label><br>" .
        "<input id=\"$id-confirmation\" name=\"{$name}[1]\" type=\"password\">" .
        "</div>");
    break;
```

---

### Submit Buttons

The form includes context-appropriate submit buttons:

| Mode | Button Label | Value |
|------|-------------|-------|
| `register` / `_register` | `CMS_L_MOD_PROFILE_040` | `_register` |
| `edit` / `_edit` / `_activate` | `CMS_L_COMMAND_SAVE` | `_edit` |

```php
// Example: Registration form shows "Register" button
// Edit form shows "Save" button
switch ($profile_message) {
    case "register":
    case "_register":
        echo("<div class=\"p\">" .
            "<button name=\"profile_message\" type=\"submit\" value=\"_register\">" .
            CMS_L_MOD_PROFILE_040 . "</button>" .
            "</div>");
        break;
    default:
        if (count($editable) > 0)
            echo("<div class=\"p\">" .
                "<button name=\"profile_message\" type=\"submit\" value=\"_edit\">" .
                CMS_L_COMMAND_SAVE . "</button>" .
                "</div>");
        break;
}
```

---

### Complete Usage Flow

```php
// 1. New user registration flow:
//    User visits /profile?profile_message=register
//    Fills form, submits with profile_message=_register
//    Module validates, stores registration, sends email
//    User clicks activation link: /profile?profile_message=activate&profile_code=XYZ
//    Module creates active profile, sets cookies, redirects

// 2. Existing user edit flow:
//    Logged-in user visits /profile
//    Module loads their profile, defaults to edit mode
//    User modifies fields, submits with profile_message=_edit
//    Module validates and saves changes

// 3. Activation flow:
//    User clicks link from registration email
//    Module validates code, creates profile from stored data
//    Logs access, removes registration data, sets auth cookies
//    Redirects to profile page with success message
```


<!-- HASH:67e0bb3258c1d8a0efe42f421e2110f6 -->
