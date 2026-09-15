# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.identification.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.identification.inc)

- **Version:** `26.9.11.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mod.identification.inc

This module handles user authentication workflows within the PWNC Web Platform, including login, password recovery, and account identification. It supports both legacy and modern password hashing methods, integrates with CAPTCHA verification, and provides secure password reset functionality via email.

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `$location` | `NULL` | Target URL for form submission after login |
| `$identification_message` | `NULL` | Controls which workflow branch executes (`__recover`, `_recover`, `recover`, or login) |
| `$identification_user` | `NULL` | Username/email input for login or recovery |
| `$identification_email` | `NULL` | Email address associated with the user |
| `$identification_captcha_key` | `NULL` | User-entered CAPTCHA key |
| `$identification_captcha_code` | `NULL` | Hidden CAPTCHA verification code |
| `$identification_code` | `NULL` | Recovery token used in password reset |

### Workflow Branches

#### `__recover`
Handles the final step of password recovery where a user submits a valid recovery code.

##### Logic Flow
1. Validates that a recovery code was provided.
2. Retrieves recovery data from `#system/identification.recover`.
3. Deletes the recovery entry immediately to prevent reuse.
4. Generates a new random password using `unique_id()`.
5. Attempts to update the password based on user type:
   - **Permission-based users**: Updates password in `#system/permission` data store.
   - **Profile-based users**: Loads profile library and updates password in the profile record.
6. Sends the new password to the user's email if successful.

##### Usage Example
When a user clicks a password recovery link sent to their email:
```php
// User visits: /identification.php?identification_message=__recover&identification_code=abc123
// System validates code, resets password, and emails new credentials
```

#### `_recover`
Processes the initial password recovery request form submission.

##### Logic Flow
1. Verifies CAPTCHA if enabled.
2. Checks for a valid username.
3. Searches for the user in permissions first, then profiles.
4. Validates the user's email address.
5. Creates a time-limited (1 hour) recovery token.
6. Sends a recovery email with a secure link containing the token.

##### Usage Example
User submits recovery form:
```html
<form method="post">
  <input name="identification_user" value="john_doe">
  <input name="identification_captcha_key">
  <input type="hidden" name="identification_captcha_code" value="...">
  <button name="identification_message" value="_recover">Recover</button>
</form>
```

#### `recover` / `_recover` (Form Display)
Renders the password recovery form interface.

##### Features
- Username input field
- Optional CAPTCHA challenge
- Submit button to initiate recovery
- Responsive layout with proper escaping

##### Usage Example
Displayed when user navigates to recovery page:
```php
// URL: /identification.php?identification_message=recover
// Shows form with username field and CAPTCHA
```

#### Default (Login)
Handles standard login functionality with security features.

##### Security Features
- **Login attempt limiting**: Blocks after `CMS_LOGIN_ATTEMPT_MAX` failed attempts for `CMS_LOGIN_BLOCK_TIME` seconds
- **Client-side password hashing**: Uses SHA-256 with salt, supports legacy MD5 hashing
- **Location validation**: Ensures redirect URLs are safe and same-origin
- **No-script fallback**: Includes hidden field for non-JS environments

##### JavaScript Functions

###### `identification_submit()`
Intercepts form submission to perform client-side password hashing before sending to server.

**Parameters**: None  
**Returns**: `false` (prevents default form submission)

**Logic**:
1. Checks if legacy hashing is requested
2. Loads MD5 library dynamically if needed
3. Calls `_identification_submit()` with appropriate flag

###### `_identification_submit(legacy)`
Performs actual password hashing and form submission.

| Parameter | Type | Description |
|-----------|------|-------------|
| `legacy` | `boolean` | Whether to use legacy MD5 hashing |

**Logic**:
1. Retrieves salt value from server
2. Gets password input value
3. Converts to UTF-8 binary
4. Applies SHA-256 hashing with salt (and MD5 if legacy mode)
5. Sets hashed value back to form field
6. Submits form programmatically

##### Usage Example
Standard login form:
```html
<form id="identification-form" action="/login" method="post">
  <input name="cms_login_user" required>
  <input type="password" name="cms_login_password" required>
  <input type="checkbox" id="identification-legacy">
  <button type="submit">Login</button>
</form>
```

### Helper Functions Used

| Function | Purpose |
|----------|---------|
| `stre($v)` | Checks if value is empty |
| `nstre($v)` | Checks if value is not empty |
| `nstreq($a, $b)` | Checks string inequality |
| `cms_load($lib)` | Loads required libraries (captcha, smtp, profile) |
| `cms_url($params)` | Generates URLs with state management |
| `cms_cache($key)` | Accesses cached values (login attempts) |
| `unique_id($len)` | Generates random strings/tokens |
| `hash64($v)` | Hashes passwords for storage |
| `verify_email($email)` | Validates email format |
| `smtp_send($to, $subject, $body)` | Sends emails via SMTP |
| `x($s)` | XML/HTML escaping |
| `q($s)` | JavaScript string encoding |
| `qb($s)` | Binary-safe JavaScript encoding |
| `parse_url($url)` | Parses URLs for validation |
| `file($path)` | Reads host whitelist file |

### Configuration Constants

| Constant | Description |
|----------|-------------|
| `CMS_L_MOD_IDENTIFICATION_*` | Language strings for UI elements |
| `CMS_LOGIN_ATTEMPT_MAX` | Maximum failed login attempts before blocking |
| `CMS_LOGIN_BLOCK_TIME` | Duration (seconds) of login block |
| `CMS_ROOT_URL` | Base URL for the platform |
| `CMS_JAVASCRIPT_URL` | Path to JavaScript libraries |
| `CMS_DATA_PATH` | Filesystem path to data storage |
| `CMS_DOMAIN` | Default domain for host validation |
| `CMS_USER` | Current logged-in user |
| `CMS_NAME` | Site name |
| `CMS_DB_PROFILE_USER` | Database table for user profiles |
| `CMS_MODULES_URL` | URL path to modules |
| `CMS_COMMAND_CONFIRM` | Text for confirm buttons |
| `CMS_PASSWORD` | Label for password fields |

### Typical Usage Scenarios

1. **User Login**: Standard authentication flow with optional legacy support
2. **Password Recovery**: Two-step process (request → reset) with email verification
3. **Account Lockout**: Automatic blocking after repeated failed attempts
4. **CAPTCHA Protection**: Optional bot prevention on recovery forms
5. **Secure Redirects**: Validated return URLs to prevent open redirect vulnerabilities


<!-- HASH:34a219554ca7897fbc9c58fb3e01732a -->
