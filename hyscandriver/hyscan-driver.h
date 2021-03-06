/* hyscan-driver.h
 *
 * Copyright 2016-2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * Contact the Screen LLC in this case - <info@screen-co.ru>.
 */

/* HyScanDriver имеет двойную лицензию.
 *
 * Во-первых, вы можете распространять HyScanDriver на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - <info@screen-co.ru>.
 */

#ifndef __HYSCAN_DRIVER_H__
#define __HYSCAN_DRIVER_H__

#include <hyscan-discover.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DRIVER             (hyscan_driver_get_type ())
#define HYSCAN_DRIVER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DRIVER, HyScanDriver))
#define HYSCAN_IS_DRIVER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DRIVER))
#define HYSCAN_DRIVER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DRIVER, HyScanDriverClass))
#define HYSCAN_IS_DRIVER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DRIVER))
#define HYSCAN_DRIVER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DRIVER, HyScanDriverClass))

typedef struct _HyScanDriver HyScanDriver;
typedef struct _HyScanDriverPrivate HyScanDriverPrivate;
typedef struct _HyScanDriverClass HyScanDriverClass;

struct _HyScanDriver
{
  GObject parent_instance;

  HyScanDriverPrivate *priv;
};

struct _HyScanDriverClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_driver_get_type          (void);

HYSCAN_API
HyScanDriver *         hyscan_driver_new               (const gchar           *path,
                                                        const gchar           *name);

HYSCAN_API
HyScanDataSchema *     hyscan_driver_get_info          (const gchar           *path,
                                                        const gchar           *name);

HYSCAN_API
gchar **               hyscan_driver_list              (const gchar           *path);

G_END_DECLS

#endif /* __HYSCAN_DRIVER_H__ */
