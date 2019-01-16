/* hyscan-driver-schema.c
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

/**
 * SECTION: hyscan-driver-schema
 * @Short_description: описание схемы информации о драйвере
 * @Title: HyScanDriverSchema
 *
 * Класс предназначен для автоматизации процесса создания схемы информации
 * о драйвере. Класс наследуется от #HyScanDataSchemaBuilder и создаёт в схеме
 * базовые параметры, такие как идентификатор и версию схемы.
 */

#include "hyscan-driver-schema.h"

#define HYSCAN_DRIVER_SCHEMA_ID                         6327345620348565983

enum
{
  PROP_O,
  PROP_VERSION
};

struct _HyScanDriverSchemaPrivate
{
  gint64       version;
};

static void    hyscan_driver_schema_set_property       (GObject               *object,
                                                        guint                  prop_id,
                                                        const GValue          *value,
                                                        GParamSpec            *pspec);
static void    hyscan_driver_schema_object_constructed (GObject               *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanDriverSchema, hyscan_driver_schema, HYSCAN_TYPE_DATA_SCHEMA_BUILDER)

static void
hyscan_driver_schema_class_init (HyScanDriverSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_driver_schema_set_property;
  object_class->constructed = hyscan_driver_schema_object_constructed;

  g_object_class_install_property (object_class, PROP_VERSION,
    g_param_spec_int64 ("version", "Version", "Driver info schema version", 0, G_MAXINT64, 0,
                        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_driver_schema_init (HyScanDriverSchema *schema)
{
  schema->priv = hyscan_driver_schema_get_instance_private (schema);
}

static void
hyscan_driver_schema_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  HyScanDriverSchema *schema = HYSCAN_DRIVER_SCHEMA (object);
  HyScanDriverSchemaPrivate *priv = schema->priv;

  switch (prop_id)
    {
    case PROP_VERSION:
      priv->version = g_value_get_int64 (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_driver_schema_object_constructed (GObject *object)
{
  HyScanDriverSchema *schema = HYSCAN_DRIVER_SCHEMA (object);
  HyScanDataSchemaBuilder *builder = HYSCAN_DATA_SCHEMA_BUILDER (object);

  G_OBJECT_CLASS (hyscan_driver_schema_parent_class)->constructed (object);

  hyscan_data_schema_builder_key_integer_create (builder, "/schema/id", "id",
                                                 "Device schema id", HYSCAN_DRIVER_SCHEMA_ID);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/id", HYSCAN_DATA_SCHEMA_ACCESS_READ);

  hyscan_data_schema_builder_key_integer_create (builder, "/schema/version", "version",
                                                 "Device schema version", schema->priv->version);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/version", HYSCAN_DATA_SCHEMA_ACCESS_READ);
}

/**
 * hyscan_driver_schema_new:
 * @version: версия схемы информации о драйвере
 *
 * Функция создаёт новый объект #HyScanDriverSchema.
 *
 * Returns: #HyScanDriverSchema. Для удаления #g_object_unref.
 */
HyScanDriverSchema *
hyscan_driver_schema_new (gint64 version)
{
  return g_object_new (HYSCAN_TYPE_DRIVER_SCHEMA,
                       "schema-id", "info",
                       "version", version,
                       NULL);
}

/**
 * hyscan_driver_schema_check_id:
 * @schema: указатель на #HyScanDataSchema
 *
 * Функция проверяет идентификатор и версию схемы информации о драйвере.
 *
 * Returns: %TRUE если идентификатор схемы валидный, иначе %FALSE.
 */
gboolean
hyscan_driver_schema_check_id (HyScanDataSchema *schema)
{
  gint64 id = 0;
  gint64 version = 0;

  if (!hyscan_data_schema_key_get_integer (schema, "/schema/id", NULL, NULL, &id, NULL) ||
      !hyscan_data_schema_key_get_integer (schema, "/schema/version", NULL, NULL, &version, NULL))
    {
      return FALSE;
    }

  if ((id != HYSCAN_DRIVER_SCHEMA_ID) || (version != HYSCAN_DRIVER_SCHEMA_VERSION))
    return FALSE;

  return TRUE;
}
