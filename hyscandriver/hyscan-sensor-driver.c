/* hyscan-sensor-driver.c
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
 * SECTION: hyscan-sensor-driver
 * @Short_description: вспомогательные функции для интерфейса #HyScanSensor
 * @Title: HyScanSensorDriver
 *
 * Функции предназначены для отправки сигналов интерфейса #HyScanSensor.
 * Эти функции предназначены для использования в драйверах устройств.
 */

#include "hyscan-sensor-driver.h"

/*
 * hyscan_sensor_driver_send_data:
 * @sensor: указатель на #HyScanSensor
 * @name: название датчика
 * @source: идентификатор источника данных #HyScanSourceType
 * @time: время приёма данных, мкс
 * @data: данные #HyScanBuffer
 *
 * Функция отправляет сигнал #HyScanSensor::sensor-data.
 */
void
hyscan_sensor_driver_send_data (gpointer          sensor,
                                const gchar      *name,
                                HyScanSourceType  source,
                                gint64            time,
                                HyScanBuffer     *data)
{
  g_return_if_fail (HYSCAN_IS_SENSOR (sensor));

  g_signal_emit_by_name (sensor, "sensor-data", name, (gint)source, time, data);
}
