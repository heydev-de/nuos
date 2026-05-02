# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.search.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/module/%23interface/ifc.search.inc)

- **Version:** `26.5.2.2`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos-dev)

---

## Search Interface Module (`ifc.search.inc`)

**Overview**
This file implements the search interface for the NUOS web platform, providing administrative controls for managing search indices, queues, and configurations. It handles scanning URLs, processing queues, managing stopwords, filtering entries, and computing search scores. The interface interacts with the `search` class to perform operations and displays results using the NUOS IFC (Interface Control) system.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DB_SEARCH_ENTRY` | `CMS_DB_SEARCH_ENTRY` | Database table name for search entries. |
| `CMS_DB_SEARCH_ENTRY_INDEX` | `CMS_DB_SEARCH_ENTRY_INDEX` | Field name for the entry index. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS` | `CMS_DB_SEARCH_ENTRY_ADDRESS` | Field name for the entry URL/address. |
| `CMS_DB_SEARCH_ENTRY_TITLE` | `CMS_DB_SEARCH_ENTRY_TITLE` | Field name for the entry title. |
| `CMS_DB_SEARCH_ENTRY_TEXT` | `CMS_DB_SEARCH_ENTRY_TEXT` | Field name for the entry text content. |
| `CMS_DB_SEARCH_ENTRY_SCORE` | `CMS_DB_SEARCH_ENTRY_SCORE` | Field name for the entry search score. |
| `CMS_DB_SEARCH_ENTRY_TIME` | `CMS_DB_SEARCH_ENTRY_TIME` | Field name for the entry creation time. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | Field name for the entry last update time. |
| `CMS_DB_SEARCH_ENTRY_ERROR` | `CMS_DB_SEARCH_ENTRY_ERROR` | Field name for the entry error status. |
| `CMS_DB_SEARCH_ENTRY_CANONICAL` | `CMS_DB_SEARCH_ENTRY_CANONICAL` | Field name for the entry canonical flag. |
| `CMS_SEARCH_QUEUE_TYPE_ALL` | `CMS_SEARCH_QUEUE_TYPE_ALL` | Queue type constant for all entries. |
| `CMS_SEARCH_QUEUE_TYPE_INTERNAL` | `CMS_SEARCH_QUEUE_TYPE_INTERNAL` | Queue type constant for internal entries. |
| `CMS_SEARCH_QUEUE_TYPE_SELECTION` | `CMS_SEARCH_QUEUE_TYPE_SELECTION` | Queue type constant for selected entries. |
| `CMS_SEARCH_QUEUE_TYPE_SUBMISSION` | `CMS_SEARCH_QUEUE_TYPE_SUBMISSION` | Queue type constant for submitted entries. |
| `CMS_SEARCH_QUEUE_TYPE_REFERENCE` | `CMS_SEARCH_QUEUE_TYPE_REFERENCE` | Queue type constant for referenced entries. |
| `CMS_SEARCH_QUEUE_TYPE_UPDATE` | `CMS_SEARCH_QUEUE_TYPE_UPDATE` | Queue type constant for entries needing updates. |
| `CMS_SEARCH_SCAN_ERROR` | `CMS_SEARCH_SCAN_ERROR` | Bitmask flag for scan errors. |
| `CMS_SEARCH_SCAN_ADDRESS_REJECTED` | `CMS_SEARCH_SCAN_ADDRESS_REJECTED` | Bitmask flag for rejected addresses. |
| `CMS_SEARCH_SCAN_UNKNOWN_ERROR` | `CMS_SEARCH_SCAN_UNKNOWN_ERROR` | Bitmask flag for unknown errors. |
| `CMS_SEARCH_SCAN_DATABASE_ERROR` | `CMS_SEARCH_SCAN_DATABASE_ERROR` | Bitmask flag for database errors. |

---

## Interface Message Handling

The interface processes messages via `CMS_IFC_MESSAGE` to perform specific actions. Each case in the `switch` statement corresponds to a distinct operation.

---

### `case "scan"`

**Purpose**
Scans a given URL and indexes its content if valid. Reports success or failure with appropriate messages.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | URL to scan. |
| `$ifc_param2` | `bool` | Whether to follow links (optional). |

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Translates the URL for language support.
2. Calls `$search->scan()` to perform the scan.
3. Maps result codes to user-friendly messages.
4. Sets `$ifc_response` to indicate success or error.

**Usage Context**
- Triggered when an administrator manually scans a URL.
- Used for on-demand indexing of external or internal content.

---

### `case "queue_add"`

**Purpose**
Adds selected search entries to the processing queue.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of entry indices to add to the queue. |

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Queries the database for entry addresses matching the provided indices.
2. Calls `$search->queue_add()` for each address.
3. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used when an administrator selects entries from the interface and adds them to the queue for re-indexing.

---

### `case "queue_add_all"`

**Purpose**
Adds all search entries to the processing queue.

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Calls `$search->queue_add_all()`.
2. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used for bulk re-indexing of all entries.

---

### `case "queue_add_internal"`

**Purpose**
Adds all internal content entries (from the content directory) to the processing queue.

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Iterates over enabled languages.
2. Uses the `map` class to retrieve internal content URLs.
3. Calls `$search->queue_add()` for each URL.
4. Sets `$ifc_response` based on the count of added entries.

**Usage Context**
- Used to ensure all internal content is indexed or re-indexed.

---

### `case "queue_remove"`

**Purpose**
Removes a specific entry from the processing queue.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Entry address to remove from the queue. |

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Calls `$search->queue_remove()`.
2. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used to manually remove entries from the queue.

---

### `case "queue_process"`

**Purpose**
Displays a form to start processing the queue for a selected queue type.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Queue type to process (e.g., `CMS_SEARCH_QUEUE_TYPE_INTERNAL`). |
| `$ifc_param2` | `bool` | Whether to follow links (optional). |

**Return Values**
- `void` (Renders a form with JavaScript for queue processing.)

**Inner Mechanisms**
1. Displays a dropdown for selecting the queue type.
2. Provides a checkbox to enable link following.
3. Uses JavaScript to redirect to `_queue_process` with the selected parameters.

**Usage Context**
- Used to initiate queue processing from the administrative interface.

---

### `case "_queue_process"`

**Purpose**
Displays the queue processing interface with embedded threads for concurrent processing.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Queue type to process. |
| `$follow_links` | `bool` | Whether to follow links (default: `FALSE`). |

**Return Values**
- `void` (Renders a progress display with embedded thread objects.)

**Inner Mechanisms**
1. Displays the remaining queue length.
2. Embeds 4 HTML objects, each loading a `queue_process_thread` message for concurrent processing.

**Usage Context**
- Used to visualize and manage queue processing in real-time.

---

### `case "queue_process_thread"`

**Purpose**
Processes a single queue entry in a background thread and updates the progress display.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Queue type to process. |
| `$follow_links` | `bool` | Whether to follow links (default: `FALSE`). |

**Return Values**
- `void` (Outputs HTML and JavaScript to update the progress display.)

**Inner Mechanisms**
1. Calls `$search->queue_process()` to process a single entry.
2. Determines the result status and updates the display accordingly.
3. Uses JavaScript to reload the thread after a delay.
4. Updates the parent window's queue length display.

**Usage Context**
- Used internally by `_queue_process` to handle concurrent queue processing.

---

### `case "entry_remove"`

**Purpose**
Removes selected search entries from the database.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of entry indices to remove. |

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Calls `$search->entry_remove()` for each entry index.
2. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used to delete entries from the search index.

---

### `case "data_process"`

**Purpose**
Displays the interface for initiating data processing (e.g., score computation).

**Return Values**
- `void` (Renders a progress display with an embedded thread object.)

**Inner Mechanisms**
1. Embeds an HTML object loading `data_process_thread` to handle the processing.

**Usage Context**
- Used to start background data processing tasks.

---

### `case "data_process_thread"`

**Purpose**
Handles the multi-step process of computing search scores and canonical entries.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$action` | `int` | Current processing step (default: `0`). |
| `$iteration` | `int` | Current iteration for iterative steps. |

**Return Values**
- `void` (Outputs HTML and JavaScript to update the progress display.)

**Inner Mechanisms**
1. Uses a state machine to handle the following steps:
   - Initialize page scores.
   - Compute page scores iteratively.
   - Finalize page scores.
   - Integrate dangling links.
   - Find canonical entries.
2. Calls the corresponding `search` class methods for each step.
3. Updates the display with progress information.
4. Uses JavaScript to reload the thread for the next step.

**Usage Context**
- Used internally by `data_process` to handle background processing.

---

### `case "filter"`

**Purpose**
Displays a form for configuring search blacklists and whitelists.

**Return Values**
- `void` (Renders a form for editing blacklist and whitelist values.)

**Inner Mechanisms**
1. Retrieves current blacklist and whitelist values from the `system` class.
2. Displays a form with textareas for editing these values.

**Usage Context**
- Used to configure URL filtering rules for the search system.

---

### `case "_filter"`

**Purpose**
Saves the updated blacklist and whitelist values to the system configuration.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Blacklist value. |
| `$ifc_param2` | `string` | Whitelist value. |

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Saves the values to the `system` configuration under the `search` key.
2. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used to persist filter configurations after editing.

---

### `case "stopword"`

**Purpose**
Displays a form for editing stopwords for each enabled language.

**Return Values**
- `void` (Renders a form with a textarea for stopword configuration.)

**Inner Mechanisms**
1. Retrieves stopword configurations for all enabled languages.
2. Displays a form with a textarea for editing stopwords.

**Usage Context**
- Used to configure language-specific stopwords for search indexing.

---

### `case "_stopword"`

**Purpose**
Saves the updated stopword configurations for all languages.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Stopword values for all languages. |

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Parses the input to extract stopword values for each language.
2. Saves the values to the `data` class under the `#system/language` key.
3. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used to persist stopword configurations after editing.

---

### `case "clean"`

**Purpose**
Cleans the search index by removing invalid or outdated entries.

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Calls `$search->clean()`.
2. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used to maintain the integrity of the search index.

---

### `case "configuration"`

**Purpose**
Displays a form for configuring search daemon settings and entry parameters.

**Return Values**
- `void` (Renders a form for editing daemon and search settings.)

**Inner Mechanisms**
1. Retrieves the current daemon status and settings.
2. Displays checkboxes for enabling the daemon and selecting queue types.
3. Displays a dropdown for configuring the maximum bit difference for entries.

**Usage Context**
- Used to configure the search system's operational parameters.

---

### `case "_configuration"`

**Purpose**
Saves the updated search configuration settings.

**Parameters**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `bool` | Whether to enable the daemon. |
| `$ifc_param2` to `$ifc_param6` | `bool` | Queue type flags. |
| `$ifc_param7` | `bool` | Whether to follow links. |
| `$ifc_param8` | `int` | Maximum bit difference for entries. |

**Return Values**
- `void` (Outputs IFC response via `$ifc_response`.)

**Inner Mechanisms**
1. Combines queue type flags into a bitmask.
2. Calls `$search->daemon_status()` to update the daemon configuration.
3. Saves the maximum bit difference to the `system` configuration.
4. Sets `$ifc_response` based on success or failure.

**Usage Context**
- Used to persist configuration changes after editing.

---

## Main Display

**Purpose**
Renders the main search interface, including a menu, search filter, and entry list.

**Inner Mechanisms**
1. **Menu**: Displays a menu with options for queue management, data processing, filtering, and configuration.
2. **Search Filter**: Provides fields for filtering entries by field, condition, and value, as well as sorting and pagination controls.
3. **Entry List**: Retrieves and displays search entries from the database, with options for selection, pagination, and navigation.

**Usage Context**
- The primary interface for administrators to manage the search system.


<!-- HASH:e9af97332decc370676ab84e2e3cd292 -->
