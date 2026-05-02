# NUOS API Documentation

[← Index](../README.md) | [`#system/lib.search.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Search Module (`lib.search.inc`)

The `search` class provides a comprehensive full-text search engine for the NUOS platform. It indexes web pages, processes search queries, and computes relevance scores using a link-based algorithm similar to PageRank. The module handles URL normalization, content extraction, word tokenization, and database management for search-related data.

---

### Constants

#### Permission Constants
| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_PERMISSION_SUBMIT` | `"submit"` | Permission identifier for search submission. |

#### Database Constants
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_ENTRY` | `CMS_DB_PREFIX . "search_entry"` | Table storing indexed entries. |
| `CMS_DB_SEARCH_ENTRY_INDEX` | `"id"` | Primary key for search entries. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS` | `"address"` | URL of the indexed entry. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS_HASH` | `"address_hash"` | 32-bit hash of the URL for indexing. |
| `CMS_DB_SEARCH_ENTRY_TITLE` | `"title"` | Title of the indexed entry. |
| `CMS_DB_SEARCH_ENTRY_TEXT` | `"text"` | Extracted text content of the entry. |
| `CMS_DB_SEARCH_ENTRY_TEXT_HASH` | `"text_hash"` | 64-bit simhash for content similarity detection. |
| `CMS_DB_SEARCH_ENTRY_TIME` | `"time"` | Timestamp of the last update. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL` | `"update_interval"` | Time interval before the next update. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | `"update_time"` | Timestamp for the next scheduled update. |
| `CMS_DB_SEARCH_ENTRY_SCORE` | `"score"` | Relevance score of the entry. |
| `CMS_DB_SEARCH_ENTRY_LINK_COUNT` | `"link_count"` | Number of outbound links. |
| `CMS_DB_SEARCH_ENTRY_ERROR` | `"error"` | Error counter for failed updates. |
| `CMS_DB_SEARCH_ENTRY_CANONICAL` | `"canonical"` | Flag indicating if the entry is canonical (non-duplicate). |
| `CMS_DB_SEARCH_WORD` | `CMS_DB_PREFIX . "search_word"` | Table storing unique words. |
| `CMS_DB_SEARCH_WORD_INDEX` | `"id"` | Primary key for words. |
| `CMS_DB_SEARCH_WORD_TEXT` | `"text"` | The word itself. |
| `CMS_DB_SEARCH_WORD_LANGUAGE` | `"language"` | Language of the word. |
| `CMS_DB_SEARCH_WEIGHT` | `CMS_DB_PREFIX . "search_weight"` | Table storing word weights for entries. |
| `CMS_DB_SEARCH_WEIGHT_WORD` | `"word"` | Foreign key to `CMS_DB_SEARCH_WORD_INDEX`. |
| `CMS_DB_SEARCH_WEIGHT_SOURCE` | `"source"` | Foreign key to `CMS_DB_SEARCH_ENTRY_INDEX` (link source). |
| `CMS_DB_SEARCH_WEIGHT_TARGET` | `"target"` | Foreign key to `CMS_DB_SEARCH_ENTRY_INDEX` (link target). |
| `CMS_DB_SEARCH_WEIGHT_VALUE` | `"value"` | Weight of the word for the target entry. |
| `CMS_DB_SEARCH_LINK` | `CMS_DB_PREFIX . "search_link"` | Table storing links between entries. |
| `CMS_DB_SEARCH_LINK_SOURCE` | `"source"` | Foreign key to `CMS_DB_SEARCH_ENTRY_INDEX` (link source). |
| `CMS_DB_SEARCH_LINK_TARGET_HASH` | `"target_hash"` | 32-bit hash of the target URL. |
| `CMS_DB_SEARCH_LINK_TARGET` | `"target"` | Foreign key to `CMS_DB_SEARCH_ENTRY_INDEX` (link target). |
| `CMS_DB_SEARCH_LINK_TEXT` | `"text"` | Anchor text of the link. |
| `CMS_DB_SEARCH_LINK_LEVEL` | `"level"` | Level of the link (0 = valid, >0 = dangling). |
| `CMS_DB_SEARCH_CLUSTER` | `CMS_DB_PREFIX . "search_cluster"` | Table storing clusters of similar entries. |
| `CMS_DB_SEARCH_CLUSTER_SOURCE` | `"source"` | Foreign key to `CMS_DB_SEARCH_ENTRY_INDEX` (cluster source). |
| `CMS_DB_SEARCH_CLUSTER_TARGET` | `"target"` | Foreign key to `CMS_DB_SEARCH_ENTRY_INDEX` (cluster target). |
| `CMS_DB_SEARCH_QUEUE` | `CMS_DB_PREFIX . "search_queue"` | Table storing URLs to be indexed. |
| `CMS_DB_SEARCH_QUEUE_ADDRESS` | `"address"` | URL to be indexed. |
| `CMS_DB_SEARCH_QUEUE_TYPE` | `"type"` | Type of queue entry. |
| `CMS_DB_SEARCH_QUEUE_TIME` | `"time"` | Timestamp for processing. |
| `CMS_DB_SEARCH_QUEUE_CODE` | `"code"` | Unique code for processing lock. |
| `CMS_DB_SEARCH_QUEUE_ERROR` | `"error"` | Error counter for failed processing. |
| `CMS_DB_SEARCH_QUEUE_DONE` | `"done"` | Flag indicating if the entry is processed. |

#### Scan Result Codes
| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_SCAN_UNKNOWN_ERROR` | `1` | Unknown error during scanning. |
| `CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED` | `2` | Redirection limit exceeded. |
| `CMS_SEARCH_SCAN_INVALID_ADDRESS` | `4` | Invalid URL. |
| `CMS_SEARCH_SCAN_ADDRESS_REJECTED` | `8` | URL rejected by blacklist/whitelist. |
| `CMS_SEARCH_SCAN_NO_CONNECTION` | `16` | Connection to the URL failed. |
| `CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT` | `32` | Unsupported content type. |
| `CMS_SEARCH_SCAN_NO_MODIFICATION` | `64` | No modification since last scan. |
| `CMS_SEARCH_SCAN_DATA_FETCH_FAILED` | `128` | Failed to fetch content. |
| `CMS_SEARCH_SCAN_NO_CONTENT` | `256` | No content extracted. |
| `CMS_SEARCH_SCAN_INDEXING_UNDESIRED` | `512` | Indexing undesired (robots directive). |
| `CMS_SEARCH_SCAN_INDEXED` | `1024` | Successfully indexed. |
| `CMS_SEARCH_SCAN_INDEXING_FAILED` | `2048` | Indexing failed. |
| `CMS_SEARCH_SCAN_DATABASE_ERROR` | `4096` | Database error. |
| `CMS_SEARCH_SCAN_FATAL_ERROR` | `CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED \| CMS_SEARCH_SCAN_INVALID_ADDRESS \| CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT \| CMS_SEARCH_SCAN_NO_CONTENT` | Fatal errors (non-retryable). |
| `CMS_SEARCH_SCAN_ERROR` | `CMS_SEARCH_SCAN_FATAL_ERROR \| CMS_SEARCH_SCAN_UNKNOWN_ERROR \| CMS_SEARCH_SCAN_NO_CONNECTION \| CMS_SEARCH_SCAN_DATA_FETCH_FAILED \| CMS_SEARCH_SCAN_INDEXING_FAILED \| CMS_SEARCH_SCAN_DATABASE_ERROR` | All errors (retryable and non-retryable). |

#### Queue Types
| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_QUEUE_TYPE_NONE` | `0` | No specific type. |
| `CMS_SEARCH_QUEUE_TYPE_INTERNAL` | `1` | Internal URL. |
| `CMS_SEARCH_QUEUE_TYPE_SELECTION` | `2` | Selected URL. |
| `CMS_SEARCH_QUEUE_TYPE_SUBMISSION` | `4` | Submitted URL. |
| `CMS_SEARCH_QUEUE_TYPE_REFERENCE` | `8` | Discovered link (reference). |
| `CMS_SEARCH_QUEUE_TYPE_UPDATE` | `16` | Update existing entry. |
| `CMS_SEARCH_QUEUE_TYPE_ALL` | `255` | All types. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `mysql` | `NULL` | MySQL database connection object. |
| `enabled` | `FALSE` | Flag indicating if the search module is enabled. |
| `address_accepted_blacklist` | `[]` | List of regex patterns for URL exclusion. |
| `address_accepted_whitelist` | `[]` | List of regex patterns for URL inclusion (applied after blacklist). |
| `scan_routing_limit` | `5` | Maximum number of permitted redirects. |
| `update_interval` | `86400` | Initial requeue interval in seconds (1 day). |
| `update_interval_min` | `3600` | Minimum requeue interval in seconds (1 hour). |
| `update_interval_max` | `604800` | Maximum requeue interval in seconds (1 week). |
| `update_interval_decrease_factor` | `0.5` | Factor to decrease requeue interval on major changes. |
| `update_interval_increase_factor` | `1.5` | Factor to increase requeue interval on minor changes. |
| `entry_set_maximum_bit_difference` | `2` | Maximum bit difference in simhash for similarity. |
| `entry_set_weight_factor_title` | `0.1` | Weight factor for the title. |
| `entry_set_weight_factor_h1` | `1.3` | Weight factor for `<h1>` headings. |
| `entry_set_weight_factor_h2` | `1.2` | Weight factor for `<h2>` headings. |
| `entry_set_weight_factor_h3` | `1.1` | Weight factor for `<h3>` headings. |
| `entry_set_weight_factor_copy` | `1.0` | Weight factor for body text. |
| `entry_set_weight_factor_side` | `0.1` | Weight factor for side content. |
| `entry_set_weight_factor_address` | `0.1` | Weight factor for URL components. |
| `entry_set_weight_factor_link` | `0.1` | Weight factor for link anchor text. |
| `entry_remove_error_limit` | `3` | Number of failed scans before permanent removal. |
| `queue_process_retry_time` | `3600` | Time in seconds before retrying a failed scan (1 hour). |
| `queue_process_error_limit` | `3` | Maximum number of retries for a failed scan. |
| `find_results_per_page` | `10` | Number of results per page. |
| `score_compute_iteration_number` | `10` | Number of score refinement iterations. |
| `score_compute_dampening_factor` | `0.85` | Inherited portion of the score (dampening factor). |

---

### Methods

---

#### `search::__construct()`

**Purpose:**
Initializes the search module, verifies database tables, and loads settings.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Initializes a MySQL connection.
2. Verifies the existence and structure of all search-related database tables.
3. Loads settings from the `system` module (blacklist, whitelist, and simhash difference threshold).
4. Sets `enabled` to `TRUE` if all tables are verified and settings are loaded.

**Usage:**
Called automatically when the `search` class is instantiated. Ensures the database is ready for search operations.

---

#### `search::scan($address, $follow_links = FALSE)`

**Purpose:**
Scans a URL, extracts its content, and indexes it if modified. Handles redirects and checks for robots directives.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to scan. |
| `$follow_links` | `bool` | If `TRUE`, adds discovered links to the queue. |

**Return Values:**
- `int`: One of the `CMS_SEARCH_SCAN_*` constants indicating the result of the scan.

**Inner Mechanisms:**
1. Checks if the module is enabled and required libraries (`html`, `http`) are loaded.
2. Standardizes the URL using `address_standardize()`.
3. Checks if the URL is already indexed and if it has been modified since the last scan.
4. Calls `_scan()` to perform the actual scanning and indexing.
5. Removes the entry from the index if a fatal error occurs.

**Usage:**
- Used to manually trigger the scanning and indexing of a URL.
- Typically called by `queue_process()` for automated indexing.

---

#### `search::_scan($address, $follow_links, $entry_time = 0)`

**Purpose:**
Internal method to scan a URL, extract its content, and index it. Handles redirects, robots directives, and content type verification.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to scan. |
| `$follow_links` | `bool` | If `TRUE`, adds discovered links to the queue. |
| `$entry_time` | `int` | Timestamp of the last update (0 if new). |

**Return Values:**
- `int`: One of the `CMS_SEARCH_SCAN_*` constants indicating the result of the scan.

**Inner Mechanisms:**
1. Follows redirects up to `scan_routing_limit`.
2. Checks if the URL is accepted by `address_accepted()`.
3. Retrieves HTTP headers and verifies the content type (HTML/XML only).
4. Checks for `X-Robots-Tag` and `meta robots` directives to skip indexing if undesired.
5. Verifies if the content has been modified since `entry_time`.
6. Extracts content using `html_page_info()`.
7. Adds discovered links to the queue if `follow_links` is `TRUE`.
8. Calls `entry_set()` to index the content.

**Usage:**
- Internal method called by `scan()`.
- Not intended for direct use.

---

#### `search::entry_set($address, $data, $language, $links = NULL)`

**Purpose:**
Indexes or updates an entry in the search database. Processes content, extracts words, and computes weights.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL of the entry. |
| `$data` | `array` | Extracted content data (title, text, headings, links, etc.). |
| `$language` | `string` | Language of the content. |
| `$links` | `array\|NULL` | Optional array of outbound links (for testing). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.
- `string`: MySQL error message if a database error occurs.

**Inner Mechanisms:**
1. Standardizes the URL and checks if it is accepted.
2. Prepares content for indexing (truncates title and text, computes simhash).
3. Checks if the entry already exists in the database.
4. Updates the entry if it exists and has changed (adjusts requeue interval based on the degree of change).
5. Adds a new entry if it does not exist (sets canonical flag if no similar entry is found).
6. Processes content sections (title, headings, body, side, URL) and adds them to the index using `_entry_set()`.
7. Processes outbound and inbound links, adding their anchor text to the index.
8. Clusters similar entries based on simhash.

**Usage:**
- Called by `_scan()` to index or update an entry.
- Can be used to manually index content by providing extracted data.

---

#### `search::_entry_set($source_index, $target_index, $text, $language, $weight_factor = 1.0)`

**Purpose:**
Internal method to process text, extract words, and store their weights in the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$source_index` | `int` | Index of the source entry (link source). |
| `$target_index` | `int` | Index of the target entry (link target). |
| `$text` | `string` | Text to process. |
| `$language` | `string` | Language of the text. |
| `$weight_factor` | `float` | Weight factor for the text. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.
- `string`: MySQL error message if a database error occurs.

**Inner Mechanisms:**
1. Strips stop words from the text using `language_strip_stopword()`.
2. Tokenizes the text into words using `tokenize_text()`.
3. Counts word occurrences and prepares them for insertion.
4. Inserts new words into the `CMS_DB_SEARCH_WORD` table.
5. Retrieves word indices and computes weights based on word frequency and `weight_factor`.
6. Inserts or updates word weights in the `CMS_DB_SEARCH_WEIGHT` table.

**Usage:**
- Internal method called by `entry_set()`.
- Not intended for direct use.

---

#### `search::entry_remove($index, $force = TRUE)`

**Purpose:**
Removes an entry from the search index. If `force` is `FALSE`, increments the error counter instead of removing the entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Index of the entry to remove. |
| `$force` | `bool` | If `TRUE`, removes the entry permanently. If `FALSE`, increments the error counter. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. If `force` is `FALSE`, checks the error counter. If the counter is below `entry_remove_error_limit`, increments it and sets the canonical flag to `0`.
2. If `force` is `TRUE` or the error counter exceeds the limit, removes the entry from `CMS_DB_SEARCH_ENTRY`.
3. Removes associated data from `CMS_DB_SEARCH_WEIGHT`, `CMS_DB_SEARCH_LINK`, and `CMS_DB_SEARCH_CLUSTER`.

**Usage:**
- Called by `scan()` to remove entries with fatal errors.
- Can be used to manually remove entries from the index.

---

#### `search::queue_add($address, $type = CMS_SEARCH_QUEUE_TYPE_NONE)`

**Purpose:**
Adds a URL to the search queue for indexing or updates an existing queue entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to add to the queue. |
| `$type` | `int` | Type of queue entry (one of the `CMS_SEARCH_QUEUE_TYPE_*` constants). |

**Return Values:**
- `bool`: `TRUE` if the URL was added or updated, `FALSE` on failure.

**Inner Mechanisms:**
1. Standardizes the URL and checks if it is accepted.
2. If the type is `CMS_SEARCH_QUEUE_TYPE_REFERENCE`, adds the URL to the queue using `INSERT IGNORE`.
3. Otherwise, adds or updates the URL in the queue using `INSERT ... ON DUPLICATE KEY UPDATE`.

**Usage:**
- Called by `_scan()` to add discovered links to the queue.
- Can be used to manually add URLs to the queue for indexing.

---

#### `search::queue_add_update()`

**Purpose:**
Adds all entries that are due for an update to the queue.

**Return Values:**
- `bool`: `TRUE` if entries were added, `FALSE` on failure.

**Inner Mechanisms:**
Calls `queue_add_all(TRUE)` to add entries with `update_time` less than the current time.

**Usage:**
- Used to schedule updates for all indexed entries.

---

#### `search::queue_add_all($update = FALSE)`

**Purpose:**
Adds all entries to the queue or only those due for an update.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$update` | `bool` | If `TRUE`, adds only entries due for an update. |

**Return Values:**
- `bool`: `TRUE` if entries were added, `FALSE` on failure.

**Inner Mechanisms:**
1. Inserts entries from `CMS_DB_SEARCH_ENTRY` into `CMS_DB_SEARCH_QUEUE` with type `CMS_SEARCH_QUEUE_TYPE_UPDATE`.
2. If `$update` is `TRUE`, filters entries by `update_time`.

**Usage:**
- Called by `queue_add_update()` to schedule updates.
- Can be used to reindex all entries.

---

#### `search::queue_remove($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

**Purpose:**
Marks queue entries as done (processed).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Type of queue entries to mark as done (one of the `CMS_SEARCH_QUEUE_TYPE_*` constants). |

**Return Values:**
- `bool`: `TRUE` if entries were marked as done, `FALSE` on failure.

**Inner Mechanisms:**
Updates `CMS_DB_SEARCH_QUEUE` to set `done` to `1` for entries matching the specified type.

**Usage:**
- Used to clean up the queue after processing.

---

#### `search::queue_length($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

**Purpose:**
Returns the number of pending queue entries.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Type of queue entries to count (one of the `CMS_SEARCH_QUEUE_TYPE_*` constants). |

**Return Values:**
- `int\|bool`: Number of pending entries, or `FALSE` on failure.

**Inner Mechanisms:**
Counts entries in `CMS_DB_SEARCH_QUEUE` where `done` is `0` and the type matches.

**Usage:**
- Used to monitor the size of the queue.

---

#### `search::queue_process($type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

**Purpose:**
Processes the next entry in the queue (scans and indexes the URL).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Type of queue entries to process (one of the `CMS_SEARCH_QUEUE_TYPE_*` constants). |
| `$follow_links` | `bool` | If `TRUE`, adds discovered links to the queue. |

**Return Values:**
- `int\|bool`: Result of the scan (one of the `CMS_SEARCH_SCAN_*` constants), or `FALSE` on failure.

**Inner Mechanisms:**
1. Sets a time limit of 600 seconds.
2. Marks the next pending queue entry with a unique code and updates its `time` to `time + queue_process_retry_time`.
3. Retrieves the URL and error counter from the queue.
4. Calls `scan()` to process the URL.
5. If a non-fatal error occurs and the error counter is below the limit, increments the counter and returns the error.
6. Marks the queue entry as done.

**Usage:**
- Called by `daemon()` to process the queue automatically.
- Can be used to manually process the next queue entry.

---

#### `search::daemon_status($enabled = TRUE, $type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

**Purpose:**
Sets the status of the search daemon (enables/disables processing and configures queue type and link following).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$enabled` | `bool` | If `TRUE`, enables the daemon. |
| `$type` | `int` | Type of queue entries to process (one of the `CMS_SEARCH_QUEUE_TYPE_*` constants). |
| `$follow_links` | `bool` | If `TRUE`, adds discovered links to the queue. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
Writes the daemon status to a file (`#system/search.daemon.status`) with exclusive lock.

**Usage:**
- Used to control the search daemon.

---

#### `search::daemon_get_status()`

**Purpose:**
Retrieves the current status of the search daemon.

**Return Values:**
- `array\|bool`: Associative array with keys `enabled`, `type`, and `follow_links`, or `FALSE` on failure.

**Inner Mechanisms:**
Reads the daemon status from the file (`#system/search.daemon.status`) with shared lock.

**Usage:**
- Called by `daemon()` to check if processing should continue.

---

#### `search::daemon($time_limit = 60)`

**Purpose:**
Runs the search daemon to process the queue continuously until the time limit is reached or the daemon is disabled.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$time_limit` | `int` | Time limit in seconds. |

**Return Values:**
- `bool`: `TRUE` if the daemon completed processing, `FALSE` on failure.

**Inner Mechanisms:**
1. Processes the queue using `queue_process()`.
2. Checks the daemon status using `daemon_get_status()`.
3. Stops if the daemon is disabled or the time limit is reached.
4. Sleeps for 1 second between iterations.

**Usage:**
- Used to run the search daemon as a background process.

---

#### `search::find($term, $page = 0, $language = CMS_LANGUAGE)`

**Purpose:**
Searches the index for entries matching the search term and returns results for the specified page.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$term` | `string` | Search term. |
| `$page` | `int` | Page number (0-based). |
| `$language` | `string` | Language of the search term. |

**Return Values:**
- `array\|bool`: Associative array of results (keys: entry index, values: entry data), or `FALSE` on failure.

**Inner Mechanisms:**
1. Tokenizes the search term and preprocesses tokens (lowercase, unique, wildcard support).
2. Retrieves word indices matching the search tokens.
3. Queries the database for entries matching the words, ordered by the number of matches and total weight.
4. Retrieves entry data (address, title, text snippet, time, score, and number of similar entries).
5. Extracts a snippet of text containing the search term or the beginning of the text.

**Usage:**
- Called by frontend modules to display search results.

---

#### `search::tag($entry_index = NULL, $limit = 10, $language = CMS_LANGUAGE)`

**Purpose:**
Retrieves the most frequent words (tags) for all entries or a specific entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$entry_index` | `int\|NULL` | Index of the entry, or `NULL` for all entries. |
| `$limit` | `int` | Maximum number of tags to return. |
| `$language` | `string` | Language of the tags. |

**Return Values:**
- `array\|bool`: Associative array of tags (keys: word, values: score or weight), or `FALSE` on failure.

**Inner Mechanisms:**
1. If `$entry_index` is `NULL`, retrieves the most frequent words across all entries, weighted by their total weight.
2. If `$entry_index` is specified, retrieves the most frequent words for the entry.
3. Filters words by length (> 2 characters) and language.

**Usage:**
- Used to display tag clouds or entry-specific tags.

---

#### `search::score_compute()`

**Purpose:**
Computes relevance scores for all entries using a link-based algorithm. Updates the database and cache markers.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Checks if there are new or updated entries by comparing the auto-increment value and last update time with cached values.
2. Calls `score_compute_initialize()` to preprocess links and entries.
3. Calls `score_compute_iterate()` for `score_compute_iteration_number` iterations to refine scores.
4. Calls `score_compute_finalize()` to finalize scores.
5. Calls `score_compute_link_level()` to retrieve the maximum link level for dangling links.
6. Calls `score_compute_iterate()` for each dangling link level to refine their scores.
7. Updates cache markers for the auto-increment value and last update time.

**Usage:**
- Called periodically to update relevance scores.

---

#### `search::score_compute_initialize()`

**Purpose:**
Initializes the score computation process. Preprocesses links and entries to prepare for iteration.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.
- `string`: MySQL error message if a database error occurs.

**Inner Mechanisms:**
1. Resets link targets and sets their level to `1`.
2. Updates link targets to point to existing entries and sets their level to `0`.
3. Removes self-links.
4. Recursively deactivates dangling links (sets their level to > `0`).
5. Resets entry scores and outbound link counts.
6. Counts valid outbound links per entry and updates `link_count`.

**Usage:**
- Internal method called by `score_compute()`.

---

#### `search::score_compute_iterate($link_level = 0)`

**Purpose:**
Performs one iteration of the score computation algorithm for entries and dangling links.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$link_level` | `int` | Level of links to process (0 for entries, > 0 for dangling links). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.
- `string`: MySQL error message if a database error occurs.

**Inner Mechanisms:**
1. Computes the inherited score portion for each target entry based on the scores of its source entries.
2. Updates the score of each target entry using the formula:
   ```
   score = (1 - dampening_factor) + dampening_factor * sum(inherited_score_portion)
   ```

**Usage:**
- Internal method called by `score_compute()`.

---

#### `search::score_compute_finalize()`

**Purpose:**
Finalizes the score computation process. Updates the outbound link count for all entries.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.
- `string`: MySQL error message if a database error occurs.

**Inner Mechanisms:**
Counts all outbound links per entry (including dangling links) and updates `link_count`.

**Usage:**
- Internal method called by `score_compute()`.

---

#### `search::score_compute_link_level()`

**Purpose:**
Retrieves the maximum link level for dangling links.

**Return Values:**
- `int\|bool`: Maximum link level, or `FALSE` on failure.

**Inner Mechanisms:**
Queries the database for the maximum value of `CMS_DB_SEARCH_LINK_LEVEL`.

**Usage:**
- Internal method called by `score_compute()`.

---

#### `search::score_compute_canonical()`

**Purpose:**
Updates the canonical flag for entries. Sets the entry with the highest score in a cluster as canonical.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.
- `string`: MySQL error message if a database error occurs.

**Inner Mechanisms:**
1. Resets the canonical flag for all entries.
2. Sets the canonical flag to `0` for entries that have a higher-scoring alternative in their cluster.

**Usage:**
- Internal method called after score computation to update canonical entries.

---

#### `search::clean()`

**Purpose:**
Cleans up the search database by removing orphaned records and optimizing tables.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Removes orphaned records from `CMS_DB_SEARCH_WEIGHT`, `CMS_DB_SEARCH_WORD`, `CMS_DB_SEARCH_LINK`, and `CMS_DB_SEARCH_CLUSTER`.
2. Optimizes all search-related tables.

**Usage:**
- Called periodically to maintain database integrity and performance.

---

#### `search::address_standardize($address)`

**Purpose:**
Standardizes a URL by normalizing its components (scheme, host, path, query).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to standardize. |

**Return Values:**
- `string\|bool`: Standardized URL, or `FALSE` on failure.

**Inner Mechanisms:**
1. Parses the URL using `analyze_url()`.
2. Verifies the scheme (http/https only).
3. Sorts the query string parameters alphabetically.
4. Reassembles the URL using `cms_build_url()`.

**Usage:**
- Called by `scan()` and `queue_add()` to normalize URLs before processing.

---

#### `search::address_accepted($address)`

**Purpose:**
Checks if a URL is accepted for indexing based on blacklist and whitelist rules.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to check. |

**Return Values:**
- `bool`: `TRUE` if the URL is accepted, `FALSE` if rejected.

**Inner Mechanisms:**
1. Checks the URL against each pattern in `address_accepted_blacklist`.
2. If a pattern matches, checks the URL against `address_accepted_whitelist` for an override.
3. Returns `TRUE` if no blacklist pattern matches or if a whitelist pattern overrides the blacklist.

**Usage:**
- Called by `scan()` and `queue_add()` to filter URLs.


<!-- HASH:ba87f44e1cb70a0aa76a73ec752b09dc -->
