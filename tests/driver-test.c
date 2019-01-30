/* driver-test.c
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

#include <hyscan-driver.h>
#include <string.h>

int
main (int    argc,
      char **argv)
{
  const gchar *path;
  gchar **names;
  guint num_drv = 0;
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

  if (names == NULL)
    g_error ("can't get drivers list");

  /* Проверяем работу на драйверах - заглушках (dummy*). */
  for (i = 0; names[i] != NULL; i++)
    {
      HyScanDriver *driver;
      HyScanDataSchema *info;

      GVariant *value;
      gint64 dummy1;
      gint64 dummy2;

      if (!g_str_has_prefix (names[i], DUMMY_DRIVER_PREFIX))
        continue;

      g_message ("Loading driver %s", names[i]);

      /* Информация о драйвере. */
      info = hyscan_driver_get_info (path, names[i]);
      if (info == NULL)
        g_error ("can't get %s driver info", names[i]);

      /* Считываем метку тестового драйвера. */
      value = hyscan_data_schema_key_get_default (info, "/dummy");
      if (value == NULL)
        g_error ("can't get %s driver mark", names[i]);

      dummy1 = g_ascii_strtoll (names[i] + strlen (DUMMY_DRIVER_PREFIX), NULL, 10);
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

      num_drv += 1;
    }

  if (num_drv != DUMMY_DRIVER_NUMBER)
    g_error ("dummy drivers not found");

  g_strfreev (names);

  g_message ("All done");

  return 0;
}
