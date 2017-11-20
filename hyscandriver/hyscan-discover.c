#include "hyscan-discover.h"

G_DEFINE_INTERFACE (HyScanDiscover, hyscan_discover, G_TYPE_OBJECT)

static void
hyscan_discover_default_init (HyScanDiscoverInterface *iface)
{
  g_signal_new ("discover-progress", HYSCAN_TYPE_DISCOVER, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                g_cclosure_marshal_VOID__DOUBLE,
                G_TYPE_NONE, 1, G_TYPE_DOUBLE);

  g_signal_new ("discover-completed", HYSCAN_TYPE_DISCOVER, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

void
hyscan_discover_start (HyScanDiscover *discover)
{
  HyScanDiscoverInterface *iface;

  g_return_if_fail (HYSCAN_IS_DISCOVER (discover));

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->start != NULL)
    (* iface->start) (discover);
}

void
hyscan_discover_stop (HyScanDiscover *discover)
{
  HyScanDiscoverInterface *iface;

  g_return_if_fail (HYSCAN_IS_DISCOVER (discover));

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->stop != NULL)
    (* iface->stop) (discover);
}

HyScanDiscoverInfo **
hyscan_discover_list (HyScanDiscover *discover,
                      guint32        *n_devices)
{
  HyScanDiscoverInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DISCOVER (discover), NULL);

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->list != NULL)
    (* iface->list) (discover, n_devices);

  return NULL;
}

HyScanDataSchema *
hyscan_discover_config (HyScanDiscover *discover,
                        const gchar    *uri)
{
  HyScanDiscoverInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DISCOVER (discover), NULL);

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->config != NULL)
    (* iface->config) (discover, uri);

  return NULL;
}

HyScanDiscoverConnect *
hyscan_discover_connect (HyScanDiscover  *discover,
                         const gchar     *uri,
                         HyScanParamList *params)
{
  HyScanDiscoverInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DISCOVER (discover), NULL);

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->connect != NULL)
    (* iface->connect) (discover, uri, params);

  return NULL;
}

void
hyscan_discover_free_info (HyScanDiscoverInfo **list)
{
  guint i;

  for (i = 0; list != NULL && list[i] != NULL; i++)
    {
      g_free ((gchar*)list[i]->model);
      g_free ((gchar*)list[i]->uri);
      g_free (list[i]);
    }

  g_free (list);
}
