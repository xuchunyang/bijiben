/*
 * biji-mail-provider.h
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

#ifndef _BIJI_MAIL_PROVIDER_H_
#define _BIJI_MAIL_PROVIDER_H_


#include "../biji-manager.h"
#include "biji-provider.h"


G_BEGIN_DECLS


#define BIJI_TYPE_MAIL_PROVIDER             (biji_mail_provider_get_type ())
#define BIJI_MAIL_PROVIDER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), BIJI_TYPE_MAIL_PROVIDER, BijiMailProvider))
#define BIJI_MAIL_PROVIDER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), BIJI_TYPE_MAIL_PROVIDER, BijiMailProviderClass))
#define BIJI_IS_MAIL_PROVIDER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIJI_TYPE_MAIL_PROVIDER))
#define BIJI_IS_MAIL_PROVIDER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), BIJI_TYPE_MAIL_PROVIDER))
#define BIJI_MAIL_PROVIDER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), BIJI_TYPE_MAIL_PROVIDER, BijiMailProviderClass))


typedef struct _BijiMailProvider         BijiMailProvider;
typedef struct _BijiMailProviderClass    BijiMailProviderClass;
typedef struct _BijiMailProviderPrivate  BijiMailProviderPrivate;


struct _BijiMailProvider
{
    BijiProvider parent_instance;
    BijiMailProviderPrivate *priv;
};

struct _BijiMailProviderClass
{
    BijiProviderClass parent_class;
};



GType biji_mail_provider_get_type (void) G_GNUC_CONST;


BijiProvider * biji_mail_provider_new (BijiManager *manager, GFile *location);


G_END_DECLS

#endif /* _BIJI_MAIL_PROVIDER_H_ */

