/* hyscan-sensor.h
 *
 * Copyright 2016-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_SENSOR_H__
#define __HYSCAN_SENSOR_H__

#include <glib-object.h>
#include <hyscan-types.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_SENSOR            (hyscan_sensor_get_type ())
#define HYSCAN_SENSOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SENSOR, HyScanSensor))
#define HYSCAN_IS_SENSOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SENSOR))
#define HYSCAN_SENSOR_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_SENSOR, HyScanSensorInterface))

typedef struct _HyScanSensor HyScanSensor;
typedef struct _HyScanSensorInterface HyScanSensorInterface;

/**
 * HyScanSensorInterface:
 * @g_iface: Базовый интерфейс.
 * @set_sound_velocity: Функция задаёт таблицу профиля скорости звука.
 * @set_enable: Функция включает или выключает приём данных.
 * @disconnect: Функция выполняет отключение от датчика.
 */
struct _HyScanSensorInterface
{
  GTypeInterface       g_iface;

  gboolean             (*set_sound_velocity)                   (HyScanSensor                  *sensor,
                                                                GList                         *svp);

  gboolean             (*set_enable)                           (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                gboolean                       enable);

  gboolean             (*disconnect)                           (HyScanSensor                  *sensor);
};

HYSCAN_API
GType                  hyscan_sensor_get_type                  (void);

HYSCAN_API
gboolean               hyscan_sensor_set_sound_velocity        (HyScanSensor                  *sensor,
                                                                GList                         *svp);

HYSCAN_API
gboolean               hyscan_sensor_set_enable                (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                gboolean                       enable);

HYSCAN_API
gboolean               hyscan_sensor_disconnect                (HyScanSensor                  *sensor);

G_END_DECLS

#endif /* __HYSCAN_SENSOR_H__ */
