/* hyscan-device-schema.c
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
 * SECTION: hyscan-device-schema
 * @Short_description: описание схемы устройства
 * @Title: HyScanDeviceSchema
 *
 * Схема устройства содержит параметры, описывающие структуру и характеристики
 * датчиков и гидролокатора. Описание датчиков должно находится в ветке
 * "/sensors", а гидролокатора в ветке "/sources". Подробное описание этих
 * веток приведено в #HyScanSensorSchema и #HyScanSonarSchema.
 *
 * Драйвер устройства может содержать параметры, определяющие его возможности.
 * Параметры, расчитанные на пользователей устройства, должны находится в ветке
 * "/params". Системные параметры, предназначенные для разработчиков, должны
 * находится в ветке "/system". Уровень вложенности, для обоих веток, не должен
 * превышать трёх, например:
 *
 * - /params/hardware/mode/type
 * - /params/software/data
 * - /system/state
 *
 * Общая информация об устройстве должна находится в ветке "/info". В ней должны
 * находится следующие обязательные параметры:
 *
 * - /info/model - модель гидролокатора, тип STRING;
 * - /info/name - название гидролокатора, тип STRING;
 * - /info/serial - серийный номер, тип STRING;
 * - /info/production-date - дата производства, тип INTEGER.
 *
 * Разработчик может добавить дополнительные параметры, которые должны
 * размещаться в ветке "/info/vendor", например:
 *
 * - /info/vendor/firmware-version;
 * - /info/vendor/service-date.
 *
 * Все дополнительные параметры в ветке "/info/vendor", должны должны находится
 * на одном уровне вложенности.
 */

#include "hyscan-device-schema.h"

/**
 * hyscan_device_schema_set_id:
 * @builder: указатель на #HyScanDataSchemaBuilder
 *
 * Функция устанавливает идентификатор и версию схемы устройства.
 *
 * Returns: %TRUE если идентификатор схемы валидный, иначе %FALSE.
 */
gboolean
hyscan_device_schema_set_id (HyScanDataSchemaBuilder *builder)
{
  HyScanDataSchema *schema;
  gchar *schema_data;
  gchar *schema_id;

  gboolean status = FALSE;

  schema_id = hyscan_data_schema_builder_get_id (builder);
  schema_data = hyscan_data_schema_builder_get_data (builder);
  schema = hyscan_data_schema_new_from_string (schema_data, schema_id);

  /* Если версия и идентификатор схемы устройства ещё не заданы, задаём их. */
  if (!hyscan_data_schema_has_key (schema, "/schema/id") &&
      !hyscan_data_schema_has_key (schema, "/schema/version"))
    {
      hyscan_data_schema_builder_key_integer_create (builder, "/schema/id", "id",
                                                     "Device schema id", HYSCAN_DEVICE_SCHEMA_ID);
      hyscan_data_schema_builder_key_set_access (builder, "/schema/id", HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

      hyscan_data_schema_builder_key_integer_create (builder, "/schema/version", "version",
                                                     "Device schema version", HYSCAN_DEVICE_SCHEMA_VERSION);
      hyscan_data_schema_builder_key_set_access (builder, "/schema/version", HYSCAN_DATA_SCHEMA_ACCESS_READONLY);

      status = TRUE;
    }

  /* Если версия и идентификатор заданы, проверяем их. */
  else if (hyscan_device_schema_check_id (schema))
    {
      status = TRUE;
    }

  g_object_unref (schema);
  g_free (schema_data);
  g_free (schema_id);

  return status;
}

/**
 * hyscan_device_schema_set_id:
 * @schema: указатель на #HyScanDataSchema
 *
 * Функция проверяет идентификатор и версию схемы устройства.
 *
 * Returns: %TRUE если идентификатор схемы валидный, иначе %FALSE.
 */
gboolean
hyscan_device_schema_check_id (HyScanDataSchema *schema)
{
  GVariant *id;
  GVariant *version;
  gboolean status = FALSE;

  id = hyscan_data_schema_key_get_default (schema, "/schema/id");
  version = hyscan_data_schema_key_get_default (schema, "/schema/version");

  /* Проверяем текущую версию и идентификатор схемы устройства. */
  if ((id != NULL) && (g_variant_get_int64 (id) == HYSCAN_DEVICE_SCHEMA_ID) &&
      (version != NULL) && (g_variant_get_int64 (version) == HYSCAN_DEVICE_SCHEMA_VERSION))
    {
      status = TRUE;
    }

  g_clear_pointer (&version, g_variant_unref);
  g_clear_pointer (&id, g_variant_unref);

  return status;
}

/**
 * hyscan_device_schema_get_boolean:
 * @schema: указатель на #HyScanDataSchema
 * @name: название параметра
 * @value: (out): значение параметра
 *
 * Функция получает значение по умолчанию для указанного параметра типа BOOLEAN.
 *
 * Return: %TRUE если значение параметра получено, иначе %FALSE.
 */
gboolean
hyscan_device_schema_get_boolean (HyScanDataSchema *schema,
                                  const gchar      *name,
                                  gboolean         *value)
{
  HyScanDataSchemaKeyType ktype;
  GVariant *vvalue;

  ktype = hyscan_data_schema_key_get_value_type (schema, name);
  if (ktype != HYSCAN_DATA_SCHEMA_KEY_BOOLEAN)
    return FALSE;

  vvalue = hyscan_data_schema_key_get_default (schema, name);
  *value = g_variant_get_boolean (vvalue);
  g_variant_unref (vvalue);

  return TRUE;
}

/**
 * hyscan_device_schema_get_integer:
 * @schema: указатель на #HyScanDataSchema
 * @name: название параметра
 * @value: (out): значение параметра
 *
 * Функция получает значение по умолчанию для указанного параметра типа INTEGER.
 *
 * Return: %TRUE если значение параметра получено, иначе %FALSE.
 */
gboolean
hyscan_device_schema_get_integer (HyScanDataSchema *schema,
                                  const gchar      *name,
                                  gint64           *value)
{
  HyScanDataSchemaKeyType ktype;
  GVariant *vvalue;

  ktype = hyscan_data_schema_key_get_value_type (schema, name);
  if (ktype != HYSCAN_DATA_SCHEMA_KEY_INTEGER)
    return FALSE;

  vvalue = hyscan_data_schema_key_get_default (schema, name);
  *value = g_variant_get_int64 (vvalue);
  g_variant_unref (vvalue);

  return TRUE;
}

/**
 * hyscan_device_schema_get_double:
 * @schema: указатель на #HyScanDataSchema
 * @name: название параметра
 * @value: (out): значение параметра
 *
 * Функция получает значение по умолчанию для указанного параметра типа DOUBLE.
 *
 * Return: %TRUE если значение параметра получено, иначе %FALSE.
 */
gboolean
hyscan_device_schema_get_double (HyScanDataSchema *schema,
                                 const gchar      *name,
                                 gdouble          *value)
{
  HyScanDataSchemaKeyType ktype;
  GVariant *vvalue;

  ktype = hyscan_data_schema_key_get_value_type (schema, name);
  if (ktype != HYSCAN_DATA_SCHEMA_KEY_DOUBLE)
    return FALSE;

  vvalue = hyscan_data_schema_key_get_default (schema, name);
  *value = g_variant_get_double (vvalue);
  g_variant_unref (vvalue);

  return TRUE;
}

/**
 * hyscan_device_schema_get_string:
 * @schema: указатель на #HyScanDataSchema
 * @name: название параметра
 *
 * Функция получает значение по умолчанию для указанного параметра типа STRING.
 *
 * Return: Значение параметры или NULL.
 */
const gchar *
hyscan_device_schema_get_string (HyScanDataSchema *schema,
                                 const gchar      *name)
{
  HyScanDataSchemaKeyType ktype;
  GVariant *vvalue;
  const gchar *value;

  ktype = hyscan_data_schema_key_get_value_type (schema, name);
  if (ktype != HYSCAN_DATA_SCHEMA_KEY_STRING)
    return NULL;

  vvalue = hyscan_data_schema_key_get_default (schema, name);
  value = g_variant_get_string (vvalue, NULL);
  g_variant_unref (vvalue);

  return value;
}
