# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.profile.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Profile Data Management (`lib.profile.inc`)

This file provides the core profile management functionality for the PWNC Web Platform, consisting of two main components:

1. **`profile_data` class** – A data container for user profile information with automatic normalization and validation.
2. **`profile` class** – A database-backed manager for creating, reading, updating, and deleting user profiles, including permission handling.

---

## `profile_data` Class

A structured container for user profile data with automatic normalization, validation, and formatting of fields.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `id` | `int` | Unique numeric identifier for the profile. |
| `time_created` | `int` | Unix timestamp of profile creation. |
| `time_updated` | `int` | Unix timestamp of last profile update. |
| `code` | `string` | Unique alphanumeric identifier (auto-generated if empty). |
| `user` | `string` | Login username (defaults to `code` or `email` if empty). |
| `password` | `string` | Plaintext password (auto-generated if empty). |
| `superuser` | `string` | Superuser identifier (if applicable). |
| `enabled` | `bool` | Whether the profile is active. |
| `company`, `prename`, `surname`, `street`, `zipcode`, `city`, `country` | `string` | Personal and address information. |
| `phone1`, `phone2`, `mobile`, `fax`, `email`, `url` | `string` | Contact details. |
| `account_number`, `financial_institution`, `bankcode`, `account_holder` | `string` | Bank account details. |
| `credit_card_number`, `credit_institute`, `credit_card_holder`, `credit_card_validity` | `string` | Credit card information. |
| `comment` | `string` | Free-form notes. |
| `field1` to `field20` | `string` | Customizable miscellaneous fields. |
| `name` | `string` | Formatted full name (e.g., "Doe, John"). |
| `address` | `string` | Formatted multi-line address. |

### Constructor (`__construct()`)

**Purpose:**
Normalizes and validates all profile fields upon instantiation or manual call.

**Parameters:**
None (operates on object properties).

**Return Value:**
None.

**Inner Mechanisms:**
- Ensures `id` is cast to integer.
- Generates a unique `code` if empty.
- Generates a random password if empty.
- Trims and normalizes all string fields using `stripspaces()` or `utf8_trim()`.
- Sets `user` to `code` or `email` if empty.
- Auto-fills `account_holder` and `credit_card_holder` from `prename` and `surname` if empty.
- Validates and normalizes `credit_card_validity` to `MM/YYYY` format.
- Constructs `name` and `address` from component fields.

**Usage Context:**
Automatically called during object creation or after database retrieval. Can be manually triggered to re-normalize data.

**Example:**
```php
$data = new profile_data();
$data->prename = "John";
$data->surname = "Doe";
$data->street = "123 Main St";
$data->city = "Metropolis";
$data->__construct(); // Normalizes fields and builds name/address
echo $data->name; // Output: "Doe, John"
echo $data->address; // Output: "123 Main St\nMetropolis"
```

---

## `profile` Class

Manages database operations for user profiles, including CRUD operations and permission checks.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_PROFILE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for profile management. |
| `CMS_DB_PROFILE` | `CMS_DB_PREFIX . "profile"` | Main profile table name. |
| `CMS_DB_PROFILE_INDEX` | `"id"` | Primary key field. |
| `CMS_DB_PROFILE_TIME_CREATED` | `"time_created"` | Creation timestamp field. |
| `CMS_DB_PROFILE_TIME_UPDATED` | `"time_updated"` | Last update timestamp field. |
| `CMS_DB_PROFILE_CODE` | `"code"` | Unique code field. |
| `CMS_DB_PROFILE_USER` | `"user"` | Username field. |
| `CMS_DB_PROFILE_PASSWORD` | `"password"` | Hashed password field. |
| `CMS_DB_PROFILE_SUPERUSER` | `"superuser"` | Superuser identifier field. |
| `CMS_DB_PROFILE_ENABLED` | `"enabled"` | Enabled status field. |
| `CMS_DB_PROFILE_COMPANY` | `"company"` | Company name field. |
| `CMS_DB_PROFILE_PRENAME` | `"prename"` | First name field. |
| `CMS_DB_PROFILE_SURNAME` | `"surname"` | Last name field. |
| `CMS_DB_PROFILE_STREET` | `"street"` | Street address field. |
| `CMS_DB_PROFILE_ZIPCODE` | `"zipcode"` | ZIP/postal code field. |
| `CMS_DB_PROFILE_CITY` | `"city"` | City field. |
| `CMS_DB_PROFILE_COUNTRY` | `"country"` | Country field. |
| `CMS_DB_PROFILE_PHONE_1` | `"phone1"` | Primary phone field. |
| `CMS_DB_PROFILE_PHONE_2` | `"phone2"` | Secondary phone field. |
| `CMS_DB_PROFILE_MOBILE` | `"mobile"` | Mobile phone field. |
| `CMS_DB_PROFILE_FAX` | `"fax"` | Fax number field. |
| `CMS_DB_PROFILE_EMAIL` | `"email"` | Email address field. |
| `CMS_DB_PROFILE_URL` | `"url"` | Website URL field. |
| `CMS_DB_PROFILE_ACCOUNT_NUMBER` | `"account_number"` | Bank account number field. |
| `CMS_DB_PROFILE_FINANCIAL_INSTITUTION` | `"financial_institution"` | Bank name field. |
| `CMS_DB_PROFILE_BANKCODE` | `"bankcode"` | Bank code field. |
| `CMS_DB_PROFILE_ACCOUNT_HOLDER` | `"account_holder"` | Account holder name field. |
| `CMS_DB_PROFILE_CREDIT_CARD_NUMBER` | `"credit_card_number"` | Credit card number field. |
| `CMS_DB_PROFILE_CREDIT_INSTITUTE` | `"credit_institute"` | Credit card issuer field. |
| `CMS_DB_PROFILE_CREDIT_CARD_HOLDER` | `"credit_card_holder"` | Credit card holder name field. |
| `CMS_DB_PROFILE_CREDIT_CARD_VALIDITY` | `"credit_card_validity"` | Credit card expiry (MM/YYYY) field. |
| `CMS_DB_PROFILE_COMMENT` | `"comment"` | Comment field. |
| `CMS_DB_PROFILE_CUSTOM` | `CMS_DB_PREFIX . "profile_custom"` | Custom fields table name. |
| `CMS_DB_PROFILE_CUSTOM_INDEX` | `"id"` | Foreign key to `profile` table. |
| `CMS_DB_PROFILE_CUSTOM_FIELD` | `"field"` | Prefix for custom fields (`field1` to `field20`). |

### Properties

| Name | Type | Description |
|------|------|-------------|
| `operator` | `bool` | Whether the current user has operator permissions. |
| `enabled` | `bool` | Whether the profile system is enabled and tables exist. |

### Constructor (`__construct($override_permission = FALSE)`)

**Purpose:**
Initializes the profile manager, verifies database tables, and checks operator permissions.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$override_permission` | `bool` | If `TRUE`, bypasses permission checks. |

**Return Value:**
None.

**Inner Mechanisms:**
- Creates a `mysql` instance and verifies the existence of `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` tables with correct schema.
- Sets `operator` based on `cms_permission(CMS_PROFILE_PERMISSION_OPERATOR)` unless overridden.
- Sets `enabled` to `TRUE` only if tables exist and are valid.

**Usage Context:**
Called during instantiation of the profile manager. Use `$override_permission` only in trusted administrative contexts.

**Example:**
```php
$profileManager = new profile(); // Checks permissions
$adminManager = new profile(TRUE); // Bypasses permission checks
```

---

### `add(&$profile_data)`

**Purpose:**
Adds a new profile to the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | `profile_data` | Profile data object (passed by reference). |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates that the system is enabled and the user has operator permissions.
- Calls `$profile_data->__construct()` to normalize data.
- Inserts core profile data into `CMS_DB_PROFILE`.
- On success, sets `$profile_data->id` to the auto-incremented ID.
- Inserts custom fields (`field1` to `field20`) into `CMS_DB_PROFILE_CUSTOM`.
- Uses `sqlesc()` for SQL escaping and `hash64()` for password hashing.

**Usage Context:**
Used to create new user profiles. Requires operator permissions.

**Example:**
```php
$data = new profile_data();
$data->prename = "Jane";
$data->surname = "Smith";
$data->email = "jane@example.com";
$data->password = "secure123";

$manager = new profile();
if ($manager->add($data)) {
    echo "Profile created with ID: " . $data->id;
} else {
    echo "Failed to create profile.";
}
```

---

### `set(&$profile_data)`

**Purpose:**
Updates an existing profile in the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | `profile_data` | Profile data object (passed by reference). Must have `id` set. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates system and operator status.
- Normalizes data via `$profile_data->__construct()`.
- Updates core profile fields in `CMS_DB_PROFILE`.
- Uses `ON DUPLICATE KEY UPDATE` to insert or update custom fields in `CMS_DB_PROFILE_CUSTOM`.
- If password is updated, re-hashes it and updates the session cookie via `cms_set_cookie()`.

**Usage Context:**
Used to update existing profiles. Requires operator permissions.

**Example:**
```php
$manager = new profile();
$data = $manager->get(42); // Retrieve profile with ID 42
if ($data) {
    $data->prename = "Janet";
    $data->password = "newsecure456";
    if ($manager->set($data)) {
        echo "Profile updated.";
    }
}
```

---

### `get($index, $index_field = CMS_DB_PROFILE_INDEX)`

**Purpose:**
Retrieves a profile by ID, code, or username.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int|string` | Value to search for (e.g., ID, code, or username). |
| `$index_field` | `string` | Field to search by (default: `CMS_DB_PROFILE_INDEX`). Other options: `CMS_DB_PROFILE_CODE`, `CMS_DB_PROFILE_USER`. |

**Return Value:**
- `profile_data|FALSE`: Profile data object on success, `FALSE` on failure.

**Inner Mechanisms:**
- Joins `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` to retrieve all fields.
- Maps database results to a `profile_data` object.
- Calls `$profile_data->__construct()` to normalize data.

**Usage Context:**
Used to fetch profile data for display, editing, or authentication.

**Example:**
```php
$manager = new profile();
$user = $manager->get("jane@example.com", CMS_DB_PROFILE_USER);
if ($user) {
    echo "Hello, " . $user->name;
}
```

---

### `del($index)`

**Purpose:**
Deletes a profile by ID.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Profile ID to delete. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Deletes from both `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM`.
- Uses `LIMIT 1` to ensure only one row is affected.

**Usage Context:**
Used to remove profiles. Requires operator permissions.

**Example:**
```php
$manager = new profile();
if ($manager->del(42)) {
    echo "Profile deleted.";
}
```

---

### `get_permission($user)`

**Purpose:**
Retrieves permission-related data for a given username.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username to check. |

**Return Value:**
- `array|FALSE`: Associative array with keys `index`, `password`, `superuser`, and `name` on success; `FALSE` if user not found or disabled.

**Inner Mechanisms:**
- Uses a static buffer to cache results and avoid repeated database queries.
- Only returns data for enabled users (`CMS_DB_PROFILE_ENABLED = '1'`).

**Usage Context:**
Used during authentication and permission checks.

**Example:**
```php
$manager = new profile();
$perm = $manager->get_permission("jane@example.com");
if ($perm) {
    echo "User ID: " . $perm["index"];
    echo "Is Superuser: " . ($perm["superuser"] ? "Yes" : "No");
}
```


<!-- HASH:e29d49c4e3b7134c2387de4c395f46eb -->
