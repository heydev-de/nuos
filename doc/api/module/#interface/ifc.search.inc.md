# NUOS API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.search.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.15.0`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Search Interface Module (`ifc.search.inc`)

This file provides the **Search Interface** for the NUOS web platform, enabling administrators to manage the search index, queue, and configuration. It handles scanning URLs, processing the indexing queue, managing stopwords, filtering search entries, and configuring search behavior.

---

### **Overview**
The search interface allows:
- **URL Scanning**: Indexing external or internal content.
- **Queue Management**: Adding, removing, and processing URLs in the indexing queue.
- **Data Processing**: Computing page scores and canonical entries.
- **Filtering & Configuration**: Managing blacklists, whitelists, stopwords, and search settings.
- **Entry Management**: Viewing, filtering, and deleting indexed entries.

The interface leverages the `search` class for core operations and the `ifc` (Interface Controller) class for UI rendering.

---

### **Constants**
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DB_SEARCH_ENTRY` | Database table name | Stores indexed search entries. |
| `CMS_DB_SEARCH_ENTRY_INDEX` | Column name | Unique identifier for entries. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS` | Column name | URL of the indexed page. |
| `CMS_DB_SEARCH_ENTRY_TITLE` | Column name | Title of the indexed page. |
| `CMS_DB_SEARCH_ENTRY_TEXT` | Column name | Extracted text content. |
| `CMS_DB_SEARCH_ENTRY_SCORE` | Column name | Relevance score. |
| `CMS_DB_SEARCH_ENTRY_TIME` | Column name | Initial indexing timestamp. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | Column name | Last update timestamp. |
| `CMS_DB_SEARCH_ENTRY_ERROR` | Column name | Error flag (non-zero if errors occurred). |
| `CMS_DB_SEARCH_ENTRY_CANONICAL` | Column name | Canonical entry flag. |
| `CMS_SEARCH_QUEUE_TYPE_*` | Bitmask values | Queue types (e.g., `INTERNAL`, `SELECTION`, `SUBMISSION`). |
| `CMS_SEARCH_SCAN_*` | Bitmask values | Scan result flags (e.g., `INDEXED`, `ERROR`, `NO_CONTENT`). |

---

### **Initialization**
#### **Library Loading & Permissions**
```php
if (!cms_load("search")) ifc_inactive($ifc_page);
ifc_permission(["" => CMS_L_ACCESS]);
```
- **Purpose**: Loads the `search` library and checks user permissions.
- **Parameters**:
  - `$ifc_page`: Current interface page identifier.
- **Usage**: Ensures the search module is available and the user has access rights.

#### **Search Instance & Defaults**
```php
$search = new search();
if (! $search->enabled) ifc_inactive($ifc_page);
```
- **Purpose**: Initializes the `search` class and checks if the search module is enabled.
- **Defaults**:
  - `$object`: `NULL` (unused in this context).
  - `$sql_table`: `CMS_DB_SEARCH_ENTRY` (default search table).
  - `$sql_filter_field`: `CMS_DB_SEARCH_ENTRY_ADDRESS` (default filter field).
  - `$sql_filter_option`: `" LIKE '%#value#%'"` (default SQL filter condition).
  - `$sql_filter_value`: `""` (default empty filter value).
  - `$sql_order`: `CMS_DB_SEARCH_ENTRY_TIME . " DESC"` (default sorting).
  - `$page`: `0` (default page number).
  - `$limit`: `25` (default rows per page).

---

### **Message Handling**
The interface processes actions via `CMS_IFC_MESSAGE`. Each case corresponds to a specific operation.

---

#### **`scan`**
```php
case "scan":
    $array = [CMS_SEARCH_SCAN_* => CMS_L_IFC_SEARCH_*];
    $ifc_param1 = translate_url($ifc_param1, NULL, CMS_LANGUAGE, TRUE);
    $result = $search->scan($ifc_param1, isset($ifc_param2));
    $ifc_response = (CMS_SEARCH_SCAN_ERROR & $result) ? CMS_MSG_ERROR : CMS_MSG_DONE;
    if (isset($array[$result])) $ifc_response .= $array[$result];
    break;
```
- **Purpose**: Scans a URL for indexing.
- **Parameters**:
  - `$ifc_param1`: URL to scan (translated via `translate_url`).
  - `$ifc_param2`: Boolean flag (follow links if `TRUE`).
- **Return**:
  - `$ifc_response`: Success/error message with localized text.
- **Inner Mechanisms**:
  - Calls `search->scan()` to index the URL.
  - Maps scan result codes to localized messages.
- **Usage**: Triggered when manually adding a URL to the index.

---

#### **`queue_add`**
```php
case "queue_add":
    if (empty($list)) break;
    $error = FALSE;
    $result = mysql_query("SELECT " . CMS_DB_SEARCH_ENTRY_ADDRESS . " FROM " . CMS_DB_SEARCH_ENTRY . " WHERE " . CMS_DB_SEARCH_ENTRY_INDEX . " IN ('" . implode("', '", $list) . "')");
    while ($resultrow = mysql_fetch_row($result)) $error |= ! $search->queue_add($resultrow[0], CMS_SEARCH_QUEUE_TYPE_SELECTION);
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;
```
- **Purpose**: Adds selected entries to the indexing queue.
- **Parameters**:
  - `$list`: Array of entry indices to add.
- **Return**:
  - `$ifc_response`: Success/error message.
- **Inner Mechanisms**:
  - Fetches URLs for selected indices.
  - Adds each URL to the queue with type `SELECTION`.
- **Usage**: Bulk-adding entries from the search results table.

---

#### **`queue_add_all`**
```php
case "queue_add_all":
    $ifc_response = $search->queue_add_all() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Adds all indexed entries to the queue for reprocessing.
- **Return**:
  - `$ifc_response`: Success/error message.
- **Usage**: Reindexing the entire search database.

---

#### **`queue_add_internal`**
```php
case "queue_add_internal":
    $language = explode(",", CMS_LANGUAGE_ENABLED);
    $count = 0;
    foreach ($language AS $value) {
        $value = stre($value) ? "" : "$value.";
        $map = new map("#system/" . $value . "directory.content");
        $list = $map->get_value_list();
        foreach ($list AS $_value) $count += $search->queue_add($_value, CMS_SEARCH_QUEUE_TYPE_INTERNAL);
    };
    $ifc_response = $count ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Adds all internal content pages to the queue.
- **Return**:
  - `$ifc_response`: Success/error message.
- **Inner Mechanisms**:
  - Iterates over enabled languages.
  - Uses `map` to fetch content URLs.
  - Adds each URL to the queue with type `INTERNAL`.
- **Usage**: Reindexing all internal content.

---

#### **`queue_remove`**
```php
case "queue_remove":
    $ifc_response = $search->queue_remove($ifc_param) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Removes a URL from the queue.
- **Parameters**:
  - `$ifc_param`: URL to remove.
- **Return**:
  - `$ifc_response`: Success/error message.
- **Usage**: Cleaning up the queue.

---

#### **`queue_process`**
```php
case "queue_process":
    $array = [CMS_L_IFC_SEARCH_* => CMS_SEARCH_QUEUE_TYPE_*];
    // ... JavaScript for queue_process_start() ...
    $ifc->set($array, "select b", $ifc_param);
    $ifc->set(CMS_L_IFC_SEARCH_016, "checkbox b", TRUE, FALSE);
    $ifc->set(CMS_L_IFC_SEARCH_057, "button", "javascript:queue_process_start();");
    $ifc->close();
    break;
```
- **Purpose**: Displays a form to start queue processing.
- **Parameters**:
  - `$ifc_param`: Queue type (e.g., `ALL`, `INTERNAL`).
- **UI Elements**:
  - Dropdown to select queue type.
  - Checkbox to enable "follow links".
  - Button to start processing.
- **Usage**: Initiating bulk indexing.

---

#### **`_queue_process`**
```php
case "_queue_process":
    $array = [CMS_SEARCH_QUEUE_TYPE_* => CMS_L_IFC_SEARCH_*];
    $follow_links = $follow_links ?? FALSE;
    // ... Display queue status and threads ...
    for ($i = 0; $i < 4; $i++) echo("<object data=\"" . x(cms_url([...])) . "\" type=\"text/html\" class=\"search-thread\"></object>");
    $ifc->close();
    break;
```
- **Purpose**: Displays the queue processing interface with 4 worker threads.
- **Parameters**:
  - `$ifc_param`: Queue type.
  - `$follow_links`: Boolean (follow links if `TRUE`).
- **Inner Mechanisms**:
  - Embeds 4 `<object>` tags, each loading a `queue_process_thread` worker.
  - Shows remaining queue length.
- **Usage**: Monitoring and managing active queue processing.

---

#### **`queue_process_thread`**
```php
case "queue_process_thread":
    echo(CMS_DOCTYPE_HTML . "<html>...</html>");
    force_flush();
    if (! isset($ifc_param)) $ifc_param = CMS_SEARCH_QUEUE_TYPE_ALL;
    if (! isset($follow_links)) $follow_links = FALSE;
    $value = $search->queue_process($ifc_param, $follow_links);
    // ... Determine status and reload delay ...
    printf($buffer, $class, $text, $time);
    // ... Update parent queue length ...
    exit();
```
- **Purpose**: Processes a single queue item in a worker thread.
- **Parameters**:
  - `$ifc_param`: Queue type.
  - `$follow_links`: Boolean (follow links if `TRUE`).
- **Return**:
  - HTML page with status and auto-reload.
- **Inner Mechanisms**:
  - Calls `search->queue_process()` to handle one item.
  - Determines status (`ok`, `error`, `done`) and reload delay.
  - Updates the parent window's queue length display.
- **Usage**: Background processing of queue items.

---

#### **`entry_remove`**
```php
case "entry_remove":
    if (empty($list)) break;
    $error = FALSE;
    foreach ($list AS $value) $error |= ! $search->entry_remove($value);
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;
```
- **Purpose**: Removes selected entries from the search index.
- **Parameters**:
  - `$list`: Array of entry indices to remove.
- **Return**:
  - `$ifc_response`: Success/error message.
- **Usage**: Bulk-deleting indexed entries.

---

#### **`data_process`**
```php
case "data_process":
    $ifc = new ifc(NULL, NULL, NULL);
    echo("<div><strong>" . CMS_L_IFC_SEARCH_046 . "</strong><br><object data=\"" . x(cms_url([...])) . "\" type=\"text/html\" class=\"search-thread\"></object></div>");
    $ifc->close();
    break;
```
- **Purpose**: Displays the data processing interface.
- **Inner Mechanisms**:
  - Embeds a worker thread (`data_process_thread`) to compute page scores.
- **Usage**: Recalculating relevance scores and canonical entries.

---

#### **`data_process_thread`**
```php
case "data_process_thread":
    if (empty($action)) $action = 0;
    switch ($action) {
        case 0: // Initialize
        case 1: // Compute page score
        case 2: // Finalize
        case 3: // Integrate dangling links
        case 4: // Find canonical entries
        case 5: // Complete
    };
    echo(CMS_DOCTYPE_HTML . "<html>...</html>");
    ob_flush();
    flush();
    switch ($action) {
        case 0: $result = $search->score_compute_initialize(); break;
        case 1: $result = $search->score_compute_iterate(); break;
        case 2: $result = $search->score_compute_finalize(); break;
        case 3: $result = $search->score_compute_iterate($iteration); break;
        case 4: $result = $search->score_compute_canonical(); break;
    };
    if ($result === TRUE) echo(jscript("location.replace(\"" . q(cms_url([...])) . "\");"));
    else echo(jscript("document.getElementById(\"loader\").src = \"" . q(CMS_IMAGES_URL) . "search/data_failure.png\";"));
    exit();
```
- **Purpose**: Processes search data in stages (score computation, canonical entries).
- **Parameters**:
  - `$action`: Current processing stage (0-5).
  - `$iteration`: Iteration counter for multi-stage processes.
- **Return**:
  - HTML page with progress visualization and auto-reload.
- **Inner Mechanisms**:
  - Calls `search->score_compute_*()` methods for each stage.
  - Displays progress via images and localized text.
  - Auto-reloads until completion or failure.
- **Usage**: Background processing of search data.

---

#### **`filter`**
```php
case "filter":
    $system = new system();
    $blacklist = $system->getval("search", "blacklist");
    $whitelist = $system->getval("search", "whitelist");
    $ifc->set(CMS_L_IFC_SEARCH_055, "info b");
    $ifc->set(CMS_L_IFC_SEARCH_053, "textarea 60x12 b", $blacklist);
    $ifc->set(CMS_L_IFC_SEARCH_054, "textarea 60x12", $whitelist);
    $ifc->close();
    break;
```
- **Purpose**: Displays a form to edit blacklist/whitelist filters.
- **UI Elements**:
  - Textarea for blacklist (URL patterns to exclude).
  - Textarea for whitelist (URL patterns to include).
- **Usage**: Configuring URL filtering rules.

---

#### **`_filter`**
```php
case "_filter":
    $system = new system();
    $system->setval($ifc_param1, "search", "blacklist");
    $system->setval($ifc_param2, "search", "whitelist");
    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Saves blacklist/whitelist filters.
- **Parameters**:
  - `$ifc_param1`: Blacklist patterns.
  - `$ifc_param2`: Whitelist patterns.
- **Return**:
  - `$ifc_response`: Success/error message.
- **Usage**: Applying filter changes.

---

#### **`stopword`**
```php
case "stopword":
    $data = new data("#system/language");
    $array = [];
    $data->move("first");
    while ($key = $data->move("next")) $array[$key] = $data->get($key, "stopword");
    $language = implode(",", array_keys($array));
    $value = language_set_array($array);
    $ifc->set(CMS_L_IFC_SEARCH_061, "info b");
    $ifc->set(CMS_L_IFC_SEARCH_060, "textarea 60x24 bl", $value, NULL, NULL, $language);
    $ifc->close();
    break;
```
- **Purpose**: Displays a form to edit stopwords for each language.
- **UI Elements**:
  - Textarea with stopwords (one per line) for all enabled languages.
- **Usage**: Configuring language-specific stopwords.

---

#### **`_stopword`**
```php
case "_stopword":
    $data = new data("#system/language");
    $array = language_get_array($ifc_param1);
    $data->move("first");
    while ($key = $data->move("next")) {
        $value = isset($array[$key]) ? $array[$key] : "";
        $data->set($value, $key, "stopword");
    };
    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Saves stopwords for all languages.
- **Parameters**:
  - `$ifc_param1`: Stopwords (one per line).
- **Return**:
  - `$ifc_response`: Success/error message.
- **Usage**: Applying stopword changes.

---

#### **`clean`**
```php
case "clean":
    $ifc_response = $search->clean() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Cleans up the search index (removes invalid entries).
- **Return**:
  - `$ifc_response`: Success/error message.
- **Usage**: Maintenance task to remove stale entries.

---

#### **`configuration`**
```php
case "configuration":
    $ifc->set(CMS_L_IFC_SEARCH_065, "title");
    $ifc->set(CMS_L_IFC_SEARCH_066, "checkbox b", TRUE, $status["enabled"]);
    // ... Queue type checkboxes ...
    $ifc->set(CMS_L_IFC_SEARCH_016, "checkbox b", TRUE, $status["follow_links"]);
    $ifc->set(CMS_L_IFC_SEARCH_063, "label");
    $ifc->set($array, "select 30", $search->entry_set_maximum_bit_difference);
    $ifc->close();
    break;
```
- **Purpose**: Displays the search configuration form.
- **UI Elements**:
  - Checkbox to enable/disable the search daemon.
  - Checkboxes for queue types (e.g., `INTERNAL`, `SELECTION`).
  - Checkbox to enable "follow links".
  - Dropdown to set the maximum bit difference for fuzzy matching.
- **Usage**: Configuring search behavior.

---

#### **`_configuration`**
```php
case "_configuration":
    $type = (isset($ifc_param2) ? CMS_SEARCH_QUEUE_TYPE_INTERNAL : 0) | ...;
    $search->daemon_status(isset($ifc_param1), $type, isset($ifc_param7));
    $system = new system();
    $system->setval($ifc_param8, "search", "difference");
    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    $search->entry_set_maximum_bit_difference = $ifc_param8;
    break;
```
- **Purpose**: Saves search configuration.
- **Parameters**:
  - `$ifc_param1`: Daemon enabled (`TRUE`/`FALSE`).
  - `$ifc_param2`-`$ifc_param6`: Queue type flags.
  - `$ifc_param7`: Follow links (`TRUE`/`FALSE`).
  - `$ifc_param8`: Maximum bit difference.
- **Return**:
  - `$ifc_response`: Success/error message.
- **Usage**: Applying configuration changes.

---

### **Main Display**
#### **Menu & Queue Status**
```php
$menu = [CMS_L_IFC_SEARCH_* => "queue_add|queue_add_internal|..."];
$ifc = new ifc($ifc_response, $ifc_page, $menu, [...]);
echo("<div class=\"info\">" . sprintf(CMS_L_IFC_SEARCH_003, ...) . "</div>");
```
- **Purpose**: Renders the main interface with:
  - A menu for search operations.
  - A status bar showing queue lengths for each type.
- **Usage**: Primary navigation and status overview.

---

#### **Search Filter Form**
```php
ifc_tab_open(CMS_L_IFC_SEARCH_001);
$ifc->set([CMS_L_URL => CMS_DB_SEARCH_ENTRY_ADDRESS, ...], "select", $sql_filter_field, NULL, "_sql_filter_field");
$ifc->set([CMS_L_IFC_SEARCH_006 => " LIKE '#value#%'", ...], "select", $sql_filter_option, NULL, "_sql_filter_option");
$ifc->set(NULL, "text 20 40 :", $sql_filter_value, NULL, "_sql_filter_value");
$ifc->set([CMS_L_URL => CMS_DB_SEARCH_ENTRY_ADDRESS, ...], "select :", $sql_order, NULL, "_sql_order");
$ifc->set([10 => "10", 25 => "25", ...], "select :", $limit, NULL, "_limit");
$ifc->set(CMS_L_COMMAND_CONFIRM, "button");
$ifc->set(CMS_L_IFC_SEARCH_044, "button", "javascript:ifc_reset(1);ifc_submit();");
ifc_tab_close();
```
- **Purpose**: Renders a form to filter and sort search entries.
- **UI Elements**:
  - Dropdown to select filter field (e.g., URL, title, score).
  - Dropdown to select filter condition (e.g., `LIKE`, `=`, `>`).
  - Text input for filter value.
  - Dropdown to select sort order.
  - Dropdown to set rows per page.
  - Buttons to apply/reset filters.

---

#### **URL Addition Form**
```php
ifc_tab_next(CMS_L_IFC_SEARCH_018);
$ifc->set(CMS_L_URL, "text 30 :", CMS_PROTOCOL . "://");
$ifc->set("…", "button", $buffer);
$ifc->set(CMS_L_IFC_SEARCH_016, "checkbox", TRUE);
$ifc->set(CMS_L_IFC_SEARCH_014, "button", "scan");
ifc_tab_close();
```
- **Purpose**: Renders a form to manually add URLs to the queue.
- **UI Elements**:
  - Text input for URL.
  - Button to open a content selector.
  - Checkbox to enable "follow links".
  - Button to scan the URL.

---

#### **Pagination & Results Table**
```php
$query = "SELECT " . CMS_DB_SEARCH_ENTRY_INDEX . ", " . CMS_DB_SEARCH_ENTRY_ADDRESS . ", ... FROM " . CMS_DB_SEARCH_ENTRY . " WHERE ... ORDER BY ...";
$result = mysql_query($query);
// ... Pagination logic ...
ifc_table_open();
echo("<colgroup><col style=\"WIDTH:0\"><col span=\"5\"></colgroup><tr><td class=\"select\"></td><th>" . CMS_L_URL . "</th>...</tr>");
while (($i++ < $limit) && ($resultrow = mysql_fetch_assoc($result))) {
    echo("<tr><td class=\"select\">");
    $ifc->set(NULL, "checkbox", $resultrow[CMS_DB_SEARCH_ENTRY_INDEX], FALSE, "list[]");
    echo("</td><td$varied><a href=\"" . x(cms_url($resultrow[CMS_DB_SEARCH_ENTRY_ADDRESS], NULL, TRUE)) . "\">" . image("search/" . $image) . " " . x(strabridge($resultrow[CMS_DB_SEARCH_ENTRY_ADDRESS], 75)) . "</a></td>...");
};
ifc_table_close();
```
- **Purpose**: Displays paginated search results.
- **UI Elements**:
  - Table with columns: Selection, URL, Title, Score, Time, Update Time.
  - Pagination controls (start, previous, next, end).
  - Selection checkboxes for bulk operations.
- **Inner Mechanisms**:
  - Uses `mysql_query` to fetch results.
  - Applies pagination logic to limit rows per page.
  - Displays icons for canonical/error entries.
  - Truncates long text with `strabridge()`.

---

### **JavaScript Functions**
| Function | Purpose |
|----------|---------|
| `qp(type)` | Opens the queue processing interface for a specific type. |
| `qr(type)` | Removes a queue type after confirmation. |
| `p(number)` | Navigates to a specific page of results. |

---

### **Usage Scenarios**
1. **Indexing Content**:
   - Use `queue_add_internal` to index all internal pages.
   - Use `scan` to manually index a specific URL.
2. **Queue Management**:
   - Use `queue_process` to start processing the queue.
   - Use `queue_remove` to clean up the queue.
3. **Data Processing**:
   - Use `data_process` to recompute page scores and canonical entries.
4. **Configuration**:
   - Use `configuration` to enable/disable the search daemon and set queue types.
5. **Filtering**:
   - Use the search filter form to narrow down results.
   - Use `filter` to configure blacklist/whitelist rules.
6. **Stopwords**:
   - Use `stopword` to edit language-specific stopwords.
7. **Maintenance**:
   - Use `clean` to remove invalid entries.
   - Use `entry_remove` to delete selected entries.


<!-- HASH:7500e1f3e43717ec93208dd0a0981e50 -->
