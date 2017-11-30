/* hyscan-discover.h
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

#ifndef __HYSCAN_DISCOVER_H__
#define __HYSCAN_DISCOVER_H__

#include <hyscan-param-list.h>
#include <hyscan-data-schema.h>

G_BEGIN_DECLS

/**
 * HYSCAN_DISCOVER_API:
 *
 * Версия API интерфейса обнаружения устройств.
 */
#define HYSCAN_DISCOVER_API    20170100

#define HYSCAN_TYPE_DISCOVER            (hyscan_discover_get_type ())
#define HYSCAN_DISCOVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DISCOVER, HyScanDiscover))
#define HYSCAN_IS_DISCOVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DISCOVER))
#define HYSCAN_DISCOVER_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_DISCOVER, HyScanDiscoverInterface))

typedef struct _HyScanDiscover HyScanDiscover;
typedef struct _HyScanDiscoverInterface HyScanDiscoverInterface;
typedef struct _HyScanDiscoverInfo HyScanDiscoverInfo;

/**
 * HyScanDiscoverConnect:
 *
 * Абстрактный тип, описывающий подключение к устройству.
 *
 */
typedef GObject HyScanDiscoverConnect;

/**
 * HyScanDiscoverInfo:
 * @info: Краткая информация об устройстве
 * @uri: Путь для подключения к устройству
 *
 * Структура содержит общую информацию об устройстве.
 */
struct _HyScanDiscoverInfo
{
  const gchar                 *info;
  const gchar                 *uri;
};

/**
 * HyScanDiscoverInterface:
 * @g_iface: Базовый интерфейс.
 * @start: Функция инициирует процесс обнаружения устройств.
 * @stop: Функция принудительно останавливает процесс обнаружения устройств.
 * @list: Функция возвращает список обнаруженных устройств с общей информацией о них.
 * @config: Функция возвращает схему с параметрами драйвера устройства.
 * @connect: Функция производит подключение к устройству.
 */
struct _HyScanDiscoverInterface
{
  GTypeInterface               g_iface;

  void                         (*start)                        (HyScanDiscover                *discover);

  void                         (*stop)                         (HyScanDiscover                *discover);

  GList *                      (*list)                         (HyScanDiscover                *discover);

  HyScanDataSchema *           (*config)                       (HyScanDiscover                *discover,
                                                                const gchar                   *uri);

  HyScanDiscoverConnect *      (*connect)                      (HyScanDiscover                *discover,
                                                                const gchar                   *uri,
                                                                HyScanParamList               *params);
};

HYSCAN_API
GType                          hyscan_discover_get_type        (void);

HYSCAN_API
void                           hyscan_discover_start           (HyScanDiscover                *discover);

HYSCAN_API
void                           hyscan_discover_stop            (HyScanDiscover                *discover);

HYSCAN_API
GList *                        hyscan_discover_list            (HyScanDiscover                *discover);

HYSCAN_API
HyScanDataSchema *             hyscan_discover_config          (HyScanDiscover                *discover,
                                                                const gchar                   *uri);

HYSCAN_API
HyScanDiscoverConnect *        hyscan_discover_connect         (HyScanDiscover                *discover,
                                                                const gchar                   *uri,
                                                                HyScanParamList               *params);

HYSCAN_API
HyScanDiscoverInfo *           hyscan_discover_info_new        (const gchar                   *info,
                                                                const gchar                   *uri);

HYSCAN_API
HyScanDiscoverInfo *           hyscan_discover_info_copy       (HyScanDiscoverInfo            *info);

HYSCAN_API
void                           hyscan_discover_info_free       (HyScanDiscoverInfo            *info);

G_END_DECLS

#endif /* __HYSCAN_DISCOVER_H__ */
