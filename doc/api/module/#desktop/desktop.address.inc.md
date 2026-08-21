# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.address.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.address.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Address Interface Module

This module provides the address book functionality for the PWNC Web Platform's desktop interface. It handles the display, creation, modification, and deletion of address book entries, as well as integration with the mailbox system for sending emails directly from the address book.

### Overview

The `desktop.address.inc` file processes interface messages (`CMS_IFC_MESSAGE`) to manage address book operations and renders the address book UI. It supports:
- **Selection** of existing addresses
- **Creation** of new addresses
- **Modification** of address details
- **Deletion** of addresses
- **Navigation** via an alphabetical index
- **Integration** with the mailbox system for direct email composition

---

## Interface Message Handling

The module processes interface messages to perform CRUD operations on address book entries. The following table outlines the supported messages and their parameters:

| Message  | Parameters (`$ifc_param`, `$ifc_param1`–`$ifc_param7`) | Description                                                                 |
|----------|--------------------------------------------------------|-----------------------------------------------------------------------------|
| `select` | `$ifc_param`: Address object ID                        | Loads an existing address for viewing or editing.                          |
| `initial`| `$ifc_param`: Initial character (A-Z, `-`)             | Filters the address list by the specified initial character.               |
| `address`| `$ifc_param`: Group/company name                       | Searches for an address by email. If not found, prepares to add a new one. |
| `add`    | `$ifc_param`: Group/company name                       | Adds a new address entry.                                                  |
| `save`   | `$ifc_param1`: Name<br>`$ifc_param2`: Company<br>`$ifc_param3`: Phone<br>`$ifc_param4`: Fax<br>`$ifc_param5`: Email<br>`$ifc_param6`: Address<br>`$ifc_param7`: Comment | Saves changes to an existing address.                                      |
| `delete` | None                                                   | Deletes the currently selected address.                                    |

---

### Message Handling Logic

#### `case "select"`
Loads an existing address for display or editing.
- **Parameters**:
  - `$ifc_param`: The ID of the address object to load.
- **Mechanism**:
  - Sets `$object` to the address object ID.
  - Clears `$initial` (used for alphabetical filtering).
- **Usage**:
  ```php
  // Example: Load address with ID 123
  $ifc_message = "select";
  $ifc_param = 123;
  include "module/#desktop/desktop.address.inc";
  ```

---

#### `case "initial"`
Filters the address list by the specified initial character.
- **Parameters**:
  - `$ifc_param`: The initial character (A-Z) or `-` for non-alphabetical entries.
- **Mechanism**:
  - Sets `$initial` to the provided character.
  - Clears `$object` to ensure no address is selected.
- **Usage**:
  ```php
  // Example: Show addresses starting with "M"
  $ifc_message = "initial";
  $ifc_param = "M";
  include "module/#desktop/desktop.address.inc";
  ```

---

#### `case "address"`
Searches for an address by email. If not found, prepares to add a new address.
- **Parameters**:
  - `$email`: The email address to search for.
  - `$ifc_param`: The group/company name (used if the address is not found).
  - `$name`: The name of the contact (optional).
- **Mechanism**:
  - Attempts to find an existing address with the provided email.
  - If not found, falls through to the `add` case, pre-filling the group/company name and email.
- **Usage**:
  ```php
  // Example: Search for an address with email "contact@example.com"
  $ifc_message = "address";
  $email = "contact@example.com";
  $ifc_param = "Example Corp";
  include "module/#desktop/desktop.address.inc";
  ```

---

#### `case "add"`
Adds a new address entry to the address book.
- **Parameters**:
  - `$ifc_param`: The group/company name.
  - `$name`: The name of the contact (optional).
  - `$email`: The email address (optional).
- **Mechanism**:
  - Checks if a parent group (subtype `address`) exists. If not, the operation fails.
  - Sets default values for the new address (e.g., name, company, email).
  - Inserts the new address into the database and saves the changes.
  - On success, sets `$ifc_response` to `CMS_MSG_DONE`; otherwise, sets it to `CMS_MSG_ERROR`.
- **Usage**:
  ```php
  // Example: Add a new address
  $ifc_message = "add";
  $ifc_param = "Example Corp";
  $name = "John Doe";
  $email = "john.doe@example.com";
  include "module/#desktop/desktop.address.inc";
  ```

---

#### `case "save"`
Saves changes to an existing address.
- **Parameters**:
  - `$ifc_param1`: Name
  - `$ifc_param2`: Company
  - `$ifc_param3`: Phone
  - `$ifc_param4`: Fax
  - `$ifc_param5`: Email
  - `$ifc_param6`: Address
  - `$ifc_param7`: Comment
- **Mechanism**:
  - Updates the address object with the provided values.
  - Saves the changes to the database.
  - On success, sets `$ifc_response` to `CMS_MSG_DONE`; otherwise, sets it to `CMS_MSG_ERROR`.
- **Usage**:
  ```php
  // Example: Save changes to an address
  $ifc_message = "save";
  $ifc_param1 = "Jane Doe";
  $ifc_param2 = "Example Corp";
  $ifc_param3 = "+1234567890";
  $ifc_param4 = "+1234567891";
  $ifc_param5 = "jane.doe@example.com";
  $ifc_param6 = "123 Example St, City";
  $ifc_param7 = "Updated contact details";
  include "module/#desktop/desktop.address.inc";
  ```

---

#### `case "delete"`
Deletes the currently selected address.
- **Mechanism**:
  - Deletes the address object from the database.
  - On success, clears `$object` and sets `$ifc_response` to `CMS_MSG_DONE`; otherwise, sets it to `CMS_MSG_ERROR`.
- **Usage**:
  ```php
  // Example: Delete the currently selected address
  $ifc_message = "delete";
  include "module/#desktop/desktop.address.inc";
  ```

---

## Main Display Logic

The module renders the address book UI, which consists of:
1. An **alphabetical index** for navigation.
2. A **list of addresses** grouped by company.
3. A **detail view** for the selected address (if any).

### Key Variables

| Variable            | Type     | Description                                                                 |
|---------------------|----------|-----------------------------------------------------------------------------|
| `$initial`          | string   | The currently selected initial character (A-Z or `-`).                     |
| `$array`            | array    | A nested array of addresses, grouped by company and name.                  |
| `$initial_count`    | array    | Counts of addresses per initial character.                                 |
| `$mailbox_user`     | string   | The user associated with the mailbox (if mailbox integration is available).|
| `$mailbox_object`   | string   | The mailbox object ID (if mailbox integration is available).               |

---

### Alphabetical Index Rendering

The alphabetical index is rendered as a vertical list of links (A-Z and `…` for non-alphabetical entries). Each link triggers the `initial` message to filter the address list.

- **Mechanism**:
  - Iterates over characters A-Z and `-`.
  - Highlights the current initial character (`$initial`).
  - Displays the count of addresses for each initial character.
- **Example Output**:
  ```html
  <tr>
    <td class="enabled"><a href="javascript:ifc_post('initial','M');"><strong>M</strong> (5)</a></td>
  </tr>
  <tr>
    <td><a href="javascript:ifc_post('initial','N');">N</a></td>
  </tr>
  ```

---

### Address List Rendering

The address list is rendered as a table with columns for **Name**, **Phone**, and **Email**. Addresses are grouped by company, and each group can be expanded to show its contacts.

- **Mechanism**:
  - Groups addresses by company and name using a nested array (`$array`).
  - Sorts companies and names using natural case-insensitive sorting.
  - Displays an "Add" link for each company group.
  - Renders email addresses as clickable links if mailbox integration is available.
- **Example Output**:
  ```html
  <tr>
    <th colspan="3">Example Corp</th>
  </tr>
  <tr>
    <td class="highlight" colspan="3">
      <a href="javascript:ifc_post('add','Example Corp');">
        <img src="desktop/command_create" /> Add
      </a>
    </td>
  </tr>
  <tr>
    <td class="varied">
      <a href="javascript:ifc_post('select','123');">
        <img src="desktop/icon_address" /> John Doe
      </a>
    </td>
    <td class="varied">+1234567890</td>
    <td class="varied">
      <a href="javascript:m('john.doe@example.com');">
        <img src="desktop/icon_mailbox" /> john.doe@example.com
      </a>
    </td>
  </tr>
  ```

---

### Address Detail View

If an address is selected (`$object` is set), its details are displayed in a form for editing.

- **Mechanism**:
  - Uses the `ifc` class to render form fields for address details (name, company, phone, fax, email, address, and comment).
  - Fields are pre-filled with the current values of the address object.
- **Example Output**:
  ```html
  <tr>
    <th>Name</th>
  </tr>
  <tr>
    <td class="varied">
      <input type="text" name="name" value="John Doe" />
      <input type="text" name="company" value="Example Corp" />
      <input type="text" name="phone" value="+1234567890" />
      <input type="text" name="email" value="john.doe@example.com" />
      <textarea name="address">123 Example St, City</textarea>
      <textarea name="comment">Primary contact</textarea>
    </td>
  </tr>
  ```

---

### Mailbox Integration

If a mailbox is available (`$mailbox_user` and `$mailbox_object` are set), the module injects JavaScript to enable direct email composition from the address book.

- **Mechanism**:
  - Defines a JavaScript function `m(value)` that loads the mailbox interface with the selected email address pre-filled.
  - Email addresses in the address list are rendered as clickable links that call this function.
- **Example JavaScript**:
  ```javascript
  function m(value) {
    load_page("index.php?desktop_display=interface&user=current_user&object=mailbox123&ifc_message=mail&to="+encodeURIComponent(value));
  }
  ```

---

## Usage Example

### Scenario: Adding a New Address

1. **Trigger the `add` message** to open the address creation form.
2. **Fill in the details** and submit the form to trigger the `save` message.

```php
// Step 1: Open the address creation form
$ifc_message = "add";
$ifc_param = "Example Corp";
include "module/#desktop/desktop.address.inc";

// Step 2: Save the new address (typically handled via form submission)
$ifc_message = "save";
$ifc_param1 = "John Doe";       // Name
$ifc_param2 = "Example Corp";   // Company
$ifc_param3 = "+1234567890";    // Phone
$ifc_param4 = "+1234567891";    // Fax
$ifc_param5 = "john@example.com"; // Email
$ifc_param6 = "123 Example St"; // Address
$ifc_param7 = "Primary contact"; // Comment
include "module/#desktop/desktop.address.inc";
```

### Scenario: Filtering Addresses by Initial

```php
// Show addresses starting with "S"
$ifc_message = "initial";
$ifc_param = "S";
include "module/#desktop/desktop.address.inc";
```

### Scenario: Sending an Email from the Address Book

1. **Ensure mailbox integration is available** (i.e., `$mailbox_user` and `$mailbox_object` are set).
2. **Click an email address** in the address list to open the mailbox interface with the recipient pre-filled.

```javascript
// Example: Clicking an email link triggers this function
m("john.doe@example.com");
```


<!-- HASH:9937d56a11eee428fcfd8db21f01570f -->
