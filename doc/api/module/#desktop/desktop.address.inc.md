# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.address.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.address.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Address Management Interface

This file implements the desktop address book interface for the PWNC Web Platform. It handles the display, creation, editing, and deletion of address entries within the desktop environment. The interface provides an alphabetical navigation system, a list of addresses grouped by company, and a detail form for individual address management.

### Message Handling Switch

The main logic is driven by a switch statement on `CMS_IFC_MESSAGE`, which determines the current operation to perform.

#### `select` Case

Sets the currently selected address object and initializes the initial letter filter.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | The object key to select |

**Usage Example:**
```php
// Selects an address object for editing
CMS_IFC_MESSAGE = "select";
$ifc_param = "address_123";
```

#### `initial` Case

Sets the initial letter filter for the address list display.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | The initial letter to filter by |

**Usage Example:**
```php
// Filters addresses starting with 'A'
CMS_IFC_MESSAGE = "initial";
$ifc_param = "A";
```

#### `address` Case

Attempts to find an existing address matching the provided email. If not found, falls through to the `add` case.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$email` | string | Email address to search for |
| `$group` | string | Company/group name |
| `$name` | string | Contact name |

**Usage Example:**
```php
// Finds or prepares to add an address
CMS_IFC_MESSAGE = "address";
$email = "john@example.com";
$group = "Acme Corp";
$name = "John Doe";
```

#### `add` Case

Creates a new address entry in the desktop data store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Company/group name |
| `$email` | string | Email address |
| `$name` | string | Contact name |
| `$initial` | string | Initial letter filter |

**Inner Mechanisms:**
1. Searches for an existing address subtype key
2. Sets buffer data with name, type, company, and email
3. Inserts the new object and saves the desktop state
4. Returns success or error response

**Usage Example:**
```php
// Adds a new address entry
CMS_IFC_MESSAGE = "add";
$ifc_param = "Acme Corp";
$email = "jane@example.com";
$name = "Jane Smith";
```

#### `save` Case

Updates an existing address object with new field values.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | Object key to update |
| `$ifc_param1` | string | Name |
| `$ifc_param2` | string | Company |
| `$ifc_param3` | string | Phone |
| `$ifc_param4` | string | Fax |
| `$ifc_param5` | string | Email |
| `$ifc_param6` | string | Address |
| `$ifc_param7` | string | Comment |

**Inner Mechanisms:**
1. Sets each field on the object using `object_set()`
2. Saves the desktop state
3. Returns success or error response

**Usage Example:**
```php
// Saves updated address information
CMS_IFC_MESSAGE = "save";
$object = "address_123";
$ifc_param1 = "Updated Name";
$ifc_param2 = "New Company";
// ... other parameters
```

#### `delete` Case

Removes an address object from the desktop data store.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | Object key to delete |

**Inner Mechanisms:**
1. Calls `delete_object()` on the desktop instance
2. Clears the current object reference
3. Returns success or error response

**Usage Example:**
```php
// Deletes an address entry
CMS_IFC_MESSAGE = "delete";
$object = "address_123";
```

### Main Display Logic

After processing the message, the interface renders the main display with alphabetical navigation, address listing, and detail form.

#### Initial Letter Calculation

When no initial is set, calculates it from the selected object's company and name fields.

**Inner Mechanisms:**
1. Concatenates company and name values
2. Takes the first character and converts to uppercase
3. Validates it's an alphabetic character, defaults to "-" if not

#### Address List Building

Iterates through all desktop data objects to build the address list.

**Inner Mechanisms:**
1. Moves through data objects sequentially
2. For "address" type objects:
   - Extracts name and company
   - Calculates initial letter for grouping
   - Counts entries per initial letter
   - Groups entries by company and name under the current initial
3. For "mailbox" type objects:
   - Records mailbox user and object references

#### Menu Construction

Builds contextual menu items based on the current state.

**Inner Mechanisms:**
1. Always includes an "Add" command
2. When an object is selected:
   - Adds an "Insert" command if `CMS_IFC_SELECT` is set, formatting the value appropriately
   - Adds "Save" and "Delete" commands

#### Interface Rendering

Creates the main interface with three columns:
1. **Alphabetical Navigation**: Letters A-Z plus "…" for non-alphabetic entries
2. **Address List**: Grouped by company with contact names and emails
3. **Detail Form**: Editable fields for the selected address (when applicable)

**Usage Example:**
```php
// The interface automatically renders when included
// with appropriate CMS_IFC_MESSAGE and $desktop context
include 'module/#desktop/desktop.address.inc';
```

#### Mailbox Integration

When a mailbox object exists, injects JavaScript for sending emails directly from the address interface.

**Inner Mechanisms:**
1. Defines a `m(value)` JavaScript function
2. Constructs a URL using `cms_url()` with mailbox parameters
3. Loads the mail composition page with the recipient pre-filled


<!-- HASH:9937d56a11eee428fcfd8db21f01570f -->
