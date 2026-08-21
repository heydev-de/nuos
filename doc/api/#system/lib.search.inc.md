# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.search.inc)

- **Version:** `26.7.31.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Search Class

The `search` class provides a comprehensive full-text search engine for the PWNC Web Platform. It handles indexing, searching, and ranking of web content, including URL management, text processing, and link analysis. The class supports multilingual content, canonical URL detection, and incremental updates.

---

### Constants

#### Permission Constants
| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_PERMISSION_SUBMIT` | `"submit"` | Permission required to submit URLs for indexing. |

#### Database Constants
| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_ENTRY` | `CMS_DB_PREFIX . "search_entry"` | Table storing indexed entries. |
| `CMS_DB_SEARCH_ENTRY_INDEX` | `"id"` | Primary key for entries. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS` | `"address"` | URL of the indexed entry. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS_HASH` | `"address_hash"` | 32-bit hash of the URL. |
| `CMS_DB_SEARCH_ENTRY_TITLE` | `"title"` | Title of the indexed entry. |
| `CMS_DB_SEARCH_ENTRY_TEXT` | `"text"` | Extracted text content. |
| `CMS_DB_SEARCH_ENTRY_TEXT_HASH` | `"text_hash"` | 64-bit simhash for similarity detection. |
| `CMS_DB_SEARCH_ENTRY_TIME` | `"time"` | Timestamp of last indexing. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL` | `"update_interval"` | Time until next reindexing. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | `"update_time"` | Timestamp for next reindexing. |
| `CMS_DB_SEARCH_ENTRY_SCORE` | `"score"` | Ranking score of the entry. |
| `CMS_DB_SEARCH_ENTRY_LINK_COUNT` | `"link_count"` | Number of outbound links. |
| `CMS_DB_SEARCH_ENTRY_ERROR` | `"error"` | Error count for failed scans. |
| `CMS_DB_SEARCH_ENTRY_CANONICAL` | `"canonical"` | Flag indicating canonical entry. |
| `CMS_DB_SEARCH_WORD` | `CMS_DB_PREFIX . "search_word"` | Table storing unique words. |
| `CMS_DB_SEARCH_WORD_INDEX` | `"id"` | Primary key for words. |
| `CMS_DB_SEARCH_WORD_TEXT` | `"text"` | Word text. |
| `CMS_DB_SEARCH_WORD_LANGUAGE` | `"language"` | Language of the word. |
| `CMS_DB_SEARCH_WEIGHT` | `CMS_DB_PREFIX . "search_weight"` | Table storing word weights. |
| `CMS_DB_SEARCH_WEIGHT_WORD` | `"word"` | Foreign key to `CMS_DB_SEARCH_WORD`. |
| `CMS_DB_SEARCH_WEIGHT_SOURCE` | `"source"` | Foreign key to source entry. |
| `CMS_DB_SEARCH_WEIGHT_TARGET` | `"target"` | Foreign key to target entry. |
| `CMS_DB_SEARCH_WEIGHT_VALUE` | `"value"` | Weight of the word in the target. |
| `CMS_DB_SEARCH_LINK` | `CMS_DB_PREFIX . "search_link"` | Table storing links between entries. |
| `CMS_DB_SEARCH_LINK_SOURCE` | `"source"` | Foreign key to source entry. |
| `CMS_DB_SEARCH_LINK_TARGET_HASH` | `"target_hash"` | Hash of the target URL. |
| `CMS_DB_SEARCH_LINK_TARGET` | `"target"` | Foreign key to target entry. |
| `CMS_DB_SEARCH_LINK_TEXT` | `"text"` | Link text. |
| `CMS_DB_SEARCH_LINK_LEVEL` | `"level"` | Link level (0 = valid, >0 = dangling). |
| `CMS_DB_SEARCH_CLUSTER` | `CMS_DB_PREFIX . "search_cluster"` | Table storing clusters of similar entries. |
| `CMS_DB_SEARCH_CLUSTER_SOURCE` | `"source"` | Foreign key to source entry. |
| `CMS_DB_SEARCH_CLUSTER_TARGET` | `"target"` | Foreign key to target entry. |
| `CMS_DB_SEARCH_QUEUE` | `CMS_DB_PREFIX . "search_queue"` | Table storing URLs to be indexed. |
| `CMS_DB_SEARCH_QUEUE_ADDRESS` | `"address"` | URL to be indexed. |
| `CMS_DB_SEARCH_QUEUE_TYPE` | `"type"` | Type of queue entry. |
| `CMS_DB_SEARCH_QUEUE_TIME` | `"time"` | Timestamp for processing. |
| `CMS_DB_SEARCH_QUEUE_CODE` | `"code"` | Unique code for processing lock. |
| `CMS_DB_SEARCH_QUEUE_ERROR` | `"error"` | Error count for failed scans. |
| `CMS_DB_SEARCH_QUEUE_DONE` | `"done"` | Flag indicating completion. |

#### Scan Result Codes
| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_SCAN_UNKNOWN_ERROR` | `1` | Unknown error during scan. |
| `CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED` | `2` | Too many redirects. |
| `CMS_SEARCH_SCAN_INVALID_ADDRESS` | `4` | Invalid URL. |
| `CMS_SEARCH_SCAN_ADDRESS_REJECTED` | `8` | URL rejected by filters. |
| `CMS_SEARCH_SCAN_NO_CONNECTION` | `16` | Connection failed. |
| `CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT` | `32` | Unsupported content type. |
| `CMS_SEARCH_SCAN_NO_MODIFICATION` | `64` | No modification since last scan. |
| `CMS_SEARCH_SCAN_DATA_FETCH_FAILED` | `128` | Failed to fetch content. |
| `CMS_SEARCH_SCAN_NO_CONTENT` | `256` | No content found. |
| `CMS_SEARCH_SCAN_INDEXING_UNDESIRED` | `512` | Indexing blocked by robots directive. |
| `CMS_SEARCH_SCAN_INDEXED` | `1024` | Successfully indexed. |
| `CMS_SEARCH_SCAN_INDEXING_FAILED` | `2048` | Indexing failed. |
| `CMS_SEARCH_SCAN_DATABASE_ERROR` | `4096` | Database error. |
| `CMS_SEARCH_SCAN_FATAL_ERROR` | `CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED \| CMS_SEARCH_SCAN_INVALID_ADDRESS \| CMS_SEARCH_SCAN_ADDRESS_REJECTED \| CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT` | Fatal errors. |
| `CMS_SEARCH_SCAN_ERROR` | `CMS_SEARCH_SCAN_FATAL_ERROR \| CMS_SEARCH_SCAN_UNKNOWN_ERROR \| CMS_SEARCH_SCAN_NO_CONNECTION \| CMS_SEARCH_SCAN_DATA_FETCH_FAILED \| CMS_SEARCH_SCAN_NO_CONTENT \| CMS_SEARCH_SCAN_INDEXING_FAILED \| CMS_SEARCH_SCAN_DATABASE_ERROR` | All errors. |

#### Queue Types
| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_QUEUE_TYPE_NONE` | `0` | No type. |
| `CMS_SEARCH_QUEUE_TYPE_INTERNAL` | `1` | Internal URL. |
| `CMS_SEARCH_QUEUE_TYPE_SELECTION` | `2` | Selected URL. |
| `CMS_SEARCH_QUEUE_TYPE_SUBMISSION` | `4` | Submitted URL. |
| `CMS_SEARCH_QUEUE_TYPE_REFERENCE` | `8` | Discovered via link. |
| `CMS_SEARCH_QUEUE_TYPE_UPDATE` | `16` | Scheduled update. |
| `CMS_SEARCH_QUEUE_TYPE_ALL` | `255` | All types. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `enabled` | `FALSE` | Flag indicating if the search engine is enabled. |
| `address_accepted_blacklist` | `[]` | List of regex patterns for URL exclusion. |
| `address_accepted_whitelist` | `[]` | List of regex patterns for URL inclusion (applied after blacklist). |
| `scan_routing_limit` | `5` | Maximum number of redirects allowed. |
| `update_interval` | `86400` | Initial requeue interval in seconds (1 day). |
| `update_interval_min` | `3600` | Minimum requeue interval in seconds (1 hour). |
| `update_interval_max` | `604800` | Maximum requeue interval in seconds (1 week). |
| `update_interval_decrease_factor` | `0.5` | Factor to decrease requeue interval on major changes. |
| `update_interval_increase_factor` | `1.5` | Factor to increase requeue interval on minor changes. |
| `entry_set_maximum_bit_difference` | `2` | Maximum simhash bit differences for similarity. |
| `entry_set_weight_factor_title` | `0.1` | Weight factor for title text. |
| `entry_set_weight_factor_h1` | `1.3` | Weight factor for `<h1>` text. |
| `entry_set_weight_factor_h2` | `1.2` | Weight factor for `<h2>` text. |
| `entry_set_weight_factor_h3` | `1.1` | Weight factor for `<h3>` text. |
| `entry_set_weight_factor_copy` | `1.0` | Weight factor for body text. |
| `entry_set_weight_factor_side` | `0.1` | Weight factor for side text. |
| `entry_set_weight_factor_address` | `0.1` | Weight factor for URL text. |
| `entry_set_weight_factor_link` | `0.1` | Weight factor for link text. |
| `entry_remove_error_limit` | `3` | Number of errors before permanent removal. |
| `queue_process_retry_time` | `3600` | Time in seconds before retrying failed scans (1 hour). |
| `queue_process_error_limit` | `3` | Maximum number of retries for failed scans. |
| `find_results_per_page` | `10` | Number of results per page. |
| `score_compute_iteration_number` | `10` | Number of score refinement iterations. |
| `score_compute_dampening_factor` | `0.85` | Inherited portion of score (PageRank dampening). |

---

### Constructor

#### `__construct()`

**Purpose:**
Initializes the search engine, verifies database tables, and loads settings.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
- Establishes a database connection.
- Verifies the existence and structure of all required tables.
- Loads settings from the system configuration (e.g., blacklist/whitelist patterns, simhash difference threshold).

**Usage Context:**
Called automatically when the `search` class is instantiated. Ensures the search engine is ready for use.

**Example:**
```php
$search = new \cms\search();
if ($search->enabled) {
    // Search engine is ready
}
```

---

### Public Methods

#### `scan($address, $follow_links = FALSE)`

**Purpose:**
Scans and indexes a URL. Checks if the URL is already indexed and updates it if modified.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to scan. |
| `$follow_links` | `bool` | Whether to follow and queue discovered links. |

**Return Values:**
- `int`: One of the `CMS_SEARCH_SCAN_*` constants indicating the result.

**Inner Mechanisms:**
- Standardizes the URL.
- Checks if the URL is already indexed and if it has been modified since the last scan.
- Delegates to `_scan()` for actual scanning and indexing.
- Removes the entry on fatal errors.

**Usage Context:**
Used to manually trigger indexing of a URL. Can be called from a queue processor or directly.

**Example:**
```php
$search = new \cms\search();
$result = $search->scan("https://example.com", TRUE);
if ($result === CMS_SEARCH_SCAN_INDEXED) {
    echo "Successfully indexed.";
}
```

---

#### `entry_set($address, $data, $language, $links = NULL)`

**Purpose:**
Indexes or updates an entry in the search database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL of the entry. |
| `$data` | `array` | Parsed HTML data (title, text, links, etc.). |
| `$language` | `string` | Language of the content. |
| `$links` | `array\|NULL` | Optional array of outbound links. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.
- `string`: MySQL error message on database failure.

**Inner Mechanisms:**
- Standardizes and checks the URL.
- Processes outbound links.
- Checks for existing entries and updates them if modified.
- Computes requeue interval based on the degree of modification.
- Indexes text content with appropriate weights.
- Processes inbound and outbound links.
- Clusters similar entries based on simhash.

**Usage Context:**
Called internally by `scan()` to index or update an entry. Can be used directly for custom indexing.

**Example:**
```php
$search = new \cms\search();
$data = [
    "title" => "Example Page",
    "text" => "This is an example page.",
    "links" => [["url" => "https://example.com/link", "text" => "Link"]]
];
$result = $search->entry_set("https://example.com", $data, "en");
if ($result === TRUE) {
    echo "Entry indexed.";
}
```

---

#### `entry_remove($index, $force = TRUE)`

**Purpose:**
Removes an entry from the search index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | ID of the entry to remove. |
| `$force` | `bool` | If `FALSE`, increments error count instead of removing. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- If `force` is `FALSE`, increments the error count and removes canonical status.
- If `force` is `TRUE` or the error count exceeds the limit, removes the entry and all associated data (weights, links, clusters).

**Usage Context:**
Used to remove outdated or erroneous entries. Can be called manually or automatically on scan errors.

**Example:**
```php
$search = new \cms\search();
$search->entry_remove(123, FALSE); // Increment error count
$search->entry_remove(123, TRUE);  // Remove permanently
```

---

#### `queue_add($address, $type = CMS_SEARCH_QUEUE_TYPE_NONE)`

**Purpose:**
Adds a URL to the indexing queue.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to add. |
| `$type` | `int` | Type of queue entry (one of `CMS_SEARCH_QUEUE_TYPE_*`). |

**Return Values:**
- `bool`: `TRUE` if added or updated, `FALSE` on failure.

**Inner Mechanisms:**
- Standardizes and checks the URL.
- Adds the URL to the queue, updating the type and timestamp if it already exists.
- Special handling for `CMS_SEARCH_QUEUE_TYPE_REFERENCE` to avoid duplicates.

**Usage Context:**
Used to schedule URLs for indexing. Can be called manually or automatically when discovering links.

**Example:**
```php
$search = new \cms\search();
$search->queue_add("https://example.com", CMS_SEARCH_QUEUE_TYPE_SUBMISSION);
```

---

#### `queue_add_update()`

**Purpose:**
Adds all entries requiring updates to the queue.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` if entries were added, `FALSE` on failure.

**Inner Mechanisms:**
- Delegates to `queue_add_all(TRUE)` to add entries where `update_time` is in the past.

**Usage Context:**
Used to schedule periodic reindexing of all entries.

**Example:**
```php
$search = new \cms\search();
$search->queue_add_update();
```

---

#### `queue_add_all($update = FALSE)`

**Purpose:**
Adds all entries to the queue, optionally filtering by update time.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$update` | `bool` | If `TRUE`, only adds entries where `update_time` is in the past. |

**Return Values:**
- `bool`: `TRUE` if entries were added, `FALSE` on failure.

**Inner Mechanisms:**
- Queries the `CMS_DB_SEARCH_ENTRY` table and adds entries to the queue.
- Uses `ON DUPLICATE KEY UPDATE` to avoid duplicates.

**Usage Context:**
Used to bulk-schedule entries for indexing or updates.

**Example:**
```php
$search = new \cms\search();
$search->queue_add_all(); // Add all entries
$search->queue_add_all(TRUE); // Add only entries needing updates
```

---

#### `queue_remove($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

**Purpose:**
Marks queue entries as done.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Type of entries to mark as done (one of `CMS_SEARCH_QUEUE_TYPE_*`). |

**Return Values:**
- `bool`: `TRUE` if entries were marked, `FALSE` on failure.

**Inner Mechanisms:**
- Updates the `done` flag for entries matching the type.

**Usage Context:**
Used to clean up the queue after processing.

**Example:**
```php
$search = new \cms\search();
$search->queue_remove(CMS_SEARCH_QUEUE_TYPE_SUBMISSION);
```

---

#### `queue_length($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

**Purpose:**
Returns the number of pending queue entries.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Type of entries to count (one of `CMS_SEARCH_QUEUE_TYPE_*`). |

**Return Values:**
- `int\|bool`: Number of pending entries, or `FALSE` on failure.

**Inner Mechanisms:**
- Counts entries where `done` is `0` and the type matches.

**Usage Context:**
Used to monitor the queue size.

**Example:**
```php
$search = new \cms\search();
$count = $search->queue_length();
echo "Pending entries: $count";
```

---

#### `queue_process($type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

**Purpose:**
Processes the next entry in the queue.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Type of entries to process (one of `CMS_SEARCH_QUEUE_TYPE_*`). |
| `$follow_links` | `bool` | Whether to follow and queue discovered links. |

**Return Values:**
- `int\|bool`: One of the `CMS_SEARCH_SCAN_*` constants, or `FALSE` on failure.

**Inner Mechanisms:**
- Locks the next entry in the queue using a unique code.
- Calls `scan()` to process the URL.
- Retries on non-fatal errors if the error count is below the limit.
- Marks the entry as done on completion.

**Usage Context:**
Used by the daemon to process the queue. Can be called manually for testing.

**Example:**
```php
$search = new \cms\search();
$result = $search->queue_process();
if ($result === CMS_SEARCH_SCAN_INDEXED) {
    echo "Entry processed.";
}
```

---

#### `daemon_status($enabled = TRUE, $type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

**Purpose:**
Sets the status of the search daemon.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$enabled` | `bool` | Whether the daemon is enabled. |
| `$type` | `int` | Type of entries to process (one of `CMS_SEARCH_QUEUE_TYPE_*`). |
| `$follow_links` | `bool` | Whether to follow and queue discovered links. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Writes the daemon status to a file in `CMS_DATA_PATH`.

**Usage Context:**
Used to control the daemon from external scripts.

**Example:**
```php
$search = new \cms\search();
$search->daemon_status(TRUE, CMS_SEARCH_QUEUE_TYPE_ALL, TRUE);
```

---

#### `daemon_get_status()`

**Purpose:**
Retrieves the status of the search daemon.

**Parameters:**
None.

**Return Values:**
- `array\|bool`: Associative array with keys `enabled`, `type`, and `follow_links`, or `FALSE` on failure.

**Inner Mechanisms:**
- Reads the daemon status from a file in `CMS_DATA_PATH`.

**Usage Context:**
Used to check the daemon status from external scripts.

**Example:**
```php
$search = new \cms\search();
$status = $search->daemon_get_status();
if ($status["enabled"]) {
    echo "Daemon is running.";
}
```

---

#### `daemon($time_limit = 60)`

**Purpose:**
Runs the search daemon for a specified time limit.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$time_limit` | `int` | Maximum runtime in seconds. |

**Return Values:**
- `bool`: `TRUE` if the daemon completed processing, `FALSE` on failure or timeout.

**Inner Mechanisms:**
- Continuously processes the queue until the time limit is reached or the queue is empty.
- Checks the daemon status before each iteration.

**Usage Context:**
Used to run the daemon as a background process.

**Example:**
```php
$search = new \cms\search();
$search->daemon(300); // Run for 5 minutes
```

---

#### `find($term, $page = 0, $language = CMS_LANGUAGE)`

**Purpose:**
Searches the index for a given term.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$term` | `string` | Search term. |
| `$page` | `int` | Page number of results. |
| `$language` | `string` | Language of the search term. |

**Return Values:**
- `array\|bool`: Associative array of results, or `FALSE` on failure.

**Inner Mechanisms:**
- Tokenizes the search term.
- Retrieves word indices matching the tokens.
- Queries the database for entries containing the words, ordered by relevance.
- Extracts quotations or excerpts from the results.
- Returns an array of results with keys `address`, `title`, `text`, `time`, `score`, and `supplemental`.

**Usage Context:**
Used to implement search functionality in the frontend.

**Example:**
```php
$search = new \cms\search();
$results = $search->find("example", 0, "en");
if ($results) {
    foreach ($results as $result) {
        echo "<h2>{$result['title']}</h2>";
        echo "<p>{$result['text']}</p>";
    }
}
```

---

#### `tag($entry_index = NULL, $limit = 10, $language = CMS_LANGUAGE)`

**Purpose:**
Retrieves the most frequent words (tags) for an entry or the entire index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$entry_index` | `int\|NULL` | ID of the entry, or `NULL` for the entire index. |
| `$limit` | `int` | Maximum number of tags to return. |
| `$language` | `string` | Language of the tags. |

**Return Values:**
- `array\|bool`: Associative array of tags and their weights, or `FALSE` on failure.

**Inner Mechanisms:**
- If `$entry_index` is `NULL`, retrieves the most frequent words across all entries.
- If `$entry_index` is specified, retrieves the most frequent words for that entry.
- Filters words by length and language.

**Usage Context:**
Used to generate tag clouds or keyword lists for entries.

**Example:**
```php
$search = new \cms\search();
$tags = $search->tag(NULL, 20, "en");
if ($tags) {
    foreach ($tags as $tag => $weight) {
        echo "<span style='font-size: {$weight}em'>$tag</span> ";
    }
}
```

---

#### `score_compute()`

**Purpose:**
Computes the ranking scores for all entries.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Checks for new or updated entries.
- Initializes the score computation.
- Iteratively refines scores using a PageRank-like algorithm.
- Finalizes the computation and updates canonical entries.
- Handles dangling links (links to non-existent entries).

**Usage Context:**
Called periodically to update ranking scores. Can be triggered manually or via a cron job.

**Example:**
```php
$search = new \cms\search();
$search->score_compute();
```

---

#### `clean()`

**Purpose:**
Cleans up orphaned data in the search database.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Removes weights, words, links, and clusters associated with non-existent entries.
- Optimizes database tables.

**Usage Context:**
Called periodically to maintain database integrity. Can be triggered manually or via a cron job.

**Example:**
```php
$search = new \cms\search();
$search->clean();
```

---

#### `address_standardize($address)`

**Purpose:**
Standardizes a URL for consistent indexing.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to standardize. |

**Return Values:**
- `string\|bool`: Standardized URL, or `FALSE` on failure.

**Inner Mechanisms:**
- Parses the URL into components.
- Normalizes the scheme, host, port, path, and query string.
- Encodes the path and reorders the query string.

**Usage Context:**
Used internally to ensure consistent URL representation.

**Example:**
```php
$search = new \cms\search();
$standardized = $search->address_standardize("https://example.com:443/path?b=2&a=1");
echo $standardized; // "https://example.com/path?a=1&b=2"
```

---

#### `address_accepted($address)`

**Purpose:**
Checks if a URL is accepted for indexing.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to check. |

**Return Values:**
- `bool`: `TRUE` if accepted, `FALSE` if rejected.

**Inner Mechanisms:**
- Checks the URL against the blacklist and whitelist patterns.
- Supports inversion markers (`!`) for negative matching.

**Usage Context:**
Used internally to filter URLs before indexing.

**Example:**
```php
$search = new \cms\search();
$search->address_accepted_blacklist = ["/example\.com/"];
$search->address_accepted_whitelist = ["/example\.com\/allowed/"];
$accepted = $search->address_accepted("https://example.com/allowed");
echo $accepted ? "Accepted" : "Rejected"; // "Accepted"
```

---

### Private Methods

#### `_scan($address, $follow_links, $entry_time = 0)`

**Purpose:**
Internal method to scan and index a URL.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to scan. |
| `$follow_links` | `bool` | Whether to follow and queue discovered links. |
| `$entry_time` | `int` | Timestamp of the last scan (0 if new). |

**Return Values:**
- `int`: One of the `CMS_SEARCH_SCAN_*` constants.

**Inner Mechanisms:**
- Follows redirects up to the routing limit.
- Checks for robots directives and content type.
- Fetches and parses the HTML content.
- Queues discovered links if `$follow_links` is `TRUE`.
- Delegates to `entry_set()` for indexing.

**Usage Context:**
Called internally by `scan()`.

---

#### `_entry_set($source_index, $target_index, $text, $language, $weight_factor = 1.0)`

**Purpose:**
Internal method to index text content and compute word weights.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$source_index` | `int` | ID of the source entry. |
| `$target_index` | `int` | ID of the target entry. |
| `$text` | `string` | Text to index. |
| `$language` | `string` | Language of the text. |
| `$weight_factor` | `float` | Weight factor for the text. |

**Return Values:**
- `bool`: `TRUE` on success.
- `string`: MySQL error message on failure.

**Inner Mechanisms:**
- Strips stop words and tokenizes the text.
- Counts word occurrences and computes weights.
- Inserts words into the word table and weights into the weight table.

**Usage Context:**
Called internally by `entry_set()`.

---


<!-- HASH:03903df894bee460d9a2956bcd9467a4 -->
