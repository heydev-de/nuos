# PWNC API Documentation

[← Index](../README.md) | [`module/interface.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/interface.php)

- **Version:** `26.6.21.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module Interface Management (`module/interface.php`)

This file serves as the central entry point for PWNC's modular interface system. It handles:

1. **Module Discovery** – Scans the interface directory for available modules (files matching `ifc.*.inc`).
2. **Permission Validation** – Ensures only permitted modules are listed.
3. **Ordering & Customization** – Applies a user-defined order file (`order`) to prioritize or group modules.
4. **Module Execution** – Loads and executes the selected module or falls back to a default interface.

---

### Constants & Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_INTERFACE_PATH` | `./interface/` | Path to the directory containing interface modules. |
| `CMS_IFC_PAGE` | *(Request parameter)* | Identifier of the currently selected module. |
| `CMS_IFC_OPTION` | *(Request parameter)* | Optional flags (e.g., `"external"`). |
| `$ifc_page` | *(Array)* | Associative array of available modules (`[id => label]`). |

---

### Functions

---

#### `ifc_module_list()`

**Purpose:**
Scans the interface directory for valid modules, checks permissions, and applies a custom order if defined.

**Parameters:**
None.

**Return Values:**
| Type | Description |
|------|-------------|
| `array` | Associative array of module IDs and their display labels (`[id => label]`). |

**Inner Mechanisms:**
1. **Directory Scanning** – Uses `opendir()` and `readdir()` to iterate over files in `CMS_INTERFACE_PATH`.
2. **File Validation** – Checks for files matching `ifc.*.inc` and verifies permissions via `cms_permission()`.
3. **Label Resolution** – Uses a constant (`CMS_L_IFC_*`) if defined; falls back to the module ID.
4. **Order Application** – Reads `order` file (if present) and reorders modules accordingly. Unlisted modules are appended at the end.
5. **Separator Handling** – Treats `-` as a visual separator in the order file.

**Usage Context:**
- Called during interface initialization to populate the module list.
- Used by the navigation system to render available modules.

**Example:**
```php
$modules = ifc_module_list();
// Output: ["dashboard" => "Dashboard", "users" => "User Management", ...]
```

---

### Execution Flow

1. **Module List Generation**
   - `ifc_module_list()` is called to retrieve available modules.

2. **Module Selection**
   - If `CMS_IFC_PAGE` is set and valid:
     - Sets a cookie to persist the selection.
     - Skips further processing if `external` is set in `CMS_IFC_OPTION`.
     - Loads the module file (`ifc.{ID}.inc`).

3. **Fallback to Default**
   - If no module is selected, `ifc_default($ifc_page)` is called to render a default interface.

**Example (Module Execution):**
```php
// URL: /interface.php?ifc_page=users
$ifc_page = ifc_module_list();
if (isset($ifc_page["users"])) {
    require(CMS_INTERFACE_PATH . "ifc.users.inc");
}
```

**Example (Default Fallback):**
```php
// URL: /interface.php
ifc_default($ifc_page);
```


<!-- HASH:cd771d9225d7f127ce54400bcd45f6e7 -->
