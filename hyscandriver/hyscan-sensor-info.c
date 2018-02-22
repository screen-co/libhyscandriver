/* hyscan-sensor-info.c
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
 * SECTION: hyscan-sensor-info
 * @Short_description: класс с информацией о датчиках
 * @Title: HyScanSensorInfo
 *
 * Класс предназначен для получения информации о датчиках по их схеме.
 * Для создания объекта используется функция #hyscan_sensor_info_new, в
 * которую передаётся схема устройства.
 *
 * Список датчиков можно получить с помощью функции
 * #hyscan_sensor_info_list_sensors.
 *
 * Параметры датчика можно получить с помощью функции
 * #hyscan_sensor_info_get_sensor.
 */

#include "hyscan-sensor-info.h"
#include "hyscan-device-schema.h"
#include <hyscan-data-schema.h>

#define SENSOR_PARAM_NAME(sensor,prefix,param)                 hyscan_sensor_info_sonar_param_name (name_buffer, \
                                                               (gulong)sizeof (name_buffer), \
                                                               sensor, prefix, param)

enum
{
  PROP_O,
  PROP_SCHEMA
};

struct _HyScanSensorInfoPrivate
{
  HyScanDataSchema            *schema;         /* Схема устройства. */
  GHashTable                  *sensors;        /* Параметры датчиков. */
  GArray                      *sensors_list;   /* Список датчиков. */
};

static void            hyscan_sensor_info_set_property         (GObject               *object,
                                                                guint                  prop_id,
                                                                const GValue          *value,
                                                                GParamSpec            *pspec);
static void            hyscan_sensor_info_object_constructed   (GObject               *object);
static void            hyscan_sensor_info_object_finalize      (GObject               *object);

static const gchar *   hyscan_sensor_info_sonar_param_name     (gchar                 *buffer,
                                                                guint                  size,
                                                                const gchar           *sensor,
                                                                const gchar           *prefix,
                                                                const gchar           *param);

static HyScanAntennaPosition *
                       hyscan_sensor_info_parse_position       (HyScanDataSchema      *schema,
                                                                const gchar           *sensor);

static GHashTable *    hyscan_sensor_info_parse_sensors        (HyScanDataSchema      *schema);

G_DEFINE_BOXED_TYPE (HyScanSensorInfoSensor, hyscan_sensor_info_sensor,
                     hyscan_sensor_info_sensor_copy, hyscan_sensor_info_sensor_free)

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSensorInfo, hyscan_sensor_info, G_TYPE_OBJECT)

static void
hyscan_sensor_info_class_init (HyScanSensorInfoClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_sensor_info_set_property;

  object_class->constructed = hyscan_sensor_info_object_constructed;
  object_class->finalize = hyscan_sensor_info_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "Sonar schema", HYSCAN_TYPE_DATA_SCHEMA,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_sensor_info_init (HyScanSensorInfo *sensor_info)
{
  sensor_info->priv = hyscan_sensor_info_get_instance_private (sensor_info);
}

static void
hyscan_sensor_info_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  HyScanSensorInfo *sensor_info = HYSCAN_SENSOR_INFO (object);
  HyScanSensorInfoPrivate *priv = sensor_info->priv;

  switch (prop_id)
    {
    case PROP_SCHEMA:
      priv->schema = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_sensor_info_object_constructed (GObject *object)
{
  HyScanSensorInfo *info = HYSCAN_SENSOR_INFO (object);
  HyScanSensorInfoPrivate *priv = info->priv;

  GHashTableIter iter;
  gpointer name;

  /* Должна быть задана схема гидролокатора. */
  if (priv->schema == NULL)
    return;

  /* Верифицируем схему устройства. */
  if (!hyscan_device_schema_check_id (priv->schema))
    {
      g_warning ("HyScanSonarInfo: unsupported device schema");
      return;
    }

  /* Информация о датчиках. */
  priv->sensors = hyscan_sensor_info_parse_sensors (priv->schema);
  if (priv->sensors == NULL)
    return;

  /* Список датчиков. */
  priv->sensors_list = g_array_new (TRUE, TRUE, sizeof (gchar*));
  g_hash_table_iter_init (&iter, priv->sensors);
  while (g_hash_table_iter_next (&iter, &name, NULL))
    g_array_append_val (priv->sensors_list, name);
}

static void
hyscan_sensor_info_object_finalize (GObject *object)
{
  HyScanSensorInfo *info = HYSCAN_SENSOR_INFO (object);
  HyScanSensorInfoPrivate *priv = info->priv;

  g_clear_object (&priv->schema);
  g_clear_pointer (&priv->sensors, g_hash_table_unref);
  g_clear_pointer (&priv->sensors_list, g_array_unref);

  G_OBJECT_CLASS (hyscan_sensor_info_parent_class)->finalize (object);
}

/* Функция формирует название параметра датчика. */
static const gchar *
hyscan_sensor_info_sonar_param_name (gchar       *buffer,
                                     guint        size,
                                     const gchar *sensor,
                                     const gchar *prefix,
                                     const gchar *param)
{
  if (prefix != NULL)
    {
      g_snprintf (buffer, size, "/sensors/%s/%s/%s",
                  sensor, prefix, param);
    }
  else
    {
      g_snprintf (buffer, size, "/sensors/%s/%s",
                  sensor, param);
    }

  return buffer;
}

/* Функция считывает информацию о местоположении антенн. */
static HyScanAntennaPosition *
hyscan_sensor_info_parse_position (HyScanDataSchema *schema,
                                   const gchar      *sensor)
{
  HyScanAntennaPosition *info = g_slice_new0 (HyScanAntennaPosition);
  gchar name_buffer[128];
  const gchar *name;

  name = SENSOR_PARAM_NAME (sensor, "antenna", "position/x");
  if (!hyscan_device_schema_get_double (schema, name, &info->x))
    goto fail;

  name = SENSOR_PARAM_NAME (sensor, "antenna", "position/y");
  if (!hyscan_device_schema_get_double (schema, name, &info->y))
    goto fail;

  name = SENSOR_PARAM_NAME (sensor, "antenna", "position/z");
  if (!hyscan_device_schema_get_double (schema, name, &info->z))
    goto fail;

  name = SENSOR_PARAM_NAME (sensor, "antenna", "position/psi");
  if (!hyscan_device_schema_get_double (schema, name, &info->psi))
    goto fail;

  name = SENSOR_PARAM_NAME (sensor, "antenna", "position/gamma");
  if (!hyscan_device_schema_get_double (schema, name, &info->gamma))
    goto fail;

  name = SENSOR_PARAM_NAME (sensor, "antenna", "position/theta");
  if (!hyscan_device_schema_get_double (schema, name, &info->theta))
    goto fail;

  return info;

fail:
  g_slice_free (HyScanAntennaPosition, info);

  return NULL;
}

/* Функция считывает параметры датчиков. */
static GHashTable *
hyscan_sensor_info_parse_sensors (HyScanDataSchema *schema)
{
  GHashTable *sensors;
  gchar **keys;
  guint i;

  keys = hyscan_data_schema_list_keys (schema);
  if (keys == NULL)
    return NULL;

  sensors = g_hash_table_new_full (g_str_hash, g_str_equal,
                                   g_free, (GDestroyNotify)hyscan_sensor_info_sensor_free);

  for (i = 0; keys[i] != NULL; i++)
    {
      gchar **keyv = g_strsplit (keys[i], "/", -1);

      /* Ищем параметр /sensors/sensor-name/id */
      if (g_strv_length (keyv) != 4)
        {
          g_strfreev (keyv);
          continue;
        }

      if ((g_strcmp0 (keyv[1], "sensors") == 0) && (g_strcmp0 (keyv[3], "id") == 0))
        {
          HyScanSensorInfoSensor *info;
          const gchar *description;
          gchar name_buffer[128];
          const gchar *name;

          name = SENSOR_PARAM_NAME (keyv[2], NULL, "description");
          description = hyscan_device_schema_get_string (schema, name);

          info = g_slice_new0 (HyScanSensorInfoSensor);
          info->name = g_strdup (keyv[2]);
          info->description = g_strdup (description);
          info->position = hyscan_sensor_info_parse_position (schema, keyv[2]);

          g_hash_table_insert (sensors, g_strdup (keyv[2]), info);
        }

      g_strfreev (keyv);
    }

  g_strfreev (keys);

  if (g_hash_table_size (sensors) == 0)
    g_clear_pointer (&sensors, g_hash_table_unref);

  return sensors;
}

/**
 * hyscan_sensor_info_new:
 * @schema: схема устройства
 *
 * Функция создаёт новый объект с информацией о датчиках.
 *
 * Returns: #HyScanSensorInfo. Для удаления #g_object_unref.
 */
HyScanSensorInfo *
hyscan_sensor_info_new (HyScanDataSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_SENSOR_INFO,
                       "schema", schema,
                       NULL);
}

/**
 * hyscan_sonar_info_get_sensors:
 * @info: указатель на #HyScanSensorInfo
 *
 * Функция возвращает список датчиков.
 *
 * Returns: (transfer none): Список датчиков.
 */
const gchar * const *
hyscan_sensor_info_list_sensors (HyScanSensorInfo *info)
{
  g_return_val_if_fail (HYSCAN_IS_SENSOR_INFO (info), NULL);

  if ((info->priv->sensors_list == NULL) || (info->priv->sensors_list->len == 0))
    return NULL;

  return (const gchar * const *)info->priv->sensors_list->data;
}

/**
 * hyscan_sensor_info_get_sensor:
 * @info: указатель на #HyScanSensorInfo
 * @name: название датчика
 *
 * функция возвращает параметры датчика.
 *
 * Returns: (transfer none): Параметры датчика или NULL.
 */
const HyScanSensorInfoSensor *
hyscan_sensor_info_get_sensor (HyScanSensorInfo *info,
                               const gchar      *name)
{
  g_return_val_if_fail (HYSCAN_IS_SENSOR_INFO (info), NULL);

  if (info->priv->sensors == NULL)
    return NULL;

  return g_hash_table_lookup (info->priv->sensors, name);
}

/**
 * hyscan_sensor_info_sensor_copy:
 * @info: структура #HyScanSensorInfoSensor для копирования
 *
 * Функция создаёт копию структуры #HyScanSensorInfoSensor.
 *
 * Returns: (transfer full): Новая структура #HyScanSensorInfoSensor.
 * Для удаления #hyscan_sensor_info_sensor_free.
 */
HyScanSensorInfoSensor *
hyscan_sensor_info_sensor_copy (const HyScanSensorInfoSensor *info)
{
  HyScanSensorInfoSensor *new_info;

  if (info == NULL)
    return NULL;

  new_info = g_slice_new (HyScanSensorInfoSensor);
  new_info->name = g_strdup (info->name);
  new_info->description = g_strdup (info->description);
  new_info->position = hyscan_antenna_position_copy (info->position);

  return new_info;
}

/**
 * hyscan_sensor_info_sensor_free:
 * @info: структура #HyScanSensorInfoSensor для удаления
 *
 * Функция удаляет структуру #HyScanSensorInfoSensor.
 */
void
hyscan_sensor_info_sensor_free (HyScanSensorInfoSensor *info)
{
  if (info == NULL)
    return;

  g_free ((gchar*)info->name);
  g_free ((gchar*)info->description);
  hyscan_antenna_position_free (info->position);

  g_slice_free (HyScanSensorInfoSensor, info);
}
