/* hyscan-device.c
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
 * SECTION: hyscan-device
 * @Short_description: интерфейс управления устройством
 * @Title: HyScanDevice
 *
 * Интерфейс управления устройством содержит функции, применимые как к
 * датчикам, так и к гидролокаторам.
 *
 * Перед началом работы рекомендуется задать профиль скорости звука. Для этого
 * используется функция #hyscan_device_set_sound_velocity. По умолчанию
 * используется фиксированное значение скорости звука, равное 1500 м/с.
 *
 * До удаления объекта управления, обязательно должно быть выполнено отключение
 * от устройства с помощью функции #hyscan_device_disconnect.
 */

#include "hyscan-device.h"
#include "hyscan-driver-marshallers.h"

G_DEFINE_INTERFACE (HyScanDevice, hyscan_device, G_TYPE_OBJECT)

static void
hyscan_device_default_init (HyScanDeviceInterface *iface)
{
  /**
   * HyScanDevice::device-state:
   * @device: указатель на #HyScanDevice
   * @dev_id: идентификатор устройства
   *
   * Данный сигнал посылается при изменении состояния устройства. Сами
   * изменения необходимо считывать через параметры в ветке "/state".
   */
  g_signal_new ("device-state", HYSCAN_TYPE_DEVICE, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                g_cclosure_marshal_VOID__STRING,
                G_TYPE_NONE, 1,
                G_TYPE_STRING);

  /**
   * HyScanDevice::device-log:
   * @device: указатель на #HyScanDevice
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
  g_signal_new ("device-log", HYSCAN_TYPE_DEVICE, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__STRING_INT64_INT_STRING,
                G_TYPE_NONE, 4,
                G_TYPE_STRING,
                G_TYPE_INT64,
                G_TYPE_INT,
                G_TYPE_STRING);
}

/**
 * hyscan_device_set_sound_velocity:
 * @device: указатель на #HyScanDevice
 * @svp: (element-type: HyScanSoundVelocity) (transfer none): таблица профиля скорости звука
 *
 * Функция задаёт таблицу профиля скорости звука.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_device_set_sound_velocity (HyScanDevice *device,
                                  GList        *svp)
{
  HyScanDeviceInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DEVICE (device), FALSE);

  iface = HYSCAN_DEVICE_GET_IFACE (device);
  if (iface->set_sound_velocity != NULL)
    return (* iface->set_sound_velocity) (device, svp);

  return TRUE;
}

/**
 * hyscan_device_disconnect:
 * @device: указатель на #HyScanDevice
 *
 * Функция выполняет отключение от устройства. Отключение обязательно должно
 * быть выполнено до удаления объекта управления устройством.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_device_disconnect (HyScanDevice *device)
{
  HyScanDeviceInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DEVICE (device), FALSE);

  iface = HYSCAN_DEVICE_GET_IFACE (device);
  if (iface->disconnect != NULL)
    return (* iface->disconnect) (device);

  return TRUE;
}
