# NUOS API Documentation

[← Index](../README.md) | [`module/interface.php`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module Interface Management (`module/interface.php`)

Core file responsible for discovering, ordering, and executing interface modules in the NUOS platform. Acts as a router between the system and individual interface components (modules) by:

1. Enumerating available modules
2. Applying custom ordering rules
3. Handling module execution based on current request context
4. Falling back to a default interface when no module is specified

---

### Constants & Paths

| Name | Value | Description |
|------|-------|-------------|
| `CMS_INTERFACE_PATH` | System-defined | Absolute filesystem path to the interface modules directory |
| `CMS_IFC_PAGE` | Request parameter | Current interface module identifier (e.g., `dashboard`, `editor`) |
| `CMS_IFC_OPTION` | Request parameter | Additional module execution options (e.g., `external`) |

---

### Functions

---

#### `ifc_module_list()`

**Purpose**
Discovers all available interface modules, checks user permissions, and applies custom ordering rules defined in an `order` file.

**Parameters**
None.

**Return Values**

| Type | Description |
|------|-------------|
| `array` | Associative array of module identifiers (keys) and their display names (values). Modules marked with `"-"` act as visual separators in UI lists. |

**Inner Mechanisms**
1. Scans `CMS_INTERFACE_PATH` for files matching the pattern `ifc.{module}.inc`.
2. Validates each file against user permissions via `cms_permission()`.
3. Resolves display names from language constants (`CMS_L_IFC_{MODULE}`) or falls back to the module identifier.
4. Reads an optional `order` file (one module per line) to reorder modules. Lines containing only `"-"` create visual separators.
5. Unordered modules are appended to the end of the list.

**Usage Context**
- Called during system initialization to populate module lists in navigation menus, dashboards, or module selectors.
- Used by the interface router to determine valid module targets.

---

### Execution Flow

1. **Module Discovery**
   - `ifc_module_list()` is called to retrieve the list of available modules.

2. **Module Selection & Execution**
   - If `CMS_IFC_PAGE` is set and valid, the corresponding module file (`ifc.{module}.inc`) is loaded.
   - A cookie (`cms_ifc_page`) is set to persist the current module across requests.
   - If `CMS_IFC_OPTION` contains `"external"`, the module is executed in isolation (no surrounding interface chrome).

3. **Default Fallback**
   - If no module is selected or the selection is invalid, `ifc_default()` is called with the module list to render a default interface (e.g., dashboard or module selector).

---

### Typical Scenarios

1. **Navigation Menu Population**
   ```php
   $modules = ifc_module_list();
   foreach ($modules as $id => $name) {
       if ($name === "-") {
           echo '<hr>'; // Visual separator
       } else {
           echo '<a href="'.u($id).'">'.$name.'</a>';
       }
   }
   ```

2. **Module Execution**
   - A request to `https://example.com/admin?ifc_page=editor` triggers:
     - `ifc_module_list()` to validate `editor` as a permitted module.
     - Loading of `ifc.editor.inc` for execution.

3. **External Module Execution**
   - A request with `ifc_page=export&ifc_option=external` loads `ifc.export.inc` without surrounding UI elements (e.g., for popups or API responses).

4. **Default Interface**
   - A request to `https://example.com/admin` (no `ifc_page`) falls back to `ifc_default()` to render a dashboard or module selector.


<!-- HASH:6e17471674d61b70a6264a5414e69313 -->
