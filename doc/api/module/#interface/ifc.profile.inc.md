# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.profile.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# Profile Interface Module

## Overview

The file `module/#interface/ifc.profile.inc` is the main interface controller for the **Profile** module in the PWNC Web Platform. It handles all user-facing operations related to managing user profiles, including:

- **Creating** new profiles
- **Editing** existing profiles
- **Deleting** selected profiles
- **Configuring** field visibility, editability, and requirements
- **Displaying** a searchable, sortable, paginated list of profiles

This interface interacts heavily with the `profile` and `profile_data` classes, the `data` class for configuration storage, and the `ifc` class for rendering forms and tables.

---

## Initialization & Permissions

### Code Block: Interface Bootstrap

```php
if (! cms_load("profile")) ifc_inactive($ifc_page);
ifc_permission(["" => CMS_L_ACCESS, CMS_PROFILE_PERMISSION_OPERATOR => CMS_L_OPERATOR]);
$_profile = new profile();
if (! $_profile->enabled) ifc_inactive($ifc_page);
init($object);
```

**Purpose:**  
Loads the `profile` library, checks access permissions, instantiates the profile object, and initializes the current object context.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `""` | string | Default access level (`CMS_L_ACCESS`) |
| `CMS_PROFILE_PERMISSION_OPERATOR` | string | Operator-level permission key |

**Usage Context:**  
Executed at the start of every request to this interface. Ensures only authorized users can proceed.

---

## Message Handling

### Switch Statement: `CMS_IFC_MESSAGE`

Handles different actions based on the value of `CMS_IFC_MESSAGE`.

#### Case: `"add"`

```php
$profile_data = new profile_data();
if ($_profile->add($profile_data)) {
    $object = $profile_data->id;
    $ifc_response = CMS_MSG_DONE;
} else {
    $ifc_response = CMS_MSG_ERROR;
}
```

**Purpose:**  
Creates a new profile entry.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | `profile_data` | New profile data object |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | Success/failure of `$_profile->add()` |

**Inner Mechanism:**  
Instantiates a new `profile_data` object, attempts to add it via `$_profile->add()`, and sets the response accordingly.

**Usage Example:**
```php
// Triggered when a user clicks "Add Profile"
$profile_data = new profile_data();
$_profile->add($profile_data); // Inserts into database
```

---

#### Case: `"delete"`

```php
if (blank($list)) break;
$error = NULL;
foreach ($list AS $value) {
    if (! $_profile->del($value)) $error = TRUE;
}
$ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
```

**Purpose:**  
Deletes one or more selected profiles.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | array | List of profile IDs to delete |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | Whether any deletion failed |

**Inner Mechanism:**  
Iterates over `$list`, calling `$_profile->del()` for each ID. Sets `$error` if any fail.

**Usage Example:**
```php
// User selects multiple rows and clicks "Delete Selected"
$list = [1, 2, 3];
foreach ($list as $id) {
    $_profile->del($id); // Removes from database
}
```

---

#### Case: `"display"` / `"display_save"`

##### Sub-case: `"display_save"`

```php
$profile_data = $_profile->get($object);
$profile_data->code = $ifc_param1;
$profile_data->user = $ifc_param2;
...
$ifc_response = $_profile->set($profile_data) ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

**Purpose:**  
Saves updated profile data after form submission.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | int | Profile ID being edited |
| `$ifc_param1..28` | mixed | Form field values mapped to profile properties |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | Success/failure of `$_profile->set()` |

**Inner Mechanism:**  
Retrieves the profile by ID, maps form parameters to object properties, then saves via `$_profile->set()`.

**Usage Example:**
```php
// After editing a profile form
$profile_data = $_profile->get(123);
$profile_data->code = "USR001";
$profile_data->user = "john_doe";
$_profile->set($profile_data); // Persists changes
```

##### Sub-case: Default (`"display"`)

```php
if (blank($object) || (! $profile_data = $_profile->get($object))) ifc_close_external();
```

**Purpose:**  
Displays the profile edit form.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | int | Profile ID to display |

**Return Values:**
| Type | Description |
|------|-------------|
| `profile_data` | Retrieved profile data object |

**Inner Mechanism:**  
Fetches the profile; if not found, closes the window. Otherwise, renders the form using `ifc` methods.

**Usage Example:**
```php
// Viewing profile #45
$profile_data = $_profile->get(45);
// Renders form with fields populated from $profile_data
```

---

## Configuration Sections

### Case: `"config_base"`

**Purpose:**  
Renders a configuration form for base profile fields (e.g., code, user, password).

**Key Elements:**
- Uses `data("#system/profile")` to load configuration.
- Displays checkboxes for `visible`, `editable`, and `required` flags per field.
- Only accessible to operators (`$_profile->operator`).

**Usage Example:**
```php
// Operator configures which fields are visible in the profile form
$data = new data("#system/profile");
$data->get(CMS_DB_PROFILE_CODE, "visible"); // Returns TRUE/FALSE
```

---

### Case: `"_config_base"`

**Purpose:**  
Processes the submitted base configuration form.

**Inner Mechanism:**  
Maps `ifc_param1` through `ifc_param78` to corresponding `visible`, `editable`, and `required` settings for each profile field.

**Usage Example:**
```php
// Saving configuration changes
$data->set(isset($ifc_param1), CMS_DB_PROFILE_CODE, "visible");
$data->save(); // Persists to system data store
```

---

### Case: `"config_custom"`

**Purpose:**  
Renders a configuration form for custom profile fields (field1–field20).

**Key Elements:**
- Loops through 20 custom fields.
- Allows setting title, visibility, editability, and requirement status.

**Usage Example:**
```php
for ($i = 1; $i < 21; $i++) {
    $name = CMS_DB_PROFILE_CUSTOM_FIELD . $i;
    $data->get($name, "title"); // Gets custom field label
}
```

---

### Case: `"_config_custom"`

**Purpose:**  
Processes the submitted custom field configuration form.

**Inner Mechanism:**  
Maps `ifc_param1` through `ifc_param80` to titles and flags for each custom field.

**Usage Example:**
```php
for ($i = 1, $_i = 0; $i < 21; $i++) {
    $title = ${"ifc_param" . ++$_i};
    $visible = isset(${"ifc_param" . ++$_i});
    $data->set($title, $name, "title");
    $data->set($visible, $name, "visible");
}
```

---

## Reset Functionality

### Case: `"reset"`

```php
cms_cache_delete([
    "profile." . CMS_USER . ".filter_field",
    ...
]);
```

**Purpose:**  
Clears cached filter, group, page, limit, and order settings for the current user.

**Usage Example:**
```php
// User clicks "Reset Filters"
cms_cache_delete(["profile.user.filter_field", ...]);
```

---

## Main Display Logic

### Filter Options

```php
$filter_option_list = [
    0 => " LIKE '#value#%'",
    1 => " LIKE '%#value#%'",
    ...
];
```

**Purpose:**  
Defines SQL WHERE clause patterns for filtering profiles.

### Order Options

```php
$order_list = [
    0 => CMS_DB_PROFILE_INDEX,
    1 => CMS_DB_PROFILE_INDEX . " DESC",
    ...
];
```

**Purpose:**  
Defines sortable columns and directions.

### Cache Sync

```php
cms_cache_sync($filter_field, "profile." . CMS_USER . ".filter_field", CMS_DB_PROFILE_SURNAME);
```

**Purpose:**  
Synchronizes UI state with cached values.

### Menu Construction

```php
$menu = $_profile->operator ? [...] : NULL;
```

**Purpose:**  
Builds navigation menu based on operator privileges.

### Search Form

```php
$ifc->set([...], "select", $filter_field, NULL, "filter_field");
```

**Purpose:**  
Renders dropdowns for selecting filter field, condition, and value.

### Group By Selector

```php
$ifc->set(["" => " ", ...], "select", $group, NULL, "group");
```

**Purpose:**  
Allows grouping results by various fields.

### Limit Selector

```php
$ifc->set([10 => "10", ...], "select", $limit, NULL, "limit");
```

**Purpose:**  
Sets number of results per page.

### Query Execution

```php
$query = "SELECT ... FROM " . CMS_DB_PROFILE . " WHERE ... GROUP BY ... ORDER BY ...";
$result = mysql_query($query);
```

**Purpose:**  
Executes the constructed SQL query to retrieve profile data.

### Pagination

```php
pagination("javascript:p(%page%);", $page, $count, CMS_L_COMMAND_NEXT, "pagination");
```

**Purpose:**  
Renders pagination controls.

### Table Rendering

```php
ifc_table_open();
echo("<colgroup>...");
while ($resultrow = mysql_fetch_assoc($result)) {
    // Render row
}
ifc_table_close();
```

**Purpose:**  
Outputs an HTML table listing all matching profiles with selection checkboxes, sorting indicators, and action links.

---

## JavaScript Functions

### `r(value)`

```javascript
function r(value) {
    ifc_return(value);
}
```

**Purpose:**  
Returns a selected value to the parent window (used in external mode).

### `d(index)`

```javascript
function d(index) {
    load_page("...");
}
```

**Purpose:**  
Opens the profile detail view in a new page.

### `o(value)`

```javascript
function o(value) {
    ifc_set("order", value);
    ifc_post();
}
```

**Purpose:**  
Changes sort order and reloads the page.

### `p(number)`

```javascript
function p(number) {
    ifc_set("page", number);
    ifc_post();
}
```

**Purpose:**  
Navigates to a specific page of results.

---

## Summary

This interface file serves as the central hub for profile management in PWNC. It supports full CRUD operations, configurable field settings, and advanced search/filter capabilities. The design emphasizes modularity, security through permission checks, and seamless integration with the platform's caching and UI systems.


<!-- HASH:81b303c08bac69c81ec9b60577234b1d -->
