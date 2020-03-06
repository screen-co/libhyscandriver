/* hyscan-sonar-schema.c
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
 * Два источника данных могут быть связанны друг с другом. Обычно такая связь
 * возникает если используется один и тот же приёмник данных, но обработка их
 * производится по разному. Например, в интерферометрическом гидролокаторе
 * возможно получение как акустического изображения, так и батиметрических
 * данных. Информация о такой связи может использоваться программой управления,
 * чтобы соответствующим образом сформировать GUI.
 *
 * Для описания источника данных используются следующие параметры:
 *
 * - dev-id - уникальный идентификатор устройства, тип STRING, обязательное;
 * - description - описание источника данных, тип STRING, необязательное;
 * - link - название связанного источника данных, тип STRING, необязательное.
 *
 * Для источника данных может быть задано смещение антенны по умолчанию.
 * Если смещение задано, должны быть определены все параметры.
 * Смещение задают следующие параметры:
 *
 * - antenna/offset/starboard - смещение антенны на правый борт, тип DOUBLE;
 * - antenna/offset/forward - смещение антенны к носу судна, тип DOUBLE;
 * - antenna/offset/vertical - смещение антенны по вертикали вниз, тип DOUBLE;
 * - antenna/offset/yaw - разворот антенны от продольной оси судна, тип DOUBLE;
 * - antenna/offset/pitch - разворот антенны от вертикальной оси судна, тип DOUBLE;
 * - antenna/offset/roll - разворот антенны от поперечной оси судна, тип DOUBLE.
 *
 * Подробное описание этих параметров приведено в #HyScanAntennaOffset.
 *
 * Наличие приёмника и его характеристики определяются параметром -
 * "receiver/capabilities". Если приёмник имеет ручной режим работы, задаётся
 * параметр receiver/time - диапазон времени приёма данных, тип DOUBLE.
 *
 * Режимы работы генератора приводятся в ветке "generator". Каждый режим
 * имеет уникальный номер, который используется при его включении. Название
 * и описание режима определяются из параметра. Название параметра используется
 * в качестве уникального идентификатора, а его значение по умолчанию в
 * качестве уникального номера.
 *
 * - generator/preset-1 - режим 1, тип INTEGER;
 * - generator/preset-2 - режим 2, тип INTEGER;
 * - generator/preset-3 - режим 3, тип INTEGER.
 *
 * Наличие системы ВАРУ и её характеристики определяются параметром -
 * "tvg/capabilities". Возможности системы ВАРУ, определённые этим параметром,
 * должны совпадать с остальными характеристиками.
 *
 * Если система ВАРУ имеет ручной режим работы, задаются следующие параметры:
 *
 * - tvg/gain - диапазон значений коэффициента усиления, тип DOUBLE;
 * - tvg/decrease - возможность уменьшения коэффициента усиления, тип BOOLEAN.
 *
 * Пример всех параметров для левого борта ГБО:
 *
 * /sources/ss-port/dev-id
 * /sources/ss-port/description
 * /sources/ss-port/link
 * /sources/ss-port/offset/starboard
 * /sources/ss-port/offset/forward
 * /sources/ss-port/offset/vertical
 * /sources/ss-port/offset/yaw
 * /sources/ss-port/offset/pitch
 * /sources/ss-port/offset/roll
 * /sources/ss-port/receiver/capabilities
 * /sources/ss-port/receiver/time
 * /sources/ss-port/generator/preset-1
 * /sources/ss-port/generator/preset-2
 * /sources/ss-port/generator/preset-3
 * /sources/ss-port/tvg/capabilities
 * /sources/ss-port/tvg/gain
 * /sources/ss-port/tvg/decrease
 *
 * Дополнительные параметры, если они необходимы, должны находится в ветках
 * "/params" и "/system". Состояние устройства приводится в ветке "/state",
 * а общая информация об устройстве и драйвере в ветке "/info". Описание этих
 * веток приведено в #HyscanDeviceSchema.
 *
 * Для создания схемы используется класс #HyScanDeviceSchema, указатель
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

struct _HyScanSonarSchemaPrivate
{
  HyScanDataSchemaBuilder     *builder;                /* Строитель схемы. */
  GHashTable                  *sources;                /* Наличие источников данных. */
};

static void      hyscan_sonar_schema_set_property      (GObject               *object,
                                                        guint                  prop_id,
                                                        const GValue          *value,
                                                        GParamSpec            *pspec);
static void      hyscan_sonar_schema_object_finalize   (GObject               *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanSonarSchema, hyscan_sonar_schema, G_TYPE_OBJECT)

static void
hyscan_sonar_schema_class_init (HyScanSonarSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_sonar_schema_set_property;
  object_class->finalize = hyscan_sonar_schema_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "Device schema", HYSCAN_TYPE_DEVICE_SCHEMA,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_sonar_schema_init (HyScanSonarSchema *schema)
{
  schema->priv = hyscan_sonar_schema_get_instance_private (schema);
  schema->priv->sources = g_hash_table_new (g_direct_hash, g_direct_equal);
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
    case PROP_SCHEMA:
      priv->builder = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_sonar_schema_object_finalize (GObject *object)
{
  HyScanSonarSchema *schema = HYSCAN_SONAR_SCHEMA (object);
  HyScanSonarSchemaPrivate *priv = schema->priv;

  g_hash_table_unref (priv->sources);
  g_clear_object (&priv->builder);

  G_OBJECT_CLASS (hyscan_sonar_schema_parent_class)->finalize (object);
}

/**
 * hyscan_sonar_schema_new:
 * @schema: указатель на #HyScanDeviceSchema
 *
 * Функция создаёт новый объект HyScanSonarSchema.
 *
 * Returns: #HyScanSonarSchema. Для удаления #g_object_unref.
 */
HyScanSonarSchema *
hyscan_sonar_schema_new (HyScanDeviceSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_SONAR_SCHEMA,
                       "schema", schema,
                       NULL);
}

/**
 * hyscan_sonar_schema_source_add_full:
 * @schema: указатель на #HyScanSonarSchema
 * @info: (transfer none): параметры сточника данныx
 *
 * Функция добавляет в схему описание источника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_add_full (HyScanSonarSchema     *schema,
                                     HyScanSonarInfoSource *info)
{
  HyScanSourceType source = info->source;
  gboolean status;

  /* Источник гидролокационных данных. */
  status = hyscan_sonar_schema_source_add (schema, source,
                                           info->dev_id,
                                           info->description);
  if (!status)
    goto exit;

  status = hyscan_sonar_schema_source_link (schema, source, info->link);
  if (!status)
    goto exit;

  /* Смещение антенны по умолчанию. */
  if (info->offset != NULL)
    {
      status = hyscan_sonar_schema_source_set_offset (schema, source, info->offset);
      if (!status)
        goto exit;
    }

  /* Приёмник данных. */
  if (info->receiver != NULL)
    {
      status = hyscan_sonar_schema_receiver_set_params (schema, source,
                                                        info->receiver->capabilities,
                                                        info->receiver->min_time,
                                                        info->receiver->max_time);
      if (!status)
        goto exit;
    }

  /* Генератор сигналов. */
  if (info->presets != NULL)
    {
      /* Преднастройки. */
      if (info->presets != NULL)
        {
          GList * cur_preset = info->presets;

          while (cur_preset != NULL)
            {
              HyScanDataSchemaEnumValue *preset = cur_preset->data;

              status = hyscan_sonar_schema_generator_add_preset (schema, source,
                                                                 preset->id,
                                                                 preset->value,
                                                                 preset->name,
                                                                 preset->description);
              if (!status)
                goto exit;

              cur_preset = g_list_next (cur_preset);
            }

        }
    }

  /* Система ВАРУ. */
  if (info->tvg != NULL)
    {
      status = hyscan_sonar_schema_tvg_set_params (schema, source,
                                                   info->tvg->capabilities,
                                                   info->tvg->min_gain,
                                                   info->tvg->max_gain,
                                                   info->tvg->decrease);
      if (!status)
        goto exit;
    }

exit:
  return status;
}

/**
 * hyscan_sonar_schema_source_add:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @dev_id: уникальный идентификатор устройства
 * @description: описание источника данных
 * @master: ведущий источник данных или #HYSCAN_SOURCE_INVALID
 *
 * Функция добавляет в схему описание источника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_add (HyScanSonarSchema            *schema,
                                HyScanSourceType              source,
                                const gchar                  *dev_id,
                                const gchar                  *description)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (!hyscan_source_is_sonar (source))
    goto exit;

  if (dev_id == NULL)
    goto exit;

  if (g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    goto exit;

  /* Уникальный идентификатор устройства. */
  SONAR_PARAM_NAME (source, "dev-id", NULL);
  status = hyscan_data_schema_builder_key_string_create (builder, key_id, "dev-id", NULL, dev_id);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  /* Описание источника данных. */
  if (description != NULL)
    {
      SONAR_PARAM_NAME (source, "description", NULL);
      status = hyscan_data_schema_builder_key_string_create (builder, key_id, "description", NULL, description);
      if (!status)
        goto exit;

      status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
      if (!status)
        goto exit;
    }

  if (status)
    g_hash_table_insert (schema->priv->sources, GINT_TO_POINTER (source), GINT_TO_POINTER (source));

exit:
  return status;
}

/**
 * hyscan_sonar_schema_source_link:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @link: связанный источник данных
 *
 * Функция определяет связь двух источников данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_link (HyScanSonarSchema *schema,
                                 HyScanSourceType   source,
                                 HyScanSourceType   link)
{
  HyScanDataSchemaBuilder *builder;
  const gchar *link_name;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    goto exit;
  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (link)))
    {
      status = TRUE;
      goto exit;
    }

  link_name = hyscan_source_get_id_by_type (link);
  if (link_name == NULL)
    goto exit;

  /* Связанный источник данных. */
  SONAR_PARAM_NAME (source, "link", NULL);
  status = hyscan_data_schema_builder_key_string_create (builder, key_id, "link", NULL,
                                                         hyscan_source_get_id_by_type (link));
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  SONAR_PARAM_NAME (link, "link", NULL);
  status = hyscan_data_schema_builder_key_string_create (builder, key_id, "link", NULL,
                                                         hyscan_source_get_id_by_type (source));
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

exit:
  return status;
}

/**
 * hyscan_sonar_schema_source_set_offset:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @offset: (transfer none): смещение антенны по умолчанию
 *
 * Функция устанавливает смещение антенны по умолчанию.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_source_set_offset (HyScanSonarSchema   *schema,
                                       HyScanSourceType     source,
                                       HyScanAntennaOffset *offset)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (!g_hash_table_contains (schema->priv->sources, GINT_TO_POINTER (source)))
    goto exit;

  /* Смещение антенны. */
  SONAR_PARAM_NAME (source, "offset/starboard", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "starboard", NULL, offset->starboard);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  SONAR_PARAM_NAME (source, "offset/forward", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "forward", NULL,  offset->forward);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  SONAR_PARAM_NAME (source, "offset/vertical", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "vertical", NULL,  offset->vertical);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  SONAR_PARAM_NAME (source, "offset/yaw", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "yaw", NULL, offset->yaw);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  SONAR_PARAM_NAME (source, "offset/pitch", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "pitch", NULL, offset->pitch);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  SONAR_PARAM_NAME (source, "offset/roll", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "roll", NULL, offset->roll);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

exit:
  return status;
}

/**
 * hyscan_sonar_schema_receiver_set_params:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данныx
 * @capabilities: флаги возможных режимов работы приёмника
 * @min_time: минимальное время приёма эхосигнала, с
 * @max_time: максимальное время приёма эхосигнала, с
 *
 * Функция устанавливает предельные параметры приёмника данных.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_receiver_set_params (HyScanSonarSchema           *schema,
                                         HyScanSourceType             source,
                                         HyScanSonarReceiverModeType  capabilities,
                                         gdouble                      min_time,
                                         gdouble                      max_time)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (!g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source)))
    goto exit;

  /* Режимы работы приёмника. */
  if (capabilities)
    {
      gchar *cap_string = g_strdup_printf ("%s %s",
        (capabilities & HYSCAN_SONAR_RECEIVER_MODE_MANUAL) ? "manual" : "",
        (capabilities & HYSCAN_SONAR_RECEIVER_MODE_AUTO) ? "auto" : "");

      SONAR_PARAM_NAME (source, "receiver/capabilities", NULL);
      status = hyscan_data_schema_builder_key_string_create (builder, key_id, "capabilities", NULL, cap_string);
      g_free (cap_string);
      if (!status)
        goto exit;

      status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
      if (!status)
        goto exit;
    }

  /* Время приёма эхосигнала источником данных. */
  SONAR_PARAM_NAME (source, "receiver/time", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "time", NULL, min_time);
  if (!status)
    goto exit;

   status = hyscan_data_schema_builder_key_double_range (builder, key_id, min_time, max_time, 1.0);
   if (!status)
     goto exit;

   status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

exit:
  return status;
}

/**
 * hyscan_sonar_schema_generator_add_preset:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @id: идентификатор режима
 * @id: идентификатор режима
 * @name: название режима
 * @description: описание режима
 *
 * Функция добавляет рабочий режим генератора.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_generator_add_preset (HyScanSonarSchema *schema,
                                          HyScanSourceType   source,
                                          const gchar       *id,
                                          guint              value,
                                          const gchar       *name,
                                          const gchar       *description)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (!g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source)))
    goto exit;

  /* Преднастройка генератора. */
  SONAR_PARAM_NAME (source, "generator", id, NULL);
  status = hyscan_data_schema_builder_key_integer_create (builder, key_id, name, description, value);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);

exit:
  return status;
}

/**
 * hyscan_sonar_schema_tvg_set_params:
 * @schema: указатель на #HyScanSonarSchema
 * @source: тип источника данных
 * @capabilities: флаги возможных режимов работы ВАРУ
 * @min_gain: минимальное значение коэффициента усиления, дБ
 * @max_gain: максимальное значение коэффициента усиления, дБ
 * @decrease: возможность уменьшения коэффициента усиления
 *
 * Функция устанавливает предельные параметры системы ВАРУ.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_schema_tvg_set_params (HyScanSonarSchema      *schema,
                                    HyScanSourceType        source,
                                    HyScanSonarTVGModeType  capabilities,
                                    gdouble                 min_gain,
                                    gdouble                 max_gain,
                                    gboolean                decrease)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_SONAR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (!g_hash_table_lookup (schema->priv->sources, GINT_TO_POINTER (source)))
    goto exit;

  /* Режимы работы ВАРУ. */
  if (capabilities)
    {
      gchar *cap_string = g_strdup_printf ("%s %s %s %s",
        (capabilities & HYSCAN_SONAR_TVG_MODE_AUTO) ? "auto" : "",
        (capabilities & HYSCAN_SONAR_TVG_MODE_CONSTANT) ? "constant" : "",
        (capabilities & HYSCAN_SONAR_TVG_MODE_LINEAR_DB) ? "linear-db" : "",
        (capabilities & HYSCAN_SONAR_TVG_MODE_LOGARITHMIC) ? "logarithmic" : "");

      SONAR_PARAM_NAME (source, "tvg/capabilities", NULL);
      status = hyscan_data_schema_builder_key_string_create (builder, key_id, "capabilities", NULL, cap_string);
      g_free (cap_string);
      if (!status)
        goto exit;

      status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
      if (!status)
        goto exit;
    }

  /* Параметры ВАРУ. */
  SONAR_PARAM_NAME (source, "tvg/gain", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "gain", NULL, min_gain);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_double_range (builder, key_id, min_gain, max_gain, 1.0);
    if (!status)
      goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  if (decrease)
    {
      SONAR_PARAM_NAME (source, "tvg/decrease", NULL);
      status = hyscan_data_schema_builder_key_boolean_create (builder, key_id, "decrease", NULL, decrease);
      if (!status)
        goto exit;

      status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
    }

exit:
  return status;
}
