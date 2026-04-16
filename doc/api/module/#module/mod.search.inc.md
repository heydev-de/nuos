# NUOS API Documentation

[← Index](../../README.md) | [`module/#module/mod.search.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Module: Search (`mod.search.inc`)

Core search module for the NUOS platform. Provides frontend functionality for:
- Performing full-text searches across indexed content
- Displaying paginated search results
- Rendering a tag cloud of popular search terms
- Submitting new URLs for indexing (with proper permissions)

Handles all UI rendering, form processing, and result presentation while delegating actual search operations to the `search` class.

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$search_message` | string | Controls module behavior: `"submit"` triggers URL submission form |
| `$search_term` | string | Current search query |
| `$search_language` | string | Language filter (`#any` or language code) |
| `$search_page` | int | Current result page (0-based) |
| `$search_submit_url` | string | URL being submitted for indexing |
| `$search_submit_message` | string | Submission action (`CMS_L_COMMAND_CANCEL` or `CMS_L_MOD_SEARCH_004`) |

---

### Module Flow

1. **Initialization**
   - Verifies search module is loaded and enabled
   - Instantiates `search` class
   - Exits if unavailable

2. **Routing**
   - `submit`: Renders URL submission form (requires `CMS_SEARCH_PERMISSION_SUBMIT`)
   - Default: Renders search form and results

3. **Search Execution**
   - Validates input
   - Calls `$search->find()` with term, page, and language
   - Displays results or "no results" message

4. **Pagination**
   - Shows navigation for previous pages
   - Offers "further results" link if more pages exist

5. **Tag Cloud**
   - Displays top 100 weighted terms when no search term is entered
   - Visual weighting via font size and opacity

6. **Permissions**
   - Displays submission link if user has `CMS_SEARCH_PERMISSION_SUBMIT`

---

### Key Functions (Implicit)

#### `search_form()`
**Purpose**: Renders the search input form with language selector.

**Parameters**:
None (uses global variables)

**Return**:
None (outputs HTML directly)

**Mechanisms**:
- Builds language dropdown from system languages
- Preserves current search term in input field
- Focuses search input via JavaScript

**Usage**:
Automatically rendered when module loads. No direct invocation.

---

#### `search_results()`
**Purpose**: Displays paginated search results or "no results" message.

**Parameters**:
None (uses global variables)

**Return**:
None (outputs HTML directly)

**Mechanisms**:
- Calls `$search->find()` with sanitized inputs
- Extracts result fields (`title`, `text`, `address`, `time`, `score`, `supplemental`)
- Formats results with:
  - Title link
  - Snippet text
  - URL, date, and score
  - Supplemental result count (if applicable)

**Usage**:
Triggered when `$search_term` is not empty.

---

#### `pagination_nav()`
**Purpose**: Generates navigation links for search result pages.

**Parameters**:
None (uses global variables)

**Return**:
None (outputs HTML directly)

**Mechanisms**:
- Shows range links for previous pages (e.g., "1-10", "11-20")
- Displays "further results" link if more pages exist
- Uses pipe separators between links

**Usage**:
Rendered when results exist or current page > 0.

---

#### `tag_cloud()`
**Purpose**: Displays a weighted tag cloud of popular search terms.

**Parameters**:
None (uses global variables)

**Return**:
None (outputs HTML directly)

**Mechanisms**:
- Fetches top 100 terms via `$search->tag()`
- Calculates visual weight:
  - Font size: 15px–40px (linear to term frequency)
  - Opacity: 0.35–1.0 (linear to term frequency)
- Sorts terms alphabetically

**Usage**:
Rendered when no search term is entered.

---

#### `submit_form()`
**Purpose**: Renders form for submitting URLs to the search index.

**Parameters**:
None (uses global variables)

**Return**:
None (outputs HTML directly)

**Mechanisms**:
- Requires `CMS_SEARCH_PERMISSION_SUBMIT`
- Adds submitted URL to queue via `$search->queue_add()`
- Provides cancel and submit buttons

**Usage**:
Triggered when `$search_message === "submit"`.

---

### Constants & Configuration

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_DB_SEARCH_ENTRY` | Database table | Stores search index entries |
| `CMS_DB_SEARCH_ENTRY_ERROR` | Column name | Error flag for entries |
| `CMS_SEARCH_PERMISSION_SUBMIT` | Permission ID | Required to submit URLs |
| `CMS_SEARCH_QUEUE_TYPE_SUBMISSION` | Queue type | Identifies submission queue items |
| `CMS_L_MOD_SEARCH_*` | Localized strings | UI text for search module |

---

### Integration Notes

1. **Dependencies**:
   - Requires `search` class (loaded via `cms_load("search")`)
   - Uses `data` class for language list
   - Relies on `cms_url()` for URL generation

2. **Security**:
   - All outputs escaped via `x()`
   - CSRF protection via `cms_url()`
   - Permission checks for submission

3. **Performance**:
   - Pagination limits results per page
   - Tag cloud limited to top 100 terms

4. **Customization**:
   - Override `insert("submit_top")`/`insert("submit_bottom")` for custom markup
   - Adjust `$search->find_results_per_page` for pagination size


<!-- HASH:524cb950f8be2e85f28af614ddf67f0a -->
