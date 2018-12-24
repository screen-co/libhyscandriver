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

#include <hyscan-sensor-schema.h>
#include <hyscan-sonar-schema.h>
#include <libxml/parser.h>
#include <string.h>

#define N_SENSORS 16

HyScanSourceType orig_sources[] =
{
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD,
  HYSCAN_SOURCE_SIDE_SCAN_PORT,
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI,
  HYSCAN_SOURCE_SIDE_SCAN_PORT_HI,
  HYSCAN_SOURCE_ECHOSOUNDER,
  HYSCAN_SOURCE_ECHOSOUNDER_HI,
  HYSCAN_SOURCE_PROFILER,
  HYSCAN_SOURCE_ECHOPROFILER,
  HYSCAN_SOURCE_BATHYMETRY_STARBOARD,
  HYSCAN_SOURCE_BATHYMETRY_PORT,
  HYSCAN_SOURCE_LOOK_AROUND_STARBOARD,
  HYSCAN_SOURCE_LOOK_AROUND_PORT
};
guint32 orig_n_sources = sizeof (orig_sources) / sizeof (HyScanSourceType);

HyScanSourceType
get_master (HyScanSourceType source)
{
  if (source == HYSCAN_SOURCE_BATHYMETRY_STARBOARD)
    return HYSCAN_SOURCE_SIDE_SCAN_STARBOARD;
  if (source == HYSCAN_SOURCE_BATHYMETRY_PORT)
    return HYSCAN_SOURCE_SIDE_SCAN_PORT;
  if (source == HYSCAN_SOURCE_ECHOPROFILER)
    return HYSCAN_SOURCE_PROFILER;

  return HYSCAN_SOURCE_INVALID;
}

HyScanAntennaPosition *
create_sensor_position (const gchar *name,
                        gdouble      seed)
{
  guint index = g_str_hash (name) % N_SENSORS;
  HyScanAntennaPosition position;

  position.x = 1.0 * index * seed;
  position.y = 2.0 * index * seed;
  position.z = 3.0 * index * seed;
  position.psi = 4.0 * index * seed;
  position.gamma = 5.0 * index * seed;
  position.theta = 6.0 * index * seed;

  return hyscan_antenna_position_copy (&position);
}

HyScanSensorInfoSensor *
create_sensor (guint   index,
               gdouble seed)
{
  HyScanSensorInfoSensor *pinfo;
  HyScanSensorInfoSensor info;

  HyScanAntennaPosition position;

  position.x = 1.0 * index * seed;
  position.y = 2.0 * index * seed;
  position.z = 3.0 * index * seed;
  position.psi = 4.0 * index * seed;
  position.gamma = 5.0 * index * seed;
  position.theta = 6.0 * index * seed;

  info.name = g_strdup_printf ("nmea-%d", index);
  info.dev_id = g_strdup_printf ("nmea-%d", index);
  info.description = g_strdup_printf ("Nmea sensor %d", index);

  if (index % 2)
    info.position = &position;
  else
    info.position = NULL;

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
  HyScanSonarInfoSource *pinfo;
  HyScanSonarInfoSource info;

  HyScanSourceType master;
  HyScanAntennaPosition position;
  HyScanSonarInfoCapabilities capabilities;
  HyScanSonarInfoReceiver receiver;
  HyScanSonarInfoGenerator generator;
  HyScanSonarInfoTVG tvg;

  HyScanSonarInfoSignal tone;
  HyScanSonarInfoSignal lfm;
  GList *presets = NULL;

  const gchar *source_name;

  memset (&info, 0, sizeof (info));
  memset (&position, 0, sizeof (position));
  memset (&capabilities, 0, sizeof (capabilities));
  memset (&receiver, 0, sizeof (receiver));
  memset (&generator, 0, sizeof (generator));
  memset (&tvg, 0, sizeof (tvg));
  memset (&tone, 0, sizeof (tone));
  memset (&lfm, 0, sizeof (lfm));

  source_name = hyscan_source_get_name_by_type (source);
  seed = seed * source;

  /* Описание источника данных. */
  info.source = source;
  info.dev_id = source_name;
  info.description = source_name;

  /* Ведущий источник данных. */
  master = get_master (source);
  info.master = master;

  /* Местоположение антенн по умолчанию. */
  if (master == HYSCAN_SOURCE_INVALID)
    {
      position.x = -seed;
      position.y = seed;
      position.z = -seed / 2.0;
      position.psi = -seed * 2.0;
      position.gamma = seed / 2.0;
      position.theta = seed * 2.0;
      info.position = &position;
    }

  /* Режимы работы источника данных. */
  capabilities.receiver = HYSCAN_SONAR_RECEIVER_MODE_NONE;
  capabilities.generator = HYSCAN_SONAR_GENERATOR_MODE_NONE;
  capabilities.tvg = HYSCAN_SONAR_TVG_MODE_NONE;
  info.capabilities = &capabilities;

  /* Параметры приёмника. */
  if ((master == HYSCAN_SOURCE_INVALID) || (master == HYSCAN_SOURCE_PROFILER))
    {
      capabilities.receiver = HYSCAN_SONAR_RECEIVER_MODE_MANUAL;
      capabilities.receiver |= (source % 2) ? HYSCAN_SONAR_RECEIVER_MODE_AUTO : 0;

      receiver.min_time = -seed;
      receiver.max_time = seed;
      info.receiver = &receiver;
    }

  /* Параметры генератора. */
  if (master == HYSCAN_SOURCE_INVALID)
    {
      HyScanSonarGeneratorSignalType signals;
      guint n_presets = source;
      guint i;

      capabilities.generator = HYSCAN_SONAR_GENERATOR_MODE_PRESET;
      capabilities.generator |= (source % 2) ? HYSCAN_SONAR_GENERATOR_MODE_AUTO : 0;
      capabilities.generator |= (source % 3) ? HYSCAN_SONAR_GENERATOR_MODE_SIMPLE : 0;
      capabilities.generator |= (source % 4) ? HYSCAN_SONAR_GENERATOR_MODE_EXTENDED : 0;

      /* Типы сигналов. */
      signals = HYSCAN_SONAR_GENERATOR_SIGNAL_NONE;
      if (capabilities.generator != HYSCAN_SONAR_GENERATOR_MODE_PRESET)
        {
          signals |= (source % 2) ? HYSCAN_SONAR_GENERATOR_SIGNAL_AUTO : 0;
          signals |= (source % 3) ? HYSCAN_SONAR_GENERATOR_SIGNAL_TONE : 0;
          signals |= (source % 4) ? HYSCAN_SONAR_GENERATOR_SIGNAL_LFM : 0;

          /* Автоматический выбор сигнала. */
          generator.automatic = signals & HYSCAN_SONAR_GENERATOR_SIGNAL_AUTO;

          /* Параметры тонального сигнала. */
          if (signals & HYSCAN_SONAR_GENERATOR_SIGNAL_TONE)
            {
              tone.duration_name = "tone";
              tone.min_duration = seed / 10;
              tone.max_duration = seed * 10;
              tone.duration_step = seed;
              tone.dirty_cycle = seed;

              generator.tone = &tone;
            }

          /* Параметры ЛЧМ сигнала. */
          if (signals & HYSCAN_SONAR_GENERATOR_SIGNAL_LFM)
            {
              lfm.duration_name = "lfm";
              lfm.min_duration = seed / 2;
              lfm.max_duration = seed * 2;
              lfm.duration_step = seed;
              lfm.dirty_cycle = seed;

              generator.lfm = &lfm;
            }
        }

      /* Преднастройки генератора. */
      for (i = 0; i < n_presets; i++)
        {
          HyScanDataSchemaEnumValue *preset;
          gchar *name = g_strdup_printf ("%s preset %d", source_name, i + 1);
          gchar *description = g_strdup_printf ("%s description %d", source_name, i + 1);

          preset = hyscan_data_schema_enum_value_new (i, name, description);
          presets = g_list_append (presets, preset);

          g_free (description);
          g_free (name);
        }
      generator.presets = presets;

      info.generator = &generator;
    }

  /* Параметры ВАРУ. */
  if ((master == HYSCAN_SOURCE_INVALID) || (master == HYSCAN_SOURCE_PROFILER))
    {
      gboolean decrease;

      capabilities.tvg = HYSCAN_SONAR_TVG_MODE_AUTO;
      capabilities.tvg |= (source % 2) ? HYSCAN_SONAR_TVG_MODE_POINTS : 0;
      capabilities.tvg |= (source % 3) ? HYSCAN_SONAR_TVG_MODE_CONSTANT : 0;
      capabilities.tvg |= (source % 4) ? HYSCAN_SONAR_TVG_MODE_LINEAR_DB : 0;
      capabilities.tvg |= (source % 5) ? HYSCAN_SONAR_TVG_MODE_LOGARITHMIC : 0;
      decrease = (source % 2);

      if (capabilities.tvg != HYSCAN_SONAR_TVG_MODE_AUTO)
        {
          tvg.min_gain = -seed;
          tvg.max_gain = seed;
          tvg.decrease = decrease;

          info.tvg = &tvg;
        }
    }

  pinfo = hyscan_sonar_info_source_copy (&info);

  g_list_free_full (presets, (GDestroyNotify)hyscan_data_schema_enum_value_free);

  return pinfo;
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

  if ((sensor1->position != NULL) ||
      (sensor2->position != NULL))
    {
      if ((sensor1->position->x != sensor2->position->x) ||
          (sensor1->position->y != sensor2->position->y) ||
          (sensor1->position->z != sensor2->position->z) ||
          (sensor1->position->psi != sensor2->position->psi) ||
          (sensor1->position->gamma != sensor2->position->gamma) ||
          (sensor1->position->theta != sensor2->position->theta))
        {
          g_error ("position failed");
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

  /* Ведущий источник данных. */
  if (source1->master != source2->master)
    g_error ("master failed");

  /* Местоположение антенн по умолчанию. */
  if ((source1->position != NULL) ||
      (source2->position != NULL))
    {
      if ((source1->position->x != source2->position->x) ||
          (source1->position->y != source2->position->y) ||
          (source1->position->z != source2->position->z) ||
          (source1->position->psi != source2->position->psi) ||
          (source1->position->gamma != source2->position->gamma) ||
          (source1->position->theta != source2->position->theta))
        {
          g_error ("position failed");
        }
    }

  /* Режимы работы источника данных. */
  if ((source1->capabilities->receiver != source2->capabilities->receiver) ||
      (source1->capabilities->generator != source2->capabilities->generator) ||
      (source1->capabilities->tvg != source2->capabilities->tvg))
    {
      g_error ("capabilities failed");
    }

  /* Параметры приёмника. */
  if ((source1->receiver != NULL) ||
      (source2->receiver != NULL))
    {
      if ((source1->receiver->min_time != source2->receiver->min_time) ||
          (source1->receiver->max_time != source2->receiver->max_time))
        {
          g_error ("receiver failed");
        }
    }

  /* Параметры генератора. */
  if ((source1->generator != NULL) ||
      (source2->generator != NULL))
    {
      GList *orig_presets = source1->generator->presets;

      /* Преднастройки генератора. */
      while (orig_presets != NULL)
        {
          HyScanDataSchemaEnumValue *orig_preset = orig_presets->data;
          GList *presets = source2->generator->presets;
          gboolean status = FALSE;

          while (presets != NULL)
            {
              HyScanDataSchemaEnumValue *preset = presets->data;

              if ((orig_preset->value == preset->value) &&
                  (g_strcmp0 (orig_preset->name, preset->name) == 0) &&
                  (g_strcmp0 (orig_preset->description, preset->description) == 0))
                {
                  if (status)
                    g_error ("%s dup", orig_preset->name);

                  status = TRUE;
                }

              presets = g_list_next (presets);
            }
          if (!status)
            g_error ("%s failed", orig_preset->name);

          orig_presets = g_list_next (orig_presets);
        }

      /* Параметры сигналов. */
      if (source1->generator->signals != source2->generator->signals)
        g_error ("generator signals failed %d %d", source1->generator->signals, source2->generator->signals);

      if (source1->generator->automatic != source2->generator->automatic)
        g_error ("generator auto signal failed");

      if ((source1->generator->tone != NULL) ||
          (source2->generator->tone != NULL))
        {
          if ((g_strcmp0 (source1->generator->tone->duration_name, source2->generator->tone->duration_name) != 0) ||
              (source1->generator->tone->min_duration != source2->generator->tone->min_duration) ||
              (source1->generator->tone->max_duration != source2->generator->tone->max_duration) ||
              (source1->generator->tone->duration_step != source2->generator->tone->duration_step) ||
              (source1->generator->tone->dirty_cycle != source2->generator->tone->dirty_cycle))
            {
              g_error ("generator tone signal failed");
            }
        }

      if ((source1->generator->lfm != NULL) ||
          (source2->generator->lfm != NULL))
        {
          if ((g_strcmp0 (source1->generator->lfm->duration_name, source2->generator->lfm->duration_name) != 0) ||
              (source1->generator->lfm->min_duration != source2->generator->lfm->min_duration) ||
              (source1->generator->lfm->max_duration != source2->generator->lfm->max_duration) ||
              (source1->generator->lfm->duration_step != source2->generator->lfm->duration_step) ||
              (source1->generator->lfm->dirty_cycle != source2->generator->lfm->dirty_cycle))
            {
              g_error ("generator lfm signal failed");
            }
        }
    }

  /* Параметры ВАРУ. */
  if ((source1->tvg != NULL) ||
      (source2->tvg != NULL))
    {
      if ((source1->tvg->min_gain != source2->tvg->min_gain) ||
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
  HyScanDataSchemaBuilder *builder;
  HyScanSensorSchema *sensor_schema;
  HyScanSonarSchema *sonar_schema;
  HyScanSensorInfo *sensor_info;
  HyScanSonarInfo *sonar_info;
  HyScanDataSchema *schema;

  const HyScanSourceType *sources;
  const gchar * const *sensors;
  guint n_sources;
  gdouble seed;
  guint i, j;

  seed = 1000.0 * g_random_double ();

  builder = hyscan_data_schema_builder_new ("device");
  sensor_schema = hyscan_sensor_schema_new (builder);
  sonar_schema = hyscan_sonar_schema_new (builder);

  hyscan_sonar_schema_set_software_ping (sonar_schema);

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
        g_error ("can't add source %s", hyscan_source_get_name_by_type (orig_sources[i]));

      hyscan_sonar_info_source_free (source);
    }

  /* Создаём схему устройства. */
  schema = hyscan_data_schema_builder_get_schema (builder);
  sensor_info = hyscan_sensor_info_new (schema);
  sonar_info = hyscan_sonar_info_new (schema);

  /* Проверяем схему устройства. */
  if (!hyscan_sonar_info_get_software_ping (sonar_info))
    g_error ("software ping failed");

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

      g_message ("Check source %s", hyscan_source_get_name_by_type (orig_sources[i]));

      for (j = 0; j < n_sources; j++)
        if (orig_sources[i] == sources[j])
          break;

      if (j == n_sources)
        g_error ("list failed");

      verify_source (orig_source, source);

      hyscan_sonar_info_source_free (orig_source);
    }

  g_object_unref (schema);
  g_object_unref (sensor_schema);
  g_object_unref (sonar_schema);
  g_object_unref (sensor_info);
  g_object_unref (sonar_info);
  g_object_unref (builder);

  g_message ("All done");

  return 0;
}
