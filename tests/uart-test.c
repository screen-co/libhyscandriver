/* driver-test.c
 *
 * Copyright 2021 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#include <hyscan-uart.h>
#include <string.h>

gchar *sender_port = NULL;
gchar *receiver_port = NULL;
gdouble timeout = 0.1;
gint baud_rate = 115200;
gint data_size = 32768;
gboolean list_uarts = FALSE;
HyScanUARTMode uart_mode;

HyScanBuffer *image;
gint sender_sync = 0;
gint receiver_sync = 0;
gint stage = 0;

gpointer
worker (gpointer data)
{
  gchar *port = data;
  gint *sync;

  HyScanUART *uart;
  HyScanBuffer *buffer;

  guint32 io_size;
  HyScanUARTStatus status;

  uart = hyscan_uart_new ();
  buffer = hyscan_buffer_new ();

  if (port == sender_port)
    sync = &sender_sync;
  else if (port == receiver_port)
    sync = &receiver_sync;
  else
    g_error ("unknown worker");

  /* Этап 1 - открытие порта. */
  while (g_atomic_int_get (&stage) != 1);

  if (!hyscan_uart_open (uart, port, uart_mode))
    g_error ("can't open port '%s'", port);
  else
    g_print ("%s port %s\n", (port == sender_port) ? "sender" : "receiver", port);

  hyscan_uart_timeout (uart, timeout, timeout);

  g_atomic_int_set (sync, 1);

  /* Этап 2 - отправка всего блока данных разом. */
  while (g_atomic_int_get (&stage) != 2);

  io_size = data_size;
  if (port == sender_port)
    status = hyscan_uart_write (uart, image, &io_size);
  else
    status = hyscan_uart_read (uart, buffer, io_size);

  if (status != HYSCAN_UART_STATUS_OK)
    g_error ("stage 2 error in %s", (port == sender_port) ? "sender" : "receiver");
  else
    g_print ("%s io full operation completed\n", (port == sender_port) ? "sender" : "receiver");

  if (port == receiver_port)
    {
      guint8 *data1;
      guint8 *data2;
      guint32 size1;
      guint32 size2;

      data1 = hyscan_buffer_get (image, NULL, &size1);
      data2 = hyscan_buffer_get (buffer, NULL, &size2);

      if (memcmp (data1, data2, size1) != 0)
        g_error ("data error");
      else
        g_print ("data ok\n");
    }

  g_atomic_int_set (sync, 2);

  /* Этап 3 - отправка половины блока и завершение приёма по таймауту. */
  while (g_atomic_int_get (&stage) != 3);

  if (port == sender_port)
    {
      hyscan_buffer_copy (buffer, image);
      hyscan_buffer_set_data_size (buffer, data_size / 2);
      status = hyscan_uart_write (uart, buffer, &io_size);
      if (status != HYSCAN_UART_STATUS_OK)
        g_error ("stage 3 error in sender");

      g_print ("sender io half operation completed\n");
    }
  else
    {
      GTimer *timer = g_timer_new ();

      io_size = data_size;
      g_timer_start (timer);
      status = hyscan_uart_read (uart, buffer, io_size);
      g_timer_stop (timer);

      if (status != HYSCAN_UART_STATUS_TIMEOUT)
        g_error ("stage 3 error in receiver");

      g_print ("receiver io half operation completed\n");

      g_print ("receiver time: estimated = %f, elapsed = %f\n",
               ((data_size / 2.0) / (baud_rate / 8)),
               g_timer_elapsed (timer, NULL));

      g_timer_destroy (timer);
    }

  g_atomic_int_set (sync, 3);

  g_clear_object (&buffer);
  g_clear_object (&uart);

  return NULL;
}

int
main (int    argc,
      char **argv)
{
  GThread *sender;
  GThread *receiver;

  guint8 *data;
  guint32 size;
  gint i;

  {
    gchar **args;
    GError *error = NULL;
    GOptionContext *context;
    GOptionEntry entries[] =
      {
        { "sender", 's', 0, G_OPTION_ARG_STRING, &sender_port, "Sender UART port", NULL },
        { "receiver", 'r', 0, G_OPTION_ARG_STRING, &receiver_port, "Receiver UART port", NULL },
        { "timeout", 't', 0, G_OPTION_ARG_DOUBLE, &timeout, "IO timeout", NULL },
        { "baud", 'b', 0, G_OPTION_ARG_INT, &baud_rate, "Baud rate (default 115200)", NULL },
        { "size", 'd', 0, G_OPTION_ARG_INT, &data_size, "Test data size (default 32768)", NULL },
        { "list", 'l', 0, G_OPTION_ARG_NONE, &list_uarts, "List all uart devices", NULL },
        { NULL }
      };

#ifdef G_OS_WIN32
    args = g_win32_get_command_line ();
#else
    args = g_strdupv (argv);
#endif

    context = g_option_context_new ("");
    g_option_context_set_help_enabled (context, TRUE);
    g_option_context_add_main_entries (context, entries, NULL);
    g_option_context_set_ignore_unknown_options (context, FALSE);
    if (!g_option_context_parse_strv (context, &args, &error))
      {
        g_print ("%s\n", error->message);
        return -1;
      }

    if (((sender_port == NULL) || (receiver_port == NULL)) && (!list_uarts))
      {
        g_print ("%s", g_option_context_get_help (context, FALSE, NULL));
        return 0;
      }

    g_option_context_free (context);
    g_strfreev (args);
  }

  /* Список UART портов. */
  if (list_uarts)
    {
      GList *list;
      GList *uart;

      uart = list = hyscan_uart_list ();
      while (uart != NULL)
        {
          HyScanUARTDevice *device = uart->data;

          g_print ("%s\t%s\n", device->name, device->path);

          uart = g_list_next (uart);
        }

      g_list_free_full (list, (GDestroyNotify)hyscan_uart_device_free);

      return 0;
    }

  /* Скорость обмена.  */
  switch (baud_rate)
    {
    case 4800:
      uart_mode = HYSCAN_UART_MODE_4800_8N1;
      break;

    case 9600:
      uart_mode = HYSCAN_UART_MODE_9600_8N1;
      break;

    case 19200:
      uart_mode = HYSCAN_UART_MODE_19200_8N1;
      break;

    case 38400:
      uart_mode = HYSCAN_UART_MODE_38400_8N1;
      break;

    case 57600:
      uart_mode = HYSCAN_UART_MODE_57600_8N1;
      break;

    case 115200:
      uart_mode = HYSCAN_UART_MODE_115200_8N1;
      break;

    case 230400:
      uart_mode = HYSCAN_UART_MODE_230400_8N1;
      break;

    case 460800:
      uart_mode = HYSCAN_UART_MODE_460800_8N1;
      break;

    case 921600:
      uart_mode = HYSCAN_UART_MODE_921600_8N1;
      break;

    default:
      g_error ("unsupported baud rate %d", baud_rate);
    }

  /* Образец данных. */
  image = hyscan_buffer_new ();
  data_size = CLAMP (data_size, 32, 1024*1024);
  hyscan_buffer_set (image, HYSCAN_DATA_BLOB, NULL, data_size);
  data = hyscan_buffer_get (image, NULL, &size);
  for (i = 0; i < data_size; i++)
    data[i] = g_random_int_range (0, 255);

  /* Потоки обмена данными. */
  sender = g_thread_new ("sender", worker, sender_port);
  receiver = g_thread_new ("receiver", worker, receiver_port);

  /* Открываем порты. */
  g_atomic_int_set (&stage, 1);
  while (g_atomic_int_get (&sender_sync) != 1 ||
         g_atomic_int_get (&receiver_sync) != 1);

  /* Обмен полным блоком данных. */
  g_atomic_int_set (&stage, 2);
  while (g_atomic_int_get (&sender_sync) != 2 ||
         g_atomic_int_get (&receiver_sync) != 2);

  /* Обмен половиной блока данных. */
  g_atomic_int_set (&stage, 3);
  while (g_atomic_int_get (&sender_sync) != 3 ||
         g_atomic_int_get (&receiver_sync) != 3);

  g_thread_join (sender);
  g_thread_join (receiver);

  g_clear_object (&image);

  g_free (sender_port);
  g_free (receiver_port);

  return 0;
}
