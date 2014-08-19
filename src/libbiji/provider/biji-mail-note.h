/*
 * biji-mail-note.h
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


#ifndef _BIJI_MAIL_NOTE_H_
#define _BIJI_MAIL_NOTE_H_ 1

#include "biji-note-id.h"
#include "biji-note-obj.h"

G_BEGIN_DECLS


#define BIJI_TYPE_MAIL_NOTE             (biji_mail_note_get_type ())
#define BIJI_MAIL_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BIJI_TYPE_MAIL_NOTE, BijiMailNote))
#define BIJI_MAIL_NOTE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BIJI_TYPE_MAIL_NOTE, BijiMailNoteClass))
#define BIJI_IS_MAIL_NOTE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIJI_TYPE_MAIL_NOTE))
#define BIJI_IS_MAIL_NOTE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BIJI_TYPE_MAIL_NOTE))
#define BIJI_MAIL_NOTE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BIJI_TYPE_MAIL_NOTE, BijiMailNoteClass))

typedef struct _BijiMailNoteClass BijiMailNoteClass;
typedef struct _BijiMailNote BijiMailNote;
typedef struct _BijiMailNotePrivate BijiMailNotePrivate;

struct _BijiMailNote
{
  BijiNoteObj parent_instance;
  BijiMailNotePrivate *priv;
};

struct _BijiMailNoteClass
{
  BijiNoteObjClass parent_class;
};


GType biji_mail_note_get_type (void) G_GNUC_CONST;


BijiNoteObj * biji_mail_note_new_from_info (BijiProvider *provider,
                                            BijiManager *manager,
                                            BijiInfoSet *set);


G_END_DECLS

#endif /* _BIJI_MAIL_NOTE_H_ */

