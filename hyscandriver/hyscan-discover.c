/* hyscan-discover.c
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
 * SECTION: hyscan-discover
 * @Short_description: интерфейс обнаружения устройств
 * @Title: HyScanDiscover
 *
 * Интерфейс предназначен для обнаружения различных устройств подключенных
 * к компьютеру. Устройствами могут являться гидролокаторы, датчики, приводы
 * и т.п. Реализация данного интерфейса зависит от устройства и должна
 * размещаться в его драйвере.
 *
 * Интерфейс содержит функции, управляющие процессом обнаружения устройств:
 *
 * - #hyscan_discover_start - инициирует обнаружение устройст;
 * - #hyscan_discover_stop - прерывает обнаружение устройств;
 *
 * Список обнаруженных устройств можно получить функцией #hyscan_discover_list.
 * Память, используемая списоком, должена быть освобождена функцией #hyscan_discover_free_info.
 *
 * При подключении к устройству можно дополнительно передать параметры драйвера.
 * Список параметров драйвера можно узнать с помощью функции #hyscan_discover_config.
 *
 * Для проверки возможности подключения к устройству до завершения процесса
 * обнаружения устройств, а также для устройств с нестандартными адресами, можно
 * использовать функцию #hyscan_discover_check.
 *
 * Для подключения к устройству используется функция #hyscan_discover_connect.
 * После подключения возвращается указатель на объект реализующий интерфейсы
 * #HyScanParam, #HyScanSonar и #HyScanSensor.
 *
 * Если произведено подключение к гидролокатору, который не содержит встроенных
 * датчиков, интерфейс #HyScanSensor может быть не реализован. Аналогично, при
 * подключении только к датчику, не реализуется интерфейс #HyScanSonar.
 *
 * Функции #hyscan_discover_start, #hyscan_discover_stop и #hyscan_discover_list
 * работают в неблокирующем режиме. Остальные функции могут заблокировать работу
 * до завешения процесса.
 */

#include "hyscan-discover.h"

G_DEFINE_INTERFACE (HyScanDiscover, hyscan_discover, G_TYPE_OBJECT)

static void
hyscan_discover_default_init (HyScanDiscoverInterface *iface)
{
  /**
   * HyScanDiscover::progress:
   * @discover: указатель на #HyScanDiscover
   * @progress: прогресс поиска в процентах (0 - 100)
   *
   * Данный сигнал периодически посылается во время процесса обнаружения и
   * несёт информацию о его прогрессе. Сигнал посылается из рабочего потока
   * драйвера, такми образом обработчики этого сигнала не могут использовать
   * функции работающие через #GMainLoop, например все функции Gtk.
   */
  g_signal_new ("progress", HYSCAN_TYPE_DISCOVER, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                g_cclosure_marshal_VOID__DOUBLE,
                G_TYPE_NONE, 1, G_TYPE_DOUBLE);

  /**
   * HyScanDiscover::completed:
   * @discover: указатель на #HyScanDiscover
   *
   * Данный сигнал посылается при завершении процесса поиска. Сигнал посылается
   * из рабочего потока драйвера, такми образом обработчики этого сигнала не
   * могут использовать функции работающие через #GMainLoop, например все
   * функции Gtk.
   */
  g_signal_new ("completed", HYSCAN_TYPE_DISCOVER, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

/**
 * hyscan_discover_start:
 * @discover: указатель на #HyScanDiscover
 *
 * Функция инициирует процесс обнаружения устройств.
 */
void
hyscan_discover_start (HyScanDiscover *discover)
{
  HyScanDiscoverInterface *iface;

  g_return_if_fail (HYSCAN_IS_DISCOVER (discover));

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->start != NULL)
    (* iface->start) (discover);
}

/**
 * hyscan_discover_start:
 * @discover: указатель на #HyScanDiscover
 *
 * Функция принудительно останавливает процесс обнаружения устройств.
 */
void
hyscan_discover_stop (HyScanDiscover *discover)
{
  HyScanDiscoverInterface *iface;

  g_return_if_fail (HYSCAN_IS_DISCOVER (discover));

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->stop != NULL)
    (* iface->stop) (discover);
}

/**
 * hyscan_discover_list:
 * @discover: указатель на #HyScanDiscover
 *
 * Функция возвращает список обнаруженных устройств с общей информацией о них.
 *
 * Память выделенная под список должна быть освобождена после использования
 * функцией #g_list_free_full. Для освобождения элементов списка необходимо
 * использовать функцию #hyscan_discover_info_free.
 *
 * Returns: (element-type HyScanDiscoverInfo) (transfer full): список устройств.
 */
GList *
hyscan_discover_list (HyScanDiscover *discover)
{
  HyScanDiscoverInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DISCOVER (discover), NULL);

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->list != NULL)
    return (* iface->list) (discover);

  return NULL;
}

/**
 * hyscan_discover_config:
 * @discover: указатель на #HyScanDiscover
 * @uri: путь для подключения к устройству
 *
 * Функция возвращает схему с параметрами драйвера устройства. Эти параметры
 * можно передать в функцию #hyscan_discover_connect.
 *
 * Returns: (transfer full): #HyScanDataSchema или NULL если параметров нет. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_discover_config (HyScanDiscover *discover,
                        const gchar    *uri)
{
  HyScanDiscoverInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DISCOVER (discover), NULL);

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->config != NULL)
    return (* iface->config) (discover, uri);

  return NULL;
}

/**
 * hyscan_discover_check:
 * @discover: указатель на #HyScanDiscover
 * @uri: путь для подключения к устройству
 *
 * Функция проверяет присутствие устройства для указанного пути.
 *
 * Returns: %TRUE если устройство присутствует, иначе %FALSE.
 */
gboolean
hyscan_discover_check (HyScanDiscover *discover,
                       const gchar    *uri)
{
  HyScanDiscoverInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DISCOVER (discover), FALSE);

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->check != NULL)
    return (* iface->check) (discover, uri);

  return FALSE;
}

/**
 * hyscan_discover_connect:
 * @discover: указатель на #HyScanDiscover
 * @uri: путь для подключения к устройству
 * @params: параметры драйвера
 *
 * Функция производит подключение к устройству.
 *
 * Returns: (transfer full): #HyScanDevice или NULL в случае ошибки. Для удаления #g_object_unref.
 */
HyScanDevice *
hyscan_discover_connect (HyScanDiscover  *discover,
                         const gchar     *uri,
                         HyScanParamList *params)
{
  HyScanDiscoverInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_DISCOVER (discover), NULL);

  iface = HYSCAN_DISCOVER_GET_IFACE (discover);
  if (iface->connect != NULL)
    return (* iface->connect) (discover, uri, params);

  return NULL;
}

/**
 * hyscan_discover_info_new:
 * @info: краткая информация об устройстве
 * @uri: путь для подключения к устройству
 *
 * Функция создаёт экземпляр структуры #HyScanDiscoverInfo.
 *
 * Returns: (transfer full): #HyScanDiscoverInfo. Для удаления #hyscan_discover_info_free.
 *
 */
HyScanDiscoverInfo *
hyscan_discover_info_new (const gchar *info,
                          const gchar *uri)
{
  HyScanDiscoverInfo list;

  list.info = info;
  list.uri = uri;

  return hyscan_discover_info_copy (&list);
}

/**
 * hyscan_discover_info_copy:
 * @info: указатель на #HyScanDiscoverInfo
 *
 * Функция создаёт копию структуры #HyScanDiscoverInfo.
 *
 * Returns: (transfer full): #HyScanDiscoverInfo. Для удаления #hyscan_discover_info_free.
 *
 */
HyScanDiscoverInfo *
hyscan_discover_info_copy (HyScanDiscoverInfo *info)
{
  HyScanDiscoverInfo *new_info;

  new_info = g_slice_new (HyScanDiscoverInfo);
  new_info->info = g_strdup (info->info);
  new_info->uri = g_strdup (info->uri);

  return new_info;
}

/**
 * hyscan_discover_info_free:
 * @info: указатель на #HyScanDiscoverInfo
 *
 * Функция освобождает память занятую структурой #HyScanDiscoverInfo.
 */
void
hyscan_discover_info_free (HyScanDiscoverInfo *info)
{
  g_free ((gchar*)info->info);
  g_free ((gchar*)info->uri);

  g_slice_free (HyScanDiscoverInfo, info);
}

G_DEFINE_BOXED_TYPE (HyScanDiscoverInfo, hyscan_discover_info, hyscan_discover_info_copy, hyscan_discover_info_free)
