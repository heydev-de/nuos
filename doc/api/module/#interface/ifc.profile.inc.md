# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.profile.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Profile Interface (`ifc.profile.inc`)

This file implements the **Profile Management Interface** for the PWNC Web Platform. It provides a complete user interface for viewing,editing, searching, and configuring user profiles, including administrative and personal data, contact information, financial details, and custom fields.

The interface is **permission-aware** and **role-based**, distinguishing between regular users (with read access) and operators (with full CRUD and configuration privileges). It integrates tightly with the platform's **data**, **permission**, **cache**, and **UI** subsystems.

---

### **Constants & Configuration Keys**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DB_PROFILE_*` | Various | Database field names for profile data (e.g., `CMS_DB_PROFILE_INDEX`, `CMS_DB_PROFILE_CODE`). |
| `CMS_DB_PROFILE_CUSTOM_FIELD` | `"custom_field"` | Prefix for custom field configuration keys. |
| `CMS_PROFILE_PERMISSION_OPERATOR` | `"operator"` | Permission key granting operator privileges. |
| `CMS_L_*` | Various | Localized string constants for UI labels, commands, and messages. |

---

### **Core Workflow**

1. **Initialization**
   - Loads the `profile` library and checks if the module is enabled.
   - Verifies user permissions (minimum `CMS_L_ACCESS`; operators require `CMS_L_OPERATOR`).
   - Instantiates the `profile` object and initializes the interface.

2. **Message Handling**
   - Processes incoming interface messages (e.g., `add`, `delete`, `display`, `config_base`).
   - Delegates actions to the `profile` object and updates the response state (`CMS_MSG_DONE`/`CMS_MSG_ERROR`).

3. **Main Display**
   - Renders a searchable, paginated list of profiles with filtering, sorting, and grouping.
   - Supports selection for bulk operations (e.g., deletion) or single-profile actions (e.g., editing).

4. **Configuration**
   - Allows operators to toggle visibility, editability, and requirement flags for all profile fields (base and custom).

---

### **Message Handlers**

#### ### `add`
**Purpose**: Creates a new profile.
**Parameters**: None (uses a new `profile_data` object).
**Return Values**:
- `CMS_MSG_DONE` on success (sets `$object` to the new profile ID).
- `CMS_MSG_ERROR` on failure.
**Inner Mechanisms**:
- Instantiates a blank `profile_data` object.
- Delegates creation to `$_profile->add()`.
- Updates the interface response and redirects to the new profile on success.
**Usage Example**:
```php
// Triggered via UI when an operator clicks "Add Profile".
// The interface automatically opens the new profile for editing.
```

---

#### ### `delete`
**Purpose**: Deletes one or more profiles.
**Parameters**:
- `$list` (array): Profile IDs to delete (from checkbox selections).
**Return Values**:
- `CMS_MSG_DONE` if all deletions succeed.
- `CMS_MSG_ERROR` if any deletion fails.
**Inner Mechanisms**:
- Iterates over `$list` and calls `$_profile->del()` for each ID.
- Aggregates errors and sets the response state.
**Usage Example**:
```php
// Bulk-delete profiles with IDs 1001, 1002, and 1003.
$list = [1001, 1002, 1003];
foreach ($list as $id) {
    $_profile->del($id); // Returns TRUE on success.
}
```

---

#### ### `display` / `display_save`
**Purpose**: Renders or saves a single profile's data.
**Parameters**:
- `$object` (string): Profile ID to display/edit.
- `$ifc_param1`–`$ifc_param28` (mixed): Form fields for profile data (e.g., `code`, `user`, `prename`).
**Return Values**:
- `CMS_MSG_DONE` on successful save.
- `CMS_MSG_ERROR` on failure.
- Closes the interface if the profile doesn’t exist.
**Inner Mechanisms**:
- **Display Mode**:
  - Fetches the profile data via `$_profile->get($object)`.
  - Renders a multi-tab form with fields for administrative, personal, contact, financial, and custom data.
  - Populates dropdowns (e.g., superuser selection) and handles UI logic (e.g., password generation).
- **Save Mode**:
  - Maps `$ifc_param*` fields to `profile_data` properties.
  - Delegates saving to `$_profile->set()`.
**Usage Example**:
```php
// Save changes to profile ID 1001.
$profile_data = $_profile->get(1001);
$profile_data->prename = "John";
$profile_data->surname = "Doe";
$_profile->set($profile_data); // Returns TRUE on success.
```

---

#### ### `config_base` / `_config_base`
**Purpose**: Configures visibility, editability, and requirement flags for base profile fields.
**Parameters**:
- `$ifc_param1`–`$ifc_param78` (bool): Checkbox states for field configurations (e.g., `visible`, `editable`, `required`).
**Return Values**:
- `CMS_MSG_DONE` on successful save.
- `CMS_MSG_ERROR` on failure.
**Inner Mechanisms**:
- **Display Mode**:
  - Renders a table with checkboxes for each field’s flags.
  - Loads current settings from the `#system/profile` data store.
- **Save Mode**:
  - Updates the data store with new flag values.
  - Saves changes via `$data->save()`.
**Usage Example**:
```php
// Make the "email" field required.
$data = new data("#system/profile");
$data->set(TRUE, CMS_DB_PROFILE_EMAIL, "required");
$data->save();
```

---

#### ### `config_custom` / `_config_custom`
**Purpose**: Configures custom profile fields (1–20).
**Parameters**:
- `$ifc_param*` (mixed): Field names, visibility, editability, and requirement flags.
**Return Values**:
- `CMS_MSG_DONE` on successful save.
- `CMS_MSG_ERROR` on failure.
**Inner Mechanisms**:
- **Display Mode**:
  - Renders a table with text inputs for field names and checkboxes for flags.
- **Save Mode**:
  - Updates the data store with new field configurations.
**Usage Example**:
```php
// Configure custom field 1 as visible and editable.
$data = new data("#system/profile");
$data->set("Department", CMS_DB_PROFILE_CUSTOM_FIELD . "1", "title");
$data->set(TRUE, CMS_DB_PROFILE_CUSTOM_FIELD . "1", "visible");
$data->set(TRUE, CMS_DB_PROFILE_CUSTOM_FIELD . "1", "editable");
$data->save();
```

---

#### ### `reset`
**Purpose**: Resets the profile list’s filter, grouping, and pagination settings.
**Parameters**: None.
**Return Values**: None.
**Inner Mechanisms**:
- Clears cached user preferences for the profile interface.
**Usage Example**:
```php
// Reset all filters and sorting for the current user.
cms_cache_delete([
    "profile." . CMS_USER . ".filter_field",
    "profile." . CMS_USER . ".filter_value",
    "profile." . CMS_USER . ".order"
]);
```

---

### **Main Display Logic**

#### **Filtering & Sorting**
- **Filter Options**:
  - `LIKE '#value#%'` (starts with), `%#value#%` (contains), etc.
- **Sorting Options**:
  - Index, code, surname (ascending/descending).
- **Grouping**:
  - By company, surname, country, etc.
- **Pagination**:
  - Supports limits of 10, 25, 50, 100, 250, or 500 rows per page.

#### **UI Components**
- **Search Bar**:
  - Dropdowns for field, condition, and value.
- **Action Buttons**:
  - Add, delete, configure, reset.
- **Profile List**:
  - Displays index, code, name, phone, and email.
  - Supports selection for bulk operations or single-profile actions (e.g., editing).
  - Clicking a profile opens it in a detailed view.

#### **JavaScript Helpers**
- `r(value)`: Returns a selected profile ID (for selection interfaces).
- `d(index)`: Opens a profile for editing.
- `o(value)`: Sets the sorting order.
- `p(number)`: Navigates to a specific page.

---

### **Usage Examples**

#### **1. Displaying a Profile**
```php
// Open profile ID 1001 in the interface.
$object = 1001;
$profile_data = $_profile->get($object);
if ($profile_data) {
    // Render the profile form.
    $ifc = new ifc(CMS_MSG_NONE, "profile", NULL, NULL, NULL, $profile_data->name);
    $ifc->set("Prename", "text 35 255 w", $profile_data->prename);
    $ifc->set("Surname", "text 35 255 w", $profile_data->surname);
    $ifc->close();
}
```

#### **2. Configuring Base Fields**
```php
// Make the "surname" field required and non-editable.
$data = new data("#system/profile");
$data->set(TRUE, CMS_DB_PROFILE_SURNAME, "required");
$data->set(FALSE, CMS_DB_PROFILE_SURNAME, "editable");
$data->save();
```

#### **3. Searching Profiles**
```php
// Filter profiles where the surname starts with "Doe".
$filter_field = CMS_DB_PROFILE_SURNAME;
$filter_option = 0; // "LIKE '#value#%'"
$filter_value = "Doe";
$query = "SELECT * FROM " . CMS_DB_PROFILE .
         " WHERE " . sqlesc($filter_field) . " LIKE '" . sqlesc($filter_value) . "%'";
$result = mysql_query($query);
```

#### **4. Bulk Deletion**
```php
// Delete profiles with IDs 1001, 1002, and 1003.
$list = [1001, 1002, 1003];
$error = FALSE;
foreach ($list as $id) {
    if (!$_profile->del($id)) {
        $error = TRUE;
    }
}
$ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
```

---

### **Key Integration Points**

| Component | Purpose |
|-----------|---------|
| `profile` class | Core logic for profile CRUD operations. |
| `profile_data` class | Data container for profile fields. |
| `data` class | Handles configuration storage (e.g., field flags). |
| `ifc` class | Renders UI elements (forms, tables, buttons). |
| `cms_cache` | Persists user preferences (e.g., filters, sorting). |
| `permission_is_user` | Validates superuser dropdown options. |
| `mysql_*` wrappers | Executes database queries for profile lists. |


<!-- HASH:2dba4153e5bcc83581bbf7665460286f -->
