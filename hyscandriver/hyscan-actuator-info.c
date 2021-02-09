/* hyscan-actuator-info.c
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
 * SECTION: hyscan-actuator-info
 * @Short_description: класс с информацией о приводе
 * @Title: HyScanActuatorInfo
 *
 * Класс предназначен для получения информации о приводе по его схеме.
 * Для создания объекта используется функция #hyscan_actuator_info_new, в
 * которую передаётся схема устройства.
 *
 * Список приводов можно узнать с помощью функции
 * #hyscan_actuator_info_list_actuators.
 *
 * Информацию о приводе можно получить с помощью функции
 * #hyscan_actuator_info_get_actuator.
 */

#include "hyscan-actuator-info.h"
#include "hyscan-device-schema.h"

#define ACTUATOR_PARAM_NAME(...)  hyscan_param_name_constructor (key_id, \
                                    (guint)sizeof (key_id), \
                                    "actuators", __VA_ARGS__)

enum
{
  PROP_O,
  PROP_SCHEMA
};

struct _HyScanActuatorInfoPrivate
{
  HyScanDataSchema    *schema;                                 /* Схема устройства. */
  GHashTable          *actuators;                              /* Параметры приводов. */
  GArray              *actuators_list;                         /* Список приводов. */
};

static void            hyscan_actuator_info_set_property       (GObject               *object,
                                                                guint                  prop_id,
                                                                const GValue          *value,
                                                                GParamSpec            *pspec);

static void            hyscan_actuator_info_object_constructed (GObject               *object);
static void            hyscan_actuator_info_object_finalize    (GObject               *object);

static GHashTable *    hyscan_actuator_info_parse_actuators    (HyScanDataSchema      *schema);

G_DEFINE_BOXED_TYPE (HyScanActuatorInfoActuator, hyscan_actuator_info_actuator,
                     hyscan_actuator_info_actuator_copy, hyscan_actuator_info_actuator_free)

G_DEFINE_TYPE_WITH_PRIVATE (HyScanActuatorInfo, hyscan_actuator_info, G_TYPE_OBJECT)

static void
hyscan_actuator_info_class_init (HyScanActuatorInfoClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_actuator_info_set_property;

  object_class->constructed = hyscan_actuator_info_object_constructed;
  object_class->finalize = hyscan_actuator_info_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "Device schema", HYSCAN_TYPE_DATA_SCHEMA,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_actuator_info_init (HyScanActuatorInfo *actuator_info)
{
  actuator_info->priv = hyscan_actuator_info_get_instance_private (actuator_info);
}

static void
hyscan_actuator_info_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  HyScanActuatorInfo *actuator_info = HYSCAN_ACTUATOR_INFO (object);
  HyScanActuatorInfoPrivate *priv = actuator_info->priv;

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
hyscan_actuator_info_object_constructed (GObject *object)
{
  HyScanActuatorInfo *actuator_info = HYSCAN_ACTUATOR_INFO (object);
  HyScanActuatorInfoPrivate *priv = actuator_info->priv;

  GHashTableIter iter;
  gpointer name;

  /* Должна быть задана схема устройства. */
  if (priv->schema == NULL)
    return;

  /* Верифицируем схему устройства. */
  if (!hyscan_device_schema_check_id (priv->schema))
    {
      g_warning ("HyScanSonarInfo: unsupported device schema");
      return;
    }

  /* Информация о приводах. */
  priv->actuators = hyscan_actuator_info_parse_actuators (priv->schema);
  if (priv->actuators == NULL)
    return;

  /* Список приводов. */
  priv->actuators_list = g_array_new (TRUE, TRUE, sizeof (gchar*));
  g_hash_table_iter_init (&iter, priv->actuators);
  while (g_hash_table_iter_next (&iter, &name, NULL))
    g_array_append_val (priv->actuators_list, name);
}

static void
hyscan_actuator_info_object_finalize (GObject *object)
{
  HyScanActuatorInfo *actuator_info = HYSCAN_ACTUATOR_INFO (object);
  HyScanActuatorInfoPrivate *priv = actuator_info->priv;

  g_clear_object (&priv->schema);
  g_clear_pointer (&priv->actuators, g_hash_table_unref);
  g_clear_pointer (&priv->actuators_list, g_array_unref);

  G_OBJECT_CLASS (hyscan_actuator_info_parent_class)->finalize (object);
}

/* Функция считывает параметры приводов. */
static GHashTable *
hyscan_actuator_info_parse_actuators (HyScanDataSchema *schema)
{
  GHashTable *actuators;
  const gchar * const *keys;
  gchar key_id[128];
  guint i;

  keys = hyscan_data_schema_list_keys (schema);
  if (keys == NULL)
    return NULL;

  actuators = g_hash_table_new_full (g_str_hash, g_str_equal,
                                     g_free, (GDestroyNotify)hyscan_actuator_info_actuator_free);

  for (i = 0; keys[i] != NULL; i++)
    {
      gchar **keyv = g_strsplit (keys[i], "/", -1);

      /* Ищем параметр /actuators/actuator-name/dev-id */
      if (g_strv_length (keyv) != 4)
        {
          g_strfreev (keyv);
          continue;
        }

      if ((g_strcmp0 (keyv[1], "actuators") == 0) && (g_strcmp0 (keyv[3], "dev-id") == 0))
        {
          HyScanActuatorInfoActuator *info;
          const gchar *description;
          const gchar *dev_id;
          HyScanActuatorModeType capabilities;
          const gchar *cap_string;
          gdouble min_range;
          gdouble max_range;
          gdouble min_speed;
          gdouble max_speed;
          gboolean status;

          ACTUATOR_PARAM_NAME (keyv[2], "dev-id", NULL);
          dev_id = hyscan_data_schema_key_get_string (schema, key_id);
          if (dev_id == NULL)
            continue;

          ACTUATOR_PARAM_NAME (keyv[2], "description", NULL);
          description = hyscan_data_schema_key_get_string (schema, key_id);

          capabilities = 0;
          ACTUATOR_PARAM_NAME (keyv[2], "capabilities", NULL);
          cap_string = hyscan_data_schema_key_get_string (schema, key_id);
          if (cap_string != NULL)
            {
              if (g_strrstr (cap_string, "scan") != NULL)
                capabilities |= HYSCAN_ACTUATOR_MODE_SCAN;

              if (g_strrstr (cap_string, "manual") != NULL)
                capabilities |= HYSCAN_ACTUATOR_MODE_MANUAL;
            }

          ACTUATOR_PARAM_NAME (keyv[2], "range", NULL);
          status = hyscan_data_schema_key_get_double (schema, key_id,
                                                      &min_range, &max_range,
                                                      NULL, NULL);
          if (!status)
            continue;

          ACTUATOR_PARAM_NAME (keyv[2], "speed", NULL);
          status = hyscan_data_schema_key_get_double (schema, key_id,
                                                      &min_speed, &max_speed,
                                                      NULL, NULL);
          if (!status)
            continue;

          info = g_slice_new0 (HyScanActuatorInfoActuator);
          info->name = g_strdup (keyv[2]);
          info->dev_id = g_strdup (dev_id);
          info->description = g_strdup (description);
          info->capabilities = capabilities;
          info->min_range = min_range;
          info->max_range = max_range;
          info->min_speed = min_speed;
          info->max_speed = max_speed;

          g_hash_table_insert (actuators, g_strdup (keyv[2]), info);
        }

      g_strfreev (keyv);
    }

  if (g_hash_table_size (actuators) == 0)
    g_clear_pointer (&actuators, g_hash_table_unref);

  return actuators;
}

/**
 * hyscan_actuator_info_new:
 * @schema: схема устройства
 *
 * Функция создаёт новый объект с информацией о датчиках.
 *
 * Returns: #HyScanActuatorInfo. Для удаления #g_object_unref.
 */
HyScanActuatorInfo *
hyscan_actuator_info_new (HyScanDataSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_ACTUATOR_INFO,
                       "schema", schema,
                       NULL);
}

/**
 * hyscan_actuator_info_get_actuator:
 * @info: указатель на #HyScanActuatorInfo
 *
 * Функция возвращает список приводов.
 *
 * Returns: (transfer none): Список приводов.
 */
const gchar * const *
hyscan_actuator_info_list_actuators (HyScanActuatorInfo *info)
{
  g_return_val_if_fail (HYSCAN_IS_ACTUATOR_INFO (info), NULL);

  if ((info->priv->actuators_list == NULL) || (info->priv->actuators_list->len == 0))
    return NULL;

  return (const gchar * const *)info->priv->actuators_list->data;
}

/**
 * hyscan_actuator_info_get_actuator:
 * @info: указатель на #HyScanActuatorInfo
 * @name: название привода
 *
 * функция возвращает параметры привода.
 *
 * Returns: (transfer none): Параметры привода или NULL.
 */
const HyScanActuatorInfoActuator *
hyscan_actuator_info_get_actuator (HyScanActuatorInfo *info,
                                   const gchar        *name)
{
  g_return_val_if_fail (HYSCAN_IS_ACTUATOR_INFO (info), NULL);

  if (info->priv->actuators == NULL)
    return NULL;

  return g_hash_table_lookup (info->priv->actuators, name);
}

/**
 * hyscan_actuator_info_actuator_copy:
 * @info: структура #HyScanActuatorInfoActuator для копирования
 *
 * Функция создаёт копию структуры #HyScanActuatorInfoActuator.
 *
 * Returns: (transfer full): Новая структура #HyScanActuatorInfoActuator.
 * Для удаления #hyscan_actuator_info_actuator_free.
 */
HyScanActuatorInfoActuator *
hyscan_actuator_info_actuator_copy (const HyScanActuatorInfoActuator *info)
{
  HyScanActuatorInfoActuator *new_info;

  if (info == NULL)
    return NULL;

  new_info = g_slice_new (HyScanActuatorInfoActuator);
  new_info->name = g_strdup (info->name);
  new_info->dev_id = g_strdup (info->dev_id);
  new_info->description = g_strdup (info->description);
  new_info->capabilities = info->capabilities;
  new_info->min_range = info->min_range;
  new_info->max_range = info->max_range;
  new_info->min_speed = info->min_speed;
  new_info->max_speed = info->max_speed;

  return new_info;
}

/**
 * hyscan_actuator_info_actuator_free:
 * @info: структура #HyScanActuatorInfoActuator для удаления
 *
 * Функция удаляет структуру #HyScanActuatorInfoActuator.
 */
void
hyscan_actuator_info_actuator_free (HyScanActuatorInfoActuator *info)
{
  if (info == NULL)
    return;

  g_free ((gchar*)info->name);
  g_free ((gchar*)info->dev_id);
  g_free ((gchar*)info->description);

  g_slice_free (HyScanActuatorInfoActuator, info);
}
