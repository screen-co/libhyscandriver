/* dummy-driver.c
 *
 * Copyright 2016-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#include "hyscan-dummy-discover.h"
#include <hyscan-data-schema-builder.h>
#include <hyscan-driver.h>
#include <gmodule.h>

static HyScanDiscover *discover = NULL;
static HyScanDataSchema *info = NULL;

G_MODULE_EXPORT void
g_module_unload (GModule *module)
{
  g_clear_object (&discover);
  g_clear_object (&info);
}

G_MODULE_EXPORT gpointer
hyscan_driver_discover (void)
{
  if (discover == NULL)
    discover = HYSCAN_DISCOVER (hyscan_dummy_discover_new ());

  return g_object_ref (discover);
}

G_MODULE_EXPORT gpointer
hyscan_driver_info (void)
{
  if (info == NULL)
    {
      HyScanDataSchemaBuilder *builder;

      builder = hyscan_data_schema_builder_new ("driver-info");

      hyscan_data_schema_builder_key_integer_create (builder, "/schema/id",
                                                     "Schema id", "Schema id",
                                                     HYSCAN_DRIVER_SCHEMA_ID);

      hyscan_data_schema_builder_key_integer_create (builder, "/schema/version",
                                                     "Schema version", "Schema version",
                                                     HYSCAN_DRIVER_SCHEMA_VERSION);

      hyscan_data_schema_builder_key_integer_create (builder, "/api/version",
                                                     "API version", "API version",
                                                     HYSCAN_DISCOVER_API);

      hyscan_data_schema_builder_key_integer_create (builder, "/dummy",
                                                     "Dummy mark", "Dummy mark",
                                                     DUMMY);

      info = hyscan_data_schema_builder_get_schema (builder);

      g_object_unref (builder);
    }

  return g_object_ref (info);
}
