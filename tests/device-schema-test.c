/* sonar-schema-test.c
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

#include <hyscan-actuator-schema.h>
#include <hyscan-sensor-schema.h>
#include <hyscan-sonar-schema.h>
#include <string.h>

#define N_ACTUATORS 8
#define N_SENSORS   16

HyScanSourceType orig_sources[] =
{
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD,
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_LOW,
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI,
  HYSCAN_SOURCE_SIDE_SCAN_PORT,
  HYSCAN_SOURCE_SIDE_SCAN_PORT_LOW,
  HYSCAN_SOURCE_SIDE_SCAN_PORT_HI,
  HYSCAN_SOURCE_ECHOSOUNDER,
  HYSCAN_SOURCE_ECHOSOUNDER_LOW,
  HYSCAN_SOURCE_ECHOSOUNDER_HI,
  HYSCAN_SOURCE_PROFILER,
  HYSCAN_SOURCE_PROFILER_ECHO,
  HYSCAN_SOURCE_BATHYMETRY_STARBOARD,
  HYSCAN_SOURCE_BATHYMETRY_STARBOARD_LOW,
  HYSCAN_SOURCE_BATHYMETRY_STARBOARD_HI,
  HYSCAN_SOURCE_BATHYMETRY_PORT,
  HYSCAN_SOURCE_BATHYMETRY_PORT_LOW,
  HYSCAN_SOURCE_BATHYMETRY_PORT_HI,
  HYSCAN_SOURCE_LOOK_AROUND_STARBOARD,
  HYSCAN_SOURCE_LOOK_AROUND_PORT,
  HYSCAN_SOURCE_FORWARD_LOOK,
  HYSCAN_SOURCE_FORWARD_ECHO
};
guint32 orig_n_sources = sizeof (orig_sources) / sizeof (HyScanSourceType);

HyScanSourceType
source_link (HyScanSourceType source)
{
  switch (source)
    {
    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD:
      return HYSCAN_SOURCE_BATHYMETRY_STARBOARD;

    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_LOW:
      return HYSCAN_SOURCE_BATHYMETRY_STARBOARD_LOW;

    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI:
      return HYSCAN_SOURCE_BATHYMETRY_STARBOARD_HI;

    case HYSCAN_SOURCE_SIDE_SCAN_PORT:
      return HYSCAN_SOURCE_BATHYMETRY_PORT;

    case HYSCAN_SOURCE_SIDE_SCAN_PORT_LOW:
      return HYSCAN_SOURCE_BATHYMETRY_PORT_LOW;

    case HYSCAN_SOURCE_SIDE_SCAN_PORT_HI:
      return HYSCAN_SOURCE_BATHYMETRY_PORT_HI;

    case HYSCAN_SOURCE_BATHYMETRY_STARBOARD:
      return HYSCAN_SOURCE_SIDE_SCAN_STARBOARD;

    case HYSCAN_SOURCE_BATHYMETRY_STARBOARD_LOW:
      return HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_LOW;

    case HYSCAN_SOURCE_BATHYMETRY_STARBOARD_HI:
      return HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI;

    case HYSCAN_SOURCE_BATHYMETRY_PORT:
      return HYSCAN_SOURCE_SIDE_SCAN_PORT;

    case HYSCAN_SOURCE_BATHYMETRY_PORT_LOW:
      return HYSCAN_SOURCE_SIDE_SCAN_PORT_LOW;

    case HYSCAN_SOURCE_BATHYMETRY_PORT_HI:
      return HYSCAN_SOURCE_SIDE_SCAN_PORT_HI;

    default:
      break;
    }

  return HYSCAN_SOURCE_INVALID;
}

HyScanActuatorInfoActuator *
create_actuator (guint   index,
                 gdouble seed)
{
  HyScanActuatorInfoActuator *pinfo;
  HyScanActuatorInfoActuator info;

  info.name = g_strdup_printf ("actuator-%d", index);
  info.dev_id = g_strdup_printf ("actuator-%d", index);
  info.description = g_strdup_printf ("Actuator %d", index);

  info.capabilities = HYSCAN_ACTUATOR_MODE_SCAN;
  info.capabilities |= (index % 2) ? HYSCAN_ACTUATOR_MODE_MANUAL : 0;
  info.min_range = 1.0 * index * seed;
  info.max_range = 2.0 * index * seed;
  info.min_speed = 3.0 * index * seed;
  info.max_speed = 4.0 * index * seed;

  pinfo = hyscan_actuator_info_actuator_copy (&info);

  g_free ((gchar*)info.description);
  g_free ((gchar*)info.dev_id);
  g_free ((gchar*)info.name);

  return pinfo;
}

HyScanSensorInfoSensor *
create_sensor (guint   index,
               gdouble seed)
{
  HyScanSensorInfoSensor *pinfo;
  HyScanSensorInfoSensor info;

  HyScanAntennaOffset offset;

  offset.starboard = 1.0 * index * seed;
  offset.forward = 2.0 * index * seed;
  offset.vertical = 3.0 * index * seed;
  offset.yaw = 4.0 * index * seed;
  offset.pitch = 5.0 * index * seed;
  offset.roll = 6.0 * index * seed;

  info.name = g_strdup_printf ("nmea-%d", index);
  info.dev_id = g_strdup_printf ("nmea-%d", index);
  info.description = g_strdup_printf ("Nmea sensor %d", index);

  if (index % 2)
    info.offset = &offset;
  else
    info.offset = NULL;

  pinfo = hyscan_sensor_info_sensor_copy (&info);

  g_free ((gchar*)info.description);
  g_free ((gchar*)info.dev_id);
  g_free ((gchar*)info.name);

  return pinfo;
}

HyScanSonarInfoSource *
create_source (HyScanSourceType source,
               gdouble          seed)
{
  HyScanSonarInfoSource info = {0};
  HyScanSonarInfoSource *pinfo;

  HyScanAntennaOffset offset = {0};
  HyScanSonarInfoReceiver receiver = {0};
  HyScanSonarInfoTVG tvg = {0};
  GList *presets = NULL;

  const gchar *source_id;
  gchar *actuator;
  HyScanSourceType i;

  memset (&info, 0, sizeof (info));
  memset (&offset, 0, sizeof (offset));
  memset (&receiver, 0, sizeof (receiver));
  memset (&tvg, 0, sizeof (tvg));

  source_id = hyscan_source_get_id_by_type (source);
  actuator = g_strdup_printf ("actuator%d", source);
  seed = seed * source;

  /* Описание источника данных. */
  info.source = source;
  info.dev_id = source_id;
  info.description = source_id;
  info.actuator = actuator;
  info.link = source_link (source);

  /* Смещение антенны по умолчанию. */
  offset.starboard = -seed;
  offset.forward = seed;
  offset.vertical = -seed / 2.0;
  offset.yaw = -seed * 2.0;
  offset.pitch = seed / 2.0;
  offset.roll = seed * 2.0;
  info.offset = &offset;

  /* Параметры приёмника. */
  receiver.capabilities = HYSCAN_SONAR_RECEIVER_MODE_MANUAL;
  receiver.capabilities |= (source % 2) ? HYSCAN_SONAR_RECEIVER_MODE_AUTO : 0;
  receiver.min_time = -seed;
  receiver.max_time = seed;
  info.receiver = &receiver;

  /* Режимы работы генератора. */
  for (i = 0; i < source; i++)
    {
      HyScanDataSchemaEnumValue *preset;
      gchar *id = g_strdup_printf ("%s-preset-%d", source_id, i + 1);
      gchar *name = g_strdup_printf ("%s name %d", source_id, i + 1);
      gchar *description = g_strdup_printf ("%s description %d", source_id, i + 1);

      preset = hyscan_data_schema_enum_value_new (i, id, name, description);
      presets = g_list_append (presets, preset);

      g_free (description);
      g_free (name);
      g_free (id);
    }
  info.presets = presets;

  /* Параметры ВАРУ. */
  tvg.capabilities = HYSCAN_SONAR_TVG_MODE_AUTO;
  tvg.capabilities |= (source % 3) ? HYSCAN_SONAR_TVG_MODE_CONSTANT : 0;
  tvg.capabilities |= (source % 4) ? HYSCAN_SONAR_TVG_MODE_LINEAR_DB : 0;
  tvg.capabilities |= (source % 5) ? HYSCAN_SONAR_TVG_MODE_LOGARITHMIC : 0;
  tvg.decrease = (source % 2);

  if (tvg.capabilities != HYSCAN_SONAR_TVG_MODE_AUTO)
    {
      tvg.min_gain = -seed;
      tvg.max_gain = seed;
    }
  info.tvg = &tvg;

  pinfo = hyscan_sonar_info_source_copy (&info);

  g_list_free_full (presets, (GDestroyNotify)hyscan_data_schema_enum_value_free);
  g_free (actuator);

  return pinfo;
}

void
verify_actuator (const HyScanActuatorInfoActuator *actuator1,
                 const HyScanActuatorInfoActuator *actuator2)
{
  if (g_strcmp0 (actuator1->name, actuator2->name) != 0)
    g_error ("name failed");
  if (g_strcmp0 (actuator1->dev_id, actuator2->dev_id) != 0)
    g_error ("dev-id failed");
  if (g_strcmp0 (actuator1->description, actuator2->description) != 0)
    g_error ("decripton failed");

  if (actuator1->capabilities != actuator2->capabilities)
    g_error ("capabilities failed");

  if ((actuator1->min_range != actuator2->min_range) ||
      (actuator1->max_range != actuator2->max_range))
    {
      g_error ("range failed");
    }

  if ((actuator1->min_speed != actuator2->min_speed) ||
      (actuator1->max_speed != actuator2->max_speed))
    {
      g_error ("speed failed");
    }
}

void
verify_sensor (const HyScanSensorInfoSensor *sensor1,
               const HyScanSensorInfoSensor *sensor2)
{
  if (g_strcmp0 (sensor1->name, sensor2->name) != 0)
    g_error ("name failed");
  if (g_strcmp0 (sensor1->dev_id, sensor2->dev_id) != 0)
    g_error ("dev-id failed");
  if (g_strcmp0 (sensor1->description, sensor2->description) != 0)
    g_error ("decripton failed");

  if ((sensor1->offset != NULL) ||
      (sensor2->offset != NULL))
    {
      if ((sensor1->offset->starboard != sensor2->offset->starboard) ||
          (sensor1->offset->forward != sensor2->offset->forward) ||
          (sensor1->offset->vertical != sensor2->offset->vertical) ||
          (sensor1->offset->yaw != sensor2->offset->yaw) ||
          (sensor1->offset->pitch != sensor2->offset->pitch) ||
          (sensor1->offset->roll != sensor2->offset->roll))
        {
          g_error ("offset failed");
        }
    }
}

void
verify_source (const HyScanSonarInfoSource *source1,
               const HyScanSonarInfoSource *source2)
{
  /* Тип источника данных. */
  if (source1->source != source2->source)
    g_error ("source failed");

  /* Уникальный идентификатор устройства. */
  if (g_strcmp0 (source1->dev_id, source2->dev_id) != 0)
    g_error ("dev-id failed");

  /* Описание источника данных. */
  if (g_strcmp0 (source1->description, source2->description) != 0)
    g_error ("description failed");

  /* Используемый привод. */
  if (g_strcmp0 (source1->actuator, source2->actuator) != 0)
    g_error ("actuator failed");

  /* Связанный источник данных. */
  if (source1->link != source2->link)
    g_error ("link failed");

  /* Смещение антенны по умолчанию. */
  if ((source1->offset != NULL) ||
      (source2->offset != NULL))
    {
      if ((source1->offset->starboard != source2->offset->starboard) ||
          (source1->offset->forward != source2->offset->forward) ||
          (source1->offset->vertical != source2->offset->vertical) ||
          (source1->offset->yaw != source2->offset->yaw) ||
          (source1->offset->pitch != source2->offset->pitch) ||
          (source1->offset->roll != source2->offset->roll))
        {
          g_error ("offset failed");
        }
    }

  /* Параметры приёмника. */
  if ((source1->receiver != NULL) ||
      (source2->receiver != NULL))
    {
      if ((source1->receiver->capabilities != source2->receiver->capabilities) ||
          (source1->receiver->min_time != source2->receiver->min_time) ||
          (source1->receiver->max_time != source2->receiver->max_time))
        {
          g_error ("receiver failed");
        }
    }

  /* Параметры генератора. */
  if ((source1->presets != NULL) ||
      (source2->presets != NULL))
    {
      GList *presets1 = source1->presets;

      /* Преднастройки генератора. */
      while (presets1 != NULL)
        {
          HyScanDataSchemaEnumValue *preset1 = presets1->data;
          GList *presets2 = source2->presets;
          gboolean status = FALSE;

          while (presets2 != NULL)
            {
              HyScanDataSchemaEnumValue *preset2 = presets2->data;

              if ((preset1->value == preset2->value) &&
                  (g_strcmp0 (preset1->id, preset2->id) == 0) &&
                  (g_strcmp0 (preset1->name, preset2->name) == 0) &&
                  (g_strcmp0 (preset1->description, preset2->description) == 0))
                {
                  if (status)
                    g_error ("%s dup", preset1->name);

                  status = TRUE;
                }

              presets2 = g_list_next (presets2);
            }
          if (!status)
            g_error ("%s failed", preset1->name);

          presets1 = g_list_next (presets1);
        }
    }

  /* Параметры ВАРУ. */
  if ((source1->tvg != NULL) ||
      (source2->tvg != NULL))
    {
      if ((source1->tvg->capabilities != source2->tvg->capabilities) ||
          (source1->tvg->min_gain != source2->tvg->min_gain) ||
          (source1->tvg->max_gain != source2->tvg->max_gain) ||
          (source1->tvg->decrease != source2->tvg->decrease))
        {
          g_error ("tvg failed");
        }
    }
}

int
main (int    argc,
      char **argv)
{
  HyScanDeviceSchema *device_schema;
  HyScanActuatorSchema *actuator_schema;
  HyScanSensorSchema *sensor_schema;
  HyScanSonarSchema *sonar_schema;
  HyScanActuatorInfo *actuator_info;
  HyScanSensorInfo *sensor_info;
  HyScanSonarInfo *sonar_info;
  HyScanDataSchema *schema;

  const HyScanSourceType *sources;
  const gchar * const *actuators;
  const gchar * const *sensors;
  guint n_sources;
  gdouble seed;
  guint i, j;

  seed = 1000.0 * g_random_double ();

  device_schema = hyscan_device_schema_new (HYSCAN_DEVICE_SCHEMA_VERSION);
  actuator_schema = hyscan_actuator_schema_new (device_schema);
  sensor_schema = hyscan_sensor_schema_new (device_schema);
  sonar_schema = hyscan_sonar_schema_new (device_schema);

  /* Создаём приводы. */
  for (i = 0; i < N_ACTUATORS; i++)
    {
      HyScanActuatorInfoActuator *actuator = create_actuator (i, seed);

      if (!hyscan_actuator_schema_add_full (actuator_schema, actuator))
        g_error ("can't add actuator %s", actuator->name);

      hyscan_actuator_info_actuator_free (actuator);
    }

  /* Создаём датчики. */
  for (i = 0; i < N_SENSORS; i++)
    {
      HyScanSensorInfoSensor *sensor = create_sensor (i, seed);

      if (!hyscan_sensor_schema_add_full (sensor_schema, sensor))
        g_error ("can't add sensor %s", sensor->name);

      hyscan_sensor_info_sensor_free (sensor);
    }

  /* Создаём источники данных. */
  for (i = 0; i < orig_n_sources; i++)
    {
      HyScanSonarInfoSource *source = create_source (orig_sources[i], seed);

      if (!hyscan_sonar_schema_source_add_full (sonar_schema, source))
        g_error ("can't add source %s", hyscan_source_get_id_by_type (orig_sources[i]));

      hyscan_sonar_info_source_free (source);
    }

  /* Создаём схему устройства. */
  schema = hyscan_data_schema_builder_get_schema (HYSCAN_DATA_SCHEMA_BUILDER (device_schema));
  actuator_info = hyscan_actuator_info_new (schema);
  sensor_info = hyscan_sensor_info_new (schema);
  sonar_info = hyscan_sonar_info_new (schema);

  /* Проверяем список приводов. */
  actuators = hyscan_actuator_info_list_actuators (actuator_info);
  if (g_strv_length ((gchar **)actuators) != N_ACTUATORS)
    g_error ("n_actuators mismatch");

  /* Проверяем параметры приводов. */
  for (i = 0; i < N_ACTUATORS; i++)
    {
      HyScanActuatorInfoActuator *orig_actuator;
      const HyScanActuatorInfoActuator *actuator;

      orig_actuator = create_actuator (i, seed);
      actuator = hyscan_actuator_info_get_actuator (actuator_info, orig_actuator->name);

      g_message ("Check actuator %s", orig_actuator->name);

      for (j = 0; j < N_ACTUATORS; j++)
        if (g_strcmp0 (orig_actuator->name, actuators[j]) == 0)
          break;

      if (j == N_ACTUATORS)
        g_error ("list failed");

      verify_actuator (orig_actuator, actuator);

      hyscan_actuator_info_actuator_free (orig_actuator);
    }

  /* Проверяем список датчиков. */
  sensors = hyscan_sensor_info_list_sensors (sensor_info);
  if (g_strv_length ((gchar **)sensors) != N_SENSORS)
    g_error ("n_sensors mismatch");

  /* Проверяем параметры датчиков. */
  for (i = 0; i < N_SENSORS; i++)
    {
      HyScanSensorInfoSensor *orig_sensor;
      const HyScanSensorInfoSensor *sensor;

      orig_sensor = create_sensor (i, seed);
      sensor = hyscan_sensor_info_get_sensor (sensor_info, orig_sensor->name);

      g_message ("Check sensor %s", orig_sensor->name);

      for (j = 0; j < N_SENSORS; j++)
        if (g_strcmp0 (orig_sensor->name, sensors[j]) == 0)
          break;

      if (j == N_SENSORS)
        g_error ("list failed");

      verify_sensor (orig_sensor, sensor);

      hyscan_sensor_info_sensor_free (orig_sensor);
    }

  /* Проверяем список источников. */
  sources = hyscan_sonar_info_list_sources (sonar_info, &n_sources);
  if ((sources == NULL) || (n_sources != orig_n_sources))
    g_error ("n_sources mismatch");

  /* Проверяем параметры источников данных. */
  for (i = 0; i < n_sources; i++)
    {
      HyScanSonarInfoSource *orig_source;
      const HyScanSonarInfoSource *source;

      orig_source = create_source (orig_sources[i], seed);
      source = hyscan_sonar_info_get_source (sonar_info, orig_sources[i]);

      g_message ("Check source %s", hyscan_source_get_id_by_type (orig_sources[i]));

      for (j = 0; j < n_sources; j++)
        if (orig_sources[i] == sources[j])
          break;

      if (j == n_sources)
        g_error ("list failed");

      verify_source (orig_source, source);

      hyscan_sonar_info_source_free (orig_source);
    }

  g_object_unref (schema);
  g_object_unref (device_schema);
  g_object_unref (actuator_schema);
  g_object_unref (sensor_schema);
  g_object_unref (sonar_schema);
  g_object_unref (actuator_info);
  g_object_unref (sensor_info);
  g_object_unref (sonar_info);

  g_message ("All done");

  return 0;
}
