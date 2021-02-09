/* hyscan-actuator-schema.c
 *
 * Copyright 2021 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-actuator-schema
 * @Short_description: класс создания схемы привода
 * @Title: HyScanActuatorSchema
 *
 * Класс предназначен для автоматизации процесса создания схемы данных
 * привода. Он содержит функции которые определяют наличие и
 * параметры привода.
 *
 * Информация о приводах находится в ветке "/actuators" схемы устройства. Схема
 * может содержать информацию о произвольном количестве приводов. Каждый
 * привод идентифицируется произвольным именем. Информация о приводе находится
 * в ветке "/actuators/actuator-name", где actuator-name - имя привода и
 * содержит следующие поля:
 *
 * - dev-id - уникальный идентификатор устройства, тип STRING, обязательное;
 * - description - поле с описанием привода, тип STRING, необязательное.
 *
 * Режимы работы привода определяются параметром "capabilities". Технические
 * характеристики привода указыватся в следующих параметрах:
 *
 * - range - диапазон углов поворота привода, тип DOUBLE;
 * - speed - диапзон скоростей вращения привода, тип DOUBLE.
 *
 * Пример всех параметров для привода с именем "hydra":
 *
 * - /actuators/hydra/dev-id
 * - /actuators/hydra/description
 * - /actuators/hydra/capabilities
 * - /actuators/hydra/range
 * - /actuators/hydra/speed
 *
 * Дополнительные параметры, если они необходимы, должны находится в ветках
 * "/params" и "/system". Состояние устройства приводится в ветке "/state",
 * а общая информация об устройстве и драйвере в ветке "/info". Описание этих
 * веток приведено в #HyscanDeviceSchema.
 *
 * Для создания схемы используется класс #HyScanDeviceSchema, указатель
 * на который передаётся в функцию #hyscan_actuator_schema_new.
 *
 * Привод добавляется с помощью функции #hyscan_actuator_schema_add_actuator. А
 * его характеристики определяются функцией #hyscan_actuator_schema_set_params.
 *
 * Функция hyscan_actuator_schema_add_full добавляет привод описанный
 * стуруктурой #HyScanActuatorInfoActuator.
 */

#include "hyscan-actuator-schema.h"
#include "hyscan-device-schema.h"

#define ACTUATOR_PARAM_NAME(...)  hyscan_param_name_constructor (key_id, \
                                    (guint)sizeof (key_id), \
                                    "actuators", __VA_ARGS__)

enum
{
  PROP_O,
  PROP_SCHEMA
};

struct _HyScanActuatorSchemaPrivate
{
  HyScanDataSchemaBuilder     *builder;                        /* Строитель схемы. */
  GHashTable                  *actuators;                      /* Наличие приводов. */
};

static void    hyscan_actuator_schema_set_property             (GObject               *object,
                                                                guint                  prop_id,
                                                                const GValue          *value,
                                                                GParamSpec            *pspec);
static void    hyscan_actuator_schema_object_constructed       (GObject               *object);
static void    hyscan_actuator_schema_object_finalize          (GObject               *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanActuatorSchema, hyscan_actuator_schema, G_TYPE_OBJECT)

static void
hyscan_actuator_schema_class_init (HyScanActuatorSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_actuator_schema_set_property;

  object_class->constructed = hyscan_actuator_schema_object_constructed;
  object_class->finalize = hyscan_actuator_schema_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "Device schema", HYSCAN_TYPE_DEVICE_SCHEMA,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_actuator_schema_init (HyScanActuatorSchema *actuator_schema)
{
  actuator_schema->priv = hyscan_actuator_schema_get_instance_private (actuator_schema);
}

static void
hyscan_actuator_schema_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  HyScanActuatorSchema *actuator_schema = HYSCAN_ACTUATOR_SCHEMA (object);
  HyScanActuatorSchemaPrivate *priv = actuator_schema->priv;

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
hyscan_actuator_schema_object_constructed (GObject *object)
{
  HyScanActuatorSchema *actuator_schema = HYSCAN_ACTUATOR_SCHEMA (object);
  HyScanActuatorSchemaPrivate *priv = actuator_schema->priv;

  priv->actuators = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
}

static void
hyscan_actuator_schema_object_finalize (GObject *object)
{
  HyScanActuatorSchema *actuator_schema = HYSCAN_ACTUATOR_SCHEMA (object);
  HyScanActuatorSchemaPrivate *priv = actuator_schema->priv;

  g_hash_table_unref (priv->actuators);
  g_clear_object (&priv->builder);

  G_OBJECT_CLASS (hyscan_actuator_schema_parent_class)->finalize (object);
}

/**
 * hyscan_actuator_schema_new:
 * @schema: указатель на #HyScanDeviceSchema
 *
 * Функция создаёт новый объект HyScanActuatorSchema.
 *
 * Returns: #HyScanActuatorSchema. Для удаления #g_object_unref.
 */
HyScanActuatorSchema *
hyscan_actuator_schema_new (HyScanDeviceSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_ACTUATOR_SCHEMA,
                       "schema", schema,
                       NULL);
}

/**
 * hyscan_actuator_schema_add_full:
 * @schema: указатель на #HyScanActuatorSchema
 * @info: (transfer none): параметры привода
 *
 * Функция добавляет в схему описание привода.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_actuator_schema_add_full (HyScanActuatorSchema       *schema,
                                 HyScanActuatorInfoActuator *info)
{
  gboolean status = FALSE;

  status = hyscan_actuator_schema_add_actuator (schema, info->name,
                                                info->dev_id,
                                                info->description,
                                                info->capabilities);
  if (!status)
    goto exit;

  status = hyscan_actuator_schema_set_params (schema, info->name,
                                              info->min_range, info->max_range,
                                              info->min_speed, info->max_speed);

exit:
  return status;
}

/**
 * hyscan_actuator_schema_add_sensor:
 * @schema: указатель на #HyScanActuatorSchema
 * @name: название привода
 * @dev_id: уникальный идентификатор устройства
 * @description: описание привода
 * @capabilities: режимы работы привода
 *
 * Функция добавляет в схему описание привода.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_actuator_schema_add_actuator (HyScanActuatorSchema   *schema,
                                     const gchar            *name,
                                     const gchar            *dev_id,
                                     const gchar            *description,
                                     HyScanActuatorModeType  capabilities)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_ACTUATOR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (g_hash_table_contains (schema->priv->actuators, name))
    goto exit;

  /* Уникальный идентификатор устройства. */
  ACTUATOR_PARAM_NAME (name, "dev-id", NULL);
  status = hyscan_data_schema_builder_key_string_create (builder, key_id, "dev-id", NULL, dev_id);
  if (!status)
    return FALSE;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    return FALSE;

  /* Описание привода. */
  if (description != NULL)
    {
      ACTUATOR_PARAM_NAME (name, "description", NULL);
      status = hyscan_data_schema_builder_key_string_create (builder, key_id, "description", NULL, description);
      if (!status)
        return FALSE;

      status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
      if (!status)
        return FALSE;
    }

  /* Режимы работы привода. */
  if (capabilities)
    {
      gchar *cap_string = g_strdup_printf ("%s %s",
        (capabilities & HYSCAN_ACTUATOR_MODE_SCAN) ? "scan" : "",
        (capabilities & HYSCAN_ACTUATOR_MODE_MANUAL) ? "manual" : "");

      ACTUATOR_PARAM_NAME (name, "capabilities", NULL);
      status = hyscan_data_schema_builder_key_string_create (builder, key_id, "capabilities", NULL, cap_string);
      g_free (cap_string);
      if (!status)
        goto exit;

      status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
      if (!status)
        goto exit;
    }

  if (status)
    g_hash_table_insert (schema->priv->actuators, g_strdup (name), NULL);

exit:
  return status;
}

/**
 * hyscan_actuator_schema_set_params:
 * @schema: указатель на #HyScanActuatorSchema
 * @name: название привода
 * @min_range: значение минимального угла поворота привода, десятичный градус
 * @max_range: значение максимального угла поворота привода, десятичный градус
 * @min_speed: минимальная скорость вращения привода, десятичный градус/с
 * @max_speed: максимальная скорость вращения привода, десятичный градус/с
 *
 * Функция устанавливает параметры привода.
 *
 * Returns: %TRUE если функция выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_actuator_schema_set_params (HyScanActuatorSchema *schema,
                                   const gchar          *name,
                                   gdouble               min_range,
                                   gdouble               max_range,
                                   gdouble               min_speed,
                                   gdouble               max_speed)
{
  HyScanDataSchemaBuilder *builder;
  gboolean status = FALSE;
  gchar key_id[128];

  g_return_val_if_fail (HYSCAN_IS_ACTUATOR_SCHEMA (schema), FALSE);

  builder = schema->priv->builder;
  if (builder == NULL)
    goto exit;

  if (!g_hash_table_contains (schema->priv->actuators, name))
    goto exit;

  /* Диапазон углов вращения. */
  ACTUATOR_PARAM_NAME (name, "range", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "range", NULL, min_range);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_double_range (builder, key_id, min_range, max_range, 1.0);
    if (!status)
      goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  /* Диапазон скоростей вращения вращения. */
  ACTUATOR_PARAM_NAME (name, "speed", NULL);
  status = hyscan_data_schema_builder_key_double_create (builder, key_id, "speed", NULL, min_speed);
  if (!status)
    goto exit;

  status = hyscan_data_schema_builder_key_double_range (builder, key_id, min_speed, max_speed, 1.0);
    if (!status)
      goto exit;

  status = hyscan_data_schema_builder_key_set_access (builder, key_id, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  if (!status)
    goto exit;

  exit:
    return status;
}
