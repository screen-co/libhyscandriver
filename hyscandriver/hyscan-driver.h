/**
 * \file hyscan-driver.h
 *
 * \brief Заголовочный файл класса загрузки драйверов гидролокаторов и датчиков
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanDriver HyScanDriver - класс загрузки драйверов гидролокаторов и датчиков
 *
 * Класс предназначен для загрузки драйверов гидролокаторов и датчиков и реализует
 * интерфейс \link HyScanDiscover \endlink.
 *
 * Загрузка драйвера производится при создании объекта функцией #hyscan_driver_new.
 * Если загрузка драйвера выполнена успешно, возврашается указатель на новый объект.
 * Иначе возвращается NULL.
 *
 * Драйвер, загруженный с помощью функции #hyscan_driver_new, не выгружается из памяти
 * до окончания работы программы, даже если удалить объект HyScanDriver.
 *
 * Информацию о загруженном драйвере можно узнать с помощью функции #hyscan_driver_get_info.
 * Информация возвращается в виде схемы данных со значениями по умолчанию. Обязательными
 * полями являются следующие:
 *
 * - /schema/id - идентификатор схемы (число, #HYSCAN_DRIVER_SCHEMA_ID);
 * - /schema/version - версия схемы (число, #HYSCAN_DRIVER_SCHEMA_VERSION);
 * - /info/name - название драйвера (строка);
 * - /info/description - описание драйвера (строка);
 * - /info/version - версия драйвера (строка);
 * - /info/id - идентификатор сборки (строка);
 * - /api/version - версия API интерфейса HyScanDiscover (число, \link HYSCAN_DISCOVER_API \endlink).
 *
 * Схема данных может содержать дополнительные поля.
 *
 * Функция #hyscan_driver_list возвращает список драйверов, доступных для загрузки из
 * указанного каталога.
 *
 */

#ifndef __HYSCAN_DRIVER_H__
#define __HYSCAN_DRIVER_H__

#include <hyscan-discover.h>
#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_DRIVER_SCHEMA_ID        6327345620348565983     /**< Идентификатор схемы. */
#define HYSCAN_DRIVER_SCHEMA_VERSION   20170100                /**< Версия схемы. */

#define HYSCAN_TYPE_DRIVER             (hyscan_driver_get_type ())
#define HYSCAN_DRIVER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DRIVER, HyScanDriver))
#define HYSCAN_IS_DRIVER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DRIVER))
#define HYSCAN_DRIVER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DRIVER, HyScanDriverClass))
#define HYSCAN_IS_DRIVER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DRIVER))
#define HYSCAN_DRIVER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DRIVER, HyScanDriverClass))

typedef struct _HyScanDriver HyScanDriver;
typedef struct _HyScanDriverPrivate HyScanDriverPrivate;
typedef struct _HyScanDriverClass HyScanDriverClass;

struct _HyScanDriver
{
  GObject parent_instance;

  HyScanDriverPrivate *priv;
};

struct _HyScanDriverClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_driver_get_type          (void);

/**
 *
 * Функция загружает драйвер из указанного каталога.
 *
 * \param path путь к каталогу с драйверами;
 * \param name имя драйвера.
 *
 * \return Указатель на \link HyScanDriver \endlink или NULL.
 *
 */
HYSCAN_API
HyScanDriver *         hyscan_driver_new               (const gchar           *path,
                                                        const gchar           *name);

/**
 *
 * Функция возвращает информацию о загруженном драйвере.
 *
 * \param path путь к каталогу с драйверами;
 * \param name имя драйвера.
 *
 * \return Информация о драйвере.
 *
 */
HYSCAN_API
HyScanDataSchema *     hyscan_driver_get_info          (const gchar           *path,
                                                        const gchar           *name);

/**
 *
 * Функция возвращает список драйверов доступных для загрузки из указанного
 * каталога. После использования память, используемая списком должна быть
 * освобождена с помощью функции g_strfreev.
 *
 * \param path путь к каталогу с драйверами.
 *
 * \return Список драйверов.
 *
 */
HYSCAN_API
gchar **               hyscan_driver_list              (const gchar           *path);

G_END_DECLS

#endif /* __HYSCAN_DRIVER_H__ */
