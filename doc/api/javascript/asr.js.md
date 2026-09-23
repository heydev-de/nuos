# PWNC API Documentation

[← Index](../README.md) | [`javascript/asr.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/asr.js)

- **Version:** `26.9.7.9`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## asr.js

The `asr.js` file provides a lightweight JavaScript utility layer for asynchronous server requests (ASR) within the PWNC Web Platform. It enables non-blocking communication with the server using the modern `fetch` API, supporting both generic GET requests and form-based POST submissions. This module is designed to enhance user experience by allowing dynamic content updates without full page reloads.

### asr_send

Sends an asynchronous GET request to the specified URL with cache-busting parameters.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `url` | string | — | The target URL for the request. May include a hash fragment. |
| `callback` | function\|null | `null` | Optional callback executed upon successful response. Receives the response text as its argument. |

**Return Value:** None (void)

**Inner Mechanisms:**
1. Generates a random query parameter name prefixed with `_` followed by 8 alphanumeric characters to prevent caching.
2. Splits the URL into base and hash components, then appends the cache-buster parameter.
3. Uses `fetch` with `cache: "no-store"` to ensure fresh responses.
4. On success, passes the response text to the callback. On failure, passes `false`.

**Usage Context:** Ideal for fetching dynamic content, checking server status, or loading partial views without reloading the page.

```javascript
// Fetch latest notifications and update UI
asr_send('/api/notifications', function(response) {
    if (response) {
        document.getElementById('notification-panel').innerHTML = response;
    } else {
        console.error('Failed to load notifications');
    }
});
```

### asr_form_bind

Binds an asynchronous submit handler to a form element, intercepting its default submission behavior.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | HTMLFormElement | — | The form element to bind the handler to. |
| `callback` | function\|null | `null` | Optional callback executed after form submission. Receives the server response text. |

**Return Value:** None (void)

**Inner Mechanisms:**
1. Attaches a `submit` event listener that prevents the default form submission.
2. Calls `asr_form_post` to handle the asynchronous POST request.
3. Overrides the form's native `submit` method to trigger the bound event listener instead.

**Usage Context:** Use when you want to convert standard HTML forms into AJAX-powered forms for seamless user interactions.

```javascript
// Bind contact form for AJAX submission
const contactForm = document.getElementById('contact-form');
asr_form_bind(contactForm, function(response) {
    if (response) {
        alert('Message sent successfully!');
        contactForm.reset();
    } else {
        alert('Failed to send message.');
    }
});
```

### asr_form_unbind

Removes the asynchronous submit handler previously attached to a form element.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | HTMLFormElement | — | The form element from which to remove the handler. |

**Return Value:** None (void)

**Inner Mechanisms:**
1. Removes the custom `submit` event listener.
2. Restores the form's original `submit` method.
3. Cleans up dynamically added properties.

**Usage Context:** Useful for cleanup when dynamically removing forms or switching between AJAX and traditional submission modes.

```javascript
// Remove AJAX binding before form removal
const tempForm = document.getElementById('temporary-form');
asr_form_unbind(tempForm);
tempForm.parentNode.removeChild(tempForm);
```

### asr_form_post

Submits form data asynchronously via POST request using FormData.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `object` | HTMLFormElement | — | The form element whose data will be submitted. |
| `callback` | function\|string | `""` | Optional callback executed after submission. Receives the server response text. |

**Return Value:** None (void)

**Inner Mechanisms:**
1. Creates a `FormData` object from the form element, automatically capturing all input fields.
2. Sends a POST request to the form's `action` URL using `fetch`.
3. On success, passes the response text to the callback. On failure, no callback is invoked.

**Usage Context:** Called internally by `asr_form_bind`, but can also be used directly for programmatic form submissions.

```javascript
// Programmatically submit search form
const searchForm = document.getElementById('search-form');
asr_form_post(searchForm, function(response) {
    document.getElementById('search-results').innerHTML = response;
});


<!-- HASH:46d99c14b472371dee4f22265f664d2a -->
