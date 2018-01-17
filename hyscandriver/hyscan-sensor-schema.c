/* hyscan-sensor-schema.c
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

/**
 * SECTION: hyscan-sensor-schema
 * @Short_description: класс создания схемы датчиков
 * @Title: HyScanSensorSchema
 *
 * Класс предназначен для автоматизации процесса создания схемы данных
 * датчиков. Он содержит функции которые определяют наличие и
 * параметры датчиков.
 */

#include "hyscan-sensor-schema.h"

struct _HyScanSensorSchemaPrivate
{
  GHashTable                  *ports;                        /* Наличие портов датчиков. */
};

static void    hyscan_sensor_schema_object_constructed       (GObject               *object);
static void    hyscan_sensor_schema_object_finalize          (GObject               *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSensorSchema, hyscan_sensor_schema, HYSCAN_TYPE_DATA_SCHEMA_BUILDER)

static void
hyscan_sensor_schema_class_init (HyScanSensorSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = hyscan_sensor_schema_object_constructed;
  object_class->finalize = hyscan_sensor_schema_object_finalize;
}

static void
hyscan_sensor_schema_init (HyScanSensorSchema *schema)
{
  schema->priv = hyscan_sensor_schema_get_instance_private (schema);
}

static void
hyscan_sensor_schema_object_constructed (GObject *object)
{
  HyScanSensorSchema *schema = HYSCAN_SENSOR_SCHEMA (object);
  HyScanDataSchemaBuilder *builder = HYSCAN_DATA_SCHEMA_BUILDER (object);
  HyScanSensorSchemaPrivate *priv = schema->priv;

  G_OBJECT_CLASS (hyscan_sensor_schema_parent_class)->constructed (object);

  priv->ports = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

  /* Версия и идентификатор схемы данных датчиков. */
  hyscan_data_schema_builder_key_integer_create (builder, "/schema/id", "id",
                                                 "Sensor schema id", HYSCAN_SENSOR_SCHEMA_ID);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/id", HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

  hyscan_data_schema_builder_key_integer_create (builder, "/schema/version", "version",
                                                 "Sensor schema version", HYSCAN_SENSOR_SCHEMA_VERSION);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/version", HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
}

static void
hyscan_sensor_schema_object_finalize (GObject *object)
{
  HyScanSensorSchema *schema = HYSCAN_SENSOR_SCHEMA (object);
  HyScanSensorSchemaPrivate *priv = schema->priv;

  g_hash_table_unref (priv->ports);

  G_OBJECT_CLASS (hyscan_sensor_schema_parent_class)->finalize (object);
}

/**
 * hyscan_sensor_schema_new:
 *
 * Функция создаёт новый объект HyScanSensorSchema.
 *
 * Returns: #HyScanSensorSchema. Для удаления #g_object_unref.
 */
HyScanSensorSchema *
hyscan_sensor_schema_new (void)
{
  return g_object_new (HYSCAN_TYPE_SENSOR_SCHEMA,
                       "schema-id", "sensor",
                       NULL);
}

/**
 * hyscan_sensor_schema_get_schema:
 * @schema: указатель на #HyScanSensorSchema
 *
 * Функция возвращает схему датчика.
 *
 * Returns: #HyScanDataSchema. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_sensor_schema_get_schema (HyScanSensorSchema *schema)
{
  HyScanDataSchemaBuilder *builder;
  HyScanDataSchema *sonar_schema;
  gchar *data;

  g_return_val_if_fail (HYSCAN_IS_SENSOR_SCHEMA (schema), NULL);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);
  data = hyscan_data_schema_builder_get_data (builder);
  sonar_schema = hyscan_data_schema_new_from_string (data, "sonar");
  g_free (data);

  return sonar_schema;
}

/**
 * hyscan_sensor_schema_add_port:
 * @schema: указатель на #HyScanSensorSchema
 * @name: название порта
 *
 * Функция добавляет в схему описание порта для подключения датчика.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_schema_add_port (HyScanSensorSchema *schema,
                               const gchar        *name)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SENSOR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  if (g_hash_table_contains (schema->priv->ports, name))
    return FALSE;

  /* Признак наличия порта. */
  key_id = g_strdup_printf ("/sensors/%s/id", name);
  hyscan_data_schema_builder_key_string_create (builder, key_id, "id", NULL, name);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  return status;
}

/**
 * hyscan_sensor_schema_source_set_position:
 * @schema: указатель на #HyScanSensorSchema
 * @name: название порта
 * @position: местоположение антенны по умолчанию
 *
 * Функция устанавливает местоположение антенны по умолчанию.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_schema_set_position (HyScanSensorSchema    *schema,
                                   const gchar           *name,
                                   HyScanAntennaPosition *position)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SENSOR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  if (!g_hash_table_contains (schema->priv->ports, name))
    return FALSE;

  prefix = g_strdup_printf ("/sensors/%s/position", name);

  /* Местоположение антенны. */
  key_id = g_strdup_printf ("%s/x", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "x", NULL, position->x);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/y", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "y", NULL,  position->y);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/z", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "z", NULL,  position->z);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/psi", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "psi", NULL, position->psi);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/gamma", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "gamma", NULL, position->gamma);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/theta", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "theta", NULL, position->theta);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

exit:
  g_free (prefix);

  return status;
}