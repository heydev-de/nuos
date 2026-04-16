# NUOS API Documentation

[← Index](../../README.md) | [`#system/common/mysql.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## MySQL Compatibility Layer for NUOS

This file provides a **backward-compatible MySQL API layer** for the NUOS platform, abstracting differences between the deprecated `mysql_*` functions and the modern `mysqli_*` extension. It ensures consistent database interaction regardless of the PHP version or installed extensions, while maintaining the original `mysql_*` function signatures.

The layer automatically detects the presence of the `mysqli` extension and routes all calls accordingly. It also manages a global connection handle (`$cms_mysql_connection`) and provides utility functions for link identifier resolution.

---

### Constants

| Name               | Value | Description                                                                 |
|--------------------|-------|-----------------------------------------------------------------------------|
| `CMS_MYSQL_IMPROVED` | `extension_loaded("mysqli")` | Boolean flag indicating whether the `mysqli` extension is available.       |
| `MYSQL_ASSOC`      | `1`   | Result type constant for associative arrays (column names as keys).         |
| `MYSQL_NUM`        | `2`   | Result type constant for numeric arrays (column indices as keys).           |
| `MYSQL_BOTH`       | `3`   | Result type constant for both associative and numeric arrays.               |

---

### Global Variables

| Name                     | Default | Description                                                                 |
|--------------------------|---------|-----------------------------------------------------------------------------|
| `$cms_mysql_connection`  | `NULL`  | Global database connection handle. Automatically set by `mysql_connect()` or `mysql_pconnect()`. |

---

### Functions

---

#### `mysql_get_link_identifier`

**Purpose:**
Resolves and validates a MySQL link identifier, ensuring compatibility between `mysql_*` and `mysqli_*` extensions. Returns the active connection handle if no valid identifier is provided.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. A MySQL link identifier (resource for `mysql_*`, `mysqli` object for `mysqli_*`). If `NULL`, the global `$cms_mysql_connection` is used. |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `resource\|object\|NULL` | Validated link identifier (resource or `mysqli` object), or `NULL` if invalid. |

**Inner Mechanisms:**
1. Checks if `mysqli` is available (`CMS_MYSQL_IMPROVED`).
2. For `mysqli`:
   - Disables automatic error reporting (to maintain backward compatibility).
   - Validates the provided identifier or falls back to the global connection.
3. For `mysql_*`:
   - Validates the provided identifier or falls back to the global connection.

**Usage Context:**
- Used internally by all other functions to resolve the link identifier.
- Rarely called directly; prefer passing `NULL` to use the global connection.

---

#### `mysql_affected_rows`

**Purpose:**
Returns the number of rows affected by the last `INSERT`, `UPDATE`, or `DELETE` query.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int`   | Number of affected rows.                                                    |
| `FALSE` | On failure (invalid link identifier).                                       |

**Inner Mechanisms:**
- Resolves the link identifier using `mysql_get_link_identifier()`.
- Delegates to `mysqli_affected_rows()` or `mysql_affected_rows()` based on `CMS_MYSQL_IMPROVED`.

**Usage Context:**
- After executing a write query to verify changes.
- Example:
  ```php
  mysql_query("UPDATE users SET name = 'John' WHERE id = 1");
  $affected = mysql_affected_rows();
  ```

---

#### `mysql_client_encoding`

**Purpose:**
Returns the character set used by the current connection.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Character set name (e.g., `"utf8"`).                                        |
| `FALSE`  | On failure (invalid link identifier).                                       |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_character_set_name()` or `mysql_client_encoding()`.

**Usage Context:**
- Debugging or ensuring character set consistency.
- Example:
  ```php
  $charset = mysql_client_encoding();
  ```

---

#### `mysql_close`

**Purpose:**
Closes the specified MySQL connection.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `bool`  | `TRUE` on success, `FALSE` on failure.                                      |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_close()` or `mysql_close()`.

**Usage Context:**
- Cleanup at the end of a script or when a connection is no longer needed.
- Example:
  ```php
  mysql_close();
  ```

---

#### `mysql_connect`

**Purpose:**
Establishes a new MySQL connection.

**Parameters:**

| Name              | Type     | Description                                                                 |
|-------------------|----------|-----------------------------------------------------------------------------|
| `$server`         | `string` | MySQL server address (e.g., `"localhost:3306"` or `"localhost:/tmp/mysql.sock"`). |
| `$username`       | `string` | MySQL username.                                                             |
| `$password`       | `string` | MySQL password.                                                             |
| `$new_link`       | `bool`   | **Ignored in `mysqli` mode.** If `TRUE`, forces a new connection even if one exists. |
| `$client_flag`    | `int`    | **Ignored in `mysqli` mode.** Client flags (e.g., `MYSQL_CLIENT_COMPRESS`). |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `resource\|object` | MySQL link identifier (resource or `mysqli` object).                        |
| `FALSE`            | On failure.                                                                 |

**Inner Mechanisms:**
- For `mysqli`:
  - Parses `$server` to extract port or socket (e.g., `"localhost:3306"` → port `3306`).
  - Calls `mysqli_connect()` with the resolved parameters.
- For `mysql_*`:
  - Calls `mysql_connect()` with the provided parameters.
- Stores the connection in `$cms_mysql_connection`.

**Usage Context:**
- Initializing a database connection at the start of a script.
- Example:
  ```php
  $conn = mysql_connect("localhost", "user", "password");
  ```

---

#### `mysql_data_seek`

**Purpose:**
Moves the internal result pointer to the specified row.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set (from `mysql_query()`).                                    |
| `$row_number` | `int`          | Row index to seek to (0-based).                                             |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `bool`  | `TRUE` on success, `FALSE` on failure.                                      |

**Inner Mechanisms:**
- Delegates to `mysqli_data_seek()` or `mysql_data_seek()`.

**Usage Context:**
- Navigating large result sets without fetching all rows.
- Example:
  ```php
  mysql_data_seek($result, 10); // Move to the 11th row
  ```

---

#### `mysql_db_name`

**Purpose:**
Retrieves the database name from a `mysql_list_dbs()` result set.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | Result set from `mysql_list_dbs()`.                                         |
| `$row`     | `int`              | Row index (0-based).                                                        |
| `$field`   | `string\|NULL`     | **Ignored in `mysqli` mode.** Column name (defaults to the first column).   |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Database name.                                                              |
| `FALSE`  | On failure (invalid row or result).                                         |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified row and fetches the first column.
- For `mysql_*`:
  - Delegates to `mysql_db_name()`.

**Usage Context:**
- Enumerating available databases.
- Example:
  ```php
  $dbs = mysql_list_dbs();
  $db = mysql_db_name($dbs, 0);
  ```

---

#### `mysql_errno`

**Purpose:**
Returns the error code for the last MySQL operation.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int`   | Error code (e.g., `1045` for access denied).                                |
| `0`     | No error.                                                                   |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_errno()` or `mysql_errno()`.

**Usage Context:**
- Error handling and debugging.
- Example:
  ```php
  if (mysql_errno()) {
      echo "MySQL Error: " . mysql_error();
  }
  ```

---

#### `mysql_error`

**Purpose:**
Returns the error message for the last MySQL operation.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Error message (e.g., `"Access denied for user 'user'@'localhost'"`).        |
| `""`     | No error.                                                                   |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_error()` or `mysql_error()`.

**Usage Context:**
- Error handling and logging.
- Example:
  ```php
  if (!mysql_query("SELECT * FROM users")) {
      die("Query failed: " . mysql_error());
  }
  ```

---

#### `mysql_fetch_array`

**Purpose:**
Fetches a row from a result set as an associative array, numeric array, or both.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |
| `$result_type` | `int`          | Result type (`MYSQL_ASSOC`, `MYSQL_NUM`, or `MYSQL_BOTH`). Defaults to `MYSQL_BOTH`. |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `array\|FALSE`     | Row data as an array, or `FALSE` if no more rows are available.             |

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_array()` or `mysql_fetch_array()`.
- Returns `FALSE` if no more rows are available (unlike the native `mysql_fetch_array()`, which returns `NULL`).

**Usage Context:**
- Iterating over query results.
- Example:
  ```php
  while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
      echo $row["username"];
  }
  ```

---

#### `mysql_fetch_assoc`

**Purpose:**
Fetches a row from a result set as an associative array.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `array\|FALSE`     | Row data as an associative array, or `FALSE` if no more rows are available. |

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_assoc()` or `mysql_fetch_assoc()`.
- Returns `FALSE` if no more rows are available.

**Usage Context:**
- Iterating over query results when only associative access is needed.
- Example:
  ```php
  while ($row = mysql_fetch_assoc($result)) {
      echo $row["username"];
  }
  ```

---

#### `mysql_fetch_field`

**Purpose:**
Returns metadata for a specific field in a result set.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$result`         | `resource\|object` | MySQL result set.                                                           |
| `$field_offset`   | `int\|NULL`        | Field index (0-based). If `NULL`, the next field is returned.               |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `object\|FALSE`    | Field metadata (see below), or `FALSE` on failure.                          |

**Field Metadata (Object Properties):**

| Property  | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `name`    | `string` | Column name.                                                                |
| `table`   | `string` | Table name.                                                                 |
| `max_length` | `int` | Maximum length of the field.                                                |
| `length`  | `int`    | Field length (as defined in the table).                                     |
| `flags`   | `int`    | Bitmask of field flags (e.g., `PRI_KEY_FLAG`, `NOT_NULL_FLAG`).             |
| `type`    | `int`    | Field type (e.g., `MYSQLI_TYPE_STRING`, `MYSQLI_TYPE_INT`).                 |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified field offset if provided.
  - Returns the result of `mysqli_fetch_field()`.
- For `mysql_*`:
  - Delegates to `mysql_fetch_field()`.

**Usage Context:**
- Inspecting result set structure (e.g., for dynamic table generation).
- Example:
  ```php
  $field = mysql_fetch_field($result, 0);
  echo "Field name: " . $field->name;
  ```

---

#### `mysql_fetch_lengths`

**Purpose:**
Returns the lengths of each field in the current row of a result set.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `array\|FALSE`     | Array of field lengths (0-based), or `FALSE` on failure.                    |

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_lengths()` or `mysql_fetch_lengths()`.

**Usage Context:**
- Determining the actual length of data in each field (e.g., for validation).
- Example:
  ```php
  $lengths = mysql_fetch_lengths($result);
  echo "Username length: " . $lengths[0];
  ```

---

#### `mysql_fetch_object`

**Purpose:**
Fetches a row from a result set as an object.

**Parameters:**

| Name          | Type               | Description                                                                 |
|---------------|--------------------|-----------------------------------------------------------------------------|
| `$result`     | `resource\|object` | MySQL result set.                                                           |
| `$class_name` | `string\|NULL`     | Class name to instantiate (defaults to `stdClass`).                         |
| `$params`     | `array\|NULL`      | Constructor parameters for the class.                                       |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `object\|FALSE`    | Row data as an object, or `FALSE` if no more rows are available.            |

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_object()` or `mysql_fetch_object()`.
- Defaults to `stdClass` if `$class_name` is `NULL`.

**Usage Context:**
- Iterating over query results when object-oriented access is preferred.
- Example:
  ```php
  while ($row = mysql_fetch_object($result, "User")) {
      echo $row->username;
  }
  ```

---

#### `mysql_fetch_row`

**Purpose:**
Fetches a row from a result set as a numeric array.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `array\|FALSE`     | Row data as a numeric array, or `FALSE` if no more rows are available.      |

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_row()` or `mysql_fetch_row()`.

**Usage Context:**
- Iterating over query results when only numeric access is needed.
- Example:
  ```php
  while ($row = mysql_fetch_row($result)) {
      echo $row[0]; // First column
  }
  ```

---

#### `mysql_field_flags`

**Purpose:**
Returns the flags associated with a field in a result set (e.g., `NOT NULL`, `PRIMARY KEY`).

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$result`         | `resource\|object` | MySQL result set.                                                           |
| `$field_offset`   | `int`              | Field index (0-based).                                                      |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string\|FALSE` | Space-separated list of flags (e.g., `"not_null primary_key"`), or `FALSE` on failure. |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified field offset.
  - Returns the `flags` property of the field object (as an integer bitmask in the original code, but typically converted to a string in native `mysql_field_flags()`).
- For `mysql_*`:
  - Delegates to `mysql_field_flags()`.

**Usage Context:**
- Inspecting field constraints (e.g., for form validation).
- Example:
  ```php
  $flags = mysql_field_flags($result, 0);
  if (strpos($flags, "primary_key") !== FALSE) {
      echo "This is a primary key field.";
  }
  ```

---

#### `mysql_field_len`

**Purpose:**
Returns the length of a field in a result set.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$result`         | `resource\|object` | MySQL result set.                                                           |
| `$field_offset`   | `int`              | Field index (0-based).                                                      |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int\|FALSE` | Field length (as defined in the table), or `FALSE` on failure.              |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified field offset.
  - Returns the `length` property of the field object.
- For `mysql_*`:
  - Delegates to `mysql_field_len()`.

**Usage Context:**
- Validating input lengths against database constraints.
- Example:
  ```php
  $length = mysql_field_len($result, 0);
  if (strlen($input) > $length) {
      echo "Input exceeds maximum length.";
  }
  ```

---

#### `mysql_field_name`

**Purpose:**
Returns the name of a field in a result set.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$result`         | `resource\|object` | MySQL result set.                                                           |
| `$field_offset`   | `int`              | Field index (0-based).                                                      |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string\|FALSE` | Field name, or `FALSE` on failure.                                          |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified field offset.
  - Returns the `name` property of the field object.
- For `mysql_*`:
  - Delegates to `mysql_field_name()`.

**Usage Context:**
- Dynamically generating forms or tables from query results.
- Example:
  ```php
  $name = mysql_field_name($result, 0);
  echo "First field: " . $name;
  ```

---

#### `mysql_field_seek`

**Purpose:**
Moves the internal field pointer to the specified field offset.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$result`         | `resource\|object` | MySQL result set.                                                           |
| `$field_offset`   | `int`              | Field index (0-based).                                                      |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `bool`  | `TRUE` on success, `FALSE` on failure.                                      |

**Inner Mechanisms:**
- Delegates to `mysqli_field_seek()` or `mysql_field_seek()`.

**Usage Context:**
- Navigating field metadata without fetching all fields.
- Example:
  ```php
  mysql_field_seek($result, 2); // Move to the 3rd field
  $field = mysql_fetch_field($result);
  ```

---

#### `mysql_field_table`

**Purpose:**
Returns the name of the table a field belongs to.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$result`         | `resource\|object` | MySQL result set.                                                           |
| `$field_offset`   | `int`              | Field index (0-based).                                                      |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string\|FALSE` | Table name, or `FALSE` on failure.                                          |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified field offset.
  - Returns the `table` property of the field object.
- For `mysql_*`:
  - Delegates to `mysql_field_table()`.

**Usage Context:**
- Joining tables dynamically or debugging complex queries.
- Example:
  ```php
  $table = mysql_field_table($result, 0);
  echo "Field belongs to table: " . $table;
  ```

---

#### `mysql_field_type`

**Purpose:**
Returns the type of a field in a result set (e.g., `"int"`, `"string"`).

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$result`         | `resource\|object` | MySQL result set.                                                           |
| `$field_offset`   | `int`              | Field index (0-based).                                                      |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string\|FALSE` | Field type (e.g., `"int"`, `"varchar"`), or `FALSE` on failure.             |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified field offset.
  - Returns the `type` property of the field object (as an integer in the original code, but typically converted to a string in native `mysql_field_type()`).
- For `mysql_*`:
  - Delegates to `mysql_field_type()`.

**Usage Context:**
- Type-checking or casting query results.
- Example:
  ```php
  $type = mysql_field_type($result, 0);
  if ($type === "int") {
      $value = (int)$value;
  }
  ```

---

#### `mysql_free_result`

**Purpose:**
Frees the memory associated with a result set.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `bool`  | `TRUE` on success, `FALSE` on failure.                                      |

**Inner Mechanisms:**
- Delegates to `mysqli_free_result()` or `mysql_free_result()`.

**Usage Context:**
- Memory management for large result sets.
- Example:
  ```php
  mysql_free_result($result);
  ```

---

#### `mysql_get_client_info`

**Purpose:**
Returns the MySQL client library version.

**Parameters:**
None.

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Client library version (e.g., `"5.7.31"`).                                  |

**Inner Mechanisms:**
- Delegates to `mysqli_get_client_info()` or `mysql_get_client_info()`.

**Usage Context:**
- Debugging or logging environment details.
- Example:
  ```php
  echo "MySQL Client Version: " . mysql_get_client_info();
  ```

---

#### `mysql_get_host_info`

**Purpose:**
Returns information about the MySQL server host.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Host information (e.g., `"Localhost via UNIX socket"`).                     |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_get_host_info()` or `mysql_get_host_info()`.

**Usage Context:**
- Debugging connection issues.
- Example:
  ```php
  echo "Host Info: " . mysql_get_host_info();
  ```

---

#### `mysql_get_proto_info`

**Purpose:**
Returns the MySQL protocol version used by the connection.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int`   | Protocol version (e.g., `10`).                                              |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_get_proto_info()` or `mysql_get_proto_info()`.

**Usage Context:**
- Debugging or ensuring protocol compatibility.
- Example:
  ```php
  echo "Protocol Version: " . mysql_get_proto_info();
  ```

---

#### `mysql_get_server_info`

**Purpose:**
Returns the MySQL server version.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Server version (e.g., `"5.7.31"`).                                          |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_get_server_info()` or `mysql_get_server_info()`.

**Usage Context:**
- Debugging or logging environment details.
- Example:
  ```php
  echo "MySQL Server Version: " . mysql_get_server_info();
  ```

---

#### `mysql_info`

**Purpose:**
Returns detailed information about the last executed query (e.g., rows affected, insert ID).

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string\|NULL` | Query information (e.g., `"Records: 3 Duplicates: 0 Warnings: 0"`), or `NULL` if no information is available. |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_info()` or `mysql_info()`.

**Usage Context:**
- Debugging write queries (e.g., `INSERT`, `UPDATE`, `DELETE`).
- Example:
  ```php
  mysql_query("INSERT INTO users (name) VALUES ('John')");
  echo mysql_info();
  ```

---

#### `mysql_insert_id`

**Purpose:**
Returns the auto-increment ID generated by the last `INSERT` query.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int`   | Auto-increment ID, or `0` if no ID was generated.                           |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_insert_id()` or `mysql_insert_id()`.

**Usage Context:**
- Retrieving the ID of a newly inserted row.
- Example:
  ```php
  mysql_query("INSERT INTO users (name) VALUES ('John')");
  $id = mysql_insert_id();
  ```

---

#### `mysql_list_dbs`

**Purpose:**
Lists all databases available on the MySQL server.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `resource\|object\|FALSE` | Result set containing database names, or `FALSE` on failure.                |

**Inner Mechanisms:**
- Resolves the link identifier.
- For `mysqli`:
  - Executes `SHOW DATABASES` and returns the result.
- For `mysql_*`:
  - Delegates to `mysql_list_dbs()`.

**Usage Context:**
- Enumerating available databases (e.g., for a database management tool).
- Example:
  ```php
  $dbs = mysql_list_dbs();
  while ($db = mysql_fetch_row($dbs)) {
      echo $db[0] . "\n";
  }
  ```

---

#### `mysql_list_processes`

**Purpose:**
Lists all active MySQL server processes.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `resource\|object\|FALSE` | Result set containing process information, or `FALSE` on failure.           |

**Inner Mechanisms:**
- Resolves the link identifier.
- For `mysqli`:
  - Executes `SHOW PROCESSLIST` and returns the result.
- For `mysql_*`:
  - Delegates to `mysql_list_processes()`.

**Usage Context:**
- Monitoring or debugging server activity.
- Example:
  ```php
  $processes = mysql_list_processes();
  while ($process = mysql_fetch_assoc($processes)) {
      echo "Process ID: " . $process["Id"] . "\n";
  }
  ```

---

#### `mysql_num_fields`

**Purpose:**
Returns the number of fields in a result set.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int`   | Number of fields.                                                           |

**Inner Mechanisms:**
- Delegates to `mysqli_num_fields()` or `mysql_num_fields()`.

**Usage Context:**
- Iterating over fields in a result set.
- Example:
  ```php
  $num_fields = mysql_num_fields($result);
  for ($i = 0; $i < $num_fields; $i++) {
      echo mysql_field_name($result, $i) . "\n";
  }
  ```

---

#### `mysql_num_rows`

**Purpose:**
Returns the number of rows in a result set.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int`   | Number of rows.                                                             |

**Inner Mechanisms:**
- Delegates to `mysqli_num_rows()` or `mysql_num_rows()`.

**Usage Context:**
- Determining the size of a result set before processing.
- Example:
  ```php
  $num_rows = mysql_num_rows($result);
  echo "Found $num_rows rows.";
  ```

---

#### `mysql_pconnect`

**Purpose:**
Establishes a persistent MySQL connection.

**Parameters:**

| Name              | Type     | Description                                                                 |
|-------------------|----------|-----------------------------------------------------------------------------|
| `$server`         | `string` | MySQL server address (e.g., `"localhost:3306"`).                            |
| `$username`       | `string` | MySQL username.                                                             |
| `$password`       | `string` | MySQL password.                                                             |
| `$client_flag`    | `int`    | **Ignored in `mysqli` mode.** Client flags.                                 |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `resource\|object` | MySQL link identifier (resource or `mysqli` object).                        |
| `FALSE`            | On failure.                                                                 |

**Inner Mechanisms:**
- For `mysqli`:
  - Parses `$server` to extract port or socket.
  - Calls `mysqli_connect()` with `"p:$server"` to enable persistence.
- For `mysql_*`:
  - Delegates to `mysql_pconnect()`.

**Usage Context:**
- High-traffic applications where connection overhead is a concern.
- Example:
  ```php
  $conn = mysql_pconnect("localhost", "user", "password");
  ```

---

#### `mysql_ping`

**Purpose:**
Checks if the connection to the MySQL server is alive.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `bool`  | `TRUE` if the connection is alive, `FALSE` otherwise.                       |

**Inner Mechanisms:**
- Resolves the link identifier.
- For `mysqli`:
  - Executes `SELECT 1` and returns `TRUE` if successful.
- For `mysql_*`:
  - Delegates to `mysql_ping()`.

**Usage Context:**
- Reconnecting if the connection has timed out.
- Example:
  ```php
  if (!mysql_ping()) {
      mysql_close();
      mysql_connect("localhost", "user", "password");
  }
  ```

---

#### `mysql_query`

**Purpose:**
Executes a SQL query on the MySQL server.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$query`          | `string`           | SQL query to execute.                                                       |
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `resource\|object\|bool` | Result set for `SELECT`, `SHOW`, `DESCRIBE`, or `EXPLAIN` queries. `TRUE` for successful write queries. `FALSE` on failure. |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_query()` or `mysql_query()`.
- Triggers a user error (`E_USER_ERROR`) if the query fails, including the error message and query.

**Usage Context:**
- Executing any SQL query (read or write).
- Example:
  ```php
  $result = mysql_query("SELECT * FROM users");
  if ($result) {
      while ($row = mysql_fetch_assoc($result)) {
          echo $row["username"] . "\n";
      }
  }
  ```

---

#### `mysql_real_escape_string`

**Purpose:**
Escapes special characters in a string for use in a SQL query.

**Parameters:**

| Name                 | Type               | Description                                                                 |
|----------------------|--------------------|-----------------------------------------------------------------------------|
| `$unescaped_string`  | `string`           | String to escape.                                                           |
| `$link_identifier`   | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Escaped string.                                                             |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_real_escape_string()` or `mysql_real_escape_string()`.

**Usage Context:**
- Preventing SQL injection in user-provided input.
- Example:
  ```php
  $username = mysql_real_escape_string($_POST["username"]);
  mysql_query("SELECT * FROM users WHERE username = '$username'");
  ```

---

#### `mysql_result`

**Purpose:**
Retrieves a single field from a result set.

**Parameters:**

| Name       | Type               | Description                                                                 |
|------------|--------------------|-----------------------------------------------------------------------------|
| `$result`  | `resource\|object` | MySQL result set.                                                           |
| `$row`     | `int`              | Row index (0-based).                                                        |
| `$field`   | `string\|int\|NULL` | Field name, index, or `NULL` (defaults to the first field).                 |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string\|FALSE` | Field value, or `FALSE` on failure.                                         |

**Inner Mechanisms:**
- For `mysqli`:
  - Seeks to the specified row.
  - Fetches the row as an array and returns the specified field.
- For `mysql_*`:
  - Delegates to `mysql_result()`.

**Usage Context:**
- Retrieving a single value from a result set (e.g., a count or ID).
- Example:
  ```php
  $count = mysql_result(mysql_query("SELECT COUNT(*) FROM users"), 0, 0);
  ```

---

#### `mysql_select_db`

**Purpose:**
Selects the active database for the connection.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$database_name`  | `string`           | Database name to select.                                                    |
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `bool`  | `TRUE` on success, `FALSE` on failure.                                      |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_select_db()` or `mysql_select_db()`.

**Usage Context:**
- Switching databases within a single connection.
- Example:
  ```php
  mysql_select_db("my_database");
  ```

---

#### `mysql_stat`

**Purpose:**
Returns the current server status (e.g., uptime, threads, queries).

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Server status (e.g., `"Uptime: 1000 Threads: 1 Questions: 5 ..."`).         |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_stat()` or `mysql_stat()`.

**Usage Context:**
- Monitoring server health.
- Example:
  ```php
  echo mysql_stat();
  ```

---

#### `mysql_thread_id`

**Purpose:**
Returns the thread ID of the current connection.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `int`   | Thread ID.                                                                  |

**Inner Mechanisms:**
- Resolves the link identifier.
- Delegates to `mysqli_thread_id()` or `mysql_thread_id()`.

**Usage Context:**
- Debugging or logging connection-specific information.
- Example:
  ```php
  echo "Thread ID: " . mysql_thread_id();
  ```

---

#### `mysql_unbuffered_query`

**Purpose:**
Executes a SQL query without buffering the result set in memory.

**Parameters:**

| Name              | Type               | Description                                                                 |
|-------------------|--------------------|-----------------------------------------------------------------------------|
| `$query`          | `string`           | SQL query to execute.                                                       |
| `$link_identifier` | `resource\|object\|NULL` | Optional. MySQL link identifier. Defaults to the global connection. |

**Return Values:**

| Type               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `resource\|object\|bool` | Result set for `SELECT`, `SHOW`, `DESCRIBE`, or `EXPLAIN` queries. `TRUE` for successful write queries. `FALSE` on failure. |

**Inner Mechanisms:**
- Resolves the link identifier.
- For `mysqli`:
  - Uses `mysqli_real_query()` and `mysqli_use_result()` to avoid buffering.
- For `mysql_*`:
  - Delegates to `mysql_unbuffered_query()`.
- Triggers a user error (`E_USER_ERROR`) if the query fails.

**Usage Context:**
- Processing large result sets without memory constraints.
- Example:
  ```php
  $result = mysql_unbuffered_query("SELECT * FROM large_table");
  while ($row = mysql_fetch_assoc($result)) {
      // Process row
  }
  ```


<!-- HASH:2a9b2ae6401f4283dc0ef03b8ca79336 -->
