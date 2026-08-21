# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/mysql.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/mysql.inc)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MySQL Compatibility Layer (`mysql.inc`)

This file provides a **MySQL compatibility layer** for the PWNC Web Platform, implementing the legacy `mysql_*` functions using PHP's modern `mysqli` extension. It ensures backward compatibility for older codebases while maintaining security and performance.

The layer includes:
- A **global connection handler** (`$cms_mysql_connection`) to manage persistent connections.
- **Result type constants** (`MYSQL_ASSOC`, `MYSQL_NUM`, `MYSQL_BOTH`) for consistent behavior.
- **Wrapper functions** for all major `mysql_*` operations, internally delegating to `mysqli`.

---

### Global Variables

| Name                     | Type       | Description                                                                 |
|--------------------------|------------|-----------------------------------------------------------------------------|
| `$cms_mysql_connection`  | `mysqli`   | Global MySQL connection object. Automatically reused if valid.             |

---

### Constants

| Name          | Value         | Description                                                                 |
|---------------|---------------|-----------------------------------------------------------------------------|
| `MYSQL_ASSOC` | `MYSQLI_ASSOC`| Return associative arrays from queries.                                     |
| `MYSQL_NUM`   | `MYSQLI_NUM`  | Return numeric arrays from queries.                                         |
| `MYSQL_BOTH`  | `MYSQLI_BOTH` | Return both associative and numeric arrays from queries (default).         |

---

### Core Functions

---

#### `mysql_get_link_identifier`
**Purpose**:
Resolves the active MySQL connection object, prioritizing user-provided identifiers over the global connection.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. User-provided connection object. If `NULL`, uses global connection.|

**Return Values**:
- `mysqli`: Active connection object.
- `NULL`: No valid connection available.

**Inner Mechanisms**:
1. Checks if `$link_identifier` is a valid `mysqli` object.
2. Falls back to `$cms_mysql_connection` if no identifier is provided.
3. Returns `NULL` if neither is valid.

**Usage Context**:
Used internally by all other functions to ensure consistent connection handling.

---

#### `mysql_affected_rows`
**Purpose**:
Returns the number of rows affected by the last `INSERT`, `UPDATE`, or `DELETE` query.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `int`: Number of affected rows.
- `0`: No rows affected or invalid query.

**Inner Mechanisms**:
Delegates to `mysqli_affected_rows()` after resolving the connection.

**Example**:
```php
mysql_query("UPDATE users SET active = 1 WHERE id = 42");
$affected = mysql_affected_rows(); // Returns 1 if the row was updated.
```

---

#### `mysql_client_encoding`
**Purpose**:
Retrieves the character set of the current connection.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `string`: Character set name (e.g., `"utf8mb4"`).
- `FALSE`: Invalid connection.

**Example**:
```php
$charset = mysql_client_encoding(); // Returns "utf8mb4" for UTF-8 connections.
```

---

#### `mysql_close`
**Purpose**:
Closes the specified MySQL connection and clears the global connection if applicable.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `TRUE`: Connection closed successfully.
- `FALSE`: Failed to close connection.

**Inner Mechanisms**:
1. Resolves the connection object.
2. Calls `mysqli_close()`.
3. Unsets `$cms_mysql_connection` if the closed connection was global.

**Example**:
```php
mysql_close(); // Closes the global connection.
```

---

#### `mysql_connect`
**Purpose**:
Establishes a new MySQL connection or reuses an existing one.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$server`         | `string`   | MySQL server address (e.g., `"localhost:3306"` or `"localhost:/tmp/mysql.sock"`). |
| `$username`       | `string`   | MySQL username.                                                             |
| `$password`       | `string`   | MySQL password.                                                             |
| `$new_link`       | `bool`     | If `TRUE`, forces a new connection even if a global one exists.             |
| `$client_flag`    | `int`      | Unused (legacy parameter).                                                  |
| `$persistent`     | `bool`     | If `TRUE`, uses persistent connections (prefixes server with `"p:"`).       |

**Return Values**:
- `mysqli`: Connection object.
- `FALSE`: Connection failed.

**Inner Mechanisms**:
1. Reuses the global connection if `$new_link` is `FALSE` and the connection is alive.
2. Parses `$server` for port/socket information.
3. Prefixes `$server` with `"p:"` for persistent connections.
4. Stores the connection in `$cms_mysql_connection` if not forced as new.

**Example**:
```php
$conn = mysql_connect("localhost", "user", "password"); // Reuses global connection if available.
```

---

#### `mysql_data_seek`
**Purpose**:
Moves the internal result pointer to a specified row.

**Parameters**:

| Name       | Type       | Description                                                                 |
|------------|------------|-----------------------------------------------------------------------------|
| `$result`  | `mysqli_result` | Query result object.                                                       |
| `$row_number` | `int`    | Row index to seek to (0-based).                                            |

**Return Values**:
- `TRUE`: Success.
- `FALSE`: Invalid row or result.

**Example**:
```php
$result = mysql_query("SELECT * FROM users");
mysql_data_seek($result, 5); // Moves to the 6th row.
$row = mysql_fetch_assoc($result);
```

---

#### `mysql_db_name`
**Purpose**:
Retrieves the database name from a `SHOW DATABASES` result.

**Parameters**:

| Name       | Type       | Description                                                                 |
|------------|------------|-----------------------------------------------------------------------------|
| `$result`  | `mysqli_result` | Result from `mysql_list_dbs()`.                                            |
| `$row`     | `int`      | Row index (0-based).                                                       |
| `$field`   | `string`   | Optional. Field name (defaults to first column).                           |

**Return Values**:
- `string`: Database name.
- `FALSE`: Invalid row or field.

**Example**:
```php
$result = mysql_list_dbs();
$db = mysql_db_name($result, 0); // Returns the first database name.
```

---

#### `mysql_errno`
**Purpose**:
Returns the error code for the last MySQL operation.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `int`: Error code (e.g., `1045` for access denied).
- `0`: No error.

**Example**:
```php
mysql_query("INVALID QUERY");
$errno = mysql_errno(); // Returns 1064 (syntax error).
```

---

#### `mysql_error`
**Purpose**:
Returns the error message for the last MySQL operation.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `string`: Error message.
- `""`: No error.

**Example**:
```php
mysql_query("INVALID QUERY");
$error = mysql_error(); // Returns "You have an error in your SQL syntax...".
```

---

#### `mysql_fetch_array`
**Purpose**:
Fetches a row from a result set as an associative, numeric, or both types of array.

**Parameters**:

| Name          | Type            | Description                                                                 |
|---------------|-----------------|-----------------------------------------------------------------------------|
| `$result`     | `mysqli_result` | Query result object.                                                       |
| `$result_type`| `int`           | One of `MYSQL_ASSOC`, `MYSQL_NUM`, or `MYSQL_BOTH` (default).              |

**Return Values**:
- `array`: Row data.
- `FALSE`: No more rows or invalid result.

**Example**:
```php
$result = mysql_query("SELECT id, name FROM users");
$row = mysql_fetch_array($result, MYSQL_ASSOC); // Returns ["id" => 1, "name" => "Alice"].
```

---

#### `mysql_fetch_assoc`
**Purpose**:
Fetches a row as an associative array.

**Parameters**:

| Name      | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `$result` | `mysqli_result` | Query result object.                                                       |

**Return Values**:
- `array`: Associative row data.
- `FALSE`: No more rows.

**Example**:
```php
$row = mysql_fetch_assoc($result); // Returns ["id" => 1, "name" => "Alice"].
```

---

#### `mysql_fetch_field`
**Purpose**:
Retrieves metadata for a specific field in a result set.

**Parameters**:

| Name            | Type            | Description                                                                 |
|-----------------|-----------------|-----------------------------------------------------------------------------|
| `$result`       | `mysqli_result` | Query result object.                                                       |
| `$field_offset` | `int`           | Optional. Field index (0-based). Defaults to current field.                |

**Return Values**:
- `object`: Field metadata (e.g., `name`, `type`, `length`).
- `FALSE`: Invalid field.

**Example**:
```php
$field = mysql_fetch_field($result, 0); // Returns metadata for the first field.
echo $field->name; // Outputs "id".
```

---

#### `mysql_fetch_lengths`
**Purpose**:
Returns the lengths of each field in the current row.

**Parameters**:

| Name      | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `$result` | `mysqli_result` | Query result object.                                                       |

**Return Values**:
- `array`: Field lengths (e.g., `[3, 5]` for `"abc", "hello"`).
- `FALSE`: No row fetched.

**Example**:
```php
$lengths = mysql_fetch_lengths($result); // Returns [1, 5] for ["1", "Alice"].
```

---

#### `mysql_fetch_object`
**Purpose**:
Fetches a row as an object, optionally of a custom class.

**Parameters**:

| Name          | Type            | Description                                                                 |
|---------------|-----------------|-----------------------------------------------------------------------------|
| `$result`     | `mysqli_result` | Query result object.                                                       |
| `$class_name` | `string`        | Optional. Class name (defaults to `stdClass`).                             |
| `$params`     | `array`         | Optional. Constructor parameters for the class.                            |

**Return Values**:
- `object`: Row data as an object.
- `FALSE`: No more rows.

**Example**:
```php
class User {}
$row = mysql_fetch_object($result, "User"); // Returns a User object.
```

---

#### `mysql_fetch_row`
**Purpose**:
Fetches a row as a numeric array.

**Parameters**:

| Name      | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `$result` | `mysqli_result` | Query result object.                                                       |

**Return Values**:
- `array`: Numeric row data (e.g., `[1, "Alice"]`).
- `FALSE`: No more rows.

**Example**:
```php
$row = mysql_fetch_row($result); // Returns [1, "Alice"].
```

---

#### `mysql_field_flags`
**Purpose**:
Returns the flags associated with a field (e.g., `"not_null primary_key"`).

**Parameters**:

| Name            | Type            | Description                                                                 |
|-----------------|-----------------|-----------------------------------------------------------------------------|
| `$result`       | `mysqli_result` | Query result object.                                                       |
| `$field_offset` | `int`           | Field index (0-based).                                                     |

**Return Values**:
- `string`: Space-separated flags.
- `FALSE`: Invalid field.

**Example**:
```php
$flags = mysql_field_flags($result, 0); // Returns "not_null primary_key" for an ID field.
```

---

#### `mysql_field_len`
**Purpose**:
Returns the length of a field.

**Parameters**:

| Name            | Type            | Description                                                                 |
|-----------------|-----------------|-----------------------------------------------------------------------------|
| `$result`       | `mysqli_result` | Query result object.                                                       |
| `$field_offset` | `int`           | Field index (0-based).                                                     |

**Return Values**:
- `int`: Field length.
- `FALSE`: Invalid field.

**Example**:
```php
$length = mysql_field_len($result, 1); // Returns 255 for a VARCHAR(255) field.
```

---

#### `mysql_field_name`
**Purpose**:
Returns the name of a field.

**Parameters**:

| Name            | Type            | Description                                                                 |
|-----------------|-----------------|-----------------------------------------------------------------------------|
| `$result`       | `mysqli_result` | Query result object.                                                       |
| `$field_offset` | `int`           | Field index (0-based).                                                     |

**Return Values**:
- `string`: Field name.
- `FALSE`: Invalid field.

**Example**:
```php
$name = mysql_field_name($result, 0); // Returns "id".
```

---

#### `mysql_field_seek`
**Purpose**:
Moves the internal field pointer to a specified field.

**Parameters**:

| Name            | Type            | Description                                                                 |
|-----------------|-----------------|-----------------------------------------------------------------------------|
| `$result`       | `mysqli_result` | Query result object.                                                       |
| `$field_offset` | `int`           | Field index (0-based).                                                     |

**Return Values**:
- `TRUE`: Success.
- `FALSE`: Invalid field.

**Example**:
```php
mysql_field_seek($result, 1); // Moves to the second field.
```

---

#### `mysql_field_table`
**Purpose**:
Returns the table name of a field.

**Parameters**:

| Name            | Type            | Description                                                                 |
|-----------------|-----------------|-----------------------------------------------------------------------------|
| `$result`       | `mysqli_result` | Query result object.                                                       |
| `$field_offset` | `int`           | Field index (0-based).                                                     |

**Return Values**:
- `string`: Table name.
- `FALSE`: Invalid field.

**Example**:
```php
$table = mysql_field_table($result, 0); // Returns "users".
```

---

#### `mysql_field_type`
**Purpose**:
Returns the type of a field (e.g., `"int"`, `"string"`).

**Parameters**:

| Name            | Type            | Description                                                                 |
|-----------------|-----------------|-----------------------------------------------------------------------------|
| `$result`       | `mysqli_result` | Query result object.                                                       |
| `$field_offset` | `int`           | Field index (0-based).                                                     |

**Return Values**:
- `string`: Field type.
- `FALSE`: Invalid field.

**Example**:
```php
$type = mysql_field_type($result, 0); // Returns "int" for an ID field.
```

---

#### `mysql_free_result`
**Purpose**:
Frees the memory associated with a result set.

**Parameters**:

| Name      | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `$result` | `mysqli_result` | Query result object.                                                       |

**Return Values**:
- `TRUE`: Always.

**Example**:
```php
mysql_free_result($result); // Frees memory.
```

---

#### `mysql_get_client_info`
**Purpose**:
Returns the MySQL client library version.

**Return Values**:
- `string`: Version (e.g., `"8.0.23"`).

**Example**:
```php
$version = mysql_get_client_info(); // Returns "8.0.23".
```

---

#### `mysql_get_host_info`
**Purpose**:
Returns the host information of the connection.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `string`: Host info (e.g., `"localhost via TCP/IP"`).

**Example**:
```php
$host = mysql_get_host_info(); // Returns "localhost via TCP/IP".
```

---

#### `mysql_get_proto_info`
**Purpose**:
Returns the protocol version of the connection.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `int`: Protocol version (e.g., `10`).

**Example**:
```php
$proto = mysql_get_proto_info(); // Returns 10.
```

---

#### `mysql_get_server_info`
**Purpose**:
Returns the MySQL server version.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `string`: Server version (e.g., `"8.0.23"`).

**Example**:
```php
$version = mysql_get_server_info(); // Returns "8.0.23".
```

---

#### `mysql_info`
**Purpose**:
Returns detailed information about the last query (e.g., `"Records: 3 Duplicates: 0 Warnings: 0"`).

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `string`: Query info.
- `NULL`: No info available.

**Example**:
```php
mysql_query("INSERT INTO users VALUES (1, 'Alice'), (2, 'Bob')");
$info = mysql_info(); // Returns "Records: 2 Duplicates: 0 Warnings: 0".
```

---

#### `mysql_insert_id`
**Purpose**:
Returns the auto-increment ID generated by the last `INSERT` query.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `int`: Insert ID.
- `0`: No ID generated.

**Example**:
```php
mysql_query("INSERT INTO users (name) VALUES ('Alice')");
$id = mysql_insert_id(); // Returns the auto-increment ID.
```

---

#### `mysql_list_dbs`
**Purpose**:
Lists all databases on the MySQL server.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `mysqli_result`: Result set for `SHOW DATABASES`.
- `FALSE`: Query failed.

**Example**:
```php
$result = mysql_list_dbs();
while ($row = mysql_fetch_row($result)) {
    echo $row[0] . "\n"; // Outputs database names.
}
```

---

#### `mysql_list_processes`
**Purpose**:
Lists all active MySQL processes.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `mysqli_result`: Result set for `SHOW PROCESSLIST`.
- `FALSE`: Query failed.

**Example**:
```php
$result = mysql_list_processes();
while ($row = mysql_fetch_assoc($result)) {
    print_r($row); // Outputs process details.
}
```

---

#### `mysql_num_fields`
**Purpose**:
Returns the number of fields in a result set.

**Parameters**:

| Name      | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `$result` | `mysqli_result` | Query result object.                                                       |

**Return Values**:
- `int`: Number of fields.

**Example**:
```php
$count = mysql_num_fields($result); // Returns 3 for a 3-column query.
```

---

#### `mysql_num_rows`
**Purpose**:
Returns the number of rows in a result set.

**Parameters**:

| Name      | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `$result` | `mysqli_result` | Query result object.                                                       |

**Return Values**:
- `int`: Number of rows.

**Example**:
```php
$count = mysql_num_rows($result); // Returns 10 for a 10-row query.
```

---

#### `mysql_pconnect`
**Purpose**:
Establishes a persistent MySQL connection.

**Parameters**:

| Name           | Type     | Description                                                                 |
|----------------|----------|-----------------------------------------------------------------------------|
| `$server`      | `string` | MySQL server address.                                                      |
| `$username`    | `string` | MySQL username.                                                            |
| `$password`    | `string` | MySQL password.                                                            |
| `$client_flag` | `int`    | Unused (legacy parameter).                                                 |

**Return Values**:
- `mysqli`: Persistent connection object.

**Inner Mechanisms**:
Delegates to `mysql_connect()` with `$persistent = TRUE`.

**Example**:
```php
$conn = mysql_pconnect("localhost", "user", "password"); // Persistent connection.
```

---

#### `mysql_ping`
**Purpose**:
Checks if the connection to the MySQL server is alive.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `TRUE`: Connection alive.
- `FALSE`: Connection dead.

**Example**:
```php
if (!mysql_ping()) {
    die("Connection lost!");
}
```

---

#### `mysql_query`
**Purpose**:
Executes a SQL query and returns the result.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$query`          | `string`   | SQL query.                                                                  |
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `mysqli_result`: Result set for `SELECT`, `SHOW`, etc.
- `TRUE`: Success for `INSERT`, `UPDATE`, etc.
- `FALSE`: Query failed.

**Inner Mechanisms**:
1. Resolves the connection.
2. Executes the query via `mysqli_query()`.
3. Triggers a warning on failure.

**Example**:
```php
$result = mysql_query("SELECT * FROM users WHERE id = 1");
```

---

#### `mysql_real_escape_string`
**Purpose**:
Escapes special characters in a string for use in SQL queries.

**Parameters**:

| Name                | Type       | Description                                                                 |
|---------------------|------------|-----------------------------------------------------------------------------|
| `$unescaped_string` | `string`   | String to escape.                                                          |
| `$link_identifier`  | `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `string`: Escaped string.

**Example**:
```php
$escaped = mysql_real_escape_string("O'Reilly"); // Returns "O\'Reilly".
mysql_query("INSERT INTO users (name) VALUES ('$escaped')");
```

---

#### `mysql_result`
**Purpose**:
Retrieves a single field from a result set.

**Parameters**:

| Name      | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| `$result` | `mysqli_result` | Query result object.                                                       |
| `$row`    | `int`           | Row index (0-based).                                                       |
| `$field`  | `string`        | Optional. Field name or index. Defaults to first field.                    |

**Return Values**:
- `mixed`: Field value.
- `FALSE`: Invalid row or field.

**Example**:
```php
$name = mysql_result($result, 0, "name"); // Returns "Alice".
```

---

#### `mysql_select_db`
**Purpose**:
Selects a database for the connection.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$database_name`  | `string`   | Database name.                                                              |
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `TRUE`: Success.
- `FALSE`: Failure.

**Example**:
```php
mysql_select_db("my_database"); // Switches to "my_database".
```

---

#### `mysql_stat`
**Purpose**:
Returns the current server status (e.g., `"Uptime: 1000 Threads: 2 Questions: 50..."`).

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `string`: Server status.

**Example**:
```php
$status = mysql_stat(); // Returns server status.
```

---

#### `mysql_thread_id`
**Purpose**:
Returns the thread ID of the current connection.

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `int`: Thread ID.

**Example**:
```php
$thread_id = mysql_thread_id(); // Returns 12345.
```

---

#### `mysql_unbuffered_query`
**Purpose**:
Executes a query without buffering the result set (memory-efficient for large datasets).

**Parameters**:

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$query`          | `string`   | SQL query.                                                                  |
| `$link_identifier`| `mysqli`   | Optional. Connection object. Defaults to global connection.                |

**Return Values**:
- `mysqli_result`: Unbuffered result set.
- `FALSE`: Query failed.

**Example**:
```php
$result = mysql_unbuffered_query("SELECT * FROM large_table");
while ($row = mysql_fetch_assoc($result)) {
    // Process rows one by one.
}
```


<!-- HASH:d15fa1fd9e56845d5c4a31141c04bb97 -->
