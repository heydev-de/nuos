# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.profile.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Profile Module (`mod.profile.inc`)

The **Profile Module** provides user registration, activation, and profile management functionality within the PWNC Web Platform. It handles:

- **User Registration**: Captures user details, validates inputs, and sends confirmation emails.
- **Account Activation**: Processes activation codes to enable user accounts.
- **Profile Editing**: Allows users to view and modify their profile data.
- **Field Configuration**: Supports customizable profile fields organized into categories (e.g., personal, contact, financial).

The module integrates with the platform’s **data**, **log**, **captcha**, and **smtp** subsystems for persistence, security, and communication.

---

## Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$profile_message` | `string` | Determines the current action: `"register"`, `"_register"`, `"activate"`, `"edit"`, `"_edit"`, `"_activate"`. |
| `$profile_param` | `array` | Form-submitted profile data (key-value pairs). |
| `$profile_user` | `string` | Username input during registration. |
| `$profile_email` | `string` | Email input during registration. |
| `$profile_captcha_key` | `string` | CAPTCHA key for verification. |
| `$profile_captcha_code` | `string` | CAPTCHA code for verification. |
| `$profile_code` | `string` | Activation code for account activation. |

---

## Core Logic Flow

1. **Initialization**
   - Loads the `profile` library and instantiates the `profile` class.
   - Checks if the module is enabled; exits if not.

2. **Action Routing**
   - **Activation**: Validates activation code, creates user account, logs access, and redirects.
   - **Profile Retrieval**: Fetches existing profile data for editing.
   - **Registration**: Displays form for new users.

3. **Field Configuration**
   - Defines **7 categories** of profile fields (e.g., login, personal, contact, financial).
   - Supports **20 custom fields** (configurable via `#system/profile` data store).
   - Fields are marked as **visible**, **editable**, or **required** based on configuration.

4. **Form Processing**
   - Validates inputs (e.g., email format, password strength, CAPTCHA).
   - Handles mismatches and displays errors.
   - Saves data on successful validation.

5. **Output Rendering**
   - Generates HTML form with dynamic fields based on configuration.
   - Displays success/error messages and CAPTCHA (if enabled).

---

## Key Functions & Methods

### `profile::add(profile_data $data)`
**Purpose**: Creates a new user profile in the database.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$data` | `profile_data` | Object containing user profile data. |

**Return Values**:
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**:
- Inserts profile data into the database.
- Handles duplicate checks and data integrity.

**Usage Example**:
```php
$profile_data = new profile_data();
$profile_data->user = "john_doe";
$profile_data->email = "john@example.com";
$profile_data->password = "secure123";
$profile->add($profile_data);
```

---

### `profile::get(string $identifier, string $field)`
**Purpose**: Retrieves a user profile by identifier (username or email).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$identifier` | `string` | Username or email. |
| `$field` | `string` | Field to match (`CMS_DB_PROFILE_USER` or `CMS_DB_PROFILE_EMAIL`). |

**Return Values**:
- `profile_data|FALSE`: Profile data object if found, `FALSE` otherwise.

**Inner Mechanisms**:
- Queries the database for the specified field.
- Returns a `profile_data` object with all fields populated.

**Usage Example**:
```php
$user_profile = $profile->get("john_doe", CMS_DB_PROFILE_USER);
if ($user_profile !== FALSE) {
    echo "User found: " . $user_profile->email;
}
```

---

### `profile::set(profile_data $data)`
**Purpose**: Updates an existing user profile.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$data` | `profile_data` | Updated profile data. |

**Return Values**:
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**:
- Validates the existence of the profile.
- Updates all fields in the database.

**Usage Example**:
```php
$user_profile->phone_1 = "+1234567890";
$profile->set($user_profile);
```

---

## Data Structures

### `profile_data` Class
Represents a user profile with the following properties (partial list):

| Property | Type | Description |
|----------|------|-------------|
| `user` | `string` | Username. |
| `email` | `string` | Email address. |
| `password` | `string` | Hashed password. |
| `prename` | `string` | First name. |
| `surname` | `string` | Last name. |
| `company` | `string` | Company name. |
| `phone_1` | `string` | Primary phone number. |
| `custom_field1` | `string` | Custom field (configurable). |

---

## Field Configuration

Fields are organized into **7 categories** with configurable visibility, editability, and requirement settings:

| Category ID | Title (Default) | Description |
|-------------|-----------------|-------------|
| 1 | Login Data | Username, password, activation code. |
| 2 | Personal Data | Name, address, country. |
| 3 | Contact Data | Phone, email, URL. |
| 4 | Bank Data | Account number, bank code. |
| 5 | Credit Card Data | Card number, validity. |
| 6 | Notes | Free-text comments. |
| 7 | Custom Fields | User-defined fields (1–20). |

**Configuration Example** (via `#system/profile` data store):
```ini
[user]
required = true
visible = true
editable = false

[custom_field1]
title = "Preferred Language"
required = false
visible = true
editable = true
```

---

## Validation Rules

| Field | Validation Rule |
|-------|-----------------|
| `user` | Must be unique; max 40 chars. |
| `email` | Must be valid format and unique. |
| `password` | Min 4 chars; must match confirmation. |
| `credit_card_validity` | Must be in `MM/YYYY` format. |
| `captcha` | Must match generated code (if enabled). |

---

## Usage Scenarios

### 1. User Registration
**Context**: New user signs up via the registration form.
**Steps**:
1. User submits `profile_user`, `profile_email`, and other required fields.
2. Module validates inputs and sends a confirmation email with an activation link.
3. User clicks the link (`profile_message=activate&profile_code=XYZ`), and the account is activated.

**Example URL**:
```plaintext
https://example.com/?profile_message=activate&profile_code=abc123def456
```

---

### 2. Profile Editing
**Context**: Logged-in user updates their profile.
**Steps**:
1. User navigates to the profile page (`profile_message=edit`).
2. Module loads existing data and renders an editable form.
3. User submits changes (`profile_message=_edit`), and the module validates and saves the data.

**Example Form Submission**:
```html
<form action="?profile_message=_edit" method="post">
    <input name="profile_param[prename]" value="John">
    <input name="profile_param[surname]" value="Doe">
    <button type="submit">Save</button>
</form>
```

---

### 3. Custom Field Integration
**Context**: Extend profile with a custom field (e.g., "Preferred Language").
**Steps**:
1. Configure the field in `#system/profile`:
   ```ini
   [custom_field1]
   title = "Preferred Language"
   visible = true
   editable = true
   ```
2. The field appears in the **Custom Fields** category during registration/editing.

---

## Error Handling

| Error | Message (CMS_L_MOD_PROFILE_*) | Cause |
|-------|-------------------------------|-------|
| 032 | "Passwords do not match." | Password and confirmation mismatch. |
| 033 | "Password is too short." | Password < 4 chars. |
| 044 | "Invalid email address." | Email format invalid. |
| 045 | "Username already exists." | Duplicate username. |
| 046 | "CAPTCHA verification failed." | Incorrect CAPTCHA code. |
| 047 | "Invalid activation code." | Activation code not found. |
| 051 | "This field is required." | Missing required field. |
| 052 | "Email already exists." | Duplicate email. |

---

## Security Considerations

1. **Password Storage**: Passwords are hashed using `hash64(cms_salt("password") . hash64($password))`.
2. **CSRF Protection**: Form submissions include CSRF tokens via `cms_url()`.
3. **CAPTCHA**: Enabled during registration to prevent automated submissions.
4. **Input Sanitization**: All outputs are escaped using `x()` (XML) or `q()` (JS/JSON).

---

## Dependencies

| Dependency | Purpose |
|------------|---------|
| `data` | Stores profile configurations and registration data. |
| `log` | Logs user registration and access. |
| `captcha` | Provides CAPTCHA verification. |
| `smtp` | Sends confirmation emails. |
| `permission` | Checks user permissions. |


<!-- HASH:67e0bb3258c1d8a0efe42f421e2110f6 -->
