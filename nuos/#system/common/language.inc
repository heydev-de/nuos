<?php /*

   NUOS Web Platform
   Copyright © 2026–present Patrick Heyer
   https://nuos-web.com

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function l($text)
{
    return language_get($text);
};

//--------------------------------------------------------------------------------------------------

function language_get($text,
                      $language = NULL,
                      $explicit = NULL)
{
    $i       = strcspn($text, CMS_LANGUAGE_SEPARATOR);
    $default = substr($text, 0, $i);

    if (! $language)
    {
        if (($language === FALSE) || $explicit) return $default;
        $language = CMS_LANGUAGE;
    };

    if (($i = strpos($text, CMS_LANGUAGE_SEPARATOR . "$language:", $i)) === FALSE)
        return $explicit ? NULL : $default;

    $i += strlen($language) + 2;

    if (($_i = strpos($text, CMS_LANGUAGE_SEPARATOR, $i)) === FALSE)
        $buffer = substr($text, $i);
    else
        $buffer = substr($text, $i, $_i - $i);

    return ($buffer || $explicit) ? $buffer : $default;
};

//--------------------------------------------------------------------------------------------------

function language_get_array($text)
{
    $array = explode(CMS_LANGUAGE_SEPARATOR, $text);

    //initialize return array
    $return = ["" => $array[0]];

    if (CMS_LANGUAGE_ENABLED)
    {
        $language = explode(",", CMS_LANGUAGE_ENABLED);
        foreach ($language AS $key => $value)
            $return[$value] = NULL;
    };

    //fill return array
    $c = count($array);

    for ($i = 1; $i < $c; $i++)
    {
        $text = $array[$i];

        if (($_i = strpos($text, ":")) !== FALSE)
        {
            $key  = substr($text, 0, $_i);
            $text = substr($text, $_i + 1);
            $return[$key] = $text;
        };
    };

    return $return;
};

//--------------------------------------------------------------------------------------------------

function language_set($text,
                      $value    = NULL,
                      $language = NULL)
{
    $i       = strcspn($text, CMS_LANGUAGE_SEPARATOR);
    $default = substr($text, 0, $i);

    if (! $language) return substr_replace($text, $value, 0, $i);

    if ($value)
        $value = CMS_LANGUAGE_SEPARATOR . "$language:$value";

    if (($i = strpos($text, CMS_LANGUAGE_SEPARATOR . "$language:", $i)) === FALSE)
        return $text . $value;

    $c = strlen($language) + 2;

    if (($_i = strpos($text, CMS_LANGUAGE_SEPARATOR, $i + $c)) === FALSE)
        return substr_replace($text, $value, $i);

    return substr_replace($text, $value, $i, $_i - $i);
};

//--------------------------------------------------------------------------------------------------

function language_set_array($array)
{
    $return = [];

    foreach ($array AS $key => $value)
    {
        if (stre($value)) continue;
        if (stre($key))   $return[] = $value . $return;
        else              $return[] = CMS_LANGUAGE_SEPARATOR . "$key:$value";
    };

    return implode("", $return);
};

//--------------------------------------------------------------------------------------------------

function language_detect($text)
{
    $data     = new data("#system/language");
    $text     = tokenize_text(utf8_strtolower($text));
    $max      = 0;
    $language = "";

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $stopword = utf8_strtolower($data->get($key, "stopword"));
        $stopword = preg_split("/\s/", $stopword, -1, PREG_SPLIT_NO_EMPTY);
        $match    = count(array_intersect($text, $stopword));

        if ($match > $max)
        {
            $max      = $match;
            $language = $key;
        };
    };

    return $language;
};

//--------------------------------------------------------------------------------------------------

function language_strip_stopword($text,
                                 $language)
{
    $data     = new data("#system/language");
    $stopword = $data->get($language, "stopword");
    $stopword = preg_split("/\s/", $stopword, -1, PREG_SPLIT_NO_EMPTY);
    $stopword = array_map(function($value) { return preg_quote($value, "/"); }, $stopword);

    return preg_replace(
        "/(^|" . CMS_REGEX_BORDER . ")" .
        "(?:" . implode("|", $stopword) . ")" .
        "(?=" . CMS_REGEX_BORDER . "|$)/iu",
        "\\1", $text);;
};

//--------------------------------------------------------------------------------------------------

function language_name($string)
{
    return (new data("#system/language"))->get($string, "name");
};

?>