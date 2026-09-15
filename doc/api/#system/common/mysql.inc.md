# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/mysql.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/mysql.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## mysql.inc

This file provides a compatibility layer that reimplements the legacy PHP `mysql_*` extension using the modern `mysqli` extension. It allows existing PWNC code that relies on the deprecated `mysql_*` API to function correctly under newer PHP versions (PHP 8.1+) where the original extension has been removed. The file defines global constants for result types, manages a single shared MySQL connection, and wraps every standard `mysql_*` function with its `mysqli` equivalent.

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `$cms_mysql_connection` | `NULL` | Holds the global MySQLi connection instance used by all wrapper functions when no explicit link identifier is provided. |

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `MYSQL_ASSOC` | `MYSQLI_ASSOC` | Fetch result as an associative array. |
| `MYSQL_NUM` | `MYSQLI_NUM` | Fetch result as a numeric array. |
| `MYSQL_BOTH` | `MYSQLI_BOTH` | Fetch result as both associative and numeric arrays. |

### mysql_get_link_identifier

Resolves which MySQLi connection to use for a given operation.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | An explicit MySQLi connection object, or `NULL` to use the global connection. |

#### Return Values

- **Type:** `mysqli\|NULL`
- **Description:** Returns the provided MySQLi instance, the global connection if available, or `NULL` if neither exists.

#### Inner Mechanisms

The function checks if the passed parameter is already a `mysqli` instance and returns it directly. If not, it falls back to the global `$cms_mysql_connection`. This ensures all wrapper functions can operate without requiring an explicit connection parameter.

#### Usage Example

```php
// Use the global connection implicitly
$link = mysql_get_link_identifier();
// Or pass an explicit connection
$link = mysql_get_link_identifier($my_mysqli_instance);
```

---

### mysql_affected_rows

Returns the number of affected rows in the previous MySQL operation.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `int`
- **Description:** Number of affected rows.

#### Usage Example

```php
mysql_query("UPDATE users SET status='active' WHERE last_login > NOW()");
echo mysql_affected_rows() . " rows updated.";
```

---

### mysql_client_encoding

Returns the name of the default character set used by the MySQL connection.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `string`
- **Description:** Character set name (e.g., `"utf8mb4"`).

#### Usage Example

```php
echo "Connection charset: " . mysql_client_encoding();
```

---

### mysql_close

Closes a previously opened database connection.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `bool`
- **Description:** `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms

After closing the connection, if it was the global connection, the global variable is reset to `NULL`.

#### Usage Example

```php
mysql_close(); // Close the global connection
```

---

### mysql_connect

Opens a new MySQL connection (or reuses an existing one).

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$server` | `string\|NULL` | `NULL` | Server hostname, optionally with port/socket. |
| `$username` | `string\|NULL` | `NULL` | MySQL username. |
| `$password` | `string\|NULL` | `NULL` | MySQL password. |
| `$new_link` | `bool\|NULL` | `NULL` | If `TRUE`, always create a new connection. |
| `$client_flag` | `int\|NULL` | `NULL` | Client flags (unused in this implementation). |
| `$persistent` | `bool` | `FALSE` | If `TRUE`, use persistent connection (`p:` prefix). |

#### Return Values

- **Type:** `mysqli\|FALSE`
- **Description:** A MySQLi connection object on success, `FALSE` on failure.

#### Inner Mechanisms

If `$new_link` is falsy and a valid global connection exists, it returns that connection instead of creating a new one. The server string is parsed for port/socket information. Persistent connections are prefixed with `p:`.

#### Usage Example

```php
// Connect to localhost with default settings
mysql_connect("localhost", "user", "pass");

// Connect with a specific port
mysql_connect("127.0.0.1:3306", "user", "pass");

// Persistent connection
mysql_pconnect("localhost", "user", "pass");
```

---

### mysql_data_seek

Moves the internal result pointer to a specified row.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$row_number` | `int` | Zero-based row number to seek to. |

#### Return Values

- **Type:** `bool`
- **Description:** `TRUE` on success, `FALSE` on failure.

#### Usage Example

```php
$result = mysql_query("SELECT * FROM users");
mysql_data_seek($result, 5); // Move to the 6th row
```

---

### mysql_db_name

Returns the database name from a result set (from `mysql_list_dbs`).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$row` | `int` | Row number to retrieve. |
| `$field` | `string\|NULL` | Optional field name; defaults to first column. |

#### Return Values

- **Type:** `string\|FALSE`
- **Description:** Database name or `FALSE` on failure.

#### Usage Example

```php
$dbs = mysql_list_dbs();
echo mysql_db_name($dbs, 0); // Print first database name
```

---

### mysql_errno

Returns the error number from the last MySQL operation.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `int`
- **Description:** MySQL error number, or 0 if no error.

#### Usage Example

```php
mysql_query("INVALID SQL");
echo "Error: " . mysql_errno();
```

---

### mysql_error

Returns the error message from the last MySQL operation.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `string`
- **Description:** MySQL error message, or empty string if no error.

#### Usage Example

```php
mysql_query("INVALID SQL");
echo "Error: " . mysql_error();
```

---

### mysql_fetch_array

Fetches a result row as an associative array, numeric array, or both.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$result` | `mysqli_result` | — | The result set identifier. |
| `$result_type` | `int` | `MYSQL_BOTH` | One of `MYSQL_ASSOC`, `MYSQL_NUM`, or `MYSQL_BOTH`. |

#### Return Values

- **Type:** `array\|FALSE`
- **Description:** An array containing the fetched row, or `FALSE` if no more rows.

#### Usage Example

```php
$result = mysql_query("SELECT id, name FROM users");
while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
    echo $row['id'] . ": " . $row['name'] . "\n";
}
```

---

### mysql_fetch_assoc

Fetches a result row as an associative array.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |

#### Return Values

- **Type:** `array\|FALSE`
- **Description:** Associative array of the fetched row, or `FALSE` if no more rows.

#### Usage Example

```php
$result = mysql_query("SELECT name, email FROM users");
while ($row = mysql_fetch_assoc($result)) {
    echo $row['name'] . " (" . $row['email'] . ")\n";
}
```

---

### mysql_fetch_field

Returns metadata for a single field in a result set.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$result` | `mysqli_result` | — | The result set identifier. |
| `$field_offset` | `int\|NULL` | `NULL` | Field offset; if `NULL`, fetches the next field. |

#### Return Values

- **Type:** `object\|FALSE`
- **Description:** A `mysqli_field` object with field metadata, or `FALSE` on failure.

#### Usage Example

```php
$result = mysql_query("SELECT id, name FROM users");
$field = mysql_fetch_field($result, 0);
echo "Field name: " . $field->name;
```

---

### mysql_fetch_lengths

Returns the length of each column in the current row.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |

#### Return Values

- **Type:** `array\|FALSE`
- **Description:** Array of column lengths, or `FALSE` on failure.

#### Usage Example

```php
$result = mysql_query("SELECT name FROM users");
mysql_fetch_assoc($result); // Fetch a row first
$lengths = mysql_fetch_lengths($result);
print_r($lengths);
```

---

### mysql_fetch_object

Fetches a result row as an object.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$result` | `mysqli_result` | — | The result set identifier. |
| `$class_name` | `string\|NULL` | `NULL` | Class name to instantiate; defaults to `stdClass`. |
| `$params` | `array\|NULL` | `NULL` | Constructor parameters for the class. |

#### Return Values

- **Type:** `object\|FALSE`
- **Description:** An object with properties matching the row columns, or `FALSE` if no more rows.

#### Usage Example

```php
$result = mysql_query("SELECT id, name FROM users");
while ($obj = mysql_fetch_object($result)) {
    echo $obj->id . ": " . $obj->name . "\n";
}
```

---

### mysql_fetch_row

Fetches a result row as a numeric array.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |

#### Return Values

- **Type:** `array\|FALSE`
- **Description:** Numeric array of the fetched row, or `FALSE` if no more rows.

#### Usage Example

```php
$result = mysql_query("SELECT id, name FROM users");
while ($row = mysql_fetch_row($result)) {
    echo $row[0] . ": " . $row[1] . "\n";
}
```

---

### mysql_field_flags

Returns a space-separated string of flags for a given field.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$field_offset` | `int` | Field offset. |

#### Return Values

- **Type:** `string\|FALSE`
- **Description:** Space-separated flag names (e.g., `"not_null primary_key"`), or `FALSE` on failure.

#### Inner Mechanisms

Maps MySQLi flag constants to human-readable strings using a predefined lookup table.

#### Usage Example

```php
$result = mysql_query("SELECT id FROM users");
echo mysql_field_flags($result, 0); // e.g., "not_null primary_key auto_increment"
```

---

### mysql_field_len

Returns the maximum length of a given field.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$field_offset` | `int` | Field offset. |

#### Return Values

- **Type:** `int\|FALSE`
- **Description:** Maximum field length, or `FALSE` on failure.

#### Usage Example

```php
$result = mysql_query("SELECT name FROM users");
echo mysql_field_len($result, 0); // Max length of 'name' column
```

---

### mysql_field_name

Returns the name of a given field.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$field_offset` | `int` | Field offset. |

#### Return Values

- **Type:** `string\|FALSE`
- **Description:** Field name, or `FALSE` on failure.

#### Usage Example

```php
$result = mysql_query("SELECT id, name FROM users");
echo mysql_field_name($result, 1); // Outputs: "name"
```

---

### mysql_field_seek

Seeks to a specified field offset within a result set.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$field_offset` | `int` | Field offset to seek to. |

#### Return Values

- **Type:** `bool`
- **Description:** `TRUE` on success, `FALSE` if offset is out of bounds.

#### Usage Example

```php
$result = mysql_query("SELECT id, name, email FROM users");
mysql_field_seek($result, 2); // Seek to the third field
```

---

### mysql_field_table

Returns the table name for a given field.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$field_offset` | `int` | Field offset. |

#### Return Values

- **Type:** `string\|FALSE`
- **Description:** Table name, or `FALSE` on failure.

#### Usage Example

```php
$result = mysql_query("SELECT u.name FROM users u");
echo mysql_field_table($result, 0); // Outputs: "u" or "users"
```

---

### mysql_field_type

Returns the type of a given field as a string.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |
| `$field_offset` | `int` | Field offset. |

#### Return Values

- **Type:** `string`
- **Description:** Field type string (e.g., `"int"`, `"string"`, `"blob"`), or `"unknown"`.

#### Inner Mechanisms

Maps MySQLi type constants to simplified type strings using a predefined lookup table.

#### Usage Example

```php
$result = mysql_query("SELECT id, name FROM users");
echo mysql_field_type($result, 0); // Outputs: "int"
echo mysql_field_type($result, 1); // Outputs: "string"
```

---

### mysql_free_result

Frees the memory associated with a result set.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |

#### Return Values

- **Type:** `bool`
- **Description:** Always returns `TRUE`.

#### Usage Example

```php
$result = mysql_query("SELECT * FROM large_table");
// Process result...
mysql_free_result($result);
```

---

### mysql_get_client_info

Returns the MySQL client library version.

#### Return Values

- **Type:** `string`
- **Description:** Client library version string.

#### Usage Example

```php
echo "MySQL client version: " . mysql_get_client_info();
```

---

### mysql_get_host_info

Returns the MySQL server host information.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `string`
- **Description:** Host information string.

#### Usage Example

```php
echo "Host info: " . mysql_get_host_info();
```

---

### mysql_get_proto_info

Returns the MySQL protocol version.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `int`
- **Description:** Protocol version number.

#### Usage Example

```php
echo "Protocol version: " . mysql_get_proto_info();
```

---

### mysql_get_server_info

Returns the MySQL server version.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `string`
- **Description:** Server version string.

#### Usage Example

```php
echo "Server version: " . mysql_get_server_info();
```

---

### mysql_info

Returns information about the last query.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `string\|NULL`
- **Description:** Query information string, or `NULL` if not applicable.

#### Usage Example

```php
mysql_query("INSERT INTO logs (msg) VALUES ('test')");
echo mysql_info(); // e.g., "Records: 1 Duplicates: 0..."
```

---

### mysql_insert_id

Returns the ID generated for an AUTO_INCREMENT column.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `int`
- **Description:** The AUTO_INCREMENT ID from the last query.

#### Usage Example

```php
mysql_query("INSERT INTO users (name) VALUES ('John')");
echo "New user ID: " . mysql_insert_id();
```

---

### mysql_list_dbs

Lists all databases on the MySQL server.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `mysqli_result`
- **Description:** Result set containing database names.

#### Usage Example

```php
$dbs = mysql_list_dbs();
while ($db = mysql_fetch_assoc($dbs)) {
    echo $db['Database'] . "\n";
}
```

---

### mysql_list_processes

Lists current MySQL server processes.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `mysqli_result`
- **Description:** Result set containing process information.

#### Usage Example

```php
$procs = mysql_list_processes();
while ($proc = mysql_fetch_assoc($procs)) {
    echo "Process ID: " . $proc['Id'] . "\n";
}
```

---

### mysql_num_fields

Returns the number of fields in a result set.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |

#### Return Values

- **Type:** `int`
- **Description:** Number of fields/columns.

#### Usage Example

```php
$result = mysql_query("SELECT id, name, email FROM users");
echo "Columns: " . mysql_num_fields($result); // Outputs: 3
```

---

### mysql_num_rows

Returns the number of rows in a result set.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$result` | `mysqli_result` | The result set identifier. |

#### Return Values

- **Type:** `int`
- **Description:** Number of rows.

#### Usage Example

```php
$result = mysql_query("SELECT * FROM users");
echo "Total users: " . mysql_num_rows($result);
```

---

### mysql_pconnect

Opens a persistent MySQL connection.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$server` | `string\|NULL` | `NULL` | Server hostname. |
| `$username` | `string\|NULL` | `NULL` | MySQL username. |
| `$password` | `string\|NULL` | `NULL` | MySQL password. |
| `$client_flag` | `int\|NULL` | `NULL` | Client flags (unused). |

#### Return Values

- **Type:** `mysqli\|FALSE`
- **Description:** A persistent MySQLi connection object, or `FALSE` on failure.

#### Inner Mechanisms

Calls `mysql_connect` with `$persistent` set to `TRUE`, which prefixes the server with `p:`.

#### Usage Example

```php
mysql_pconnect("localhost", "user", "pass");
```

---

### mysql_ping

Checks if the MySQL connection is alive.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `bool`
- **Description:** `TRUE` if connection is alive, `FALSE` otherwise.

#### Inner Mechanisms

Executes a simple `SELECT 1` query to verify connectivity.

#### Usage Example

```php
if (!mysql_ping()) {
    mysql_connect("localhost", "user", "pass");
}
```

---

### mysql_query

Executes a MySQL query.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$query` | `string` | SQL query string. |
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `mysqli_result\|bool`
- **Description:** Result set for SELECT queries, `TRUE` for successful non-result queries, `FALSE` on error.

#### Inner Mechanisms

Wraps `mysqli_query` and triggers a user warning with the error message and query if the query fails.

#### Usage Example

```php
$result = mysql_query("SELECT * FROM users WHERE active=1");
if ($result) {
    while ($row = mysql_fetch_assoc($result)) {
        echo $row['name'] . "\n";
    }
}
```

---

### mysql_real_escape_string

Escapes special characters in a string for use in SQL queries.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$unescaped_string` | `string` | The string to escape. |
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `string`
- **Description:** Escaped string safe for SQL queries.

#### Usage Example

```php
$name = mysql_real_escape_string($_POST['name']);
mysql_query("SELECT * FROM users WHERE name='$name'");
```

---

### mysql_result

Returns a specific field from a result row.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$result` | `mysqli_result` | — | The result set identifier. |
| `$row` | `int` | — | Row number to retrieve. |
| `$field` | `string\|int\|NULL` | `NULL` | Field name or offset; defaults to first column. |

#### Return Values

- **Type:** `string\|FALSE`
- **Description:** Field value, or `FALSE` on failure.

#### Usage Example

```php
$result = mysql_query("SELECT name FROM users");
echo mysql_result($result, 0, "name"); // First row's name
```

---

### mysql_select_db

Selects a MySQL database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$database_name` | `string` | Database name to select. |
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `bool`
- **Description:** `TRUE` on success, `FALSE` on failure.

#### Usage Example

```php
mysql_select_db("myapp_db");
```

---

### mysql_stat

Returns the current MySQL server status.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `string`
- **Description:** Server status string.

#### Usage Example

```php
echo mysql_stat(); // e.g., "Threads: 2 Questions: 15 Slow queries: 0..."
```

---

### mysql_thread_id

Returns the current MySQL thread ID.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `int`
- **Description:** Thread ID.

#### Usage Example

```php
echo "Thread ID: " . mysql_thread_id();
```

---

### mysql_unbuffered_query

Executes a MySQL query without buffering the result set.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$query` | `string` | SQL query string. |
| `$link_identifier` | `mysqli\|NULL` | Optional MySQLi connection; defaults to the global connection. |

#### Return Values

- **Type:** `mysqli_result\|bool`
- **Description:** Unbuffered result set, or `FALSE` on error.

#### Inner Mechanisms

Uses `MYSQLI_USE_RESULT` flag to avoid buffering, which can reduce memory usage for large result sets.

#### Usage Example

```php
$result = mysql_unbuffered_query("SELECT * FROM large_log_table");
while ($row = mysql_fetch_assoc($result)) {
    process_row($row);
}
```


<!-- HASH:d15fa1fd9e56845d5c4a31141c04bb97 -->
