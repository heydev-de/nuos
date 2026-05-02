# NUOS API Documentation

[← Index](../../README.md) | [`module/#module/mod.identification.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: Identification (`mod.identification.inc`)

Core authentication module for the NUOS platform handling user login, password recovery, and session management. Provides UI components and backend logic for secure user identification with support for legacy password hashing, CAPTCHA verification, and brute-force protection.

---

### Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$location` | `NULL` | Target URL after successful login. |
| `$identification_message` | `NULL` | Controls module state: `"__recover"` (process recovery), `"_recover"` (initiate recovery), `"recover"` (display recovery form), or `NULL` (display login form). |
| `$identification_user` | `NULL` | User identifier (username or email) for login/recovery. |
| `$identification_email` | `NULL` | User email for recovery. |
| `$identification_captcha_key` | `NULL` | CAPTCHA input value from user. |
| `$identification_captcha_code` | `NULL` | Expected CAPTCHA verification code. |
| `$identification_code` | `NULL` | Recovery token for password reset. |
| `$recover_message` | Predefined HTML | Dynamic message displayed in recovery form. |

---

### Core Logic Flow

#### 1. Password Recovery Processing (`__recover`)

##### Purpose
Processes a password reset request using a valid recovery token. Validates the token, retrieves associated user data, generates a new password, updates credentials, and sends a notification email.

##### Parameters
None (relies on global `$identification_code`).

##### Return Values
None (outputs HTML response and exits).

##### Inner Mechanisms
1. **Token Validation**: Checks if `$identification_code` is non-empty.
2. **Data Retrieval**: Loads recovery data from `#system/identification.recover` using the token as key.
3. **User Resolution**: Determines user type (`permission` or `profile`) and retrieves email.
4. **Password Update**:
   - For `permission`: Updates password in `#system/permission` data store.
   - For `profile`: Updates password in user profile via `profile` module.
5. **Email Dispatch**: Sends new password via SMTP if available.
6. **Cleanup**: Deletes recovery token after use.

##### Usage Context
Triggered when a user clicks the recovery link in their email. URL must include `identification_message=__recover` and `identification_code=<token>`.

---

#### 2. Password Recovery Initiation (`_recover`)

##### Purpose
Validates user input (username and CAPTCHA) and initiates password recovery by generating a recovery token, storing it, and sending a recovery email.

##### Parameters
None (relies on globals `$identification_user`, `$identification_captcha_key`, `$identification_captcha_code`).

##### Return Values
None (outputs HTML response and exits on success).

##### Inner Mechanisms
1. **CAPTCHA Verification**: Validates CAPTCHA if the module is loaded.
2. **User Lookup**: Searches for user in `permission` and `profile` data stores.
3. **Email Validation**: Uses `verify_email()` to check if the user has a valid email.
4. **Token Generation**: Creates a 16-character unique token with 1-hour expiry.
5. **Data Storage**: Stores recovery data in `#system/identification.recover`.
6. **Email Dispatch**: Sends recovery link via SMTP.

##### Usage Context
Triggered when user submits the recovery form with `identification_message=_recover`.

---

#### 3. Recovery Form Rendering (`recover`)

##### Purpose
Renders the password recovery form with username input and optional CAPTCHA.

##### Parameters
None (relies on globals `$identification_user`, `$recover_message`).

##### Return Values
None (outputs HTML form).

##### Inner Mechanisms
1. **Form Structure**: Renders a `<form>` with `POST` method targeting the current URL.
2. **Dynamic Messaging**: Displays `$recover_message` (success/error).
3. **CAPTCHA Integration**: Dynamically loads and renders CAPTCHA if the module is available.
4. **CSRF Protection**: Uses `cms_url()` to generate form action with CSRF token.

##### Usage Context
Displayed when `identification_message=recover` or after failed recovery initiation (`_recover`).

---

#### 4. Login Form Rendering

##### Purpose
Renders the login form with username, password, and optional legacy password checkbox. Handles brute-force protection and dynamic messaging.

##### Parameters
None (relies on globals `$location`, `$attempt_count`, `$response`).

##### Return Values
None (outputs HTML form).

##### Inner Mechanisms
1. **Brute-Force Protection**:
   - Tracks failed attempts per IP using `cms_cache()`.
   - Blocks login for `CMS_LOGIN_BLOCK_TIME` if `CMS_LOGIN_ATTEMPT_MAX` is exceeded.
   - Displays remaining attempts.
2. **Dynamic Location Handling**:
   - Redirects to `$location` after login.
   - Falls back to current URL if `$location` is empty.
3. **Legacy Password Support**:
   - Uses JavaScript to hash passwords client-side (SHA-256 or MD5+SHA-256 for legacy).
   - Salt is retrieved from `cms_cache("cms.salt_password")`.
4. **Form Structure**:
   - Username and password inputs with `autocomplete` attributes.
   - Legacy password checkbox for backward compatibility.
   - Link to recovery form.

##### Usage Context
Default state when no `identification_message` is set. Used for all standard login attempts.

---

### JavaScript Functions

#### `identification_submit()`

##### Purpose
Handles form submission for login, applying client-side password hashing.

##### Parameters
None.

##### Return Values
`false` to prevent default form submission.

##### Inner Mechanisms
1. Checks if legacy password checkbox is checked.
2. Dynamically loads `md5.js` if legacy mode is enabled.
3. Calls `_identification_submit()` with legacy flag.

##### Usage Context
Triggered by `onsubmit` event of the login form.

---

#### `_identification_submit(legacy = true)`

##### Purpose
Performs client-side password hashing before form submission.

##### Parameters

| Name | Type | Description |
|------|------|-------------|
| `legacy` | `boolean` | If `true`, applies MD5 before SHA-256. |

##### Return Values
None.

##### Inner Mechanisms
1. Retrieves salt from `cms_cache("cms.salt_password")`.
2. Hashes password:
   - Legacy: `sha256(salt + md5(password))`
   - Modern: `sha256(salt + sha256(password))`
3. Updates password field value and submits form.

##### Usage Context
Called by `identification_submit()` after optional MD5 script load.

---

### Key Utility Integrations

| Utility | Purpose |
|---------|---------|
| `cms_url()` | Generates URLs with CSRF protection and parameter merging. |
| `cms_cache()` | Tracks login attempts and retrieves password salt. |
| `cms_load()` | Dynamically loads `captcha`, `smtp`, and `profile` modules. |
| `data` class | Manages recovery tokens and user permissions. |
| `profile` class | Handles user profile data for non-system users. |
| `captcha` class | Generates and verifies CAPTCHA challenges. |
| `smtp_send()` | Dispatches recovery and password emails. |
| `unique_id()` | Generates secure tokens for recovery. |
| `verify_email()` | Validates email format. |
| `x()`, `q()`, `qb()` | Escaping for HTML, JavaScript, and binary data. |
| `insert()` | Renders module-specific templates (`recover_top`, `recover_bottom`, `top`, `bottom`). |

---

### Constants Used

| Constant | Description |
|----------|-------------|
| `CMS_L_MOD_IDENTIFICATION_*` | Localized strings for UI text. |
| `CMS_LOGIN_ATTEMPT_MAX` | Maximum allowed failed login attempts. |
| `CMS_LOGIN_BLOCK_TIME` | Duration (seconds) to block login after max attempts. |
| `CMS_ROOT_URL` | Base URL of the NUOS installation. |
| `CMS_MODULES_URL` | Path to modules directory. |
| `CMS_JAVASCRIPT_URL` | Path to JavaScript assets. |
| `CMS_DATA_URL` | Path to dynamic data (e.g., CAPTCHA images). |
| `CMS_USER` | Current authenticated user (or `"anonymous"`). |
| `CMS_NAME` | Site name. |
| `CMS_DB_PROFILE_USER` | Key for user lookup in profile data. |
| `CMS_L_COMMAND_CONFIRM` | Localized "Confirm" text. |
| `CMS_L_PASSWORD` | Localized "Password" text. |

---

### Security Considerations

1. **Brute-Force Protection**: IP-based rate limiting using `cms_cache()`.
2. **Client-Side Hashing**: Passwords are hashed before transmission to mitigate MITM attacks.
3. **CSRF Protection**: All forms use `cms_url()` to include CSRF tokens.
4. **CAPTCHA**: Optional CAPTCHA for recovery to prevent automated abuse.
5. **Token Expiry**: Recovery tokens expire after 1 hour.
6. **Secure Tokens**: `unique_id(16)` generates cryptographically secure tokens.
7. **Escaping**: All dynamic output is escaped using `x()`, `q()`, or `qb()`.

---

### Typical Usage Scenarios

1. **Standard Login**:
   - User navigates to login page.
   - Module renders login form.
   - User submits credentials; client-side hashing is applied.
   - Server validates credentials and redirects to `$location`.

2. **Password Recovery**:
   - User clicks "Forgot password" link.
   - Module renders recovery form with CAPTCHA.
   - User submits username; recovery email is sent.
   - User clicks recovery link in email.
   - Module processes token, generates new password, and sends it via email.

3. **Brute-Force Mitigation**:
   - After `CMS_LOGIN_ATTEMPT_MAX` failed attempts, login is blocked for `CMS_LOGIN_BLOCK_TIME`.
   - User sees a countdown message.

4. **Legacy Password Migration**:
   - Users with old MD5-hashed passwords can log in using the legacy checkbox.
   - After login, their password is automatically upgraded to SHA-256.


<!-- HASH:ec389d1f6bc729b9a148ce175e88625e -->
