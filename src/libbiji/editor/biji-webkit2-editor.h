/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 4 -*-  */
/*
 * biji-webkit2-editor.h
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

#ifndef _BIJI_WEBKIT2_EDITOR_H_
#define _BIJI_WEBKIT2_EDITOR_H_

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

G_BEGIN_DECLS

/* FIXME: use note-obj as private variable */
typedef enum
{
  BIJI_NO_FORMAT,
  BIJI_BOLD,
  BIJI_ITALIC,
  BIJI_STRIKE,
  BIJI_BULLET_LIST,
  BIJI_ORDER_LIST
} BijiEditorFormat;

#define BIJI_TYPE_WEBKIT2_EDITOR             (biji_webkit2_editor_get_type ())
#define BIJI_WEBKIT2_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BIJI_TYPE_WEBKIT2_EDITOR, BijiWebkit2Editor))
#define BIJI_WEBKIT2_EDITOR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BIJI_TYPE_WEBKIT2_EDITOR, BijiWebkit2EditorClass))
#define BIJI_IS_WEBKIT2_EDITOR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIJI_TYPE_WEBKIT2_EDITOR))
#define BIJI_IS_WEBKIT2_EDITOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BIJI_TYPE_WEBKIT2_EDITOR))
#define BIJI_WEBKIT2_EDITOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BIJI_TYPE_WEBKIT2_EDITOR, BijiWebkit2EditorClass))

typedef struct _BijiWebkit2EditorClass BijiWebkit2EditorClass;
typedef struct _BijiWebkit2Editor BijiWebkit2Editor;
typedef struct _BijiWebkit2EditorPrivate BijiWebkit2EditorPrivate;


struct _BijiWebkit2EditorClass
{
  WebKitWebViewClass parent_class;
};

struct _BijiWebkit2Editor
{
  WebKitWebView parent_instance;

  BijiWebkit2EditorPrivate *priv;
};

GType biji_webkit2_editor_get_type (void) G_GNUC_CONST;

BijiWebkit2Editor * biji_webkit2_editor_new (void); // FIXME: add argument

void biji_webkit2_editor_apply_format (BijiWebkit2Editor *self, gint format);
gboolean biji_webkit2_editor_has_selection (BijiWebkit2Editor *self);
gchar * biji_webkit2_editor_get_selection (BijiWebkit2Editor *self);
void biji_webkit2_editor_cut (BijiWebkit2Editor *self);
void biji_webkit2_editor_copy (BijiWebkit2Editor *self);
void biji_webkit2_editor_paste (BijiWebkit2Editor *self);
void biji_webkit2_editor_set_font (BijiWebkit2Editor *self, gchar *font);

G_END_DECLS

#endif /* _BIJI_WEBKIT2_EDITOR_H_ */

