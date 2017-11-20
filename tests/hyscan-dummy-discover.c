#include "hyscan-dummy-discover.h"

static void    hyscan_dummy_discover_interface_init           (HyScanDiscoverInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HyScanDummyDiscover, hyscan_dummy_discover, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_DISCOVER, hyscan_dummy_discover_interface_init))

static void
hyscan_dummy_discover_class_init (HyScanDummyDiscoverClass *klass)
{
}

static void
hyscan_dummy_discover_init (HyScanDummyDiscover *dummy)
{
}

static void
hyscan_dummy_discover_interface_init (HyScanDiscoverInterface *iface)
{
  iface->start = NULL;
  iface->stop = NULL;
  iface->list = NULL;
  iface->config = NULL;
  iface->connect = NULL;
}
