#ifndef __HYSCAN_DUMMY_DISCOVER_H__
#define __HYSCAN_DUMMY_DISCOVER_H__

#include <hyscan-discover.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DUMMY_DISCOVER             (hyscan_dummy_discover_get_type ())
#define HYSCAN_DUMMY_DISCOVER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DUMMY_DISCOVER, HyScanDummyDiscover))
#define HYSCAN_IS_DUMMY_DISCOVER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DUMMY_DISCOVER))
#define HYSCAN_DUMMY_DISCOVER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DUMMY_DISCOVER, HyScanDummyDiscoverClass))
#define HYSCAN_IS_DUMMY_DISCOVER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DUMMY_DISCOVER))
#define HYSCAN_DUMMY_DISCOVER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DUMMY_DISCOVER, HyScanDummyDiscoverClass))

typedef struct _HyScanDummyDiscover HyScanDummyDiscover;
typedef struct _HyScanDummyDiscoverPrivate HyScanDummyDiscoverPrivate;
typedef struct _HyScanDummyDiscoverClass HyScanDummyDiscoverClass;

struct _HyScanDummyDiscover
{
  GObject parent_instance;

  HyScanDummyDiscoverPrivate *priv;
};

struct _HyScanDummyDiscoverClass
{
  GObjectClass parent_class;
};

GType                          hyscan_dummy_discover_get_type  (void);

G_END_DECLS

#endif /* __HYSCAN_DUMMY_DISCOVER_H__ */
