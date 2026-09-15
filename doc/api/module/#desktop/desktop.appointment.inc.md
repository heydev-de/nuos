# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.appointment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.appointment.inc)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Appointment Management Interface

This file implements the desktop appointment management interface for the PWNC Web Platform. It provides a calendar-based UI for viewing, creating, editing, and deleting appointments. The interface displays year, month, day, and hourly views with appointment data, allowing users to navigate through time periods and manage appointment details.

The module operates through a message-driven architecture where different actions (select, time, add, save, delete) trigger specific behaviors. It integrates with the desktop data system to persist appointment objects and uses the platform's internationalization and UI components.

### Message Handling

The module processes different interface messages through a switch statement on `CMS_IFC_MESSAGE`:

| Message | Description |
|---------|-------------|
| `select` | Selects an appointment object for viewing/editing |
| `time` | Sets the current time context for the calendar view |
| `add` | Creates a new appointment at a specified time |
| `save` | Updates an existing appointment's properties |
| `delete` | Removes an appointment from the system |

#### select

Sets the current object context to the specified appointment for viewing or editing.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | string | The object identifier of the appointment to select |

**Usage:**
```php
// Selects appointment with ID "abc123" for editing
ifc_post('select', 'abc123');
```

#### time

Sets the time context for the calendar view without selecting any specific appointment.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | integer | Unix timestamp representing the time to view |

**Usage:**
```php
// Navigate calendar to January 15, 2024
ifc_post('time', mktime(6, 0, 0, 1, 15, 2024));
```

#### add

Creates a new appointment at the specified time. The appointment is initialized with default values and a 24-hour expiration time.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | integer | Unix timestamp for the appointment time |

**Inner Mechanisms:**
1. Seeks an available slot in the desktop data storage
2. Creates a buffer with default appointment data including name, type, time, and expiration
3. Inserts the new object and saves the desktop state
4. Sets the response to indicate success or failure

**Usage:**
```php
// Add appointment at 2:30 PM today
ifc_post('add', mktime(14, 30, 0, 6, 15, 2024));
```

#### save

Updates an existing appointment's properties including name, time, expiration, content, place, and participants.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | The appointment object identifier |
| `$ifc_param1` | string | Appointment name/title |
| `$ifc_param2` | string | Appointment time (parsed as date string) |
| `$ifc_param3` | boolean | Whether to set 24-hour expiration |
| `$ifc_param4` | string | Appointment content/description |
| `$ifc_param5` | string | Appointment location/place |
| `$ifc_param6` | string | Appointment participants |

**Inner Mechanisms:**
1. Updates the appointment name
2. Parses the time parameter and updates the appointment time
3. Conditionally sets or clears the expiration flag
4. Updates content, place, and participant fields
5. Saves changes to the desktop storage

**Usage:**
```php
// Save updated appointment details
ifc_post('save', 'abc123', 'Team Meeting', '2024-06-15 14:30', true, 
         'Discuss project timeline', 'Conference Room A', 'John, Sarah');
```

#### delete

Removes an appointment from the system permanently.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$object` | string | The appointment object identifier to delete |

**Usage:**
```php
// Delete appointment with ID "abc123"
ifc_post('delete', 'abc123');
```

### Calendar Display Logic

The main display section generates a comprehensive calendar interface with multiple time views:

#### Time Normalization

The system normalizes time to 15-minute intervals and adjusts for a day starting at 6 AM:

```php
$time -= $time % 900; // Round down to nearest 15 minutes
if ($hour < 6) {
    $hour += 24; // Treat early morning hours as previous day
    $day -= 1;
}
```

#### Appointment Listing

Appointments are collected and organized by hour and minute for the currently viewed day:

```php
$list[$_hour][$_minute][] = $key; // Group appointments by time slot
$count[$_year][$_month][$_day] = ($count[$_year][$_month][$_day] ?? 0) + 1; // Count per day
```

#### Year View

Generates a 10-year range (5 years before and after current view) with appointment counts:

```php
for ($_year = $year - 5; $_year <= $year + 5; $_year++) {
    // Skip invalid years (before 1970 or after 2037)
    if (($_year < 1970) || ($_year > 2037)) continue;
    // Generate year cell with appointment count
}
```

#### Month View

Displays all 12 months with appointment counts for the selected year:

```php
for ($_month = 1; $_month <= 12; $_month++) {
    $_count = $sum_recursive($count[$year][$_month]);
    // Generate month cell with count
}
```

#### Day View

Shows weekday abbreviations and day numbers with appointment counts:

```php
for ($_day = 1; $_day <= $days_per_month; $_day++) {
    $day_week = (int)date("w", $_time);
    $day_name = substr(weekday($day_week), 0, 2);
    // Generate day cell with count
}
```

#### Hour View

Creates a detailed hourly schedule from 6 AM to 6 AM next day (24-hour format) with 15-minute intervals:

```php
for ($_hour = 6; $_hour < 30; $_hour++) {
    for ($_minute = 0; $_minute <= 45; $_minute += 15) {
        // Generate time slot with appointment links or add button
    }
}
```

### Menu System

The context menu changes based on whether an appointment is selected:

```php
$menu = stre($object) ? NULL : // No menu when no object selected
[CMS_L_COMMAND_SAVE . "|desktop/command_save" => "save",
 CMS_L_COMMAND_DELETE . "|desktop/command_delete" => "#delete"];
```

### Appointment Data Form

When an appointment is selected, a detailed form is displayed with fields for:

| Field | Type | Description |
|-------|------|-------------|
| Name | Text input | Appointment title |
| Date/Time | DateTime picker | Appointment time |
| Expiration | Checkbox | Set 24-hour expiration |
| Content | Textarea | Detailed description |
| Place | Textarea | Location |
| Participant | Textarea | Attendees |

**Usage Example:**
```php
// Full workflow example
// 1. Navigate to a time
ifc_post('time', mktime(6, 0, 0, 6, 15, 2024));

// 2. Add an appointment
ifc_post('add', mktime(14, 30, 0, 6, 15, 2024));

// 3. Select the newly created appointment (assuming it returns object ID)
ifc_post('select', $newObjectId);

// 4. Save updated details
ifc_post('save', $newObjectId, 'Project Review', '2024-06-15 14:30', 
         true, 'Review Q2 deliverables', 'Building 3, Room 205', 'Management Team');

// 5. Delete if needed
ifc_post('delete', $newObjectId);
```


<!-- HASH:1cd6ba8966f0b34f583457e9c6fb4102 -->
