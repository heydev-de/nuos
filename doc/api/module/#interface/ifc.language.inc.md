# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.language.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.language.inc)

- **Version:** `26.9.7.10`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Language Interface Module

The `ifc.language.inc` file is a PWNC Web Platform interface module responsible for managing website languages. It provides a complete administrative interface for creating, editing, deleting, and enabling/disabling languages within the system. The module handles language metadata (names, tags), associated icons, stopwords, and default language configuration.

### Overview

This interface operates through message-based actions (`select`, `add`, `set`, `del`, `enable`) that correspond to CRUD operations on language definitions. Languages are stored in the `#system/language` data container, with icons managed separately in `#system/language.image`. The module integrates with the system configuration to manage default languages and uses filesystem operations for icon management.

### Key Components

| Component | Type | Description |
|-----------|------|-------------|
| `$object` | Variable | Currently selected language tag |
| `$data` | Data instance | Manages `#system/language` data container |
| `$map` | Map instance | Manages `#system/language.image` icon mappings |
| `$system` | System instance | Manages system-wide configuration |
| `$list` | Array | List of selected languages for batch operations |

### Message Handling

#### `select`
Sets the currently selected language object from the interface parameter.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | string | Language tag to select |

**Usage:**
```php
// Selects the language with tag "en"
$object = "en";
```

#### `add`
Creates a new undefined language entry with a default name.

**Mechanism:**
1. Sets a default name (`CMS_L_IFC_LANGUAGE_010`) for the new language
2. Saves the data container
3. Returns success/error message

**Usage:**
```php
// Adds a new language with default name
$data->set(["name" => "New Language"], "x-undefined");
$object = "x-undefined";
```

#### `set`
Updates an existing language's properties including name, tag, icon, and stopwords.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | string | New language tag |
| `$ifc_param2` | string | New language name |
| `$ifc_param3` | mixed | Default language flag |
| `$ifc_file1` | file | Icon upload |
| `$ifc_param5` | string | Stopwords |

**Mechanism:**
1. Sanitizes the new tag (alphanumeric, lowercase)
2. Handles tag renaming with icon migration
3. Updates language name
4. Manages default language configuration
5. Processes icon uploads/deletions
6. Saves stopwords

**Usage:**
```php
// Updates language "en" with new name and icon
$ifc_param1 = "en";
$ifc_param2 = "English";
$ifc_param3 = true; // Set as default
$ifc_file1 = $_FILES['icon'];
$ifc_param5 = "the,a,an";
```

#### `del`
Deletes selected languages and their associated icons.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | array | Array of language tags to delete |

**Mechanism:**
1. Iterates through selected languages
2. Deletes associated icons from filesystem
3. Removes icon mappings
4. Deletes language data
5. Saves both data containers

**Usage:**
```php
// Deletes languages "fr" and "de"
$list = ["fr", "de"];
foreach ($list AS $value) {
    $data->del($value);
}
```

#### `enable`
Toggles which languages are enabled in the system.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | array | Array of language tags to enable |

**Mechanism:**
1. Retrieves current default languages
2. Ensures primary language remains enabled
3. Updates system configuration
4. Creates filesystem entries for enabled languages
5. Saves system configuration

**Usage:**
```php
// Enables "en" and "fr" languages
$list = ["en", "fr"];
$system->setval(implode(",", $list), "language", "default");
```

### Display Rendering

The main display section renders:
1. A table of all languages with selection checkboxes
2. An editing panel for the selected language
3. Controls for managing default languages
4. Icon upload/delete functionality
5. Stopword management

### JavaScript Functions

#### `s(index)`
Selects a language by posting to the interface.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `index` | string | Language tag to select |

**Usage:**
```javascript
// Selects language "en"
s('en');
```

#### `language_select_active()`
Toggles selection of active (enabled) languages.

**Mechanism:**
1. Gets all language checkboxes
2. Checks/unchecks based on enabled status
3. Uses `object.click()` to trigger selection

**Usage:**
```javascript
// Toggles selection of all enabled languages
language_select_active();
```

### Constants Used

| Constant | Description |
|----------|-------------|
| `CMS_L_ACCESS` | Access permission level |
| `CMS_L_IFC_LANGUAGE_004` | "Tag" label |
| `CMS_L_IFC_LANGUAGE_005` | "Name" label |
| `CMS_L_IFC_LANGUAGE_006` | "Settings" label |
| `CMS_L_IFC_LANGUAGE_007` | "Icon" label |
| `CMS_L_IFC_LANGUAGE_009` | "Enable" command |
| `CMS_L_IFC_LANGUAGE_010` | Default language name |
| `CMS_L_IFC_LANGUAGE_011` | "Select Active" button |
| `CMS_L_IFC_LANGUAGE_012` | "Stopwords" label |
| `CMS_L_IFC_LANGUAGE_013` | "Primary Language" label |
| `CMS_L_COMMAND_ADD` | "Add" command |
| `CMS_L_COMMAND_DELETE` | "Delete" command |
| `CMS_L_COMMAND_SAVE` | "Save" command |
| `CMS_L_ALL` | "All" label |
| `CMS_L_INVERT` | "Invert" label |
| `CMS_L_NONE` | "None" label |
| `CMS_MSG_DONE` | Success message |
| `CMS_MSG_ERROR` | Error message |
| `CMS_DATA_PATH` | Data directory path |
| `CMS_DATA_URL` | Data directory URL |
| `CMS_USER` | Current user identifier |

### File Operations

The module performs several filesystem operations:
- Creates language directory if needed (`mkpath`)
- Moves uploaded icon files (`move_uploaded_file`)
- Renames icon files during tag changes (`rename`)
- Deletes icon files (`unlink`)
- Checks file existence (`is_file`)

### Data Structure

Languages are stored with the following structure:
```
#system/language
├── [tag] => [
│   ├── "name" => Language name
│   └── "stopword" => Comma-separated stopwords
│   ]
└── ...

#system/language.image
├── [tag] => icon_filename
└── ...
```

### System Configuration

Default languages are stored in system configuration:
```
system.language.default = "en,fr,de"
```

The first language in the comma-separated list is considered the primary language.


<!-- HASH:86a2fae6734e78306bf9b764804c0cea -->
