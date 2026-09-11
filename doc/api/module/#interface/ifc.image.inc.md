# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.image.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Image Interface Module

The `ifc.image.inc` file is the interface controller for the **Image Management** module within the PWNC Web Platform. It handles all user interactions related to managing images, including uploading, editing, replacing, deleting, categorizing, and configuring image settings.

This interface supports both internal (uploaded) and external (linked) images, provides multi-language support, and integrates with the platform's caching and permission systems.

### Key Features:
- Upload single or multiple images
- Add/edit image metadata (name, URL, category)
- Replace existing internal images
- Delete images and manage categories
- Configure image processing preferences
- Clear image cache
- Multi-language support via `language_set()` / `language_get()`
- Permission-based access control using `CMS_IMAGE_PERMISSION_OPERATOR`

---

## Constants and Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IMAGE_PERMISSION_OPERATOR` | Defined elsewhere | Permission level required for advanced operations like upload, edit, delete, and configuration |
| `CMS_L_ACCESS` | Defined elsewhere | Base access level for viewing the interface |
| `CMS_USER` | Dynamic | Current user identifier used in cache keys |
| `CMS_IFC_PAGE` | Dynamic | Current interface page identifier |
| `CMS_IFC_MESSAGE` | Dynamic | Action/message type being processed (e.g., "upload", "edit") |
| `CMS_PROTOCOL` | Dynamic | Protocol used for external URLs (http/https) |

---

## Initialization

### Loading Libraries and Permissions

```php
if (! cms_load("image")) ifc_inactive($ifc_page);
```
Loads the `image` library. If it fails, the interface becomes inactive.

```php
ifc_permission(["" => CMS_L_ACCESS, CMS_IMAGE_PERMISSION_OPERATOR => CMS_L_OPERATOR]);
```
Sets up permission levels:
- Default access (`""`) requires `CMS_L_ACCESS`
- Operator-level actions require `CMS_IMAGE_PERMISSION_OPERATOR`

```php
cms_cache_init($object, "image." . CMS_USER . ".object");
```
Initializes `$object` from permanent cache if available.

```php
init($language);
```
Initializes language context.

---

## Message Handling / Sub Displays

The main logic is driven by a `switch` statement on `CMS_IFC_MESSAGE`. Each case handles a specific action.

### Case: `select`

Handles selection of an object (image) and updates the language mapping.

```php
$object = language_set($object, $ifc_param, $language);
```

**Parameters:**
- `$object`: Selected image object
- `$ifc_param`: New value to set
- `$language`: Language context

**Usage Example:**
When a user selects an image from the list, this sets the current object for that language.

---

### Case: `select_language`

Updates the active language.

```php
$language = $ifc_param;
```

**Parameters:**
- `$ifc_param`: New language code

**Usage Example:**
Switching between languages in a multilingual setup.

---

### Case: `display`

Displays a preview of the selected image.

```php
if (! $object) exit();
cms_cache("image." . CMS_USER . ".object", $object, TRUE);
$image = translate_url($object);
$size = getimagesize($image, $info);
$_image = image_process($image, 600, 600, NULL, FALSE, FALSE);
preview(...);
exit();
```

**Parameters:**
- `$object`: Image identifier
- `$image`: Translated physical path
- `$size`: Dimensions from `getimagesize()`
- `$_image`: Processed thumbnail

**Return Values:**
Outputs HTML preview inside an iframe.

**Usage Example:**
Used when displaying a live preview of the selected image in the right-hand panel.

---

### Case: `upload`

Displays the upload form for a single image.

```php
$image = new image();
$_object = language_get($object, $language, TRUE);
$category = $image->data->get($_object, "category") ?? "";
$ifc = new ifc(...);
$ifc->set(CMS_L_NAME, "text 40 40 bl");
$ifc->set(CMS_L_IFC_IMAGE_002 . " (GIF, JPG, PNG, SVG, WEBP)", "file 40 b");
$ifc->set(CMS_L_IFC_IMAGE_014, "label");
$ifc->set(image_get_select(), "list 40 40 b", $category);
$ifc->set(CMS_L_IFC_IMAGE_015, "title");
$ifc->set(CMS_L_IFC_IMAGE_013, "text 35 40");
echo("<span class=\"element\">.ext</span>");
```

**Parameters:**
- `$ifc_param1`: Name input field
- `$ifc_param2`: File input field
- `$ifc_param3`: Category dropdown

**Usage Example:**
User clicks "Upload" button → form appears with fields for name, file, and category.

---

### Case: `_upload`

Processes the uploaded file.

```php
if (isset($ifc_file1_name)) {
    $image = new image();
    if ($_object = $image->add($ifc_file1, $ifc_file1_name, $ifc_param1, $ifc_param2, $ifc_param3)) {
        $object = language_set($object, $_object, $language);
        $ifc_response = CMS_MSG_DONE;
        break;
    };
}
$ifc_response = CMS_MSG_ERROR;
```

**Parameters:**
- `$ifc_file1`: Uploaded file data
- `$ifc_file1_name`: Original filename
- `$ifc_param1`: User-provided name
- `$ifc_param2`: Category
- `$ifc_param3`: Filename override

**Return Values:**
Sets `$ifc_response` to success or error message.

**Usage Example:**
After submitting the upload form, this processes the file and stores it.

---

### Case: `upload_multi`

Displays the multi-upload form.

Similar to `upload`, but allows selecting multiple files at once.

```php
$ifc->set(CMS_L_IFC_IMAGE_002 . " (GIF, JPG, PNG, SVG, WEBP)", "multifile 40 b");
```

**Usage Example:**
User clicks "Upload Multiple" → form with multi-file selector appears.

---

### Case: `_upload_multi`

Processes multiple uploaded files.

```php
if (is_array($ifc_file1)) $array = array_combine($ifc_file1, $ifc_file1_name);
else $array = [$ifc_file1 => $ifc_file1_name];

foreach ($array AS $key => $value) {
    if ($__object = $image->add($key, $value, NULL, $ifc_param1))
        $_object = $__object;
    else
        $error = TRUE;
}
```

**Parameters:**
- `$ifc_file1`: Array of uploaded files
- `$ifc_file1_name`: Array of filenames
- `$ifc_param1`: Category

**Usage Example:**
Bulk upload of several images into one category.

---

### Case: `add` / `edit`

Displays the add/edit form for image metadata.

For **add**:
```php
$ifc->set(CMS_L_NAME, "text 40 40 bl", $ifc_param1 ?? NULL);
$ifc->set(CMS_L_URL, "text 40 256 b", $ifc_param2 ?? CMS_PROTOCOL . "://");
```

For **edit**:
```php
$ifc_param1 = $image->data->get($_object, "name");
$ifc_param2 = $image->data->get($_object, "url");
$flag = $image->internal($_object);
```

If internal, shows filename instead of URL.

**Usage Example:**
Editing an existing image's name, URL, or category.

---

### Case: `_add`

Adds a new external image link.

```php
if ($_object = $image->link($ifc_param2, $ifc_param1, $ifc_param3)) {
    $object = language_set($object, $_object, $language);
    $ifc_response = CMS_MSG_DONE;
    break;
}
$ifc_response = CMS_MSG_ERROR;
```

**Parameters:**
- `$ifc_param2`: External URL
- `$ifc_param1`: Name
- `$ifc_param3`: Category

**Usage Example:**
Linking to an external image resource.

---

### Case: `_edit`

Edits an existing image entry.

```php
if ($_object = $image->set($_object, $ifc_param1, $ifc_param3, $flag ? $ifc_param4 : $ifc_param2)) {
    $object = language_set($object, $_object, $language);
    $ifc_response = CMS_MSG_DONE;
    break;
}
$ifc_response = CMS_MSG_ERROR;
```

**Parameters:**
- `$_object`: Index of image to edit
- `$ifc_param1`: New name
- `$ifc_param3`: New category
- `$ifc_param4` or `$ifc_param2`: New filename or URL depending on whether it's internal

**Usage Example:**
Updating an image's name or category after initial creation.

---

### Case: `replace`

Displays the replace form for internal images.

```php
if (! $image->internal($_object)) {
    $ifc_response = CMS_L_IFC_IMAGE_012;
    break;
}
$ifc->set(CMS_L_IFC_IMAGE_002, "file 40");
```

**Usage Example:**
Replacing an uploaded image file while keeping its metadata.

---

### Case: `_replace`

Handles the actual replacement of an internal image.

```php
if (isset($ifc_file1_name)) {
    $ifc_response = $image->replace($_object, $ifc_file1, $ifc_file1_name) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
}
$ifc_response = CMS_MSG_ERROR;
```

**Parameters:**
- `$_object`: Index of image to replace
- `$ifc_file1`: New uploaded file
- `$ifc_file1_name`: New filename

**Usage Example:**
Uploading a new version of an existing image.

---

### Case: `delete`

Deletes selected images and cleans up references.

```php
foreach ($_object AS $value)
    $flag_error &= ! $image->unlink($value);

// Remove invalid links from other languages
foreach ($array AS $value) {
    if (($_object = language_get($object, $value, TRUE)) === NULL) continue;
    if ($image->data->get($_object) === NULL) continue;
    ...
}
```

**Parameters:**
- `$_object`: Array of selected image indices

**Usage Example:**
Deleting one or more images from the system.

---

### Case: `category_rename`

Displays the category rename form.

```php
$ifc->set(CMS_L_NAME, "text 40 40", $category);
```

**Usage Example:**
Renaming a category associated with selected images.

---

### Case: `_category_rename`

Renames the category across all matching images.

```php
while ($key = $image->data->move("next")) {
    $_category = $image->data->get($key, "category") ?? "";
    if (streq($_category, $category))
        $image->data->set($ifc_param1, $key, "category");
}
$ifc_response = $image->data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

**Parameters:**
- `$ifc_param1`: New category name

**Usage Example:**
Renaming "Nature" to "Landscape" across all images tagged under "Nature".

---

### Case: `clear_cache`

Clears cached image thumbnails and processed versions.

```php
set_time_limit(600);
if (
    $image->operator &&
    cms_load("filemanager") &&
    ((! is_dir(CMS_DATA_PATH . "image/cache/")) || filemanager_delete(CMS_DATA_PATH . "image/cache/")) &&
    ((! is_dir(CMS_DATA_PATH . "#content/cache/")) || filemanager_delete(CMS_DATA_PATH . "#content/cache/"))
) {
    $ifc_response = CMS_MSG_DONE;
    break;
}
$ifc_response = CMS_MSG_ERROR;
```

**Usage Example:**
Admin clears all generated thumbnails to force regeneration.

---

### Case: `config`

Displays the configuration form for image processing options.

```php
$ifc->set(CMS_L_IFC_IMAGE_006, "label");
$ifc->set(["WebP" => "webp", "JPEG" => "jpg"], "select 40 b", $ifc_param1);
$ifc->set(CMS_L_IFC_IMAGE_018, "label");
$ifc->set([...], "select 40 b", $ifc_param2);
$ifc->set(CMS_L_IFC_IMAGE_005, "checkbox", TRUE, $ifc_param3);
```

**Parameters:**
- `$ifc_param1`: Preferred output format
- `$ifc_param2`: Maximum resolution
- `$ifc_param3`: Daemon processing flag

**Usage Example:**
Setting default compression format to WebP and max size to UHD-I.

---

### Case: `_config`

Saves the configuration values.

```php
if (! cms_permission(CMS_IMAGE_PERMISSION_OPERATOR)) break;
$system = new system();
$system->setval($ifc_param1, "image", "preference");
$system->setval($ifc_param2, "image", "resolution");
$system->setval($ifc_param3 ?? FALSE, "image", "daemon");
$ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

**Usage Example:**
Persisting admin-chosen image settings globally.

---

## Main Display Section

After handling messages, the interface renders the main UI:

### Object Selection Logic

```php
$_object = language_get($object, $language, TRUE);
$array = image_get_array();

if (nstre($_object) && in_array_recursive($_object, $array)) {
    $category = $image->data->get($_object, "category") ?? "";
    cms_cache("image." . CMS_USER . ".category", $category, TRUE);
} else {
    $category = cms_cache("image." . CMS_USER . ".category") ?? "";
    if (! isset($array[$category])) $category = "";
    if (is_array($array[$category])) {
        $_object = current($array[$category]);
        $object = language_set($object, $_object, $language);
    }
}
```

Determines which image/category to show based on current state or cache.

---

### Menu Construction

Builds dynamic menu items based on permissions and selected object:

```php
$menu = NULL;
if (CMS_IFC_SELECT && $_object)
    $menu[CMS_L_COMMAND_INSERT . "|image/command_apply"] = "javascript:ifc_return();";

if ($image->operator) {
    $menu[CMS_L_IFC_IMAGE_001 . "|image/command_upload"] = "upload";
    $menu[CMS_L_IFC_IMAGE_016 . "|image/command_upload_multi"] = "upload_multi";
    $menu[CMS_L_IFC_IMAGE_009 . "|image/command_add"] = "add";
    if ($_object) {
        $menu[CMS_L_COMMAND_EDIT . "|image/command_edit"] = "edit";
        $menu[CMS_L_IFC_IMAGE_008 . "|image/command_overwrite"] = "replace";
        $menu[CMS_L_COMMAND_DELETE . "|image/command_delete"] = "#delete";
        $menu[CMS_L_IFC_IMAGE_010 . "|image/command_rename"] = "category_rename";
    }
    $menu[CMS_L_IFC_IMAGE_011 . "|image/command_clear"] = "#clear_cache";
    $menu[CMS_L_IFC_IMAGE_003 . "|image/command_configuration"] = "config";
}
```

**Usage Example:**
Operators see full menu; non-operators only see basic insert option.

---

### JavaScript Integration

Includes client-side scripting for interactivity:

```javascript
function image_select(value) {
    var language = "<?php echo(q($language));?>";
    if (ifc_language_get(ifc_get("object"), language) === value) return;
    ifc_set("object", ifc_language_set(ifc_get("object"), value, language));
    document.getElementById("image-display").src = "...";
}
```

Updates the displayed image dynamically without reloading the page.

---

### HTML Rendering

Renders the category selector, image list, and display area:

```php
ifc_table_open();
echo("<colgroup>...</colgroup><tr><td>");
// Category dropdown
echo("<select ...>");
foreach ($array AS $key => $value) {
    ...
}
echo("</select>");

// Image list
if (is_array($array[$category])) {
    echo("<div id=\"_object\" ...>");
    foreach ($array[$category] AS $key => $value) {
        $_image = image_process(translate_url($value), 100, 100);
        echo("<label><input ...><image src=\"" . x($_image) . "\">" . x($key) . "</label>");
    }
    echo("</div>");
}

// Language selector
if (CMS_IFC_SELECT && CMS_LANGUAGE_ENABLED)
    language_selector($language, "...");

echo("</td><td class=\"iframe\"><iframe id=\"image-display\" src=\"...\">...</iframe></td></tr>");
ifc_table_close();
$ifc->close();
```

**Usage Example:**
Final rendered view shows categories on the left, thumbnails in the middle, and a live preview on the right.

---

## Summary

This interface file orchestrates the entire image management workflow in PWNC, providing a rich, interactive experience for administrators and content editors. It leverages core platform utilities such as:

- `cms_load()` for loading dependencies
- `cms_cache()` for persistent storage
- `language_set()` / `language_get()` for multilingual support
- `image_process()` for generating thumbnails
- `translate_url()` for resolving logical paths
- `ifc` class for building forms and menus
- `cms_permission()` for role-based access control

Developers can extend functionality by modifying the `image` library or adding new cases to the switch block.


<!-- HASH:dc0ad45b62c22672309a04a9531ff86c -->
