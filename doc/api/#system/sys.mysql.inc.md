# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.mysql.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.mysql.inc)

- **Version:** `26.9.23.8`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

# mysql Class

## Overview
The `mysql` class serves as the core database abstraction layer for the PWNC Web Platform. It provides a high-performance, zero-dependency interface to MySQL/MariaDB, handling connection management, schema verification, and data manipulation. It utilizes the legacy `mysql_*` functions (internally mapped to `mysqli`) for compatibility and performance in specific contexts, wrapped with custom security and utility functions like `sqlesc`.

This class is responsible for:
*   **Connection Management:** Establishing and caching database connections with version validation.
*   **Schema Migration:** Automatically verifying and altering table structures to match defined schemas.
*   **Data I/O:** Exporting and importing data in various formats (SQL, CSV, Excel, HTML).
*   **Backup/Restore:** Full database backup and restoration mechanisms.

## Properties

| Name | Value/Default | Description |
| :--- | :--- | :--- |
| `$database` | NULL | The name of the database to connect to. |
| `$host` | NULL | The database host address. |
| `$user` | NULL | The database username. |
| `$password` | NULL | The database password. |
| `$software` | NULL | Detected software type ("MySQL" or "MariaDB"). |
| `$version` | NULL | Detected software version. |
| `$engine` | "InnoDB" | Default storage engine for tables. |
| `$charset` | "utf8mb4" | Default character set for tables. |
| `$collation` | "utf8mb4_unicode_ci" | Default collation for tables. |

## Methods

### __construct()
Initializes the database connection using configuration values from the `system` class.

**Inner Mechanisms:**
1.  Instantiates the `system` class to retrieve configuration parameters.
2.  Populates class properties with these parameters.
3.  Immediately calls the `connection()` method to establish the link.

**Usage Example:**
```php
// Instantiation is usually handled automatically by the framework.
// However, if instantiated manually:
$db = new \cms\mysql();
// Connection is established automatically via __construct()
```

### connection()
Establishes or retrieves the database connection, validates the server software and version, and sets session parameters.

**Inner Mechanisms:**
1.  Checks a static array `$init` to ensure the connection ID is not re-initialized.
2.  Attempts to connect using `mysql_connect`. If no connection exists, it fails if credentials are missing.
3.  Queries the server for `VERSION()` and parses the string to detect MySQL vs. MariaDB.
4.  Validates version requirements (MySQL >= 5.6, MariaDB >= 10).
5.  Sets session SQL mode to empty, character set, collation, and timezone.
6.  Selects the database.

**Return Values:**
*   `resource` (MySQL link identifier) on success.
*   `FALSE` on failure.

**Usage Example:**
```php
$db = new \cms\mysql();
$link = $db->connection();
if ($link) {
    echo "Connected to " . $db->software . " " . $db->version;
}
```

### get($index, $column, $table, $index_key = "id")
Retrieves a single value from a table based on an index.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$index` | mixed | Required | The value to search for in the index column. |
| `$column` | string | Required | The column name to retrieve. |
| `$table` | string | Required | The table name. |
| `$index_key` | string | "id" | The column name used as the index. |

**Return Values:**
*   `mixed` The value of the column, or `FALSE` on failure.

**Usage Example:**
```php
// Get the title of content with ID 5
$title = $db->get(5, 'title', 'content');
```

### set($index, $column, $table, $value, $index_key = "id")
Updates a single value in a table based on an index.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$index` | mixed | Required | The value to match in the index column. |
| `$column` | string | Required | The column name to update. |
| `$table` | string | Required | The table name. |
| `$value` | mixed | Required | The new value to set. |
| `$index_key` | string | "id" | The column name used as the index. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Update the status of user ID 42 to 'active'
$db->set(42, 'status', 'users', 'active');
```

### delete($index, $table, $index_key = "id", $parent_key = "container")
Deletes a record and recursively deletes all child records (if a parent-child relationship exists).

**Inner Mechanisms:**
1.  Checks if the record has children by querying the `parent_key`.
2.  If children exist, it recursively calls `delete()` for each child.
3.  Finally, it deletes the target record itself.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$index` | mixed | Required | The ID of the record to delete. |
| `$table` | string | Required | The table name. |
| `$index_key` | string | "id" | The column name used as the index. |
| `$parent_key` | string | "container" | The column name used to identify parent-child relationships. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Delete a category and all its sub-categories
$db->delete(10, 'categories');
```

### is_child($index, $parent, $table, $index_key = "id", $parent_key = "container")
Recursively checks if a specific index is a descendant of a parent index.

**Inner Mechanisms:**
1.  Queries the table to find the parent of the current `$index`.
2.  If the parent matches the target `$parent`, returns `TRUE`.
3.  If not, repeats the process with the found parent until a match is found or `FALSE` is returned.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$index` | mixed | Required | The child index to check. |
| `$parent` | mixed | Required | The parent index to compare against. |
| `$table` | string | Required | The table name. |
| `$index_key` | string | "id" | The column name used as the index. |
| `$parent_key` | string | "container" | The column name used to identify parent-child relationships. |

**Return Values:**
*   `bool` `TRUE` if `$index` is a child of `$parent`, `FALSE` otherwise.

**Usage Example:**
```php
// Check if category 5 is a child of category 2
if ($db->is_child(5, 2, 'categories')) {
    echo "Category 5 is inside Category 2";
}
```

### verify_table($table, $column, $index = NULL, $mapping = NULL)
Verifies the structure of a table against a desired schema and applies necessary `ALTER TABLE` statements.

**Inner Mechanisms:**
1.  **Caching:** Checks a schema hash. If the table exists and the hash matches the cache, it skips processing.
2.  **Creation:** If the table doesn't exist, it creates it using the provided `$column` definition.
3.  **Comparison:** If the table exists, it creates a temporary table with the desired schema.
4.  **Diffing:** It compares columns and indices between the source table and the temporary table using regex and index definitions.
5.  **Migration:** It generates SQL statements to rename columns (via mapping), add missing columns, modify existing ones, drop indices, and manage `AUTO_INCREMENT`.
6.  **Execution:** It executes the generated statements and updates the cache.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$table` | string | Required | The table name. |
| `$column` | array | Required | Associative array defining columns (`'col_name' => 'type'`). |
| `$index` | array | NULL | Array of index definitions. |
| `$mapping` | array | NULL | Array mapping old column names to new column names. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Define a new schema
$columns = [
    'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
    'title' => 'VARCHAR(255) NOT NULL',
    'content' => 'TEXT',
    'created' => 'DATETIME NOT NULL'
];
$indexes = [
    'PRIMARY' => ['id' => 'PRIMARY KEY']
];

// Verify and create/modify the table
$db->verify_table('articles', $columns, $indexes);
```

### export_sql($table, $file = NULL)
Exports the structure of a table to a SQL file.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$table` | string | Required | The table name. |
| `$file` | string | NULL | Path to the output file. Defaults to `CMS_DATA_PATH . "#database/$table"`. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Export the 'users' table structure to a file
$db->export_sql('users', '/path/to/users.sql');
```

### export_csv($table, $separator = ",", $delimiter = "\"", $file = NULL, $set_fields = FALSE)
Exports table data to a CSV file with custom formatting.

**Inner Mechanisms:**
1.  Retrieves column names.
2.  Constructs a complex `SELECT` query using `CONCAT_WS` to handle escaping and delimiters manually for performance.
3.  Writes the file using `fwrite` with file locking.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$table` | string | Required | The table name. |
| `$separator` | string | "," | Field separator. |
| `$delimiter` | string | "\"" | Field delimiter. |
| `$file` | string | NULL | Path to the output file. |
| `$set_fields` | bool | FALSE | If `TRUE`, writes the header row. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Export 'products' to CSV with tab separator
$db->export_csv('products', "\t", "\"", '/path/to/products.csv', TRUE);
```

### export_excel($table, $file = NULL, $set_fields = FALSE)
Exports table data to an Excel-compatible file (UTF-16LE CSV).

**Inner Mechanisms:**
1.  Calls `export_csv` with tab separators.
2.  Opens the generated CSV and the target file.
3.  Writes a UTF-16LE Byte Order Mark (`\xFF\xFE`).
4.  Uses a stream filter (`convert.iconv.UTF-8/UTF-16LE`) to convert the content in real-time.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$table` | string | Required | The table name. |
| `$file` | string | NULL | Path to the output file (e.g., `.xls`). |
| `$set_fields` | bool | FALSE | If `TRUE`, writes the header row. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Export 'sales' to an Excel file
$db->export_excel('sales', '/path/to/sales.xls', TRUE);
```

### export_html($table, $file = NULL, $set_fields = FALSE)
Exports table data to a formatted HTML file.

**Inner Mechanisms:**
1.  Retrieves column names.
2.  Executes a query to fetch data.
3.  Writes standard HTML5 boilerplate with a `<table>`.
4.  Optionally writes a `<thead>` row with field names.
5.  Writes `<tbody>` rows with data, applying XML escaping (`x`) and newline-to-br conversion (`nl2br`).

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$table` | string | Required | The table name. |
| `$file` | string | NULL | Path to the output file (`.htm`). |
| `$set_fields` | bool | FALSE | If `TRUE`, writes the header row. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Export 'logs' to HTML
$db->export_html('logs', '/path/to/logs.htm', TRUE);
```

### import_csv($file, $separator = ",", $delimiter = "\"", $table = NULL, $get_fields = FALSE, $ignore_first_row = FALSE, $ignore_existing = TRUE, $mapping = NULL)
Imports data from a CSV file into a database table.

**Inner Mechanisms:**
1.  **Parsing:** Uses a custom 2-byte lookahead buffer parser (faster than `fgetcsv`) to handle escaped delimiters and complex CSV structures.
2.  **Mapping:** If `$mapping` is provided, it maps CSV columns to DB columns.
3.  **Insertion:** Uses `INSERT IGNORE` (default) or `REPLACE` to handle duplicates.
4.  **Buffering:** Batches inserts every 100 rows to optimize performance.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$file` | string | Required | Path to the CSV file. |
| `$separator` | string | "," | Field separator. |
| `$delimiter` | string | "\"" | Field delimiter. |
| `$table` | string | NULL | Target table name. |
| `$get_fields` | bool | FALSE | If `TRUE`, returns the header row as an array. |
| `$ignore_first_row` | bool | FALSE | Skips the first row of the CSV. |
| `$ignore_existing` | bool | TRUE | Uses `INSERT IGNORE` (skips duplicates) or `REPLACE`. |
| `$mapping` | array | NULL | Array mapping CSV indices to DB column names. |

**Return Values:**
*   `array` If `$get_fields` is `TRUE`, returns the header row.
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Import data from a CSV file, mapping columns
$mapping = [1 => 'username', 2 => 'email']; // CSV col 1 -> DB col username
$db->import_csv('/path/to/users.csv', ',', '"', 'users', FALSE, FALSE, TRUE, $mapping);
```

### backup()
Backs up all non-backup tables to the `#database/backup/` directory.

**Inner Mechanisms:**
1.  Lists all tables.
2.  For each table (excluding those starting with `#backup_`), it calls `export_sql` and `export_csv`.
3.  Returns `TRUE` only if all exports succeed.

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Create a full backup
if ($db->backup()) {
    echo "Backup completed successfully.";
}
```

### restore()
Restores tables from the `#database/backup/` directory.

**Inner Mechanisms:**
1.  Scans the backup directory for `.sql` files.
2.  For each file, it renames the existing table to a backup (e.g., `#backup_table_timestamp`).
3.  Executes the SQL file to create the new table.
4.  Imports the corresponding CSV file.
5.  If restoration fails, it renames the backup table back to the original name.

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Restore from backup
if ($db->restore()) {
    echo "Database restored successfully.";
}
```

### drop_table($table)
Drops a specific table from the database.

**Parameters:**
| Parameter | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `$table` | string | Required | The table name to drop. |

**Return Values:**
*   `bool` `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
// Drop a temporary table
$db->drop_table('temp_analysis');
```


<!-- HASH:c555a36be33338e3867f9b822b026642 -->
