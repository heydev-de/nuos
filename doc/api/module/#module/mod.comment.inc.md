# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.comment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.comment.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

The file `module/#module/mod.comment.inc` is the **comment module** for the PWNC Web Platform. It handles all aspects of comment management including:

- Displaying existing comments with pagination
- Adding new comments (with spam detection and CAPTCHA support)
- Editing existing comments
- Rating comments (positive/negative)
- Administrative actions: enabling, disabling, deleting, and marking as spam
- Email notifications for new comments and approvals
- Frontend forms for both adding and editing comments

The module integrates with the `comment` library class, database constants, and various utility functions for security, escaping, and URL generation.

## Global Variables

| Variable | Description |
|----------|-------------|
| `$comment_message` | Action to perform: `add`, `edit`, `_edit`, `rate_good`, `rate_bad`, `disable`, `enable`, `delete`, `spam` |
| `$comment_index` | Index/ID of the comment being operated on |
| `$comment_page` | Current page number for pagination |
| `$comment_add_name` | Name field for adding a comment |
| `$comment_add_email` | Email field for adding a comment |
| `$comment_add_url` | URL field for adding a comment |
| `$comment_add_text` | Text content for adding a comment |
| `$comment_add_message` | Submit button value for add form |
| `$comment_add_captcha_key` | User-entered CAPTCHA key |
| `$comment_add_captcha_code` | Hidden CAPTCHA verification code |
| `$comment_edit_name` | Name field for editing a comment |
| `$comment_edit_email` | Email field for editing a comment |
| `$comment_edit_url` | URL field for editing a comment |
| `$comment_edit_text` | Text content for editing a comment |
| `$comment_edit_message` | Submit button value for edit form |

## Configuration Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `$default_status` | `CMS_DB_COMMENT_STATUS_INACTIVE` | Default status for new comments |
| `$notification_email` | `""` | Email address for comment notifications |
| `$display_order` | `FALSE` | Whether to display comments in ascending order |
| `$spam_threshold` | `95` | Spam probability threshold (0-100) |
| `$use_captcha` | `FALSE` | Whether to require CAPTCHA for anonymous users |

## Main Processing Flow

### Initialization

The module begins by:
1. Loading the `comment` library via `cms_load("comment")`
2. Determining the instance (content index) from global config or `CMS_CONTENT_INDEX`
3. Creating a `comment` object instance
4. Checking if comments are enabled and if the user has appropriate permissions
5. Setting up configuration variables from global settings

### Message Processing

The core logic uses a `switch` statement on `$comment_message` to handle different actions:

#### `add` - Adding a New Comment

Handles the submission of a new comment:

1. **Permission Check**: Verifies the user has writer access
2. **Anonymous User Handling**: Pre-fills name and email for logged-in users
3. **Input Validation**:
   - Strips extra spaces from name and text
   - Validates name is not empty
   - Validates email format
   - Normalizes URL (adds `https://` prefix if missing)
   - Validates text is not empty
4. **CAPTCHA Verification**: If enabled, verifies the CAPTCHA response
5. **Comment Addition**: Calls `$comment->add()` with validated data
6. **Error Handling**: Maps return codes to appropriate error messages:
   - `-1`: Spam detected
   - `-2`: Duplicate post
   - `FALSE`: General error
7. **Success Handling**: Sets success message based on default status
8. **Email Notification**: Sends notification to configured email address
9. **Form Reset**: Clears input fields after successful submission

**Usage Example**:
```php
// When a user submits the comment form
$comment_message = "add";
$comment_add_name = "John Doe";
$comment_add_email = "john@example.com";
$comment_add_url = "https://johndoe.com";
$comment_add_text = "Great article!";
$comment_add_captcha_key = "user_input";
$comment_add_captcha_code = "hidden_code";
```

#### `rate_good` / `rate_bad` - Rating Comments

Allows users to rate comments positively or negatively:

1. Calls `$comment->rate_good()` or `$comment->rate_bad()` with the comment index
2. Sets error if rating fails

**Usage Example**:
```php
// User clicks "Good" button
$comment_message = "rate_good";
$comment_index = 42;
```

#### `_edit` - Processing Comment Edit

Handles the actual saving of edited comment data:

1. **Permission Check**: Verifies operator access
2. **Cancel Handling**: If cancel button pressed, resets message
3. **Input Validation**: Same as add operation
4. **Comment Update**: Calls `$comment->edit()` with updated data
5. **Success/Error Handling**: Sets appropriate messages

**Usage Example**:
```php
// Form submission for editing
$comment_message = "_edit";
$comment_index = 42;
$comment_edit_name = "Updated Name";
$comment_edit_email = "updated@example.com";
$comment_edit_url = "https://updated.com";
$comment_edit_text = "Updated text content";
```

#### `disable` / `enable` - Status Management

Administrative actions to hide/show comments:

- **disable**: Sets comment status to `CMS_DB_COMMENT_STATUS_HIDDEN`
- **enable**: Sets comment status to `CMS_DB_COMMENT_STATUS_ACTIVE` and sends approval notification if transitioning from inactive

**Usage Example**:
```php
// Admin enables a comment
$comment_message = "enable";
$comment_index = 42;
```

#### `delete` / `spam` - Comment Removal

Administrative actions to remove comments:

- **delete**: Permanently removes comment
- **spam**: Removes comment and marks as spam (trains spam filter)

**Usage Example**:
```php
// Admin deletes a comment
$comment_message = "delete";
$comment_index = 42;
```

## Comment Display

### Database Query

Retrieves comments with:
- Filtering by instance
- Status filtering (active only for non-operators)
- Ordering (ascending or descending by time)
- Pagination (10 comments per page)

### HTML Output

Generates structured HTML with:
- Schema.org microdata (`itemscope`, `itemtype="https://schema.org/Comment"`)
- Comment head section (author name, timestamp)
- Comment body (text content)
- Comment foot (rating display)
- Administrative controls (for operators)

### JavaScript Functions

Generates client-side JavaScript for:
- `comment_rate_bad(index)`: Negative rating confirmation
- `comment_rate_good(index)`: Positive rating confirmation
- `comment_delete(index)`: Delete confirmation
- `comment_spam(index)`: Spam marking confirmation

## Forms

### Add Comment Form

Displayed when:
- User has writer access
- No success message is set
- Default case in form switch

Includes fields for:
- Name (for anonymous users)
- Email (for anonymous users or unverified emails)
- Homepage URL
- Comment text
- CAPTCHA (if enabled)

### Edit Comment Form

Displayed when:
- User has operator access
- `$comment_message` is `edit` or `_edit`

Pre-fills form with existing comment data and includes:
- Name field
- Email field
- Homepage URL field
- Comment text area
- Cancel and Save buttons

## Pagination

Uses the `pagination()` function to generate navigation links with:
- URL template using `cms_url()` with page placeholder
- Current page tracking
- Total page count calculation
- CSS class for styling

## Permissions

At the end of the module, displays permission information using:
```php
permission([
    CMS_COMMENT_PERMISSION_READER . ".$instance" => CMS_L_READ,
    CMS_COMMENT_PERMISSION_WRITER . ".$instance" => CMS_L_WRITE,
    CMS_COMMENT_PERMISSION_OPERATOR . ".$instance" => CMS_L_OPERATOR
]);
```

## Helper Functions Used

| Function | Purpose |
|----------|---------|
| `cms_load()` | Loads required libraries |
| `sqlesc()` | SQL-escapes values |
| `x()` | XML-escapes values for HTML output |
| `q()` | JS/JSON-encodes strings |
| `u()` | Generates URLs with parameters |
| `cms_url()` | Core URL generator |
| `cms_param()` | Manages query string parameters |
| `verify_email()` | Validates email format |
| `stripspaces()` | Removes extra whitespace |
| `friendly_date()` | Formats dates for display |
| `pagination()` | Generates pagination HTML |
| `permission()` | Displays permission information |
| `insert()` | Inserts content at specified position |
| `jscript()` | Outputs JavaScript code |
| `smtp_send()` | Sends email notifications |

## Constants Used

| Constant | Description |
|----------|-------------|
| `CMS_MSG_UNAVAILABLE` | Message shown when module is unavailable |
| `CMS_USER` | Current user identifier |
| `CMS_NAME` | Current user's display name |
| `CMS_EMAIL` | Current user's email |
| `CMS_IPHASH` | IP hash for rating tracking |
| `CMS_MODULES_URL` | Base URL for modules |
| `CMS_DB_COMMENT_*` | Database table and field constants |
| `CMS_L_MOD_COMMENT_*` | Language strings for comments |
| `CMS_L_NAME` | Language string for "Name" |
| `CMS_L_EMAIL` | Language string for "Email" |
| `CMS_L_COMMAND_*` | Language strings for commands |
| `CMS_COMMENT_PERMISSION_*` | Permission level constants |
| `CMS_L_READ` | Language string for "Read" |
| `CMS_L_WRITE` | Language string for "Write" |
| `CMS_L_OPERATOR` | Language string for "Operator" |


<!-- HASH:cb54642679f9844441a81cf38505bdd0 -->
