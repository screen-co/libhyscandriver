/* hyscan-sensor-schema.h
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

#ifndef __HYSCAN_SENSOR_SCHEMA_H__
#define __HYSCAN_SENSOR_SCHEMA_H__

#include <hyscan-sensor.h>
#include <hyscan-data-schema-builder.h>

G_BEGIN_DECLS

/**
 * HYSCAN_SENSOR_SCHEMA_ID:
 *
 * Идентификатор схемы гидролокатора.
 */
#define HYSCAN_SENSOR_SCHEMA_ID               3948567498563439485

/**
 * HYSCAN_SENSOR_SCHEMA_VERSION:
 *
 * Версия схемы гидролокатора.
 */
#define HYSCAN_SENSOR_SCHEMA_VERSION          20180100

#define HYSCAN_TYPE_SENSOR_SCHEMA             (hyscan_sensor_schema_get_type ())
#define HYSCAN_SENSOR_SCHEMA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SENSOR_SCHEMA, HyScanSensorSchema))
#define HYSCAN_IS_SENSOR_SCHEMA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SENSOR_SCHEMA))
#define HYSCAN_SENSOR_SCHEMA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_SENSOR_SCHEMA, HyScanSensorSchemaClass))
#define HYSCAN_IS_SENSOR_SCHEMA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_SENSOR_SCHEMA))
#define HYSCAN_SENSOR_SCHEMA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_SENSOR_SCHEMA, HyScanSensorSchemaClass))

typedef struct _HyScanSensorSchema HyScanSensorSchema;
typedef struct _HyScanSensorSchemaPrivate HyScanSensorSchemaPrivate;
typedef struct _HyScanSensorSchemaClass HyScanSensorSchemaClass;

struct _HyScanSensorSchema
{
  HyScanDataSchemaBuilder parent_instance;

  HyScanSensorSchemaPrivate *priv;
};

struct _HyScanSensorSchemaClass
{
  HyScanDataSchemaBuilderClass parent_class;
};

HYSCAN_API
GType                  hyscan_sensor_schema_get_type           (void);

HYSCAN_API
HyScanSensorSchema    *hyscan_sensor_schema_new                (void);

HYSCAN_API
HyScanDataSchema *     hyscan_sensor_schema_get_schema         (HyScanSensorSchema    *schema);

HYSCAN_API
gboolean               hyscan_sensor_schema_add_port           (HyScanSensorSchema    *schema,
                                                                const gchar           *name);

HYSCAN_API
gboolean               hyscan_sensor_schema_set_position       (HyScanSensorSchema    *schema,
                                                                const gchar           *name,
                                                                HyScanAntennaPosition *position);

G_END_DECLS

#endif /* __HYSCAN_SENSOR_SCHEMA_H__ */
