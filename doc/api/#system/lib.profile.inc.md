# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.profile.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## profile_data

The `profile_data` class is a data container that holds all information related to a user profile. It is used as a transfer object between the application logic and the database layer. The class does not contain any methods for persistence; instead, it normalizes and prepares data during construction.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `id` | int | Unique profile identifier |
| `time_created` | int | Unix timestamp of creation |
| `time_updated` | int | Unix timestamp of last update |
| `code` | string | Unique code for the profile |
| `user` | string | Username (derived from email if not set) |
| `password` | string | Raw password (converted to random bytes if set) |
| `superuser` | string | Superuser flag or role |
| `enabled` | bool | Whether the profile is active |
| `company` | string | Company name |
| `prename` | string | First name |
| `surname` | string | Last name |
| `street` | string | Street address |
| `zipcode` | string | Postal/ZIP code |
| `city` | string | City |
| `country` | string | Country |
| `phone1` | string | Primary phone number |
| `phone2` | string | Secondary phone number |
| `mobile` | string | Mobile phone number |
| `fax` | string | Fax number |
| `email` | string | Email address |
| `url` | string | Website URL |
| `account_number` | string | Bank account number |
| `financial_institution` | string | Financial institution name |
| `bankcode` | string | Bank routing code |
| `account_holder` | string | Bank account holder name |
| `credit_card_number` | string | Credit card number |
| `credit_institute` | string | Credit card issuing institute |
| `credit_card_holder` | string | Credit card holder name |
| `credit_card_validity` | string | Credit card expiration date (MM/YYYY) |
| `comment` | string | Free-text comment |
| `field1`–`field20` | string | Custom fields |
| `name` | string | Computed full name |
| `address` | string | Computed formatted address |

### __construct

Normalizes and prepares all profile data fields upon instantiation.

#### Parameters

None.

#### Return Values

None.

#### Inner Mechanisms

- Casts `id` to integer.
- Strips whitespace from text fields using `stripspaces()`.
- Generates a unique `code` via `unique_id()` if empty.
- Converts a non-empty `password` to 32 random bytes using `random_bytes(32)`.
- Derives `user` from `email` if `user` is empty.
- Auto-fills `account_holder` and `credit_card_holder` from `prename` and `surname` if empty.
- Parses and normalizes `credit_card_validity` into `MM/YYYY` format.
- Computes `name` based on presence of `prename` and `surname`.
- Builds a multi-line `address` string from `street`, `zipcode`, `city`, and `country`.

#### Usage Example

```php
$data = new profile_data();
$data->prename = "John";
$data->surname = "Doe";
$data->email = "john@example.com";
$data->__construct(); // Normalize data
echo $data->name;     // Outputs: "John Doe"
echo $data->user;     // Outputs: "john@example.com"
```

---

## profile

The `profile` class is the main service class for managing user profiles. It handles database table verification, permission checks, and CRUD operations (create, read, update, delete) for profile records.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_PROFILE_PERMISSION_OPERATOR` | `"operator"` | Permission key required for profile operations |

### Database Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_PROFILE` | `CMS_DB_PREFIX . "profile"` | Main profile table name |
| `CMS_DB_PROFILE_INDEX` | `"id"` | Primary key column |
| `CMS_DB_PROFILE_TIME_CREATED` | `"time_created"` | Creation timestamp column |
| `CMS_DB_PROFILE_TIME_UPDATED` | `"time_updated"` | Update timestamp column |
| `CMS_DB_PROFILE_CODE` | `"code"` | Unique code column |
| `CMS_DB_PROFILE_USER` | `"user"` | Username column |
| `CMS_DB_PROFILE_PASSWORD` | `"password"` | Password hash column |
| `CMS_DB_PROFILE_SUPERUSER` | `"superuser"` | Superuser role column |
| `CMS_DB_PROFILE_ENABLED` | `"enabled"` | Enabled status column |
| `CMS_DB_PROFILE_COMPANY` | `"company"` | Company column |
| `CMS_DB_PROFILE_PRENAME` | `"prename"` | First name column |
| `CMS_DB_PROFILE_SURNAME` | `"surname"` | Last name column |
| `CMS_DB_PROFILE_STREET` | `"street"` | Street column |
| `CMS_DB_PROFILE_ZIPCODE` | `"zipcode"` | ZIP code column |
| `CMS_DB_PROFILE_CITY` | `"city"` | City column |
| `CMS_DB_PROFILE_COUNTRY` | `"country"` | Country column |
| `CMS_DB_PROFILE_PHONE_1` | `"phone1"` | Phone 1 column |
| `CMS_DB_PROFILE_PHONE_2` | `"phone2"` | Phone 2 column |
| `CMS_DB_PROFILE_MOBILE` | `"mobile"` | Mobile column |
| `CMS_DB_PROFILE_FAX` | `"fax"` | Fax column |
| `CMS_DB_PROFILE_EMAIL` | `"email"` | Email column |
| `CMS_DB_PROFILE_URL` | `"url"` | URL column |
| `CMS_DB_PROFILE_ACCOUNT_NUMBER` | `"account_number"` | Account number column |
| `CMS_DB_PROFILE_FINANCIAL_INSTITUTION` | `"financial_institution"` | Financial institution column |
| `CMS_DB_PROFILE_BANKCODE` | `"bankcode"` | Bank code column |
| `CMS_DB_PROFILE_ACCOUNT_HOLDER` | `"account_holder"` | Account holder column |
| `CMS_DB_PROFILE_CREDIT_CARD_NUMBER` | `"credit_card_number"` | Credit card number column |
| `CMS_DB_PROFILE_CREDIT_INSTITUTE` | `"credit_institute"` | Credit institute column |
| `CMS_DB_PROFILE_CREDIT_CARD_HOLDER` | `"credit_card_holder"` | Credit card holder column |
| `CMS_DB_PROFILE_CREDIT_CARD_VALIDITY` | `"credit_card_validity"` | Credit card validity column |
| `CMS_DB_PROFILE_COMMENT` | `"comment"` | Comment column |
| `CMS_DB_PROFILE_CUSTOM` | `CMS_DB_PREFIX . "profile_custom"` | Custom fields table name |
| `CMS_DB_PROFILE_CUSTOM_INDEX` | `"id"` | Custom fields primary key |
| `CMS_DB_PROFILE_CUSTOM_FIELD` | `"field"` | Prefix for custom field columns |

### Properties

| Name | Type | Description |
|------|------|-------------|
| `operator` | bool | Whether the current user has operator permissions |
| `enabled` | bool | Whether the profile system is enabled |

### __construct

Initializes the profile system by verifying database tables and checking permissions.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$override_permission` | bool | If `TRUE`, bypasses permission check and grants operator access |

#### Return Values

None.

#### Inner Mechanisms

- Creates a `mysql` instance and verifies the existence of `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` tables with their respective schemas.
- If both tables are verified, sets `enabled` to `TRUE`.
- Sets `operator` based on either the override flag or `cms_permission()` check.

#### Usage Example

```php
$profile = new profile();
if ($profile->enabled && $profile->operator) {
    // User can manage profiles
}
```

### add

Inserts a new profile record into the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | profile_data | Reference to a `profile_data` object containing profile information |

#### Return Values

| Type | Description |
|------|-------------|
| bool | `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms

- Checks if the system is enabled and the user is an operator.
- Calls `$profile_data->__construct()` to normalize data.
- Inserts the main profile record with all standard fields.
- Inserts custom fields (`field1`–`field20`) into the custom table.
- Sets `$profile_data->id` to the newly inserted ID.

#### Usage Example

```php
$profile = new profile();
$data = new profile_data();
$data->prename = "Alice";
$data->surname = "Smith";
$data->email = "alice@example.com";
if ($profile->add($data)) {
    echo "Profile created with ID: " . $data->id;
}
```

### set

Updates an existing profile record in the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | profile_data | Reference to a `profile_data` object with updated information |

#### Return Values

| Type | Description |
|------|-------------|
| bool | `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms

- Checks if the system is enabled and the user is an operator.
- Calls `$profile_data->__construct()` to normalize data.
- Updates the main profile record, conditionally updating the password if a new one was provided.
- Uses `ON DUPLICATE KEY UPDATE` to insert or update custom fields.
- If the password was changed, sets a cookie with the salted password hash.

#### Usage Example

```php
$profile = new profile();
$data = $profile->get(1);
$data->prename = "Updated";
$data->password = "newpassword123";
if ($profile->set($data)) {
    echo "Profile updated successfully.";
}
```

### get

Retrieves a profile record from the database by index.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | mixed | — | The value to search for |
| `$index_field` | string | `CMS_DB_PROFILE_INDEX` | The column name to search in |

#### Return Values

| Type | Description |
|------|-------------|
| profile_data | A populated `profile_data` object on success |
| bool | `FALSE` on failure |

#### Inner Mechanisms

- Checks if the system is enabled.
- Executes a `SELECT` query joining the main profile table with the custom fields table.
- Maps the result row to a new `profile_data` object.
- Calls `$profile_data->__construct()` to normalize retrieved data.

#### Usage Example

```php
$profile = new profile();
$data = $profile->get(1);
if ($data) {
    echo "Name: " . $data->name;
    echo "Email: " . $data->email;
}
```

### del

Deletes a profile record and its associated custom fields from the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | int | The profile ID to delete |

#### Return Values

| Type | Description |
|------|-------------|
| bool | `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms

- Checks if the system is enabled and the user is an operator.
- Deletes the record from both the main profile table and the custom fields table.

#### Usage Example

```php
$profile = new profile();
if ($profile->del(5)) {
    echo "Profile deleted successfully.";
}
```

### get_permission

Retrieves permission-related data for a given user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$user` | string | The username to look up |

#### Return Values

| Type | Description |
|------|-------------|
| array | An associative array with keys `index`, `password`, `superuser`, and `name` |
| bool | `FALSE` if the user is not found or disabled |

#### Inner Mechanisms

- Checks if the system is enabled.
- Uses a static buffer to cache results per user.
- Queries the database for the user's ID, password hash, superuser status, and name.
- Returns an array with the retrieved data or `FALSE` if not found.

#### Usage Example

```php
$profile = new profile();
$permission = $profile->get_permission("john@example.com");
if ($permission) {
    echo "User ID: " . $permission["index"];
    echo "Is Superuser: " . ($permission["superuser"] ? "Yes" : "No");
}
```


<!-- HASH:e2249f070dad52c5e6f2e3d8fc8ec8b5 -->
