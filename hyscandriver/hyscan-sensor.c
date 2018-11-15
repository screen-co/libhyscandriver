/* hyscan-sensor.c
 *
 * Copyright 2016-2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-sensor
 * @Short_description: интерфейс управления датчиком
 * @Title: HyScanSensor
 *
 * Датчиком является любая система, предназначенная для дополнения
 * гидролокационных данных вспомогательной информацией. Это может
 * быть: навигационная информация, информация об ориентации в пространстве,
 * информация о скорости звука и т.п.
 *
 * Каждый из датчиков идентифицируется уникальным названием.
 *
 * Перед началом работы рекомендуется задать профиль скорости звука. Для этого
 * используется функция #hyscan_sensor_set_sound_velocity. По умолчанию используется
 * фиксированное значение скорости звука, равное 1500 м/с.
 *
 * Приём данных каждым из датчиков можно включить или выключить с помощью
 * функции #hyscan_sensor_set_enable.
 *
 * До удаления объекта управления, обязательно должно быть выполнено отключение
 * от датчика с помощью функции #hyscan_sensor_disconnect.
 */

#include "hyscan-sensor.h"
#include "hyscan-driver-marshallers.h"

#include <hyscan-buffer.h>

G_DEFINE_INTERFACE (HyScanSensor, hyscan_sensor, G_TYPE_OBJECT)

static void
hyscan_sensor_default_init (HyScanSensorInterface *iface)
{
  /**
   * HyScanSonar::device-state:
   * @sensor: указатель на #HyScanSensor
   * @dev_id: идентификатор устройства
   *
   * Данный сигнал посылается при изменении состояния устройства. Сами
   * изменения необходимо считывать через параметры в ветке "/state".
   */
  g_signal_new ("device-state", HYSCAN_TYPE_SENSOR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                g_cclosure_marshal_VOID__STRING,
                G_TYPE_NONE, 1,
                G_TYPE_STRING);

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

  /**
   * HyScanSensor::sensor-log:
   * @sensor: указатель на #HyScanSensor
   * @source: источник сообщения (NULL терминированная строка)
   * @time: время приёма сообщения, мкс
   * @level: тип сообщения #HyScanLogLevel
   * @message: сообщение (NULL терминированная строка)
   *
   * В процессе работы драйвер может отправлять различные информационные и
   * диагностические сообщения. При получении такого сообщения интерфейс
   * посылает данный сигнал, в котором передаёт их пользователю. Название
   * источника сообщения определяется драйвером.
   */
  g_signal_new ("sensor-log", HYSCAN_TYPE_SENSOR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__STRING_INT64_INT_STRING,
                G_TYPE_NONE, 4,
                G_TYPE_STRING,
                G_TYPE_INT64,
                G_TYPE_INT,
                G_TYPE_STRING);
}

/**
 * hyscan_sensor_set_sound_velocity:
 * @sensor: указатель на #HyScanSensor
 * @svp: (element-type: HyScanSoundVelocity): таблица профиля скорости звука
 *
 * Функция задаёт таблицу профиля скорости звука.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_set_sound_velocity (HyScanSensor *sensor,
                                  GList        *svp)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_sound_velocity != NULL)
    return (* iface->set_sound_velocity) (sensor, svp);

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

/**
 * hyscan_sensor_disconnect:
 * @sensor: указатель на #HyScanSensor
 *
 * Функция выполняет отключение от датчика. Отключение обязательно должно
 * быть выполнено до удаления объекта управления датчиком.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_disconnect (HyScanSensor *sensor)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->disconnect != NULL)
    return (* iface->disconnect) (sensor);

  return FALSE;
}
