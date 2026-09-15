# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.search.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Search Module

The `mod.search.inc` file is the frontend controller for the **Search module** in the PWNC Web Platform. It handles both the display of a search form, execution of search queries, rendering of results with pagination, and a tag cloud of popular search terms. It also provides a submission interface for users to queue new URLs for indexing.

This module relies on the `search` library (loaded via `cms_load`) and interacts with the database to count indexed entries. It uses global variables to manage state such as the current search term, language filter, page number, and messages.

---

### Global Variables

| Name | Default | Description |
|------|---------|-------------|
| `$search_message` | `NULL` | Controls which view to render (`"submit"` or default search). |
| `$search_term` | `NULL` | The current search query string entered by the user. |
| `$search_language` | `NULL` | Language filter for search results (`#any` means all languages). |
| `$search_page` | `NULL` | Current page number for paginated results. |
| `$search_submit_url` | `NULL` | URL submitted for indexing (used in the submit form). |
| `$search_submit_message` | `NULL` | Action triggered from the submit form (e.g., cancel or submit). |

---

## Main Flow

1. **Library Check**: Loads the `search` library and checks if the search feature is enabled.
2. **View Routing**:
   - If `$search_message == "submit"`, renders the URL submission form.
   - Otherwise, renders the main search interface.
3. **Search Execution**:
   - Counts total indexed entries.
   - Displays a language selector dropdown.
   - Renders the search input field.
   - Executes the search if a term is provided.
   - Displays results with pagination links.
   - Shows a tag cloud of frequently searched terms.
4. **Permissions**:
   - Read access for all users.
   - Write/submit access controlled by `CMS_SEARCH_PERMISSION_SUBMIT`.

---

## Submit View (`case "submit"`)

### Purpose
Renders a form allowing authorized users to submit a URL for indexing.

### Parameters (via POST)
| Name | Type | Description |
|------|------|-------------|
| `search_submit_url` | string | The URL to be queued for indexing. |
| `search_submit_message` | string | Either `CMS_L_COMMAND_CANCEL` (to cancel) or `CMS_L_MOD_SEARCH_004` (to submit). |

### Logic
- Checks user permission via `cms_permission(CMS_SEARCH_PERMISSION_SUBMIT)`.
- If the user cancels, exits early.
- Otherwise, queues the URL using `$search->queue_add()` and displays the form.

### Example Usage
A logged-in editor navigates to `?search_message=submit`, enters a URL like `https://example.com/new-article`, and clicks the submit button. The URL is added to the search index queue.

---

## Search View (Default)

### Purpose
Displays the main search interface including:
- Total indexed entry count.
- Language filter dropdown.
- Search input field.
- Search results with pagination.
- Tag cloud of popular terms.
- Link to submit new URLs (if permitted).

### Key Functions Used

#### `mysql_query`
Executes a SQL query to count indexed entries where `CMS_DB_SEARCH_ENTRY_ERROR = '0'`.

#### `$search->find($search_term, $search_page, $_search_language)`
Performs the actual search.

| Parameter | Type | Description |
|----------|------|-------------|
| `$search_term` | string | Query string to search for. |
| `$search_page` | int | Page number for pagination. |
| `$_search_language` | string\|NULL | Language code or `NULL` for any language. |

Returns:
- `FALSE` if no results found.
- Array of result rows on success.

Each row contains:
- `$title`: Title of the matched page.
- `$address`: URL of the matched page.
- `$text`: Snippet/excerpt of the matched content.
- `$time`: Timestamp of last update.
- `$score`: Relevance score.
- `$supplemental`: Number of similar pages (optional).

#### `$search->tag(NULL, 100, $_search_language)`
Retrieves top 100 most frequently searched/indexed terms for the tag cloud.

| Parameter | Type | Description |
|----------|------|-------------|
| `NULL` | — | Reserved for future use. |
| `100` | int | Maximum number of tags to return. |
| `$_search_language` | string\|NULL | Language filter. |

Returns:
- Associative array mapping term strings to their frequency scores.

### Example Usage
A visitor types `"PHP tutorial"` into the search box and submits. The system executes a search across indexed pages, returning relevant matches sorted by relevance. Results are displayed with pagination controls and a tag cloud suggesting related topics.

---

## Helper Functions Used

| Function | Description |
|---------|-------------|
| `cms_load("search")` | Loads the search library. |
| `cms_permission(...)` | Checks if the current user has required permissions. |
| `cms_url(...)` | Generates URLs preserving current state. |
| `x(...)` | Escapes output for HTML context. |
| `select(...)` | Renders an HTML `<select>` dropdown. |
| `insert(...)` | Inserts reusable template snippets. |
| `permission(...)` | Displays permission legend. |
| `jscript(...)` | Outputs inline JavaScript. |
| `friendly_date(...)` | Formats timestamps into human-readable dates. |
| `number_format(...)` | Formats numbers with locale-specific separators. |

---

## Output Structure

```html
<section id="search" class="search">
  <!-- Header with indexed count -->
  <h2>...</h2>

  <!-- Search Form -->
  <form name="search_input">
    <!-- Language Selector -->
    <select>...</select>

    <!-- Search Input -->
    <input type="text" name="search_term">

    <!-- Submit Button -->
    <input type="submit">
  </form>

  <!-- Search Results -->
  <div class="search-result">
    <ol>
      <li>
        <a href="..."><strong>Title</strong></a><br>
        Excerpt text...
        <small>URL | Date | Score</small>
      </li>
    </ol>
  </div>

  <!-- Pagination -->
  <nav class="search-pagination">
    <a href="?search_page=0">1 - 10</a> |
    <a href="?search_page=1">11 - 20</a> |
    <a href="?search_page=2">More results</a>
  </nav>

  <!-- Tag Cloud -->
  <div class="search-cloud">
    <a href="?search_term=term1" style="font-size:2em;opacity:0.8">term1</a>
    ...
  </div>

  <!-- Submit New URL Link -->
  <a href="?search_message=submit">Submit URL</a>
</section>
```

---

## Security Considerations

- All user inputs are escaped using `x()` before being rendered in HTML.
- URLs are generated using `cms_url()` which includes CSRF protection.
- Permissions are enforced before allowing URL submissions.
- SQL queries use predefined constants for table/column names to prevent injection.

---

## Constants Referenced

| Constant | Description |
|---------|-------------|
| `CMS_MSG_UNAVAILABLE` | Message shown when search module is disabled. |
| `CMS_SEARCH_PERMISSION_SUBMIT` | Permission key for submitting URLs. |
| `CMS_L_MOD_SEARCH_*` | Language strings for UI labels/buttons. |
| `CMS_L_COMMAND_CANCEL` | Label for the cancel button. |
| `CMS_L_URL` | Label for the URL input field. |
| `CMS_L_READ` / `CMS_L_WRITE` | Permission level labels. |
| `CMS_DB_SEARCH_ENTRY` | Database table name for indexed entries. |
| `CMS_DB_SEARCH_ENTRY_ERROR` | Column indicating indexing errors. |
| `CMS_LANGUAGE` | Current site language. |
| `CMS_L_DECIMAL_SEPARATOR` | Locale decimal separator. |
| `CMS_L_THOUSAND_SEPARATOR` | Locale thousands separator. |


<!-- HASH:6fee4cc784fc23c179edb8edf4ac0273 -->
