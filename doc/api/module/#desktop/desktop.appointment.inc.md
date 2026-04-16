# NUOS API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.appointment.inc`](https://github.com/heydev-de/nuos/blob/main/nuos/{{SAFE_LINK}})

- **Version:** `26.4.14.8`
- **Website:** [nuos-web.com](https://nuos-web.com)
- **Repository:** [GitHub](https://github.com/heydev-de/nuos)

---

## Desktop Appointment Module

This file implements the appointment management interface for the NUOS desktop environment. It handles creation, modification, deletion, and display of calendar appointments, providing a user interface for navigating dates, times, and appointment details.

---

### Message Handling / Sub Display

The module processes interface messages (`CMS_IFC_MESSAGE`) to manage appointment state and operations. The following cases are handled:

| Message  | Purpose                                                                                     | Parameters (`$ifc_param`)                     |
|----------|---------------------------------------------------------------------------------------------|-----------------------------------------------|
| `select` | Selects an existing appointment for editing.                                                | Appointment object ID                         |
| `time`   | Sets the current time context for the calendar display.                                     | Unix timestamp                                |
| `add`    | Creates a new appointment at the specified time.                                            | Unix timestamp                                |
| `save`   | Saves changes to an existing appointment.                                                   | See `save` case parameters below              |
| `delete` | Deletes the currently selected appointment.                                                 | None (uses `$object`)                         |

#### Inner Mechanisms
- **State Management**: Uses `$object` (current appointment) and `$time` (current time context) to maintain UI state.
- **Data Operations**: Leverages the `$desktop` object to interact with the underlying data store (e.g., `seek`, `insert`, `object_set`, `delete_object`).
- **Response Handling**: Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR` to signal operation success/failure.

---

### Main Display

Renders the appointment calendar and detail view. The display is divided into:
1. **Navigation Controls**: Year, month, and day selectors.
2. **Time Grid**: Hour-by-hour view of appointments for the selected day.
3. **Appointment Editor**: Form for editing appointment details (visible when `$object` is set).

#### Key Variables

| Variable               | Type      | Description                                                                                     |
|------------------------|-----------|-------------------------------------------------------------------------------------------------|
| `$time_current`        | int       | Current Unix timestamp.                                                                         |
| `$time`                | int       | Selected time context (defaults to current time or appointment time).                          |
| `$hour`, `$minute`     | int       | Extracted hour/minute from `$time`.                                                            |
| `$day`, `$month`, `$year` | int    | Extracted day/month/year from `$time`.                                                          |
| `$day_count`           | array     | Tracks the number of appointments per day (key: day, value: count).                            |
| `$array`               | array     | Nested array of appointments by hour/minute (e.g., `$array[hour][minute][] = appointment_id`).  |
| `$buffer_*`            | string    | HTML buffers for year/month/day/hour selectors.                                                 |
| `$array_*`             | array     | Dropdown options for day/month/year/hour/minute selectors.                                     |

#### Inner Mechanisms
- **Time Normalization**: Adjusts `$time` to the nearest 15-minute interval (`$time -= $time % 900`).
- **Edge Case Handling**: Corrects for hours < 6 (e.g., 00:00–05:00) by shifting to the previous day.
- **Appointment Aggregation**: Scans all appointments to populate `$day_count` and `$array` for the current month.
- **Dynamic UI Generation**: Builds HTML tables for navigation and time grid, with conditional styling (e.g., `highlight` for days with appointments, `varied` for weekends).
- **Localization**: Uses `month()`, `weekday()`, and `CMS_L_*` constants for translated labels.

---

### Functions/Methods

#### `case "save"`
Saves changes to an existing appointment.

##### Parameters
| Parameter      | Type   | Description                                                                                     |
|----------------|--------|-------------------------------------------------------------------------------------------------|
| `$ifc_param1`  | string | Appointment name.                                                                               |
| `$ifc_param2`  | int    | Day (1–31).                                                                                     |
| `$ifc_param3`  | int    | Month (1–12).                                                                                   |
| `$ifc_param4`  | int    | Year (1970–2037).                                                                               |
| `$ifc_param5`  | int    | Hour (0–23).                                                                                    |
| `$ifc_param6`  | int    | Minute (0, 15, 30, 45).                                                                         |
| `$ifc_param7`  | bool   | Whether to set an expiration (24h after appointment time).                                     |
| `$ifc_param8`  | string | Appointment content/description.                                                                |
| `$ifc_param9`  | string | Place/location.                                                                                 |
| `$ifc_param10` | string | Participants (comma-separated).                                                                 |

##### Return/Response
- Sets `$ifc_response` to `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

##### Inner Logic
1. Converts input date/time to a Unix timestamp using `mktime()`.
2. Updates the appointment object with new values via `$desktop->object_set()`.
3. Saves the desktop state via `$desktop->save()`.

##### Usage
Triggered when the user submits the appointment editor form. Form fields must match the parameter order above.

---

### UI Components

#### Navigation Selectors
- **Year/Month/Day Lists**: Clickable tables for navigating dates. Highlights the current selection and days with appointments.
- **Time Grid**: Displays appointments in 15-minute increments, with "Add" buttons for creating new appointments.

#### Appointment Editor
Visible when `$object` is set. Includes:
- **Name**: Text input for the appointment title.
- **Date/Time**: Dropdown selectors for day, month, year, hour, and minute.
- **Expiration**: Checkbox to set an expiration (24h after appointment time).
- **Content**: Textarea for detailed description.
- **Place**: Textarea for location.
- **Participants**: Textarea for participant names/emails.

#### Menu
Dynamic menu items based on context:
- **Save**: Visible when editing an appointment (`$object` is set).
- **Delete**: Visible when editing an appointment.

---

### Integration Points

#### Core Functions Used
| Function               | Purpose                                                                                     |
|------------------------|---------------------------------------------------------------------------------------------|
| `ifc_post()`           | JavaScript helper to send interface messages (e.g., `select`, `time`, `add`).               |
| `mktime()`             | Converts date/time components to Unix timestamp.                                            |
| `date()`               | Formats timestamps for display.                                                             |
| `month()`, `weekday()` | Localized month/weekday names.                                                              |
| `x()`                  | XML-escapes strings for HTML output.                                                        |
| `qrx()`                | Escapes strings for JavaScript/URL contexts.                                                |
| `image()`              | Generates HTML for icons.                                                                   |
| `ifc_table_open()`     | Opens an HTML table with NUOS-specific styling.                                              |
| `ifc_varied()`         | Applies alternating row styling.                                                            |

#### Data Flow
1. **Interface Messages**: Incoming messages (e.g., `select`, `save`) are processed to update state.
2. **Desktop Data**: Appointments are stored in the `$desktop->data` collection, with methods like:
   - `seek()`: Finds appointments by subtype.
   - `insert()`: Adds new appointments.
   - `object_get()`/`object_set()`: Gets/sets appointment properties.
   - `delete_object()`: Removes appointments.
3. **Output**: The `ifc` object (`$ifc`) renders the UI, combining navigation, time grid, and editor.

---

### Typical Usage Scenarios

#### Creating an Appointment
1. User clicks an "Add" button in the time grid.
2. The `add` message is sent with the selected timestamp.
3. A new appointment is created with default values (name, expiration).
4. The editor form appears for the new appointment.

#### Editing an Appointment
1. User clicks an appointment in the time grid.
2. The `select` message is sent with the appointment ID.
3. The editor form is populated with the appointment's data.
4. User modifies fields and clicks "Save".
5. The `save` message is sent with updated values.

#### Deleting an Appointment
1. User selects an appointment and clicks "Delete".
2. The `delete` message is sent.
3. The appointment is removed from the data store.

#### Navigating Dates
1. User clicks a year/month/day in the navigation tables.
2. The `time` message is sent with the new timestamp.
3. The time grid updates to show appointments for the selected date.


<!-- HASH:4a145462fa3b6524f2a627c6cb44093c -->
