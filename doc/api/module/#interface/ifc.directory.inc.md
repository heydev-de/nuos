# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.directory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.directory.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Directory Interface

The `ifc.directory.inc` file implements the directory management interface for the PWNC Web Platform. It provides a comprehensive UI for managing hierarchical directory structures, including adding, editing, deleting, copying, cutting, sorting, and cleaning directory entries. It also manages directory type icons and their associated metadata.

### Key Components

| Component | Description |
|-----------|-------------|
| `directory` | Core class for directory operations (load, save, insert, append, del) |
| `data` | Data abstraction layer for reading/writing structured data |
| `flexview` | Hierarchical view component for displaying directory trees |
| `ifc` | Interface controller for rendering forms, menus, and handling responses |

### Message Handling

The interface processes various messages via `CMS_IFC_MESSAGE`, each triggering specific actions:

#### Type Management Messages

##### `type`, `type_select`, `type_add`, `type_save`, `type_delete`

These messages handle directory type icon management:

- **type**: Initializes the type object from the directory data.
- **type_select**: Sets the currently selected type object.
- **type_add**: Creates a new type object with a default name.
- **type_save**: Saves type properties including icon uploads (normal and active states).
- **type_delete**: Removes selected type objects and cleans up associated files and references.

**Parameters**:

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Selected type object key |
| `$ifc_param1` | string | Type name |
| `$ifc_file1`, `$ifc_file2` | array | Uploaded files for normal and active icons |
| `$ifc_file1_name`, `$ifc_file2_name` | string | Original filenames of uploaded icons |
| `$ifc_param2`, `$ifc_param3` | boolean | Flags for additional options |
| `$list` | array | List of selected type objects for deletion |

**Return Values**: None (sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`)

**Usage Example**:
```php
// Adding a new directory type
$ifc_param1 = "New Type";
$data = new data("#system/directory.type");
$_type_object = $data->append();
$data->set($_type_object, $_type_object, "#subtype");
$data->save();
```

#### Directory Management Messages

##### `select`

Sets the currently selected directory object.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Object key to select |

##### `save`

Saves the current directory object's properties, including canonical URL processing for multilingual support.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | Object key |
| `$ifc_param1` | string | Name |
| `$ifc_param5` | string | Description |
| `$ifc_param6` | string | URL |
| `$ifc_param9` | string | Subtype |
| `$ifc_param2` | boolean | Hidden flag |
| `$ifc_param3` | boolean | Placeholder flag |
| `$ifc_param4` | boolean | Dynamic flag |
| `$ifc_param10` | string | Image button |
| `$ifc_param11` | string | Image hover |
| `$ifc_param12` | string | Image active |
| `$ifc_param7` | string | Path |
| `$ifc_param8` | string | Canonical URL |

**Usage Example**:
```php
$directory = new directory();
$directory->set($object, $name, $description, $url, $subtype, $hidden, $placeholder, $dynamic, $image_button, $image_hover, $image_active, $path, $canonical);
$directory->save();
```

##### `add`, `add_target`

Handles adding new directory entries with a two-step process:
1. `add`: Initializes form fields with defaults
2. `add_target`: Displays the add form with a target selection tree

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Target parent object |
| `$ifc_param1` | string | Name |
| `$ifc_param2` | boolean | Hidden flag |
| `$ifc_param3` | boolean | Placeholder flag |
| `$ifc_param4` | boolean | Dynamic flag |
| `$ifc_param5` | string | Description |
| `$ifc_param6` | string | URL |
| `$ifc_param7` | string | Subtype |

##### `add_insert`, `add_append`

Inserts or appends a new directory entry under the selected target.

**Parameters**: Same as `add`

**Return Values**: Sets `$object` to the new entry's key on success

##### `copy_insert`, `copy_append`, `cut_insert`, `cut_append`

Handles copying or cutting directory entries with buffer management.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Comma-separated source,target keys |

**Mechanism**:
1. Clones the directory data structure
2. Copies or cuts the source section in the clone
3. Transmits the buffer to the master data structure
4. Inserts or appends the buffer at the target location

##### `sort`

Sorts directory tree branches below the selected entry by name.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | Object to sort under |

##### `clean`

Purges empty directory branches that are not placeholders and have no URL.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | string | Root object to clean from |

**Mechanism**:
1. Builds a path stack to the object
2. Traverses the tree tracking container depth
3. Deletes containers below the limit depth
4. Selects the rightmost existing object in the path

##### `del`

Deletes a directory entry and adjusts selection to the last existing parent.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Object key to delete |

### Main Display

Renders the primary directory management interface with:
- Canonical URL display
- Hierarchical directory tree with drag-and-drop support
- Trash bin for deletions
- Property editor for selected objects
- Tabbed interface for advanced properties (description, URL, path, canonical, images, subtype)

**JavaScript Integration**:
The interface includes a `directory_flexview_event` function that handles drag-and-drop operations:
- `dropon`: Moves (cut) or deletes entries
- `dropon_alt`: Copies entries

**Usage Example**:
```php
// Rendering the main directory interface
$ifc = new ifc($ifc_response, $ifc_page, $menu, ["object" => $object]);
$flexview->show_hierarchy($object, "javascript:ifc_post('select','%index%');", "name", NULL, directory_get_type(), "", "directory_flexview_event");
```

### Helper Functions

| Function | Description |
|----------|-------------|
| `directory_get_type()` | Returns available directory types |
| `directory_get_type_select()` | Generates HTML select options for types |
| `directory_get_canonical($object)` | Gets canonical URL for an object |
| `directory_flexview_display_function` | Custom display callback for flexview |
| `directory_flexview_event` | JavaScript event handler for drag-drop |

### Caching

The interface uses permanent caching for the selected object:
```php
cms_cache_init($object, "directory." . CMS_USER . ".object");
if (nstre(CMS_IFC_MESSAGE)) cms_cache("directory." . CMS_USER . ".object", $object, TRUE);
```

This maintains selection state across requests for the current user.


<!-- HASH:498a10a395532bf817ff6af99688cffa -->
