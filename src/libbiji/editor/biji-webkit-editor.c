/*
 * biji-webkit-editor.c
 * Copyright (C) Pierre-Yves LUYTEN 2012 <py@luyten.fr>
 * Copyright (C) Chunyang Xu 2014 <xuchunyang56@gmail.com>
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

#include "../biji-string.h"
#include "../biji-manager.h"

#include "biji-webkit-editor.h"

/* Prop */
enum {
  PROP_0,
  PROP_NOTE,
  NUM_PROP
};

/* Signals */
enum {
  EDITOR_CLOSED,
  EDITOR_SIGNALS
};

static guint biji_editor_signals [EDITOR_SIGNALS] = { 0 };

static GParamSpec *properties[NUM_PROP] = { NULL, };

struct _BijiWebkitEditorPrivate
{
  BijiNoteObj *note;
  gulong color_changed;
  /* TODO: add handle content changed */
  gchar *selected_text;

  WebKitSettings* settings;
  /* TODO: add spell check */
};

G_DEFINE_TYPE (BijiWebkitEditor, biji_webkit_editor, WEBKIT_TYPE_WEB_VIEW);

static void
set_editor_color (GtkWidget *web_view, GdkRGBA *color)
{
  gchar *script;

  script = g_strdup_printf ("setBackgroundColor(%f, %f, %f, %f)",
                            color->red * 255,
                            color->green * 255,
                            color->blue * 255,
                            color->alpha);
  webkit_web_view_run_javascript (WEBKIT_WEB_VIEW (web_view),
                                  script,
                                  NULL,
                                  NULL,
                                  NULL);
  g_free (script);
}

static void
on_note_color_changed (BijiNoteObj *note, BijiWebkitEditor *self)
{
  GdkRGBA color;

  if (biji_note_obj_get_rgba(note,&color))
    set_editor_color (GTK_WIDGET (self), &color);
}

static void
on_content_changed (WebKitWebView *view)
{
  // TODO:
  g_message ("TODO: %s", __func__);
}


static void
on_note_save (WebKitWebView *web_view, gpointer user_data)
{
  // TODO:
  g_message ("TODO: %s", __func__);
}

static void
biji_webkit_editor_init (BijiWebkitEditor *self)
{
  WebKitWebView *view = WEBKIT_WEB_VIEW (self);
  BijiWebkitEditorPrivate *priv;

  priv = G_TYPE_INSTANCE_GET_PRIVATE (self, BIJI_TYPE_WEBKIT_EDITOR, BijiWebkitEditorPrivate);
  self->priv = priv;

  priv->settings = webkit_settings_new ();
  priv->selected_text = g_strdup ("");
}

static void
biji_webkit_editor_finalize (GObject *object)
{
  BijiWebkitEditor *self = BIJI_WEBKIT_EDITOR (object);
  BijiWebkitEditorPrivate *priv = self->priv;

  G_OBJECT_CLASS (biji_webkit_editor_parent_class)->finalize (object);
}

static void
biji_webkit_editor_constructed (GObject *obj)
{
  G_OBJECT_CLASS (biji_webkit_editor_parent_class)->constructed (obj);

  BijiWebkitEditor *self;
  BijiWebkitEditorPrivate *priv;
  WebKitWebView *view;
  gchar *html_path;
  gchar *html;
  GdkRGBA color;

  self = BIJI_WEBKIT_EDITOR (obj);
  view = WEBKIT_WEB_VIEW (self);
  priv = self->priv;

  /* Do not segfault at finalize
   * if the note died */
  g_object_add_weak_pointer (G_OBJECT (priv->note), (gpointer*) &priv->note);

  html = biji_note_obj_get_html (priv->note);

  if (!html)
    html = html_from_plain_text ("");

  /* Settings */
  webkit_web_view_set_settings (view, priv->settings);

  webkit_web_view_load_html (view, html, NULL);

  /* FIXME: cannot apply color before html is loaded */
  if (biji_note_obj_get_rgba (priv->note, &color))
    set_editor_color (GTK_WIDGET (self), &color);

  priv->color_changed = g_signal_connect (priv->note,
                                          "color-changed",
                                          G_CALLBACK (on_note_color_changed),
                                          self);

  /* FIXME: "Save" in webkit1:
   * update title & save note's meta (mtime, content, etc)
   * but the content changed signal in webkit1 has been remove now
   */
  g_signal_connect (view, "close", G_CALLBACK (on_note_save), NULL);
}

static void
biji_webkit_editor_get_property (GObject  *object,
                                 guint     property_id,
                                 GValue   *value,
                                 GParamSpec *pspec)
{
  BijiWebkitEditor *self = BIJI_WEBKIT_EDITOR (object);

  switch (property_id)
    {
    case PROP_NOTE:
      g_value_set_object (value, self->priv->note);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
biji_webkit_editor_set_property (GObject  *object,
                                 guint     property_id,
                                 const GValue *value,
                                 GParamSpec *pspec)
{
  BijiWebkitEditor *self = BIJI_WEBKIT_EDITOR (object);

  switch (property_id)
    {
    case PROP_NOTE:
      self->priv->note = g_value_get_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
biji_webkit_editor_class_init (BijiWebkitEditorClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = biji_webkit_editor_constructed;
  object_class->finalize = biji_webkit_editor_finalize;
  object_class->get_property = biji_webkit_editor_get_property;
  object_class->set_property = biji_webkit_editor_set_property;

  properties[PROP_NOTE] = g_param_spec_object ("note",
                                               "Note",
                                               "Biji Note Obj",
                                               BIJI_TYPE_NOTE_OBJ,
                                               G_PARAM_READWRITE  |
                                               G_PARAM_CONSTRUCT |
                                               G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class,PROP_NOTE,properties[PROP_NOTE]);

  biji_editor_signals[EDITOR_CLOSED] = g_signal_new ("closed",
                                                     G_OBJECT_CLASS_TYPE (klass),
                                                     G_SIGNAL_RUN_FIRST,
                                                     0,
                                                     NULL,
                                                     NULL,
                                                     g_cclosure_marshal_VOID__VOID,
                                                     G_TYPE_NONE,
                                                     0);

  g_type_class_add_private (klass, sizeof (BijiWebkitEditorPrivate));

}


BijiWebkitEditor *
biji_webkit_editor_new (BijiNoteObj *note)
{
  return g_object_new (BIJI_TYPE_WEBKIT_EDITOR,
                       "note", note,
                       NULL);
}

void
biji_webkit_editor_apply_format_new (BijiWebkitEditor *self, gchar *format)
{
  gchar *script;

  script = g_strdup_printf ("document.execCommand('%s', false, false)", format);
  webkit_web_view_run_javascript (WEBKIT_WEB_VIEW (self),
                                  script,
                                  NULL,
                                  NULL,
                                  NULL);
  g_free (script);
}

void
biji_webkit_editor_apply_format (BijiWebkitEditor *self, gint format)
{
  BijiWebkitEditorPrivate *priv = self->priv;
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
  BijiWebkitEditor *self = BIJI_WEBKIT_EDITOR (user_data);
  BijiWebkitEditorPrivate *priv = self->priv;

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
      priv->selected_text = g_strdup (str_value);
      g_print ("Script result: %s\n", str_value); /* FIXME: return str_value */
      g_free (str_value);
    }
  else
    {
      g_warning ("Error running javascript: unexpected return value");
    }
  g_message ("%s", __func__);
  webkit_javascript_result_unref (js_result);
}

static void
web_view_get_selected_html (BijiWebkitEditor *self)
{
  webkit_web_view_run_javascript (WEBKIT_WEB_VIEW (self),
                                  "getSelectionHtml()",
                                  NULL,
                                  web_view_javascript_finished,
                                  self);
  g_message ("%s", __func__);
}

gboolean
biji_webkit_editor_has_selection (BijiWebkitEditor *self)
{
  /* TODO */
  web_view_get_selected_html (self);
  g_message ("TODO: %s", __func__);
  return false;
}

gchar *
biji_webkit_editor_get_selection (BijiWebkitEditor *self)
{
  /* TODO */
  g_message ("TODO: %s", __func__);
  web_view_get_selected_html (WEBKIT_WEB_VIEW (self));
  return NULL;
}

void
biji_webkit_editor_cut (BijiWebkitEditor *self)
{
  webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW (self),
                                           WEBKIT_EDITING_COMMAND_CUT);
}

void
biji_webkit_editor_copy (BijiWebkitEditor *self)
{
  webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW (self),
                                           WEBKIT_EDITING_COMMAND_COPY);
}

void
biji_webkit_editor_paste (BijiWebkitEditor *self)
{
  webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW (self),
                                           WEBKIT_EDITING_COMMAND_PASTE);
}

void
biji_webkit_editor_set_font (BijiWebkitEditor *self, gchar *font)
{
  BijiWebkitEditorPrivate *priv = self->priv;
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
