/* hyscan-driver.c
 *
 * Copyright 2016-2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-driver
 * @Short_description: класс загрузки драйверов устройств
 * @Title: HyScanDriver
 *
 * Класс предназначен для загрузки драйверов гидролокаторов и датчиков и
 * реализует интерфейс #HyScanDiscover.
 *
 * Драйвер должен размещаться в динамически загружаемой библиотеке, с именем
 * вида: hyscan-DRIVER_NAME.drv, где DRIVER_NAME - название драйвера
 * гидролокатора. В названии драйвера допускаются строчные и прописные буквы
 * латинского алфавита и цифры.
 *
 * Драйвер должен содержать экспортируемые функции с именами
 * "hyscan_driver_discover" и "hyscan_driver_info". При вызове функции
 * "hyscan_driver_discover" должен возвращаться указатель на объект,
 * реализующий интерфейс #HyScanDiscover. При вызове функции
 * "hyscan_driver_info" должен возвращаться указатель на объект
 * #HyScanDataSchema. Этот объект должен содержать схему параметров с
 * информацией о драйвере.
 *
 * После загрузки драйвера и подключения к гидролокатору или датчику можно
 * использовать интерфейсы #HyScanSonar и #HyScanSensor для работы с этими
 * устройствами.
 *
 * Загрузка драйвера производится при создании объекта функцией
 * #hyscan_driver_new. Если загрузка драйвера выполнена успешно, возврашается
 * указатель на новый объект, иначе возвращается NULL.
 *
 * Драйвер, загруженный с помощью функции #hyscan_driver_new, не выгружается
 * из памяти до окончания работы программы, даже если удалить объект.
 *
 * Информацию о драйвере можно узнать с помощью функции #hyscan_driver_get_info.
 * Информация возвращается в виде схемы данных со значениями по умолчанию.
 * Обязательными полями являются следующие:
 *
 * - /schema/id - идентификатор схемы (число, #HYSCAN_DRIVER_SCHEMA_ID);
 * - /schema/version - версия схемы (число, #HYSCAN_DRIVER_SCHEMA_VERSION);
 * - /info/name - название драйвера (строка);
 * - /info/description - описание драйвера (строка);
 * - /info/version - версия драйвера (строка);
 * - /info/id - идентификатор сборки (строка);
 *
 * Схема данных может содержать дополнительные поля.
 *
 * Функция #hyscan_driver_list возвращает список драйверов, доступных для
 * загрузки из указанного каталога.
 */

#include "hyscan-driver-schema.h"
#include "hyscan-driver.h"

#include <gmodule.h>
#include <string.h>

#define HYSCAN_DRIVER_NAME_PREFIX      "hyscan"
#define HYSCAN_DRIVER_NAME_EXTENSION   "drv"
#define HYSCAN_DRIVER_DISCOVER_SYMBOL  "hyscan_driver_discover"
#define HYSCAN_DRIVER_INFO_SYMBOL      "hyscan_driver_info"

enum
{
  PROP_O,
  PROP_PATH,
  PROP_NAME
};

typedef HyScanDiscover *(*hyscan_discover_func) (void);
typedef HyScanDataSchema *(*hyscan_info_func) (void);

struct _HyScanDriverPrivate
{
  gchar                       *path;           /* Путь к драйверу. */
  gchar                       *name;           /* Название драёвера. */
  HyScanDiscover              *discover;       /* Интерфейс HyScanDiscover */
};

static void                    hyscan_driver_interface_init      (HyScanDiscoverInterface *iface);
static void                    hyscan_driver_set_property        (GObject                 *object,
                                                                  guint                    prop_id,
                                                                  const GValue            *value,
                                                                  GParamSpec              *pspec);
static void                    hyscan_driver_object_constructed  (GObject                 *object);
static void                    hyscan_driver_object_finalize     (GObject                 *object);

static HyScanDiscover *        hyscan_driver_get_discover_int    (GModule                 *module);
static HyScanDataSchema *      hyscan_driver_get_info_int        (GModule                 *module);

static GModule *               hyscan_driver_load_driver         (const gchar             *path,
                                                                  const gchar             *name);

G_DEFINE_TYPE_WITH_CODE (HyScanDriver, hyscan_driver, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (HyScanDriver)
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_DISCOVER, hyscan_driver_interface_init))

static void
hyscan_driver_class_init (HyScanDriverClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_driver_set_property;

  object_class->constructed = hyscan_driver_object_constructed;
  object_class->finalize = hyscan_driver_object_finalize;

  g_object_class_install_property (object_class, PROP_PATH,
    g_param_spec_string ("path", "Path", "Path to device drivers", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_NAME,
    g_param_spec_string ("name", "Name", "Driver name", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_driver_init (HyScanDriver *driver)
{
  driver->priv = hyscan_driver_get_instance_private (driver);
}

static void
hyscan_driver_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  HyScanDriver *driver = HYSCAN_DRIVER (object);
  HyScanDriverPrivate *priv = driver->priv;

  switch (prop_id)
    {
    case PROP_PATH:
      priv->path = g_value_dup_string (value);
      break;

    case PROP_NAME:
      priv->name = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_driver_object_constructed (GObject *object)
{
  HyScanDriver *driver = HYSCAN_DRIVER (object);
  HyScanDriverPrivate *priv = driver->priv;
  GModule *module;

  /* Загрузка драйвера. */
  module = hyscan_driver_load_driver (priv->path, priv->name);

  /* Интерфейс HyScanDiscover. */
  priv->discover = hyscan_driver_get_discover_int (module);
  if (priv->discover == NULL)
    g_clear_pointer (&module, g_module_close);
}

static void
hyscan_driver_object_finalize (GObject *object)
{
  HyScanDriver *driver = HYSCAN_DRIVER (object);
  HyScanDriverPrivate *priv = driver->priv;

  g_free (priv->path);
  g_free (priv->name);

  g_clear_object (&priv->discover);

  G_OBJECT_CLASS (hyscan_driver_parent_class)->finalize (object);
}

/* Функция возвращает указатель на интерфейс HyScanDiscover. */
static HyScanDiscover *
hyscan_driver_get_discover_int (GModule *module)
{
  hyscan_discover_func discover_func;
  HyScanDiscover *discover = NULL;

  if (module == NULL)
    return NULL;

  if (g_module_symbol (module, HYSCAN_DRIVER_DISCOVER_SYMBOL, (gpointer *) &discover_func))
    {
      discover = discover_func ();

      if (!HYSCAN_IS_DISCOVER (discover))
        g_clear_object (&discover);
    }

  return discover;
}

/* Функция возвращает указатель на объект HyScanDataSchema. */
static HyScanDataSchema *
hyscan_driver_get_info_int (GModule *module)
{
  hyscan_info_func info_func;
  HyScanDataSchema *info = NULL;

  if (module == NULL)
    return NULL;

  if (!g_module_symbol (module, HYSCAN_DRIVER_INFO_SYMBOL, (gpointer *) &info_func))
    return NULL;

  info = info_func ();
  if (hyscan_driver_schema_check_id (info))
    return info;

  g_clear_object (&info);

  return NULL;
}

/* Функция загружает драйвер. */
static GModule *
hyscan_driver_load_driver (const gchar *path,
                           const gchar *name)
{
  gchar *module_name;
  gchar *module_path;

  GModule *module;

  gpointer discover = NULL;
  gpointer info = NULL;

  /* Путь к файлу драйвера. */
  module_name = g_strdup_printf ("%s-%s.%s",
                                 HYSCAN_DRIVER_NAME_PREFIX,
                                 name,
                                 HYSCAN_DRIVER_NAME_EXTENSION);
  module_path = g_build_filename (path, module_name, NULL);

  /* Загрузка драйвера. */
  module = g_module_open (module_path, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
  if (module != NULL)
    {
      g_module_symbol (module, HYSCAN_DRIVER_DISCOVER_SYMBOL, (gpointer *) &discover);
      g_module_symbol (module, HYSCAN_DRIVER_INFO_SYMBOL, (gpointer *) &info);
    }

  if ((discover == NULL) || (info == NULL))
    g_clear_pointer (&module, g_module_close);

  g_free (module_name);
  g_free (module_path);

  return module;
}

static void
hyscan_driver_discover_start (HyScanDiscover *discover)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover != NULL)
    hyscan_discover_start (driver->priv->discover);
}

static void
hyscan_driver_discover_stop (HyScanDiscover *discover)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover != NULL)
    hyscan_discover_stop (driver->priv->discover);
}

static GList *
hyscan_driver_discover_list (HyScanDiscover *discover)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover == NULL)
    return NULL;

  return hyscan_discover_list (driver->priv->discover);
}

static HyScanDataSchema *
hyscan_driver_discover_config (HyScanDiscover *discover,
                               const gchar    *uri)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover == NULL)
    return NULL;

  return hyscan_discover_config (driver->priv->discover, uri);
}

static gboolean
hyscan_driver_discover_check (HyScanDiscover  *discover,
                              const gchar     *uri,
                              HyScanParamList *params)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover == NULL)
    return FALSE;

  return hyscan_discover_check (driver->priv->discover, uri, params);
}

static HyScanDevice *
hyscan_driver_discover_connect (HyScanDiscover  *discover,
                                const gchar     *uri,
                                HyScanParamList *params)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover == NULL)
    return NULL;

  return hyscan_discover_connect (driver->priv->discover, uri, params);
}

/**
 * hyscan_driver_new:
 * @path: путь к каталогу с драйверами
 * @name: название драйвера
 *
 * Функция загружает драйвер из указанного каталога.
 *
 * Returns: #HyScanDriver. Для удаления #g_object_unref.
 */
HyScanDriver *
hyscan_driver_new (const gchar *path,
                   const gchar *name)
{
  HyScanDriver *driver;

  driver = g_object_new (HYSCAN_TYPE_DRIVER,
                         "path", path,
                         "name", name,
                         NULL);

  if (driver->priv->discover == NULL)
    g_clear_object (&driver);

  return driver;
}

/**
 * hyscan_driver_get_info:
 * @path: путь к каталогу с драйверами
 * @name: название драйвера
 *
 * Функция возвращает информацию о драйвере.
 *
 * Returns: #HyScanDataSchema. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_driver_get_info (const gchar *path,
                        const gchar *name)
{
  GModule *module;
  HyScanDataSchema *info;

  /* Загружаем драйвер. */
  module = hyscan_driver_load_driver (path, name);
  info = hyscan_driver_get_info_int (module);
  g_clear_pointer (&module, g_module_close);

  return info;
}

/**
 * hyscan_driver_list:
 * @path: путь к каталогу с драйверами
 *
 * Функция возвращает список названий драйверов доступных для загрузки из
 * указанного каталога.
 *
 * Returns (transfer full): список названий драйверов. Для удаления #g_strfreev.
 */
gchar **
hyscan_driver_list (const gchar *path)
{
  GDir *dir;
  GArray *names;
  GError *error = NULL;

  gchar *pattern;
  gsize prefix_len;
  gsize suffix_len;
  const gchar *name;

  /* Каталог с драйверами. */
  if ((dir = g_dir_open (path, 0, &error)) == NULL)
    {
      g_warning ("HyScanDriver: %s", error->message);
      g_error_free (error);
      return NULL;
    }

  /* Список драйверов. */
  names = g_array_new (TRUE, TRUE, sizeof (gchar*));

  /* Шаблон имени драйвера. */
  pattern = g_strdup_printf ("^%s-[0-9A-Za-z]+\\.%s$",
                             HYSCAN_DRIVER_NAME_PREFIX,
                             HYSCAN_DRIVER_NAME_EXTENSION);

  /* Длина префикса имени драйвера, до названия драйвера. */
  prefix_len = strlen (HYSCAN_DRIVER_NAME_PREFIX) + 1;

  /* Длина окончания имени драйвера, после названия драйвера. */
  suffix_len = strlen (HYSCAN_DRIVER_NAME_EXTENSION) + 1;

  /* Поиск драйверов в каталоге. */
  while ((name = g_dir_read_name (dir)) != NULL)
    {
      gchar *driver_name;
      GModule *module;
      HyScanDataSchema *info;

      /* Проверка имени файла по шаблону имени драйвера. */
      if (!g_regex_match_simple (pattern, name, 0, 0))
        continue;

      /* Название драйвера. */
      driver_name = g_utf8_substring (name, prefix_len, strlen (name) - suffix_len);

      /* Загружаем драйвер. */
      module = hyscan_driver_load_driver (path, driver_name);
      info = hyscan_driver_get_info_int (module);

      /* Проверка валидности драйвера. */
      if (info == NULL)
        g_clear_pointer (&driver_name, g_free);

      g_clear_pointer (&module, g_module_close);

      g_clear_object (&info);

      if (driver_name != NULL)
        g_array_append_val (names, driver_name);
    }

  g_dir_close (dir);
  g_free (pattern);

  if (names->len == 0)
    {
      g_array_unref (names);
      return NULL;
    }

  return (gchar**)g_array_free (names, FALSE);
}

static void
hyscan_driver_interface_init (HyScanDiscoverInterface *iface)
{
  iface->start = hyscan_driver_discover_start;
  iface->stop = hyscan_driver_discover_stop;
  iface->list = hyscan_driver_discover_list;
  iface->config = hyscan_driver_discover_config;
  iface->check = hyscan_driver_discover_check;
  iface->connect = hyscan_driver_discover_connect;
}
