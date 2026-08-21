# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.search.inc)

- **Version:** `26.8.14.2`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Search Interface Module (`ifc.search.inc`)

This file implements the **Search Interface** for the PWNC Web Platform, providing a user interface for managing and interacting with the search subsystem. It handles:

- **Search index management** (scanning, queueing, processing)
- **Data processing** (score computation, canonical entry detection)
- **Filtering and configuration** (blacklists, whitelists, stopwords)
- **User interaction** (displaying search entries, pagination, sorting)

The interface relies on the `search` class (loaded via `cms_load("search")`) and integrates with the PWNC IFC (Interface Controller) system to handle user actions and display results.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_SEARCH_SCAN_*` | Various | Status codes returned by `search->scan()` indicating success, errors, or specific conditions. |
| `CMS_SEARCH_QUEUE_TYPE_*` | Bitmask values | Types of queue entries (e.g., `INTERNAL`, `SELECTION`, `SUBMISSION`). |
| `CMS_DB_SEARCH_ENTRY_*` | Column names | Database columns for the search entry table. |

---

## Interface Workflow

### Initialization
```php
if (! cms_load("search")) ifc_inactive($ifc_page);
ifc_permission(["" => CMS_L_ACCESS]);
$search = new search();
if (! $search->enabled) ifc_inactive($ifc_page);
init($object);
```
- **Purpose**: Loads the search module, checks permissions, and initializes the search object.
- **Usage**: Automatically executed when the interface is accessed.

---

## Message Handling

The interface processes user actions via `CMS_IFC_MESSAGE`. Each case corresponds to a specific action.

---

### `scan`
```php
case "scan":
    $array = [...];
    $ifc_param1 = translate_url($ifc_param1, NULL, CMS_LANGUAGE, TRUE);
    $result = $search->scan($ifc_param1, isset($ifc_param2));
    $ifc_response = (CMS_SEARCH_SCAN_ERROR & $result) ? CMS_MSG_ERROR : CMS_MSG_DONE;
    if (isset($array[$result])) $ifc_response .= $array[$result];
    break;
```
- **Purpose**: Scans a URL for indexing.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$ifc_param1` | `string` | URL to scan. |
  | `$ifc_param2` | `bool` | Whether to follow links (optional). |
- **Return**: Status message (success/error) appended to `$ifc_response`.
- **Inner Mechanisms**:
  - Uses `translate_url()` to resolve logical URLs.
  - Calls `search->scan()` and maps result codes to user-friendly messages.
- **Usage Example**:
  ```php
  // Scan a URL for indexing
  $result = $search->scan("https://example.com", true);
  ```

---

### `queue_add`
```php
case "queue_add":
    if (blank($list)) break;
    $error = FALSE;
    $result = mysql_query("SELECT " . CMS_DB_SEARCH_ENTRY_ADDRESS . " FROM " . CMS_DB_SEARCH_ENTRY . " WHERE " . CMS_DB_SEARCH_ENTRY_INDEX . " IN ('" . implode("', '", sqlesc($list)) . "')");
    while ($resultrow = mysql_fetch_row($result))
        $error |= ! $search->queue_add($resultrow[0], CMS_SEARCH_QUEUE_TYPE_SELECTION);
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;
```
- **Purpose**: Adds selected entries to the processing queue.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$list` | `array` | Array of entry indices to queue. |
- **Return**: Status message (`CMS_MSG_ERROR` or `CMS_MSG_DONE`).
- **Inner Mechanisms**:
  - Fetches addresses for selected indices.
  - Adds each address to the queue with type `SELECTION`.
- **Usage Example**:
  ```php
  // Add entries with indices 1, 2, 3 to the queue
  $list = [1, 2, 3];
  $search->queue_add("https://example.com", CMS_SEARCH_QUEUE_TYPE_SELECTION);
  ```

---

### `queue_add_all`
```php
case "queue_add_all":
    $ifc_response = $search->queue_add_all() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Adds all entries to the processing queue.
- **Return**: Status message (`CMS_MSG_ERROR` or `CMS_MSG_DONE`).
- **Usage Example**:
  ```php
  // Queue all entries for processing
  $search->queue_add_all();
  ```

---

### `queue_add_internal`
```php
case "queue_add_internal":
    $language = explode(",", CMS_LANGUAGE_ENABLED);
    $count = 0;
    foreach ($language AS $value) {
        $value = stre($value) ? "" : "$value.";
        $map = new map("#system/" . $value . "directory.content");
        $list = $map->get_value_list();
        foreach ($list AS $_value)
            $count += $search->queue_add($_value, CMS_SEARCH_QUEUE_TYPE_INTERNAL);
    };
    $ifc_response = $count ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Adds all internal content entries to the queue.
- **Return**: Status message (`CMS_MSG_ERROR` or `CMS_MSG_DONE`).
- **Inner Mechanisms**:
  - Iterates over enabled languages.
  - Uses `map` to fetch content entries and adds them to the queue.
- **Usage Example**:
  ```php
  // Queue all internal content for processing
  $search->queue_add("content://home", CMS_SEARCH_QUEUE_TYPE_INTERNAL);
  ```

---

### `queue_process` and `_queue_process`
```php
case "queue_process":
    // Display UI for selecting queue type and follow-links option
    $ifc = new ifc(NULL, NULL, NULL);
    // ... (UI setup)
    $ifc->close();
    break;

case "_queue_process":
    // Display queue processing status and spawn threads
    $array = [...];
    $follow_links = $follow_links ?? FALSE;
    $ifc = new ifc(NULL, NULL, NULL);
    echo("<div>...</div>");
    $ifc->close();
    break;
```
- **Purpose**: Initiates queue processing and displays progress.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$ifc_param` | `int` | Queue type (e.g., `CMS_SEARCH_QUEUE_TYPE_ALL`). |
  | `$follow_links` | `bool` | Whether to follow links during processing. |
- **Return**: HTML output for the processing interface.
- **Inner Mechanisms**:
  - Spawns 4 threads (via `<object>` tags) to process the queue in parallel.
  - Updates the UI with remaining queue length.
- **Usage Example**:
  ```php
  // Process the queue for internal content
  $search->queue_process(CMS_SEARCH_QUEUE_TYPE_INTERNAL, true);
  ```

---

### `queue_process_thread`
```php
case "queue_process_thread":
    echo(CMS_DOCTYPE_HTML . "<html>...</html>");
    force_flush();
    $value = $search->queue_process($ifc_param, $follow_links);
    // ... (Update UI based on result)
    exit();
```
- **Purpose**: Processes a single queue entry in a thread.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$ifc_param` | `int` | Queue type. |
  | `$follow_links` | `bool` | Whether to follow links. |
- **Return**: HTML output for the thread status.
- **Inner Mechanisms**:
  - Calls `search->queue_process()` to handle a single entry.
  - Updates the UI with success/failure status and reloads after a delay.
- **Usage Example**:
  ```php
  // Process a single queue entry
  $result = $search->queue_process(CMS_SEARCH_QUEUE_TYPE_ALL, false);
  ```

---

### `data_process` and `data_process_thread`
```php
case "data_process":
    // Display UI for data processing
    $ifc = new ifc(NULL, NULL, NULL);
    echo("<div>...</div>");
    $ifc->close();
    break;

case "data_process_thread":
    init($action, 0);
    switch ($action) {
        case 0: // Initialize page score
            $result = $search->score_compute_initialize();
            break;
        case 1: // Compute page score
            $result = $search->score_compute_iterate();
            break;
        // ... (Other cases)
    };
    // ... (Update UI)
    exit();
```
- **Purpose**: Processes search data (e.g., score computation, canonical entry detection).
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$action` | `int` | Current processing step (0-5). |
  | `$iteration` | `int` | Current iteration for score computation. |
- **Return**: HTML output for the processing status.
- **Inner Mechanisms**:
  - Executes a multi-step process:
    1. Initialize page scores.
    2. Compute scores iteratively.
    3. Finalize scores.
    4. Integrate dangling links.
    5. Find canonical entries.
  - Updates the UI with progress and reloads after each step.
- **Usage Example**:
  ```php
  // Initialize score computation
  $search->score_compute_initialize();
  // Compute scores for one iteration
  $search->score_compute_iterate();
  ```

---

### `filter` and `_filter`
```php
case "filter":
    $system = new system();
    $blacklist = $system->getval("search", "blacklist");
    $whitelist = $system->getval("search", "whitelist");
    // ... (Display UI)
    break;

case "_filter":
    $system = new system();
    $system->setval($ifc_param1, "search", "blacklist");
    $system->setval($ifc_param2, "search", "whitelist");
    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Manages URL filtering (blacklists/whitelists).
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$ifc_param1` | `string` | Blacklist patterns (one per line). |
  | `$ifc_param2` | `string` | Whitelist patterns (one per line). |
- **Return**: Status message (`CMS_MSG_ERROR` or `CMS_MSG_DONE`).
- **Usage Example**:
  ```php
  // Set blacklist and whitelist
  $system = new system();
  $system->setval("*.spam.com\n*.malware.com", "search", "blacklist");
  $system->setval("*.example.com", "search", "whitelist");
  $system->save();
  ```

---

### `stopword` and `_stopword`
```php
case "stopword":
    $data = new data("#system/language");
    $array = [];
    $data->move("first");
    while ($key = $data->move("next"))
        $array[$key] = $data->get($key, "stopword");
    // ... (Display UI)
    break;

case "_stopword":
    $data = new data("#system/language");
    $array = language_get_array($ifc_param1);
    $data->move("first");
    while ($key = $data->move("next"))
        $data->set($array[$key] ?? "", $key, "stopword");
    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Manages stopwords for search indexing.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$ifc_param1` | `string` | Stopwords (one per line, language-specific). |
- **Return**: Status message (`CMS_MSG_ERROR` or `CMS_MSG_DONE`).
- **Usage Example**:
  ```php
  // Set stopwords for English
  $data = new data("#system/language");
  $data->set("the\nand\nof", "en", "stopword");
  $data->save();
  ```

---

### `clean`
```php
case "clean":
    $ifc_response = $search->clean() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Cleans the search index (removes outdated entries).
- **Return**: Status message (`CMS_MSG_ERROR` or `CMS_MSG_DONE`).
- **Usage Example**:
  ```php
  // Clean the search index
  $search->clean();
  ```

---

### `reset`
```php
case "reset":
    $filter_field = NULL;
    $filter_option = NULL;
    $filter_value = NULL;
    $order = NULL;
    $page = NULL;
    $limit = NULL;
    cms_cache_delete([
        "search." . CMS_USER . ".filter_field",
        "search." . CMS_USER . ".filter_option",
        "search." . CMS_USER . ".filter_value",
        "search." . CMS_USER . ".order",
        "search." . CMS_USER . ".page",
        "search." . CMS_USER . ".limit"
    ]);
    break;
```
- **Purpose**: Resets search filters and pagination settings.
- **Usage Example**:
  ```php
  // Reset search filters for the current user
  cms_cache_delete(["search." . CMS_USER . ".filter_field"]);
  ```

---

### `configuration` and `_configuration`
```php
case "configuration":
    // Display configuration UI
    $ifc = new ifc($ifc_response, $ifc_page, TRUE, NULL, "_configuration", CMS_L_IFC_SEARCH_062);
    // ... (UI setup)
    break;

case "_configuration":
    $type = (isset($ifc_param2) ? CMS_SEARCH_QUEUE_TYPE_INTERNAL : 0) |
            (isset($ifc_param3) ? CMS_SEARCH_QUEUE_TYPE_SELECTION : 0) |
            // ... (Other types);
    $search->daemon_status(isset($ifc_param1), $type, isset($ifc_param7));
    $system = new system();
    $system->setval($ifc_param8, "search", "difference");
    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    $search->entry_set_maximum_bit_difference = $ifc_param8;
    break;
```
- **Purpose**: Configures search settings (daemon, bit difference threshold).
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$ifc_param1` | `bool` | Enable/disable daemon. |
  | `$ifc_param2-$ifc_param6` | `bool` | Queue types to process. |
  | `$ifc_param7` | `bool` | Follow links. |
  | `$ifc_param8` | `int` | Maximum bit difference for entries. |
- **Return**: Status message (`CMS_MSG_ERROR` or `CMS_MSG_DONE`).
- **Usage Example**:
  ```php
  // Enable daemon for internal content
  $search->daemon_status(true, CMS_SEARCH_QUEUE_TYPE_INTERNAL, false);
  ```

---

## Main Display

### Filter and Pagination
```php
// Sync settings
cms_cache_sync($filter_field, "search." . CMS_USER . ".filter_field", CMS_DB_SEARCH_ENTRY_ADDRESS);
cms_cache_sync($filter_option, "search." . CMS_USER . ".filter_option", 1);
cms_cache_sync($filter_value, "search." . CMS_USER . ".filter_value", "");
cms_cache_sync($page, "search." . CMS_USER . ".page", 0);
cms_cache_sync($limit, "search." . CMS_USER . ".limit", 25);
cms_cache_sync($order, "search." . CMS_USER . ".order", 7);
```
- **Purpose**: Synchronizes filter and pagination settings with the cache.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$filter_field` | `string` | Field to filter by (e.g., `CMS_DB_SEARCH_ENTRY_ADDRESS`). |
  | `$filter_option` | `int` | Filter condition (e.g., `LIKE '%value%'`). |
  | `$filter_value` | `string` | Filter value. |
  | `$page` | `int` | Current page. |
  | `$limit` | `int` | Rows per page. |
  | `$order` | `int` | Sorting order. |

---

### Search Results Table
```php
// Retrieve data
$query = "SELECT " . CMS_DB_SEARCH_ENTRY_INDEX . ", " .
         CMS_DB_SEARCH_ENTRY_ADDRESS . ", " .
         // ... (Other columns)
         "FROM " . CMS_DB_SEARCH_ENTRY . " ";
if (nstre($filter_value)) $query .= "WHERE " . sqlesc($filter_field) . str_replace("#value#", sqlesc($filter_value), $filter_option_list[$filter_option] ?? $filter_option_list[1]) . " ";
$query .= "ORDER BY " . sqlesc($order_list[$order] ?? $order_list[7]);
$result = mysql_query($query);
```
- **Purpose**: Fetches search entries based on filters and sorting.
- **Inner Mechanisms**:
  - Constructs a SQL query with dynamic filtering and sorting.
  - Uses `sqlesc()` for safe SQL escaping.
- **Usage Example**:
  ```php
  // Fetch entries filtered by title containing "example"
  $query = "SELECT * FROM " . CMS_DB_SEARCH_ENTRY . " WHERE " . CMS_DB_SEARCH_ENTRY_TITLE . " LIKE '%example%'";
  $result = mysql_query($query);
  ```

---

### Pagination and Table Rendering
```php
// Pagination
if ($count > $limit) {
    $count = ceil($count / $limit);
    $page = min((int)$page, $count - 1);
    pagination("javascript:p(%page%);", $page, $count, CMS_L_COMMAND_NEXT, "pagination");
    mysql_data_seek($result, $page * $limit);
};

// Table rendering
ifc_table_open();
echo("<colgroup>...</colgroup><tr>...</tr>");
while (($i++ < $limit) && ($resultrow = mysql_fetch_assoc($result))) {
    // Render row
};
ifc_table_close();
```
- **Purpose**: Renders a paginated table of search entries.
- **Inner Mechanisms**:
  - Uses `pagination()` to generate page links.
  - Renders rows with icons, addresses, titles, scores, and timestamps.
- **Usage Example**:
  ```php
  // Render a table of search results
  ifc_table_open();
  while ($row = mysql_fetch_assoc($result)) {
      echo("<tr><td>" . x($row[CMS_DB_SEARCH_ENTRY_ADDRESS]) . "</td></tr>");
  };
  ifc_table_close();
  ```

---

## Helper Functions

### `f1` and `f2`
```php
$f1 = function($order, $asc, $dsc) {
    switch ((int)$order) {
        case $asc: return " ↗";
        case $dsc: return " ↘";
    };
    return "";
};

$f2 = function($order, $asc, $dsc) {
    switch ((int)$order) {
        default:
        case $dsc: return "javascript:o($asc);";
        case $asc: return "javascript:o($dsc);";
    };
};
```
- **Purpose**:
  - `f1`: Adds an arrow indicator for sorting direction.
  - `f2`: Generates a JavaScript link to toggle sorting.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$order` | `int` | Current sort order. |
  | `$asc` | `int` | Ascending sort order value. |
  | `$dsc` | `int` | Descending sort order value. |
- **Usage Example**:
  ```php
  // Add sorting indicator to a column header
  echo("<th>" . CMS_L_URL . $f1($order, 0, 1) . "</th>");
  // Add sorting toggle link
  echo("<a href=\"" . $f2($order, 0, 1) . "\">Sort</a>");
  ```

---

## JavaScript Functions

### `qp` and `qr`
```php
function qp(type) {
    load_page("<?php echo(str_replace("%replace%", "'+encodeURIComponent(type)+'", q(cms_url([...]))));?>");
};

function qr(type) {
    if (confirm("<?php echo(q(CMS_L_COMMAND_DELETE));?>?")) ifc_post("queue_remove", type);
};
```
- **Purpose**:
  - `qp`: Loads the queue processing interface for a specific queue type.
  - `qr`: Removes a queue type after confirmation.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `type` | `string` | Queue type (e.g., `CMS_SEARCH_QUEUE_TYPE_ALL`). |

### `o` and `p`
```php
function o(value) {
    ifc_set("order", value);
    ifc_post();
};

function p(number) {
    ifc_set("page", number);
    ifc_post();
};
```
- **Purpose**:
  - `o`: Sets the sorting order and refreshes the page.
  - `p`: Sets the page number and refreshes the page.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `value` | `int` | Sort order value. |
  | `number` | `int` | Page number. |


<!-- HASH:43106a72789427d7441835bb066eab03 -->
