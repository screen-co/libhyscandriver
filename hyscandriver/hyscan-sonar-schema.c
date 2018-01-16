/* hyscan-sonar-schema.c
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
 * SECTION: hyscan-sonar-schema
 * @Short_description: класс создания схемы гидролокатора
 * @Title: HyScanSonarSchema
 *
 * Класс предназначен для автоматизации процесса создания схемы данных
 * гидролокатора. Он содержит функции которые определяют наличие и
 * параметры подсистем гидролокатора.
 */

#include "hyscan-sonar-schema.h"

struct _HyScanSonarSchemaPrivate
{
  GHashTable                  *sources;                        /* Наличие источников данных. */
  GHashTable                  *slaves;                         /* Ведомые источники данных. */
  GHashTable                  *generators;                     /* Наличие генераторов. */
  GHashTable                  *tvgs;                           /* Наличие ВАРУ. */
  GHashTable                  *channels;                       /* Наличие приёмного канала. */
};

static void    hyscan_sonar_schema_object_constructed          (GObject                       *object);
static void    hyscan_sonar_schema_object_finalize             (GObject                       *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSonarSchema, hyscan_sonar_schema, HYSCAN_TYPE_DATA_SCHEMA_BUILDER)

static void
hyscan_sonar_schema_class_init (HyScanSonarSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = hyscan_sonar_schema_object_constructed;
  object_class->finalize = hyscan_sonar_schema_object_finalize;
}

static void
hyscan_sonar_schema_init (HyScanSonarSchema *schema)
{
  schema->priv = hyscan_sonar_schema_get_instance_private (schema);
}

static void
hyscan_sonar_schema_object_constructed (GObject *object)
{
  HyScanSonarSchema *schema = HYSCAN_SONAR_SCHEMA (object);
  HyScanDataSchemaBuilder *builder = HYSCAN_DATA_SCHEMA_BUILDER (object);
  HyScanSonarSchemaPrivate *priv = schema->priv;

  G_OBJECT_CLASS (hyscan_sonar_schema_parent_class)->constructed (object);

  priv->sources = g_hash_table_new (g_direct_hash, g_direct_equal);
  priv->slaves = g_hash_table_new (g_direct_hash, g_direct_equal);
  priv->generators = g_hash_table_new (g_direct_hash, g_direct_equal);
  priv->tvgs = g_hash_table_new (g_direct_hash, g_direct_equal);
  priv->channels = g_hash_table_new (g_direct_hash, g_direct_equal);

  /* Версия и идентификатор схемы данных гидролокатора. */
  hyscan_data_schema_builder_key_integer_create (builder, "/schema/id", "id",
                                                 "Sonar schema id", HYSCAN_SONAR_SCHEMA_ID);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/id", HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

  hyscan_data_schema_builder_key_integer_create (builder, "/schema/version", "version",
                                                 "Sonar schema version", HYSCAN_SONAR_SCHEMA_VERSION);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/version", HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
}

static void
hyscan_sonar_schema_object_finalize (GObject *object)
{
  HyScanSonarSchema *schema = HYSCAN_SONAR_SCHEMA (object);
  HyScanSonarSchemaPrivate *priv = schema->priv;

  g_hash_table_unref (priv->sources);
  g_hash_table_unref (priv->slaves);
  g_hash_table_unref (priv->generators);
  g_hash_table_unref (priv->tvgs);
  g_hash_table_unref (priv->channels);

  G_OBJECT_CLASS (hyscan_sonar_schema_parent_class)->finalize (object);
}

/**
 * hyscan_sonar_schema_new:
 *
 * Функция создаёт новый объект HyScanSonarSchema.
 *
 * Returns: #HyScanSonarSchema. Для удаления #g_object_unref.
 */
HyScanSonarSchema *
hyscan_sonar_schema_new (void)
{
  return g_object_new (HYSCAN_TYPE_SONAR_SCHEMA,
                       "schema-id", "sonar",
                       NULL);
}

/**
 * hyscan_sonar_schema_get_schema:
 * @schema: указатель на #HyScanSonarSchema
 *
 * Функция возвращает схему гидролокатора.
 *
 * Returns: #HyScanDataSchema. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_sonar_schema_get_schema (HyScanSonarSchema *schema)
{
  HyScanDataSchemaBuilder *builder;
  HyScanDataSchema *sonar_schema;
  gchar *data;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), NULL);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);
  data = hyscan_data_schema_builder_get_data (builder);
  sonar_schema = hyscan_data_schema_new_from_string (data, "sonar");
  g_free (data);

  return sonar_schema;
}

/**
 * hyscan_sonar_schema_set_software_ping:
 * @schema: указатель на #HyScanSonarSchema
 * @software_ping: возможность программного управления излучением
 *
 * Функция добавляет в схему информацию о возможности программного управления
 * излучением.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_set_software_ping (HyScanSonarSchema *schema,
                                       gboolean           software_ping)
{
  HyScanDataSchemaBuilder *builder;

  gboolean status;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  key_id = g_strdup ("/sources/software-ping");
  hyscan_data_schema_builder_key_boolean_create (builder, key_id, "software-ping", NULL, software_ping);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

  g_free (key_id);

  return status;
}

/**
 * hyscan_sonar_schema_source_add:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @antenna_vpattern: диаграмма направленности антенны в вертикальной плоскости, рад
 * @antenna_hpattern: диаграмма направленности антенны в горизонтальной плоскости, рад
 * @antenna_frequency: центральная частота, Гц
 * @antenna_bandwidth: полоса пропускания, Гц
 * @min_receive_time: минимальное время приёма эхосигнала, с
 * @max_receive_time: максимальное время приёма эхосигнала, с
 * @auto_receive_time: возможность автоматического выбора времени приёма
 *
 * Функция добавляет в схему описание источника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_add (HyScanSonarSchema *schema,
                                HyScanSourceType   source,
                                gdouble            antenna_vpattern,
                                gdouble            antenna_hpattern,
                                gdouble            antenna_frequency,
                                gdouble            antenna_bandwidth,
                                gdouble            min_receive_time,
                                gdouble            max_receive_time,
                                gboolean           auto_receive_time)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s", source_name);

  /* Признак наличия источника данных. */
  key_id = g_strdup_printf ("%s/id", prefix);
  hyscan_data_schema_builder_key_string_create (builder, key_id, "id", NULL, source_name);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Диаграмма направленности антенны в вертикальной плоскости. */
  key_id = g_strdup_printf ("%s/antenna/pattern/vertical", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "vertical-pattern", NULL, antenna_vpattern);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Диаграмма направленности антенны в горизонтальной плоскости. */
  key_id = g_strdup_printf ("%s/antenna/pattern/horizontal", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "horizontal-pattern", NULL, antenna_hpattern);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Центральная частота. */
  key_id = g_strdup_printf ("%s/antenna/frequency", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "antenna-frequency", NULL, antenna_frequency);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Полоса пропускания. */
  key_id = g_strdup_printf ("%s/antenna/bandwidth", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "antenna-bandwidth", NULL, antenna_bandwidth);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Время приёма эхосигнала источником данных. */
  key_id = g_strdup_printf ("%s/control/min-receive-time", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "min-receive-time", NULL, min_receive_time);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/control/max-receive-time", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "max-receive-time", NULL, max_receive_time);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Возможность автоматического выбора времени приёма. */
  key_id = g_strdup_printf ("%s/control/auto-receive-time", prefix);
  hyscan_data_schema_builder_key_boolean_create (builder, key_id, "auto-receive-time", NULL, auto_receive_time);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (status)
    status = g_hash_table_insert (schema->priv->sources, GINT_TO_POINTER (source), NULL);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_source_set_master:
 * @schema: указатель на #HyScanSonarSchema
 * @master: ведущий источник данных
 * @slave: ведомый источик данных
 *
 * Функция устанавливает связь ведущего и ведомого источников данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_set_master (HyScanSonarSchema *schema,
                                       HyScanSourceType   master,
                                       HyScanSourceType   slave)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *master_name;
  const gchar *slave_name;
  gboolean status;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  master_name = hyscan_source_get_name_by_type (master);
  slave_name = hyscan_source_get_name_by_type (slave);
  if ((master_name == NULL) || (slave_name == NULL))
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (master)))
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (slave)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->slaves, GINT_TO_POINTER (slave)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->generators, GINT_TO_POINTER (slave)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->tvgs, GINT_TO_POINTER (slave)))
    return FALSE;

  key_id = g_strdup_printf ("/sources/%s/master", slave_name);
  hyscan_data_schema_builder_key_string_create (builder, key_id, "master", NULL, master_name);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (status)
    g_hash_table_insert (schema->priv->slaves, GINT_TO_POINTER (slave), GINT_TO_POINTER (master));

  return status;
}

/**
 * hyscan_sonar_schema_channel_add:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @channel: индекс канала данных
 * @antenna_voffset: вертикальное смещение антенны в блоке, м
 * @antenna_hoffset: горизнотальное смещение антенны в блоке, м
 * @adc_offset: смещение 0 АЦП
 * @adc_vref: опорное напряжение АЦП, В
 *
 * Функция добавляет в схему описание приёмного канала.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_add_channel (HyScanSonarSchema *schema,
                                        HyScanSourceType   source,
                                        guint              channel,
                                        gdouble            antenna_voffset,
                                        gdouble            antenna_hoffset,
                                        gint               adc_offset,
                                        gdouble            adc_vref)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/channels/%d", source_name, channel);
  if (g_hash_table_contains (schema->priv->channels, GINT_TO_POINTER (g_str_hash (prefix))))
    {
      status = FALSE;
      goto exit;
    }

  /* Вертикальное смещение антенны в блоке. */
  key_id = g_strdup_printf ("%s/antenna/offset/vertical", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "vertical-offset", NULL,  antenna_voffset);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Горизонтальное смещение антенны в блоке. */
  key_id = g_strdup_printf ("%s/antenna/offset/horizontal", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "horizontal-offset", NULL, antenna_hoffset);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Смещение 0 АЦП. */
  key_id = g_strdup_printf ("%s/adc/offset", prefix);
  hyscan_data_schema_builder_key_integer_create (builder, key_id, "offset", NULL, adc_offset);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Опорное напряжение АЦП. */
  key_id = g_strdup_printf ("%s/adc/vref", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "vref", NULL, adc_vref);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (status)
    status = g_hash_table_insert (schema->priv->channels, GINT_TO_POINTER (g_str_hash (prefix)), NULL);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_generator_add:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @capabilities: флаги возможных режимов работы генератора
 * @signals: флаги возможных типов сигналов
 *
 * Функция добавляет описание генератора.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_generator_add (HyScanSonarSchema              *schema,
                                   HyScanSourceType                source,
                                   HyScanSonarGeneratorModeType    capabilities,
                                   HyScanSonarGeneratorSignalType  signals)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->slaves, GINT_TO_POINTER (source)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->generators, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/generator", source_name);

  /* Режимы работы генератора. */
  key_id = g_strdup_printf ("%s/capabilities", prefix);
  hyscan_data_schema_builder_key_integer_create (builder, key_id, "capabilities", NULL, capabilities);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Возможные сигналы. */
  key_id = g_strdup_printf ("%s/signals", prefix);
  hyscan_data_schema_builder_key_integer_create (builder, key_id, "signals", NULL, signals);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (status)
    g_hash_table_insert (schema->priv->generators, GINT_TO_POINTER (source), NULL);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_generator_set_signal_params:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @signal: тип сигнала
 * @min_duration: минимальная длительность, с
 * @max_duration: максимальная длительность, с
 * @durty_cycle: допустимая скважность
 *
 * Функция устанавливает предельные параметры генератора сигналов.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_generator_set_signal_params (HyScanSonarSchema             *schema,
                                                 HyScanSourceType               source,
                                                 HyScanSonarGeneratorSignalType signal,
                                                 gdouble                        min_duration,
                                                 gdouble                        max_duration,
                                                 gdouble                        durty_cycle)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  const gchar *signal_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (signal == HYSCAN_SONAR_GENERATOR_SIGNAL_TONE)
    signal_name = "tone";
  else if ((signal == HYSCAN_SONAR_GENERATOR_SIGNAL_LFM) || (signal == HYSCAN_SONAR_GENERATOR_SIGNAL_LFMD))
    signal_name = "lfm";
  else
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  if (!g_hash_table_contains (schema->priv->generators, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/generator/%s", source_name, signal_name);

  /* Параметры сигнала. */
  key_id = g_strdup_printf ("%s/min-duration", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "min-duration", NULL, min_duration);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/max-duration", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "max-duration", NULL, max_duration);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/durty-cycle", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "durty-cycle", NULL, durty_cycle);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (status)
    g_hash_table_insert (schema->priv->generators, GINT_TO_POINTER (source), NULL);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_generator_add_preset:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @preset_id: идентификатор преднастройки
 * @preset_name: название преднастройки
 * @preset_description: описание преднастройки
 *
 * Функция добавляет преднастроенный режим генератора.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_generator_add_preset (HyScanSonarSchema *schema,
                                          HyScanSourceType   source,
                                          gint               preset_id,
                                          const gchar       *preset_name,
                                          const gchar       *preset_description)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  if (!g_hash_table_contains (schema->priv->generators, GINT_TO_POINTER (source)))
    return FALSE;

  /* Преднастройка генератора. */
  key_id = g_strdup_printf ("/sources/%s/generator/presets/%d", source_name, preset_id);
  status = hyscan_data_schema_builder_key_integer_create (builder, key_id, preset_name, preset_description, preset_id);
  if (status)
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  return status;
}

/**
 * hyscan_sonar_schema_tvg_add:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @capabilities: флаги возможных режимов работы ВАРУ
 * @min_gain: минимальное значение коэффициента усиления, дБ
 * @max_gain: максимальное значение коэффициента усиления, дБ
 * @can_decrease: возможность уменьшения коэффициента усиления
 *
 * Функция добавляет в схему описание системы ВАРУ.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_tvg_add (HyScanSonarSchema      *schema,
                             HyScanSourceType        source,
                             HyScanSonarTVGModeType  capabilities,
                             gdouble                 min_gain,
                             gdouble                 max_gain,
                             gboolean                can_decrease)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->slaves, GINT_TO_POINTER (source)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->tvgs, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/tvg", source_name);

  /* Режимы работы ВАРУ. */
  key_id = g_strdup_printf ("%s/capabilities", prefix);
  hyscan_data_schema_builder_key_integer_create (builder, key_id, "capabilities", NULL, capabilities);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Параметры ВАРУ. */
  key_id = g_strdup_printf ("%s/min-gain", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "min-gain", NULL, min_gain);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/max-gain", prefix);
  hyscan_data_schema_builder_key_double_create (builder, key_id, "max-gain", NULL, max_gain);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  key_id = g_strdup_printf ("%s/can-decrease", prefix);
  hyscan_data_schema_builder_key_boolean_create (builder, key_id, "can-decrease", NULL, can_decrease);
  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (status)
    g_hash_table_insert (schema->priv->tvgs, GINT_TO_POINTER (source), NULL);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_source_set_position:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @position: местоположение антенны по умолчанию
 *
 * Функция устанавливает местоположение антенны по умолчанию.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_set_position (HyScanSonarSchema     *schema,
                                         HyScanSourceType       source,
                                         HyScanAntennaPosition *position)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = HYSCAN_DATA_SCHEMA_BUILDER (schema);

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/position", source_name);

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
