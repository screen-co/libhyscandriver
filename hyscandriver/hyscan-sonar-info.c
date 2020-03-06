/* hyscan-sonar-info.c
 *
 * Copyright 2018-2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-sonar-info
 * @Short_description: класс с информацией о гидролокаторе
 * @Title: HyScanSonarInfo
 *
 * Класс предназначен для получения информации о гидролокаторе по его схеме.
 * Для создания объекта используется функция #hyscan_sonar_info_new, в
 * которую передаётся схема устройства.
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

#include <string.h>

#define SONAR_PARAM_NAME(source, ...)  hyscan_param_name_constructor (key_id, \
                                         (guint)sizeof (key_id), \
                                         "sources", \
                                         hyscan_source_get_id_by_type (source), \
                                         __VA_ARGS__)

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
};

static void            hyscan_sonar_info_set_property          (GObject               *object,
                                                                guint                  prop_id,
                                                                const GValue          *value,
                                                                GParamSpec            *pspec);
static void            hyscan_sonar_info_object_constructed    (GObject               *object);
static void            hyscan_sonar_info_object_finalize       (GObject               *object);

static gpointer        hyscan_sonar_info_preset_copy           (gconstpointer          preset,
                                                                gpointer               data);

static HyScanSourceType *
                       hyscan_sonar_info_list_sources_int      (HyScanDataSchema      *schema,
                                                                guint32               *n_sources);

static HyScanAntennaOffset *
                       hyscan_sonar_info_parse_offset          (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static GList *
                       hyscan_sonar_info_parse_presets         (HyScanDataSchema      *schema,
                                                                HyScanSourceType       source);

static HyScanSonarInfoReceiver *
                       hyscan_sonar_info_parse_receiver        (HyScanDataSchema      *schema,
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

G_DEFINE_BOXED_TYPE (HyScanSonarInfoReceiver, hyscan_sonar_info_receiver,
                     hyscan_sonar_info_receiver_copy, hyscan_sonar_info_receiver_free)

G_DEFINE_BOXED_TYPE (HyScanSonarInfoTVG, hyscan_sonar_info_tvg,
                     hyscan_sonar_info_tvg_copy, hyscan_sonar_info_tvg_free)

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

  /* Параметры источников данных. */
  priv->sources = hyscan_sonar_info_parse_sources (priv->schema);

  if (priv->sources == NULL)
    return;

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

/* Функция копирует структуру HyScanDataSchemaEnumValue. */
static gpointer
hyscan_sonar_info_preset_copy (gconstpointer preset,
                               gpointer      data)
{
  return hyscan_data_schema_enum_value_copy (preset);
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
          HyScanSourceType source = hyscan_source_get_type_by_id (keyv[2]);

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
static HyScanAntennaOffset *
hyscan_sonar_info_parse_offset (HyScanDataSchema *schema,
                                HyScanSourceType  source)
{
  HyScanAntennaOffset offset = {0};
  gchar key_id[128];

  SONAR_PARAM_NAME (source, "offset/starboard", NULL);
  if (!hyscan_data_schema_key_get_double (schema, key_id, NULL, NULL, &offset.starboard, NULL))
    return NULL;

  SONAR_PARAM_NAME (source, "offset/forward", NULL);
  if (!hyscan_data_schema_key_get_double (schema, key_id, NULL, NULL, &offset.forward, NULL))
    return NULL;

  SONAR_PARAM_NAME (source, "offset/vertical", NULL);
  if (!hyscan_data_schema_key_get_double (schema, key_id, NULL, NULL, &offset.vertical, NULL))
    return NULL;

  SONAR_PARAM_NAME (source, "offset/yaw", NULL);
  if (!hyscan_data_schema_key_get_double (schema, key_id, NULL, NULL, &offset.yaw, NULL))
    return NULL;

  SONAR_PARAM_NAME (source, "offset/pitch", NULL);
  if (!hyscan_data_schema_key_get_double (schema, key_id, NULL, NULL, &offset.pitch, NULL))
    return NULL;

  SONAR_PARAM_NAME (source, "offset/roll", NULL);
  if (!hyscan_data_schema_key_get_double (schema, key_id, NULL, NULL, &offset.roll, NULL))
    return NULL;

  return hyscan_antenna_offset_copy (&offset);
}

/* Функция считывает информацию о приёмнике. */
static HyScanSonarInfoReceiver *
hyscan_sonar_info_parse_receiver (HyScanDataSchema *schema,
                                  HyScanSourceType  source)
{
  HyScanSonarInfoReceiver info = {0};
  const gchar *cap_string;
  gchar key_id[128];
  gboolean status;

  /* Режимы работы приёмника. */
  SONAR_PARAM_NAME (source, "receiver/capabilities", NULL);
  cap_string = hyscan_data_schema_key_get_string (schema, key_id);
  if (cap_string != NULL)
    {
      if (g_strrstr (cap_string, "manual") != NULL)
        info.capabilities |= HYSCAN_SONAR_RECEIVER_MODE_MANUAL;

      if (g_strrstr (cap_string, "auto") != NULL)
        info.capabilities |= HYSCAN_SONAR_RECEIVER_MODE_AUTO;
    }

  /* Диапазон времени приёма. */
  if (info.capabilities & HYSCAN_SONAR_RECEIVER_MODE_MANUAL)
    {
      SONAR_PARAM_NAME (source, "receiver/time", NULL);
      status = hyscan_data_schema_key_get_double (schema, key_id,
                                                  &info.min_time, &info.max_time,
                                                  NULL, NULL);
      if (!status)
        return NULL;
    }

  return hyscan_sonar_info_receiver_copy (&info);
}

/* Функция считывает информацию о режимах работы генератора. */
static GList *
hyscan_sonar_info_parse_presets (HyScanDataSchema *schema,
                                 HyScanSourceType  source)
{
  GList *presets = NULL;
  const gchar * const *keys;
  guint preset_offset;
  gchar key_id[128];
  guint i;

  SONAR_PARAM_NAME (source, "generator/", NULL);
  preset_offset = strlen (key_id);

  keys = hyscan_data_schema_list_keys (schema);
  for (i = 0; keys[i] != NULL; i++)
    {
      HyScanDataSchemaEnumValue preset;
      const gchar *id;
      gint64 value;

      if (!g_str_has_prefix (keys[i], key_id))
        continue;

      if (!hyscan_data_schema_key_get_integer (schema, keys[i], NULL, NULL, &value, NULL))
        continue;

      id = keys[i] + preset_offset;
      if (id == NULL)
        continue;

      preset.value = value;
      preset.id = id;
      preset.name = hyscan_data_schema_key_get_name (schema, keys[i]);
      preset.description = hyscan_data_schema_key_get_description (schema, keys[i]);

      presets = g_list_prepend (presets, hyscan_data_schema_enum_value_copy (&preset));
    }

  return g_list_reverse (presets);
}

/* Функция считывает информацию о системе ВАРУ. */
static HyScanSonarInfoTVG *
hyscan_sonar_info_parse_tvg (HyScanDataSchema *schema,
                             HyScanSourceType  source)
{
  HyScanSonarInfoTVG info = {0};
  const gchar *cap_string;
  gchar key_id[128];
  gboolean status;

  /* Режимы работы ВАРУ. */
  SONAR_PARAM_NAME (source, "tvg/capabilities", NULL);
  cap_string = hyscan_data_schema_key_get_string (schema, key_id);
  if (cap_string != NULL)
    {
      if (g_strrstr (cap_string, "auto") != NULL)
        info.capabilities |= HYSCAN_SONAR_TVG_MODE_AUTO;

      if (g_strrstr (cap_string, "constant") != NULL)
        info.capabilities |= HYSCAN_SONAR_TVG_MODE_CONSTANT;

      if (g_strrstr (cap_string, "linear-db") != NULL)
        info.capabilities |= HYSCAN_SONAR_TVG_MODE_LINEAR_DB;

      if (g_strrstr (cap_string, "logarithmic") != NULL)
        info.capabilities |= HYSCAN_SONAR_TVG_MODE_LOGARITHMIC;
    }

  if ((info.capabilities & HYSCAN_SONAR_TVG_MODE_CONSTANT) ||
      (info.capabilities & HYSCAN_SONAR_TVG_MODE_LINEAR_DB) ||
      (info.capabilities & HYSCAN_SONAR_TVG_MODE_LOGARITHMIC))
    {
      SONAR_PARAM_NAME (source, "tvg/gain", NULL);
      status = hyscan_data_schema_key_get_double (schema, key_id,
                                                  &info.min_gain, &info.max_gain,
                                                  NULL, NULL);
      if (!status)
        return NULL;

      SONAR_PARAM_NAME (source, "tvg/decrease", NULL);
      if (!hyscan_data_schema_key_get_boolean (schema, key_id, &info.decrease))
        info.decrease = FALSE;
    }

  return hyscan_sonar_info_tvg_copy (&info);
}

/* Функция считывает информацию о гидролокационном источнике данных. */
static HyScanSonarInfoSource *
hyscan_sonar_info_parse_source (HyScanDataSchema *schema,
                                HyScanSourceType  source)
{
  gchar key_id[128];

  const gchar *dev_id = NULL;
  const gchar *description = NULL;
  const gchar *link = NULL;
  HyScanSonarInfoSource *info = NULL;
  HyScanAntennaOffset *offset = NULL;
  HyScanSonarInfoReceiver *receiver = NULL;
  HyScanSonarInfoTVG *tvg = NULL;
  GList *presets = NULL;

  /* уникальный идентификатор устройства. */
  SONAR_PARAM_NAME (source, "dev-id", NULL);
  dev_id = hyscan_data_schema_key_get_string (schema, key_id);
  if (dev_id == NULL)
    goto fail;

  /* Описание источника данных. */
  SONAR_PARAM_NAME (source, "description", NULL);
  description = hyscan_data_schema_key_get_string (schema, key_id);

  /* Связанный источник данных. */
  SONAR_PARAM_NAME (source, "link", NULL);
  link = hyscan_data_schema_key_get_string (schema, key_id);

  /* Смещение антенн по умолчанию. */
  offset = hyscan_sonar_info_parse_offset (schema, source);

  /* Параметры приёмника. */
  receiver = hyscan_sonar_info_parse_receiver (schema, source);
  if (receiver == NULL)
    goto fail;

  /* Параметры генератора. */
  presets = hyscan_sonar_info_parse_presets (schema, source);

  /* Параметры ВАРУ. */
  tvg = hyscan_sonar_info_parse_tvg (schema, source);
  if (tvg == NULL)
    goto fail;

  info = g_slice_new0 (HyScanSonarInfoSource);
  info->source = source;
  info->dev_id = g_strdup (dev_id);
  info->link = hyscan_source_get_type_by_id (link);
  info->description = g_strdup (description);
  info->offset = offset;
  info->receiver = receiver;
  info->presets = presets;
  info->tvg = tvg;

  return info;

fail:
  hyscan_sonar_info_receiver_free (receiver);
  g_list_free_full (presets, (GDestroyNotify)hyscan_data_schema_enum_value_free);
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
  new_info->link = info->link;
  new_info->offset = hyscan_antenna_offset_copy (info->offset);
  new_info->receiver = hyscan_sonar_info_receiver_copy (info->receiver);
  new_info->presets = g_list_copy_deep (info->presets, hyscan_sonar_info_preset_copy, NULL);
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
  hyscan_antenna_offset_free (info->offset);
  hyscan_sonar_info_receiver_free (info->receiver);
  g_list_free_full (info->presets, (GDestroyNotify)hyscan_data_schema_enum_value_free);
  hyscan_sonar_info_tvg_free (info->tvg);

  g_slice_free (HyScanSonarInfoSource, info);
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
