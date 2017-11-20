#include <hyscan-driver.h>
#include <libxml/parser.h>

int
main (int    argc,
      char **argv)
{
  const gchar *path;
  gchar **names;
  guint i;

  /* Путь к драйверам. */
  if (argv[1] == NULL)
    {
      g_print ("Usage: driver-test <path-to-drivers>\n");
      return -1;
    }

  /* Список всех драйверов. */
  path = argv[1];
  names = hyscan_driver_list (path);

  /* Проверяем работу на драйверах - заглушках (dummy*). */
  for (i = 0; names != NULL && names[i] != NULL; i++)
    {
      HyScanDriver *driver;
      HyScanDataSchema *info;

      GVariant *value;
      gint64 dummy1;
      gint64 dummy2;

      if (!g_str_has_prefix (names[i], "dummy"))
        continue;

      /* Информация о драйвере. */
      info = hyscan_driver_get_info (path, names[i]);
      if (info == NULL)
        g_error ("can't get %s driver info", names[i]);

      /* Считываем метку тестового драйвера. */
      value = hyscan_data_schema_key_get_default (info, "/dummy");
      if (value == NULL)
        g_error ("can't get %s driver mark", names[i]);

      dummy1 = g_ascii_strtoll (names[i] + 5, NULL, 10);
      dummy2 = g_variant_get_int64 (value);
      if (dummy1 != dummy2)
        g_error ("incorrect %s driver mark", names[i]);
      g_variant_unref (value);

      /* Загружаем драйвер. */
      driver = hyscan_driver_new (path, names[i]);
      if (driver == NULL)
        g_error ("can't load driver %s", names[i]);

      if (!HYSCAN_IS_DISCOVER (driver))
        g_error ("incorrect driver %s", names[i]);

      g_object_unref (driver);
      g_object_unref (info);
    }

  g_strfreev (names);
  xmlCleanupParser ();

  g_message ("All done");

  return 0;
}
