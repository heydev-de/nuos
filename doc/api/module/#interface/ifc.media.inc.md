# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.media.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.media.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Interface

The `ifc.media.inc` file serves as the interface controller for managing media assets within the PWNC Web Platform. It handles various operations such as uploading, displaying, editing, replacing, deleting, and categorizing media files. The interface interacts with the `media` and `media_type` libraries to perform these tasks, leveraging caching mechanisms and language support for multilingual environments.

### Key Components

| Component | Description |
|-----------|-------------|
| `media` | Core library for handling media objects, including file uploads, linking, replacement, and deletion. |
| `media_type` | Manages media type definitions, including extensions, MIME types, and associated HTML code templates. |
| `ifc` | Interface class used to build forms and manage user interactions. |
| `language_*` functions | Handle language-specific object mappings and selections. |

### Message Handling

The interface processes different messages via `CMS_IFC_MESSAGE`, which determines the action to be performed:

#### `select`
Sets the selected media object based on the provided parameter and language.

#### `select_language`
Updates the current language context.

#### `display`
Displays a preview of the selected media object along with its size and MIME type information.

#### `upload`
Renders an upload form for a single media file, allowing users to specify name, file, type, category, and filename.

#### `_upload`
Processes the uploaded media file, adding it to the system and updating the object reference.

#### `upload_multi`
Renders a form for uploading multiple media files simultaneously.

#### `_upload_multi`
Handles batch processing of multiple uploaded files, adding each to the system.

#### `add` / `edit`
Displays a form for adding or editing media links, pre-filling fields when editing existing entries.

#### `_add`
Creates a new media link from the submitted form data.

#### `_edit`
Updates an existing media entry with the submitted form data.

#### `replace`
Displays a form for replacing an internal media file with a new one.

#### `_replace`
Processes the replacement of an internal media file.

#### `delete`
Deletes selected media objects and updates language references accordingly.

#### `category_rename`
Displays a form for renaming the category of a selected media object.

#### `_category_rename`
Renames the category across all media objects sharing the same category.

#### `type` / `type_select` / `type_add` / `type_set` / `type_delete`
Manages media type definitions, including selection, addition, modification, and deletion of types.

### Main Display

The main display section renders the primary media management interface, featuring:

- A category dropdown for filtering media by category.
- A list of media objects within the selected category, with checkboxes for selection.
- Language selector for multilingual contexts.
- An iframe for previewing the selected media object.
- Action buttons for inserting, uploading, adding, editing, replacing, deleting, and managing categories and types.

### Usage Example

```php
// Initialize the media interface
$media = new media();
$object = "example_media_object";
$language = "en";

// Set the selected object for the current language
$object = language_set($object, "new_media_object", $language);

// Display the media interface
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object" => $object, "language" => $language]
);
```

This example demonstrates initializing the media interface, setting a selected object, and rendering the interface with appropriate parameters.


<!-- HASH:291129f31e52031e18999d4acb99bee3 -->
