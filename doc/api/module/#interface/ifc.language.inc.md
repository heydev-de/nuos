# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.language.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.language.inc)

- **Version:** `26.8.7.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Language Interface Module (`ifc.language.inc`)

This file implements the **Language Management Interface** for the PWNC Web Platform. It provides a user interface for:
- **Viewing** all available languages
- **Adding** new languages
- **Editing** language properties (name, icon, stopwords)
- **Deleting** languages
- **Enabling/disabling** languages as default options
- **Setting** a primary default language

The interface interacts with the system's language configuration, stored in `#system/language`, and manages language icons via a mapping file (`#system/language.image`).

---

### Core Functionality

#### **1. Permission & Initialization**
| Step | Description |
|------|-------------|
| **Permission Check** | Requires `CMS_L_ACCESS` permission to access the interface. |
| **Cache Initialization** | Retrieves the currently selected language object from permanent cache using `cms_cache_init()`. |
| **Class Instantiation** | Initializes core classes: `system`, `data` (for language data), and `map` (for language icons). |

---

### **Message Handling (Sub-Display Logic)**

The interface processes user actions via `CMS_IFC_MESSAGE` and updates the system accordingly.

#### **`select`**
**Purpose:**
Sets the currently selected language object.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Language tag (e.g., `en`, `de`). |

**Return/Effect:**
- Updates `$object` to the selected language tag.
- No explicit return value; modifies the interface state.

**Usage Example:**
```php
// User clicks on a language in the list
ifc_post("select", "fr"); // Selects French as the active language
```

---

#### **`add`**
**Purpose:**
Adds a new language with a default name (`CMS_L_IFC_LANGUAGE_010`).

**Parameters:**
None (uses hardcoded default).

**Return/Effect:**
| Return Value | Description |
|--------------|-------------|
| `CMS_MSG_DONE` | Language added successfully. |
| `CMS_MSG_ERROR` | Failed to save language data. |

**Inner Mechanisms:**
1. Checks if `x-undefined` (default language entry) exists.
2. If not, creates it with a default name.
3. Sets `$object` to `x-undefined` and saves the data.

**Usage Example:**
```php
// User clicks "Add Language" button
ifc_post("add"); // Creates a new language entry
```

---

#### **`set`**
**Purpose:**
Updates properties of the selected language (name, icon, stopwords, default status).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | New language tag (sanitized). |
| `$ifc_param2` | `string` | New display name. |
| `$ifc_param3` | `bool` | If `TRUE`, adds language to default list. |
| `$ifc_param4` | `bool` | If `TRUE`, deletes the current icon. |
| `$ifc_param5` | `string` | Stopwords (comma-separated). |
| `$ifc_file1` | `file` | Uploaded icon file. |

**Return/Effect:**
| Return Value | Description |
|--------------|-------------|
| `CMS_MSG_DONE` | Language updated successfully. |
| `CMS_MSG_ERROR` | Failed to save data or icon map. |

**Inner Mechanisms:**
1. **Tag Sanitization:**
   - Removes non-alphanumeric characters from `$ifc_param1`.
   - Converts to lowercase.
2. **Name Handling:**
   - If no name is provided (`$ifc_param2`), retains the current name.
   - If the tag changes, renames the language and updates the icon filename.
3. **Icon Management:**
   - Deletes the old icon if a new one is uploaded or deletion is requested.
   - Processes uploads (supports `gif`, `jpg`, `png`, `svg`, `webp`).
4. **Default Language:**
   - Adds the language to the system's default language list if `$ifc_param3` is set.
5. **Stopwords:**
   - Updates the stopwords list for the language.

**Usage Example:**
```php
// Update German language properties
ifc_post("set", "de", "Deutsch", TRUE, FALSE, "der,die,das");
ifc_file("icon", "/path/to/german_flag.png"); // Uploads a new icon
```

---

#### **`del`**
**Purpose:**
Deletes selected languages and their associated icons.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | `array` | Array of language tags to delete. |

**Return/Effect:**
| Return Value | Description |
|--------------|-------------|
| `CMS_MSG_DONE` | Languages deleted successfully. |
| `CMS_MSG_ERROR` | Failed to save data or icon map. |

**Inner Mechanisms:**
1. Iterates over `$list` and:
   - Deletes the language icon (if it exists).
   - Removes the language from the data store.
2. Resets `$object` to `NULL`.

**Usage Example:**
```php
// Delete French and Spanish languages
$list = ["fr", "es"];
ifc_post("del", $list);
```

---

#### **`enable`**
**Purpose:**
Enables/disables languages as default options for the system.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | `array` | Array of language tags to enable. Empty array disables all. |

**Return/Effect:**
| Return Value | Description |
|--------------|-------------|
| `CMS_MSG_DONE` | Default languages updated successfully. |
| `CMS_MSG_ERROR` | Failed to save system settings or update directory. |

**Inner Mechanisms:**
1. If `$list` is non-empty:
   - Ensures the primary language (first in the default list) is included.
   - Updates the system's default language list.
2. If `$list` is empty:
   - Clears the default language list.
3. Triggers filesystem updates via `directory_create_filesystem()`.

**Usage Example:**
```php
// Enable German and French as default languages
$list = ["de", "fr"];
ifc_post("enable", $list);
```

---

### **Main Display Logic**

#### **Cache Management**
- Permanently caches the selected language object if a message was processed (`CMS_IFC_MESSAGE` is non-empty).

#### **Primary Language Detection**
- Retrieves the system's default language list and identifies the primary language (first in the list).

#### **Menu Construction**
| Menu Item | Action | Description |
|-----------|--------|-------------|
| `CMS_L_COMMAND_ADD` | `add` | Adds a new language. |
| `CMS_L_COMMAND_DELETE_SELECTED` | `#del` | Deletes selected languages. |
| `CMS_L_IFC_LANGUAGE_009` | `#enable` | Enables selected languages as default. |

#### **Table Rendering**
1. **Language List Table:**
   - Displays all languages with columns for **selection**, **name**, and **tag**.
   - Highlights the currently selected language.
   - Shows language icons (fallback to default if none exists).
   - Provides selection controls (All/Invert/None/Active).

2. **Language Edit Panel (if `$object` is set):**
   - **Tag:** Editable text field (30 chars max).
   - **Name:** Editable text field (40 chars max).
   - **Primary Language:** Checkbox to set as primary.
   - **Icon:** File upload and delete option.
   - **Stopwords:** Textarea for comma-separated stopwords.

#### **JavaScript Helpers**
- `s(index)`: Posts a `select` message to set the active language.
- `language_select_active()`: Checks all enabled languages in the selection list.

---

### **Usage Example: Full Workflow**
```php
// 1. Add a new language (e.g., Italian)
ifc_post("add");

// 2. Set properties for Italian
ifc_post("set", "it", "Italiano", TRUE, FALSE, "il,lo,la,i,gli,le");
ifc_file("icon", "/path/to/italian_flag.png");

// 3. Enable Italian as a default language
$list = ["it", "en", "de"]; // Italian, English, German
ifc_post("enable", $list);

// 4. Select Italian for editing
ifc_post("select", "it");
```


<!-- HASH:86a2fae6734e78306bf9b764804c0cea -->
