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
 *
 * Информация о гидролокационных источниках данных находится в ветке
 * "/sources" схемы устройства. Схема может содержать информацию об источниках,
 * типы которых приведены в #HyScanSourceType. Информация об источнике находится
 * в ветке "/sources/source-name", где source-name - название источника. Для
 * определения названия источника и его типа необходимо использовать функции
 * #hyscan_source_get_name_by_type и #hyscan_source_get_type_by_name.
 *
 * Отдельно приводится параметр определяющий информацию о программном
 * управлении излучением - "/sources/software-ping". Он имеет тип BOOLEAN
 * и является не обязательным. Если он отсутствует, программное управление
 * излучением невозможно.
 *
 * Для описания источника данных используются следующие параметры:
 *
 * - id - поле определяющее налчичие источника, тип STRING, обязательное;
 * - description - поле с описанием источника, тип STRING, необязательное;
 * - master - название ведущего источника данных, тип STRING, необязательное.
 *
 * Для источника данных может быть задано местоположение антенн по умолчанию.
 * Если местоположение антенны залано, должны быть определены все параметры.
 * Местоположение антенны задают следующие параметры:
 *
 * - antenna/position/x - смещение антенны по оси X, тип DOUBLE;
 * - antenna/position/y - смещение антенны по оси Y, тип DOUBLE;
 * - antenna/position/z - смещение антенны по оси Z, тип DOUBLE;
 * - antenna/position/psi - поворот антенны по курсу, тип DOUBLE;
 * - antenna/position/gamma - поворот антенны по крену, тип DOUBLE;
 * - antenna/position/theta - поворот антенны по дифференту, тип DOUBLE.
 *
 * Характеристики приёмной антенны определяются следующие параметры:
 *
 * - antenna/pattern/vertical - диаграмма направленности в вертикальной плоскости, тип DOUBLE;
 * - antenna/pattern/horizontal - диаграмма направленности в горизонтальной плоскости, тип DOUBLE;
 * - antenna/frequency - центральная частота, тип DOUBLE;
 * - antenna/bandwidth - полоса, тип DOUBLE.
 *
 * Если источник данных является ведомым, допускается не задавать характеристики
 * приёмной антенны. В этом случае используются параметры ведущего источника.
 *
 * Наличие приёмника и его характеристики определяются параметром -
 * "receiver/capabilities". Ведущий источник данных всегда должен иметь
 * приёмник, у ведомого он может отсутствовать. Если приёмник имеет ручной
 * режим работы, задаются следующие параметры:
 *
 * - receiver/min-time - минимальной время приёма данных, тип DOUBLE;
 * - receiver/max-time - максимальное время приёма данных, тип DOUBLE.
 *
 * Приёмник может иметь несколько приёмных каналов, их наличие и характеристики
 * определяются следующими параметрами:
 *
 * - channels/index/antenna/offset/vertical - вертикальное смещение антенны в блоке, тип DOUBLE;
 * - channels/index/antenna/offset/horizontal - горизонтальное смещение антенны в блоке, тип DOUBLE;
 * - channels/index/adc/offset - смещение 0 АЦП, тип INTEGER;
 * - channels/index/adc/vref - опорное напряжение АЦП, тип DOUBLE.
 *
 * Где index - номер приёмного канала 1, 2, 3 и т.д.
 *
 * Наличие генератора и его характеристики определяются параметром -
 * "generator/capabilities". Возможности генератора, определённые этим параметром,
 * должны совпадать с остальными характеристиками. Ведущий источник данных всегда
 * должен иметь генератор, у ведомого он может отсутствовать.
 *
 * Генератор может иметь преднастроенные режимы работы. Их список и характеристики
 * приводятся в подветке "generator/presets". Каждый преднастроенный режим
 * имеет уникальный номер, который используется при его включении. Название
 * и описание преднастроенного режима определяются из параметра. Например,
 * для преднастроенных режимов с номерами 1, 2 и 3 задаются следующие параметры:
 *
 * - generator/presets/1 - преднастроенный режим 1, тип INTEGER;
 * - generator/presets/2 - преднастроенный режим 2, тип INTEGER;
 * - generator/presets/3 - преднастроенный режим 3, тип INTEGER.
 *
 * Возможность использовать автоматический выбор сигнала определяется параметром
 * "generator/automatic".  Он имеет тип BOOLEAN и является не обязательным.
 * Если он отсутствует, автоматический выбор сигнала невозможен.
 *
 * Если генератор может излучать тональный сигнал, должны быть заданы следующие
 * параметры:
 *
 * - tone/min-duration - минимальная длительность сигнала, тип DOUBLE;
 * - tone/max-duration - максимальная длительность сигнала, тип DOUBLE;
 * - tone/dirty-cycle - допустимая скважность, тип DOUBLE.
 *
 * Если генератор может излучать ЛЧМ сигнал, должны быть заданы следующие
 * параметры:
 *
 * - lfm/min-duration - минимальная длительность сигнала, тип DOUBLE;
 * - lfm/max-duration - максимальная длительность сигнала, тип DOUBLE;
 * - lfm/dirty-cycle - допустимая скважность, тип DOUBLE.
 *
 * Наличие системы ВАРУ и её характеристики определяются параметром -
 * "tvg/capabilities". Возможности системы ВАРУ, определённые этим параметром,
 * должны совпадать с остальными характеристиками.
 *
 * Если система ВАРУ имеет ручной режим работы, задаются следующие параметры:
 *
 * - tvg/min-gain - минимальное значение коэффициента усиления, тип DOUBLE;
 * - tvg/max-gain - максимальное значение коэффициента усиления, тип DOUBLE;
 * - tvg/can-decrease - возможность уменьшения коэффициента усиления, тип BOOLEAN.
 *
 * Пример всех параметров для левого борта ГБО:
 *
 * /sources/ss-port/id
 * /sources/ss-port/description
 * /sources/ss-port/master
 * /sources/ss-port/antenna/position/x
 * /sources/ss-port/antenna/position/y
 * /sources/ss-port/antenna/position/z
 * /sources/ss-port/antenna/position/psi
 * /sources/ss-port/antenna/position/gamma
 * /sources/ss-port/antenna/position/theta
 * /sources/ss-port/antenna/pattern/vertical
 * /sources/ss-port/antenna/pattern/horizontal
 * /sources/ss-port/antenna/frequency
 * /sources/ss-port/antenna/bandwidth
 * /sources/ss-port/receiver/capabilities
 * /sources/ss-port/receiver/min-time
 * /sources/ss-port/receiver/max-time
 * /sources/ss-port/channels/1/antenna/offset/vertical
 * /sources/ss-port/channels/1/antenna/offset/horizontal
 * /sources/ss-port/channels/1/adc/offset
 * /sources/ss-port/channels/1/adc/vref
 * /sources/ss-port/generator/capabilities
 * /sources/ss-port/generator/automatic
 * /sources/ss-port/generator/presets/1
 * /sources/ss-port/generator/presets/2
 * /sources/ss-port/generator/presets/3
 * /sources/ss-port/tone/min-duration
 * /sources/ss-port/tone/max-duration
 * /sources/ss-port/tone/dirty-cycle
 * /sources/ss-port/lfm/min-duration
 * /sources/ss-port/lfm/max-duration
 * /sources/ss-port/lfm/dirty-cycle
 * /sources/ss-port/tvg/capabilities
 * /sources/ss-port/tvg/min-gain
 * /sources/ss-port/tvg/max-gain
 * /sources/ss-port/tvg/can-decrease
 *
 * Дополнительные настройки, если они необходимы, должны находится в ветке
 * "/params". Описание этой ветки приведено в #HyscanDeviceSchema.
 *
 * Для создания схемы используется класс #HyScanDataSchemaBuilder, указатель
 * на который передаётся в функцию #hyscan_sonar_schema_new.
 *
 * Источник гидролокационных данных добавляется с помощью функции
 * #hyscan_sonar_schema_source_add. После этого можно задавать его параметры
 * с помощью остальных функций.
 *
 * Функция hyscan_sonar_schema_source_add_full добавляет источник гидролокационных
 * данных описанный стуруктурой #HyScanSonarInfoSource.
 */

#include "hyscan-sonar-schema.h"
#include "hyscan-device-schema.h"

enum
{
  PROP_O,
  PROP_BUILDER
};

struct _HyScanSonarSchemaPrivate
{
  HyScanDataSchemaBuilder     *builder;                        /* Строитель схемы. */
  GHashTable                  *sources;                        /* Наличие источников данных. */
  GHashTable                  *slaves;                         /* Таблица ведомых/ведущих источников данных. */
};

static void      hyscan_sonar_schema_set_property              (GObject                 *object,
                                                                guint                    prop_id,
                                                                const GValue            *value,
                                                                GParamSpec              *pspec);

static void      hyscan_sonar_schema_object_constructed        (GObject                 *object);
static void      hyscan_sonar_schema_object_finalize           (GObject                 *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSonarSchema, hyscan_sonar_schema, G_TYPE_OBJECT)

static void
hyscan_sonar_schema_class_init (HyScanSonarSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_sonar_schema_set_property;

  object_class->constructed = hyscan_sonar_schema_object_constructed;
  object_class->finalize = hyscan_sonar_schema_object_finalize;

  g_object_class_install_property (object_class, PROP_BUILDER,
    g_param_spec_object ("builder", "Builder", "Schema builder", HYSCAN_TYPE_DATA_SCHEMA_BUILDER,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_sonar_schema_init (HyScanSonarSchema *schema)
{
  schema->priv = hyscan_sonar_schema_get_instance_private (schema);
}

static void
hyscan_sonar_schema_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  HyScanSonarSchema *schema = HYSCAN_SONAR_SCHEMA (object);
  HyScanSonarSchemaPrivate *priv = schema->priv;

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
hyscan_sonar_schema_object_constructed (GObject *object)
{
  HyScanSonarSchema *schema = HYSCAN_SONAR_SCHEMA (object);
  HyScanSonarSchemaPrivate *priv = schema->priv;
  HyScanDataSchemaBuilder *builder = priv->builder;

  if (builder == NULL)
    return;

  if (!hyscan_device_schema_set_id (builder))
    return;

  priv->sources = g_hash_table_new_full (g_direct_hash, g_direct_equal,
                                         NULL, (GDestroyNotify)hyscan_sonar_info_capabilities_free);

  priv->slaves = g_hash_table_new (g_direct_hash, g_direct_equal);

}

static void
hyscan_sonar_schema_object_finalize (GObject *object)
{
  HyScanSonarSchema *schema = HYSCAN_SONAR_SCHEMA (object);
  HyScanSonarSchemaPrivate *priv = schema->priv;

  g_clear_pointer (&priv->sources, g_hash_table_unref);
  g_clear_pointer (&priv->slaves, g_hash_table_unref);
  g_clear_object (&priv->builder);

  G_OBJECT_CLASS (hyscan_sonar_schema_parent_class)->finalize (object);
}

/**
 * hyscan_sonar_schema_new:
 * @builder: указатель на #HyScanDataSchemaBuilder
 *
 * Функция создаёт новый объект HyScanSonarSchema.
 *
 * Returns: #HyScanSonarSchema. Для удаления #g_object_unref.
 */
HyScanSonarSchema *
hyscan_sonar_schema_new (HyScanDataSchemaBuilder *builder)
{
  return g_object_new (HYSCAN_TYPE_SONAR_SCHEMA,
                       "builder", builder,
                       NULL);
}

/**
 * hyscan_sonar_schema_set_software_ping:
 * @schema: указатель на #HyScanSonarSchema
 *
 * Функция добавляет в схему информацию о программном управлении излучением.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_set_software_ping (HyScanSonarSchema *schema)
{
  HyScanDataSchemaBuilder *builder;

  gboolean status = FALSE;
  const gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  key_id = "/sources/software-ping";
  if (hyscan_data_schema_builder_key_boolean_create (builder, key_id, "software-ping", NULL, TRUE))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

  return status;
}

/**
 * hyscan_sonar_schema_source_add_full:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @info: (transfer none): параметры сточника данныx
 *
 * Функция добавляет в схему описание источника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_add_full (HyScanSonarSchema     *schema,
                                     HyScanSourceType       source,
                                     HyScanSonarInfoSource *info)
{
  gboolean status;

  /* Источник гидролокационных данных. */
  status = hyscan_sonar_schema_source_add (schema, source,
                                           info->capabilities->receiver,
                                           info->capabilities->generator,
                                           info->capabilities->tvg);
  if (!status)
    return FALSE;

  /* Описание источника данных. */
  if (info->description != NULL)
    if (!hyscan_sonar_schema_source_set_description (schema, source, info->description))
      return FALSE;

  /* Ведущий источник данных. */
  if (info->master != HYSCAN_SOURCE_INVALID)
    if (!hyscan_sonar_schema_source_set_master (schema, source, info->master))
      return FALSE;

  /* Местоположение антенн по умолчанию. */
  if (info->position != NULL)
    if (!hyscan_sonar_schema_source_set_position (schema, source, info->position))
      return FALSE;

  /* Параметры антенны. */
  if (info->antenna != NULL)
    {
      status = hyscan_sonar_schema_antenna_set_params (schema, source,
                                                       info->antenna->vpattern,
                                                       info->antenna->hpattern,
                                                       info->antenna->frequency,
                                                       info->antenna->bandwidth);
      if (!status)
        return FALSE;
    }

  /* Приёмник данных. */
  if (info->capabilities->receiver & HYSCAN_SONAR_RECEIVER_MODE_MANUAL)
    {
      status = hyscan_sonar_schema_receiver_set_params (schema, source,
                                                        info->receiver->min_time,
                                                        info->receiver->max_time);
      if (!status)
        return FALSE;
    }

  /* Приёмные каналы. */
  if (info->channels != NULL)
    {
      GList *cur_channel = info->channels;

      while (cur_channel != NULL)
        {
          HyScanSonarInfoChannel *channel = cur_channel->data;

          status = hyscan_sonar_schema_receiver_add_channel (schema, source,
                                                             channel->channel,
                                                             channel->antenna_voffset,
                                                             channel->antenna_hoffset,
                                                             channel->adc_offset,
                                                             channel->adc_vref);
          if (!status)
            return FALSE;

          cur_channel = g_list_next (cur_channel);
        }
    }

  /* Генератор сигналов. */
  if (info->generator != NULL)
    {
      /* Автоматический выбор сигнала. */
      if (info->generator->automatic)
        if (!hyscan_sonar_schema_generator_add_auto (schema, source))
          return FALSE;

      /* Параметры тонального сигнала. */
      if (info->generator->tone != NULL)
        {
          status = hyscan_sonar_schema_generator_set_params (schema, source,
                                                             HYSCAN_SONAR_GENERATOR_SIGNAL_TONE,
                                                             info->generator->tone->min_duration,
                                                             info->generator->tone->max_duration,
                                                             info->generator->tone->dirty_cycle);
          if (!status)
            return FALSE;
        }

      /* Параметры ЛЧМ сигнала. */
      if (info->generator->lfm != NULL)
        {
          status = hyscan_sonar_schema_generator_set_params (schema, source,
                                                             HYSCAN_SONAR_GENERATOR_SIGNAL_LFM,
                                                             info->generator->lfm->min_duration,
                                                             info->generator->lfm->max_duration,
                                                             info->generator->lfm->dirty_cycle);
          if (!status)
            return FALSE;
        }

      /* Преднастройки. */
      if (info->generator->presets != NULL)
        {
          GList * cur_preset = info->generator->presets;

          while (cur_preset != NULL)
            {
              HyScanDataSchemaEnumValue *preset = cur_preset->data;

              status = hyscan_sonar_schema_generator_add_preset (schema, source,
                                                                 preset->value,
                                                                 preset->name,
                                                                 preset->description);
              if (!status)
                return FALSE;

              cur_preset = g_list_next (cur_preset);
            }

        }
    }

  /* Система ВАРУ. */
  if (info->tvg != NULL)
    {
      status = hyscan_sonar_schema_tvg_set_params (schema, source,
                                                   info->tvg->min_gain,
                                                   info->tvg->max_gain,
                                                   info->tvg->can_decrease);
      if (!status)
        return FALSE;
    }

  return TRUE;
}

/**
 * hyscan_sonar_schema_source_add:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @master: ведущий источник данных или #HYSCAN_SOURCE_INVALID
 * @receiver_capabilities: флаги возможных режимов работы приёмника
 * @generator_capabilities: флаги возможных режимов работы генератора
 * @tvg_capabilities: флаги возможных режимов работы ВАРУ
 *
 * Функция добавляет в схему описание источника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_add (HyScanSonarSchema            *schema,
                                HyScanSourceType              source,
                                HyScanSonarReceiverModeType   receiver_capabilities,
                                HyScanSonarGeneratorModeType  generator_capabilities,
                                HyScanSonarTVGModeType        tvg_capabilities)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *cap_string;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  if (!hyscan_source_is_sonar (source))
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s", source_name);

  /* Признак наличия источника данных. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/id", prefix);
  if (hyscan_data_schema_builder_key_string_create (builder, key_id, "id", NULL, source_name))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Режимы работы приёмника. */
  if (receiver_capabilities)
    {
      cap_string = g_strdup_printf ("%s %s",
        (receiver_capabilities & HYSCAN_SONAR_RECEIVER_MODE_MANUAL) ? "manual" : "",
        (receiver_capabilities & HYSCAN_SONAR_RECEIVER_MODE_AUTO) ? "auto" : "");

      status = FALSE;
      key_id = g_strdup_printf ("%s/receiver/capabilities", prefix);
      if (hyscan_data_schema_builder_key_string_create (builder, key_id, "capabilities", NULL, cap_string))
        status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

      g_free (cap_string);
      g_free (key_id);

      if (!status)
        goto exit;
    }

  /* Режимы работы генератора. */
  if (generator_capabilities)
    {
      cap_string = g_strdup_printf ("%s %s %s %s",
        (generator_capabilities & HYSCAN_SONAR_GENERATOR_MODE_PRESET) ? "preset" : "",
        (generator_capabilities & HYSCAN_SONAR_GENERATOR_MODE_AUTO) ? "auto" : "",
        (generator_capabilities & HYSCAN_SONAR_GENERATOR_MODE_SIMPLE) ? "simple" : "",
        (generator_capabilities & HYSCAN_SONAR_GENERATOR_MODE_EXTENDED) ? "extended" : "");

      status = FALSE;
      key_id = g_strdup_printf ("%s/generator/capabilities", prefix);
      if (hyscan_data_schema_builder_key_string_create (builder, key_id, "capabilities", NULL, cap_string))
        status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

      g_free (cap_string);
      g_free (key_id);

      if (!status)
        goto exit;
    }

  /* Режимы работы ВАРУ. */
  if (tvg_capabilities)
    {
      cap_string = g_strdup_printf ("%s %s %s %s",
        (tvg_capabilities & HYSCAN_SONAR_TVG_MODE_AUTO) ? "auto" : "",
        (tvg_capabilities & HYSCAN_SONAR_TVG_MODE_POINTS) ? "points" : "",
        (tvg_capabilities & HYSCAN_SONAR_TVG_MODE_LINEAR_DB) ? "linear-db" : "",
        (tvg_capabilities & HYSCAN_SONAR_TVG_MODE_LOGARITHMIC) ? "logarithmic" : "");

      status = FALSE;
      key_id = g_strdup_printf ("%s/tvg/capabilities", prefix);
      if (hyscan_data_schema_builder_key_string_create (builder, key_id, "capabilities", NULL, cap_string))
        status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

      g_free (cap_string);
      g_free (key_id);
    }

  if (status)
    {
      HyScanSonarInfoCapabilities capabilities;

      capabilities.receiver = receiver_capabilities;
      capabilities.generator = generator_capabilities;
      capabilities.tvg = tvg_capabilities;

      g_hash_table_insert (schema->priv->sources,
                           GINT_TO_POINTER (source),
                           hyscan_sonar_info_capabilities_copy (&capabilities));
    }

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_source_set_description:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @description: описание источника данныx
 *
 * Функция добавляет текстовое описание источника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_set_description (HyScanSonarSchema *schema,
                                            HyScanSourceType   source,
                                            const gchar       *description)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status = FALSE;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  if (!hyscan_source_is_sonar (source))
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  key_id = g_strdup_printf ("/sources/%s/description", source_name);
  if (hyscan_data_schema_builder_key_string_create (builder, key_id, "description", NULL, description))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  return status;
}

/**
 * hyscan_sonar_schema_source_set_master:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @master: ведущий источник данных или #HYSCAN_SOURCE_INVALID
 *
 * Функция устанавливает ведущий источник данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_set_master (HyScanSonarSchema *schema,
                                       HyScanSourceType   source,
                                       HyScanSourceType   master)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  const gchar *master_name;
  gboolean status = FALSE;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  if (!hyscan_source_is_sonar (source))
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  master_name = hyscan_source_get_name_by_type (master);
  if ((source_name == NULL) || (master_name == NULL))
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (master)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->slaves, GINT_TO_POINTER (source)))
    return FALSE;

  key_id = g_strdup_printf ("/sources/%s/master", source_name);
  if (hyscan_data_schema_builder_key_string_create (builder, key_id, "master", NULL, master_name))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (status)
    g_hash_table_insert (schema->priv->slaves, GINT_TO_POINTER (source), GINT_TO_POINTER (master));

  return status;
}

/**
 * hyscan_sonar_schema_source_set_position:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @position: (transfer none): местоположение антенны по умолчанию
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

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  if (g_hash_table_contains (schema->priv->slaves, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/antenna/position", source_name);

  /* Местоположение антенны. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/x", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "x", NULL, position->x))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/y", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "y", NULL,  position->y))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/z", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "z", NULL,  position->z))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/psi", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "psi", NULL, position->psi))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/gamma", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "gamma", NULL, position->gamma))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/theta", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "theta", NULL, position->theta))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_antenna_set_params:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @antenna_vpattern: диаграмма направленности антенны в вертикальной плоскости, рад
 * @antenna_hpattern: диаграмма направленности антенны в горизонтальной плоскости, рад
 * @antenna_frequency: центральная частота, Гц
 * @antenna_bandwidth: полоса пропускания, Гц
 *
 * Функция устанавливает параметры антенны источника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_antenna_set_params (HyScanSonarSchema *schema,
                                        HyScanSourceType   source,
                                        gdouble            antenna_vpattern,
                                        gdouble            antenna_hpattern,
                                        gdouble            antenna_frequency,
                                        gdouble            antenna_bandwidth)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s", source_name);

  /* Диаграмма направленности антенны в вертикальной плоскости. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/antenna/pattern/vertical", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "vertical-pattern", NULL, antenna_vpattern))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Диаграмма направленности антенны в горизонтальной плоскости. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/antenna/pattern/horizontal", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "horizontal-pattern", NULL, antenna_hpattern))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Центральная частота. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/antenna/frequency", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "antenna-frequency", NULL, antenna_frequency))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Полоса пропускания. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/antenna/bandwidth", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "antenna-bandwidth", NULL, antenna_bandwidth))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_receiver_set_params:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данныx
 * @min_time: минимальное время приёма эхосигнала, с
 * @max_time: максимальное время приёма эхосигнала, с
 *
 * Функция устанавливает предельные параметры приёмника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_receiver_set_params (HyScanSonarSchema *schema,
                                         HyScanSourceType   source,
                                         gdouble            min_time,
                                         gdouble            max_time)
{
  HyScanDataSchemaBuilder *builder;
  HyScanSonarInfoCapabilities *capabilities;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  capabilities = g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source));
  if ((capabilities == NULL) || !(capabilities->receiver & HYSCAN_SONAR_RECEIVER_MODE_MANUAL))
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/receiver", source_name);

  /* Время приёма эхосигнала источником данных. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/min-time", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "min-time", NULL, min_time))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/max-time", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "max-time", NULL, max_time))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_receiver_add_channel:
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
hyscan_sonar_schema_receiver_add_channel (HyScanSonarSchema *schema,
                                          HyScanSourceType   source,
                                          guint              channel,
                                          gdouble            antenna_voffset,
                                          gdouble            antenna_hoffset,
                                          gint               adc_offset,
                                          gdouble            adc_vref)
{
  HyScanDataSchemaBuilder *builder;
  HyScanSonarInfoCapabilities *capabilities;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  capabilities = g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source));
  if ((capabilities == NULL) || !capabilities->receiver)
    return FALSE;

  prefix = g_strdup_printf ("/sources/%s/receiver/channels/%d", source_name, channel);

  /* Вертикальное смещение антенны в блоке. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/antenna/offset/vertical", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "vertical-offset", NULL,  antenna_voffset))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Горизонтальное смещение антенны в блоке. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/antenna/offset/horizontal", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "horizontal-offset", NULL, antenna_hoffset))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Смещение 0 АЦП. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/adc/offset", prefix);
  if (hyscan_data_schema_builder_key_integer_create (builder, key_id, "offset", NULL, adc_offset))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  /* Опорное напряжение АЦП. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/adc/vref", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "vref", NULL, adc_vref))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

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
  HyScanSonarInfoCapabilities *capabilities;
  const gchar *source_name;
  gboolean status;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  capabilities = g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source));
  if ((capabilities == NULL) ||
      !(capabilities->generator & HYSCAN_SONAR_GENERATOR_MODE_PRESET))
    {
      return FALSE;
    }

  /* Преднастройка генератора. */
  status = FALSE;
  key_id = g_strdup_printf ("/sources/%s/generator/presets/%d", source_name, preset_id);
  if (hyscan_data_schema_builder_key_integer_create (builder, key_id, preset_name, preset_description, preset_id))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  return status;
}

/**
 * hyscan_sonar_schema_generator_add_auto:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @capabilities: флаги возможных режимов работы генератора
 * @signals: флаги возможных типов сигналов
 *
 * Функция задаёт возможность использования "автоматического" сигнала.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_generator_add_auto (HyScanSonarSchema *schema,
                                        HyScanSourceType   source)
{
  HyScanDataSchemaBuilder *builder;
  HyScanSonarInfoCapabilities *capabilities;
  const gchar *source_name;
  gboolean status;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  capabilities = g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source));
  if ((capabilities == NULL) ||
      (capabilities->generator == HYSCAN_SONAR_GENERATOR_MODE_NONE) ||
      (capabilities->generator == HYSCAN_SONAR_GENERATOR_MODE_PRESET))
    {
      return FALSE;
    }

  status = FALSE;
  key_id = g_strdup_printf ("/sources/%s/generator/automatic", source_name);
  if (hyscan_data_schema_builder_key_boolean_create (builder, key_id, "automatic", NULL, TRUE))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  return status;
}

/**
 * hyscan_sonar_schema_generator_set_params:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @signal: тип сигнала
 * @min_duration: минимальная длительность, с
 * @max_duration: максимальная длительность, с
 * @dirty_cycle: допустимая скважность
 *
 * Функция устанавливает предельные параметры сигнала.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_generator_set_params (HyScanSonarSchema             *schema,
                                          HyScanSourceType               source,
                                          HyScanSonarGeneratorSignalType signal,
                                          gdouble                        min_duration,
                                          gdouble                        max_duration,
                                          gdouble                        dirty_cycle)
{
  HyScanDataSchemaBuilder *builder;
  HyScanSonarInfoCapabilities *capabilities;
  const gchar *source_name;
  const gchar *signal_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  if (signal == HYSCAN_SONAR_GENERATOR_SIGNAL_TONE)
    signal_name = "tone";
  else if (signal == HYSCAN_SONAR_GENERATOR_SIGNAL_LFM)
    signal_name = "lfm";
  else
    return FALSE;

  capabilities = g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source));
  if ((capabilities == NULL) ||
      (capabilities->generator == HYSCAN_SONAR_GENERATOR_MODE_NONE) ||
      (capabilities->generator == HYSCAN_SONAR_GENERATOR_MODE_PRESET))
    {
      return FALSE;
    }

  prefix = g_strdup_printf ("/sources/%s/generator/%s", source_name, signal_name);

  /* Параметры сигнала. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/min-duration", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "min-duration", NULL, min_duration))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/max-duration", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "max-duration", NULL, max_duration))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/dirty-cycle", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "dirty-cycle", NULL, dirty_cycle))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

exit:
  g_free (prefix);

  return status;
}

/**
 * hyscan_sonar_schema_tvg_set_params:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @min_gain: минимальное значение коэффициента усиления, дБ
 * @max_gain: максимальное значение коэффициента усиления, дБ
 * @can_decrease: возможность уменьшения коэффициента усиления
 *
 * Функция устанавливает предельные параметры системы ВАРУ.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_tvg_set_params (HyScanSonarSchema *schema,
                                    HyScanSourceType   source,
                                    gdouble            min_gain,
                                    gdouble            max_gain,
                                    gboolean           can_decrease)
{
  HyScanDataSchemaBuilder *builder;
  HyScanSonarInfoCapabilities *capabilities;
  const gchar *source_name;
  gboolean status;
  gchar *prefix;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    return FALSE;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return FALSE;

  capabilities = g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source));
  if ((capabilities == NULL) ||
      (capabilities->tvg == HYSCAN_SONAR_TVG_MODE_NONE) ||
      (capabilities->tvg == HYSCAN_SONAR_TVG_MODE_AUTO))
    {
      return FALSE;
    }

  prefix = g_strdup_printf ("/sources/%s/tvg", source_name);

  /* Параметры ВАРУ. */
  status = FALSE;
  key_id = g_strdup_printf ("%s/min-gain", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "min-gain", NULL, min_gain))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/max-gain", prefix);
  if (hyscan_data_schema_builder_key_double_create (builder, key_id, "max-gain", NULL, max_gain))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

  if (!status)
    goto exit;

  status = FALSE;
  key_id = g_strdup_printf ("%s/can-decrease", prefix);
  if (hyscan_data_schema_builder_key_boolean_create (builder, key_id, "can-decrease", NULL, can_decrease))
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  g_free (key_id);

exit:
  g_free (prefix);

  return status;
}
