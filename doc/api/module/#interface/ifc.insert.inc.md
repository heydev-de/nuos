# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.insert.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.insert.inc)

- **Version:** `26.8.7.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Interface: Insert Management (`ifc.insert.inc`)

This file implements the **Insert Management Interface** for the PWNC Web Platform. It provides a user interface for creating, editing, deleting, and organizing reusable code snippets (inserts) that can be embedded in content. These inserts are stored in a hierarchical structure and can be assigned to specific objects (e.g., pages, modules) for quick access.

The interface supports:
- **Tree-based navigation** of insert containers and snippets
- **Code editing** (plain text or HTML)
- **Preview functionality**
- **Assignment** of inserts to objects
- **CRUD operations** (Create, Read, Update, Delete)

---

## Core Workflow

The interface operates in two main modes:
1. **Main Display** (`CMS_IFC_MESSAGE` not set or `select`, `add`, `delete`, `save`)
   - Shows a tree of all inserts and containers
   - Allows selection, addition, and deletion of containers
   - Manages assignment of inserts to objects

2. **Insert Editor** (`CMS_IFC_MESSAGE` = `insert`, `insert_select`, `insert_display`, `insert_add`, `insert_save`, `insert_delete`)
   - Edits individual code snippets
   - Supports both plain text and HTML input
   - Provides live preview

---

## Key Components

### 1. Initialization and State Management

```php
// Load required libraries
if (!cms_load("flexview")) ifc_inactive($ifc_page);

// Check permissions
ifc_permission(["" => CMS_L_ACCESS]);

// Retrieve selected object from cache
cms_cache_init($object, "insert." . CMS_USER . ".object");

// Initialize variables
$list ??= isset($object) ? [$object] : [];
```

| Variable | Type | Description |
|----------|------|-------------|
| `$object` | `string` | Currently selected object (container or insert) |
| `$list` | `array` | List of selected objects (for multi-select operations) |
| `$insert_object` | `string` | Currently selected insert (code snippet) |

---

### 2. Message Handling

The interface uses `CMS_IFC_MESSAGE` to determine the current action. The following table summarizes the supported messages:

| Message | Purpose |
|---------|---------|
| `insert` | Open insert editor for currently assigned insert |
| `insert_select` | Select a specific insert for editing |
| `insert_display` | Preview the selected insert |
| `insert_add` | Add a new insert |
| `insert_save` | Save changes to an insert |
| `insert_delete` | Delete selected inserts |
| `select` | Select an object in the main tree |
| `_add` | Internal handler for adding new containers |
| `add` | Add a new container |
| `delete` | Delete selected containers/objects |
| `save` | Save insert assignment to an object |

---

## Functionality Breakdown

### `insert` Message Handler

Handles all insert-related operations (editing, previewing, CRUD).

#### Data Initialization
```php
$data = new data("#system/insert.code");
```
- Loads the data store for code inserts from `#system/insert.code`
- Uses the `data` class for structured storage and retrieval

#### Key Sub-Operations

##### `insert_select`
```php
$insert_object = $ifc_param;
```
- **Purpose**: Selects an insert for editing
- **Parameter**: `$ifc_param` - The ID of the insert to select
- **Effect**: Sets `$insert_object` to the specified ID

##### `insert_display`
```php
$code = l($data->get($insert_object, "code"));
preview($data->get($insert_object, "html") ? $code : parse_text($code));
exit();
```
- **Purpose**: Renders a preview of the selected insert
- **Mechanism**:
  - Retrieves the code from the data store
  - If the insert is marked as HTML (`html=1`), renders it directly
  - Otherwise, processes it through `parse_text()` for plain text formatting
  - Uses `preview()` to display the result in a modal/preview window
  - Terminates execution to prevent further interface rendering

##### `insert_add`
```php
$data->set_buffer([["name" => CMS_L_IFC_INSERT_002]]);
$_insert_object = $data->append();
if ($data->save()) {
    $insert_object = $_insert_object;
    $ifc_response = CMS_MSG_DONE;
    break;
};
$ifc_response = CMS_MSG_ERROR;
```
- **Purpose**: Creates a new insert with a default name
- **Mechanism**:
  - Sets a buffer with a default name (localized string `CMS_L_IFC_INSERT_002`)
  - Appends the new entry to the data store
  - Saves the data store
  - On success, sets `$insert_object` to the new ID and returns success
  - On failure, returns an error

##### `insert_save`
```php
if (stre($ifc_param1 = utf8_trim($ifc_param1)))
    $ifc_param1 = $data->get($insert_object, "name");
$html = (int)$ifc_tab_1 === 2;
$data->set([
    "name" => $ifc_param1,
    "html" => $html,
    "code" => $html ? $ifc_param3 : $ifc_param2
], $insert_object);
$ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```
- **Purpose**: Saves changes to an insert
- **Parameters**:
  - `$ifc_param1`: New name for the insert (falls back to existing name if empty)
  - `$ifc_tab_1`: Determines if the content is HTML (tab 2) or plain text (tab 1)
  - `$ifc_param2`: Plain text content
  - `$ifc_param3`: HTML content
- **Mechanism**:
  - Preserves the existing name if no new name is provided
  - Determines content type based on active tab
  - Updates the data store with new values
  - Saves changes and returns success/error

##### `insert_delete`
```php
if (blank($_list)) break; //no selection
foreach ($_list AS $value) $data->del($value);
if ($data->save()) {
    // Clean up references in #system/insert
    $data_insert = new data("#system/insert");
    $data_insert->move("first");
    while ($key = $data_insert->move("next")) {
        $_key = $data_insert->get($key, "insert");
        if ($data->get($_key) === NULL) $data_insert->del($key, "insert");
    };
    $data_insert->save();
    // Deselect if deleted
    if (in_array($insert_object, $_list)) $insert_object = "";
    $ifc_response = CMS_MSG_DONE;
    break;
};
$ifc_response = CMS_MSG_ERROR;
```
- **Purpose**: Deletes selected inserts and cleans up references
- **Parameters**:
  - `$_list`: Array of insert IDs to delete
- **Mechanism**:
  - Skips if no inserts are selected
  - Deletes each insert from the data store
  - Cleans up references in the main insert data store (`#system/insert`)
  - Deselects the insert if it was deleted
  - Returns success/error

---

### Main Display Handler

Handles the primary interface for navigating and managing the insert hierarchy.

#### Data Initialization
```php
$data = new data("#system/insert");
if ($data->get($object) === NULL) $object = ""; // Check selected object
// Cache selected object permanently
if (nstre(CMS_IFC_MESSAGE)) cms_cache("insert." . CMS_USER . ".object", $object, TRUE);
```
- Loads the main insert data store
- Validates the selected object
- Caches the selected object for persistence across sessions

#### Key Sub-Operations

##### `select`
```php
$object = $ifc_param;
$list = [$object];
```
- **Purpose**: Selects an object in the tree
- **Parameter**: `$ifc_param` - The ID of the object to select
- **Effect**: Updates `$object` and `$list` with the new selection

##### `_add` (Internal)
```php
if (nstre($ifc_param1 = utf8_trim($ifc_param1))) {
    $data = new data("#system/insert");
    $array = explode(".", "$object.$ifc_param1");
    $value = NULL;
    foreach ($array AS $_value) {
        if (stre($_value)) continue;
        $__value = $value;
        if ($value === NULL) $value = $_value;
        else $value .= ".$_value";
        if ($data->get($value) === NULL) {
            $data->buffer = [$value => ["name" => $_value, "#type" => "container"],
                            "/$value" => ["#type" => "/container"]];
            $data->insert($__value);
        };
    };
    if ($data->save()) {
        $object = $value;
        $list = [$value];
        $ifc_response = CMS_MSG_DONE;
        break;
    };
};
$ifc_response = CMS_MSG_ERROR;
```
- **Purpose**: Creates a new container in the insert hierarchy
- **Parameter**: `$ifc_param1` - The name/path of the new container
- **Mechanism**:
  - Splits the path by dots (e.g., "parent.child")
  - Creates each segment of the path if it doesn't exist
  - Uses the `data` class's buffer and insert methods to create containers
  - On success, selects the new container and returns success
  - On failure, returns an error

##### `add`
```php
$ifc = new ifc($ifc_response, $ifc_page, TRUE, ["object" => $object, "list" => $list], "_add", CMS_L_COMMAND_ADD);
$ifc->set(CMS_L_NAME, "text 40 256 b");
$ifc->close();
```
- **Purpose**: Displays a form to add a new container
- **Mechanism**:
  - Creates an `ifc` (Interface Control) object
  - Adds a text field for the container name
  - Submits to the `_add` handler

##### `delete`
```php
if (count($list) === 0) break; //no selection;
$data = new data("#system/insert");
$stack = [];
$_object = $object;
// Get path to object
while (($_object = $data->move("parent", $_object)) !== FALSE)
    array_unshift($stack, $_object);
array_unshift($stack, "");
// Delete
foreach ($list AS $value) $data->del($value);
if ($data->save()) {
    // Select last existing object in path
    while ($data->get($object) === NULL) {
        $object = array_pop($stack);
        if (stre($object)) break;
    };
    $list = [$object];
    $ifc_response = CMS_MSG_DONE;
    break;
};
$ifc_response = CMS_MSG_ERROR;
```
- **Purpose**: Deletes selected containers/objects
- **Mechanism**:
  - Skips if nothing is selected
  - Builds the path to the selected object
  - Deletes each selected object
  - On success, selects the nearest existing parent object
  - Returns success/error

##### `save`
```php
$data = new data("#system/insert");
$data->set($insert_object, $object, "insert");
$ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```
- **Purpose**: Saves the assignment of an insert to an object
- **Mechanism**:
  - Sets the `insert` property of the object to the selected insert ID
  - Saves the data store
  - Returns success/error

---

## Interface Components

### Insert List Table

Displays a list of available inserts with selection controls.

```php
ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .
     // Header
     "<tr>" .
     "<td class=\"select\"></td>" .
     "<th>" . CMS_L_IFC_INSERT_004 . "</th>" .
     "</tr>");
data_sort($data, "name");
$data->move("first");
while ($key = $data->move("next")) {
    $name = x($data->get($key, "name"));
    if (streq($key, $insert_object)) $name = "<strong>$name</strong>";
    $varied = ifc_varied();
    echo("<tr>" .
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $key, streq($key, $insert_object), "_list[]");
    echo("</td>" .
         "<td$varied>" .
         "<a href=\"javascript:ifc_post('insert_select','" . qx($key) . "');\">" .
         image("insert/icon_insert") . " $name" .
         "</a>" .
         "</td>" .
         "</tr>");
};
// Selection controls
echo("<tr>" .
     "<td colspan=\"2\" class=\"select\" style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL, "button", "javascript:ifc_list_activate(\"_list\");");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert(\"_list\");");
$ifc->set(CMS_L_NONE, "button", "javascript:ifc_list_deactivate(\"_list\");");
echo("</td>" .
     "</tr>");
ifc_table_close();
```

| Component | Purpose |
|-----------|---------|
| `ifc_table_open()`/`ifc_table_close()` | Wraps content in a styled table |
| `data_sort($data, "name")` | Sorts inserts by name |
| `$ifc->set(NULL, "checkbox", ...)` | Creates selection checkboxes |
| Selection controls | Buttons to select all, invert selection, or clear selection |

---

### Insert Editor

Provides a form for editing insert properties and content.

```php
ifc_table_open();
// Header
echo("<tr>" .
     "<th>" . CMS_L_IFC_INSERT_005 . "</th>" .
     "</tr>" .
     // Settings
     "<tr>" .
     "<td>");
// Name field
$ifc->set(CMS_L_NAME, "text 40 80", $data->get($insert_object, "name"));
// Display button
$ifc->set(CMS_L_COMMAND_SHOW, "button",
          "javascript:load_page('" . q(cms_url(["ifc_page" => CMS_IFC_PAGE,
                                              "ifc_message" => "insert_display",
                                              "insert_object" => $insert_object])) . "');");
// Save button
$ifc->set(CMS_L_COMMAND_SAVE, "button b", "insert_save");
// Content tabs
init($ifc_tab_1, $data->get($insert_object, "html") ? 2 : 1);
// Text tab
ifc_tab_open(CMS_L_IFC_INSERT_007);
$ifc->set(NULL, "texteditor 60x15 ilrt", $data->get($insert_object, "code"));
// HTML tab
ifc_tab_next(CMS_L_IFC_INSERT_006);
$ifc->set(NULL, "code_html 60x15 l", $data->get($insert_object, "code"));
ifc_tab_close();
```

| Component | Purpose |
|-----------|---------|
| `ifc_tab_open()`/`ifc_tab_next()`/`ifc_tab_close()` | Creates tabbed interface for content type |
| `texteditor` | Plain text editor with formatting options |
| `code_html` | HTML code editor |
| Sync script | JavaScript to keep text and HTML editors in sync |

---

### Tree View (FlexView)

Displays the hierarchical structure of inserts and containers.

```php
$flexview = new flexview();
$flexview->import_data($data);
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_checkbox_identifier("list");
$flexview->set_checkbox_list($list);
$flexview->show_tree(
    $object, // Index
    "javascript:ifc_post('select','%index%');", // Action
    "name" // Name key
);
```

| Parameter | Purpose |
|-----------|---------|
| `$object` | Currently selected object |
| Action | JavaScript to execute when an item is clicked |
| `"name"` | Key to use for display names |

---

## Usage Examples

### Example 1: Creating a New Insert

**Scenario**: A developer wants to create a reusable HTML snippet for a call-to-action button.

1. Navigate to the Insert Management interface
2. Click "Code" to open the insert editor
3. Click "Add" to create a new insert
4. Enter a name (e.g., "CTA Button")
5. Select the HTML tab
6. Enter the HTML code:
   ```html
   <div class="cta-button">
       <a href="#" class="button">Click Here</a>
   </div>
   ```
7. Click "Save"

**Result**: The new insert is available for use in content.

---

### Example 2: Organizing Inserts in Containers

**Scenario**: A content manager wants to organize inserts by category.

1. In the main interface, select a parent container (or root)
2. Click "Add" to create a new container
3. Enter a name (e.g., "Forms")
4. Click "Save"
5. Select the new "Forms" container
6. Click "Code" to open the insert editor
7. Create new inserts within this container (e.g., "Contact Form", "Newsletter Signup")

**Result**: Inserts are now organized hierarchically for easier management.

---

### Example 3: Assigning an Insert to a Page

**Scenario**: A developer wants to assign a standard footer to a specific page.

1. Navigate to the page in the content management interface
2. Open the Insert Management interface for that page
3. In the right panel, select the desired insert from the dropdown
4. Click "Save"

**Result**: The selected insert is now associated with the page and can be embedded using the appropriate template tag.

---

## Integration with Other Components

### Template Integration

Inserts can be embedded in templates or content using the `insert()` function:

```php
<?php
// Embed an insert by ID
echo insert("footer.standard");

// Embed an insert assigned to the current object
echo insert();
?>
```

### Data Structure

Inserts are stored in two primary data stores:

1. **`#system/insert`**
   - Stores the hierarchical structure of containers and objects
   - Each object can have an `insert` property pointing to an insert in `#system/insert.code`

2. **`#system/insert.code`**
   - Stores the actual code snippets
   - Each entry contains:
     - `name`: Display name
     - `html`: Boolean (1 for HTML, 0 for plain text)
     - `code`: The actual content

---

## Security Considerations

1. **Permission Check**: The interface checks for `CMS_L_ACCESS` permission before allowing access.
2. **Input Sanitization**: Uses `utf8_trim()`, `x()` (XML escaping), and `qx()` (combined escaping) for output.
3. **CSRF Protection**: All form submissions use the platform's built-in CSRF protection via `ifc_post()`.
4. **Content Type Handling**: Properly distinguishes between HTML and plain text content to prevent XSS.

---

## Performance Considerations

1. **Caching**: Uses `cms_cache()` to persist the selected object across sessions.
2. **Data Loading**: Only loads necessary data stores (`#system/insert` or `#system/insert.code`) based on the current operation.
3. **Tree Rendering**: Uses the `flexview` component for efficient rendering of hierarchical data.

---

## Localization

The interface uses localized strings for all user-facing text:

| Constant | Typical Value | Usage |
|----------|---------------|-------|
| `CMS_L_IFC_INSERT_002` | "New Insert" | Default name for new inserts |
| `CMS_L_IFC_INSERT_003` | "Insert" | Label for insert selection |
| `CMS_L_IFC_INSERT_004` | "Inserts" | Title for insert list |
| `CMS_L_IFC_INSERT_005` | "Settings" | Title for insert editor |
| `CMS_L_IFC_INSERT_006` | "HTML" | Tab label for HTML content |
| `CMS_L_IFC_INSERT_007` | "Text" | Tab label for plain text content |
| `CMS_L_IFC_INSERT_008` | "Inserts" | Title for main tree view |

---

## Error Handling

The interface uses the platform's standard error handling:

| Response | Meaning | User Feedback |
|----------|---------|---------------|
| `CMS_MSG_DONE` | Operation succeeded | Success message displayed |
| `CMS_MSG_ERROR` | Operation failed | Error message displayed |

Errors are displayed through the `ifc` class's response mechanism.


<!-- HASH:7706a5cd02f745b4b2160c411a7e08eb -->
