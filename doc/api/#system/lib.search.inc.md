# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.search.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

The `search` class in `#system/lib.search.inc` implements a full-text search engine for the PWNC Web Platform. It provides web crawling, content indexing, link graph analysis, PageRank-style scoring, and query-based search functionality. The system maintains six database tables: `search_entry` (indexed pages), `search_word` (unique words), `search_weight` (word-to-entry weights), `search_link` (inbound/outbound link relationships), `search_cluster` (similar-content groups), and `search_queue` (crawling task queue).

The class is instantiated once per request and operates as a singleton-like service. It supports incremental crawling with adaptive requeue intervals, simhash-based duplicate detection, canonical entry selection, and a daemon mode for background processing.

## Constants

### Permission Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_PERMISSION_SUBMIT` | `"submit"` | Permission key for submitting URLs to the search index |

### Database Table Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_ENTRY` | `CMS_DB_PREFIX . "search_entry"` | Table storing indexed page entries |
| `CMS_DB_SEARCH_WORD` | `CMS_DB_PREFIX . "search_word"` | Table storing unique words |
| `CMS_DB_SEARCH_WEIGHT` | `CMS_DB_PREFIX . "search_weight"` | Table storing word-to-entry weight associations |
| `CMS_DB_SEARCH_LINK` | `CMS_DB_PREFIX . "search_link"` | Table storing link relationships between entries |
| `CMS_DB_SEARCH_CLUSTER` | `CMS_DB_PREFIX . "search_cluster"` | Table storing similarity clusters between entries |
| `CMS_DB_SEARCH_QUEUE` | `CMS_DB_PREFIX . "search_queue"` | Table storing the crawling task queue |

### Database Column Constants

#### `search_entry` Table Columns

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_ENTRY_INDEX` | `"id"` | Primary key (auto-increment) |
| `CMS_DB_SEARCH_ENTRY_ADDRESS` | `"address"` | Full URL of the indexed page |
| `CMS_DB_SEARCH_ENTRY_ADDRESS_HASH` | `"address_hash"` | 16-byte binary hash of the address |
| `CMS_DB_SEARCH_ENTRY_TITLE` | `"title"` | Page title (max 255 chars) |
| `CMS_DB_SEARCH_ENTRY_TEXT` | `"text"` | Page text content (max 65535 chars) |
| `CMS_DB_SEARCH_ENTRY_TEXT_HASH` | `"text_hash"` | 64-bit simhash of the text content |
| `CMS_DB_SEARCH_ENTRY_TIME` | `"time"` | Last modification timestamp |
| `CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL` | `"update_interval"` | Seconds between re-crawls |
| `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | `"update_time"` | Next scheduled crawl time |
| `CMS_DB_SEARCH_ENTRY_SCORE` | `"score"` | PageRank-style score (0.0–1.0) |
| `CMS_DB_SEARCH_ENTRY_LINK_COUNT` | `"link_count"` | Number of outbound links |
| `CMS_DB_SEARCH_ENTRY_ERROR` | `"error"` | Consecutive error count |
| `CMS_DB_SEARCH_ENTRY_CANONICAL` | `"canonical"` | Whether this is the canonical entry in its cluster |

#### `search_word` Table Columns

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_WORD_INDEX` | `"id"` | Primary key |
| `CMS_DB_SEARCH_WORD_TEXT` | `"text"` | The word text (binary) |
| `CMS_DB_SEARCH_WORD_LANGUAGE` | `"language"` | Language code (ASCII) |

#### `search_weight` Table Columns

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_WEIGHT_WORD` | `"word"` | Foreign key to `search_word.id` |
| `CMS_DB_SEARCH_WEIGHT_SOURCE` | `"source"` | Foreign key to `search_entry.id` (source page) |
| `CMS_DB_SEARCH_WEIGHT_TARGET` | `"target"` | Foreign key to `search_entry.id` (target page) |
| `CMS_DB_SEARCH_WEIGHT_VALUE` | `"value"` | Weight value (TF-IDF-like) |

#### `search_link` Table Columns

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_LINK_SOURCE` | `"source"` | Foreign key to `search_entry.id` (linking page) |
| `CMS_DB_SEARCH_LINK_TARGET_HASH` | `"target_hash"` | 16-byte binary hash of target address |
| `CMS_DB_SEARCH_LINK_TARGET` | `"target"` | Foreign key to `search_entry.id` (linked page) |
| `CMS_DB_SEARCH_LINK_TEXT` | `"text"` | Anchor text of the link |
| `CMS_DB_SEARCH_LINK_LEVEL` | `"level"` | Link depth level (0 = direct, >0 = dangling) |

#### `search_cluster` Table Columns

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_CLUSTER_SOURCE` | `"source"` | Foreign key to `search_entry.id` |
| `CMS_DB_SEARCH_CLUSTER_TARGET` | `"target"` | Foreign key to `search_entry.id` |

#### `search_queue` Table Columns

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_SEARCH_QUEUE_ADDRESS` | `"address"` | URL to crawl |
| `CMS_DB_SEARCH_QUEUE_TYPE` | `"type"` | Bitmask of queue type flags |
| `CMS_DB_SEARCH_QUEUE_TIME` | `"time"` | Scheduled processing time |
| `CMS_DB_SEARCH_QUEUE_CODE` | `"code"` | Unique processing lock identifier |
| `CMS_DB_SEARCH_QUEUE_ERROR` | `"error"` | Error count for this queue item |
| `CMS_DB_SEARCH_QUEUE_DONE` | `"done"` | Whether the item has been processed |

### Scan Result Code Constants

These are bit-flag constants returned by `scan()` and `_scan()`:

| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_SCAN_UNKNOWN_ERROR` | `1` | Unknown error occurred |
| `CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED` | `2` | Too many HTTP redirects |
| `CMS_SEARCH_SCAN_INVALID_ADDRESS` | `4` | URL is invalid |
| `CMS_SEARCH_SCAN_ADDRESS_REJECTED` | `8` | URL rejected by blacklist/whitelist |
| `CMS_SEARCH_SCAN_NO_CONNECTION` | `16` | HTTP connection failed or server error (5xx) |
| `CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT` | `32` | Content type is not HTML/XML |
| `CMS_SEARCH_SCAN_NO_MODIFICATION` | `64` | Page has not been modified since last scan |
| `CMS_SEARCH_SCAN_DATA_FETCH_FAILED` | `128` | Failed to fetch page content |
| `CMS_SEARCH_SCAN_NO_CONTENT` | `256` | Page has no text content |
| `CMS_SEARCH_SCAN_INDEXING_UNDESIRED` | `512` | `noindex` or `none` robots directive |
| `CMS_SEARCH_SCAN_INDEXED` | `1024` | Page was successfully indexed |
| `CMS_SEARCH_SCAN_INDEXING_FAILED` | `2048` | Indexing failed (database error) |
| `CMS_SEARCH_SCAN_DATABASE_ERROR` | `4096` | Database error during indexing |
| `CMS_SEARCH_SCAN_FATAL_ERROR` | `2\|4\|8\|32` | Combination of fatal error flags |
| `CMS_SEARCH_SCAN_ERROR` | `CMS_SEARCH_SCAN_FATAL_ERROR \| 1\|16\|128\|256\|2048\|4096` | All error flags combined |

### Queue Type Constants

Bitmask flags for queue item types:

| Name | Value | Description |
|------|-------|-------------|
| `CMS_SEARCH_QUEUE_TYPE_NONE` | `0` | No type |
| `CMS_SEARCH_QUEUE_TYPE_INTERNAL` | `1` | Internal system task |
| `CMS_SEARCH_QUEUE_TYPE_SELECTION` | `2` | User-selected URL |
| `CMS_SEARCH_QUEUE_TYPE_SUBMISSION` | `4` | User-submitted URL |
| `CMS_SEARCH_QUEUE_TYPE_REFERENCE` | `8` | Discovered via link crawling |
| `CMS_SEARCH_QUEUE_TYPE_UPDATE` | `16` | Re-crawl existing entry |
| `CMS_SEARCH_QUEUE_TYPE_ALL` | `255` | All types |

## Properties

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enabled` | `bool` | `FALSE` | Whether the search system is active |
| `$address_accepted_blacklist` | `array` | `[]` | Regex patterns for URL exclusion |
| `$address_accepted_whitelist` | `array` | `[]` | Regex patterns for URL inclusion (overrides blacklist) |
| `$scan_routing_limit` | `int` | `5` | Maximum HTTP redirects to follow |
| `$update_interval` | `int` | `86400` | Initial requeue interval in seconds (1 day) |
| `$update_interval_min` | `int` | `3600` | Minimum requeue interval (1 hour) |
| `$update_interval_max` | `int` | `604800` | Maximum requeue interval (1 week) |
| `$update_interval_decrease_factor` | `float` | `0.5` | Factor to decrease interval on significant changes |
| `$update_interval_increase_factor` | `float` | `1.5` | Factor to increase interval on no changes |
| `$entry_set_maximum_bit_difference` | `int` | `2` | Max simhash bit differences for similarity |
| `$entry_set_weight_factor_title` | `float` | `0.1` | Weight factor for page titles |
| `$entry_set_weight_factor_h1` | `float` | `1.3` | Weight factor for H1 headings |
| `$entry_set_weight_factor_h2` | `float` | `1.2` | Weight factor for H2 headings |
| `$entry_set_weight_factor_h3` | `float` | `1.1` | Weight factor for H3 headings |
| `$entry_set_weight_factor_copy` | `float` | `1.0` | Weight factor for body copy |
| `$entry_set_weight_factor_side` | `float` | `0.1` | Weight factor for sidebar content |
| `$entry_set_weight_factor_address` | `float` | `0.1` | Weight factor for URL address text |
| `$entry_set_weight_factor_link` | `float` | `0.1` | Weight factor for link anchor text |
| `$entry_remove_error_limit` | `int` | `3` | Error count before permanent removal |
| `$queue_process_retry_time` | `int` | `3600` | Seconds before retrying a failed scan |
| `$queue_process_error_limit` | `int` | `3` | Max retries before giving up |
| `$find_results_per_page` | `int` | `10` | Search results per page |
| `$score_compute_iteration_number` | `int` | `10` | PageRank iterations |
| `$score_compute_dampening_factor` | `float` | `0.85` | PageRank dampening factor |

## Methods

### `__construct()`

Initializes the search system by verifying all six database tables exist with the correct schema, then loads configuration settings from the system.

**Parameters:** None

**Return value:** `void`

**Inner mechanisms:**
1. Creates a `mysql` instance and calls `verify_table()` for each of the six tables, defining column types, primary keys, unique constraints, and indexes.
2. If all tables verify successfully, loads blacklist/whitelist regex patterns from system settings (split by newlines).
3. Loads the simhash bit-difference threshold from system settings.
4. Sets `$enabled = TRUE` only if all tables verified.

**Usage context:** Called automatically when the `search` class is instantiated. The system must have the `search` module enabled and the database tables must be creatable.

```php
$search = new search();
if ($search->enabled) {
    // Search system is ready
}
```

### `scan($address, $follow_links = FALSE)`

Scans a URL, retrieves its content, and indexes it. If the entry already exists, checks whether it has been modified before re-indexing.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | `string` | — | URL to scan |
| `$follow_links` | `bool` | `FALSE` | Whether to follow and queue outbound links |

**Return value:** `int` — A `CMS_SEARCH_SCAN_*` bit-flag constant indicating the scan result.

**Inner mechanisms:**
1. Returns `CMS_SEARCH_SCAN_UNKNOWN_ERROR` if search is disabled or required libraries (`html`, `http`) cannot be loaded.
2. Standardizes the address via `address_standardize()`.
3. Queries the database for an existing entry by address hash.
4. If found, calls `_scan()` with the existing entry's timestamp to check for modifications.
5. If the scan result indicates an error, calls `entry_remove()` to handle cleanup.
6. If no existing entry, calls `_scan()` without a timestamp.

**Usage context:** Used by the daemon and queue processor to crawl and index URLs. Can also be called directly for one-off indexing.

```php
$search = new search();
$result = $search->scan("https://example.com/page", TRUE);
if ($result === CMS_SEARCH_SCAN_INDEXED) {
    echo "Page indexed successfully";
} elseif ($result === CMS_SEARCH_SCAN_NO_MODIFICATION) {
    echo "Page unchanged, skipping";
}
```

### `_scan($address, $follow_links, $entry_time = 0)`

Internal method that performs the actual HTTP request, content retrieval, and indexing.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | `string` | — | Standardized URL to scan |
| `$follow_links` | `bool` | — | Whether to follow outbound links |
| `$entry_time` | `int` | `0` | Timestamp of last indexed version (for modification check) |

**Return value:** `int` — A `CMS_SEARCH_SCAN_*` bit-flag constant.

**Inner mechanisms:**
1. **Redirect handling:** Loops up to `$scan_routing_limit` times, following `Location` headers. Returns `CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED` if exceeded.
2. **Address admission:** Checks `address_accepted()` against blacklist/whitelist. Returns `CMS_SEARCH_SCAN_ADDRESS_REJECTED` if rejected.
3. **HTTP header retrieval:** Uses `http_header()`. Returns `CMS_SEARCH_SCAN_NO_CONNECTION` on failure or 5xx status.
4. **Robots meta check:** Checks `x-robots-tag` header for `noindex`/`none`. Returns `CMS_SEARCH_SCAN_INDEXING_UNDESIRED`.
5. **Content type verification:** Only accepts `text/html`, `application/xhtml+xml`, `application/xml`. Returns `CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT` otherwise.
6. **Error count reset:** Resets the entry's error count to 0 if the address is reachable.
7. **Modification check:** Compares `Last-Modified` header with `$entry_time`. Returns `CMS_SEARCH_SCAN_NO_MODIFICATION` if unchanged.
8. **Content retrieval:** Uses `html_page_info()` to fetch and parse the page. Returns `CMS_SEARCH_SCAN_DATA_FETCH_FAILED` on failure.
9. **Content validation:** Returns `CMS_SEARCH_SCAN_NO_CONTENT` if text is empty.
10. **Link queuing:** If `$follow_links` is true and robots directives allow, queues all discovered links as `CMS_SEARCH_QUEUE_TYPE_REFERENCE`.
11. **Meta robots check:** Checks `<meta name="robots">` for `noindex`/`none`. Returns `CMS_SEARCH_SCAN_INDEXING_UNDESIRED`.
12. **Language detection:** Extracts language from meta tags, HTTP headers, or auto-detection.
13. **Indexing:** Calls `entry_set()` and returns `CMS_SEARCH_SCAN_INDEXED` on success, `CMS_SEARCH_SCAN_INDEXING_FAILED` on failure, or `CMS_SEARCH_SCAN_DATABASE_ERROR` on database errors.

**Usage context:** Internal method called by `scan()`. Not intended for direct use.

### `entry_set($address, $data, $language, $links = NULL)`

Creates or updates a search entry in the index. This is the core indexing method that processes page content, extracts words, builds link relationships, and clusters similar entries.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | `string` | — | URL of the page |
| `$data` | `array` | — | Parsed page data with keys: `title`, `text`, `h1`, `h2`, `h3`, `copy`, `side`, `links` |
| `$language` | `string` | — | Language code for the page |
| `$links` | `array\|NULL` | `NULL` | Optional pre-processed links (not used in current implementation) |

**Return value:** `bool\|string` — `TRUE` on success, `FALSE` if disabled or address rejected, or a MySQL error string on database failure.

**Inner mechanisms:**
1. **Address processing:** Standardizes the address and checks admission. Returns `FALSE` for invalid addresses, `TRUE` for rejected addresses (skip indexing).
2. **Hash computation:** Computes address hash (`hash32`), text simhash, and truncates title/text.
3. **Link preprocessing:** Standardizes each link URL, hashes it, removes self-links, and cleanses anchor text.
4. **Existing entry check:** Queries for an existing entry by address hash.
5. **If entry exists:**
   - If title and text are unchanged, increases the requeue interval (exponential backoff) and returns `TRUE`.
   - If changed, deletes all previous word weights, links, and cluster associations, then updates the entry with new content.
   - Computes simhash delta to determine whether to increase or decrease the requeue interval.
6. **If new entry:**
   - Checks for similar canonical entries using simhash comparison.
   - Inserts the new entry with initial score of 1.0.
7. **Word indexing:** For each content type (title, h1, h2, h3, copy, side, address), calls `_entry_set()` to tokenize, strip stopwords, insert words, and compute TF-IDF-like weights.
8. **Outbound link processing:** For each outbound link, looks up the target entry index and calls `_entry_set()` to index the anchor text as a weighted word association.
9. **Inbound link processing:** Queries for entries that link to this page and indexes their anchor text.
10. **Similarity clustering:** Finds entries with similar simhash values and creates bidirectional cluster associations.

**Usage context:** Called by `_scan()` after content retrieval. Can also be called directly to index pre-parsed content.

```php
$search = new search();
$data = [
    "title" => "Example Page",
    "text" => "This is the main content of the page...",
    "h1" => "Main Heading",
    "h2" => "Subheading",
    "h3" => "",
    "copy" => "Body text content",
    "side" => "Sidebar text",
    "links" => [
        ["url" => "https://example.com/other", "text" => "Other Page"]
    ]
];
$result = $search->entry_set("https://example.com/page", $data, "en");
if ($result === TRUE) {
    echo "Entry indexed";
}
```

### `_entry_set($source_index, $target_index, $text, $language, $weight_factor = 1.0)`

Internal method that tokenizes text, inserts words into the word table, and computes/stores word weights.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$source_index` | `int` | — | Entry ID of the source page |
| `$target_index` | `int` | — | Entry ID of the target page |
| `$text` | `string` | — | Text content to index |
| `$language` | `string` | — | Language code for stopword filtering |
| `$weight_factor` | `float` | `1.0` | Maximum weight multiplier |

**Return value:** `bool\|string` — `TRUE` on success, or a MySQL error string on failure.

**Inner mechanisms:**
1. **Stopword removal:** Calls `language_strip_stopword()` to remove common words.
2. **Tokenization:** Calls `tokenize_text()` to split text into words.
3. **Word counting:** Counts occurrences of each word (case-insensitive via `utf8_strtolower`).
4. **Word insertion:** Inserts new words into `search_word` table using `INSERT IGNORE`.
5. **Word lookup:** Retrieves word IDs for all words in the current language.
6. **Weight computation:** For each word, computes weight as `(1/total_words * weight_factor) * sqrt(occurrence_count)`.
7. **Weight storage:** Inserts or updates weights in `search_weight` table using `ON DUPLICATE KEY UPDATE` to accumulate weights.

**Usage context:** Internal method called by `entry_set()` for each content type and link. Not intended for direct use.

### `entry_remove($index, $force = TRUE)`

Removes an entry from the search index, either permanently or by incrementing the error count.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | — | Entry ID to remove |
| `$force` | `bool` | `TRUE` | If `TRUE`, remove permanently; if `FALSE`, increment error count first |

**Return value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanisms:**
1. If `$force` is `FALSE`, queries the current error count. If it's below `$entry_remove_error_limit`, increments the error count and sets `canonical = 0`, then returns.
2. If `$force` is `TRUE` or error limit is reached, permanently deletes the entry from `search_entry`.
3. Cleans up all related data: word weights (where source or target matches), outbound links, and cluster associations.

**Usage context:** Called by `scan()` when a scan results in a fatal error. Can also be called directly to remove entries.

```php
$search = new search();
$search->entry_remove(42, TRUE); // Permanently remove entry 42
```

### `queue_add($address, $type = CMS_SEARCH_QUEUE_TYPE_NONE)`

Adds a URL to the crawling queue.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | `string` | — | URL to add to queue |
| `$type` | `int` | `CMS_SEARCH_QUEUE_TYPE_NONE` | Queue type bitmask |

**Return value:** `bool` — `TRUE` if the queue was modified, `FALSE` otherwise.

**Inner mechanisms:**
1. Standardizes and validates the address.
2. If the type includes `CMS_SEARCH_QUEUE_TYPE_REFERENCE`, uses `INSERT IGNORE` (discovered links are only added once).
3. Otherwise, uses `INSERT ... ON DUPLICATE KEY UPDATE` to update the type, time, and reset error/done flags for existing queue entries.

**Usage context:** Called by `_scan()` to queue discovered links, and by external code to submit URLs for crawling.

```php
$search = new search();
$search->queue_add("https://example.com/new-page", CMS_SEARCH_QUEUE_TYPE_SUBMISSION);
```

### `queue_add_update()`

Adds update tasks for all existing entries to the queue.

**Parameters:** None

**Return value:** `bool` — `TRUE` if the queue was modified.

**Inner mechanisms:** Calls `queue_add_all(TRUE)` which inserts all entries from `search_entry` into `search_queue` with type `CMS_SEARCH_QUEUE_TYPE_UPDATE`, but only for entries whose `update_time` has passed.

**Usage context:** Used to schedule re-crawling of all indexed pages.

```php
$search = new search();
$search->queue_add_update(); // Schedule all entries for re-crawling
```

### `queue_add_all($update = FALSE)`

Adds entries to the queue, optionally only those due for update.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$update` | `bool` | `FALSE` | If `TRUE`, only add entries whose `update_time` has passed |

**Return value:** `bool` — `TRUE` if the queue was modified.

**Inner mechanisms:** Inserts all entries from `search_entry` into `search_queue` with type `CMS_SEARCH_QUEUE_TYPE_UPDATE`. If `$update` is `TRUE`, filters by `update_time < current_time`. Uses `ON DUPLICATE KEY UPDATE` to avoid duplicates.

**Usage context:** Called by `queue_add_update()`. Can be called directly to queue all entries.

### `queue_remove($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

Marks queue entries as done (processed).

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$type` | `int` | `CMS_SEARCH_QUEUE_TYPE_ALL` | Queue type bitmask to filter |

**Return value:** `bool` — `TRUE` if any rows were affected.

**Inner mechanisms:** Updates `search_queue` setting `done = 1` for entries matching the type filter.

**Usage context:** Used to clean up the queue after processing.

### `queue_length($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

Returns the number of pending queue items.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$type` | `int` | `CMS_SEARCH_QUEUE_TYPE_ALL` | Queue type bitmask to filter |

**Return value:** `int\|bool` — Count of pending items, or `FALSE` on failure.

**Inner mechanisms:** Counts rows in `search_queue` where `done = 0` and optionally filtered by type.

**Usage context:** Used to monitor queue size and determine if the daemon should continue processing.

```php
$search = new search();
$count = $search->queue_length(CMS_SEARCH_QUEUE_TYPE_ALL);
echo "Pending items: $count";
```

### `queue_process($type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

Processes the next item in the queue by scanning its URL.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$type` | `int` | `CMS_SEARCH_QUEUE_TYPE_ALL` | Queue type bitmask to filter |
| `$follow_links` | `bool` | `FALSE` | Whether to follow outbound links during scan |

**Return value:** `int\|bool` — Scan result code on success, `FALSE` if no items to process.

**Inner mechanisms:**
1. Generates a unique processing code and marks one queue item for processing (using the code as a lock).
2. Fetches the marked queue item's address and error count.
3. Calls `scan()` on the address.
4. If a non-fatal error occurs and error count is below the limit, increments the error count and returns the scan result.
5. If successful or fatal error, marks the queue item as done.
6. Returns the scan result code.

**Usage context:** Called by the daemon and can be called directly for batch processing.

```php
$search = new search();
$result = $search->queue_process(CMS_SEARCH_QUEUE_TYPE_ALL, TRUE);
if ($result === CMS_SEARCH_SCAN_INDEXED) {
    echo "Processed and indexed a page";
}
```

### `daemon_status($enabled = TRUE, $type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

Writes the daemon status to a file.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enabled` | `bool` | `TRUE` | Whether the daemon should be active |
| `$type` | `int` | `CMS_SEARCH_QUEUE_TYPE_ALL` | Queue type to process |
| `$follow_links` | `bool` | `FALSE` | Whether to follow links during scanning |

**Return value:** `bool` — Result of `write_file()`.

**Inner mechanisms:** Writes a semicolon-delimited string of the three parameters to `#system/search.daemon.status`.

**Usage context:** Used to configure the daemon before starting it.

```php
$search = new search();
$search->daemon_status(TRUE, CMS_SEARCH_QUEUE_TYPE_ALL, TRUE);
```

### `daemon_get_status()`

Reads the daemon status from file.

**Parameters:** None

**Return value:** `array\|bool` — Array with keys `enabled`, `type`, `follow_links`, or `FALSE` on failure.

**Inner mechanisms:** Reads and parses the status file written by `daemon_status()`.

**Usage context:** Called by `daemon()` to check if it should continue running.

### `daemon($time_limit = 60)`

Runs the search daemon, processing queue items in a loop.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$time_limit` | `int` | `60` | Maximum seconds to run before yielding |

**Return value:** `bool` — `TRUE` if the daemon completed its time slice, `FALSE` if disabled or status unavailable.

**Inner mechanisms:**
1. Loops until the time limit is exceeded.
2. Checks daemon status; exits if disabled.
3. Calls `queue_process()` to process the next item.
4. If no items to process, returns `TRUE`.
5. Sleeps 1 second between iterations.

**Usage context:** Intended to be called periodically (e.g., via cron) to process the search queue in the background.

```php
$search = new search();
$search->daemon(300); // Run for up to 5 minutes
```

### `find($term, $page = 0, $language = CMS_LANGUAGE)`

Searches the index for pages matching the given search term.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$term` | `string` | — | Search query string |
| `$page` | `int` | `0` | Page number (0-indexed) |
| `$language` | `string` | `CMS_LANGUAGE` | Language to filter words by |

**Return value:** `array\|bool` — Associative array of results keyed by entry ID, or `FALSE` on failure.

**Inner mechanisms:**
1. **Tokenization:** Splits the search term into tokens, lowercases them, and filters for words longer than 2 characters.
2. **Wildcard matching:** For longer words, creates `LIKE` patterns for partial matching.
3. **Word lookup:** Queries `search_word` for matching word IDs, using exact match for short words and `LIKE` for longer words.
4. **Result retrieval:** Queries `search_weight` joined with `search_entry` to find entries containing matching words, grouped by entry ID.
5. **Scoring:** Orders results by: (1) number of matching tokens, (2) sum of word weights × entry score.
6. **Pagination:** Limits results to `$find_results_per_page` per page.
7. **Data enrichment:** For each result, retrieves full entry data, extracts a text quotation around the search term, and counts similar entries (cluster size).

**Usage context:** Called by the search frontend to display results.

```php
$search = new search();
$results = $search->find("php mysql tutorial", 0, "en");
if ($results) {
    foreach ($results as $id => $entry) {
        echo "<h3>{$entry['title']}</h3>";
        echo "<p>{$entry['text']}</p>";
        echo "<p>Score: {$entry['score']}, Similar: {$entry['supplemental']}</p>";
    }
}
```

### `tag($entry_index = NULL, $limit = 10, $language = CMS_LANGUAGE)`

Retrieves the most frequently used words (tags) either across all entries or for a specific entry.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$entry_index` | `int\|NULL` | `NULL` | Entry ID to get tags for, or `NULL` for all entries |
| `$limit` | `int` | `10` | Maximum number of tags to return |
| `$language` | `string` | `CMS_LANGUAGE` | Language to filter words by |

**Return value:** `array\|bool` — Associative array of word => score, or `FALSE` on failure.

**Inner mechanisms:**
- **All entries mode (`$entry_index === NULL`):** Computes each word's total weight as a percentage of the sum of all weights, then returns the top `$limit` words.
- **Single entry mode:** Returns the top `$limit` words by weight for the specified entry.
- Both modes filter by language (if specified) and require words to be longer than 2 characters.

**Usage context:** Used to generate tag clouds for the search index.

```php
$search = new search();
$tags = $search->tag(NULL, 20, "en"); // Top 20 words across all entries
foreach ($tags as $word => $score) {
    echo "<span style='font-size: " . ($score * 10) . "em'>$word</span>";
}
```

### `score_compute()`

Computes PageRank-style scores for all entries. This is the main entry point for score computation.

**Parameters:** None

**Return value:** `bool` — `TRUE` on success, `FALSE` on failure.

**Inner mechanisms:**
1. **Change detection:** Checks if the `search_entry` table's auto-increment value or any entry's modification time has changed since the last computation. If not, returns `TRUE` immediately.
2. **Initialization:** Calls `score_compute_initialize()` to reset link levels and scores.
3. **Iteration:** Runs `$score_compute_iteration_number` iterations of `score_compute_iterate()`.
4. **Finalization:** Calls `score_compute_finalize()` to update link counts.
5. **Dangling link processing:** Retrieves the maximum link level and iterates scores for dangling links from highest to lowest level.
6. **Cache update:** Stores the current hash and time in cache to prevent redundant computation.

**Usage context:** Called periodically (e.g., via cron) to update entry scores.

```php
$search = new search();
$search->score_compute(); // Recompute all scores
```

### `score_compute_initialize()`

Initializes the score computation by resetting link levels and entry scores.

**Parameters:** None

**Return value:** `bool\|string` — `TRUE` on success, or a MySQL error string.

**Inner mechanisms:**
1. Resets all link targets to 0 and levels to 1.
2. Updates link targets by matching `target_hash` to entry address hashes, setting level to 0 for matched links.
3. Removes self-links (where source = target).
4. Recursively deactivates dangling links (links whose targets don't exist) by incrementing their level.
5. Resets all entry scores to 1.0 (for error-free entries) or 0.0 (for entries with errors).
6. Resets all link counts to 0.
7. Counts valid outbound links per source entry and stores in `link_count`.

**Usage context:** Called by `score_compute()` as the first step.

### `score_compute_iterate($link_level = 0)`

Performs one iteration of the PageRank-style score computation.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$link_level` | `int` | `0` | Link level to process (0 = direct links) |

**Return value:** `bool\|string` — `TRUE` on success, or a MySQL error string.

**Inner mechanisms:**
1. Computes the inherited score portion for each target: `source_score / source_link_count`.
2. Computes the new score for each target: `(1 - dampening_factor) + dampening_factor * SUM(inherited_portions)`.
3. Updates all entry scores in a single query.

**Usage context:** Called by `score_compute()` in a loop.

### `score_compute_finalize()`

Finalizes score computation by updating link counts for all entries.

**Parameters:** None

**Return value:** `bool\|string` — `TRUE` on success, or a MySQL error string.

**Inner mechanisms:** Counts all outbound links per source entry (including dangling ones) and updates `link_count`.

**Usage context:** Called by `score_compute()` after all iterations.

### `score_compute_link_level()`

Returns the maximum link level in the link table.

**Parameters:** None

**Return value:** `int\|bool` — Maximum link level, or `FALSE` on failure.

**Inner mechanisms:** Queries `MAX(link_level)` from `search_link`.

**Usage context:** Called by `score_compute()` to determine how many dangling link iterations to perform.

### `score_compute_canonical()`

Determines which entries in each similarity cluster are canonical.

**Parameters:** None

**Return value:** `bool\|string` — `TRUE` on success, or a MySQL error string.

**Inner mechanisms:**
1. Resets all entries to `canonical = 1`.
2. For each cluster pair, if the target entry has a higher score than the source, sets the source's `canonical = 0`.
3. If scores are equal, the entry with the lower ID is marked non-canonical.

**Usage context:** Called after score computation to ensure only the best entry in each cluster appears in search results.

```php
$search = new search();
$search->score_compute_canonical();
```

### `clean()`

Removes orphaned data from all search tables and optimizes them.

**Parameters:** None

**Return value:** `bool` — `TRUE` if all optimizations succeeded.

**Inner mechanisms:**
1. Deletes weights where source or target entry no longer exists.
2. Deletes words that have no associated weights.
3. Deletes links where the source entry no longer exists.
4. Deletes cluster associations where either source or target no longer exists.
5. Runs `OPTIMIZE TABLE` on all six tables.

**Usage context:** Called periodically for maintenance.

```php
$search = new search();
$search->clean(); // Clean up orphaned data
```

### `address_standardize($address)`

Standardizes a URL to a canonical form for consistent storage and comparison.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | `string` | — | URL to standardize |

**Return value:** `string\|bool` — Standardized URL, or `FALSE` if invalid.

**Inner mechanisms:**
1. Parses the URL with `analyze_url()`.
2. Normalizes the scheme: `http` with port 80 and `https` with port 443 have their ports removed.
3. Converts the host to punycode.
4. Collapses multiple path separators and URL-encodes the path.
5. Sorts query parameters alphabetically (using a recursive function that handles nested arrays).
6. Reassembles the URL with `cms_build_url()`.

**Usage context:** Called by `scan()`, `entry_set()`, and `queue_add()` to normalize URLs before storage.

```php
$search = new search();
$normalized = $search->address_standardize("https://example.com:443/path/../page?b=2&a=1");
// Returns: "https://example.com/page?a=1&b=2"
```

### `address_accepted($address)`

Checks whether a URL is accepted based on blacklist and whitelist regex patterns.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$address` | `string` | — | URL to check |

**Return value:** `bool` — `TRUE` if the address is accepted, `FALSE` if rejected.

**Inner mechanisms:**
1. Iterates through `$address_accepted_blacklist` patterns.
2. Each pattern can be prefixed with `!` for inversion (match = accept instead of reject).
3. If a blacklist pattern matches (or doesn't match when inverted), checks the whitelist.
4. If a whitelist pattern matches (or doesn't match when inverted), the address is accepted.
5. If no whitelist pattern matches, the address is rejected.
6. If no blacklist pattern matches, the address is accepted.

**Usage context:** Called by `scan()`, `entry_set()`, and `queue_add()` to filter URLs.

```php
$search = new search();
$search->address_accepted_blacklist = ["/^\/private\//", "!/^\/public\//"];
$search->address_accepted_whitelist = ["/example\.com/"];
$accepted = $search->address_accepted("https://example.com/public/page"); // TRUE
```


<!-- HASH:1f4c1dd4292d6054c852a4d019defb11 -->
