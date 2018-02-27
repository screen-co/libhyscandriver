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
 * В системе могут использоваться несколько однотипных датчиков, например
 * два и более датчиков систем позиционирования ГЛОНАСС или GPS. Для того,
 * чтобы различать информацию от этих датчиков, имеется возможность добавить
 * метку к данным каждого из датчиков. Такой меткой является номер канала.
 * Для задания номера канала предназначена функция #hyscan_sensor_set_channel.
 *
 * Часто, данные от датчиков имеют определённую задержку во времени. Это
 * связанно с необходимостью обработки данных датчиком или использованием
 * низкоскоростных каналов связи. Для компенсации этой задержки, время приёма
 * данных датчиком уменьшается на величину заданную функцией #hyscan_sensor_set_latency.
 *
 * Приём данных каждым из датчиков можно включить или выключить с помощью
 * функции #hyscan_sensor_set_enable.
 */

#include "hyscan-sensor.h"
#include "hyscan-driver-marshallers.h"

#include <hyscan-buffer.h>

G_DEFINE_INTERFACE (HyScanSensor, hyscan_sensor, G_TYPE_OBJECT)

static void
hyscan_sensor_default_init (HyScanSensorInterface *iface)
{
  /**
   * HyScanSonar::sensor-state:
   * @sensor: указатель на #HyScanSensor
   * @name: название датчика
   * @state: состояние датчика
   *
   * Данный сигнал посылается при изменении состояния датчика.
   */
  g_signal_new ("sensor-state", HYSCAN_TYPE_SENSOR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__STRING_BOOLEAN,
                G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_BOOLEAN);

  /**
   * HyScanSensor::sensor-data:
   * @sensor: указатель на #HyScanSensor
   * @name: название датчика
   * @time: время приёма данных, мкс
   * @data: данные #HyScanBuffer
   *
   * Данный сигнал посылается при получении данных от датчика.
   */
  g_signal_new ("sensor-data", HYSCAN_TYPE_SENSOR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__STRING_INT64_OBJECT,
                G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_INT64, HYSCAN_TYPE_BUFFER);

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
                G_TYPE_NONE, 4, G_TYPE_STRING, G_TYPE_INT64, G_TYPE_INT, G_TYPE_STRING);
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
 * hyscan_sensor_set_channel:
 * @sensor: указатель на #HyScanSensor
 * @name: название датчика
 * @channel: номер канала
 *
 * Функция устанавливает номер приёмного канала для указанного датчика.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_set_channel (HyScanSensor *sensor,
                           const gchar  *name,
                           guint         channel)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_channel != NULL)
    return (* iface->set_channel) (sensor, name, channel);

  return FALSE;
}

/**
 * hyscan_sensor_set_latency:
 * @sensor: указатель на #HyScanSensor
 * @name: название датчика
 * @latency: задержка приёма данных, мкс
 *
 * Функция устанавливает величину компенсации задержки при приёме данных датчиком.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sensor_set_latency (HyScanSensor *sensor,
                           const gchar  *name,
                           gint64        latency)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_latency != NULL)
    return (* iface->set_latency) (sensor, name, latency);

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
