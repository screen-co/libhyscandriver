/* hyscan-uart.c
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

/**
 * SECTION: hyscan-uart
 * @Short_description: класс обмена данными через UART порт
 * @Title: HyScanUART
 *
 * Класс предназначен для обмена данными через UART порт. Перед началом работы
 * необходимо указать какой порт использовать для обмена и в каком режиме. Для
 * этого предназначена функция #hyscan_uart_open. Функция #hyscan_uart_close
 * должна быть вызвана после заверщения работы с портом. Класс допускает
 * повторное использование. Т.е. можно закрыть UART порт, а затем открыть
 * другой или тот-же самый в другом режиме работы.
 *
 * Узнать путь к открытому UART порту и режиму его работы можно с помощью
 * функций #hyscan_uart_get_path и #hyscan_uart_get_mode.
 *
 * При обмене данными через UART порт, возможны ситуации таймаута при чтении
 * или записи. По умолчанию таймаут установлен в 1 секунду. Это означает, что
 * если в течение этого времени не было принято (отправлено) ни одного байта,
 * функции обмена данными завершают свою работу с соответствующей ошибкой
 * (#HyScanUARTStatus). Для задания пользовательских параметров таймаута
 * предназначена функция #hyscan_uart_timeout.
 *
 * Для обмена данными предназначены функции #hyscan_uart_read,
 * #hyscan_uart_read_byte, #hyscan_uart_write и #hyscan_uart_write_byte.
 *
 * Если обмен данными завершился с ошибкой #HYSCAN_UART_STATUS_ERROR, то это
 * обозначает что порт более не доступен. Требуется его закрыть и попытаться
 * открыть заново.
 *
 * Список UART портов, доступных в системе, можно получить с помощью функции
 * #hyscan_uart_list.
 */

#include "hyscan-uart.h"

#if defined (G_OS_UNIX)

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#define HANDLE gint
#define INVALID_HANDLE_VALUE -1

#elif defined (G_OS_WIN32)

#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>
#include <tchar.h>

#else

#error "unsupported platform"

#endif

#define DEFAULT_TIMEOUT        1.0     /* Таймаут по умолчанию. */

struct _HyScanUARTPrivate
{
  HANDLE               fd;             /* Дескриптор открытого порта. */
  gchar               *path;           /* Путь к UART порту. */
  HyScanUARTMode       mode;           /* Текущий режим работы порта. */
  guint32              block_size;     /* Размер блока записи данных. */
  gdouble              rx_timeout;     /* Таймаут операции чтения данных. */
  gdouble              tx_timeout;     /* Таймаут операции записи данных. */
};

static void    hyscan_uart_object_constructed       (GObject               *object);
static void    hyscan_uart_object_finalize          (GObject               *object);

static guint32 hyscan_uart_get_speed                (HyScanUARTMode         mode);

static HANDLE  hyscan_uart_open_internal            (const gchar           *path,
                                                     HyScanUARTMode         mode);

static void    hyscan_uart_close_internal           (HANDLE                 fd);

static void    hyscan_uart_timeout_internal         (HANDLE                 fd,
                                                     gdouble                rx_timeout,
                                                     gdouble                tx_timeout);

static HyScanUARTStatus
               hyscan_uart_read_internal            (HyScanUARTPrivate     *priv,
                                                     guint8                *buffer,
                                                     guint32               *size);

static HyScanUARTStatus
               hyscan_uart_write_internal           (HyScanUARTPrivate     *priv,
                                                     guint8                *buffer,
                                                     guint32               *size);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanUART, hyscan_uart, G_TYPE_OBJECT)

static void
hyscan_uart_class_init (HyScanUARTClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = hyscan_uart_object_constructed;
  object_class->finalize = hyscan_uart_object_finalize;
}

static void
hyscan_uart_init (HyScanUART *uart)
{
  uart->priv = hyscan_uart_get_instance_private (uart);
}

static void
hyscan_uart_object_constructed (GObject *object)
{
  HyScanUART *uart = HYSCAN_UART (object);
  HyScanUARTPrivate *priv = uart->priv;

  priv->fd = INVALID_HANDLE_VALUE;
}

static void
hyscan_uart_object_finalize (GObject *object)
{
  HyScanUART *uart = HYSCAN_UART (object);

  hyscan_uart_close (uart);

  G_OBJECT_CLASS (hyscan_uart_parent_class)->finalize (object);
}

static guint32
hyscan_uart_get_speed (HyScanUARTMode mode)
{
  switch (mode)
    {
    case HYSCAN_UART_MODE_4800_8N1:
      return 600;
    case HYSCAN_UART_MODE_9600_8N1:
      return 1200;
    case HYSCAN_UART_MODE_19200_8N1:
      return 2400;
    case HYSCAN_UART_MODE_38400_8N1:
      return 4800;
    case HYSCAN_UART_MODE_57600_8N1:
      return 7200;
    case HYSCAN_UART_MODE_115200_8N1:
      return 14400;
    case HYSCAN_UART_MODE_230400_8N1:
      return 28800;
    case HYSCAN_UART_MODE_460800_8N1:
      return 57600;
    case HYSCAN_UART_MODE_921600_8N1:
      return 115200;
    default:
      break;
    }

  return 0;
}

#if defined (G_OS_UNIX)

static HANDLE
hyscan_uart_open_internal (const gchar    *path,
                           HyScanUARTMode  mode)
{
  struct termios options = {0};
  HANDLE fd;

  /* Открываем порт. */
  fd = open (path, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  if (fd < 0)
    goto fail;

  /* Выбор режима работы. */
  switch (mode)
    {
    case HYSCAN_UART_MODE_4800_8N1:
      options.c_cflag = B4800;
      break;

    case HYSCAN_UART_MODE_9600_8N1:
      options.c_cflag = B9600;
      break;

    case HYSCAN_UART_MODE_19200_8N1:
      options.c_cflag = B19200;
      break;

    case HYSCAN_UART_MODE_38400_8N1:
      options.c_cflag = B38400;
      break;

    case HYSCAN_UART_MODE_57600_8N1:
      options.c_cflag = B57600;
      break;

    case HYSCAN_UART_MODE_115200_8N1:
      options.c_cflag = B115200;
      break;

    case HYSCAN_UART_MODE_230400_8N1:
      options.c_cflag = B230400;
      break;

    case HYSCAN_UART_MODE_460800_8N1:
      options.c_cflag = B460800;
      break;

    case HYSCAN_UART_MODE_921600_8N1:
      options.c_cflag = B921600;
      break;

    default:
      goto fail;
    }

  /* Устанавливаем параметры устройства. */
  cfmakeraw (&options);
  if (tcflush (fd, TCIFLUSH) != 0)
    goto fail;
  if (tcsetattr (fd, TCSANOW, &options) != 0)
    goto fail;

  return fd;

fail:
  return INVALID_HANDLE_VALUE;
}

static void
hyscan_uart_close_internal (HANDLE fd)
{
  close (fd);
}

static void
hyscan_uart_timeout_internal (HANDLE  fd,
                              gdouble rx_timeout,
                              gdouble tx_timeout)
{

}

static HyScanUARTStatus
hyscan_uart_read_internal (HyScanUARTPrivate *priv,
                           guint8            *buffer,
                           guint32           *size)
{
  fd_set set;
  struct timeval tv;

  guint32 total = 0;
  guint32 remain = *size;

  *size = 0;
  while (remain > 0)
    {
      gint selected;
      gssize readed;

      /* Ожидаем новые данные в течение timeout секунд. */
      FD_ZERO (&set);
      tv.tv_sec = (gint)priv->rx_timeout;
      tv.tv_usec = (gint)(G_USEC_PER_SEC * priv->rx_timeout) % G_USEC_PER_SEC;
      FD_SET (priv->fd, &set);

      selected = select (priv->fd + 1, &set, NULL, NULL, &tv);
      if (selected < 0)
        return  HYSCAN_UART_STATUS_ERROR;
      if (selected == 0)
        return  HYSCAN_UART_STATUS_TIMEOUT;

      /* Считываем доступные данные. */
      readed = read (priv->fd, buffer + total, remain);
      if ((readed < 0) && (errno != EAGAIN) && (errno != EWOULDBLOCK))
        return  HYSCAN_UART_STATUS_ERROR;

      total += readed;
      remain -= readed;
      *size = total;
    }

  return HYSCAN_UART_STATUS_OK;
}

static HyScanUARTStatus
hyscan_uart_write_internal (HyScanUARTPrivate *priv,
                            guint8            *buffer,
                            guint32           *size)
{
  fd_set set;
  struct timeval tv;

  guint32 total = 0;
  guint32 remain = *size;

  *size = 0;
  while (remain > 0)
    {
      gint selected;
      gssize written;

      /* Ожидаем возможность отправки данных timeout секунд. */
      FD_ZERO (&set);
      tv.tv_sec = (gint)priv->tx_timeout;
      tv.tv_usec = (gint)(G_USEC_PER_SEC * priv->tx_timeout) % G_USEC_PER_SEC;
      FD_SET (priv->fd, &set);

      selected = select (priv->fd + 1, NULL, &set, NULL, &tv);
      if (selected < 0)
        return  HYSCAN_UART_STATUS_ERROR;
      if (selected == 0)
        return  HYSCAN_UART_STATUS_TIMEOUT;

      /* Отправляем данные. */
      written = write (priv->fd, buffer + total, MIN (priv->block_size, remain));
      if ((written < 0) && (errno != EAGAIN) && (errno != EWOULDBLOCK))
        return  HYSCAN_UART_STATUS_ERROR;

      total += written;
      remain -= written;
      *size = total;
    }

  return HYSCAN_UART_STATUS_OK;
}

#elif defined (G_OS_WIN32)

static HANDLE
hyscan_uart_open_internal (const gchar    *path,
                           HyScanUARTMode  mode)
{
  HANDLE fd;
  DCB dcb = {0};

  /* Открываем порт. */
  fd = CreateFile (path, GENERIC_READ | GENERIC_WRITE , 0, 0, OPEN_EXISTING, 0, 0);
  if (fd == INVALID_HANDLE_VALUE)
    goto fail;

  /* Выбор режима работы. */
  switch (mode)
    {

    case HYSCAN_UART_MODE_4800_8N1:
      BuildCommDCB ("4800,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_9600_8N1:
      BuildCommDCB ("9600,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_19200_8N1:
      BuildCommDCB ("19200,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_38400_8N1:
      BuildCommDCB ("38400,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_57600_8N1:
      BuildCommDCB ("57600,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_115200_8N1:
      BuildCommDCB ("115200,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_230400_8N1:
      BuildCommDCB ("230400,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_460800_8N1:
      BuildCommDCB ("460800,n,8,1", &dcb);
      break;

    case HYSCAN_UART_MODE_921600_8N1:
      BuildCommDCB ("921600,n,8,1", &dcb);
      break;

    default:
      goto fail;
    }

  if (!SetCommState (fd, &dcb))
    goto fail;

  return fd;

fail:
  return INVALID_HANDLE_VALUE;
}

static void
hyscan_uart_close_internal (HANDLE fd)
{
  CloseHandle (fd);
}

static void
hyscan_uart_timeout_internal (HANDLE  fd,
                              gdouble rx_timeout,
                              gdouble tx_timeout)
{
  COMMTIMEOUTS cto = {0};

  cto.ReadIntervalTimeout = MAXDWORD;
  cto.ReadTotalTimeoutMultiplier = MAXDWORD;
  cto.ReadTotalTimeoutConstant = 1000 * rx_timeout;
  cto.WriteTotalTimeoutMultiplier = 0;
  cto.WriteTotalTimeoutConstant = 1000 * tx_timeout;

  SetCommTimeouts (fd, &cto);
}

static HyScanUARTStatus
hyscan_uart_read_internal (HyScanUARTPrivate *priv,
                           guint8            *buffer,
                           guint32           *size)
{
  guint32 total = 0;
  guint32 remain = *size;

  *size = 0;
  while (remain > 0)
    {
      DWORD readed;
      BOOL status;

      status = ReadFile (priv->fd, buffer + total, remain, &readed, NULL);
      if ((!status) || (readed == 0))
        {
          if (GetLastError ())
            return  HYSCAN_UART_STATUS_ERROR;

          return  HYSCAN_UART_STATUS_TIMEOUT;
        }

      total += readed;
      remain -= readed;
      *size = total;
    }

  return HYSCAN_UART_STATUS_OK;
}

static HyScanUARTStatus
hyscan_uart_write_internal (HyScanUARTPrivate *priv,
                            guint8            *buffer,
                            guint32           *size)
{
  guint32 total = 0;
  guint32 remain = *size;

  *size = 0;
  while (remain > 0)
    {
      DWORD written;
      BOOL status;

      status = WriteFile (priv->fd, buffer + total, MIN (priv->block_size, remain), &written, NULL);
      total += written;
      remain -= written;
      *size = total;

      if (!status)
        {
          if (GetLastError () == ERROR_COUNTER_TIMEOUT)
            return  HYSCAN_UART_STATUS_TIMEOUT;

          return  HYSCAN_UART_STATUS_ERROR;
        }
    }

  return HYSCAN_UART_STATUS_OK;
}

#endif

/**
 * hyscan_uart_new:
 *
 * Функция создаёт новый объект #HyScanUART.
 *
 * Returns: #HyScanUART. Для удаления #g_object_unref.
 */
HyScanUART *
hyscan_uart_new (void)
{
  return g_object_new (HYSCAN_TYPE_UART, NULL);
}

/**
 * hyscan_uart_open:
 * @uart: указатель на #HyScanUART
 * @path: UART порт
 * @mode: режим работы
 *
 * Функция открывает UART порт и задаёт режим его работы. При этом
 * автоматически устанавливается таймаут обмена данными 1с.
 *
 * Returns: %TRUE если UART порт открыт, иначе %FALSE.
 */
gboolean
hyscan_uart_open (HyScanUART     *uart,
                  const gchar    *path,
                  HyScanUARTMode  mode)
{
  HyScanUARTPrivate *priv;

  g_return_val_if_fail (HYSCAN_IS_UART (uart), FALSE);

  priv = uart->priv;

  hyscan_uart_close (uart);
  priv->fd = hyscan_uart_open_internal (path, mode);
  if (priv->fd != INVALID_HANDLE_VALUE)
    {
      priv->path = g_strdup (path);
      priv->mode = mode;
    }

  hyscan_uart_timeout (uart, DEFAULT_TIMEOUT, DEFAULT_TIMEOUT);

  return (priv->fd != INVALID_HANDLE_VALUE);
}

/**
 * hyscan_uart_close:
 * @uart: указатель на #HyScanUART
 *
 * Функция закрывает UART порт.
 */
void
hyscan_uart_close (HyScanUART *uart)
{
  HyScanUARTPrivate *priv;

  g_return_if_fail (HYSCAN_IS_UART (uart));

  priv = uart->priv;

  if (priv->fd == INVALID_HANDLE_VALUE)
    return;

  hyscan_uart_close_internal (priv->fd);

  g_clear_pointer (&priv->path, g_free);
  priv->mode = HYSCAN_UART_MODE_DISABLED;
  priv->fd = INVALID_HANDLE_VALUE;
}

/**
 * hyscan_uart_get_path:
 * @uart: указатель на #HyScanUART
 *
 * Функция возвращает путь к UART порту.
 *
 * Returns: (nullable): Путь к UART порту или NULL.
 */
const gchar *
hyscan_uart_get_path (HyScanUART *uart)
{
  g_return_val_if_fail (HYSCAN_IS_UART (uart), NULL);

  return uart->priv->path;
}

/**
 * hyscan_uart_get_mode:
 * @uart: указатель на #HyScanUART
 *
 * Функция возвращает режим работы UART порта.
 *
 * Returns: Режим работы UART порта.
 */
HyScanUARTMode
hyscan_uart_get_mode (HyScanUART *uart)
{
  g_return_val_if_fail (HYSCAN_IS_UART (uart), HYSCAN_UART_MODE_DISABLED);

  return uart->priv->mode;
}

/**
 * hyscan_uart_timeout:
 * @uart: указатель на #HyScanUART
 * @rx_timeout: таймаут при чтении данных, с
 * @tx_timeout: таймаут при записи данных, с
 *
 * Функция устанавливает таймаут обмена данными.
 */
void
hyscan_uart_timeout (HyScanUART *uart,
                     gdouble     rx_timeout,
                     gdouble     tx_timeout)
{
  HyScanUARTPrivate *priv;
  guint32 block_size;

  g_return_if_fail (HYSCAN_IS_UART (uart));

  priv = uart->priv;

  rx_timeout = CLAMP (rx_timeout, 0.001, 60.0);
  tx_timeout = CLAMP (tx_timeout, 0.001, 60.0);

  priv->rx_timeout = rx_timeout;
  priv->tx_timeout = tx_timeout;

  block_size = hyscan_uart_get_speed (priv->mode) * tx_timeout / 10;
  priv->block_size = CLAMP (block_size, 16, 512);

  hyscan_uart_timeout_internal (priv->fd, rx_timeout, tx_timeout);
}

/**
 * hyscan_uart_read:
 * @uart: указатель на #HyScanUART
 * @buffer: буфер для принятых данных
 * @size: размер принимаемых данных
 *
 * Функция считывает данные из UART порта. Требуемый размер данных
 * указывается в @size. Сколько данных было реально принято можно
 * узнать функцией #hyscan_buffer_get_data_size. Размер принятых
 * данных может быть меньше требуемого, если обмен завершился с
 * ошибкой или по таймауту.
 *
 * Returns: Статус приёма данных.
 */
HyScanUARTStatus
hyscan_uart_read (HyScanUART   *uart,
                  HyScanBuffer *buffer,
                  guint32       size)
{
  HyScanUARTStatus status;
  guint8 *data;

  g_return_val_if_fail (HYSCAN_IS_UART (uart), FALSE);

  hyscan_buffer_set (buffer, HYSCAN_DATA_BLOB, NULL, size);
  data = hyscan_buffer_get (buffer, NULL, &size);

  status = hyscan_uart_read_internal (uart->priv, data, &size);
  if (status != HYSCAN_UART_STATUS_ERROR)
    hyscan_buffer_set_data_size (buffer, size);

  return status;
}

/**
 * hyscan_uart_read_byte:
 * @uart: указатель на #HyScanUART
 * @data: (out): буфер для принятых данных
 *
 * Функция считывает один байт из UART порта.
 *
 * Returns: Статус приёма данных.
 */
HyScanUARTStatus
hyscan_uart_read_byte (HyScanUART *uart,
                       guint8     *data)
{
  guint32 size = 1;

  g_return_val_if_fail (HYSCAN_IS_UART (uart), FALSE);

  return hyscan_uart_read_internal (uart->priv, data, &size);
}

/**
 * hyscan_uart_write:
 * @uart: указатель на #HyScanUART
 * @buffer: буфер отправляемых данных
 * @size: (out): размер отправленных данных
 *
 * Функция отправляет данные в UART порт. Функция пытается отправить
 * все данные из буфера. Размер отправленных данных может быть меньше
 * требуемого, если обмен завершился с ошибкой или по таймауту.
 *
 * Returns: Статус отправки данных.
 */
HyScanUARTStatus
hyscan_uart_write (HyScanUART   *uart,
                   HyScanBuffer *buffer,
                   guint32      *size)
{
  HyScanUARTStatus status;
  guint32 written;
  guint8 *data;

  g_return_val_if_fail (HYSCAN_IS_UART (uart), FALSE);

  data = hyscan_buffer_get (buffer, NULL, &written);
  status = hyscan_uart_write_internal (uart->priv, data, &written);
  (size != NULL) ? *size = written : 0;

  return status;
}

/**
 * hyscan_uart_write_byte:
 * @uart: указатель на #HyScanUART
 * @data: данные для отправки
 *
 * Функция отправляет один байт в UART порт.
 *
 * Returns: Статус отправки данных.
 */
HyScanUARTStatus
hyscan_uart_write_byte (HyScanUART *uart,
                        guint8      data)
{
  guint32 written = 1;

  g_return_val_if_fail (HYSCAN_IS_UART (uart), FALSE);

  return hyscan_uart_write_internal (uart->priv, &data, &written);
}

/**
 * hyscan_uart_list:
 *
 * Функция возвращает список UART устройств.
 *
 * Память выделенная под список должна быть освобождена после использования
 * функцией #g_list_free_full. Для освобождения элементов списка необходимо
 * использовать функцию #hyscan_uart_device_free.
 *
 * Returns: (element-type HyScanUARTDevice) (transfer full): Список UART
 * устройств или NULL.
 */
#if defined (G_OS_UNIX)
GList *
hyscan_uart_list (void)
{
  GList *list = NULL;

  GDir *dir;
  const gchar *device;

  /* Список всех устройств. */
  dir = g_dir_open ("/dev", 0, NULL);
  if (dir == NULL)
    return NULL;

  while ((device = g_dir_read_name (dir)) != NULL)
    {
      struct termios options;
      gboolean usb_serial;
      gboolean serial;
      gchar *path;
      int fd;

      /* Пропускаем элементы "." и ".." */
      if (device[0] =='.' && device[1] == 0)
        continue;
      if (device[0] =='.' && device[1] =='.' && device[2] == 0)
        continue;

      /* Пропускаем все устройства, имена которых не начинаются на ttyS или ttyUSB. */
      serial = g_str_has_prefix (device, "ttyS");
      usb_serial = g_str_has_prefix (device, "ttyUSB");
      if (!serial && !usb_serial)
        continue;

      /* Открываем устройство и проверяем его тип. */
      path = g_strdup_printf ("/dev/%s", device);
      fd = open (path, O_RDWR | O_NONBLOCK | O_NOCTTY);
      g_free (path);

      /* Если не смогли открыть файл, возможно у нас нет на это прав. */
      if (fd < 0)
        continue;

      /* Проверяем тип устройства. */
      if (tcgetattr (fd, &options) == 0)
        {
          HyScanUARTDevice *port;
          gint index;

          if (serial)
            index = g_ascii_strtoll(device + 4, NULL, 10);
          else
            index = g_ascii_strtoll(device + 6, NULL, 10);

          port = g_slice_new (HyScanUARTDevice);
          port->name = g_strdup_printf ("%s%d", serial ? "COM" : "USBCOM", index + 1);
          port->path = g_strdup_printf ("/dev/%s", device);

          list = g_list_prepend (list, port);
        }

      close (fd);
    }

  g_dir_close (dir);

  return list;
}

#elif defined (G_OS_WIN32)

GList *
hyscan_uart_list (void)
{
  GList *list = NULL;

  HDEVINFO dev_info;
  SP_DEVINFO_DATA dev_data;
  DWORD dev_index;

  /* Список всех устройств типа UART. */
  dev_info = SetupDiGetClassDevs ((const GUID *)&GUID_DEVCLASS_PORTS, NULL, NULL, DIGCF_PRESENT);
  dev_data.cbSize = sizeof (SP_DEVINFO_DATA);
  dev_index = 0;

  while (SetupDiEnumDeviceInfo(dev_info, dev_index, &dev_data))
    {
      HKEY hkey;
      LONG status;
      gchar port_path[1024] = {0};
      gchar port_name[1024] = {0};
      DWORD port_path_length = sizeof (port_path);
      DWORD port_name_length = sizeof (port_name);

      HyScanUARTDevice *port;
      gboolean is_usb = FALSE;

      dev_index += 1;

      /* Путь к порту из реестра. */
      hkey = SetupDiOpenDevRegKey (dev_info, &dev_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
      status = RegQueryValueEx (hkey, _T ("PortName"), NULL, NULL, (LPBYTE)port_path, &port_path_length);
      RegCloseKey(hkey);

      if (status != EXIT_SUCCESS)
        continue;

      /* Пропускаем LPT порты. */
      if (g_str_has_prefix (port_path, "LPT"))
        continue;

      /* Описание порта. */
      status = SetupDiGetDeviceRegistryProperty (dev_info, &dev_data,
                                                 SPDRP_FRIENDLYNAME, NULL, (PBYTE)port_name,
                                                 port_name_length, &port_name_length);
      if (!status)
        port_name_length = 0;

      if ((g_strstr_len (port_name, port_name_length, "USB") != NULL) ||
          (g_strstr_len (port_name, port_name_length, "usb") != NULL))
        {
          is_usb = TRUE;
        }

      port = g_slice_new (HyScanUARTDevice);
      port->path = g_strdup_printf ("\\\\.\\%s", port_path);

      if (is_usb)
        port->name = g_strdup_printf("USB%s", port_path);
      else
        port->name = g_strdup_printf("%s", port_path);

      list = g_list_prepend (list, port);
  }

  return list;
}

#endif

/**
 * hyscan_uart_device_copy:
 * @device: структура #HyScanUARTDevice для копирования
 *
 * Функция создаёт копию структуры #HyScanUARTDevice.
 *
 * Returns: (transfer full): Новая структура #HyScanUARTDevice.
 * Для удаления #hyscan_uart_device_free.
 */
HyScanUARTDevice *
hyscan_uart_device_copy (const HyScanUARTDevice *device)
{
  HyScanUARTDevice *new_device = g_slice_new (HyScanUARTDevice);

  new_device->name = g_strdup (device->name);
  new_device->path = g_strdup (device->path);

  return new_device;
}

/**
 * hyscan_uart_device_free:
 * @device: структура #HyScanUARTDevice для удаления
 *
 * Функция удаляет структуру #HyScanUARTDevice.
 */
void
hyscan_uart_device_free (HyScanUARTDevice *device)
{
  g_free ((gchar*)device->name);
  g_free ((gchar*)device->path);
  g_slice_free (HyScanUARTDevice, device);
}

G_DEFINE_BOXED_TYPE (HyScanUARTDevice, hyscan_uart_device, hyscan_uart_device_copy, hyscan_uart_device_free)
