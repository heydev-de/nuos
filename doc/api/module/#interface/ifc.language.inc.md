# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.language.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Language Interface Module (`ifc.language.inc`)

Handles language management within the NUOS platform, including:
- Language selection, creation, modification, and deletion
- Language icon (flag) management
- Default language configuration
- Stopword management for search optimization

This interface module provides both backend logic (message handling) and frontend display components for managing multilingual support in the system.

---

### Constants and Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$object` | `cms_cache("language." . CMS_USER . ".object")` | Currently selected language object (tag) |
| `$list` | `[]` | Array of selected language tags for bulk operations |
| `$system` | `new system()` | System configuration handler |
| `$data` | `new data("#system/language")` | Language data storage handler |
| `$map` | `new map("#system/language.image")` | Language icon mapping storage |

---

### Message Handling

Processes interface messages (`CMS_IFC_MESSAGE`) to perform CRUD operations on languages.

#### ### `case "select":`
**Purpose:**
Selects a language as the current working object.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Language tag to select |

**Return/Output:**
- Sets `$object` to the selected language tag

**Usage:**
Triggered when a user clicks on a language in the list.

---

#### ### `case "add":`
**Purpose:**
Adds a new undefined language to the system.

**Inner Mechanisms:**
1. Checks if "x-undefined" language already exists
2. If not, creates it with a default name (`CMS_L_IFC_LANGUAGE_010`)
3. Sets the new language as the current object

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure

**Usage:**
Triggered when the "Add" command is executed.

---

#### ### `case "set":`
**Purpose:**
Updates language settings including:
- Language tag (identifier)
- Display name
- Default language status
- Language icon (flag)
- Stopwords

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New language tag (sanitized) |
| `$ifc_param2` | `string` | Display name |
| `$ifc_param3` | `bool` | Whether to set as primary default language |
| `$ifc_param4` | `bool` | Whether to delete the current icon |
| `$ifc_param5` | `string` | Stopwords (comma-separated) |
| `$ifc_file1` | `file` | Uploaded icon file |
| `$ifc_file1_name` | `string` | Original filename of the uploaded icon |

**Inner Mechanisms:**
1. **Tag Sanitization:** Removes non-alphanumeric characters and converts to lowercase
2. **Tag Change Handling:**
   - Renames existing icon file if tag changes
   - Deletes target language if it already exists
3. **Name Update:** Sets the display name (falls back to tag if empty)
4. **Default Language:**
   - If set as primary, prepends to the default language list
   - Maintains uniqueness in the list
5. **Icon Management:**
   - Deletes existing icon if new file uploaded or delete option selected
   - Processes valid image uploads (GIF, JPEG, PNG, SVG, WEBP)
   - Moves uploaded file to the language data directory
6. **Stopwords:** Updates the stopword list for the language

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure

**Usage:**
Triggered when saving language settings via the interface form.

---

#### ### `case "del":`
**Purpose:**
Deletes selected languages and their associated icons.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of language tags to delete |

**Inner Mechanisms:**
1. Iterates through each language in `$list`
2. Deletes the associated icon file if it exists
3. Removes the language entry from the data store
4. Clears the current object if it was deleted

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure
- Sets `$object` to `NULL` if the current object was deleted

**Usage:**
Triggered when the "Delete Selected" command is executed.

---

#### ### `case "enable":`
**Purpose:**
Enables selected languages as default system languages.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of language tags to enable |

**Inner Mechanisms:**
1. Retrieves current default languages
2. If no selection, clears the default language list
3. Otherwise, updates the list with selected languages, ensuring the primary language remains first
4. Triggers filesystem directory creation for enabled languages

**Return/Output:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure

**Usage:**
Triggered when the "Enable" command is executed.

---

### Main Display

Renders the language management interface with two main sections:
1. **Language List:** Displays all available languages with selection controls
2. **Language Settings:** Form for editing the currently selected language

#### Key Display Components

| Component | Description |
|-----------|-------------|
| **Menu** | Provides commands: Add, Delete Selected, Enable |
| **Language Table** | Lists all languages with columns for selection, tag, and name |
| **Selection Controls** | Buttons to select all, invert selection, deselect all, or select active languages |
| **Settings Form** | Editable fields for tag, name, default status, icon, and stopwords |

#### JavaScript Functions

| Function | Purpose |
|----------|---------|
| `s(index)` | Selects a language by its tag via AJAX |
| `language_select_active()` | Toggles checkboxes to select only currently enabled languages |

#### Display Logic

1. **Caching:** Persists the selected language object in cache after any message handling
2. **Primary Language:** Identifies the primary default language for highlighting
3. **Table Rendering:**
   - Sorts languages alphabetically by tag
   - Highlights the current object
   - Displays language icons if available
   - Provides selection checkboxes
4. **Form Rendering:**
   - Only shown if a language is selected
   - Includes all editable fields with appropriate input types
   - Handles file uploads for language icons
   - Provides a save button to apply changes

---

### Usage Context

**Typical Scenarios:**
1. **Adding a New Language:**
   - Click "Add" to create a new undefined language
   - Select the new language and fill in its details
   - Upload an icon if desired
   - Save the changes

2. **Modifying a Language:**
   - Select the language from the list
   - Update the tag, name, or stopwords as needed
   - Change the icon by uploading a new file or deleting the existing one
   - Save the changes

3. **Setting Default Languages:**
   - Select languages from the list
   - Click "Enable" to set them as default system languages

4. **Deleting Languages:**
   - Select languages to delete using the checkboxes
   - Click "Delete Selected" to remove them from the system

**Integration Points:**
- **System Configuration:** Default languages are stored in the system configuration
- **Search Optimization:** Stopwords are used to filter search queries
- **User Interface:** Language selection affects content display and localization
- **File System:** Language icons and directories are managed on the filesystem

**Dependencies:**
- `system` class for global configuration
- `data` class for language data storage
- `map` class for language-icon mapping
- `ifc` class for interface rendering
- `directory` module for filesystem operations


<!-- HASH:9aa79f0b2def7261a0bdf43edaa0056b -->
