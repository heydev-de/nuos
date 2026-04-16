# NUOS API Documentation

[← Index](../../README.md) | [`module/#module/mod.profile.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Profile Module (`mod.profile.inc`)

The **Profile Module** provides user registration, activation, and profile management functionality within the NUOS web platform. It handles:

- **User Registration**: Collects user data, validates inputs, and sends confirmation emails.
- **Account Activation**: Processes activation codes to enable user accounts.
- **Profile Editing**: Allows users to view and modify their profile information.
- **Form Validation**: Ensures data integrity through server-side validation (e.g., email format, password strength, required fields).
- **Captcha Integration**: Prevents automated submissions during registration.

---

### Global Variables

| Name                     | Type     | Description                                                                                     |
|--------------------------|----------|-------------------------------------------------------------------------------------------------|
| `$profile_message`       | `string` | Determines the current action (e.g., `"register"`, `"activate"`, `"edit"`).                    |
| `$profile_param`         | `array`  | User-submitted form data for profile fields.                                                   |
| `$profile_user`          | `string` | Username input during registration.                                                            |
| `$profile_email`         | `string` | Email input during registration.                                                               |
| `$profile_captcha_key`   | `string` | CAPTCHA key submitted by the user.                                                             |
| `$profile_captcha_code`  | `string` | CAPTCHA verification code.                                                                     |
| `$profile_code`          | `string` | Activation code for account verification.                                                      |

---

### Core Workflow

1. **Initialization**
   - Loads the `profile` library and checks if the module is enabled.
   - Instantiates the `profile` class and sets operator privileges.

2. **Activation**
   - Validates activation codes and enables user accounts.
   - Logs successful registrations and redirects users post-activation.

3. **Profile Retrieval/Creation**
   - Fetches existing user profiles or initializes a new `profile_data` object.

4. **Field Configuration**
   - Defines profile fields across 7 categories (e.g., credentials, contact info, payment details).
   - Dynamically loads custom fields from the system configuration.

5. **Form Processing**
   - Validates user inputs (e.g., email format, password strength, CAPTCHA).
   - Handles registration (with email confirmation) or profile updates.

6. **UI Rendering**
   - Generates HTML forms with dynamic fields based on configuration.
   - Displays validation errors and success messages.

---

### Key Methods and Logic

#### **Activation Handling**
```php
if (streq($profile_message, "activate")) { ... }
```
- **Purpose**: Processes account activation using a verification code.
- **Parameters**: Relies on `$profile_code` (URL parameter).
- **Mechanism**:
  1. Retrieves registration data using the code.
  2. Validates the code and creates a user profile if valid.
  3. Logs the event and redirects the user.
- **Usage**: Triggered when users click the activation link in their email.

---

#### **Profile Retrieval**
```php
elseif (($profile_data = $profile->get(CMS_USER, CMS_DB_PROFILE_USER)) !== FALSE) { ... }
```
- **Purpose**: Fetches an existing user profile for editing.
- **Parameters**:
  - `CMS_USER`: Current logged-in user identifier.
  - `CMS_DB_PROFILE_USER`: Database field for usernames.
- **Return**: `profile_data` object or `FALSE` if not found.
- **Usage**: Loads user data when accessing the profile edit page.

---

#### **Field Configuration**
```php
$field = [1 => [CMS_DB_PROFILE_CODE => CMS_L_MOD_PROFILE_008, ...], ...];
```
- **Purpose**: Defines profile fields and their display labels.
- **Structure**:
  - **Categories**: Grouped into 7 sections (e.g., credentials, contact info).
  - **Custom Fields**: Dynamically loaded from `#system/profile` data.
- **Usage**: Used to render form fields and validate inputs.

---

#### **Form Validation**
```php
while (in_array($profile_message, ["_register", "_edit"])) { ... }
```
- **Purpose**: Validates and processes form submissions.
- **Parameters**:
  - `$profile_param`: User-submitted data.
  - `$editable`: Fields marked as editable in the configuration.
- **Mechanism**:
  1. Checks for required fields and data mismatches (e.g., password confirmation).
  2. Validates email formats and CAPTCHA responses.
  3. Saves data if validation passes.
- **Usage**: Handles both registration and profile updates.

---

#### **Registration Workflow**
```php
if (count($mismatch) === 0) { ... }
```
- **Purpose**: Completes the registration process.
- **Mechanism**:
  1. Generates a unique activation code and temporary password.
  2. Stores registration data with an expiration timestamp.
  3. Sends a confirmation email with activation instructions.
- **Usage**: Triggered after successful form validation.

---

#### **UI Rendering**
```php
foreach ($visible AS $key => $value) { ... }
```
- **Purpose**: Dynamically generates form fields based on configuration.
- **Parameters**:
  - `$visible`: Fields marked as visible in the configuration.
  - `$editable`: Fields marked as editable.
  - `$required`: Fields marked as required.
- **Mechanism**:
  - Renders input types (text, password, textarea, select) based on field keys.
  - Displays validation errors inline.
- **Usage**: Called during form rendering for both registration and editing.

---

### Helper Functions

| Function               | Purpose                                                                                     |
|------------------------|---------------------------------------------------------------------------------------------|
| `verify_email($email)` | Validates email format.                                                                     |
| `unique_id()`          | Generates a unique identifier (e.g., for activation codes).                                 |
| `hash64($str)`         | Creates a base64-encoded hash (used for password encryption).                               |
| `cms_set_cookie($arr)` | Sets authentication cookies (e.g., `cms_user`, `cms_password`).                             |
| `cms_url($params)`     | Generates URLs with query parameters (e.g., for activation links).                          |
| `x($str)`              | Escapes strings for XML/HTML output.                                                        |
| `insert($section)`     | Includes template sections (e.g., `top`, `bottom`).                                         |

---

### Usage Scenarios

1. **User Registration**
   - **Trigger**: User submits the registration form.
   - **Flow**:
     1. Form data is validated.
     2. A confirmation email is sent with an activation link.
     3. User clicks the link to activate their account.

2. **Account Activation**
   - **Trigger**: User accesses the activation URL (e.g., `profile_message=activate&profile_code=XYZ`).
   - **Flow**:
     1. The system verifies the activation code.
     2. The user account is enabled.
     3. The user is redirected to the login page.

3. **Profile Editing**
   - **Trigger**: Logged-in user accesses the profile edit page.
   - **Flow**:
     1. Existing profile data is loaded.
     2. User updates fields and submits the form.
     3. Changes are saved if validation passes.

---

### Error Handling

| Error Type               | Message Key                     | Description                                                                 |
|--------------------------|---------------------------------|-----------------------------------------------------------------------------|
| Invalid Activation Code  | `CMS_L_MOD_PROFILE_047`         | Displayed when the activation code is not found.                            |
| Registration Error       | `CMS_MSG_ERROR`                 | Generic error during registration.                                          |
| Email Mismatch           | `CMS_L_MOD_PROFILE_044`         | Invalid email format.                                                       |
| Password Mismatch        | `CMS_L_MOD_PROFILE_032`         | Password and confirmation do not match.                                     |
| CAPTCHA Mismatch         | `CMS_L_MOD_PROFILE_046`         | Incorrect CAPTCHA response.                                                 |
| Required Field Missing   | `CMS_L_MOD_PROFILE_051`         | A required field is left empty.                                             |
| Username/Email Taken     | `CMS_L_MOD_PROFILE_045`/`052`   | Username or email already exists in the system.                             |

---

### Dependencies

| Dependency       | Purpose                                                                                     |
|------------------|---------------------------------------------------------------------------------------------|
| `profile`        | Core library for profile management (e.g., `profile->add()`, `profile->get()`).             |
| `data`           | Handles data storage/retrieval (e.g., registration data, custom fields).                    |
| `log`            | Logs user registration and access events.                                                   |
| `captcha`        | Provides CAPTCHA verification for registration.                                             |
| `smtp`           | Sends confirmation emails.                                                                  |


<!-- HASH:cf7df698ee50422ddce0c36aad7078af -->
