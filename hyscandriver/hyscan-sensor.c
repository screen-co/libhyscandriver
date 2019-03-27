/* hyscan-sensor.c
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
 * SECTION: hyscan-sensor
 * @Short_description: интерфейс управления датчиком
 * @Title: HyScanSensor
 *
 * Датчиком является любая система, предназначенная для дополнения
 * гидролокационных данных вспомогательной информацией. Это может
 * быть: навигационная информация, информация об ориентации в пространстве,
 * информация о скорости звука и т.п.
 *
 * Каждый из датчиков идентифицируется уникальным названием. Задать смещение
 * приёмных антенн датчика можно с помощью функции
 * #hyscan_sensor_antenna_set_offset. Приём данных каждым из датчиков можно
 * включить или выключить с помощью функции #hyscan_sensor_set_enable.
 */

#include "hyscan-sensor.h"
#include "hyscan-driver-marshallers.h"

#include <hyscan-buffer.h>

G_DEFINE_INTERFACE (HyScanSensor, hyscan_sensor, G_TYPE_OBJECT)

static void
hyscan_sensor_default_init (HyScanSensorInterface *iface)
{
  /**
   * HyScanSensor::sensor-data:
   * @sensor: указатель на #HyScanSensor
   * @name: название датчика
   * @source: идентификатор источника данных #HyScanSourceType
   * @time: время приёма данных, мкс
   * @data: данные #HyScanBuffer
   *
   * Данный сигнал посылается при получении данных от датчика.
   */
  g_signal_new ("sensor-data", HYSCAN_TYPE_SENSOR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__STRING_INT_INT64_OBJECT,
                G_TYPE_NONE, 4,
                G_TYPE_STRING,
                G_TYPE_INT,
                G_TYPE_INT64,
                HYSCAN_TYPE_BUFFER);
}

/**
 * hyscan_sensor_antenna_set_offset:
 * @sensor: указатель на #HyScanSensor
 * @name: название датчика
 * @offset: смещение приёмной антенны
 *
 * Функция задаёт смещение приёмных антенн датчика. Подробное
 * описание параметров приводится в #HyScanTypes.
 *
 * Если для датчика задано смещение по умолчанию, изменить его нельзя.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_antenna_set_offset (HyScanSensor              *sensor,
                                  const gchar               *name,
                                  const HyScanAntennaOffset *offset)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->antenna_set_offset != NULL)
    return (* iface->antenna_set_offset) (sensor, name, offset);

  return FALSE;
}

/**
 * hyscan_sensor_set_enable:
 * @sensor: указатель на #HyScanSensor
 * @name: название датчика
 * @enable: включён или выключен
 *
 * Функция включает или выключает приём данных.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_set_enable (HyScanSensor *sensor,
                          const gchar  *name,
                          gboolean      enable)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_enable != NULL)
    return (* iface->set_enable) (sensor, name, enable);

  return FALSE;
}
