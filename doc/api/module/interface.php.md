# PWNC API Documentation

[← Index](../README.md) | [`module/interface.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/interface.php)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## module/interface.php

This file serves as the main entry point for the PWNC interface module system. It dynamically loads and executes interface modules based on the current request, handling module listing, selection, and execution. The file also includes special handling for generating MCP (Multi-Chieftain Protocol) client bundles and displaying a default interface page when no specific module is selected.

### ifc_module_list()

Retrieves a list of available interface modules from the interface directory, applying permission checks and ordering based on a configuration file.

#### Parameters

None.

#### Return Values

| Type | Description |
|------|-------------|
| `array` | An associative array of available interface modules where keys are module names and values are their display names. Returns an empty array if the interface directory cannot be opened. |

#### Inner Mechanisms

1. Opens the interface directory (`CMS_INTERFACE_PATH`)
2. Iterates through files matching the pattern `ifc.*.inc`
3. For each matching file, checks if the user has permission to access the corresponding interface module
4. Attempts to resolve a display name from a constant (`CMS_L_IFC_<MODULE_NAME>`) or falls back to the module name
5. Reads an optional `order` file to determine the display order of modules
6. Processes the order file, supporting `-` as a separator and appending any unordered modules at the end

#### Usage Example

```php
// Get list of available interface modules
$modules = ifc_module_list();
foreach ($modules as $name => $displayName) {
    echo "<li>$displayName</li>";
}
```

This would output a list of all accessible interface modules with their display names.

### Module Execution Logic

After retrieving the module list, the script determines which module to execute based on the `CMS_IFC_PAGE` constant:

#### When a Module is Selected

If `CMS_IFC_PAGE` is set and exists in the module list:
1. Sets a cookie to remember the selected page
2. Checks if the option contains "external" (which nullifies the module list)
3. Includes and executes the corresponding module file (`ifc.<module_name>.inc`)

#### When No Module is Selected

If no valid module is selected:
1. Checks if an MCP bundle download is requested (`CMS_IFC_MESSAGE === "mcpb"`)
2. If so, generates and downloads the MCP client bundle
3. Otherwise, displays the default interface page using `ifc_default()`

#### Usage Context

This file is typically accessed directly via a web request to the interface section of PWNC. The module to execute is determined by URL parameters or configuration constants. For example:

- Accessing `/interface.php?page=settings` would load `ifc.settings.inc`
- Accessing `/interface.php?message=mcpb` would trigger MCP bundle generation
- Accessing `/interface.php` without parameters would show the default interface page

The module system allows for extensible interface components that can be easily added by creating new `ifc.*.inc` files in the interface directory, provided they follow the naming convention and permission requirements.


<!-- HASH:141c10ebcb82af7c7ebbd060bcee9ca4 -->
