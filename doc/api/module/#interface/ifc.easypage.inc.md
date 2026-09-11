# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.easypage.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.easypage.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## EasyPage Interface Module

The `ifc.easypage.inc` file implements the **EasyPage** interface controller for the PWNC Web Platform. It provides a hierarchical directory management system that allows users to organize content pages in a tree-like structure. This interface supports creating, editing, moving, deleting, and previewing directory entries, with optional linking to content items.

### Overview

This module serves as the backend logic for managing a directory structure where each node can represent either a container (folder) or a content page. It integrates with the `content`, `directory`, `flexview`, and `template` libraries to provide full CRUD operations on directory entries, including drag-and-drop reordering, template selection, and content linking.

---

## Constants and Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_L_ACCESS` | — | Minimum access level required to view this interface |
| `CMS_USER` | — | Current user identifier used in cache keys |
| `CMS_IFC_PAGE` | — | Identifier for the current interface page |
| `CMS_IFC_MESSAGE` | — | Action/message type being processed (e.g., "select", "add", "save") |
| `CMS_L_COMMAND_*` | — | Language constants for UI labels like Add, Save, Delete, etc. |
| `CMS_L_IFC_EASYPAGE_*` | — | Language constants specific to EasyPage interface elements |
| `CMS_DB_CONTENT` | — | Database table name for content entries |
| `CMS_DB_CONTENT_SCHEDULE` | — | Database table name for scheduled content actions |

---

## Initialization

### Library Loading

```php
if (! cms_load("content")) ifc_inactive($ifc_page);
if (! cms_load("directory")) ifc_inactive($ifc_page);
if (! cms_load("flexview")) ifc_inactive($ifc_page);
if (! cms_load("template")) ifc_inactive($ifc_page);
```

**Purpose:** Loads required libraries (`content`, `directory`, `flexview`, `template`). If any fail to load, the interface is marked inactive.

**Parameters:** None  
**Return:** None  

---

### Permission Check

```php
ifc_permission(["" => CMS_L_ACCESS]);
```

**Purpose:** Ensures the current user has sufficient permissions to access this interface.

**Parameters:**  
- Array mapping permission scopes to required access levels  

**Return:** None  

---

### Content Object Initialization

```php
$content = new content();
if (! $content->enabled) ifc_inactive($ifc_page);
```

**Purpose:** Instantiates the `content` class and checks if it's enabled.

**Parameters:** None  
**Return:** None  

---

### Cache Initialization

```php
cms_cache_init($object, "directory." . CMS_USER . ".object");
```

**Purpose:** Initializes caching for the selected directory object using a user-specific key.

**Parameters:**  
- `$object`: Selected directory object index  
- Key string: `"directory.{USER}.object"`  

**Return:** None  

---

## Message Handling / Sub Display

Handles various sub-actions based on `CMS_IFC_MESSAGE`.

### Case: `"select"`

```php
case "select":
    $object = $ifc_param;
    break;
```

**Purpose:** Sets the currently selected directory object.

**Parameters:**  
- `$ifc_param`: Index of the selected object  

**Return:** None  

---

### Case: `"add"`

```php
case "add":
    $ifc_param = $object;
    $ifc_param1 = CMS_L_IFC_EASYPAGE_007;
    $ifc_param2 = cms_cache("directory." . CMS_USER . ".hidden") ? TRUE : NULL;
    $ifc_param3 = (string)cms_cache("template." . CMS_USER . ".page");
```

**Purpose:** Prepares form fields for adding a new directory entry.

**Parameters:**  
- `$ifc_param`: Parent object index  
- `$ifc_param1`: Default title label  
- `$ifc_param2`: Hidden flag from cache  
- `$ifc_param3`: Template name from cache  

**Return:** None  

---

### Case: `"add_target"`

```php
case "add_target":
    $data = new data("#system/directory");
    $flexview = new flexview();
    $flexview->import_data($data);
```

**Purpose:** Displays the add form with target selection via FlexView hierarchy.

**Parameters:**  
- `$data`: Data handler for directory system  
- `$flexview`: Visual tree component  

**Return:** None  

#### JavaScript Function: `easypage_template_preview()`

```javascript
function easypage_template_preview()
{
    var value = ifc_get("ifc_param3");
    if (value) load_page("...");
}
```

**Purpose:** Triggers live preview of selected template by loading its URL dynamically.

**Parameters:**  
- `value`: Template name retrieved from input field  

**Return:** None  

---

### Case: `"add_insert"` / `"add_append"`

```php
case "add_insert":
case "add_append":
    if (! language_get($ifc_param1, FALSE))
        $ifc_param1 = language_set($ifc_param1, CMS_L_UNKNOWN, FALSE);
    $content->writer = TRUE;
    if ($index = $content->create($ifc_param1, $ifc_param3))
    {
        $content->editor = TRUE;
        $content->publisher = TRUE;
        if (($index = $content->publish(...)) !== FALSE)
        {
            ...
        }
    }
```

**Purpose:** Creates a new content item and publishes it under the specified directory.

**Parameters:**  
- `$ifc_param1`: Title of the new content  
- `$ifc_param3`: Template name  
- `$ifc_param`: Target directory index  
- `substr(CMS_IFC_MESSAGE, 4)`: Action type ("insert" or "append")  

**Return:**  
- On success: Redirects to main display  
- On failure: Sets `$ifc_response = CMS_MSG_ERROR`

---

### Case: `"save"`

```php
case "save":
    $directory = new directory();
    $directory->data->set($ifc_param1, $object, "name");
    $directory->data->set(isset($ifc_param2), $object, "hidden");
    if ($directory->save())
    {
        ...
    }
```

**Purpose:** Saves changes to an existing directory entry, including name, hidden status, and linked content metadata.

**Parameters:**  
- `$ifc_param1`: New name/title  
- `$ifc_param2`: Hidden checkbox state  
- `$ifc_param3`, `$ifc_param4`, `$ifc_param5`: Content title, description, keywords  

**Return:**  
- On success: `$ifc_response = CMS_MSG_DONE`  
- On failure: `$ifc_response = CMS_MSG_ERROR`

---

### Case: `"delete"`

```php
case "delete":
    $directory = new directory();
    $stack = [];
    $_object = $object;
    ...
```

**Purpose:** Deletes a directory branch recursively, collecting linked content and schedules before removal.

**Parameters:**  
- `$ifc_param`: Object to delete  
- `$object`: Current selection after deletion  

**Return:**  
- On success: `$ifc_response = CMS_MSG_DONE`  
- On failure: `$ifc_response = CMS_MSG_ERROR`

---

### Case: `"insert"` / `"append"`

```php
case "insert":
case "append":
    $directory = new directory();
    $ifc_param = explode(",", $ifc_param);
    $value = $ifc_param[0];
    $ifc_param = $ifc_param[1];
    ...
```

**Purpose:** Moves a directory entry into another position within the hierarchy.

**Parameters:**  
- `$value`: Source object index  
- `$ifc_param`: Target object index  

**Return:**  
- On success: `$ifc_response = CMS_MSG_DONE`  
- On failure: `$ifc_response = CMS_MSG_ERROR`

---

### Case: `"template_preview"`

```php
case "template_preview":
    template_preview($object);
    exit();
```

**Purpose:** Renders a preview of the template associated with the selected object.

**Parameters:**  
- `$object`: Directory object index  

**Return:** Exits script after rendering preview

---

## Main Display

Renders the primary EasyPage interface showing the directory hierarchy and details panel.

### Cache Selected Object

```php
if (CMS_IFC_MESSAGE !== "") cms_cache("directory." . CMS_USER . ".object", $object, TRUE);
```

**Purpose:** Persists the currently selected object in cache.

**Parameters:**  
- `$object`: Selected object index  
- Key: `"directory.{USER}.object"`  

**Return:** None  

---

### Menu Setup

```php
$menu = [CMS_L_COMMAND_ADD . "|easypage/command_create" => "add"];
```

**Purpose:** Defines top-level menu options.

**Parameters:**  
- Label and command mapping  

**Return:** None  

---

### Directory Hierarchy Rendering

Uses `flexview` to render the directory tree with drag-and-drop support.

#### JavaScript Event Handler: `directory_easypage_event()`

```javascript
function directory_easypage_event(event, source, target)
{
    ...
}
```

**Purpose:** Handles drag-and-drop events for moving directory entries.

**Parameters:**  
- `event`: Type of event ("dropon")  
- `source`: Dragged element  
- `target`: Drop target  

**Return:** None  

---

### Trash Bin Section

```php
echo("<td id=\"trashbin\" ...>" . image("easypage/command_delete") . " " . CMS_L_COMMAND_DELETE . "</td>");
```

**Purpose:** Renders a trash bin area for deleting entries via drag-and-drop.

**Parameters:**  
- Image and label constants  

**Return:** None  

---

### Details Panel (Conditional)

Displays editable properties of the selected directory entry when `$object` is set.

#### Edit Button

```php
$url = translate_url("directory://$object", [...], CMS_LANGUAGE, TRUE);
$ifc->set(CMS_L_COMMAND_EDIT, "button", "javascript:load_page('" . q($url) . "');");
```

**Purpose:** Opens the linked content editor in a new page.

**Parameters:**  
- `$object`: Directory object index  
- Options array for translation  

**Return:** None  

---

### Usage Example

To use the EasyPage interface programmatically:

```php
// Load the interface
include('module/#interface/ifc.easypage.inc');

// Select an object
$_GET['ifc_param'] = 123;
define('CMS_IFC_MESSAGE', 'select');

// Process the request
// The interface will now show the selected directory entry
```

This example demonstrates how to integrate the EasyPage interface into a custom workflow by setting appropriate global variables and including the interface file.


<!-- HASH:a5226c4b6753ef8edfe685f905bc8c45 -->
