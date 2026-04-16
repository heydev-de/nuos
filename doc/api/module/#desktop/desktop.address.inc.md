# NUOS API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.address.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Address Module (`desktop.address.inc`)

**Overview**
This file implements the address book interface for the NUOS desktop environment. It provides functionality for managing contact information (addresses, phone numbers, emails, etc.) through CRUD operations (Create, Read, Update, Delete) and integrates with the mailbox system for direct email composition. The module handles both the display of contact lists (grouped alphabetically) and individual contact details, with support for selection callbacks in interactive workflows.

---

### Core Interface Handling

#### Message Dispatch (`CMS_IFC_MESSAGE` Switch)
The module processes interface commands through a message-based system. Each case handles a distinct operation:

| Message Type | Purpose                                                                 | Parameters                     | Response Handling                     |
|--------------|-------------------------------------------------------------------------|--------------------------------|----------------------------------------|
| `select`     | Loads an existing address object for viewing/editing.                   | `$ifc_param`: Object ID        | Sets `$object`; clears `$initial`.    |
| `initial`    | Filters the address list by the first letter of contact names.          | `$ifc_param`: Letter or `-`    | Sets `$initial`; clears `$object`.    |
| `add`        | Creates a new address entry.                                            | `$ifc_param`: Company name     | Sets `$object` on success; error otherwise. |
| `save`       | Updates an existing address with form data.                             | `$ifc_param1-7`: Field values  | Clears `$initial` on success.          |
| `delete`     | Removes an address object.                                              | None                           | Clears `$object` on success.           |

**Inner Mechanisms:**
- **`add`**: Validates the existence of an address subtype in the desktop data structure before insertion. Uses `$desktop->data->set_buffer()` to stage the new record.
- **`save`**: Updates all fields of the address object via `$desktop->object_set()` and persists changes with `$desktop->save()`.
- **Error Handling**: Sets `$ifc_response` to `CMS_MSG_ERROR` or `CMS_MSG_DONE` to signal operation status to the frontend.

**Usage Context:**
- Triggered by user actions (e.g., clicking "Add" or "Save" buttons).
- Used in workflows requiring contact selection (e.g., email composition).

---

### Display Logic

#### Initial Letter Filtering
**Purpose:**
Generates an alphabetical index for the address list and counts contacts per letter.

**Mechanisms:**
1. **Initial Calculation**: Derives the initial letter from the `company` or `name` field of the current object (or defaults to `-` for non-alphabetic characters).
2. **Counting**: Iterates through all address objects, categorizing them by their initial letter and populating `$initial_count`.

**Output:**
- Renders a vertical menu of letters (A-Z) with contact counts.
- Highlights the active filter letter.

---

#### Address List Rendering
**Purpose:**
Displays contacts grouped by company, with support for filtering by initial letter.

**Data Structure:**
- `$array`: Nested associative array (`$company[$name][] = $object_id`) for grouped display.

**Mechanisms:**
1. **Grouping**: Contacts are sorted alphabetically by company and name.
2. **Varied Rows**: Uses `ifc_varied()` to alternate row colors for readability.
3. **Mailbox Integration**: Renders email links if a mailbox object exists (`$mailbox_user` and `$mailbox_object` are set).

**Output:**
- Table with columns for **Name**, **Phone**, and **Email**.
- "Add" buttons for each company group.
- Clickable rows to select a contact.

---

#### Contact Details Form
**Purpose:**
Renders an editable form for the currently selected contact.

**Fields:**
| Field       | Label Constant               | Input Type          | Max Length | Notes                     |
|-------------|------------------------------|---------------------|------------|----------------------------|
| `name`      | `CMS_L_NAME`                 | Text                | 40         |                            |
| `company`   | `CMS_L_DESKTOP_ADDRESS_003`  | Text (bold)         | 80         |                            |
| `phone`     | `CMS_L_DESKTOP_ADDRESS_004`  | Text (bold)         | 40         |                            |
| `fax`       | `CMS_L_DESKTOP_ADDRESS_005`  | Text (bold)         | 80         |                            |
| `email`     | `CMS_L_EMAIL`                | Text (bold)         | 80         |                            |
| `address`   | `CMS_L_DESKTOP_ADDRESS_001`  | Textarea (4 rows)   | 256        |                            |
| `comment`   | `CMS_L_DESKTOP_ADDRESS_002`  | Textarea (4 rows)   | 256        |                            |

**Mechanisms:**
- Uses the `ifc` class to generate form inputs with validation rules.
- Fields are rendered in a table layout with bold labels for key information.

---

### Menu System
**Purpose:**
Dynamically generates action buttons based on context.

**Menu Items:**
| Condition               | Item                          | Action                                                                 |
|-------------------------|-------------------------------|------------------------------------------------------------------------|
| Always                  | `CMS_L_COMMAND_ADD`           | Triggers `add` message.                                                |
| `$object` exists        | `CMS_L_COMMAND_INSERT`        | Returns a formatted value (e.g., `"Name" <email>`) to the caller.      |
| `$object` exists        | `CMS_L_COMMAND_SAVE`          | Triggers `save` message.                                               |
| `$object` exists        | `CMS_L_COMMAND_DELETE`        | Triggers `delete` message (requires confirmation).                     |

**Return Value Handling:**
- For `CMS_IFC_SELECT = "email"`, formats the contact as `"Name" <email>`.
- Other selections return the contact name.

---

### Key Utility Functions
| Function/Method         | Purpose                                                                 |
|-------------------------|-------------------------------------------------------------------------|
| `$desktop->data->seek()`| Locates the address subtype in the desktop data structure.              |
| `$desktop->object_get()`| Retrieves a field value from an object.                                 |
| `$desktop->object_set()`| Updates a field value in an object.                                     |
| `$desktop->save()`      | Persists changes to the desktop data.                                   |
| `$desktop->delete_object()`| Removes an object from the desktop.                                 |
| `qrx()`                 | Escapes strings for JavaScript/URL contexts (combines `q()` and `r()`).|
| `x()`                   | Escapes strings for XML/HTML output.                                    |
| `ifc_varied()`          | Generates alternating row classes (`class="varied"`).                  |

---

### Integration Points
1. **Mailbox System**:
   - Detects mailbox objects during address list iteration.
   - Generates email composition links using `cms_url()` with parameters:
     ```php
     ["desktop_display" => "interface", "user" => $mailbox_user, "object" => $mailbox_object, "ifc_message" => "mail", "to" => $email]
     ```

2. **Selection Callbacks**:
   - Supports `CMS_IFC_SELECT` to return values to parent interfaces (e.g., email recipient selection).

3. **Desktop Data Structure**:
   - Relies on the desktop’s hierarchical data model (`$desktop->data`) to store and retrieve address objects.

---

### Typical Usage Scenarios
1. **Contact Management**:
   - Users browse, add, edit, or delete contacts via the desktop interface.
   - Contacts are grouped by company for efficient navigation.

2. **Email Composition**:
   - Users select a contact from the address book to populate the "To" field in an email.

3. **Data Export**:
   - The `CMS_IFC_SELECT` mechanism allows other modules to retrieve contact details (e.g., for invoicing or reports).

4. **Alphabetical Filtering**:
   - Users click on letters (A-Z) or "…" to filter contacts by initial letter.


<!-- HASH:b3e5502c6272b07db4d936581d458f90 -->
