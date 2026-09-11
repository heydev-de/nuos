# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.mysql.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.mysql.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mysql

The `mysql` class is the core database abstraction layer for the PWNC Web Platform. It encapsulates MySQL/MariaDB connection management, schema verification and migration, data access primitives, and import/export utilities. It uses internal `mysql_*` wrappers (backed by `mysqli`) and integrates with the platform's escaping and caching utilities.

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$database` | `NULL` | Database name retrieved from system configuration. |
| `$host` | `NULL` | Database host address. |
| `$user` | `NULL` | Database username. |
| `$password` | `NULL` | Database password. |
| `$software` | `NULL` | Detected database software (`"MySQL"` or `"MariaDB"`). |
| `$version` | `NULL` | Detected database version string. |
| `$engine` | `"InnoDB"` | Default storage engine for table creation. |
| `$charset` | `"utf8mb4"` | Default character set for connections and tables. |
| `$collation` | `"utf8mb4_unicode_ci"` | Default collation for connections and tables. |

### __construct

Initializes the MySQL instance by loading connection parameters from the system configuration and establishing a database connection.

**Parameters:** None.

**Return Value:** None (constructor).

**Inner Mechanism:** Instantiates a `system` object to retrieve MySQL credentials via `getval("mysql", ...)`, then calls `connection()` to establish the link.

**Usage Example:**
```php
$db = new cms\mysql();
// Connection is established automatically during construction.
```

### connection

Establishes or retrieves a cached MySQL connection, validates the database version, and configures session settings.

**Parameters:** None.

**Return Value:**
- `mysqli` link identifier on success.
- `FALSE` if connection fails or required parameters are missing.

**Inner Mechanism:**
1. Checks for an existing global connection via `mysql_get_link_identifier()`.
2. If none exists, attempts `mysql_connect()` using stored credentials.
3. Parses the database version string to detect MySQL vs MariaDB and extract the version number.
4. Enforces minimum version requirements (MySQL ≥ 5.6, MariaDB ≥ 10).
5. Sets session SQL mode, character set, collation, and time zone.
6. Selects the target database.

**Usage Example:**
```php
$db = new cms\mysql();
$link = $db->connection();
if ($link === FALSE) {
    die("Database connection failed.");
}
```

### get

Retrieves a single scalar value from a specified table column based on an index match.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | mixed | Value to match in the index column. |
| `$column` | string | Column name to retrieve. |
| `$table` | string | Table name to query. |
| `$index_key` | string | Column name used for the WHERE clause (default: `"id"`). |

**Return Value:**
- Scalar value of the matched column on success.
- `FALSE` on failure or no match.

**Inner Mechanism:** Constructs and executes a `SELECT ... LIMIT 1` query with properly escaped identifiers and values using `sqlesc()`.

**Usage Example:**
```php
$db = new cms\mysql();
$title = $db->get(42, "title", "posts");
echo $title; // Outputs the title of post with id=42
```

### set

Updates a single column value in a table row identified by an index key.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | mixed | Value to match in the index column. |
| `$column` | string | Column name to update. |
| `$table` | string | Table name to update. |
| `$value` | mixed | New value to set. |
| `$index_key` | string | Column name used for the WHERE clause (default: `"id"`). |

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:** Builds and executes an `UPDATE ... LIMIT 1` statement with escaped identifiers and values.

**Usage Example:**
```php
$db = new cms\mysql();
$db->set(42, "status", "posts", "published");
// Sets status to 'published' for the post with id=42
```

### delete

Recursively deletes a row and all its child rows from a hierarchical table structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | mixed | Value to match in the index column. |
| `$table` | string | Table name to delete from. |
| `$index_key` | string | Column name used for the WHERE clause (default: `"id"`). |
| `$parent_key` | string | Column name referencing the parent (default: `"container"`). |

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:**
1. Queries for all child rows where `parent_key` matches the given index.
2. Recursively calls `delete()` on each child.
3. Deletes the target row itself.

**Usage Example:**
```php
$db = new cms\mysql();
$db->delete(10, "comments");
// Deletes comment with id=10 and all nested replies
```

### is_child

Determines whether a given row is a descendant of a specified parent row in a hierarchical table.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | mixed | Starting row index to check. |
| `$parent` | mixed | Target parent index to find. |
| `$table` | string | Table name to traverse. |
| `$index_key` | string | Column name for row identification (default: `"id"`). |
| `$parent_key` | string | Column name referencing the parent (default: `"container"`). |

**Return Value:**
- `TRUE` if `$index` is a descendant of `$parent`.
- `FALSE` otherwise or on failure.

**Inner Mechanism:** Iteratively walks up the parent chain starting from `$index`, comparing each ancestor against `$parent` until a match is found or the root is reached.

**Usage Example:**
```php
$db = new cms\mysql();
if ($db->is_child(15, 3, "categories")) {
    echo "Category 15 is a child of category 3.";
}
```

### verify_table

Ensures a table exists with the correct schema, creating or migrating it as needed.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$table` | string | Table name to verify. |
| `$column` | array | Associative array of column definitions (name => SQL type). |
| `$index` | array | Array of index definition strings (default: `[]`). |
| `$mapping` | array | Column rename mapping (default: `[]`). |

**Return Value:**
- `TRUE` if the table schema is valid or successfully updated.
- `FALSE` on failure.

**Inner Mechanism:**
1. Caches the list of all tables to avoid repeated queries.
2. Computes a schema hash from the provided definition and compares it with the cached hash.
3. If the table doesn't exist, creates it with the specified columns, indexes, engine, charset, and collation.
4. If the table exists but the schema has changed, performs a detailed structural comparison and generates `ALTER TABLE` statements to reconcile differences (add/remove columns, modify types, manage indexes, handle auto-increment).

**Usage Example:**
```php
$db = new cms\mysql();
$db->verify_table("users", [
    "id"    => "INT UNSIGNED NOT NULL AUTO_INCREMENT",
    "name"  => "VARCHAR(255) NOT NULL",
    "email" => "VARCHAR(255) NOT NULL"
], [
    "PRIMARY KEY (`id`)",
    "UNIQUE KEY `email` (`email`)"
]);
// Creates or migrates the 'users' table to match the schema
```

### export_sql

Exports a table's structure (CREATE statement) to a `.sql` file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$table` | string | Table name to export. |
| `$file` | string | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:** Executes `SHOW CREATE TABLE`, retrieves the CREATE statement, and writes it to a `.sql` file after ensuring the directory exists.

**Usage Example:**
```php
$db = new cms\mysql();
$db->export_sql("posts");
// Writes the CREATE TABLE statement for 'posts' to data/#database/posts.sql
```

### export_csv

Exports table data to a CSV file with configurable separators and delimiters.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$table` | string | Table name to export. |
| `$separator` | string | Field separator character (default: `","`). |
| `$delimiter` | string | Field delimiter character (default: `"\""`). |
| `$file` | string | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |
| `$set_fields` | bool | Whether to include a header row with field names (default: `FALSE`). |

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:**
1. Retrieves column names via `SHOW COLUMNS`.
2. Constructs a complex `SELECT` query that concatenates all columns into a single CSV-formatted string per row, handling escaping and NULL values.
3. Writes the result to a file, optionally prepending a header row.

**Usage Example:**
```php
$db = new cms\mysql();
$db->export_csv("posts", ",", "\"", null, TRUE);
// Exports 'posts' table to data/#database/posts.csv with headers
```

### export_excel

Exports table data to an Excel-compatible `.xls` file in UTF-16LE format.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$table` | string | Table name to export. |
| `$file` | string | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |
| `$set_fields` | bool | Whether to include a header row (default: `FALSE`). |

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:**
1. Requires the `mbstring` extension.
2. Delegates to `export_csv()` with tab separator to create a temporary CSV.
3. Converts the CSV to UTF-16LE with BOM using `iconv` stream filter.
4. Cleans up the temporary file.

**Usage Example:**
```php
$db = new cms\mysql();
$db->export_excel("posts");
// Exports 'posts' table to data/#database/posts.xls
```

### export_html

Exports table data to a standalone HTML file with a styled table.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$table` | string | Table name to export. |
| `$file` | string | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |
| `$set_fields` | bool | Whether to include a header row (default: `FALSE`). |

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:**
1. Retrieves column names via `SHOW COLUMNS`.
2. Queries all rows with columns cast to UTF8MB4 strings.
3. Generates a complete HTML document with `<table>`, `<thead>`, and `<tbody>` sections.
4. Applies XML escaping (`x()`) and line break conversion (`nl2br()`) to cell content.

**Usage Example:**
```php
$db = new cms\mysql();
$db->export_html("posts", null, TRUE);
// Exports 'posts' table to data/#database/posts.htm with headers
```

### import_csv

Imports data from a CSV file into a database table with flexible field mapping.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$file` | string | Path to the CSV file to import. |
| `$separator` | string | Field separator character (default: `","`). |
| `$delimiter` | string | Field delimiter character (default: `"\""`). |
| `$table` | string | Target table name. |
| `$get_fields` | bool | If `TRUE`, returns field names from the first row instead of importing (default: `FALSE`). |
| `$ignore_first_row` | bool | Whether to skip the first row (e.g., headers) (default: `FALSE`). |
| `$ignore_existing` | bool | Use `INSERT IGNORE` if `TRUE`, `REPLACE` if `FALSE` (default: `TRUE`). |
| `$mapping` | array | Column mapping array (column index => target column name) (default: `NULL`). |

**Return Value:**
- If `$get_fields` is `TRUE`: array of field names from the first row.
- Otherwise: `TRUE` on success, `FALSE` on failure.

**Inner Mechanism:**
1. Builds an `INSERT IGNORE` or `REPLACE` statement.
2. If `$mapping` is provided, maps CSV columns to database fields.
3. Parses the CSV file in 64KB chunks using a custom state-machine parser that handles delimiters, escaped delimiters, and multi-byte content.
4. Buffers up to 100 rows before executing batch inserts.
5. Uses `_binary` prefix for values to preserve binary data integrity.

**Usage Example:**
```php
$db = new cms\mysql();
$db->import_csv("/path/to/data.csv", ",", "\"", "users", FALSE, TRUE, TRUE, [
    0 => "name",
    1 => "email",
    2 => "role"
]);
// Imports CSV data into 'users' table, skipping the header row
```

### backup

Creates SQL and CSV backups of all non-backup tables in the database.

**Parameters:** None.

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:**
1. Retrieves all table names via `SHOW TABLES`.
2. For each table not prefixed with `#backup_`, calls `export_sql()` and `export_csv()` to create backup files in the `#database/backup/` directory.

**Usage Example:**
```php
$db = new cms\mysql();
$db->backup();
// Creates .sql and .csv backups of all application tables
```

### restore

Restores tables from SQL and CSV backup files.

**Parameters:** None.

**Return Value:**
- `TRUE` on success.
- `FALSE` on failure.

**Inner Mechanism:**
1. Scans the `#database/backup/` directory for `.sql` files.
2. For each file, reads the SQL and executes it to recreate the table.
3. If the table already exists, renames it to a `#backup_` prefixed name before restoring.
4. If restoration fails, renames the backup back.
5. Imports data from the corresponding `.csv` file using `import_csv()`.

**Usage Example:**
```php
$db = new cms\mysql();
$db->restore();
// Restores all tables from backup files in data/#database/backup/
```

### drop_table

Drops a table from the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$table` | string | Table name to drop. |

**Return Value:**
- Result of `mysql_query()` (`TRUE` on success, `FALSE` on failure).

**Inner Mechanism:** Executes a `DROP TABLE` query with the table name properly escaped.

**Usage Example:**
```php
$db = new cms\mysql();
$db->drop_table("temp_data");
// Drops the 'temp_data' table from the database
```


<!-- HASH:3ca2c27c1a7a981193674ddf4378e254 -->
