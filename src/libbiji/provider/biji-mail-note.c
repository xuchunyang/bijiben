/*
 * biji-mail-note.c
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

/* Most code is from biji-local-note.c and biji-own-could-note.c*/

#include <uuid/uuid.h>

#include "biji-mail-note.h"
#include "biji-mail-provider.h"

#include "../serializer/biji-lazy-serializer.h"

struct _BijiMailNotePrivate
{
  BijiMailProvider* prov;
  BijiNoteID *id;  

  GFile *location;
  gchar *basename;
  gchar *html;
};


G_DEFINE_TYPE (BijiMailNote, biji_mail_note, BIJI_TYPE_NOTE_OBJ);


/* Iface */


const gchar *
mail_note_get_place (BijiItem *mail)
{
  BijiMailNote *self;
  const BijiProviderInfo *info;

  g_return_val_if_fail (BIJI_IS_MAIL_NOTE (mail), NULL);

  self = BIJI_MAIL_NOTE (mail);
  info = biji_provider_get_info (self->priv->prov);

  return info->name;
}


gchar *
mail_note_get_html (BijiNoteObj *note)
{
  if (BIJI_IS_MAIL_NOTE (note))
    return g_strdup (BIJI_MAIL_NOTE (note)->priv->html);

  else
    return NULL;
}


void
mail_note_set_html (BijiNoteObj *note,
                    gchar *html)
{
  if (BIJI_MAIL_NOTE (note)->priv->html)
    g_free (BIJI_MAIL_NOTE (note)->priv->html);

  if (html)
    BIJI_MAIL_NOTE (note)->priv->html = g_strdup (html);
}


void
mail_note_save (BijiNoteObj *note)
{
  const BijiProviderInfo *prov_info;
  BijiInfoSet *info;
  BijiItem *item;
  BijiMailNote *self;

  g_return_if_fail (BIJI_IS_MAIL_NOTE (note));

  self = BIJI_MAIL_NOTE (note);
  item = BIJI_ITEM (note);

  /* File save */
  biji_lazy_serialize (note);

  /* Tracker */
  prov_info = biji_provider_get_info (self->priv->prov);
  info = biji_info_set_new ();

  info->url = (gchar*) biji_item_get_uuid (item);
  info->title = (gchar*) biji_item_get_title (item);
  info->content = (gchar*) biji_note_obj_get_raw_text (note);
  info->mtime = biji_item_get_mtime (item);
  info->created = biji_note_obj_get_create_date (note);
  info->datasource_urn = g_strdup (prov_info->datasource);

  biji_tracker_ensure_ressource_from_info  (biji_item_get_manager (item),
                                            info);
}


/* GObj */


static void
biji_mail_note_init (BijiMailNote *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, BIJI_TYPE_MAIL_NOTE, BijiMailNotePrivate);
  self->priv->html = NULL;
}

static void
biji_mail_note_finalize (GObject *object)
{
  BijiMailNote *self;

  g_return_if_fail (BIJI_IS_MAIL_NOTE (object));

  self = BIJI_NOTE_NOTE (object);

  g_free (self->priv->html);

  G_OBJECT_CLASS (biji_mail_note_parent_class)->finalize (object);
}

static gboolean
item_no         (BijiItem * item)
{
  return FALSE;
}


static gboolean
note_no         (BijiNoteObj *item)
{
  return FALSE;
}

static gboolean
ocloud_note_delete (BijiNoteObj *note)
{
  BijiMailNote *self;

  self = BIJI_MAIL_NOTE (note);
  biji_note_delete_from_tracker (BIJI_NOTE_OBJ (self));
  return g_file_delete (self->priv->location, NULL, NULL);
}


static gchar*
mail_note_get_basename (BijiNoteObj *note)
{
  return BIJI_MAIL_NOTE (note)->priv->basename;
}

static void
biji_mail_note_class_init (BijiMailNoteClass *klass)
{
  GObjectClass     *g_object_class;
  BijiItemClass    *item_class;
  BijiNoteObjClass *note_class;

  g_object_class = G_OBJECT_CLASS (klass);
  item_class = BIJI_ITEM_CLASS (klass);
  note_class = BIJI_NOTE_OBJ_CLASS (klass);

  g_object_class->finalize = biji_mail_note_finalize;

  item_class->is_collectable = item_no;
  item_class->has_color = item_no;
  item_class->get_place = mail_note_get_place;

  note_class->get_basename = mail_note_get_basename;
  note_class->get_html = mail_note_get_html;
  note_class->set_html = mail_note_set_html;
  note_class->save_note = mail_note_save;
  note_class->can_format = note_no;
  note_class->archive = ocloud_note_delete;
  note_class->is_trashed = note_no;

  g_type_class_add_private (klass, sizeof (BijiMailNotePrivate));
}

BijiNoteObj * biji_mail_note_new_from_info (BijiProvider *prov,
                                            BijiManager *manager,
                                            BijiInfoSet *info)
{
  BijiNoteID *id;
  gchar *sane_title;
  BijiNoteObj *retval;
  BijiMailNote *mail_note;

  /* First, sanitize the title, assuming no other thread
   * mess up with the InfoSet */
  if (info->title != NULL)
  {
    sane_title = biji_str_replace (info->title, ".mail", "");
    g_free (info->title);
    info->title = sane_title;
  }


  /* Now actually create the stuff */

  id = biji_note_id_new_from_info (info);

  retval = g_object_new (BIJI_TYPE_MAIL_NOTE,
                         "manager", manager,
                         "id", id,
                         NULL);

  mail_note = BIJI_MAIL_NOTE (retval);
  mail_note->priv->id = id;
  mail_note->priv->prov = prov;
  biji_note_obj_set_create_date (retval, info->created);
  mail_note->priv->basename = g_file_get_basename (mail_note->priv->location);

  return retval;

}
