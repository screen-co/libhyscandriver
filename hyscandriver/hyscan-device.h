/* hyscan-device.h
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

#ifndef __HYSCAN_DEVICE_H__
#define __HYSCAN_DEVICE_H__

#include <hyscan-types.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DEVICE            (hyscan_device_get_type ())
#define HYSCAN_DEVICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DEVICE, HyScanDevice))
#define HYSCAN_IS_DEVICE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DEVICE))
#define HYSCAN_DEVICE_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_DEVICE, HyScanDeviceInterface))

typedef struct _HyScanDevice HyScanDevice;
typedef struct _HyScanDeviceInterface HyScanDeviceInterface;

/**
 * HyScanDeviceStatusType:
 * @HYSCAN_DEVICE_STATUS_ERROR: ошибка, устройство не может продолжать работу
 * @HYSCAN_DEVICE_STATUS_CRITICAL: в работе устройства присутствуют постоянные сбои
 * @HYSCAN_DEVICE_STATUS_WARNING: в работе устройства присутствуют кратковременные сбои
 * @HYSCAN_DEVICE_STATUS_OK: устройство функционирует в штатном режиме
 *
 * Статус устройства.
 */
typedef enum
{
  HYSCAN_DEVICE_STATUS_ERROR,
  HYSCAN_DEVICE_STATUS_CRITICAL,
  HYSCAN_DEVICE_STATUS_WARNING,
  HYSCAN_DEVICE_STATUS_OK
} HyScanDeviceStatusType;

/**
 * HyScanDeviceInterface:
 * @g_iface: Базовый интерфейс.
 * @sync: Функция синхронизирует состояние устройства в соответствии с заданными параметрами.
 * @set_sound_velocity: Функция задаёт таблицу профиля скорости звука.
 * @disconnect: Функция выполняет отключение от устройства.
 */
struct _HyScanDeviceInterface
{
  GTypeInterface       g_iface;

  gboolean             (*sync)                                 (HyScanDevice                   *device);

  gboolean             (*set_sound_velocity)                   (HyScanDevice                   *device,
                                                                GList                          *svp);

  gboolean             (*disconnect)                           (HyScanDevice                   *device);
};

HYSCAN_API
GType                  hyscan_device_get_type                  (void);

HYSCAN_API
gboolean               hyscan_device_sync                      (HyScanDevice                   *device);

HYSCAN_API
gboolean               hyscan_device_set_sound_velocity        (HyScanDevice                   *device,
                                                                GList                          *svp);

HYSCAN_API
gboolean               hyscan_device_disconnect                (HyScanDevice                   *device);

G_END_DECLS

#endif /* __HYSCAN_DEVICE_H__ */
