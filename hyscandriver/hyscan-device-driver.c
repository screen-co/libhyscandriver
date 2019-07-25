/* hyscan-device-driver.c
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
 * SECTION: hyscan-device-driver
 * @Short_description: вспомогательные функции для интерфейса #HyScanDevice
 * @Title: HyScanDeviceDriver
 *
 * Функции предназначены для отправки сигналов интерфейса #HyScanDevice.
 * Эти функции предназначены для использования в драйверах устройств.
 */

#include "hyscan-device-driver.h"

/*
 * hyscan_device_driver_send_state:
 * @device: указатель на #HyScanDevice
 * @dev_id: идентификатор устройства
 *
 * Функция отправляет сигнал #HyScanDevice::device-state.
 */
void
hyscan_device_driver_send_state (gpointer      device,
                                 const gchar  *dev_id)
{
  g_return_if_fail (HYSCAN_IS_DEVICE (device));

  g_signal_emit_by_name (device, "device-state", dev_id);
}

/*
 * hyscan_device_driver_send_log:
 * @device: указатель на #HyScanDevice
 * @source: источник сообщения (NULL терминированная строка)
 * @time: время приёма сообщения, мкс
 * @level: тип сообщения #HyScanLogLevel
 * @message: сообщение (NULL терминированная строка)
 *
 * Функция отправляет сигнал #HyScanDevice::device-log.
 */
void
hyscan_device_driver_send_log (gpointer        device,
                               const gchar    *source,
                               gint64          time,
                               HyScanLogLevel  level,
                               const gchar    *message)
{
  g_return_if_fail (HYSCAN_IS_DEVICE (device));

  g_signal_emit_by_name (device, "device-log", source, time, level, message);
}
