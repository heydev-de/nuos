/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

this.name ||= "content_edit_" + Math.floor(Math.random() * 99999999);
var content_buffer = null;

function content_load(url)
{
    parent.location.replace(url);
};

function content_edit_open(url)
{
    load_page(url);
};

function content_edit_command(url,
                              text = "")
{
    if ((text !== "") && (! confirm(text + "?"))) return;
    location.replace(url
        .replace(/%left%/g, fx_position_left())
        .replace(/%top%/g,  fx_position_top()));
};

function content_edit_prompt(url,
                             text,
                             value,
                             pattern,
                             negate = false)
{
    value = prompt(text, value);
    if (value === null) return;

    if (! pattern.test(value))
    {
        alert(CMS_L_MOD_CONTENT_013);
        return;
    };

    content_edit_command(url.replace(/%return%/g, negate ? -value : value));
};

function content_edit_copy(url,
                           range)
{
    asr_send(url);
    content_buffer = range;
};

function content_edit_paste(url)
{
    content_edit_command(url, CMS_L_COMMAND_PASTE);
};

function content_edit_swap(url)
{
    if (content_buffer) content_edit_command(url, CMS_L_MOD_CONTENT_005);
    else                alert(CMS_L_MOD_CONTENT_002);
};

function content_edit_kick1(url)
{
    content_edit_prompt(url, CMS_L_MOD_CONTENT_011, 1, /^[0-9]+$/);
};

function content_edit_kick2(url)
{
    content_edit_prompt(url, CMS_L_MOD_CONTENT_012, 1, /^[0-9]+$/, true);
};

function content_edit_clear(url)
{
    content_edit_command(url, CMS_L_COMMAND_DELETE);
};

function content_edit_repeat(url,
                             value)
{
    content_edit_prompt(url, CMS_L_MOD_CONTENT_009, value, /^[0-9]+$/);
};

function content_edit_shift(url,
                            value)
{
    content_edit_prompt(url, CMS_L_MOD_CONTENT_010, value, /^-?[0-9]+$/);
};

function content_edit_switch(url,
                             value)
{
    content_edit_command(url.replace(/%return%/g, (value === "") ? "1" : ""));
};

function content_edit_apply(url)
{
    content_edit_command(url, CMS_L_MOD_CONTENT_003);
};

function content_edit_revert(url)
{
    content_edit_command(url, CMS_L_MOD_CONTENT_004);
};

function content_edit_restore(left,
                              top,
                              selector)
{
    const func = () =>
    {
        fx_style(document.documentElement, "opacity", false);
        fx_style(document.documentElement, "pointer-events", false);

        //highlight change
        if (! selector) return;
        for (const node of document.querySelectorAll(selector))
            node.classList.add("tp-edited");
    };

    if (document.URL.includes("#"))
    {
        fx_event_listen(window, "pageshow", func);
        return;
    };

    const scroll_behavior = fx_style(document.documentElement, "scroll-behavior");
    fx_style(document.documentElement, "scroll-behavior", "auto");
    fx_event_listen(window, "pageshow", () =>
    {
        fx_update_window_size();
        fx_scroll_container.scrollTo(
            fx_document_width  / 100 * left,
            fx_document_height / 100 * top);
        fx_style(document.documentElement, "scroll-behavior", scroll_behavior);
        func();
    });
};