/* hyscan-uart.h
 *
 * Copyright 2016-2021 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_UART_H__
#define __HYSCAN_UART_H__

#include <hyscan-buffer.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_UART             (hyscan_uart_get_type ())
#define HYSCAN_UART(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_UART, HyScanUART))
#define HYSCAN_IS_UART(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_UART))
#define HYSCAN_UART_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_UART, HyScanUARTClass))
#define HYSCAN_IS_UART_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_UART))
#define HYSCAN_UART_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_UART, HyScanUARTClass))

typedef struct _HyScanUART HyScanUART;
typedef struct _HyScanUARTPrivate HyScanUARTPrivate;
typedef struct _HyScanUARTClass HyScanUARTClass;
typedef struct _HyScanUARTDevice HyScanUARTDevice;

struct _HyScanUART
{
  GObject parent_instance;

  HyScanUARTPrivate *priv;
};

struct _HyScanUARTClass
{
  GObjectClass parent_class;
};

/**
 * HyScanUARTMode:
 * @HYSCAN_UART_MODE_DISABLED: Порт отключен.
 * @HYSCAN_UART_MODE_AUTO: Автоматический выбор режима работы.
 * @HYSCAN_UART_MODE_4800_8N1: Скорость 4800 бод, 8N1.
 * @HYSCAN_UART_MODE_9600_8N1: Скорость 9600 бод, 8N1.
 * @HYSCAN_UART_MODE_19200_8N1: Скорость 19200 бод, 8N1.
 * @HYSCAN_UART_MODE_38400_8N1: Скорость 38400 бод, 8N1.
 * @HYSCAN_UART_MODE_57600_8N1: Скорость 57600 бод, 8N1.
 * @HYSCAN_UART_MODE_115200_8N1: Скорость 115200 бод, 8N1.
 * @HYSCAN_UART_MODE_230400_8N1: Скорость 230400 бод, 8N1.
 * @HYSCAN_UART_MODE_460800_8N1: Скорость 460800 бод, 8N1.
 * @HYSCAN_UART_MODE_921600_8N1: Скорость 921600 бод, 8N1.
 *
 * Режимы работы UART порта.
 */
typedef enum
{
  HYSCAN_UART_MODE_DISABLED,
  HYSCAN_UART_MODE_AUTO,
  HYSCAN_UART_MODE_4800_8N1,
  HYSCAN_UART_MODE_9600_8N1,
  HYSCAN_UART_MODE_19200_8N1,
  HYSCAN_UART_MODE_38400_8N1,
  HYSCAN_UART_MODE_57600_8N1,
  HYSCAN_UART_MODE_115200_8N1,
  HYSCAN_UART_MODE_230400_8N1,
  HYSCAN_UART_MODE_460800_8N1,
  HYSCAN_UART_MODE_921600_8N1
} HyScanUARTMode;

/**
 * HyScanUARTStatus:
 * @HYSCAN_UART_STATUS_OK: Без ошибок.
 * @HYSCAN_UART_STATUS_TIMEOUT: Таймаут обмена.
 * @HYSCAN_UART_STATUS_ERROR: Ошибка ввода/вывода.
 *
 * Статус обмена данными через UART порт.
 */
typedef enum
{
  HYSCAN_UART_STATUS_OK,
  HYSCAN_UART_STATUS_TIMEOUT,
  HYSCAN_UART_STATUS_ERROR
} HyScanUARTStatus;

/**
 * HyScanUARTDevice:
 * @name: название UART порта
 * @path: путь к файлу устройства порта
 *
 * Описание UART порта.
 */
struct _HyScanUARTDevice
{
  const gchar         *name;
  const gchar         *path;
};

HYSCAN_API
GType                  hyscan_uart_device_get_type     (void);

HYSCAN_API
GType                  hyscan_uart_get_type            (void);

HYSCAN_API
HyScanUART *           hyscan_uart_new                 (void);

HYSCAN_API
gboolean               hyscan_uart_open                (HyScanUART                *uart,
                                                        const gchar               *path,
                                                        HyScanUARTMode             mode);

HYSCAN_API
void                   hyscan_uart_close               (HyScanUART                *uart);

HYSCAN_API
const gchar *          hyscan_uart_get_path            (HyScanUART                *uart);

HYSCAN_API
HyScanUARTMode         hyscan_uart_get_mode            (HyScanUART                *uart);

HYSCAN_API
void                   hyscan_uart_timeout             (HyScanUART                *uart,
                                                        gdouble                    rx_timeout,
                                                        gdouble                    tx_timeout);

HYSCAN_API
HyScanUARTStatus       hyscan_uart_read                (HyScanUART                *uart,
                                                        HyScanBuffer              *buffer,
                                                        guint32                    size);

HYSCAN_API
HyScanUARTStatus       hyscan_uart_read_byte           (HyScanUART                *uart,
                                                        guint8                    *data);

HYSCAN_API
HyScanUARTStatus       hyscan_uart_write               (HyScanUART                *uart,
                                                        HyScanBuffer              *buffer,
                                                        guint32                   *size);

HYSCAN_API
HyScanUARTStatus       hyscan_uart_write_byte          (HyScanUART                *uart,
                                                        guint8                     data);

HYSCAN_API
GList *                hyscan_uart_list                (void);

HYSCAN_API
HyScanUARTDevice *     hyscan_uart_device_copy         (const HyScanUARTDevice    *device);

HYSCAN_API
void                   hyscan_uart_device_free         (HyScanUARTDevice          *device);

G_END_DECLS

#endif /* __HYSCAN_UART_H__ */
