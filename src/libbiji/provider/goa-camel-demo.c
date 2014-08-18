/*
 * compile:
 * gcc camel-demo.c `pkg-config --cflags --libs goa-1.0 goa-backend-1.0 camel-1.2 evolution-mail-3.0  evolution-shell-3.0  evolution-data-server-1.2`
 */

/* for camel */
#include <camel/camel.h>

/* for evolution */
#include <shell/e-shell.h>

/* for goa */
#define GOA_API_IS_SUBJECT_TO_CHANGE
#include <goa/goa.h>

void
download_mail ()
{
  CamelSession *session;

  gchar *mail_data_dir;
  gchar *mail_cache_dir;

  mail_data_dir = g_build_filename ("~/.local/share/bijiben", "mail", NULL);
  mail_cache_dir = g_build_filename ("~/.cache/bijiben", "mail", NULL);

  session = g_object_new (
    CAMEL_TYPE_SESSION,
    "online", FALSE,
    "user-data-dir", mail_data_dir,
    "user-cache-dir", mail_cache_dir,
    NULL);


  g_free (mail_data_dir);
  g_free (mail_cache_dir);

  g_object_unref (session);
}

void
deal_with_mail_service (GoaObject *goa)
{
  GoaMail * goa_mail = NULL;

    
  goa_mail = goa_object_get_mail (GOA_OBJECT (goa));
  if (! goa_mail)
  {
    g_error ("get goa_mail failed");
  }
  else
  {
    if (goa_mail_get_imap_supported (goa_mail))
    {
      gchar *tmp;
      tmp = goa_mail_dup_imap_host (goa_mail);
      g_print ("imap address: %s\n", tmp);

      tmp = goa_mail_dup_imap_user_name(goa_mail);
      g_print ("imap user name: %s\n", tmp);
      g_free (tmp);
      
      /* TODO #1 How to use goa and camel to log in email (authentication)  */
      
      /* TODO #2 How to download emails in a folder like "INBOX" */
      download_mail ();
    }

    g_object_unref (goa_mail);
  }
}

int
main (int argc, char **argv)
{
  GError *error = NULL;
  GoaClient *client;
  GList *accounts, *l;
  GoaAccount *account;

  
  client = goa_client_new_sync (NULL, &error);
  if (!client)
  {
    g_error ("Could not create GoaClient: %s", error->message);
    return 1;
  }

  accounts = goa_client_get_accounts (client);
  for (l = accounts; l != NULL; l = l->next)
  {
    GoaObject *object = GOA_OBJECT (l->data);
    account = goa_object_get_account (object);
    g_print ("%s AT %s\n",
             goa_account_get_presentation_identity (account), /* e.g. xuchuyang56@gmail.com */
             goa_account_get_provider_name (account)); /* e.g. Google */

    /* Check if account support mail and active */
    if (goa_object_peek_mail (object) &&
        ! goa_account_get_mail_disabled (account))
      deal_with_mail_service (l->data);

    g_print ("\n\n");
  }

  g_list_free_full (accounts, (GDestroyNotify) g_object_unref);

  return 0;
}
