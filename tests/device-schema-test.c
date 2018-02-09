/* sonar-schema-test.c
 *
 * Copyright 2016-2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
  HYSCAN_SOURCE_PROFILER,
  HYSCAN_SOURCE_ECHOSOUNDER_HI,
  HYSCAN_SOURCE_BATHYMETRY_STARBOARD,
  HYSCAN_SOURCE_BATHYMETRY_PORT,
  HYSCAN_SOURCE_LOOK_AROUND_STARBOARD,
  HYSCAN_SOURCE_LOOK_AROUND_PORT,
  HYSCAN_SOURCE_INVALID
};

HyScanSourceType
get_master (HyScanSourceType source)
{
  if (source == HYSCAN_SOURCE_BATHYMETRY_STARBOARD)
    return HYSCAN_SOURCE_SIDE_SCAN_STARBOARD;
  if (source == HYSCAN_SOURCE_BATHYMETRY_PORT)
    return HYSCAN_SOURCE_SIDE_SCAN_PORT;
  if (source == HYSCAN_SOURCE_ECHOSOUNDER_HI)
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

HyScanSonarInfoSource *
create_source (HyScanSourceType source,
               gdouble          seed)
{
  HyScanSonarInfoSource *pinfo;
  HyScanSonarInfoSource info;

  HyScanSourceType master;
  HyScanAntennaPosition position;
  HyScanSonarInfoCapabilities capabilities;
  HyScanSonarInfoAntenna antenna;
  HyScanSonarInfoReceiver receiver;
  HyScanSonarInfoGenerator generator;
  HyScanSonarInfoTVG tvg;

  HyScanSonarInfoSignal tone;
  HyScanSonarInfoSignal lfm;
  GList *channels = NULL;
  GList *presets = NULL;

  const gchar *source_name;

  memset (&info, 0, sizeof (info));
  memset (&position, 0, sizeof (position));
  memset (&capabilities, 0, sizeof (capabilities));
  memset (&antenna, 0, sizeof (antenna));
  memset (&receiver, 0, sizeof (receiver));
  memset (&generator, 0, sizeof (generator));
  memset (&tvg, 0, sizeof (tvg));
  memset (&tone, 0, sizeof (tone));
  memset (&lfm, 0, sizeof (lfm));

  source_name = hyscan_source_get_name_by_type (source);
  seed = seed * source;

  /* Описание источника данных. */
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

  /* Параметры антенны. */
  if ((master == HYSCAN_SOURCE_INVALID) || (master == HYSCAN_SOURCE_PROFILER))
    {
      antenna.vpattern = -seed;
      antenna.vpattern = seed;
      antenna.frequency = seed / 2.0;
      antenna.bandwidth = seed * 2.0;
      info.antenna = &antenna;
    }

  /* Параметры приёмника. */
  if ((master == HYSCAN_SOURCE_INVALID) || (master == HYSCAN_SOURCE_PROFILER))
    {
      capabilities.receiver = HYSCAN_SONAR_RECEIVER_MODE_MANUAL;
      capabilities.receiver |= (source % 2) ? HYSCAN_SONAR_RECEIVER_MODE_AUTO : 0;

      receiver.min_time = -seed;
      receiver.max_time = seed;
      info.receiver = &receiver;
    }

  /* Параметры приёмных каналов. */
  if ((master == HYSCAN_SOURCE_INVALID) || (master == HYSCAN_SOURCE_PROFILER))
    {
      guint n_channels = 1;
      guint i;

      if ((source == HYSCAN_SOURCE_SIDE_SCAN_STARBOARD) || (source == HYSCAN_SOURCE_SIDE_SCAN_PORT))
        n_channels = 3;

      for (i = 1; i <= n_channels; i++)
        {
          HyScanSonarInfoChannel channel;

          channel.channel = i;
          channel.antenna_voffset = -seed * i;
          channel.antenna_hoffset = seed * i;
          channel.adc_offset = seed * i / 2.0;
          channel.adc_vref = seed * i * 2.0;

          channels = g_list_append (channels, hyscan_sonar_info_channel_copy (&channel));
        }
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
              tone.min_duration = -seed;
              tone.max_duration = seed;
              tone.dirty_cycle = seed / 2.0;

              generator.tone = &tone;
            }

          /* Параметры ЛЧМ сигнала. */
          if (signals & HYSCAN_SONAR_GENERATOR_SIGNAL_LFM)
            {
              lfm.min_duration = seed;
              lfm.max_duration = -seed;
              lfm.dirty_cycle = seed * 2.0;

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
      gboolean can_decrease;

      capabilities.tvg = HYSCAN_SONAR_TVG_MODE_AUTO;
      capabilities.tvg |= (source % 2) ? HYSCAN_SONAR_TVG_MODE_POINTS : 0;
      capabilities.tvg |= (source % 3) ? HYSCAN_SONAR_TVG_MODE_LINEAR_DB : 0;
      capabilities.tvg |= (source % 4) ? HYSCAN_SONAR_TVG_MODE_LOGARITHMIC : 0;
      can_decrease = (source % 2);

      if (capabilities.tvg != HYSCAN_SONAR_TVG_MODE_AUTO)
        {
          tvg.min_gain = -seed;
          tvg.max_gain = seed;
          tvg.can_decrease = can_decrease;

          info.tvg = &tvg;
        }
    }

  pinfo = hyscan_sonar_info_source_copy (&info);

  g_list_free_full (channels, (GDestroyNotify)hyscan_sonar_info_channel_free);
  g_list_free_full (presets, (GDestroyNotify)hyscan_data_schema_enum_value_free);

  return pinfo;
}

gboolean
verify_source (const HyScanSonarInfoSource *orig_source,
               const HyScanSonarInfoSource *orig_master,
               const HyScanSonarInfoSource *source)
{
  HyScanSonarInfoAntenna *antenna;

  if ((orig_source == NULL) || (source == NULL))
    return FALSE;

  /* Описание источника данных. */
  if (g_strcmp0 (orig_source->description, source->description) != 0)
    {
      g_message ("description failed");
      return FALSE;
    }

  /* Ведущий источник данных. */
  if (orig_source->master != source->master)
    {
      g_message ("master failed");
      return FALSE;
    }

  /* Местоположение антенн по умолчанию. */
  if (orig_source->position != NULL)
    {
      if ((orig_source->position->x != source->position->x) ||
          (orig_source->position->y != source->position->y) ||
          (orig_source->position->z != source->position->z) ||
          (orig_source->position->psi != source->position->psi) ||
          (orig_source->position->gamma != source->position->gamma) ||
          (orig_source->position->theta != source->position->theta))
        {
          g_message ("position failed");
          return FALSE;
        }
    }

  /* Режимы работы источника данных. */
  if ((orig_source->capabilities->receiver != source->capabilities->receiver) ||
      (orig_source->capabilities->generator != source->capabilities->generator) ||
      (orig_source->capabilities->tvg != source->capabilities->tvg))
    {
      g_message ("capabilities failed");
      return FALSE;
    }

  /* Параметры антенны. */
  if (orig_source->antenna != NULL)
    antenna = orig_source->antenna;
  else
    antenna = orig_master->antenna;

  if ((antenna->vpattern != source->antenna->vpattern) ||
      (antenna->hpattern != source->antenna->hpattern) ||
      (antenna->frequency != source->antenna->frequency) ||
      (antenna->bandwidth != source->antenna->bandwidth))
    {
      g_message ("antenna failed");
      return FALSE;
    }

  /* Параметры приёмника. */
  if (orig_source->receiver != NULL)
    {
      if ((orig_source->receiver->min_time != source->receiver->min_time) ||
          (orig_source->receiver->max_time != source->receiver->max_time))
        {
          g_message ("receiver failed");
          return FALSE;
        }
    }

  /* Параметры приёмных каналов. */
  if (orig_source->channels != NULL)
    {
      GList *orig_channels = orig_source->channels;

      while (orig_channels != NULL)
        {
          HyScanSonarInfoChannel *orig_channel = orig_channels->data;
          GList *channels = source->channels;
          gboolean status = FALSE;

          while (channels != NULL)
            {
              HyScanSonarInfoChannel *channel = channels->data;

              if ((orig_channel->channel == channel->channel) &&
                  (orig_channel->antenna_voffset == channel->antenna_voffset) &&
                  (orig_channel->antenna_hoffset == channel->antenna_hoffset) &&
                  (orig_channel->adc_offset == channel->adc_offset) &&
                  (orig_channel->adc_vref == channel->adc_vref))
                {
                  if (status)
                    {
                      g_message ("channel %d dup", orig_channel->channel);
                      return FALSE;
                    }

                  status = TRUE;
                }

              channels = g_list_next (channels);
            }
          if (!status)
            {
              g_message ("channel %d failed", orig_channel->channel);
              return FALSE;
            }

          orig_channels = g_list_next (orig_channels);
        }
    }

  /* Параметры генератора. */
  if (orig_source->generator != NULL)
    {
      GList *orig_presets = orig_source->generator->presets;

      /* Преднастройки генератора. */
      while (orig_presets != NULL)
        {
          HyScanDataSchemaEnumValue *orig_preset = orig_presets->data;
          GList *presets = source->generator->presets;
          gboolean status = FALSE;

          while (presets != NULL)
            {
              HyScanDataSchemaEnumValue *preset = presets->data;

              if ((orig_preset->value == preset->value) &&
                  (g_strcmp0 (orig_preset->name, preset->name) == 0) &&
                  (g_strcmp0 (orig_preset->description, preset->description) == 0))
                {
                  if (status)
                    {
                      g_message ("%s dup", orig_preset->name);
                      return FALSE;
                    }

                  status = TRUE;
                }

              presets = g_list_next (presets);
            }
          if (!status)
            {
              g_message ("%s failed", orig_preset->name);
              return FALSE;
            }

          orig_presets = g_list_next (orig_presets);
        }

      /* Параметры сигналов. */
      if (orig_source->generator->signals != source->generator->signals)
        {
          g_message ("generator signals failed %d %d", orig_source->generator->signals, source->generator->signals);
          return FALSE;
        }

      if (orig_source->generator->automatic != source->generator->automatic)
        {
          g_message ("generator auto signal failed");
          return FALSE;
        }

      if (orig_source->generator->tone != NULL)
        {
          if ((orig_source->generator->tone->min_duration != source->generator->tone->min_duration) ||
              (orig_source->generator->tone->max_duration != source->generator->tone->max_duration) ||
              (orig_source->generator->tone->dirty_cycle != source->generator->tone->dirty_cycle))
            {
              g_message ("generator tone signal failed");
              return FALSE;
            }
        }

      if (orig_source->generator->lfm != NULL)
        {
          if ((orig_source->generator->lfm->min_duration != source->generator->lfm->min_duration) ||
              (orig_source->generator->lfm->max_duration != source->generator->lfm->max_duration) ||
              (orig_source->generator->lfm->dirty_cycle != source->generator->lfm->dirty_cycle))
            {
              g_message ("generator lfm signal failed");
              return FALSE;
            }
        }
    }

  /* Параметры ВАРУ. */
  if (orig_source->tvg != NULL)
    {
      if ((orig_source->tvg->min_gain != source->tvg->min_gain) ||
          (orig_source->tvg->max_gain != source->tvg->max_gain) ||
          (orig_source->tvg->can_decrease != source->tvg->can_decrease))
        {
          g_message ("tvg failed");
          return FALSE;
        }
    }

  return TRUE;
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
  gchar *schema_data;

  const HyScanSourceType *sources;
  const gchar * const * sensors;
  guint n_sources;
  gdouble seed;
  guint i;

  seed = g_random_double ();

  builder = hyscan_data_schema_builder_new ("device");
  sensor_schema = hyscan_sensor_schema_new (builder);
  sonar_schema = hyscan_sonar_schema_new (builder);

  hyscan_sonar_schema_set_software_ping (sonar_schema);

  /* Создаём датчики. */
  for (i = 0; i < N_SENSORS; i++)
    {
      gchar *sensor = g_strdup_printf ("nmea-%d", i);
      gchar *description = g_strdup_printf ("Nmea sensor %d", i);

      if (!hyscan_sensor_schema_add_sensor (sensor_schema, sensor, description))
        g_error ("can't add sensor %s", sensor);

      if (i % 2)
        {
          HyScanAntennaPosition *position;
          position = create_sensor_position (sensor, seed);

          if (!hyscan_sensor_schema_set_position (sensor_schema, sensor, position))
            g_error ("can't set sensor %s position", sensor);

          hyscan_antenna_position_free (position);
        }

      g_free (description);
      g_free (sensor);
    }

  /* Создаём источники данных. */
  for (i = 0; orig_sources[i] != HYSCAN_SOURCE_INVALID; i++)
    {
      HyScanSonarInfoSource *source = create_source (orig_sources[i], seed);

      if (!hyscan_sonar_schema_source_add_full (sonar_schema, orig_sources[i], source))
        g_error ("can't add source %s", hyscan_source_get_name_by_type (orig_sources[i]));

      hyscan_sonar_info_source_free (source);
    }

  /* Создаём схему устройства. */
  schema_data = hyscan_data_schema_builder_get_data (builder);
  schema = hyscan_data_schema_new_from_string (schema_data, "device");
  g_free (schema_data);

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
      gchar *sensor = g_strdup_printf ("nmea-%d", i);
      gchar *orig_description = g_strdup_printf ("Nmea sensor %d", i);
      const gchar *description;

      g_message ("Check sensor %s", sensor);

      description = hyscan_sensor_info_get_description (sensor_info, sensor);
      if (g_strcmp0 (orig_description, description) != 0)
        g_error ("decripton failed");

      if (i % 2)
        {
          HyScanAntennaPosition *orig_position;
          const HyScanAntennaPosition *position;

          orig_position = create_sensor_position (sensor, seed);
          position = hyscan_sensor_info_get_position (sensor_info, sensor);

          if ((orig_position->x != position->x) ||
              (orig_position->y != position->y) ||
              (orig_position->z != position->z) ||
              (orig_position->psi != position->psi) ||
              (orig_position->gamma != position->gamma) ||
              (orig_position->theta != position->theta))
            {
              g_error ("position failed");
            }

          hyscan_antenna_position_free (orig_position);
        }

      g_free (orig_description);
      g_free (sensor);
    }

  /* Проверяем список источников. */
  sources = hyscan_sonar_info_list_sources (sonar_info, &n_sources);
  if (n_sources != ((sizeof (orig_sources) / sizeof (HyScanSourceType)) - 1))
    g_error ("n_sources mismatch");

  /* Проверяем параметры источников данных. */
  for (i = 0; i < n_sources; i++)
    {
      HyScanSonarInfoSource *orig_source;
      HyScanSonarInfoSource *orig_master;
      const HyScanSonarInfoSource *source;

      orig_source = create_source (sources[i], seed);
      orig_master = create_source (get_master (sources[i]), seed);
      source = hyscan_sonar_info_get_source (sonar_info, sources[i]);

      g_message ("Check source %s", hyscan_source_get_name_by_type (sources[i]));
      if (!verify_source (orig_source, orig_master, source))
        g_error ("failed");

      hyscan_sonar_info_source_free (orig_master);
      hyscan_sonar_info_source_free (orig_source);
    }

  g_object_unref (schema);
  g_object_unref (sensor_schema);
  g_object_unref (sonar_schema);
  g_object_unref (sensor_info);
  g_object_unref (sonar_info);
  g_object_unref (builder);
  xmlCleanupParser ();

  g_message ("All done");

  return 0;
}
