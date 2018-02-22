/* hyscan-dummy-discover.c
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

#include "hyscan-dummy-discover.h"

static void    hyscan_dummy_discover_interface_init           (HyScanDiscoverInterface *iface);

G_DEFINE_TYPE_WITH_CODE (HyScanDummyDiscover, hyscan_dummy_discover, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_DISCOVER, hyscan_dummy_discover_interface_init))

static void
hyscan_dummy_discover_class_init (HyScanDummyDiscoverClass *klass)
{
}

static void
hyscan_dummy_discover_init (HyScanDummyDiscover *dummy)
{
}

HyScanDummyDiscover *
hyscan_dummy_discover_new (void)
{
  return g_object_new (HYSCAN_TYPE_DUMMY_DISCOVER, NULL);
}

static void
hyscan_dummy_discover_interface_init (HyScanDiscoverInterface *iface)
{
  iface->start = NULL;
  iface->stop = NULL;
  iface->list = NULL;
  iface->check = NULL;
  iface->config = NULL;
  iface->connect = NULL;
}
