/*
 * \file hyscan-driver.c
 *
 * \brief Исходный файл класса загрузки драйверов гидролокаторов и датчиков
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-driver.h"

#include <gmodule.h>
#include <string.h>

#define HYSCAN_DRIVER_NAME_PREFIX      "hyscan"
#define HYSCAN_DRIVER_NAME_SUFFIX      "drv"
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
    goto fail;

  if (g_module_symbol (module, HYSCAN_DRIVER_INFO_SYMBOL, (gpointer *) &info_func))
    {
      GVariant *value = NULL;
      gint64 api_version = 0;
      gint64 schema_version = 0;
      gint64 schema_id = 0;

      /* Проверка типа. */
      info = info_func ();
      if (!HYSCAN_IS_DATA_SCHEMA (info))
        goto fail;

      /* Идентификатор и версия схемы. */
      if ((value = hyscan_data_schema_key_get_default (info, "/schema/id")) != NULL)
        {
          schema_id = g_variant_get_int64 (value);
          g_variant_unref (value);
        }

      if ((value = hyscan_data_schema_key_get_default (info, "/schema/version")) != NULL)
        {
          schema_version = g_variant_get_int64 (value);
          g_variant_unref (value);
        }

      if ((schema_id != HYSCAN_DRIVER_SCHEMA_ID) || (schema_version != HYSCAN_DRIVER_SCHEMA_VERSION))
        {
          g_warning ("HyScanDriver: %s: incorrect driver schema", g_module_name (module));
          goto fail;
        }

      /* Версия API. */
      if ((value = hyscan_data_schema_key_get_default (info, "/api/version")) != NULL)
        {
          api_version = g_variant_get_int64 (value);
          g_variant_unref (value);
        }

      if (api_version != HYSCAN_DISCOVER_API)
        {
          g_warning ("HyScanDriver: %s: incorrect api version", g_module_name (module));
          goto fail;
        }

      return info;
    }

fail:
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
  module_name = g_strdup_printf ("%s-%s-%s.%s",
                                 HYSCAN_DRIVER_NAME_PREFIX,
                                 name,
                                 HYSCAN_DRIVER_NAME_SUFFIX,
                                 G_MODULE_SUFFIX);
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

static HyScanDiscoverInfo **
hyscan_driver_discover_list (HyScanDiscover *discover,
                             guint32        *n_devices)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover == NULL)
    return NULL;

  return hyscan_discover_list (driver->priv->discover, n_devices);
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

static HyScanDiscoverConnect *
hyscan_driver_discover_connect (HyScanDiscover  *discover,
                                const gchar     *uri,
                                HyScanParamList *params)
{
  HyScanDriver *driver = HYSCAN_DRIVER (discover);

  if (driver->priv->discover == NULL)
    return NULL;

  return hyscan_discover_connect (driver->priv->discover, uri, params);
}

/* Функция загружает драйвер из указанного каталога. */
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

/* Функция возвращает информацию о загруженном драйвере. */
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
  names = g_array_sized_new (TRUE, TRUE, sizeof (gpointer), 16);

  /* Шаблон имени драйвера. */
  pattern = g_strdup_printf ("^%s-[0-9A-Za-z]+-%s\\.%s$",
                             HYSCAN_DRIVER_NAME_PREFIX,
                             HYSCAN_DRIVER_NAME_SUFFIX,
                             G_MODULE_SUFFIX);

  /* Длина префикса имени драйвера, до названия драйвера. */
  prefix_len = strlen (HYSCAN_DRIVER_NAME_PREFIX) + 1;

  /* Длина окончания имени драйвера, после названия драйвера. */
  suffix_len = strlen (HYSCAN_DRIVER_NAME_SUFFIX) + 1 + strlen (G_MODULE_SUFFIX) + 1;

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

  return (gchar**)g_array_free (names, (names->len == 0));
}

static void
hyscan_driver_interface_init (HyScanDiscoverInterface *iface)
{
  iface->start = hyscan_driver_discover_start;
  iface->stop = hyscan_driver_discover_stop;
  iface->list = hyscan_driver_discover_list;
  iface->config = hyscan_driver_discover_config;
  iface->connect = hyscan_driver_discover_connect;
}
