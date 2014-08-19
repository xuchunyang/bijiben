/*
 * biji-mail-provider.c
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

/* Note: most code is from biji-local-provider.c */

#include <uuid/uuid.h>

#include "biji-mail-note.h"

#include "biji-mail-provider.h"

struct _BijiMailProviderPrivate
{
  BijiProviderInfo info;

  GFile * location;
  GHashTable *items;

  BijiProviderHelper *living_helper;
};



G_DEFINE_TYPE (BijiMailProvider, biji_mail_provider, BIJI_TYPE_PROVIDER);


/* Properties */
enum {
  PROP_0,
  PROP_LOCATION, /* FIXME 定制自己的参数，这里参考是 local-provider，它只需要一个参数，用来初始化，就是 path */
  BIJI_MAIL_PROP
};



static GParamSpec *properties[BIJI_MAIL_PROP] = { NULL, };

static void
load_from_location (BijiProviderHelper *helper)
{
  GFile *file;
  gchar *path;
  BijiMailProvider *self;
  
  self = BIJI_MAIL_PROVIDER (helper->provider);

  if (helper->group == BIJI_LIVING_ITEMS)
  {
    file = self->priv->location;
  }
  else /* BIJI_ARCHIVED_ITEMS */
    g_error ("no archives here");

  /* TODO Read content of files in a directory as note */
  path = g_file_get_path (file);
  g_message ("todo: read %s content as note", path);
}


static void
biji_mail_provider_constructed (GObject *object)
{
  BijiMailProvider *self;

  g_return_if_fail (BIJI_IS_MAIL_PROVIDER (object));
  self = BIJI_MAIL_PROVIDER (object);


  /* TODO */
  load_from_location (self->priv->living_helper);
}



static void
biji_mail_provider_init (BijiMailProvider *self)
{
  BijiMailProviderPrivate *priv;

  priv = self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, BIJI_TYPE_MAIL_PROVIDER, BijiMailProviderPrivate);
  priv->items = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, NULL);
  /* TODO: Add initialization code here */

  /* Info */
  priv->info.unique_id = "xcy-local";
  priv->info.datasource = g_strdup_printf ("xcy-local:%s",
                                           priv->info.unique_id);
  priv->info.name = "Xcy Local storage";
  priv->info.icon =
    gtk_image_new_from_icon_name ("user-home", GTK_ICON_SIZE_INVALID);
  gtk_image_set_pixel_size (GTK_IMAGE (priv->info.icon), 48);
  g_object_ref (priv->info.icon);


  /* Helpers */
  priv->living_helper = biji_provider_helper_new (BIJI_PROVIDER (self),
                                                  BIJI_LIVING_ITEMS);

}

static void
biji_mail_provider_finalize (GObject *object)
{
  BijiMailProvider *self;

  g_return_if_fail (BIJI_IS_MAIL_PROVIDER (object));

  self = BIJI_MAIL_PROVIDER (object);

  g_object_unref (self->priv->location);

  biji_provider_helper_free (self->priv->living_helper);
  
  G_OBJECT_CLASS (biji_mail_provider_parent_class)->finalize (object);
}

static void
biji_mail_provider_set_property (GObject *object,
                                 guint property_id,
                                 const GValue *value,
                                 GParamSpec *pspec)
{
  BijiMailProvider *self = BIJI_MAIL_PROVIDER (object);


  switch (property_id)
  {
  case PROP_LOCATION:
    self->priv->location = g_value_dup_object (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
biji_mail_provider_get_property (GObject *object,
                                 guint property_id,
                                 GValue *value,
                                 GParamSpec *pspec)
{
  BijiMailProvider *self = BIJI_MAIL_PROVIDER (object);
  
  switch (property_id)
  {
  case PROP_LOCATION:
    g_value_set_object (value, self->priv->location);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

const BijiProviderInfo *
mail_provider_get_info (BijiProvider *provider)
{
  BijiMailProvider *self;

  self = BIJI_MAIL_PROVIDER (provider);
  return &(self->priv->info);
}

/* UUID skeleton for new notes */
static gchar *
_get_uuid (void)
{
  uuid_t unique;
  char out[40];

  uuid_generate (unique);
  uuid_unparse_lower (unique, out);
  return g_strdup_printf ("%s.note", out);
}


static BijiNoteObj *
_get_note_skeleton (BijiMailProvider *self)
{
  BijiNoteObj *ret = NULL;
  BijiManager *manager;
  gchar * folder, * name, *path;
  BijiInfoSet info_set;

  manager = biji_provider_get_manager (BIJI_PROVIDER (self));
  info_set.title = NULL;
  info_set.content = NULL;
  info_set.mtime = 0;
  folder = g_file_get_path (self->priv->location);

  while (! ret)                 /* TODO do not know why */
  {
    name = _get_uuid ();
    path = g_build_filename (folder, name, NULL);
    g_free (name);
    info_set.url = path;

    if (!biji_manager_get_item_at_path (manager, path))
      ret = biji_local_note_new_from_info (BIJI_PROVIDER (self), manager, &info_set); /* TODO: we need mail note here */

    g_free (path);
  }

  biji_note_obj_set_all_dates_now (ret);
  return ret;
}

static BijiNoteObj *
mail_provider_create_new_note (BijiProvider *self,
                               gchar *str)
{
  BijiNoteObj *ret = _get_note_skeleton (BIJI_MAIL_PROVIDER (self));
  BijiManager *manager;

  manager = biji_provider_get_manager (self);

  if (str)
  {
    gchar *unique, *html;

    unique = biji_manager_get_unique_title (manager, str);
    html = html_from_plain_text (str);

    biji_note_obj_set_title (ret, unique);
    biji_note_obj_set_raw_text (ret, str);
    biji_note_obj_set_html (ret, html);

    g_free (unique);
    g_free (html);

    /* Only save note if content */
    biji_note_obj_save_note (ret);
  }

  return ret;
}

static void
mail_provider_create_note_full (BijiProvider *provider)
{
  return;
}
                                
static void
mail_provider_load_archives (BijiProvider *provider)
{
  return;
}

static void
biji_mail_provider_class_init (BijiMailProviderClass *klass)
{
  GObjectClass* object_class;
  BijiProviderClass *provider_class;

  object_class = G_OBJECT_CLASS (klass);
  provider_class = BIJI_PROVIDER_CLASS (klass);
  
  object_class = G_OBJECT_CLASS (klass);
  provider_class = BIJI_PROVIDER_CLASS (klass);

  object_class->constructed = biji_mail_provider_constructed;
  object_class->finalize = biji_mail_provider_finalize;
  object_class->get_property = biji_mail_provider_get_property;
  object_class->set_property = biji_mail_provider_set_property;

  provider_class->get_info = mail_provider_get_info;
  provider_class->create_new_note = mail_provider_create_new_note;
  provider_class->create_note_full = mail_provider_create_note_full;
  provider_class->load_archives = mail_provider_load_archives;
  
  
  properties[PROP_LOCATION] =
    g_param_spec_object ("location",
                         "The manager location",
                         "The location where the notes are loaded and saved",
                         G_TYPE_FILE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, BIJI_MAIL_PROP, properties);

  
  g_type_class_add_private ((gpointer) klass, sizeof (BijiMailProviderPrivate));


}


BijiProvider *
biji_mail_provider_new (BijiManager *manager, GFile *location)
{
    return g_object_new (BIJI_TYPE_MAIL_PROVIDER,
                         "manager", manager,
                         "location", location,
                         NULL);
}
