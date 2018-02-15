/* hyscan-dummy-discover.h
 *
 * Copyright 2016-2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
 *
 * This file is part of HyScanDriver library.
 *
 * HyScanDriver is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HyScanDriver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this code under a commercial license.
 * Contact the Screen LLC in this case - info@screen-co.ru
 */

/* HyScanDriver имеет двойную лицензию.
 *
 * Во первых, вы можете распространять HyScanDriver на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
 */

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

HYSCAN_API
GType                          hyscan_dummy_discover_get_type  (void);

HYSCAN_API
HyScanDummyDiscover *          hyscan_dummy_discover_new       (void);

G_END_DECLS

#endif /* __HYSCAN_DUMMY_DISCOVER_H__ */
