/* hyscan-sensor-info.h
 *
 * Copyright 2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_SENSOR_INFO_H__
#define __HYSCAN_SENSOR_INFO_H__

#include <hyscan-sensor.h>
#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_SENSOR_INFO_SENSOR      (hyscan_sensor_info_sensor_get_type ())

#define HYSCAN_TYPE_SENSOR_INFO             (hyscan_sensor_info_get_type ())
#define HYSCAN_SENSOR_INFO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SENSOR_INFO, HyScanSensorInfo))
#define HYSCAN_IS_SENSOR_INFO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SENSOR_INFO))
#define HYSCAN_SENSOR_INFO_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_SENSOR_INFO, HyScanSensorInfoClass))
#define HYSCAN_IS_SENSOR_INFO_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_SENSOR_INFO))
#define HYSCAN_SENSOR_INFO_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_SENSOR_INFO, HyScanSensorInfoClass))

typedef struct _HyScanSensorInfo HyScanSensorInfo;
typedef struct _HyScanSensorInfoPrivate HyScanSensorInfoPrivate;
typedef struct _HyScanSensorInfoClass HyScanSensorInfoClass;
typedef struct _HyScanSensorInfoSensor HyScanSensorInfoSensor;

struct _HyScanSensorInfo
{
  GObject parent_instance;

  HyScanSensorInfoPrivate *priv;
};

struct _HyScanSensorInfoClass
{
  GObjectClass parent_class;
};

/**
 * HyScanSensorInfoSensor:
 * @name: название датчика
 * @dev_id: уникальный идентификатор устройства
 * @description: описание датчика
 * @position: местоположение приёмной антенны по умолчанию
 *
 * Параметры датчика.
 */
struct _HyScanSensorInfoSensor
{
  const gchar                     *name;
  const gchar                     *dev_id;
  const gchar                     *description;
  HyScanAntennaPosition           *position;
};

HYSCAN_API
GType                          hyscan_sensor_info_sensor_get_type      (void);

HYSCAN_API
GType                          hyscan_sensor_info_get_type             (void);

HYSCAN_API
HyScanSensorInfo *             hyscan_sensor_info_new                  (HyScanDataSchema              *schema);

HYSCAN_API
const gchar * const *          hyscan_sensor_info_list_sensors         (HyScanSensorInfo              *info);

HYSCAN_API
const HyScanSensorInfoSensor  *hyscan_sensor_info_get_sensor           (HyScanSensorInfo              *info,
                                                                        const gchar                   *name);

HYSCAN_API
HyScanSensorInfoSensor *       hyscan_sensor_info_sensor_copy          (const HyScanSensorInfoSensor  *info);

HYSCAN_API
void                           hyscan_sensor_info_sensor_free          (HyScanSensorInfoSensor        *info);

G_END_DECLS

#endif /* __HYSCAN_SENSOR_INFO_H__ */
