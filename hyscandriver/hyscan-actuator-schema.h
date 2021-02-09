/* hyscan-actuator-schema.h
 *
 * Copyright 2021 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_ACTUATOR_SCHEMA_H__
#define __HYSCAN_ACTUATOR_SCHEMA_H__

#include <hyscan-actuator-info.h>
#include <hyscan-device-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_ACTUATOR_SCHEMA             (hyscan_actuator_schema_get_type ())
#define HYSCAN_ACTUATOR_SCHEMA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_ACTUATOR_SCHEMA, HyScanActuatorSchema))
#define HYSCAN_IS_ACTUATOR_SCHEMA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_ACTUATOR_SCHEMA))
#define HYSCAN_ACTUATOR_SCHEMA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_ACTUATOR_SCHEMA, HyScanActuatorSchemaClass))
#define HYSCAN_IS_ACTUATOR_SCHEMA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_ACTUATOR_SCHEMA))
#define HYSCAN_ACTUATOR_SCHEMA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_ACTUATOR_SCHEMA, HyScanActuatorSchemaClass))

typedef struct _HyScanActuatorSchema HyScanActuatorSchema;
typedef struct _HyScanActuatorSchemaPrivate HyScanActuatorSchemaPrivate;
typedef struct _HyScanActuatorSchemaClass HyScanActuatorSchemaClass;

struct _HyScanActuatorSchema
{
  GObject parent_instance;

  HyScanActuatorSchemaPrivate *priv;
};

struct _HyScanActuatorSchemaClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_actuator_schema_get_type         (void);

HYSCAN_API
HyScanActuatorSchema * hyscan_actuator_schema_new              (HyScanDeviceSchema            *schema);

HYSCAN_API
gboolean               hyscan_actuator_schema_add_full         (HyScanActuatorSchema          *schema,
                                                                HyScanActuatorInfoActuator    *info);

HYSCAN_API
gboolean               hyscan_actuator_schema_add_actuator     (HyScanActuatorSchema          *schema,
                                                                const gchar                   *name,
                                                                const gchar                   *dev_id,
                                                                const gchar                   *description,
                                                                HyScanActuatorModeType         capabilities);

HYSCAN_API
gboolean               hyscan_actuator_schema_set_params       (HyScanActuatorSchema          *schema,
                                                                const gchar                   *name,
                                                                gdouble                        min_range,
                                                                gdouble                        max_range,
                                                                gdouble                        min_speed,
                                                                gdouble                        max_speed);

G_END_DECLS

#endif /* __HYSCAN_ACTUATOR_SCHEMA_H__ */
