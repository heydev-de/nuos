# PWNC API Documentation

[← Index](../README.md) | [`javascript/asr.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/asr.js)

- **Version:** `26.7.21.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## ASR (Asynchronous Server Request) Module

JavaScript utility for handling asynchronous server requests in PWNC. Provides functions for sending GET/POST requests without page reloads, with optional callback handling. Designed for seamless integration with PWNC's URL and parameter management system.

---

### `asr_send(url, callback = null)`

Sends an asynchronous GET request to the specified URL with cache-busting and optional callback.

#### Parameters

| Name       | Type       | Default | Description                                                                 |
|------------|------------|---------|-----------------------------------------------------------------------------|
| `url`      | string     | -       | Target URL (may include query string and fragment)                          |
| `callback` | function   | `null`  | Optional callback function receiving the server response (`string|false`)   |

#### Return Value
`void`

#### Inner Mechanisms
1. **Cache Busting**: Appends a random 8-character alphanumeric parameter to the URL to prevent browser caching
2. **URL Parsing**: Preserves existing query parameters and fragments while adding the cache-buster
3. **Fetch API**: Uses `fetch()` with `no-store` cache policy
4. **Error Handling**: Returns `false` to callback on HTTP errors or network failures

#### Usage Context
- Loading dynamic content without page reloads
- Fetching data for client-side rendering
- Polling server for updates

#### Example
```javascript
// Load user profile asynchronously
asr_send(
    u('content://user/profile', {id: 42}),
    function(response) {
        if (response !== false) {
            document.getElementById('profile').innerHTML = response;
        } else {
            console.error('Failed to load profile');
        }
    }
);
```

---

### `asr_form_bind(object, callback = null)`

Binds a form to asynchronous submission handling. Overrides default form submission behavior to use `asr_form_post()`.

#### Parameters

| Name       | Type       | Default | Description                                                                 |
|------------|------------|---------|-----------------------------------------------------------------------------|
| `object`   | HTMLFormElement | -   | Form element to bind                                                        |
| `callback` | function   | `null`  | Optional callback function receiving the server response (`string`)         |

#### Return Value
`void`

#### Inner Mechanisms
1. **Event Override**: Prevents default form submission via `event.preventDefault()`
2. **Method Preservation**: Stores original `submit()` method for later restoration
3. **Event Dispatch**: Replaces `submit()` with event-triggering version to maintain compatibility

#### Usage Context
- Converting traditional forms to AJAX submissions
- Progressive enhancement of existing forms
- Forms requiring client-side validation before submission

#### Example
```javascript
// Bind contact form to async submission
const contactForm = document.getElementById('contact-form');
asr_form_bind(contactForm, function(response) {
    if (response.includes('success')) {
        alert('Message sent!');
        contactForm.reset();
    }
});
```

---

### `asr_form_unbind(object)`

Restores a form's original submission behavior after `asr_form_bind()`.

#### Parameters

| Name     | Type       | Default | Description                     |
|----------|------------|---------|---------------------------------|
| `object` | HTMLFormElement | -   | Form element to unbind          |

#### Return Value
`void`

#### Inner Mechanisms
1. **Event Cleanup**: Removes the custom submit event listener
2. **Method Restoration**: Reverts `submit()` to its original implementation

#### Usage Context
- Temporary form binding (e.g., during modal display)
- Cleanup before form removal from DOM
- Testing scenarios requiring original behavior

#### Example
```javascript
// Unbind form before removing from DOM
asr_form_unbind(contactForm);
document.body.removeChild(contactForm);
```

---

### `asr_form_post(object, callback = "")`

Submits a form asynchronously via POST request.

#### Parameters

| Name       | Type       | Default | Description                                                                 |
|------------|------------|---------|-----------------------------------------------------------------------------|
| `object`   | HTMLFormElement | -   | Form element to submit                                                      |
| `callback` | function   | `""`    | Optional callback function receiving the server response (`string|false`)   |

#### Return Value
`void`

#### Inner Mechanisms
1. **FormData**: Creates multipart/form-data payload from form fields
2. **POST Request**: Uses `fetch()` with POST method and form data body
3. **Response Handling**: Returns raw text response or `false` on failure

#### Usage Context
- Form submissions requiring file uploads
- Complex forms with many fields
- POST requests requiring CSRF protection (handled automatically by PWNC's backend)

#### Example
```javascript
// Submit login form asynchronously
const loginForm = document.getElementById('login-form');
asr_form_post(loginForm, function(response) {
    if (response.includes('dashboard')) {
        window.location = u('content://dashboard');
    } else {
        document.getElementById('error').textContent = 'Login failed';
    }
});
```


<!-- HASH:46d99c14b472371dee4f22265f664d2a -->
