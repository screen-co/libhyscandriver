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
 * Во-первых, вы можете распространять HyScanDriver на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
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
 *
 * Информация о датчиках находится в ветке "/sensors" схемы устройства. Схема
 * может содержать информацию о произвольном количестве датчиков. Каждый
 * датчик идентифицируется произвольным именем. Информация о датчике находится
 * в ветке "/sensors/sensor-name", где sensor-name - имя датчика и содержит
 * следующие поля:
 *
 * - dev-id - уникальный идентификатор устройства, тип STRING, обязательное;
 * - description - поле с описанием датчика, тип STRING, необязательное.
 *
 * Для датчика может быть задано местоположение антенн по умолчанию. Если
 * местоположение антенны залано, должны быть определены все параметры.
 *
 * Местоположение антенны задают следующие параметры:
 *
 * - antenna/position/x - смещение антенны по оси X, тип DOUBLE;
 * - antenna/position/y - смещение антенны по оси Y, тип DOUBLE;
 * - antenna/position/z - смещение антенны по оси Z, тип DOUBLE;
 * - antenna/position/psi - поворот антенны по курсу, тип DOUBLE;
 * - antenna/position/gamma - поворот антенны по крену, тип DOUBLE;
 * - antenna/position/theta - поворот антенны по дифференту, тип DOUBLE.
 *
 * Подробное описание этих параметров приведено в #HyScanAntennaPosition.
 *
 * Пример всех параметров для датчика с именем "nmea":
 *
 * - /sensors/nmea/dev-id
 * - /sensors/nmea/description
 * - /sensors/nmea/antenna/position/x
 * - /sensors/nmea/antenna/position/y
 * - /sensors/nmea/antenna/position/z
 * - /sensors/nmea/antenna/position/psi
 * - /sensors/nmea/antenna/position/gamma
 * - /sensors/nmea/antenna/position/theta
 *
 * Дополнительные параметры, если они необходимы, должны находится в ветках
 * "/params" и "/system". Состояние устройства приводится в ветке "/state",
 * а общая информация об устройстве и драйвере в ветке "/info". Описание этих
 * веток приведено в #HyscanDeviceSchema.
 *
 * Для создания схемы используется класс #HyScanDataSchemaBuilder, указатель
 * на который передаётся в функцию #hyscan_sensor_schema_new.
 *
 * Датчик добавляется с помощью функции #hyscan_sensor_schema_add_sensor.
 *
 * Местоположение антенны по умолчанию задаётся с помощью функции
 * #hyscan_sensor_schema_set_position.
 *
 * Функция hyscan_sensor_schema_add_full добавляет датчик описанный
 * стуруктурой #HyScanSensorInfoSensor.
 */

#include "hyscan-sensor-schema.h"
#include "hyscan-device-schema.h"

enum
{
  PROP_O,
  PROP_BUILDER
};

struct _HyScanSensorSchemaPrivate
{
  HyScanDataSchemaBuilder     *builder;                      /* Строитель схемы. */
  GHashTable                  *sensors;                      /* Наличие датчиков. */
};

static void    hyscan_sensor_schema_set_property             (GObject                 *object,
                                                              guint                    prop_id,
                                                              const GValue            *value,
                                                              GParamSpec              *pspec);

static void    hyscan_sensor_schema_object_constructed       (GObject                 *object);
static void    hyscan_sensor_schema_object_finalize          (GObject                 *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSensorSchema, hyscan_sensor_schema, G_TYPE_OBJECT)

static void
hyscan_sensor_schema_class_init (HyScanSensorSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_sensor_schema_set_property;

  object_class->constructed = hyscan_sensor_schema_object_constructed;
  object_class->finalize = hyscan_sensor_schema_object_finalize;

  g_object_class_install_property (object_class, PROP_BUILDER,
    g_param_spec_object ("builder", "Builder", "Schema builder", HYSCAN_TYPE_DATA_SCHEMA_BUILDER,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_sensor_schema_init (HyScanSensorSchema *schema)
{
  schema->priv = hyscan_sensor_schema_get_instance_private (schema);
}

static void
hyscan_sensor_schema_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  HyScanSensorSchema *schema = HYSCAN_SENSOR_SCHEMA (object);
  HyScanSensorSchemaPrivate *priv = schema->priv;

  switch (prop_id)
    {
    case PROP_BUILDER:
      priv->builder = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_sensor_schema_object_constructed (GObject *object)
{
  HyScanSensorSchema *schema = HYSCAN_SENSOR_SCHEMA (object);
  HyScanSensorSchemaPrivate *priv = schema->priv;
  HyScanDataSchemaBuilder *builder = priv->builder;

  G_OBJECT_CLASS (hyscan_sensor_schema_parent_class)->constructed (object);

  if (builder == NULL)
    return;

  if (!hyscan_device_schema_set_id (builder))
    return;

  priv->sensors = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
}

static void
hyscan_sensor_schema_object_finalize (GObject *object)
{
  HyScanSensorSchema *schema = HYSCAN_SENSOR_SCHEMA (object);
  HyScanSensorSchemaPrivate *priv = schema->priv;

  g_hash_table_unref (priv->sensors);
  g_clear_object (&priv->builder);

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
hyscan_sensor_schema_new (HyScanDataSchemaBuilder *builder)
{
  return g_object_new (HYSCAN_TYPE_SENSOR_SCHEMA,
                       "builder", builder,
                       NULL);
}

/**
 * hyscan_sensor_schema_add_full:
 * @schema: указатель на #HyScanSensorSchema
 * @info: (transfer none): параметры датчика
 *
 * Функция добавляет в схему описание датчика.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_schema_add_full (HyScanSensorSchema     *schema,
                               HyScanSensorInfoSensor *info)
{
  if (!hyscan_sensor_schema_add_sensor (schema, info->name, info->dev_id, info->description))
    return FALSE;

  if (info->position !=NULL)
    {
      if (!hyscan_sensor_schema_set_position (schema, info->name, info->position))
        return FALSE;
    }

  return TRUE;
}

/**
 * hyscan_sensor_schema_add_sensor:
 * @schema: указатель на #HyScanSensorSchema
 * @name: название датчика
 * @dev_id: уникальный идентификатор устройства
 * @description: описание датчика
 *
 * Функция добавляет в схему описание датчика.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_schema_add_sensor (HyScanSensorSchema *schema,
                                 const gchar        *name,
                                 const gchar        *dev_id,
                                 const gchar        *description)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SENSOR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  if (g_hash_table_contains (schema->priv->sensors, name))
    return FALSE;

  /* Уникальный идентификатор устройства. */
  status = FALSE;
  key_id = g_strdup_printf ("/sensors/%s/dev-id", name);
  if (hyscan_data_schema_builder_key_string_create (builder, key_id, "dev-id", NULL, dev_id))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    return FALSE;

  /* Описание датчика. */
  if (description != NULL)
    {
      status = FALSE;
      key_id = g_strdup_printf ("/sensors/%s/description", name);
      if (hyscan_data_schema_builder_key_string_create (builder, key_id, "description", NULL, description))
        status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
      g_free (key_id);

      if (!status)
        return FALSE;
    }

  if (status)
    g_hash_table_insert (schema->priv->sensors, g_strdup (name), NULL);

  return status;
}

/**
 * hyscan_sensor_schema_source_set_position:
 * @schema: указатель на #HyScanSensorSchema
 * @name: название датчика
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
  gboolean status = FALSE;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SENSOR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sensors, name))
    return FALSE;

  prefix = g_strdup_printf ("/sensors/%s/antenna/position", name);

  /* Местоположение антенны. */
  key_id = g_strdup_printf ("%s/x", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "x", NULL, position->x))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/y", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "y", NULL,  position->y))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/z", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "z", NULL,  position->z))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/psi", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "psi", NULL, position->psi))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/gamma", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "gamma", NULL, position->gamma))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/theta", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "theta", NULL, position->theta))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

exit:
  g_free (prefix);

  return status;
}
