/**
 * \file hyscan-sonar.h
 *
 * \brief Заголовочный файл интерфейса управления гидролокатором
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2017
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanSonar HyScanSonar - интерфейс управления гидролокатором
 *
 * Гидролокаторы имеют в своём составе несколько подсистем, каждая из которых
 * решает определённую задачу. Конфигурация и характеристики подсистем могут
 * различаться. Они зависят от конкретной модели гидролокатора и его текущей
 * аппаратной настройки.
 *
 * Можно выделить следующие подсистемы гидролокатора:
 * - управление излучением;
 * - генератор излучаемого сигнала;
 * - регулировка усиления и дискретизации эхосигнала;
 *
 * Управление излучением в штатном режиме осуществляется с использованием внутреннего
 * таймера гидролокатора или внешней команды. Для некоторых гидролокаторов существует
 * возможность программного управления синхронизацией излучения. Выбрать режим
 * синхонизации излучения можно с помощью функции #hyscan_sonar_set_software_ping.
 *
 * В зависимости от типа, гидролокатор может иметь различное число источников
 * данных. Каждый из источников гидролокационных данных связан с одним из «бортов»
 * гидролокатора.
 *
 * Рабочая дистанция для каждого источника данных определяется временем приёма
 * эхосигнала. Управление временем приёма осуществляется индивидуально для каждого
 * источника данных. Для этого используются функции #hyscan_sonar_receiver_set_time и
 * #hyscan_sonar_receiver_set_auto. Функция #hyscan_sonar_receiver_set_time
 * устанавливает фиксированное время приёма эхосигнала, функция
 * #hyscan_sonar_receiver_set_auto включает автоматическую настройку времени приёма.
 * Использовать функцию #hyscan_sonar_receiver_set_auto можно только если такая
 * возможность предусмотрена гидролокатором.
 *
 * Включить или выключить приём эхосигнала можно с помощью функции #hyscan_sonar_receiver_set_enable.
 *
 * С каждым источником гидролокационных данных связан свой генератор излучаемого
 * сигнала. Управление генератором может осуществляться в одном из доступных режимов.
 * Режим работы генератора может задаваться независимо для каждого источника данных.
 *
 * Возможны следующие режимы работы генератора:
 *
 * - #HYSCAN_SONAR_GENERATOR_MODE_PRESET - преднастройки;
 * - #HYSCAN_SONAR_GENERATOR_MODE_AUTO - автоматический;
 * - #HYSCAN_SONAR_GENERATOR_MODE_SIMPLE - упрощённый;
 * - #HYSCAN_SONAR_GENERATOR_MODE_EXTENDED - расширенный.
 *
 * В режиме преднастроек пользователь может выбрать один из предварительно
 * настроенных сигналов. Установка режима работы по преднастройкам осуществляется
 * функцией #hyscan_sonar_generator_set_preset. В остальных режимах пользователь
 * может выбрать один из доступных сигналов и определить его параметры с разной
 * степенью детализации.
 *
 * Возможны следующие типы сигналов:
 *
 * - #HYSCAN_SONAR_GENERATOR_SIGNAL_AUTO - автоматический выбор сигнала;
 * - #HYSCAN_SONAR_GENERATOR_SIGNAL_TONE - тональный сигнал;
 * - #HYSCAN_SONAR_GENERATOR_SIGNAL_LFM - линейно-частотно модулированный сигнал;
 * - #HYSCAN_SONAR_GENERATOR_SIGNAL_LFMD - линейно-частотно модулированный сигнал с уменьшением частоты.
 *
 * Выбор режима работы генератора осуществляется с помощью функций
 * #hyscan_sonar_generator_set_auto, #hyscan_sonar_generator_set_simple и
 * #hyscan_sonar_generator_set_extended.
 *
 * Включить или выключить излучение сигнала можно с помощью функции #hyscan_sonar_generator_set_enable.
 *
 * С каждым источником гидролокационных данных связана своя подсистема регулировки
 * усиления - ВАРУ. Управление усилением может осуществляться в одном из доступных
 * режимов. Режим работы усиления может задаваться независимо для каждого источника данных.
 *
 * Возможны следующие режимы управления усилением:
 *
 * - #HYSCAN_SONAR_TVG_MODE_AUTO - автоматический;
 * - #HYSCAN_SONAR_TVG_MODE_POINTS - по заданым точкам;
 * - #HYSCAN_SONAR_TVG_MODE_LINEAR_DB - линейное изменение усиления в дБ;
 * - #HYSCAN_SONAR_TVG_MODE_LOGARITHMIC - управление усилением по логарифмическому закону.
 *
 * При выборе автоматического режима работы, система ВАРУ будет самостоятельно
 * управлять усилением приёмных каналов. Включение автоматического режима осуществляется
 * функцией #hyscan_sonar_tvg_set_auto.
 *
 * Функция #hyscan_sonar_tvg_set_linear_db активирует режим ВАРУ, при котором
 * коэффициент усиления, в дБ, линейно изменяется на указанную величину каждые
 * 100 метров.
 *
 * Управление усилением по логарифмическому закону включается функцией
 * #hyscan_sonar_tvg_set_logarithmic. При этом используется следующая формула при
 * расчёте коэффициента усиления:
 *
 * K = gain0 + beta * log (r) + alpha * r,
 *
 * где:
 *
 * - gain0 - начальный коэффициент усиления, дБ;
 * - beta - коэффициент отражения цели, дБ;
 * - aplha - коэффициент затухания, дБ/м;
 * - r - расстояние, м.
 *
 * Включить или выключить управление усилением можно с помощью функции
 * #hyscan_sonar_tvg_set_enable. В выключенном состоянии устанавливается минимально
 * возможное усиление.
 *
 * Включить гидролокатор в рабочий режим, в соответствии с установленными
 * параметрами, можно при помощи функции #hyscan_sonar_start, остановить
 * #hyscan_sonar_stop.
 *
 * Любые параметры гидролокатора, определяющие его работу можно менять как в
 * рабочем состоянии, так и в режиме ожидания. Однако, если параметры изменяются
 * в рабочем режиме, необходимо сигнализировать об этом гидролокатору с помощью
 * функции #hyscan_sonar_sync. Это необходимо для атомарного применения нескольких
 * изменившихся параметров, например для одновременного изменения времени приёма для
 * левого и правого борта и т.п.
 *
 * Функция #hyscan_sonar_ping используется для программного управления излучением.
 *
 * При изменении излучаемого сигнала, интерфейс посылает сигнал "sonar-signal", в
 * котором передаёт новый образ сигнала для свёртки. Прототип обработчика сигнала:
 *
 * \code
 *
 * void    sonar_signal_cb   (HyScanSonar          *sonar,
 *                            HyScanSourceType      source,
 *                            gint64                time,
 *                            guint32               n_points,
 *                            HyScanComplexFloat   *points,
 *                            gpointer              user_data);
 *
 * \endcode
 *
 * Где:
 * - source - идентификатор источника данных;
 * - time - время начала действия сигнала, мкс;
 * - n_points - число точек образа сигнала;
 * - points - образ сигнала.
 *
 * При изменении параметров ВАРУ, класс посылает сигнал "sonar-tvg", в котором
 * передаёт новые коэффициенты усиления. Прототип обработчика сигнала:
 *
 * \code
 *
 * void    sonar_tvg_cb      (HyScanSonar          *sonar,
 *                            HyScanSourceType      source,
 *                            gint64                time,
 *                            guint32               n_gains,
 *                            gfloat               *gains,
 *                            gpointer              user_data);
 *
 * \endcode
 *
 * Где:
 * - source - идентификатор источника данных;
 * - time - время начала действия параметров ВАРУ, мкс;
 * - n_gains - число коэффициентов усиления;
 * - gains - коэффициенты усиления.
 *
 * При получении сырых данных от гидролокатора класс посылает сигнал "sonar-raw-data".
 * Если при приёме данных было отключено излучение посылается сигнал "sonar-noise-data".
 * Прототип обработчика для этих сигналов:
 *
 * \code
 *
 * void    sonar_data_cb     (HyScanSonar          *sonar,
 *                            HyScanSourceType      source,
 *                            guint                 channel,
 *                            gint64                time,
 *                            HyScanDataType        type,
 *                            guint32               size,
 *                            gpointer              data,
 *                            gpointer              user_data);
 *
 * \endcode
 *
 * Где:
 * - source - идентификатор источника данных;
 * - channel - индекс канала данных;
 * - time - время приёма данных, мкс;
 * - type - тип данных;
 * - size - размер данных в байтах;
 * - data - данные.
 *
 * При получении обработанных акустических данных от гидролокатора, класс посылает
 * сигнал "acoustic-data", в котором передаёт их пользователю. Прототип обработчика
 * сигнала:
 *
 * \code
 *
 * void    sonar_data_cb     (HyScanSonar          *sonar,
 *                            HyScanSourceType      source,
 *                            gint64                time,
 *                            HyScanDataType        type,
 *                            guint32               size,
 *                            gpointer              data,
 *                            gpointer              user_data);
 *
 * \endcode
 *
 * Где:
 * - source - идентификатор источника данных;
 * - time - время приёма данных, мкс;
 * - type - тип данных;
 * - size - размер данных в байтах;
 * - data - данные.
 *
 * В процессе работы драйвер может отправлять различные информационные и
 * диагностические сообщения. При получении такого сообщения интерфейс
 * посылает сигнал "sonar-log", в котором передаёт их пользователю.
 * Прототип обработчика сигнала:
 *
 * \code
 *
 * void    sonar_log_cb      (HyScanSonar          *sonar,
 *                            HyScanSourceType      source,
 *                            gint64                time,
 *                            HyScanLogLevel        level,
 *                            const gchar          *message
 *                            gpointer              user_data);
 *
 * \endcode
 *
 * Где:
 * - source - идентификатор источника данных;
 * - time - время приёма сообщения, мкс;
 * - level - тип сообщения;
 * - message - сообщение (NULL терминированная строка).
 *
 * Принятое сообщение имеет отнощение к тому источнику данных, идентификатор которого
 * указан в source. Если source = 0, сообщение относится к драйверу в целом.
 *
 */

#ifndef __HYSCAN_SONAR_H__
#define __HYSCAN_SONAR_H__

#include <hyscan-types.h>
#include <hyscan-data-schema.h>

G_BEGIN_DECLS

/** \brief Режимы работы генератора */
typedef enum
{
  HYSCAN_SONAR_GENERATOR_MODE_INVALID          = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_GENERATOR_MODE_PRESET           = (1 << 0),     /**< Режим работы по преднастройкам. */
  HYSCAN_SONAR_GENERATOR_MODE_AUTO             = (1 << 1),     /**< Автоматический режим работы. */
  HYSCAN_SONAR_GENERATOR_MODE_SIMPLE           = (1 << 2),     /**< Упрощённый режим работы. */
  HYSCAN_SONAR_GENERATOR_MODE_EXTENDED         = (1 << 3)      /**< Режим работы по установленным параметрам. */
} HyScanSonarGeneratorModeType;

/** \brief Типы сигналов */
typedef enum
{
  HYSCAN_SONAR_GENERATOR_SIGNAL_INVALID        = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_GENERATOR_SIGNAL_AUTO           = (1 << 0),     /**< Автоматический выбор типа сигнала. */
  HYSCAN_SONAR_GENERATOR_SIGNAL_TONE           = (1 << 1),     /**< Тональный сигнал. */
  HYSCAN_SONAR_GENERATOR_SIGNAL_LFM            = (1 << 2),     /**< Линейно-частотно модулированный сигнал. */
  HYSCAN_SONAR_GENERATOR_SIGNAL_LFMD           = (1 << 3)      /**< Линейно-частотно модулированный сигнал с уменьшением частоты. */
} HyScanSonarGeneratorSignalType;

/** \brief Режимы работы системы ВАРУ */
typedef enum
{
  HYSCAN_SONAR_TVG_MODE_INVALID                = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_TVG_MODE_AUTO                   = (1 << 0),     /**< Автоматический режим работы. */
  HYSCAN_SONAR_TVG_MODE_POINTS                 = (1 << 1),     /**< Усиление заданное точками. */
  HYSCAN_SONAR_TVG_MODE_LINEAR_DB              = (1 << 2),     /**< Линейное увеличение усиления в дБ / 100 метров. */
  HYSCAN_SONAR_TVG_MODE_LOGARITHMIC            = (1 << 3)      /**< Управление усилением по логарифмическому закону. */
} HyScanSonarTVGModeType;

/** \brief Типы синхронизации излучения */
typedef enum
{
  HYSCAN_SONAR_SYNC_INVALID                    = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_SYNC_AUTO                       = 101,          /**< Автоматическая синхронизация (выбирается драйвером). */
  HYSCAN_SONAR_SYNC_SOFTWARE                   = 102           /**< Программная синхронизация. */
} HyScanSonarSyncType;

#define HYSCAN_TYPE_SONAR            (hyscan_sonar_get_type ())
#define HYSCAN_SONAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SONAR, HyScanSonar))
#define HYSCAN_IS_SONAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SONAR))
#define HYSCAN_SONAR_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_SONAR, HyScanSonarInterface))

typedef struct _HyScanSonar HyScanSonar;
typedef struct _HyScanSonarInterface HyScanSonarInterface;

struct _HyScanSonarInterface
{
  GTypeInterface                         g_iface;

  gboolean                               (*set_sync_type)                      (HyScanSonar                    *sonar,
                                                                                HyScanSonarSyncType             sync_type);

  gboolean                               (*receiver_set_time)                  (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         receive_time);

  gboolean                               (*receiver_set_auto)                  (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source);

  gboolean                               (*receiver_set_enable)                (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gboolean                        enable);

  gboolean                               (*generator_set_preset)               (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gint64                          preset);

  gboolean                               (*generator_set_auto)                 (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                HyScanSonarGeneratorSignalType  signal);

  gboolean                               (*generator_set_simple)               (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                HyScanSonarGeneratorSignalType  signal,
                                                                                gdouble                         power);

  gboolean                               (*generator_set_extended)             (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                HyScanSonarGeneratorSignalType  signal,
                                                                                gdouble                         duration,
                                                                                gdouble                         power);

  gboolean                               (*generator_set_enable)               (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gboolean                        enable);

  gboolean                               (*tvg_set_auto)                       (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         level,
                                                                                gdouble                         sensitivity);

  gboolean                               (*tvg_set_points)                     (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         time_step,
                                                                                gdouble                        *gains,
                                                                                guint32                         n_gains);

  gboolean                               (*tvg_set_linear_db)                  (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         gain0,
                                                                                gdouble                         gain_step);

  gboolean                               (*tvg_set_logarithmic)                (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         gain0,
                                                                                gdouble                         beta,
                                                                                gdouble                         alpha);

  gboolean                               (*tvg_set_enable)                     (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gboolean                        enable);

  gboolean                               (*start)                              (HyScanSonar                    *sonar,
                                                                                const gchar                    *track_name,
                                                                                HyScanTrackType                 track_type);

  gboolean                               (*stop)                               (HyScanSonar                    *sonar);

  gboolean                               (*sync)                               (HyScanSonar                    *sonar);

  gboolean                               (*ping)                               (HyScanSonar                    *sonar);
};

HYSCAN_API
GType                                    hyscan_sonar_get_type                 (void);

/**
 *
 * Функция задаёт время приёма эхосигнала.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param receive_time время приёма эхосигнала, секунды.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_receiver_set_time        (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         receive_time);

/**
 *
 * Функция задаёт автоматический выбор времени приёма эхосигнала.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_receiver_set_auto        (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source);

/**
 *
 * Функция включает или выключает приём эхосигнала.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param enable включён или выключен.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_receiver_set_enable      (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gboolean                        enable);



/**
 *
 * Функция включает преднастроенный режим работы генератора.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param preset идентификатор преднастройки.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_generator_set_preset     (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gint64                          preset);

/**
 *
 * Функция включает автоматический режим работы генератора.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param signal тип сигнала.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_generator_set_auto       (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                HyScanSonarGeneratorSignalType  signal);

/**
 *
 * Функция включает упрощённый режим работы генератора.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param signal тип сигнала;
 * \param power энергия сигнала, проценты.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_generator_set_simple     (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                HyScanSonarGeneratorSignalType  signal,
                                                                                gdouble                         power);

/**
 *
 * Функция включает расширенный режим работы генератора.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param signal тип сигнала;
 * \param duration длительность сигнала, с;
 * \param power энергия сигнала, проценты.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_generator_set_extended   (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                HyScanSonarGeneratorSignalType  signal,
                                                                                gdouble                         duration,
                                                                                gdouble                         power);

/**
 *
 * Функция включает или выключает формирование сигнала генератором.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param enable включён или выключен.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_generator_set_enable     (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gboolean                        enable);

/**
 *
 * Функция включает автоматический режим управления системой ВАРУ.
 *
 * Если в качестве значений параметров уровня сигнала и (или) чувствительности
 * передать отрицательное число, будут установлены значения по умолчанию.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param level целевой уровень сигнала;
 * \param sensitivity чувствительность автомата регулировки.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_tvg_set_auto             (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         level,
                                                                                gdouble                         sensitivity);

/**
 *
 * Функция устанавливает усиление определённое контрольными точками,
 * заданными на равномерной временной оси.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param time_step интервал времени между точками, с;
 * \param gains массив коэффициентов усиления, дБ;
 * \param n_gains число коэффициентов усиления.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_tvg_set_points           (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         time_step,
                                                                                gdouble                        *gains,
                                                                                guint32                         n_gains);

/**
 *
 * Функция устанавливает линейное увеличение усиления в дБ на 100 метров.
 *
 * Начальный уровень усиления должен находится в пределах от -20 дБ, до максимально
 * возможного усиления. Величина изменения усиления должна находится в пределах от 0
 * до 40 дБ.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param gain0 начальный уровень усиления, дБ;
 * \param gain_step величина изменения усиления каждые 100 метров, дБ.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_tvg_set_linear_db        (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         gain0,
                                                                                gdouble                         gain_step);

/**
 *
 * Функция устанавливает логарифмический вид закона усиления системой ВАРУ.
 *
 * Начальный уровень усиления должен находится в пределах от -20 дБ, до максимально
 * возможного усиления. Значение коэффициента поглощения цели должно находится в
 * пределах от 0 дБ до 40 дБ. Значение коэффициента затухания должно находится в
 * пределах от 0 дБ/м до 1 дБ/м.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param gain0 начальный уровень усиления, дБ;
 * \param beta коэффициент поглощения цели, дБ;
 * \param alpha коэффициент затухания, дБ/м.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_tvg_set_logarithmic      (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gdouble                         gain0,
                                                                                gdouble                         beta,
                                                                                gdouble                         alpha);

/**
 *
 * Функция включает или выключает систему ВАРУ.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param source идентификатор источника данных;
 * \param enable включёна или выключена.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_tvg_set_enable           (HyScanSonar                    *sonar,
                                                                                HyScanSourceType                source,
                                                                                gboolean                        enable);
/**
 *
 * Функция устанавливает программное управление излучением. Данная функция должна
 * быть вызвана перед запуском гидролокатора с помощью функции #hyscan_sonar_start.
 *
 * \param sonar указатель на \link HyScanSonar \endlink.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_set_software_ping        (HyScanSonar                    *sonar);

/**
 *
 * Функция переводит гидролокатор в рабочий режим.
 *
 * \param sonar указатель на \link HyScanSonar \endlink;
 * \param track_name название галса, в который записывать данные;
 * \param track_type тип галса.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_start                    (HyScanSonar                    *sonar,
                                                                                const gchar                    *track_name,
                                                                                HyScanTrackType                 track_type);

/**
 *
 * Функция переводит гидролокатор в ждущий режим и отключает запись данных.
 *
 * \param sonar указатель на \link HyScanSonar \endlink.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_stop                     (HyScanSonar                    *sonar);

/**
 *
 * Функция синхронизирует состояние гидролокатора в соответствии с заданными
 * параметрами. Данная функция должна вызываться при изменении параметров во
 * время работы гидролокатора. При переводе гидролокатора в рабочий режим данная
 * функция вызывается автоматически.
 *
 * \param sonar указатель на \link HyScanSonar \endlink.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_sync                     (HyScanSonar                    *sonar);

/**
 *
 * Функция выполняет один цикл излучения сигнала и приёма данных. Для использования
 * этой функции данный тип синхронизации должен быть включён функцией
 * #hyscan_sonar_set_software_ping. Гидролокатор должен находится в рабочем режиме
 * (#hyscan_sonar_start).
 *
 * \param sonar указатель на \link HyScanSonar \endlink.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean                                 hyscan_sonar_ping                     (HyScanSonar                    *sonar);

G_END_DECLS

#endif /* __HYSCAN_SONAR_H__ */
