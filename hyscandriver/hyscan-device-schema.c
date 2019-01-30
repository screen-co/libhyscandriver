/* hyscan-device-schema.c
 *
 * Copyright 2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-device-schema
 * @Short_description: описание схемы устройства
 * @Title: HyScanDeviceSchema
 *
 * Класс предназначен для автоматизации процесса создания схемы данных
 * устройства. Класс наследуется от #HyScanDataSchemaBuilder и создаёт в схеме
 * устройства базовые параметры, такие как идентификатор и версию схемы.
 *
 * Схема устройства содержит параметры, описывающие структуру и характеристики
 * датчиков и гидролокатора. Описание датчиков должно находится в ветке
 * "/sensors", а гидролокационных источников данных в ветке "/sources".
 * Подробное описание этих веток приведено в #HyScanSensorSchema и
 * #HyScanSonarSchema.
 *
 * Драйвер устройства может содержать параметры, управляющие его поведением.
 * Параметры, предназначенные для пользователей устройства, должны находится в
 * ветке "/params/dev-id". Системные параметры, предназначенные для
 * разработчиков, должны находится в ветке "/system/dev-id". Уровень
 * вложенности для обеих веток не должен превышать трёх, например:
 *
 * - /params/h5se7/mode/type
 * - /system/h5se7/state
 *
 * Параметры, находящиеся в ветках "/params" и "/system" допускают изменения.
 *
 * Для информирования пользователя о текущем состоянии устройства предназначена
 * ветка "/state/dev-id". В ней должен находится обязательный параметр -
 * status с типом ENUM (enum-id = "status"), который содержит текущий статус
 * устройства.
 *
 * Дополнительные параметры должны размещаться в отдельных ветках. Одна ветка
 * предназначена для одного параметра. В ветке должны быть следующие параметры:
 *
 * - value - значение, тип зависит от значения;
 * - status - статус, тип ENUM (enum-id = "status").
 *
 * Например для отображения статуса системы питания, параметры должны иметь
 * следующий вид:
 *
 * - /state/h5se7/psu/value - значение напряжения питания;
 * - /state/h5se7/psu/status - статус системы питания.
 *
 * Название величины значения должно находиться в поле описания параметра
 * (/state/h5se7/psu/value). Название и описание контролируемого параметра
 * должно находиться в описании ветки (/state/h5se7/psu). Поле status может
 * принимать одно из значений #HyScanDeviceStatusType (enum-id = "status").
 *
 * Параметры в ветке "/state" могут изменяться во время работы устройства, но
 * доступны только для чтения. При изменении этих параметров, драйвер
 * устройства посылает сигнал "device-state".
 *
 * Общая информация об устройстве должна находится в ветке "/info/dev-id".
 * Параметры в ветке "/info" не изменяются в процессе работы устройства и
 * доступны только для чтения.
 *
 * Во все ветках "dev-id" - уникальный идентификатор устройства.
 */

#include "hyscan-device-schema.h"
#include "hyscan-device.h"

#define HYSCAN_DEVICE_SCHEMA_ID                         1374652938475623487

enum
{
  PROP_O,
  PROP_VERSION
};

struct _HyScanDeviceSchemaPrivate
{
  gint64       version;
};

static void    hyscan_device_schema_set_property       (GObject       *object,
                                                        guint          prop_id,
                                                        const GValue  *value,
                                                        GParamSpec    *pspec);
static void    hyscan_device_schema_object_constructed (GObject       *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanDeviceSchema, hyscan_device_schema, HYSCAN_TYPE_DATA_SCHEMA_BUILDER)

static void
hyscan_device_schema_class_init (HyScanDeviceSchemaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_device_schema_set_property;
  object_class->constructed = hyscan_device_schema_object_constructed;

  g_object_class_install_property (object_class, PROP_VERSION,
    g_param_spec_int64 ("version", "Version", "Device schema version", 0, G_MAXINT64, 0,
                        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_device_schema_init (HyScanDeviceSchema *schema)
{
  schema->priv = hyscan_device_schema_get_instance_private (schema);
}

static void
hyscan_device_schema_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  HyScanDeviceSchema *schema = HYSCAN_DEVICE_SCHEMA (object);
  HyScanDeviceSchemaPrivate *priv = schema->priv;

  switch (prop_id)
    {
    case PROP_VERSION:
      priv->version = g_value_get_int64 (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_device_schema_object_constructed (GObject *object)
{
  HyScanDeviceSchema *schema = HYSCAN_DEVICE_SCHEMA (object);
  HyScanDataSchemaBuilder *builder = HYSCAN_DATA_SCHEMA_BUILDER (object);

  G_OBJECT_CLASS (hyscan_device_schema_parent_class)->constructed (object);

  hyscan_data_schema_builder_key_integer_create (builder, "/schema/id", "id",
                                                 "Device schema id", HYSCAN_DEVICE_SCHEMA_ID);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/id", HYSCAN_DATA_SCHEMA_ACCESS_READ);

  hyscan_data_schema_builder_key_integer_create (builder, "/schema/version", "version",
                                                 "Device schema version", schema->priv->version);
  hyscan_data_schema_builder_key_set_access (builder, "/schema/version", HYSCAN_DATA_SCHEMA_ACCESS_READ);

  hyscan_data_schema_builder_enum_create (builder, HYSCAN_DEVICE_STATUS_ENUM);
  hyscan_data_schema_builder_enum_value_create (builder, HYSCAN_DEVICE_STATUS_ENUM,
                                                HYSCAN_DEVICE_STATUS_ERROR, "Error", NULL);
  hyscan_data_schema_builder_enum_value_create (builder, HYSCAN_DEVICE_STATUS_ENUM,
                                                HYSCAN_DEVICE_STATUS_CRITICAL, "Critical", NULL);
  hyscan_data_schema_builder_enum_value_create (builder, HYSCAN_DEVICE_STATUS_ENUM,
                                                HYSCAN_DEVICE_STATUS_WARNING, "Warning", NULL);
  hyscan_data_schema_builder_enum_value_create (builder, HYSCAN_DEVICE_STATUS_ENUM,
                                                HYSCAN_DEVICE_STATUS_OK, "Ok", NULL);
}

/**
 * hyscan_device_schema_new:
 * @version: версия схемы устройства
 *
 * Функция создаёт новый объект #HyScanDeviceSchema.
 *
 * Returns: #HyScanDeviceSchema. Для удаления #g_object_unref.
 */
HyScanDeviceSchema *
hyscan_device_schema_new (gint64 version)
{
  return g_object_new (HYSCAN_TYPE_DEVICE_SCHEMA,
                       "schema-id", "device",
                       "version", version,
                       NULL);
}

/**
 * hyscan_device_schema_check_id:
 * @schema: указатель на #HyScanDataSchema
 *
 * Функция проверяет идентификатор и версию схемы устройства.
 *
 * Returns: %TRUE если идентификатор схемы валидный, иначе %FALSE.
 */
gboolean
hyscan_device_schema_check_id (HyScanDataSchema *schema)
{
  gint64 id = 0;
  gint64 version = 0;

  if (!hyscan_data_schema_key_get_integer (schema, "/schema/id", NULL, NULL, &id, NULL) ||
      !hyscan_data_schema_key_get_integer (schema, "/schema/version", NULL, NULL, &version, NULL))
    {
      return FALSE;
    }

  if ((id != HYSCAN_DEVICE_SCHEMA_ID) || (version != HYSCAN_DEVICE_SCHEMA_VERSION))
    return FALSE;

  return TRUE;
}
