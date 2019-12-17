/* hyscan-sensor-schema.c
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
 * Для датчика может быть задано смещение антенны по умолчанию. Если
 * смещение антенны задано, должны быть определены все параметры.
 *
 * Смещение антенны задают следующие параметры:
 *
 * - antenna/offset/x - смещение антенны по оси X, тип DOUBLE;
 * - antenna/offset/y - смещение антенны по оси Y, тип DOUBLE;
 * - antenna/offset/z - смещение антенны по оси Z, тип DOUBLE;
 * - antenna/offset/psi - поворот антенны по курсу, тип DOUBLE;
 * - antenna/offset/gamma - поворот антенны по крену, тип DOUBLE;
 * - antenna/offset/theta - поворот антенны по дифференту, тип DOUBLE.
 *
 * Подробное описание этих параметров приведено в #HyScanAntennaOffset.
 *
 * Пример всех параметров для датчика с именем "nmea":
 *
 * - /sensors/nmea/dev-id
 * - /sensors/nmea/description
 * - /sensors/nmea/antenna/offset/x
 * - /sensors/nmea/antenna/offset/y
 * - /sensors/nmea/antenna/offset/z
 * - /sensors/nmea/antenna/offset/psi
 * - /sensors/nmea/antenna/offset/gamma
 * - /sensors/nmea/antenna/offset/theta
 *
 * Дополнительные параметры, если они необходимы, должны находится в ветках
 * "/params" и "/system". Состояние устройства приводится в ветке "/state",
 * а общая информация об устройстве и драйвере в ветке "/info". Описание этих
 * веток приведено в #HyscanDeviceSchema.
 *
 * Для создания схемы используется класс #HyScanDeviceSchema, указатель
 * на который передаётся в функцию #hyscan_sensor_schema_new.
 *
 * Датчик добавляется с помощью функции #hyscan_sensor_schema_add_sensor.
 *
 * Смещение антенны по умолчанию задаётся с помощью функции
 * #hyscan_sensor_schema_set_offset.
 *
 * Функция hyscan_sensor_schema_add_full добавляет датчик описанный
 * стуруктурой #HyScanSensorInfoSensor.
 */

#include "hyscan-sensor-schema.h"
#include "hyscan-device-schema.h"

#define SENSOR_PARAM_NAME(...)  hyscan_param_name_constructor (key_id, \
                                  (guint)sizeof (key_id), \
                                  "sensors", __VA_ARGS__)

enum
{
  PROP_O,
  PROP_SCHEMA
};

struct _HyScanSensorSchemaPrivate
{
  HyScanDataSchemaBuilder     *builder;                        /* Строитель схемы. */
  GHashTable                  *sensors;                        /* Наличие датчиков. */
};

static void    hyscan_sensor_schema_set_property               (GObject                 *object,
                                                                guint                    prop_id,
                                                                const GValue            *value,
                                                                GParamSpec              *pspec);

static void    hyscan_sensor_schema_object_constructed         (GObject                 *object);
static void    hyscan_sensor_schema_object_finalize            (GObject                 *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSensorSchema, hyscan_sensor_schema, G_TYPE_OBJECT)

static void
hyscan_sensor_schema_class_init (HyScanSensorSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_sensor_schema_set_property;

  object_class->constructed = hyscan_sensor_schema_object_constructed;
  object_class->finalize = hyscan_sensor_schema_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "Device schema", HYSCAN_TYPE_DEVICE_SCHEMA,
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
    case PROP_SCHEMA:
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
 * @schema: указатель на #HyScanDeviceSchema
 *
 * Функция создаёт новый объект HyScanSensorSchema.
 *
 * Returns: #HyScanSensorSchema. Для удаления #g_object_unref.
 */
HyScanSensorSchema *
hyscan_sensor_schema_new (HyScanDeviceSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_SENSOR_SCHEMA,
                       "schema", schema,
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

  if (info->offset !=NULL)
    {
      if (!hyscan_sensor_schema_set_offset (schema, info->name, info->offset))
        return FALSE;
    }

  return TRUE;
}

/**
 * hyscan_sensor_schema_add_sensor:
 * @schema: указатель на #HyScanSensorSchema
 * @sensor: название датчика
 * @dev_id: уникальный идентификатор устройства
 * @description: описание датчика
 *
 * Функция добавляет в схему описание датчика.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_schema_add_sensor (HyScanSensorSchema *schema,
                                 const gchar        *sensor,
                                 const gchar        *dev_id,
                                 const gchar        *description)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SENSOR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  if (g_hash_table_contains (schema->priv->sensors, sensor))
    return FALSE;

  /* Уникальный идентификатор устройства. */
  status = FALSE;
  SENSOR_PARAM_NAME (sensor, "dev-id", NULL);
  if (hyscan_data_schema_builder_key_string_create (builder, key_id, "dev-id", NULL, dev_id))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

  if (!status)
    return FALSE;

  /* Описание датчика. */
  if (description != NULL)
    {
      status = FALSE;
      SENSOR_PARAM_NAME (sensor, "description", NULL);
      if (hyscan_data_schema_builder_key_string_create (builder, key_id, "description", NULL, description))
        status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

      if (!status)
        return FALSE;
    }

  if (status)
    g_hash_table_insert (schema->priv->sensors, g_strdup (sensor), NULL);

  return status;
}

/**
 * hyscan_sensor_schema_source_set_offset:
 * @schema: указатель на #HyScanSensorSchema
 * @name: название датчика
 * @offset: смещение антенны по умолчанию
 *
 * Функция устанавливает смещение антенны по умолчанию.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_schema_set_offset (HyScanSensorSchema  *schema,
                                 const gchar         *sensor,
                                 HyScanAntennaOffset *offset)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SENSOR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sensors, sensor))
    return FALSE;


  /* Cмещение антенны. */
  status = FALSE;
  SENSOR_PARAM_NAME (sensor, "offset/starboard", NULL);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "x", NULL, offset->starboard))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

  if (!status)
    goto exit;

  status = FALSE;
  SENSOR_PARAM_NAME (sensor, "offset/forward", NULL);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "y", NULL,  offset->forward))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

  if (!status)
    goto exit;

  status = FALSE;
  SENSOR_PARAM_NAME (sensor, "offset/vertical", NULL);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "z", NULL,  offset->vertical))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

  if (!status)
    goto exit;

  status = FALSE;
  SENSOR_PARAM_NAME (sensor, "offset/yaw", NULL);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "psi", NULL, offset->yaw))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

  if (!status)
    goto exit;

  status = FALSE;
  SENSOR_PARAM_NAME (sensor, "offset/pitch", NULL);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "gamma", NULL, offset->pitch))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

  if (!status)
    goto exit;

  status = FALSE;
  SENSOR_PARAM_NAME (sensor, "offset/roll", NULL);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "theta", NULL, offset->roll))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

exit:

  return status;
}
