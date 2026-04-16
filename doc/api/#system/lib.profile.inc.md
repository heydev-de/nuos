# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.profile.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Profile Data Management

This file provides the core classes for managing user profile data within the NUOS platform. It consists of two primary classes:

1. **`profile_data`** – A data container for all profile-related information (personal, contact, financial, and administrative data).
2. **`profile`** – A service class handling database operations (CRUD) for user profiles, including permission checks and data validation.

---

## `profile_data` Class

A structured container for user profile data, ensuring consistent formatting and validation of fields.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `id` | `int` | Unique profile identifier. |
| `time_created` | `int` | Unix timestamp of profile creation. |
| `time_updated` | `int` | Unix timestamp of last update. |
| `code` | `string` | Unique alphanumeric profile code. |
| `user` | `string` | Login username (defaults to email or code). |
| `password` | `string` | Plaintext password (hashed before storage). |
| `superuser` | `string` | Superuser identifier (if applicable). |
| `enabled` | `bool` | Profile activation status. |
| `company` | `string` | Company name. |
| `prename` | `string` | First name. |
| `surname` | `string` | Last name. |
| `street` | `string` | Street address. |
| `zipcode` | `string` | Postal code. |
| `city` | `string` | City. |
| `country` | `string` | Country. |
| `phone1`, `phone2` | `string` | Primary and secondary phone numbers. |
| `mobile` | `string` | Mobile phone number. |
| `fax` | `string` | Fax number. |
| `email` | `string` | Email address. |
| `url` | `string` | Website URL. |
| `account_number` | `string` | Bank account number. |
| `financial_institution` | `string` | Bank name. |
| `bankcode` | `string` | Bank routing code. |
| `account_holder` | `string` | Account holder name (auto-filled from prename + surname). |
| `credit_card_number` | `string` | Credit card number. |
| `credit_institute` | `string` | Credit card issuer. |
| `credit_card_holder` | `string` | Credit card holder name (auto-filled from prename + surname). |
| `credit_card_validity` | `string` | Credit card expiry (MM/YYYY, auto-corrected). |
| `comment` | `string` | Administrative notes. |
| `field1`–`field20` | `string` | Customizable miscellaneous fields. |
| `name` | `string` | Full name (auto-generated: "prename surname" or "surname, prename"). |
| `address` | `string` | Formatted address (multi-line, auto-generated from street, zipcode, city, country). |

### `__construct()`

**Purpose:**
Validates and normalizes all profile data fields upon instantiation or manual call.

**Parameters:**
None (operates on object properties).

**Return Values:**
None (modifies object properties in-place).

**Inner Mechanisms:**
- **Index:** Casts `id` to integer.
- **Code:** Strips whitespace; generates a unique ID if empty.
- **Strings:** Trims whitespace from all text fields using `stripspaces()`.
- **URL:** Trims using `utf8_trim()` for multibyte safety.
- **Username:** Defaults to `email` if empty, otherwise uses `code`.
- **Account/Credit Card Holder:** Auto-fills from `prename` + `surname` if empty.
- **Credit Card Validity:** Validates and corrects MM/YYYY format; defaults to current month/year if invalid.
- **Name:** Constructs full name from `prename` and `surname`.
- **Address:** Builds multi-line address string from street, zipcode, city, and country.

**Usage Context:**
- Automatically called during object creation.
- Can be manually invoked to re-normalize data after property modifications.

---

## `profile` Class

Handles database operations for user profiles, including creation, retrieval, updates, and deletion.

### Constants

#### Permission
| Name | Value | Description |
|------|-------|-------------|
| `CMS_PROFILE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for profile management. |

#### Database Schema
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_PROFILE` | `CMS_DB_PREFIX . "profile"` | Main profile table. |
| `CMS_DB_PROFILE_INDEX` | `"id"` | Primary key. |
| `CMS_DB_PROFILE_TIME_CREATED` | `"time_created"` | Creation timestamp. |
| `CMS_DB_PROFILE_TIME_UPDATED` | `"time_updated"` | Last update timestamp. |
| `CMS_DB_PROFILE_CODE` | `"code"` | Unique profile code. |
| `CMS_DB_PROFILE_USER` | `"user"` | Login username. |
| `CMS_DB_PROFILE_PASSWORD` | `"password"` | Hashed password. |
| `CMS_DB_PROFILE_SUPERUSER` | `"superuser"` | Superuser identifier. |
| `CMS_DB_PROFILE_ENABLED` | `"enabled"` | Activation status (`'1'` or `'0'`). |
| `CMS_DB_PROFILE_COMPANY`–`CMS_DB_PROFILE_COMMENT` | Various | Personal, contact, financial, and miscellaneous data fields. |
| `CMS_DB_PROFILE_CUSTOM` | `CMS_DB_PREFIX . "profile_custom"` | Custom fields table. |
| `CMS_DB_PROFILE_CUSTOM_INDEX` | `"id"` | Foreign key to `CMS_DB_PROFILE_INDEX`. |
| `CMS_DB_PROFILE_CUSTOM_FIELD` | `"field"` | Prefix for custom fields (`field1`–`field20`). |

### Properties

| Name | Type | Description |
|------|------|-------------|
| `mysql` | `mysql` | Database connection instance. |
| `operator` | `bool` | Permission flag for profile operations. |
| `enabled` | `bool` | Service availability flag. |

### `__construct($override_permission = FALSE)`

**Purpose:**
Initializes the profile service, verifies database tables, and checks operator permissions.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$override_permission` | `bool` | `FALSE` | Bypasses permission check if `TRUE`. |

**Return Values:**
None (initializes object state).

**Inner Mechanisms:**
- Creates a new `mysql` instance.
- Verifies the existence and schema of `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` tables.
- Sets `operator` flag based on `CMS_PROFILE_PERMISSION_OPERATOR` permission or `$override_permission`.
- Sets `enabled` to `TRUE` if tables are verified.

**Usage Context:**
- Called during service initialization.
- Use `$override_permission` for administrative scripts requiring elevated access.

---

### `add(&$profile_data)`

**Purpose:**
Inserts a new profile into the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | `profile_data` | Profile data object (passed by reference). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates service availability and operator permissions.
- Calls `$profile_data->__construct()` to normalize data.
- Inserts into `CMS_DB_PROFILE` with current timestamps.
- On success, sets `$profile_data->id` to the inserted ID.
- Inserts custom fields into `CMS_DB_PROFILE_CUSTOM`.

**Usage Context:**
- Used when registering new users or creating profiles programmatically.
- Example:
  ```php
  $data = new profile_data();
  $data->prename = "John";
  $data->surname = "Doe";
  $data->email = "john@example.com";
  $profile_service->add($data);
  ```

---

### `set(&$profile_data)`

**Purpose:**
Updates an existing profile in the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | `profile_data` | Profile data object (passed by reference). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates service availability and operator permissions.
- Calls `$profile_data->__construct()` to normalize data.
- Hashes password if modified.
- Updates `CMS_DB_PROFILE` with current timestamp.
- Uses `INSERT ... ON DUPLICATE KEY UPDATE` for custom fields in `CMS_DB_PROFILE_CUSTOM`.
- Updates session cookie if password was changed.

**Usage Context:**
- Used for profile edits, password changes, or administrative updates.
- Example:
  ```php
  $data = $profile_service->get(123);
  $data->prename = "Jonathan";
  $profile_service->set($data);
  ```

---

### `get($index, $index_field = CMS_DB_PROFILE_INDEX)`

**Purpose:**
Retrieves a profile by its identifier.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `mixed` | — | Profile identifier (ID, code, or username). |
| `$index_field` | `string` | `CMS_DB_PROFILE_INDEX` | Field to search by (`id`, `code`, or `user`). |

**Return Values:**
- `profile_data`: Populated profile data object on success.
- `bool`: `FALSE` on failure.

**Inner Mechanisms:**
- Joins `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` tables.
- Maps database fields to `profile_data` properties.
- Calls `$profile_data->__construct()` to normalize retrieved data.

**Usage Context:**
- Used for displaying, editing, or authenticating profiles.
- Example:
  ```php
  $data = $profile_service->get("john.doe", CMS_DB_PROFILE_USER);
  ```

---

### `del($index)`

**Purpose:**
Deletes a profile from the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Profile ID to delete. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Deletes from both `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM`.
- Uses `LIMIT 1` to ensure single-row deletion.

**Usage Context:**
- Used for account removal or administrative cleanup.
- Example:
  ```php
  $profile_service->del(123);
  ```

---

### `get_permission($user)`

**Purpose:**
Retrieves permission-related data for a given username.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username to check. |

**Return Values:**
- `array`: Associative array with keys `index`, `password`, `superuser`, and `name` on success.
- `bool`: `FALSE` if user not found or disabled.

**Inner Mechanisms:**
- Uses static `$buffer` to cache results and avoid redundant queries.
- Queries `CMS_DB_PROFILE` for enabled users only.

**Usage Context:**
- Used during authentication and permission checks.
- Example:
  ```php
  $permission = $profile_service->get_permission("john.doe");
  if ($permission && $permission["superuser"]) { /* ... */ }
  ```


<!-- HASH:c3d95e5950942f7cf296df5f946f7cfd -->
