/* hyscan-driver-schema.h
 *
 * Copyright 2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_DRIVER_SCHEMA_H__
#define __HYSCAN_DRIVER_SCHEMA_H__

#include <hyscan-data-schema-builder.h>

G_BEGIN_DECLS

/**
 * HYSCAN_DRIVER_SCHEMA_VERSION:
 *
 * Версия схемы информации о драйвере.
 */
#define HYSCAN_DRIVER_SCHEMA_VERSION          20190100

#define HYSCAN_TYPE_DRIVER_SCHEMA             (hyscan_driver_schema_get_type ())
#define HYSCAN_DRIVER_SCHEMA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DRIVER_SCHEMA, HyScanDriverSchema))
#define HYSCAN_IS_DRIVER_SCHEMA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DRIVER_SCHEMA))
#define HYSCAN_DRIVER_SCHEMA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DRIVER_SCHEMA, HyScanDriverSchemaClass))
#define HYSCAN_IS_DRIVER_SCHEMA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DRIVER_SCHEMA))
#define HYSCAN_DRIVER_SCHEMA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DRIVER_SCHEMA, HyScanDriverSchemaClass))

typedef struct _HyScanDriverSchema HyScanDriverSchema;
typedef struct _HyScanDriverSchemaPrivate HyScanDriverSchemaPrivate;
typedef struct _HyScanDriverSchemaClass HyScanDriverSchemaClass;

struct _HyScanDriverSchema
{
  HyScanDataSchemaBuilder parent_instance;

  HyScanDriverSchemaPrivate *priv;
};

struct _HyScanDriverSchemaClass
{
  HyScanDataSchemaBuilderClass parent_class;
};

HYSCAN_API
GType                  hyscan_driver_schema_get_type   (void);

HYSCAN_API
HyScanDriverSchema *   hyscan_driver_schema_new        (gint64                 version);

HYSCAN_API
gboolean               hyscan_driver_schema_check_id   (HyScanDataSchema      *schema);

G_END_DECLS

#endif /* __HYSCAN_DRIVER_SCHEMA_H__ */
