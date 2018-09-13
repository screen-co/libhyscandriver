/* hyscan-sonar-info.c
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
 * SECTION: hyscan-sonar-info
 * @Short_description: класс с информацией о гидролокаторе
 * @Title: HyScanSonarInfo
 *
 * Класс предназначен для получения информации о гидролокаторе по его схеме.
 * Для создания объекта используется функция #hyscan_sonar_info_new, в
 * которую передаётся схема устройства.
 *
 * Определить возможность программного управления излучением можно с помощью
 * функции #hyscan_sonar_info_get_software_ping.
 *
 * Список источников данных можно узнать с помощью функции
 * #hyscan_sonar_info_list_sources.
 *
 * Информацию об источнике данных можно получить с помощью функции
 * #hyscan_sonar_info_get_source.
 */

#include "hyscan-sonar-info.h"
#include "hyscan-device-schema.h"
#include "hyscan-sonar-schema.h"
#include "hyscan-discover.h"
#include <hyscan-param.h>

#define SONAR_PARAM_NAME(source,prefix,param)                    hyscan_sonar_info_sonar_param_name (name_buffer, \
                                                                 (gulong)sizeof (name_buffer), \
                                                                 hyscan_source_get_name_by_type (source), \
                                                                 prefix, \
                                                                 param)

enum
{
  PROP_O,
  PROP_SCHEMA
};

struct _HyScanSonarInfoPrivate
{
  HyScanDataSchema            *schema;         /* Схема устройства. */
  GHashTable                  *sources;        /* Параметры источников данных. */
  GArray                      *sources_list;   /* Список источников данных. */
  gboolean                     software_ping;  /* Признак программного управления излучением. */
};

static void            hyscan_sonar_info_set_property          (GObject               *object,
                                                                guint                  prop_id,
                                                                const GValue          *value,
                                                                GParamSpec            *pspec);
static void            hyscan_sonar_info_object_constructed    (GObject               *object);
static void            hyscan_sonar_info_object_finalize       (GObject               *object);

static const gchar *   hyscan_sonar_info_sonar_param_name      (gchar                 *buffer,
                                                                guint                  size,
                                                                const gchar           *source,
                                                                const gchar           *prefix,
                                                                const gchar           *param);

static HyScanSourceType *
                       hyscan_sonar_info_list_sources_int      (HyScanDataSchema      *schema,
                                                                guint32               *n_sources);

static HyScanAntennaPosition *
                       hyscan_sonar_info_parse_position        (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static HyScanSonarInfoCapabilities *
                       hyscan_sonar_info_parse_capabilities    (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static GList *
                       hyscan_sonar_info_parse_presets         (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static HyScanSonarInfoSignal *
                       hyscan_sonar_info_parse_signal          (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source,
                                                                const gchar           *name);

static HyScanSonarInfoReceiver *
                       hyscan_sonar_info_parse_receiver        (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static HyScanSonarInfoGenerator *
                       hyscan_sonar_info_parse_generator       (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static HyScanSonarInfoTVG *
                       hyscan_sonar_info_parse_tvg             (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static HyScanSonarInfoSource *
                       hyscan_sonar_info_parse_source          (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static GHashTable *    hyscan_sonar_info_parse_sources         (HyScanDataSchema      *schema);

G_DEFINE_BOXED_TYPE (HyScanSonarInfoSource, hyscan_sonar_info_source,
                     hyscan_sonar_info_source_copy, hyscan_sonar_info_source_free)

G_DEFINE_BOXED_TYPE (HyScanSonarInfoCapabilities, hyscan_sonar_info_capabilities,
                     hyscan_sonar_info_capabilities_copy, hyscan_sonar_info_capabilities_free)

G_DEFINE_BOXED_TYPE (HyScanSonarInfoReceiver, hyscan_sonar_info_receiver,
                     hyscan_sonar_info_receiver_copy, hyscan_sonar_info_receiver_free)

G_DEFINE_BOXED_TYPE (HyScanSonarInfoGenerator, hyscan_sonar_generator_tvg,
                     hyscan_sonar_info_generator_copy, hyscan_sonar_info_generator_free)

G_DEFINE_BOXED_TYPE (HyScanSonarInfoTVG, hyscan_sonar_info_tvg,
                     hyscan_sonar_info_tvg_copy, hyscan_sonar_info_tvg_free)

G_DEFINE_BOXED_TYPE (HyScanSonarInfoSignal, hyscan_sonar_info_signal,
                     hyscan_sonar_info_signal_copy, hyscan_sonar_info_signal_free)

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSonarInfo, hyscan_sonar_info, G_TYPE_OBJECT)

static void
hyscan_sonar_info_class_init (HyScanSonarInfoClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_sonar_info_set_property;

  object_class->constructed = hyscan_sonar_info_object_constructed;
  object_class->finalize = hyscan_sonar_info_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "Sonar schema", HYSCAN_TYPE_DATA_SCHEMA,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_sonar_info_init (HyScanSonarInfo *info)
{
  info->priv = hyscan_sonar_info_get_instance_private (info);
}

static void
hyscan_sonar_info_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  HyScanSonarInfo *info = HYSCAN_SONAR_INFO (object);
  HyScanSonarInfoPrivate *priv = info->priv;

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
hyscan_sonar_info_object_constructed (GObject *object)
{
  HyScanSonarInfo *info = HYSCAN_SONAR_INFO (object);
  HyScanSonarInfoPrivate *priv = info->priv;

  GHashTableIter iter;
  gpointer key, value;

  /* Должна быть задана схема гидролокатора. */
  if (priv->schema == NULL)
    return;

  /* Верифицируем схему устройства. */
  if (!hyscan_device_schema_check_id (priv->schema))
    {
      g_warning ("HyScanSonarInfo: unsupported device schema");
      return;
    }

  /* Признак программного управления излучением. */
  if (!hyscan_device_schema_get_boolean (priv->schema, "/sources/software-ping", &priv->software_ping))
    priv->software_ping = FALSE;

  /* Параметры источников данных. */
  priv->sources = hyscan_sonar_info_parse_sources (priv->schema);

  /* Список источников данных. */
  priv->sources_list = g_array_new (FALSE, FALSE, sizeof (HyScanSourceType));
  g_hash_table_iter_init (&iter, priv->sources);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      HyScanSourceType source = GPOINTER_TO_INT (key);
      g_array_append_val (priv->sources_list, source);
    }
}

static void
hyscan_sonar_info_object_finalize (GObject *object)
{
  HyScanSonarInfo *info = HYSCAN_SONAR_INFO (object);
  HyScanSonarInfoPrivate *priv = info->priv;

  g_clear_object (&priv->schema);
  g_clear_pointer (&priv->sources, g_hash_table_unref);
  g_clear_pointer (&priv->sources_list, g_array_unref);

  G_OBJECT_CLASS (hyscan_sonar_info_parent_class)->finalize (object);
}

/* Функция формирует название параметра гидролокатора. */
static const gchar *
hyscan_sonar_info_sonar_param_name (gchar       *buffer,
                                    guint        size,
                                    const gchar *source,
                                    const gchar *prefix,
                                    const gchar *param)
{
  if (prefix != NULL)
    {
      g_snprintf (buffer, size, "/sources/%s/%s/%s",
                  source, prefix, param);
    }
  else
    {
      g_snprintf (buffer, size, "/sources/%s/%s",
                  source, param);
    }

  return buffer;
}

/* Функуция возвращает список источников гидролокационных данных. */
HyScanSourceType *
hyscan_sonar_info_list_sources_int (HyScanDataSchema *schema,
                                    guint32          *n_sources)
{
  const gchar * const *keys;
  GArray *sources;
  guint i;

  keys = hyscan_data_schema_list_keys (schema);
  if (keys == NULL)
    return NULL;

  sources = g_array_new (FALSE, FALSE, sizeof (HyScanSourceType));

  for (i = 0; keys[i] != NULL; i++)
    {
      gchar **keyv = g_strsplit (keys[i], "/", -1);

      /* Ищем параметр /sources/source-name/dev-id */
      if (g_strv_length (keyv) != 4)
        {
          g_strfreev (keyv);
          continue;
        }

      if ((g_strcmp0 (keyv[1], "sources") == 0) && (g_strcmp0 (keyv[3], "dev-id") == 0))
        {
          HyScanSourceType source = hyscan_source_get_type_by_name (keyv[2]);

          if (!hyscan_source_is_sonar (source))
            continue;

          g_array_append_val (sources, source);
        }

      g_strfreev (keyv);
    }

  if (sources->len == 0)
    {
      g_array_unref (sources);
      return NULL;
    }

  *n_sources = sources->len;
  return (HyScanSourceType *)g_array_free (sources, FALSE);
}

/* Функция считывает информацию о местоположении антенн. */
static HyScanAntennaPosition *
hyscan_sonar_info_parse_position (HyScanDataSchema *schema,
                                  HyScanSourceType  source)
{
  HyScanAntennaPosition info;
  gchar name_buffer[128];
  const gchar *name;

  name = SONAR_PARAM_NAME (source, "position", "x");
  if (!hyscan_device_schema_get_double (schema, name, NULL, NULL, &info.x, NULL))
    return NULL;

  name = SONAR_PARAM_NAME (source, "position", "y");
  if (!hyscan_device_schema_get_double (schema, name, NULL, NULL, &info.y, NULL))
    return NULL;

  name = SONAR_PARAM_NAME (source, "position", "z");
  if (!hyscan_device_schema_get_double (schema, name, NULL, NULL, &info.z, NULL))
    return NULL;

  name = SONAR_PARAM_NAME (source, "position", "psi");
  if (!hyscan_device_schema_get_double (schema, name, NULL, NULL, &info.psi, NULL))
    return NULL;

  name = SONAR_PARAM_NAME (source, "position", "gamma");
  if (!hyscan_device_schema_get_double (schema, name, NULL, NULL, &info.gamma, NULL))
    return NULL;

  name = SONAR_PARAM_NAME (source, "position", "theta");
  if (!hyscan_device_schema_get_double (schema, name, NULL, NULL, &info.theta, NULL))
    return NULL;

  return hyscan_antenna_position_copy (&info);
}

/* Функция считывает информацию о режимах работы источника данных. */
static HyScanSonarInfoCapabilities *
hyscan_sonar_info_parse_capabilities (HyScanDataSchema *schema,
                                      HyScanSourceType  source)
{
  HyScanSonarInfoCapabilities info;
  gchar name_buffer[128];
  const gchar *name;

  const gchar *cap_string;
  gchar **caps;
  guint i;

  info.receiver = HYSCAN_SONAR_RECEIVER_MODE_NONE;
  info.generator = HYSCAN_SONAR_GENERATOR_MODE_NONE;
  info.tvg = HYSCAN_SONAR_TVG_MODE_NONE;

  /* Режимы работы приёмника. */
  name = SONAR_PARAM_NAME (source, "receiver", "capabilities");
  cap_string = hyscan_device_schema_get_string (schema, name);
  if (cap_string != NULL)
    {
      caps = g_strsplit (cap_string, " ", -1);
      for (i = 0; (caps != NULL) && (caps[i] != NULL); i++)
        {
          if (g_ascii_strcasecmp (caps[i], "manual") == 0)
            info.receiver |= HYSCAN_SONAR_RECEIVER_MODE_MANUAL;

          if (g_ascii_strcasecmp (caps[i], "auto") == 0)
            info.receiver |= HYSCAN_SONAR_RECEIVER_MODE_AUTO;
        }
      g_strfreev (caps);
    }

  /* Режимы работы генератора. */
  name = SONAR_PARAM_NAME (source, "generator", "capabilities");
  cap_string = hyscan_device_schema_get_string (schema, name);
  if (cap_string != NULL)
    {
      caps = g_strsplit (cap_string, " ", -1);
      for (i = 0; (caps != NULL) && (caps[i] != NULL); i++)
        {
          if (g_ascii_strcasecmp (caps[i], "preset") == 0)
            info.generator |= HYSCAN_SONAR_GENERATOR_MODE_PRESET;

          if (g_ascii_strcasecmp (caps[i], "auto") == 0)
            info.generator |= HYSCAN_SONAR_GENERATOR_MODE_AUTO;

          if (g_ascii_strcasecmp (caps[i], "simple") == 0)
            info.generator |= HYSCAN_SONAR_GENERATOR_MODE_SIMPLE;

          if (g_ascii_strcasecmp (caps[i], "extended") == 0)
            info.generator |= HYSCAN_SONAR_GENERATOR_MODE_EXTENDED;
        }
      g_strfreev (caps);
    }

  /* Режимы работы ВАРУ. */
  name = SONAR_PARAM_NAME (source, "tvg", "capabilities");
  cap_string = hyscan_device_schema_get_string (schema, name);
  if (cap_string != NULL)
    {
      caps = g_strsplit (cap_string, " ", -1);
      for (i = 0; (caps != NULL) && (caps[i] != NULL); i++)
        {
          if (g_ascii_strcasecmp (caps[i], "auto") == 0)
            info.tvg |= HYSCAN_SONAR_TVG_MODE_AUTO;

          if (g_ascii_strcasecmp (caps[i], "points") == 0)
            info.tvg |= HYSCAN_SONAR_TVG_MODE_POINTS;

          if (g_ascii_strcasecmp (caps[i], "constant") == 0)
            info.tvg |= HYSCAN_SONAR_TVG_MODE_CONSTANT;

          if (g_ascii_strcasecmp (caps[i], "linear-db") == 0)
            info.tvg |= HYSCAN_SONAR_TVG_MODE_LINEAR_DB;

          if (g_ascii_strcasecmp (caps[i], "logarithmic") == 0)
            info.tvg |= HYSCAN_SONAR_TVG_MODE_LOGARITHMIC;
        }
      g_strfreev (caps);
    }

  return hyscan_sonar_info_capabilities_copy (&info);
}

/* Функция считывает информацию о приёмнике. */
static HyScanSonarInfoReceiver *
hyscan_sonar_info_parse_receiver (HyScanDataSchema *schema,
                                  HyScanSourceType  source)
{
  HyScanSonarInfoReceiver info;
  gchar name_buffer[128];
  const gchar *name;
  gboolean status;

  /* Основная информация. */
  name = SONAR_PARAM_NAME (source, "receiver", "time");
  status = hyscan_device_schema_get_double (schema, name, &info.min_time,
                                                          &info.max_time,
                                                          NULL,
                                                          NULL);
  if (!status)
    return NULL;

  return hyscan_sonar_info_receiver_copy (&info);
}

/* Функция считывает информацию о преднастрйках генератора. */
static GList *
hyscan_sonar_info_parse_presets (HyScanDataSchema *schema,
                                 HyScanSourceType  source)
{
  GList *presets = NULL;
  const gchar * const *keys;
  gchar prefix[128];
  guint i;

  g_snprintf (prefix, sizeof (prefix),
              "/sources/%s/generator/presets/",
              hyscan_source_get_name_by_type (source));

  keys = hyscan_data_schema_list_keys (schema);
  for (i = 0; keys[i] != NULL; i++)
    {
      HyScanDataSchemaEnumValue preset;
      gint64 id;

      if (!g_str_has_prefix (keys[i], prefix))
        continue;

      if (!hyscan_device_schema_get_integer (schema, keys[i], NULL, NULL, &id, NULL))
        continue;

      preset.value = id;
      preset.name = hyscan_data_schema_key_get_name (schema, keys[i]);
      preset.description = hyscan_data_schema_key_get_description (schema, keys[i]);

      presets = g_list_prepend (presets, hyscan_data_schema_enum_value_copy (&preset));
    }

  return presets;
}

/* Функция считывает информацию о сигнале. */
static HyScanSonarInfoSignal *
hyscan_sonar_info_parse_signal (HyScanDataSchema *schema,
                                HyScanSourceType  source,
                                const gchar      *signal)
{
  HyScanSonarInfoSignal info;
  gchar name_buffer[128];
  const gchar *name;
  gchar prefix[128];

  gboolean status;

  g_snprintf (prefix, sizeof (prefix), "generator/%s", signal);

  name = SONAR_PARAM_NAME (source, prefix, "duration");
  status = hyscan_device_schema_get_double (schema, name, &info.min_duration,
                                                          &info.max_duration,
                                                          NULL,
                                                          &info.duration_step);
  if (!status)
    return NULL;

  info.duration_name = hyscan_data_schema_key_get_description (schema, name);
  if (info.duration_name == NULL)
    return NULL;

  name = SONAR_PARAM_NAME (source, prefix, "dirty-cycle");
  status = hyscan_device_schema_get_double (schema, name, NULL,
                                                          NULL,
                                                          &info.dirty_cycle,
                                                          NULL);
  if (!status)
    return NULL;


  return hyscan_sonar_info_signal_copy (&info);
}

/* Функция считывает информацию о генераторе. */
static HyScanSonarInfoGenerator *
hyscan_sonar_info_parse_generator (HyScanDataSchema *schema,
                                   HyScanSourceType  source)
{
  HyScanSonarInfoGenerator *info;
  gchar name_buffer[128];
  const gchar *name;

  gboolean automatic;
  HyScanSonarInfoSignal *tone;
  HyScanSonarInfoSignal *lfm;
  GList *presets;

  /* Автоматический выбор сигнала. */
  name = SONAR_PARAM_NAME (source, "generator", "automatic");
  if (!hyscan_device_schema_get_boolean (schema, name, &automatic))
    automatic = FALSE;

  /* Параметры тонального сигнала. */
  tone = hyscan_sonar_info_parse_signal (schema, source, "tone");

  /* Параметры ЛЧМ сигнала. */
  lfm = hyscan_sonar_info_parse_signal (schema, source, "lfm");

  /* Преднастройки генератора. */
  presets = hyscan_sonar_info_parse_presets (schema, source);

  if (!automatic && (tone == NULL) && (lfm == NULL) && (presets == NULL))
    return NULL;

  info = g_slice_new0 (HyScanSonarInfoGenerator);

  info->signals = HYSCAN_SONAR_GENERATOR_SIGNAL_NONE;
  info->signals |= automatic ? HYSCAN_SONAR_GENERATOR_SIGNAL_AUTO : 0;
  info->signals |= (tone != NULL) ? HYSCAN_SONAR_GENERATOR_SIGNAL_TONE : 0;
  info->signals |= (lfm != NULL) ? HYSCAN_SONAR_GENERATOR_SIGNAL_LFM : 0;

  info->presets = presets;
  info->automatic = automatic;
  info->tone = tone;
  info->lfm = lfm;

  return info;
}

/* Функция считывает информацию о системе ВАРУ. */
static HyScanSonarInfoTVG *
hyscan_sonar_info_parse_tvg (HyScanDataSchema *schema,
                             HyScanSourceType  source)
{
  HyScanSonarInfoTVG info;
  gchar name_buffer[128];
  const gchar *name;
  gboolean status;

  name = SONAR_PARAM_NAME (source, "tvg", "gain");
  status = hyscan_device_schema_get_double (schema, name, &info.min_gain,
                                                          &info.max_gain,
                                                          NULL,
                                                          NULL);
  if (!status)
    return NULL;

  name = SONAR_PARAM_NAME (source, "tvg", "decrease");
  if (!hyscan_device_schema_get_boolean (schema, name, &info.decrease))
    info.decrease = FALSE;

  return hyscan_sonar_info_tvg_copy (&info);
}

/* Функция считывает информацию о гидролокационном источнике данных. */
static HyScanSonarInfoSource *
hyscan_sonar_info_parse_source (HyScanDataSchema *schema,
                                HyScanSourceType  source)
{
  gchar name_buffer[128];
  const gchar *name;

  HyScanSonarInfoSource *info = NULL;
  const gchar *dev_id = NULL;
  const gchar *description = NULL;
  gint64 master = HYSCAN_SOURCE_INVALID;
  HyScanAntennaPosition *position = NULL;
  HyScanSonarInfoCapabilities *capabilities = NULL;
  HyScanSonarInfoReceiver *receiver = NULL;
  HyScanSonarInfoGenerator *generator = NULL;
  HyScanSonarInfoTVG *tvg = NULL;

  /* уникальный идентификатор устройства. */
  name = SONAR_PARAM_NAME (source, NULL, "dev-id");
  dev_id = hyscan_device_schema_get_string (schema, name);
  if (dev_id == NULL)
    goto fail;

  /* Описание источника данных. */
  name = SONAR_PARAM_NAME (source, NULL, "description");
  description = hyscan_device_schema_get_string (schema, name);

  /* Ведущий источник данных. */
  name = SONAR_PARAM_NAME (source, NULL, "master");
  master = hyscan_source_get_type_by_name (hyscan_device_schema_get_string (schema, name));

  /* Местоположение антенн по умолчанию. */
  position = hyscan_sonar_info_parse_position (schema, source);

  /* Режимы работы источника данных. */
  capabilities = hyscan_sonar_info_parse_capabilities (schema, source);
  if (capabilities == NULL)
    goto fail;

  /* Параметры приёмника. */
  if (capabilities->receiver & HYSCAN_SONAR_RECEIVER_MODE_MANUAL)
    {
      receiver = hyscan_sonar_info_parse_receiver (schema, source);
      if (receiver == NULL)
        goto fail;
    }

  /* Параметры генератора. */
  if (capabilities->generator)
    {
      generator = hyscan_sonar_info_parse_generator (schema, source);
      if (generator == NULL)
        goto fail;

      if ((capabilities->generator != HYSCAN_SONAR_GENERATOR_MODE_NONE) &&
          (capabilities->generator != HYSCAN_SONAR_GENERATOR_MODE_PRESET))
        {
          if ((!generator->automatic) &&
              (generator->tone == NULL) &&
              (generator->lfm == NULL))
            {
              goto fail;
            }
        }

      if (capabilities->generator & HYSCAN_SONAR_GENERATOR_MODE_PRESET)
        if (generator->presets == NULL)
          goto fail;
    }

  /* Параметры ВАРУ. */
  if ((capabilities->tvg != HYSCAN_SONAR_TVG_MODE_NONE) &&
      (capabilities->tvg != HYSCAN_SONAR_TVG_MODE_AUTO))
    {
      tvg = hyscan_sonar_info_parse_tvg (schema, source);
      if (tvg == NULL)
        goto fail;
    }

  info = g_slice_new0 (HyScanSonarInfoSource);
  info->source = source;
  info->dev_id = g_strdup (dev_id);
  info->description = g_strdup (description);
  info->master = master;
  info->position = position;
  info->capabilities = capabilities;
  info->receiver = receiver;
  info->generator = generator;
  info->tvg = tvg;

  return info;

fail:
  hyscan_sonar_info_capabilities_free (capabilities);
  hyscan_sonar_info_receiver_free (receiver);
  hyscan_sonar_info_generator_free (generator);
  hyscan_sonar_info_tvg_free (tvg);

  return NULL;
}

/* Функция считывает информацию по всем гидролокационным источникам. */
static GHashTable *
hyscan_sonar_info_parse_sources (HyScanDataSchema *schema)
{
  GHashTable *info;

  HyScanSourceType *sources;
  guint32 n_sources;
  guint32 i;

  /* Список источников данных. */
  sources = hyscan_sonar_info_list_sources_int (schema, &n_sources);
  if (sources == NULL)
    return NULL;

  info = g_hash_table_new_full (NULL, NULL, NULL, (GDestroyNotify)hyscan_sonar_info_source_free);
  for (i = 0; i < n_sources; i++)
    {
      HyScanSonarInfoSource *source;

      source = hyscan_sonar_info_parse_source (schema, sources[i]);
      if (source != NULL)
        g_hash_table_insert (info, GINT_TO_POINTER (sources[i]), source);
    }

  g_free (sources);

  return info;
}

/**
 * hyscan_sonar_info_new:
 * @schema: схема устройства
 *
 * Функция создаёт новый объект с информацией о гидролокаторе.
 *
 * Returns: #HyScanSonarInfo. Для удаления #g_object_unref.
 */
HyScanSonarInfo *
hyscan_sonar_info_new (HyScanDataSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_SONAR_INFO,
                       "schema", schema,
                       NULL);
}

/**
 * hyscan_sonar_info_get_software_ping:
 * @info: указатель на #HyScanSonarInfo
 *
 * Функция определяет возможность программного управления излучением.
 *
 * Returns: %TRUE - если имеется возможность программного управления
 * излучением, иначе %FALSE.
 */
gboolean
hyscan_sonar_info_get_software_ping (HyScanSonarInfo *info)
{
  g_return_val_if_fail (HYSCAN_IS_SONAR_INFO (info), FALSE);

  return info->priv->software_ping;
}

/**
 * hyscan_sonar_info_get_sources:
 * @info: указатель на #HyScanSonarInfo
 * @n_sources: число источников гидролокационных данных
 *
 * Функция возвращает список источников гидролокационных данных.
 *
 * Returns: (transfer none): Список источников гидролокационных данных.
 */
const HyScanSourceType *
hyscan_sonar_info_list_sources (HyScanSonarInfo *info,
                               guint32         *n_sources)
{
  g_return_val_if_fail (HYSCAN_IS_SONAR_INFO (info), NULL);

  if (info->priv->sources_list == NULL)
    return NULL;

  *n_sources = info->priv->sources_list->len;
  if (info->priv->sources_list->len == 0)
    return NULL;

  return (HyScanSourceType *)info->priv->sources_list->data;
}

/**
 * hyscan_sonar_info_get_source:
 * @info: указатель на #HyScanSonarInfo
 * @source: источник гидролокационных данных
 *
 * Функция возвращает параметры источника гидролокационных данных.
 *
 * Returns: (transfer none): Параметры гидролокационного источника данных или NULL.
 */
const HyScanSonarInfoSource *
hyscan_sonar_info_get_source (HyScanSonarInfo  *info,
                              HyScanSourceType  source)
{
  g_return_val_if_fail (HYSCAN_IS_SONAR_INFO (info), NULL);

  if (info->priv->sources == NULL)
    return NULL;

  return g_hash_table_lookup (info->priv->sources, GINT_TO_POINTER (source));
}

/**
 * hyscan_sonar_info_source_copy:
 * @info: структура #HyScanSonarInfoSource для копирования
 *
 * Функция создаёт копию структуры #HyScanSonarInfoSource.
 *
 * Returns: (transfer full): Новая структура #HyScanSonarInfoSource.
 * Для удаления #hyscan_sonar_info_source_free.
 */
HyScanSonarInfoSource *
hyscan_sonar_info_source_copy (const HyScanSonarInfoSource *info)
{
  HyScanSonarInfoSource *new_info;

  if (info == NULL)
    return NULL;

  new_info = g_slice_new (HyScanSonarInfoSource);
  new_info->source = info->source;
  new_info->dev_id = g_strdup (info->dev_id);
  new_info->description = g_strdup (info->description);
  new_info->master = info->master;
  new_info->position = hyscan_antenna_position_copy (info->position);
  new_info->capabilities = hyscan_sonar_info_capabilities_copy (info->capabilities);
  new_info->receiver = hyscan_sonar_info_receiver_copy (info->receiver);
  new_info->generator = hyscan_sonar_info_generator_copy (info->generator);
  new_info->tvg = hyscan_sonar_info_tvg_copy (info->tvg);

  return new_info;
}

/**
 * hyscan_sonar_info_source_free:
 * @info: структура #HyScanSonarInfoSource для удаления
 *
 * Функция удаляет структуру #HyScanSonarInfoSource.
 */
void
hyscan_sonar_info_source_free (HyScanSonarInfoSource *info)
{
  if (info == NULL)
    return;

  g_free ((gchar*)info->dev_id);
  g_free ((gchar*)info->description);
  hyscan_antenna_position_free (info->position);
  hyscan_sonar_info_capabilities_free (info->capabilities);
  hyscan_sonar_info_receiver_free (info->receiver);
  hyscan_sonar_info_generator_free (info->generator);
  hyscan_sonar_info_tvg_free (info->tvg);

  g_slice_free (HyScanSonarInfoSource, info);
}

/**
 * hyscan_sonar_info_capabilities_copy:
 * @info: структура #HyScanSonarInfoCapabilities для копирования
 *
 * Функция создаёт копию структуры #HyScanSonarInfoCapabilities.
 *
 * Returns: (transfer full): Новая структура #HyScanSonarInfoCapabilities.
 * Для удаления #hyscan_sonar_info_capabilities_free.
 */
HyScanSonarInfoCapabilities *
hyscan_sonar_info_capabilities_copy (const HyScanSonarInfoCapabilities *info)
{
  if (info != NULL)
    return g_slice_dup (HyScanSonarInfoCapabilities, info);

  return NULL;
}

/**
 * hyscan_sonar_info_capabilities_free:
 * @info: структура #HyScanSonarInfoCapabilities для удаления
 *
 * Функция удаляет структуру #HyScanSonarInfoCapabilities.
 */
void
hyscan_sonar_info_capabilities_free (HyScanSonarInfoCapabilities *info)
{
  if (info != NULL)
    g_slice_free (HyScanSonarInfoCapabilities, info);
}

/**
 * hyscan_sonar_info_receiver_copy:
 * @info: структура #HyScanSonarInfoReceiver для копирования
 *
 * Функция создаёт копию структуры #HyScanSonarInfoReceiver.
 *
 * Returns: (transfer full): Новая структура #HyScanSonarInfoReceiver.
 * Для удаления #hyscan_sonar_info_receiver_free.
 */
HyScanSonarInfoReceiver *
hyscan_sonar_info_receiver_copy (const HyScanSonarInfoReceiver *info)
{
  if (info != NULL)
    return g_slice_dup (HyScanSonarInfoReceiver, info);

  return NULL;
}

/**
 * hyscan_sonar_info_receiver_free:
 * @info: структура #HyScanSonarInfoReceiver для удаления
 *
 * Функция удаляет структуру #HyScanSonarInfoReceiver.
 */
void
hyscan_sonar_info_receiver_free (HyScanSonarInfoReceiver *info)
{
  if (info != NULL)
    g_slice_free (HyScanSonarInfoReceiver, info);
}

/**
 * hyscan_sonar_info_generator_copy:
 * @info: структура #HyScanSonarInfoGenerator для копирования
 *
 * Функция создаёт копию структуры #HyScanSonarInfoGenerator.
 *
 * Returns: (transfer full): Новая структура #HyScanSonarInfoGenerator.
 * Для удаления #hyscan_sonar_info_generator_free.
 */
HyScanSonarInfoGenerator *
hyscan_sonar_info_generator_copy (const HyScanSonarInfoGenerator *info)
{
  HyScanSonarInfoGenerator *new_info;
  GList *presets, *new_presets;

  if (info == NULL)
    return NULL;

  new_presets = NULL;
  presets = info->presets;
  while (presets != NULL)
    {
      new_presets = g_list_prepend (new_presets,
                                    hyscan_data_schema_enum_value_copy (presets->data));

      presets = g_list_next (presets);
    }

  new_info = g_slice_new (HyScanSonarInfoGenerator);
  new_info->presets = g_list_reverse (new_presets);
  new_info->automatic = info->automatic;
  new_info->tone = hyscan_sonar_info_signal_copy (info->tone);
  new_info->lfm = hyscan_sonar_info_signal_copy (info->lfm);

  new_info->signals = HYSCAN_SONAR_GENERATOR_SIGNAL_NONE;
  new_info->signals |= info->automatic ? HYSCAN_SONAR_GENERATOR_SIGNAL_AUTO : 0;
  new_info->signals |= (info->tone != NULL) ? HYSCAN_SONAR_GENERATOR_SIGNAL_TONE : 0;
  new_info->signals |= (info->lfm != NULL) ? HYSCAN_SONAR_GENERATOR_SIGNAL_LFM : 0;

  return new_info;
}

/**
 * hyscan_sonar_info_generator_free:
 * @info: структура #HyScanSonarInfoGenerator для удаления
 *
 * Функция удаляет структуру #HyScanSonarInfoGenerator.
 */
void
hyscan_sonar_info_generator_free (HyScanSonarInfoGenerator *info)
{
  if (info == NULL)
    return;

  hyscan_sonar_info_signal_free (info->tone);
  hyscan_sonar_info_signal_free (info->lfm);
  g_list_free_full (info->presets, (GDestroyNotify)hyscan_data_schema_enum_value_free);

  g_slice_free (HyScanSonarInfoGenerator, info);
}

/**
 * hyscan_sonar_info_tvg_copy:
 * @info: структура #HyScanSonarInfoTVG для копирования
 *
 * Функция создаёт копию структуры #HyScanSonarInfoTVG.
 *
 * Returns: (transfer full): Новая структура #HyScanSonarInfoTVG.
 * Для удаления #hyscan_sonar_info_tvg_free.
 */
HyScanSonarInfoTVG *
hyscan_sonar_info_tvg_copy (const HyScanSonarInfoTVG *info)
{
  if (info != NULL)
    return g_slice_dup (HyScanSonarInfoTVG, info);

  return NULL;
}

/**
 * hyscan_sonar_info_tvg_free:
 * @info: структура #HyScanSonarInfoTVG для удаления
 *
 * Функция удаляет структуру #HyScanSonarInfoTVG.
 */
void
hyscan_sonar_info_tvg_free (HyScanSonarInfoTVG *info)
{
  if (info != NULL)
    g_slice_free (HyScanSonarInfoTVG, info);
}

/**
 * hyscan_sonar_info_signal_copy:
 * @info: структура #HyScanSonarInfoSignal для копирования
 *
 * Функция создаёт копию структуры #HyScanSonarInfoSignal.
 *
 * Returns: (transfer full): Новая структура #HyScanSonarInfoSignal.
 * Для удаления #hyscan_sonar_info_signal_free.
 */
HyScanSonarInfoSignal *
hyscan_sonar_info_signal_copy (const HyScanSonarInfoSignal *info)
{
  HyScanSonarInfoSignal *new_info;

  if (info == NULL)
    return NULL;

  new_info = g_slice_new (HyScanSonarInfoSignal);
  new_info->duration_name = g_strdup (info->duration_name);
  new_info->min_duration = info->min_duration;
  new_info->max_duration = info->max_duration;
  new_info->duration_step = info->duration_step;
  new_info->dirty_cycle = info->dirty_cycle;

  return new_info;
}

/**
 * hyscan_sonar_info_signal_free:
 * @info: структура #HyScanSonarInfoSignal для удаления
 *
 * Функция удаляет структуру #HyScanSonarInfoSignal.
 */
void
hyscan_sonar_info_signal_free (HyScanSonarInfoSignal *info)
{
  if (info == NULL)
    return;

  g_free ((gchar*)info->duration_name);

  g_slice_free (HyScanSonarInfoSignal, info);
}
