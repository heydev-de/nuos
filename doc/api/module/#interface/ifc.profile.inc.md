# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.profile.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Profile Interface Module (`ifc.profile.inc`)

This file implements the **Profile Management Interface** for the NUOS web platform. It provides a complete user interface for viewing, creating, editing, and configuring user profiles, including their personal data, contact information, financial details, and custom fields.

The interface supports:
- **CRUD operations** (Create, Read, Update, Delete) for user profiles
- **Search and filtering** with configurable fields, conditions, and sorting
- **Pagination** for large datasets
- **Configuration** of visible, editable, and required fields
- **Grouping** of results by specific fields
- **Custom field management** (20 configurable fields)
- **Operator permissions** for administrative actions

---

### Constants and Configuration

The following constants are used throughout the interface (defined in the core configuration):

| Name | Description |
|------|-------------|
| `CMS_DB_PROFILE_*` | Database field names for profile data (e.g., `CMS_DB_PROFILE_SURNAME`, `CMS_DB_PROFILE_EMAIL`) |
| `CMS_DB_PROFILE_CUSTOM_FIELD` | Prefix for custom field configuration (e.g., `CMS_DB_PROFILE_CUSTOM_FIELD1`) |
| `CMS_L_*` | Localized string constants for UI labels (e.g., `CMS_L_IFC_PROFILE_001` for "Index") |
| `CMS_IFC_*` | Interface control constants (e.g., `CMS_IFC_MESSAGE`, `CMS_IFC_PAGE`) |
| `CMS_PROFILE_PERMISSION_OPERATOR` | Permission level required for administrative actions |
| `CMS_L_ACCESS`, `CMS_L_OPERATOR` | Permission level constants |

---

### Initialization and Setup

The interface begins by:
1. Loading the **Profile** module (`cms_load("profile")`).
2. Checking user permissions (`ifc_permission`).
3. Instantiating the **`profile`** class (`$_profile = new profile()`).
4. Initializing default values for filtering, sorting, grouping, and pagination.

#### Key Initialization Variables

| Variable | Default Value | Description |
|----------|---------------|-------------|
| `$object` | `NULL` | ID of the profile being viewed/edited |
| `$sql_filter_field` | `CMS_DB_PROFILE_SURNAME` | Field used for filtering profiles |
| `$sql_filter_option` | `" LIKE '#value#%'"` | SQL condition for filtering (e.g., "starts with") |
| `$sql_filter_value` | `NULL` | Value to filter by (trimmed from `$_sql_filter_value`) |
| `$sql_order` | `CMS_DB_PROFILE_SURNAME` | Field used for sorting results |
| `$group` | `CMS_DB_PROFILE_COMPANY` | Field used for grouping results |
| `$page` | `0` | Current page for pagination |
| `$limit` | `25` | Number of profiles displayed per page |

---

### Message Handling and Sub-Displays

The interface processes different **messages** (actions) via a `switch` statement on `CMS_IFC_MESSAGE`. Each case handles a specific action:

| Message | Description | Typical Usage |
|---------|-------------|---------------|
| `"add"` | Creates a new profile | Triggered by the "Add" button |
| `"delete"` | Deletes selected profiles | Triggered by the "Delete Selected" button |
| `"display"` | Shows a profile in read-only mode | Triggered by clicking a profile in the list |
| `"display_save"` | Saves changes to a profile | Triggered by the "Save" button in the edit form |
| `"config_base"` | Configures base profile fields | Displays the base field configuration form |
| `"_config_base"` | Saves base field configuration | Processes the submitted configuration |
| `"config_custom"` | Configures custom profile fields | Displays the custom field configuration form |
| `"_config_custom"` | Saves custom field configuration | Processes the submitted configuration |
| `"sql_order"` | Changes the sorting field | Triggered by clicking a column header |
| `"page"` | Changes the current page | Triggered by pagination controls |

---

### Core Functions and Methods

#### `ifc_inactive($ifc_page)`
**Purpose**: Deactivates the interface and displays an inactive message.
**Parameters**:
- `$ifc_page` (string): The interface page identifier.
**Return Values**: None (terminates execution).
**Inner Mechanisms**: Calls the `ifc_inactive` function to halt interface rendering.
**Usage Context**: Used when the profile module is disabled or permissions are insufficient.

---

#### `ifc_permission($permissions)`
**Purpose**: Checks if the current user has the required permissions.
**Parameters**:
- `$permissions` (array): Associative array of permission levels (e.g., `["" => CMS_L_ACCESS, "operator" => CMS_L_OPERATOR]`).
**Return Values**: None (terminates execution if permissions are insufficient).
**Inner Mechanisms**: Uses the `permission_check` function to validate permissions.
**Usage Context**: Ensures users have the correct access level before proceeding.

---

#### `ifc_close_external()`
**Purpose**: Closes an external interface window (e.g., a profile display popup).
**Parameters**: None.
**Return Values**: None (terminates execution).
**Inner Mechanisms**: Outputs JavaScript to close the window.
**Usage Context**: Used when a profile does not exist or cannot be displayed.

---

#### `ifc_varied()`
**Purpose**: Generates alternating row styling for tables.
**Parameters**: None.
**Return Values**: (string) `" class=\"varied\""` or an empty string.
**Inner Mechanisms**: Toggles between styled and unstyled rows for better readability.
**Usage Context**: Applied to table rows in lists and forms.

---

### Profile Data Management

#### `profile->add($profile_data)`
**Purpose**: Adds a new profile to the database.
**Parameters**:
- `$profile_data` (`profile_data`): An instance of the `profile_data` class containing the new profile's data.
**Return Values**: (bool) `TRUE` on success, `FALSE` on failure.
**Inner Mechanisms**: Validates and inserts the profile data into the database.
**Usage Context**: Called in the `"add"` message handler.

---

#### `profile->del($id)`
**Purpose**: Deletes a profile by its ID.
**Parameters**:
- `$id` (string): The profile ID to delete.
**Return Values**: (bool) `TRUE` on success, `FALSE` on failure.
**Inner Mechanisms**: Removes the profile from the database.
**Usage Context**: Called in the `"delete"` message handler for each selected profile.

---

#### `profile->get($id)`
**Purpose**: Retrieves a profile by its ID.
**Parameters**:
- `$id` (string): The profile ID to retrieve.
**Return Values**: (`profile_data`) An instance of `profile_data` containing the profile's data, or `FALSE` if not found.
**Inner Mechanisms**: Queries the database for the profile and populates a `profile_data` object.
**Usage Context**: Used in the `"display"` and `"display_save"` message handlers.

---

#### `profile->set($profile_data)`
**Purpose**: Updates an existing profile.
**Parameters**:
- `$profile_data` (`profile_data`): An instance of `profile_data` containing the updated data.
**Return Values**: (bool) `TRUE` on success, `FALSE` on failure.
**Inner Mechanisms**: Validates and updates the profile data in the database.
**Usage Context**: Called in the `"display_save"` message handler to save changes.

---

### Interface Components

#### `ifc` Class
The `ifc` class is used to generate interface elements (forms, tables, buttons, etc.). Key methods include:

| Method | Purpose | Parameters |
|--------|---------|------------|
| `ifc->set($label, $type, $value, $checked, $name)` | Renders a form element | `$label`: Label text; `$type`: Element type (e.g., "text", "checkbox"); `$value`: Default value; `$checked`: For checkboxes; `$name`: Field name |
| `ifc->close()` | Finalizes the interface output | None |

#### `ifc_tab_open($title)`, `ifc_tab_next($title)`, `ifc_tab_close()`
**Purpose**: Creates tabbed interface sections.
**Parameters**:
- `$title` (string): The tab title.
**Usage Context**: Used to organize profile data into logical sections (e.g., "Base Data", "Contact Data").

#### `ifc_table_open($class)`, `ifc_table_close()`
**Purpose**: Opens and closes HTML tables.
**Parameters**:
- `$class` (string): Optional CSS class (e.g., `"min"` for compact tables).
**Usage Context**: Used to structure forms and lists.

---

### JavaScript Functions

The interface includes several JavaScript functions for dynamic behavior:

| Function | Purpose | Parameters |
|----------|---------|------------|
| `r(value)` | Returns a selected value to the parent window | `value`: The value to return |
| `d(index)` | Opens a profile in a new window for viewing/editing | `index`: The profile ID |
| `p(number)` | Navigates to a specific page in the pagination | `number`: The page number |

---

### Usage Scenarios

#### Viewing a Profile List
1. The interface loads and initializes default filtering, sorting, and pagination values.
2. The main display renders a search form, column headers, and a paginated list of profiles.
3. Users can:
   - Filter profiles by field and condition.
   - Sort by clicking column headers.
   - Group results by specific fields.
   - Navigate through pages using pagination controls.

#### Editing a Profile
1. The user clicks a profile in the list, triggering the `"display"` message.
2. The interface loads the profile data and renders an edit form.
3. The user modifies the data and clicks "Save", triggering the `"display_save"` message.
4. The interface validates and saves the changes, then refreshes the display.

#### Configuring Profile Fields
1. An operator clicks "Configuration" or "Extended Configuration", triggering the `"config_base"` or `"config_custom"` message.
2. The interface displays a form with checkboxes for each field, allowing the operator to set visibility, editability, and required status.
3. The operator submits the form, triggering the `"_config_base"` or `"_config_custom"` message.
4. The interface saves the configuration and updates the display.

#### Adding a Profile
1. An operator clicks "Add", triggering the `"add"` message.
2. The interface creates a new profile and opens it for editing.
3. The operator fills in the data and saves it, triggering the `"display_save"` message.

#### Deleting Profiles
1. The user selects one or more profiles using checkboxes.
2. The user clicks "Delete Selected", triggering the `"delete"` message.
3. The interface deletes the selected profiles and refreshes the list.

---

### Security Considerations
- **Permissions**: All administrative actions (add, delete, configure) require operator permissions.
- **CSRF Protection**: The `cms_param` and `cms_url` functions include CSRF protection.
- **Input Validation**: All user inputs are escaped using `sqlesc`, `q`, `r`, and `x` functions to prevent SQL injection and XSS attacks.
- **Password Generation**: The "Generate Password" button uses `unique_id(8)` to create a secure random password.

---

### Localization
All UI labels are localized using the `l()` function, which retrieves translated strings from the language files. Constants like `CMS_L_IFC_PROFILE_001` map to these strings.

---

### Error Handling
- The interface uses `CMS_MSG_DONE` and `CMS_MSG_ERROR` to indicate success or failure of operations.
- Errors during profile operations (e.g., add, delete, save) are captured and displayed to the user.
- If a profile does not exist, the interface closes the display window or shows an appropriate message.


<!-- HASH:daac405287843280d4ea7e3a65c27155 -->
