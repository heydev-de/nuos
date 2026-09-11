# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.search.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Search Interface

The `ifc.search.inc` file implements the administrative interface for managing the search functionality within the PWNC Web Platform. It provides a comprehensive control panel for configuring, monitoring, and maintaining the search system, including queue management, data processing, filtering, stopwords, and daemon configuration.

### Overview

This interface file is loaded when a user accesses the search management section of the admin panel. It handles various operations through a message-based dispatch system (`CMS_IFC_MESSAGE`), where each case corresponds to a specific action such as scanning URLs, processing queues, configuring settings, or displaying the main search dashboard.

The interface relies heavily on the `search` class (loaded via `cms_load("search")`) and interacts with the database using PWNC's MySQL wrappers. It also uses the `ifc` class for rendering forms and UI components, and various utility functions for URL generation, escaping, and localization.

---

## Message Handling / Sub Display

### `scan`

Handles the scanning of a single URL to be indexed by the search engine.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | The URL to scan, resolved via `translate_url()` |
| `$ifc_param2` | boolean (optional) | Whether to follow links during the scan |

**Return Value:** Sets `$ifc_response` to either `CMS_MSG_DONE` or `CMS_MSG_ERROR` with an appropriate message based on the scan result.

**Mechanism:** Calls `$search->scan()` with the translated URL and link-following flag. Maps the returned status code to a localized message using a predefined array.

**Usage Example:**
```php
// Triggered when CMS_IFC_MESSAGE == "scan"
// Scans a URL and reports success/failure
```

---

### `queue_add`

Adds selected entries to the search processing queue.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | array | List of entry indices to add to the queue |

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Queries the database for addresses corresponding to the selected entry indices, then calls `$search->queue_add()` for each address with the selection queue type.

**Usage Example:**
```php
// Adds selected search entries to the queue for processing
```

---

### `queue_add_all`

Adds all eligible entries to the search processing queue.

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Delegates to `$search->queue_add_all()`.

**Usage Example:**
```php
// Adds all internal content to the search queue
```

---

### `queue_add_internal`

Adds all internal content entries to the search queue, organized by language.

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Iterates over enabled languages, loads content maps for each language's directory, retrieves value lists, and queues each entry with the internal queue type.

**Usage Example:**
```php
// Queues all internal content for indexing, grouped by language
```

---

### `queue_remove`

Removes a specific entry from the search queue.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Identifier of the queue entry to remove |

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Calls `$search->queue_remove()`.

**Usage Example:**
```php
// Removes a specific item from the search queue
```

---

### `queue_process`

Displays the queue processing interface with options to select queue types and follow links.

**Mechanism:** Renders a form with a dropdown for queue type selection and a checkbox for following links. Includes JavaScript to initiate the processing.

**Usage Example:**
```php
// Displays UI for starting queue processing
```

---

### `_queue_process`

Processes the search queue in background threads.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | string | Queue type to process |
| `$follow_links` | boolean | Whether to follow links during processing |

**Mechanism:** Spawns multiple `<object>` elements (threads) that load `queue_process_thread`. Each thread processes a portion of the queue and reloads itself until completion.

**Usage Example:**
```php
// Processes the search queue in parallel threads
```

---

### `queue_process_thread`

Background thread that processes individual items from the search queue.

**Mechanism:** Outputs an HTML page with a loader element. Calls `$search->queue_process()` to handle the next queue item. Updates the parent window's remaining count and reloads itself with a delay based on the result.

**Usage Example:**
```php
// Background worker that processes one queue item at a time
```

---

### `entry_remove`

Removes selected search entries from the index.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | array | List of entry indices to remove |

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Iterates over the list and calls `$search->entry_remove()` for each entry.

**Usage Example:**
```php
// Removes selected entries from the search index
```

---

### `data_process`

Displays the data processing interface for computing page scores and integrating links.

**Mechanism:** Renders a container with an embedded thread object that loads `data_process_thread`.

**Usage Example:**
```php
// Shows UI for initiating data processing
```

---

### `data_process_thread`

Background thread that performs iterative data processing tasks.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$action` | int | Current processing step (0-5) |
| `$iteration` | int | Current iteration number |

**Mechanism:** Executes a state machine with 6 steps:
1. Initialize page score
2. Compute page score (iterative)
3. Finalize page score
4. Integrate dangling links
5. Find canonical entries
6. Complete

Each step calls the appropriate method on the search object and reloads itself with updated parameters.

**Usage Example:**
```php
// Background worker that computes page scores iteratively
```

---

### `filter`

Displays the search filter configuration form.

**Mechanism:** Loads blacklist and whitelist values from the system settings and renders a form with textareas for editing them.

**Usage Example:**
```php
// Shows form for configuring search filters
```

---

### `_filter`

Saves the search filter configuration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Blacklist content |
| `$ifc_param2` | string | Whitelist content |

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Updates system settings with the provided blacklist and whitelist values, then saves.

**Usage Example:**
```php
// Saves updated search filter settings
```

---

### `stopword`

Displays the stopword configuration form for all enabled languages.

**Mechanism:** Loads language data, retrieves stopword arrays for each language, and renders a form with a textarea for editing stopwords.

**Usage Example:**
```php
// Shows form for configuring search stopwords per language
```

---

### `_stopword`

Saves the stopword configuration.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Serialized stopword data |

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Parses the stopword data and updates each language's stopword setting, then saves.

**Usage Example:**
```php
// Saves updated stopword configuration
```

---

### `clean`

Cleans up the search index.

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Calls `$search->clean()`.

**Usage Example:**
```php
// Cleans up orphaned or invalid search entries
```

---

### `reset`

Resets the user's search filter and display settings.

**Mechanism:** Deletes cached user-specific search settings (filter field, option, value, order, page, limit).

**Usage Example:**
```php
// Resets search interface to default state
```

---

### `configuration`

Displays the search daemon and configuration settings.

**Mechanism:** Retrieves daemon status and renders a form with checkboxes for enabling the daemon, selecting queue types, enabling link following, and setting the maximum bit difference for fuzzy matching.

**Usage Example:**
```php
// Shows form for configuring search daemon settings
```

---

### `_configuration`

Saves the search daemon and configuration settings.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | boolean | Enable/disable daemon |
| `$ifc_param2-6` | boolean | Queue type flags (internal, selection, submission, reference, update) |
| `$ifc_param7` | boolean | Follow links flag |
| `$ifc_param8` | int | Maximum bit difference |

**Return Value:** Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Mechanism:** Updates daemon status, queue types, link following, and maximum bit difference setting.

**Usage Example:**
```php
// Saves updated search daemon configuration
```

---

## Main Display

### Filter Option List

Defines SQL LIKE patterns for search filtering:

| Index | Pattern | Description |
|-------|---------|-------------|
| 0 | ` LIKE '#value#%'` | Starts with |
| 1 | ` LIKE '%#value#%'` | Contains |
| 2 | ` LIKE '%#value#'` | Ends with |
| 3 | `>'#value#'` | Greater than |
| 4 | `='#value#'` | Equal to |
| 5 | `<'#value#'` | Less than |

### Order List

Defines sorting options for search results:

| Index | Column | Description |
|-------|--------|-------------|
| 0 | `CMS_DB_SEARCH_ENTRY_ADDRESS` | Address ascending |
| 1 | `CMS_DB_SEARCH_ENTRY_ADDRESS DESC` | Address descending |
| 2 | `CMS_DB_SEARCH_ENTRY_TITLE` | Title ascending |
| 3 | `CMS_DB_SEARCH_ENTRY_TITLE DESC` | Title descending |
| 4 | `CMS_DB_SEARCH_ENTRY_SCORE` | Score ascending |
| 5 | `CMS_DB_SEARCH_ENTRY_SCORE DESC` | Score descending |
| 6 | `CMS_DB_SEARCH_ENTRY_TIME` | Time ascending |
| 7 | `CMS_DB_SEARCH_ENTRY_TIME DESC` | Time descending |
| 8 | `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | Update time ascending |
| 9 | `CMS_DB_SEARCH_ENTRY_UPDATE_TIME DESC` | Update time descending |

### Cache Sync

Synchronizes user-specific search settings with cached values:

| Variable | Cache Key | Default |
|----------|-----------|---------|
| `$filter_field` | `search.{user}.filter_field` | `CMS_DB_SEARCH_ENTRY_ADDRESS` |
| `$filter_option` | `search.{user}.filter_option` | `1` |
| `$filter_value` | `search.{user}.filter_value` | `""` |
| `$page` | `search.{user}.page` | `0` |
| `$limit` | `search.{user}.limit` | `25` |
| `$order` | `search.{user}.order` | `7` |

### Menu

Defines the navigation menu for the search interface:

| Label | Action |
|-------|--------|
| Queue Add | `queue_add` |
| Queue Add Internal | `queue_add_internal` |
| Queue Add All | `queue_add_all` |
| Delete Selected | `#entry_remove` |
| Data Process | `javascript:load_page(...)` |
| Filter | `filter` |
| Stopword | `stopword` |
| Clean | `#clean` |
| Configuration | `configuration` |

### JavaScript Functions

#### `qp(type)`

Initiates queue processing for the specified type.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | string | Queue type to process |

**Mechanism:** Constructs a URL with the queue type and loads it via `load_page()`.

**Usage Example:**
```javascript
qp('all'); // Start processing all queue types
```

#### `qr(type)`

Prompts for confirmation and removes a queue type.

| Parameter | Type | Description |
|-----------|------|-------------|
| `type` | string | Queue type to remove |

**Mechanism:** Shows a confirmation dialog, then posts to `queue_remove` with the type.

**Usage Example:**
```javascript
qr('internal'); // Remove internal queue items
```

#### `o(value)`

Sets the order parameter and submits the form.

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | int | Order index |

**Mechanism:** Sets the `order` field and triggers form submission.

**Usage Example:**
```javascript
o(5); // Sort by score descending
```

#### `p(number)`

Sets the page number and submits the form.

| Parameter | Type | Description |
|-----------|------|-------------|
| `number` | int | Page number |

**Mechanism:** Sets the `page` field and triggers form submission.

**Usage Example:**
```javascript
p(2); // Go to page 2
```

### Helper Functions

#### `$f1($order, $asc, $dsc)`

Returns a sort direction indicator.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$order` | int | Current order index |
| `$asc` | int | Ascending order index |
| `$dsc` | int | Descending order index |

**Return Value:** Returns `" ↗"` for ascending, `" ↘"` for descending, or empty string.

**Usage Example:**
```php
$f1(0, 0, 1); // Returns " ↗" if order is 0
```

#### `$f2($order, $asc, $dsc)`

Returns a JavaScript function call for toggling sort order.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$order` | int | Current order index |
| `$asc` | int | Ascending order index |
| `$dsc` | int | Descending order index |

**Return Value:** Returns a JavaScript call string to toggle between ascending and descending.

**Usage Example:**
```php
$f2(0, 0, 1); // Returns "javascript:o(1);" if order is 0
```

### Search Results Table

Displays search entries with the following columns:

| Column | Description |
|--------|-------------|
| Selection | Checkbox for batch operations |
| Address | URL with icon indicating status |
| Title | Page title |
| Score | Relevance score |
| Time | Indexing time |
| Update Time | Last update time |

### Selection Controls

Provides buttons for managing row selections:

| Button | Action |
|--------|--------|
| All | Select all rows |
| Invert | Invert selection |
| None | Deselect all rows |


<!-- HASH:43106a72789427d7441835bb066eab03 -->
