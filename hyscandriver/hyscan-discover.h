/**
 * \file hyscan-discover.h
 *
 * \brief Заголовочный файл интерфейса обнаружения гидролокаторов и датчиков
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanDiscover HyScanDiscover - интерфейс обнаружения гидролокаторов и датчиков
 *
 * Интерфейс предназначен для обнаружения гидролокаторов и датчиков подключенных
 * к компьютеру. Реализация данного интерфейса зависит от устройства и должна
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
 * При подключении к устройству, можно дополнительно передать параметры драйвера.
 * Список параметров драйвера можно узнать с помощью функции #hyscan_discover_config.
 *
 * Для подключения к устройству используется функция #hyscan_discover_connect.
 * После подключения возвращается указатель на объект реализующий интерфейсы
 * \link HyScanParam \endlink, \link HyScanSonar \endlink и \link HyScanSensor \endlink.
 * Если произведено подключение к гидролокатору, который не содержит встроенных датчиков
 * интерфейс \link HyScanSensor \endlink может быть не реализован. Аналогично,
 * при подключении к датчику не реализуется интерфейс \link HyScanSonar \endlink.
 *
 * В процесс обнаружения устройств посылаются сигналы: "discover-progress" и
 * "discover-completed". Сигнал "discover-progress" периодически посылается
 * во время процесса обнаружения и несёт информацию о его прогрессе. Прототип
 * обработчика сигнала:
 *
 * \code
 *
 * void    discover_progress  (HyScanDiscover       *discover,
 *                             gdouble               progress,
 *                             gpointer              user_data);
 *
 * \endcode
 *
 * Где progress - прогресс поиска в процентах (0 - 100).
 *
 * Сигнал "discover-completed" посылается при завершении процесса поиска.
 * Прототип обработчика сигнала:
 *
 * \code
 *
 * void    discover_completed (HyScanDiscover       *discover,
 *                             gpointer              user_data);
 *
 * \endcode
 *
 */

#ifndef __HYSCAN_DISCOVER_H__
#define __HYSCAN_DISCOVER_H__

#include <hyscan-param-list.h>
#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_DISCOVER_API    20170100        /**< Версия API интерфейса. */

/** \brief Общая информация о гидролокаторе или датчике. */
typedef struct
{
  const gchar                 *model;          /**< Модель гидролокатора или датчика. */
  const gchar                 *uri;            /**< Путь для подключения. */
} HyScanDiscoverInfo;

typedef GObject HyScanDiscoverConnect;

#define HYSCAN_TYPE_DISCOVER            (hyscan_discover_get_type ())
#define HYSCAN_DISCOVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DISCOVER, HyScanDiscover))
#define HYSCAN_IS_DISCOVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DISCOVER))
#define HYSCAN_DISCOVER_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_DISCOVER, HyScanDiscoverInterface))

typedef struct _HyScanDiscover HyScanDiscover;
typedef struct _HyScanDiscoverInterface HyScanDiscoverInterface;

struct _HyScanDiscoverInterface
{
  GTypeInterface               g_iface;

  void                         (*start)                        (HyScanDiscover                *discover);

  void                         (*stop)                         (HyScanDiscover                *discover);

  HyScanDiscoverInfo **        (*list)                         (HyScanDiscover                *discover,
                                                                guint32                       *n_devices);

  HyScanDataSchema *           (*config)                       (HyScanDiscover                *discover,
                                                                const gchar                   *uri);

  HyScanDiscoverConnect *      (*connect)                      (HyScanDiscover                *discover,
                                                                const gchar                   *uri,
                                                                HyScanParamList               *params);
};

HYSCAN_API
GType                          hyscan_discover_get_type        (void);

/**
 *
 * Функция инициирует процесс обнаружения гидролокаторов и датчиков.
 *
 * \param discover указатель на интерфейс \link HyScanDiscover \endlink.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                           hyscan_discover_start           (HyScanDiscover                *discover);

/**
 *
 * Функция останавливает процесс обнаружения гидролокаторов и датчиков.
 *
 * \param discover указатель на интерфейс \link HyScanDiscover \endlink.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                           hyscan_discover_stop            (HyScanDiscover                *discover);

/**
 *
 * Функция возвращает список обнаруженных устройств с общей информацией о них.
 * Память выделенная под список должна быть освобождена после использования
 * функцией #hyscan_discover_free_info.
 *
 * \param discover указатель на интерфейс \link HyScanDiscover \endlink;
 * \param n_devices число обнаруженных устройств.
 *
 * \return Список устройств или NULL.
 *
 */
HYSCAN_API
HyScanDiscoverInfo **          hyscan_discover_list            (HyScanDiscover                *discover,
                                                                guint32                       *n_devices);

/**
 *
 * Функция возвращает схему с параметрами драйвера устройства. Эти параметры
 * можно передать в функцию #hyscan_discover_connect.
 *
 * Если драйвер не содержит настраиваемых параметров, функция вернёт NULL.
 *
 * \param discover указатель на интерфейс \link HyScanDiscover \endlink;
 * \param uri путь для подключения к устройству.
 *
 * \return Указатель на \link HyScanDataSchema \endlink или NULL.
 *
 */
HYSCAN_API
HyScanDataSchema *             hyscan_discover_config          (HyScanDiscover                *discover,
                                                                const gchar                   *uri);

/**
 *
 * Функция производит подключение к гидролокатору или датчику.
 *
 * \param discover указатель на интерфейс \link HyScanDiscover \endlink;
 * \param uri путь для подключения;
 * \param params параметры драйвера или NULL.
 *
 * \return Указатель на подключение или NULL.
 *
 */
HYSCAN_API
HyScanDiscoverConnect *        hyscan_discover_connect         (HyScanDiscover                *discover,
                                                                const gchar                   *uri,
                                                                HyScanParamList               *params);

/**
 *
 * Функция освобождает память занятую списоком устройств.
 *
 * \param list список устройств \link HyScanDiscoverInfo \endlink.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                           hyscan_discover_free_info       (HyScanDiscoverInfo           **list);

G_END_DECLS

#endif /* __HYSCAN_DISCOVER_H__ */
