# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.content.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/module/%23interface/ifc.content.inc)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Overview

This file (`ifc.content.inc`) serves as the primary interface for the NUOS Content Management System (CMS). It provides a comprehensive user interface for managing content, including creation, editing, publishing, versioning, and distribution. The interface integrates with multiple NUOS modules such as `content`, `content_pool`, `directory`, `document`, `flexview`, and `template`.

The file handles user permissions, content state management, and various content operations through a message-driven switch-case structure. It supports multilingual content, directory linking, scheduling, and RSS feed management.

---

## Functions and Message Handlers

### Message Handling / Sub Display

The following section documents the message handlers that drive the interface's functionality. Each case in the switch statement corresponds to a specific action or display mode.

---

### `case "select"`

**Purpose:**
Selects a content object and locates its topmost directory linkage.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index to select.             |

**Return Values:**
None (modifies `$object` and `$directory_object` in the global scope).

**Inner Mechanisms:**
1. Retrieves the directory data.
2. Iterates through directory entries to find a URL that starts with `content://$object`.
3. Sets `$directory_object` to the topmost directory entry that links to the content.

**Usage Context:**
Used when a content object is selected from the interface, updating the directory view to highlight the linked directory entry.

---

### `case "directory_select"`

**Purpose:**
Selects a directory object and resolves its linked content object.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Directory index to select.           |

**Return Values:**
None (modifies `$object`, `$user`, and `$directory_object` in the global scope).

**Inner Mechanisms:**
1. Retrieves the selected directory object and its URL.
2. Resolves directory dereferencing (handles nested `directory://` links).
3. For content links (`content://`), verifies the content owner and switches the user context if applicable.
4. Clears `$object` if no valid content link is found.

**Usage Context:**
Used when a directory entry is selected, updating the content view to reflect the linked content.

---

### `case "_meta"`

**Purpose:**
Updates the metadata (title, description, keywords, image, comment, and template) of a content object.

**Parameters:**
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$object`    | string | Content index.                       |
| `$ifc_param1` | string | Title.                               |
| `$ifc_param2` | string | Description.                         |
| `$ifc_param3` | string | Keywords.                            |
| `$ifc_param4` | string | Image path.                          |
| `$ifc_param5` | string | Comment.                             |
| `$ifc_param6` | string | Template index.                      |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Retrieves the current text of the content object.
2. Calls `content->update()` to save the new metadata.
3. Closes the external interface if `CMS_IFC_OPTION` is set to `"external"`.

**Usage Context:**
Used to save metadata changes for a content object.

---

### `case "meta"`

**Purpose:**
Displays a form for editing the metadata of a content object.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Retrieves the metadata of the content object.
2. Initializes an `ifc` (interface) object with a form for editing metadata.
3. Includes a JavaScript function for previewing the selected template.
4. Sets form fields for title, description, keywords, image, comment, and template.

**Usage Context:**
Used to display the metadata editing form for a content object.

---

### `case "edit_range"`

**Purpose:**
Displays a form for editing a text range within a content object.

**Parameters:**
| Name     | Type   | Description                          |
|----------|--------|--------------------------------------|
| `$object` | string | Content index.                       |
| `$range`  | string | Range identifier within the content. |
| `$id`     | string | DOM ID of the range element.         |

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Initializes an `ifc` object with a text editor for the specified range.
2. Includes a JavaScript function to save the edited range back to the parent window.

**Usage Context:**
Used to edit a specific text range within a content object.

---

### `case "edit_value"`

**Purpose:**
Displays a form for editing a value range within a content object.

**Parameters:**
| Name     | Type   | Description                          |
|----------|--------|--------------------------------------|
| `$object` | string | Content index.                       |
| `$range`  | string | Range identifier within the content. |
| `$id`     | string | DOM ID of the range element.         |

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Initializes an `ifc` object with a textarea for the specified value range.
2. Includes a JavaScript function to save the edited value back to the parent window.

**Usage Context:**
Used to edit a specific value range within a content object.

---

### `case "edit_plugin"`

**Purpose:**
Displays a form for editing a plugin URL within a content object.

**Parameters:**
| Name     | Type   | Description                          |
|----------|--------|--------------------------------------|
| `$object` | string | Content index.                       |
| `$range`  | string | Range identifier within the content. |
| `$id`     | string | DOM ID of the range element.         |

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Initializes an `ifc` object with a text input for the plugin URL.
2. Includes a JavaScript function to save the edited URL back to the parent window.

**Usage Context:**
Used to edit a plugin URL within a content object.

---

### `case "edit_href"`, `case "edit_href_select_language"`

**Purpose:**
Displays a form for editing a hyperlink (href) within a content object, supporting multilingual URLs.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$object`  | string | Content index.                       |
| `$range`   | string | Range identifier within the content. |
| `$language` | string | Language code for the href.          |
| `$value`   | string | Current href value.                  |
| `$id`      | string | DOM ID of the range element.         |

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Initializes an `ifc` object with form fields for editing the href.
2. Includes JavaScript functions for selecting and saving href values.
3. Displays dropdowns for directory and publication links, and a text input for custom URLs.
4. Supports language selection for multilingual hrefs.

**Usage Context:**
Used to edit hyperlinks within a content object, with support for multilingual URLs.

---

### `case "create"`

**Purpose:**
Displays a form for creating a new content object.

**Parameters:**
None.

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Initializes an `ifc` object with a form for creating a new content object.
2. Includes a JavaScript function for previewing the selected template.
3. Sets default values for title, template, and comment.

**Usage Context:**
Used to create a new content object.

---

### `case "_create"`

**Purpose:**
Creates a new content object with the provided metadata.

**Parameters:**
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | string | Title.                               |
| `$ifc_param2` | string | Template index.                      |
| `$ifc_param3` | string | Comment.                             |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->create()` to create the new content object.
2. Updates `$object` with the new content index on success.
3. Caches the selected template for future use.

**Usage Context:**
Used to save a newly created content object.

---

### `case "display"`

**Purpose:**
Displays the parsed content of a content object.

**Parameters:**
| Name     | Type   | Description                          |
|----------|--------|--------------------------------------|
| `$object` | string | Content index.                       |

**Return Values:**
None (outputs the parsed content and exits).

**Inner Mechanisms:**
1. Calls `content_parse()` to parse and display the content.
2. Exits the script to prevent further output.

**Usage Context:**
Used to display the final rendered content of an object.

---

### `case "apply"`

**Purpose:**
Displays a form for applying changes to a content object, with options for immediate or scheduled application.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Retrieves the scheduled application time for the content object.
2. Initializes an `ifc` object with options for immediate or scheduled application.
3. Includes form fields for selecting the application date and time.

**Usage Context:**
Used to apply changes to a content object, either immediately or at a scheduled time.

---

### `case "_apply"`

**Purpose:**
Applies changes to a content object based on the selected option.

**Parameters:**
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | int    | Application option (0: immediate, 1: scheduled, 2: remove schedule). |
| `$ifc_param2` - `$ifc_param6` | int | Day, month, year, hour, and minute for scheduled application. |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->apply()` with the appropriate parameters based on the selected option.
2. Handles immediate application, scheduled application, and schedule removal.

**Usage Context:**
Used to execute the application of changes to a content object.

---

### `case "apply_all"`

**Purpose:**
Applies changes to all content objects with pending edits.

**Parameters:**
None.

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Retrieves all content objects with pending edits.
2. Calls `content->apply()` for each object.

**Usage Context:**
Used to apply changes to all content objects with pending edits in one operation.

---

### `case "revert"`

**Purpose:**
Reverts a content object to its last applied state.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->revert()` to revert the content object.

**Usage Context:**
Used to discard pending edits and revert a content object to its last applied state.

---

### `case "authorize"`

**Purpose:**
Displays a form for authorizing a content object (approving it for publication).

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
None (outputs an interface form).

**Inner Mechanisms:**
1. Retrieves the editor's comment for the content object.
2. Initializes an `ifc` object with a textarea for adding an authorization comment.

**Usage Context:**
Used to authorize a content object for publication.

---

### `case "_authorize"`

**Purpose:**
Authorizes a content object with the provided comment.

**Parameters:**
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$object`    | string | Content index.                       |
| `$ifc_param1` | string | Authorization comment.               |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->authorize()` to authorize the content object.

**Usage Context:**
Used to save the authorization of a content object.

---

### `case "derive_draft"`

**Purpose:**
Creates a draft copy of a content object.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->derive_draft()` to create a draft copy of the content object.

**Usage Context:**
Used to create a draft copy of a content object for further editing.

---

### `case "publish"`, `case "_publish"`, `case "__publish"`, `case "___publish"`, `case "publish_replace"`, `case "publish_insert"`, `case "publish_append"`

**Purpose:**
Handles the publication of a content object, including scheduling and directory linking.

**Parameters:**
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$object`    | string | Content index.                       |
| `$ifc_param` - `$ifc_param14` | mixed | Publication parameters (title, time, directory index, etc.). |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Retrieves the publisher's comment and title for the content object.
2. Displays a form for selecting publication options (immediate or scheduled) and directory linking.
3. Handles the publication process based on the selected options.

**Usage Context:**
Used to publish a content object, with options for scheduling and linking to a directory.

---

### `case "withdraw"`

**Purpose:**
Withdraws a published content object.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->withdraw()` to withdraw the content object.

**Usage Context:**
Used to withdraw a published content object.

---

### `case "duplicate"`

**Purpose:**
Creates a duplicate of a content object.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->duplicate()` to create a duplicate of the content object.
2. Updates `$object` with the new content index on success.

**Usage Context:**
Used to create a duplicate of a content object.

---

### `case "copy"`

**Purpose:**
Creates a copy of a content object.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->copy()` to create a copy of the content object.
2. Updates `$object` with the new content index on success.

**Usage Context:**
Used to create a copy of a content object.

---

### `case "send"`, `case "_send"`

**Purpose:**
Displays a form for sending a content object to other users, with options for original, duplicate, or copy.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Retrieves the content object's type, status, and author.
2. Determines the list of users who can receive the content.
3. Displays a form for selecting recipients and sending the content.
4. Handles the sending process for originals, duplicates, and copies.

**Usage Context:**
Used to send a content object to other users.

---

### `case "delete"`

**Purpose:**
Deletes selected content objects.

**Parameters:**
| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| `$list` | array  | Array of content indices to delete.  |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Calls `content->delete()` for each content index in `$list`.

**Usage Context:**
Used to delete one or more content objects.

---

### `case "template_preview"`

**Purpose:**
Displays a preview of a template.

**Parameters:**
| Name     | Type   | Description                          |
|----------|--------|--------------------------------------|
| `$object` | string | Template index.                      |

**Return Values:**
None (outputs the template preview and exits).

**Inner Mechanisms:**
1. Calls `template_preview()` to display the template preview.
2. Exits the script to prevent further output.

**Usage Context:**
Used to preview a template.

---

### `case "clear_cache"`

**Purpose:**
Clears the content cache.

**Parameters:**
None.

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. Verifies the user has publisher permissions.
2. Calls `cms_cache_clean()` to clear the content cache.

**Usage Context:**
Used to clear the content cache, typically for performance or consistency reasons.

---

### `case "version"`, `case "version_display"`, `case "version_store"`, `case "version_retrieve"`, `case "_version_retrieve"`

**Purpose:**
Handles versioning of content objects, including storing, retrieving, and displaying versions.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content or version index.            |
| `$version`  | string | Version index.                       |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. **`version`**: Displays a list of versions for a content object.
2. **`version_display`**: Displays the parsed content of a specific version.
3. **`version_store`**: Stores a new version of a content object.
4. **`version_retrieve`**: Displays a form for retrieving a version, with options for immediate or scheduled retrieval.
5. **`_version_retrieve`**: Retrieves a version of a content object based on the selected option.

**Usage Context:**
Used to manage versioning of content objects, including storing, retrieving, and displaying versions.

---

### `case "schedule"`, `case "schedule_save"`, `case "schedule_delete"`

**Purpose:**
Handles the scheduling of content operations (apply, retrieve, publish, withdraw).

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |
| `$list`     | array  | Array of schedule hashes to delete.  |
| `$time`     | array  | Array of time values to update.      |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. **`schedule`**: Displays a list of scheduled operations for a content object.
2. **`schedule_save`**: Updates the scheduled times for selected operations.
3. **`schedule_delete`**: Deletes selected scheduled operations.

**Usage Context:**
Used to manage scheduled operations for content objects.

---

### `case "pool"`, `case "pool_select"`, `case "pool_select_language"`, `case "pool_display"`, `case "pool_buffer"`, `case "pool_add"`, `case "_pool_add"`, `case "pool_edit"`, `case "_pool_edit"`, `case "pool_source"`, `case "pool_delete"`, `case "pool_category_rename"`, `case "_pool_category_rename"`, `case "pool_synchronize_all"`

**Purpose:**
Handles the content pool, which stores reusable content fragments.

**Parameters:**
| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `$pool_object`  | string | Pool object index.                   |
| `$pool_type`    | string | Type of pool object.                 |
| `$pool_language` | string | Language code for the pool object.   |
| `$ifc_param`    | string | Pool object index or category.       |
| `$ifc_param1`   | string | Name or category for the pool object.|
| `$ifc_param2`   | string | Category for the pool object.        |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. **`pool`**: Displays the content pool interface.
2. **`pool_select`**: Selects a pool object.
3. **`pool_select_language`**: Selects a language for the pool object.
4. **`pool_display`**: Displays the parsed content of a pool object.
5. **`pool_buffer`**: Caches the text of a pool object.
6. **`pool_add`**: Displays a form for adding a new pool object.
7. **`_pool_add`**: Adds a new pool object.
8. **`pool_edit`**: Displays a form for editing a pool object.
9. **`_pool_edit`**: Saves edits to a pool object.
10. **`pool_source`**: Redirects to the source of a pool object.
11. **`pool_delete`**: Deletes selected pool objects.
12. **`pool_category_rename`**: Displays a form for renaming a pool category.
13. **`_pool_category_rename`**: Renames a pool category.
14. **`pool_synchronize_all`**: Synchronizes all pool objects.

**Usage Context:**
Used to manage reusable content fragments in the content pool.

---

### `case "rss"`, `case "rss_select"`, `case "rss_add"`, `case "rss_save"`, `case "rss_delete"`, `case "rss_default"`, `case "rss_assign"`, `case "_rss_assign"`

**Purpose:**
Handles RSS feed management, including channel creation, editing, and assignment to content objects.

**Parameters:**
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$rss_object` | string | RSS channel index.                   |
| `$ifc_param`  | string | RSS channel index or content index.  |
| `$ifc_param1` - `$ifc_param6` | string | RSS channel parameters (name, description, link, image, category, default). |
| `$_list`      | array  | Array of RSS channel indices.        |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. **`rss`**: Displays the RSS feed management interface.
2. **`rss_select`**: Selects an RSS channel.
3. **`rss_add`**: Adds a new RSS channel.
4. **`rss_save`**: Saves changes to an RSS channel.
5. **`rss_delete`**: Deletes selected RSS channels.
6. **`rss_default`**: Sets selected RSS channels as default.
7. **`rss_assign`**: Displays a form for assigning RSS channels to a content object.
8. **`_rss_assign`**: Assigns selected RSS channels to a content object.

**Usage Context:**
Used to manage RSS feeds, including channel creation, editing, and assignment to content objects.

---

### `case "configuration"`, `case "_configuration"`

**Purpose:**
Handles the configuration of content module settings, such as extra column labels and IDs.

**Parameters:**
| Name         | Type   | Description                          |
|--------------|--------|--------------------------------------|
| `$ifc_param1` | string | Extra column label.                  |
| `$ifc_param2` | string | Extra column ID.                     |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. **`configuration`**: Displays a form for configuring content module settings.
2. **`_configuration`**: Saves the configured settings.

**Usage Context:**
Used to configure additional settings for the content module.

---

### `case "flag"`, `case "_flag"`

**Purpose:**
Handles the setting of flags for a content object, such as sitemap exclusion and meta robots directives.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Content index.                       |
| `$ifc_param1` - `$ifc_param3` | bool | Flag settings (sitemap exclude, noindex, nofollow). |

**Return Values:**
- Sets `$ifc_response` to `CMS_MSG_DONE` on success.
- Sets `$ifc_response` to `CMS_MSG_ERROR` on failure.

**Inner Mechanisms:**
1. **`flag`**: Displays a form for setting flags on a content object.
2. **`_flag`**: Saves the flag settings for a content object.

**Usage Context:**
Used to set flags for a content object, such as excluding it from the sitemap or setting meta robots directives.

---

### `case "analyze"`, `case "analyze_span"`

**Purpose:**
Analyzes the content of a content object, providing insights such as word count, markup quota, and word span occurrences.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$object`  | string | Content index.                       |
| `$ifc_param` | int   | Word span size for analysis.         |

**Return Values:**
None (outputs an analysis interface).

**Inner Mechanisms:**
1. Retrieves the content object's metadata and text.
2. Parses the content and calculates metrics such as word count, markup quota, and word span occurrences.
3. Displays the analysis in a tabbed interface.

**Usage Context:**
Used to analyze the content of a content object for SEO and readability insights.

---

### `case "debug"`

**Purpose:**
Displays a debug view of a content object's document structure.

**Parameters:**
| Name     | Type   | Description                          |
|----------|--------|--------------------------------------|
| `$object` | string | Content index.                       |

**Return Values:**
None (outputs a debug interface).

**Inner Mechanisms:**
1. Retrieves the content object's text and template.
2. Parses the document and displays its structure in a tree view.

**Usage Context:**
Used for debugging the structure of a content object's document.

---

### `case "sql_select"`, `case "sql_order"`, `case "page"`

**Purpose:**
Handles the selection of content object filters, sorting, and pagination.

**Parameters:**
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | SQL select filter, order field, or page number. |

**Return Values:**
None (modifies `$sql_select`, `$sql_order`, or `$page` in the global scope).

**Inner Mechanisms:**
1. **`sql_select`**: Updates the SQL select filter.
2. **`sql_order`**: Toggles the SQL order field between ascending and descending.
3. **`page`**: Updates the current page number for pagination.

**Usage Context:**
Used to update the content object list filters, sorting, and pagination.

---

## Main Display

The main display section renders the primary interface for the content module, including:

1. **User and Object Type Selection**: Dropdowns for selecting the user and content object type.
2. **Object Filter**: Inputs for filtering content objects by field and value.
3. **Directory View**: A tree view of the directory structure, highlighting linked content objects.
4. **Selected Object Information**: Displays detailed information about the selected content object.
5. **Object List**: A paginated list of content objects with controls for editing, publishing, and other operations.

**Inner Mechanisms:**
- **Icon and Status Mapping**: Maps content status and type to icons and labels.
- **Menu Generation**: Generates a context-sensitive menu based on user permissions.
- **Directory Preparation**: Resolves directory linkages and highlights the selected directory and content objects.
- **Object List Query**: Generates and executes a query to retrieve the list of content objects based on the selected filters.
- **Pagination**: Handles pagination for large result sets.
- **JavaScript Functions**: Includes JavaScript functions for interacting with the interface (e.g., selecting, editing, publishing).

**Usage Context:**
The main display is the primary interface for managing content objects, providing access to all content operations and views.


<!-- HASH:579a0a4efd85c2ca8e5d50d4951db4cb -->
