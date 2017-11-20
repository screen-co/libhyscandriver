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
    discover = g_object_new (HYSCAN_TYPE_DUMMY_DISCOVER, NULL);

  return g_object_ref (discover);
}

G_MODULE_EXPORT gpointer
hyscan_driver_info (void)
{
  if (info == NULL)
    {
      HyScanDataSchemaBuilder *builder;
      gchar *schema_data;

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

      schema_data = hyscan_data_schema_builder_get_data (builder);
      info = hyscan_data_schema_new_from_string (schema_data, "driver-info");

      g_object_unref (builder);
      g_free (schema_data);
    }

  return g_object_ref (info);
}
