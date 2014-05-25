/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 4 -*-  */
/*
 * biji-webkit2-editor.c
 * Copyright (C) 2014 Chunyang Xu <xuchunyang56@gmail.com>
 *
 * bijiben is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * bijiben is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <JavaScriptCore/JSStringRef.h>
#include <JavaScriptCore/JSValueRef.h>

#include "biji-webkit2-editor.h"


struct _BijiWebkit2EditorPrivate
{
  /* TODO: Add BijiNoteObj, EEditorSelection and spell_check */
  gchar *note;

  gulong content_changed;
  gulong color_changed;
  WebKitSettings* settings;
};


G_DEFINE_TYPE (BijiWebkit2Editor, biji_webkit2_editor, WEBKIT_TYPE_WEB_VIEW);

static void
biji_webkit2_editor_init (BijiWebkit2Editor *self)
{
  WebKitWebView *view = WEBKIT_WEB_VIEW (self);
  BijiWebkit2EditorPrivate *priv;

  priv = G_TYPE_INSTANCE_GET_PRIVATE (self, BIJI_TYPE_WEBKIT2_EDITOR, BijiWebkit2EditorPrivate);
  self->priv = priv;

  priv->note = "Note content";
  priv->settings = webkit_settings_new ();

}

static void
biji_webkit2_editor_constructed (GObject *obj)
{
  G_OBJECT_CLASS (biji_webkit2_editor_parent_class)->constructed (obj);

  BijiWebkit2Editor *self;
  BijiWebkit2EditorPrivate *priv;
  WebKitWebView *view;

  self = BIJI_WEBKIT2_EDITOR (obj);
  view = WEBKIT_WEB_VIEW (self);
  priv = self->priv;

  /* Settings */
  webkit_web_view_set_settings (view, priv->settings);

  webkit_web_view_load_html (view,
                             "<html><head><style>body {color:blue}</style></head>"
                             "<body contentEditable='true'>Hello, world!</body></html>",
                             NULL);
}

static void
biji_webkit2_editor_finalize (GObject *object)
{
  BijiWebkit2Editor *self = BIJI_WEBKIT2_EDITOR (object);
  BijiWebkit2EditorPrivate *priv = self->priv;

  G_OBJECT_CLASS (biji_webkit2_editor_parent_class)->finalize (object);
}

static void
biji_webkit2_editor_class_init (BijiWebkit2EditorClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (BijiWebkit2EditorPrivate));

  object_class->constructed = biji_webkit2_editor_constructed;
  object_class->finalize = biji_webkit2_editor_finalize;
}


BijiWebkit2Editor *
biji_webkit2_editor_new (void)
{
  return g_object_new (BIJI_TYPE_WEBKIT2_EDITOR, NULL);
}

void
biji_webkit2_editor_apply_format (BijiWebkit2Editor *self, gint format)
{
  BijiWebkit2EditorPrivate *priv = self->priv;
  gchar *command = NULL;
  gchar *script = NULL;

  switch (format)
  {
    case BIJI_BOLD:
      command = "bold";
      break;

    case BIJI_ITALIC:
      command = "italic";
      break;

    case BIJI_STRIKE:
      command = "strikethrough";
      break;

    case BIJI_BULLET_LIST:
      g_warning ("%s : TODO bullet list", __func__);
      break;

    case BIJI_ORDER_LIST:
      g_warning ("%s : TODO order list", __func__);
      break;

    default:
      g_warning ("%s : Invalid format", __func__);
    }

  if (command != NULL)
  {
    script = g_strdup_printf ("document.execCommand('%s', false, null)", command);
    webkit_web_view_run_javascript (WEBKIT_WEB_VIEW (self),
                                    script,
                                    NULL,
                                    NULL,
                                    NULL);
  }

  g_free(script);
}

static void
web_view_javascript_finished (GObject      *object,
                              GAsyncResult *result,
                              gpointer      user_data)
{
  WebKitJavascriptResult *js_result;
  JSValueRef              value;
  JSGlobalContextRef      context;
  GError                 *error = NULL;

  js_result = webkit_web_view_run_javascript_finish (WEBKIT_WEB_VIEW (object),
                                                     result,
                                                     &error);
  if (!js_result)
    {
      g_warning ("Error running javascript: %s", error->message);
      g_error_free (error);
      return;
    }

  context = webkit_javascript_result_get_global_context (js_result);
  value = webkit_javascript_result_get_value (js_result);
  if (JSValueIsString (context, value))
    {
      JSStringRef js_str_value;
      gchar      *str_value;
      gsize       str_length;

      js_str_value = JSValueToStringCopy (context, value, NULL);
      str_length = JSStringGetMaximumUTF8CStringSize (js_str_value);
      str_value = (gchar *)g_malloc (str_length);
      JSStringGetUTF8CString (js_str_value, str_value, str_length);
      JSStringRelease (js_str_value);
      g_print ("Script result: %s\n", str_value); /* FIXME: return str_value */
      g_free (str_value);
    }
  else
    {
      g_warning ("Error running javascript: unexpected return value");
    }

  webkit_javascript_result_unref (js_result);
}

static void
web_view_get_selected_html (WebKitWebView *web_view)
{
  gchar *script;
  script = g_strdup_printf ("function getSelectionHtml() {"
                            "    var html = '';"
                            "    if (typeof window.getSelection != 'undefined') {"
                            "        var sel = window.getSelection();"
                            "        if (sel.rangeCount) {"
                            "            var container = document.createElement('div');"
                            "            for (var i = 0, len = sel.rangeCount; i < len; ++i) {"
                            "                container.appendChild(sel.getRangeAt(i).cloneContents());"
                            "            }"
                            "            html = container.innerHTML;"
                            "        }"
                            "    } else if (typeof document.selection != 'undefined') {"
                            "        if (document.selection.type == 'Text') {"
                            "            html = document.selection.createRange().htmlText;"
                            "        }"
                            "    }"
                            "    return html;"
                            "}"
                            "getSelectionHtml()");
  webkit_web_view_run_javascript (web_view,
                                  script,
                                  NULL,
                                  web_view_javascript_finished,
                                  NULL);

  g_free (script);
}

gboolean
biji_webkit2_editor_has_selection (BijiWebkit2Editor *self)
{
  if (biji_webkit2_editor_get_selection (self) == "")
    return false;
  else
    return true;
}

gchar *
biji_webkit2_editor_get_selection (BijiWebkit2Editor *self)
{
  /* FIXME: */
  web_view_get_selected_html (WEBKIT_WEB_VIEW (self));
  return "";
}

void
biji_webkit2_editor_cut (BijiWebkit2Editor *self)
{
  webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW (self),
                                           WEBKIT_EDITING_COMMAND_CUT);
}

void
biji_webkit2_editor_copy (BijiWebkit2Editor *self)
{
  webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW (self),
                                           WEBKIT_EDITING_COMMAND_COPY);
}

void
biji_webkit2_editor_paste (BijiWebkit2Editor *self)
{
  webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW (self),
                                           WEBKIT_EDITING_COMMAND_PASTE);
}

void
biji_webkit2_editor_set_font (BijiWebkit2Editor *self, gchar *font)
{
  BijiWebkit2EditorPrivate *priv = self->priv;
  PangoFontDescription *font_desc;

  /* parse : but we only parse font properties we'll be able
   * to transfer to webkit editor
   * Maybe is there a better way than webkitSettings,
   * eg applying format to the whole body */
  font_desc = pango_font_description_from_string (font);
  const gchar * family = pango_font_description_get_family (font_desc);
  gint size = pango_font_description_get_size (font_desc) / 1000 ;

  /* Set */
  g_object_set (G_OBJECT(priv->settings),
                "default-font-family", family,
                "default-font-size", size,
                NULL);

  pango_font_description_free (font_desc);
}

