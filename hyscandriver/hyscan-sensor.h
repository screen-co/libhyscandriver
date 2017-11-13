/**
 * \file hyscan-sensor.h
 *
 * \brief Заголовочный файл интерфейса управления датчиками
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2017
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanSensor HyScanSensor - интерфейс управления датчиками
 *
 * Датчиком является любая система, предназначенная для дополнения
 * гидролокационных данных вспомогательной информацией. Это может
 * быть: навигационная информация, информация об ориентации в пространстве,
 * информация о скорости звука и т.п.
 *
 * Каждый из датчиков подключается к точке ввода информации, называемой
 * портом. К одному порту может быть подключен только один датчик.
 *
 * Перед началом работы рекомендуется задать профиль скорости звука. Для этого
 * используется функция #hyscan_sonar_set_sound_velocity. По умолчанию используется
 * фиксированное значение скорости звука, равное 1500 м/с.
 *
 * В системе могут использоваться несколько однотипных датчиков, например
 * два и более датчиков систем позиционирования ГЛОНАСС или GPS. Для того,
 * чтобы различать информацию от этих датчиков, имеется возможность добавить
 * метку к данным каждого из портов. Такой меткой является номер канала.
 * Для задания номера канала предназначена функция #hyscan_sensor_set_channel.
 *
 * Часто, данные от датчиков имеют определённую задержку во времени. Это
 * связанно с необходимостью обработки данных датчиком или использованием
 * низкоскоростных каналов связи. Для компенсации этой задержки, время приёма
 * данных портом уменьшается на величину заданную функцией #hyscan_sensor_set_latency.
 *
 * Приём данных каждым из датчиков можно включить или выключить с помощью
 * функции #hyscan_sensor_set_enable.
 *
 * При получении данных от датчиков интерфейс посылает сигнал "sensor-data",
 * в котором передаёт их пользователю. Прототип обработчика сигнала:
 *
 * \code
 *
 * void    sensor_data_cb    (HyScanSensor             *sensor,
 *                            const gchar              *name,
 *                            gint64                    time,
 *                            HyScanSourceType          type,
 *                            guint32                   size,
 *                            gpointer                  data,
 *                            gpointer                  user_data);
 *
 * \endcode
 *
 * Где:
 * - name - название порта;
 * - time - время приёма данных с учётом задержки, мкс;
 * - type - тип данных;
 * - size - размер данных в байтах;
 * - data - данные.
 *
 * В процессе работы драйвер может отправлять различные информационные и
 * диагностические сообщения. При получении такого сообщения интерфейс
 * посылает сигнал "sensor-log", в котором передаёт их пользователю.
 * Прототип обработчика сигнала:
 *
 * \code
 *
 * void    sensor_log_cb     (HyScanSensor             *sensor,
 *                            const gchar              *name,
 *                            gint64                    time,
 *                            HyScanLogLevel            level,
 *                            const gchar              *message
 *                            gpointer                  user_data);
 *
 * \endcode
 *
 * Где:
 * - name - название порта;
 * - time - время приёма сообщения, мкс;
 * - level - тип сообщения;
 * - message - сообщение (NULL терминированная строка).
 *
 * Принятое сообщение имеет отнощение к тому порту, название которого указано в name.
 * Если name = NULL, сообщение относится к драйверу в целом.
 *
 */

#ifndef __HYSCAN_SENSOR_H__
#define __HYSCAN_SENSOR_H__

#include <glib-object.h>
#include <hyscan-types.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_SENSOR            (hyscan_sensor_get_type ())
#define HYSCAN_SENSOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SENSOR, HyScanSensor))
#define HYSCAN_IS_SENSOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SENSOR))
#define HYSCAN_SENSOR_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_SENSOR, HyScanSensorInterface))

typedef struct _HyScanSensor HyScanSensor;
typedef struct _HyScanSensorInterface HyScanSensorInterface;

struct _HyScanSensorInterface
{
  GTypeInterface       g_iface;

  gboolean             (*set_sound_velocity)                   (HyScanSensor                  *sensor,
                                                                HyScanSoundVelocity           *sound_velocity,
                                                                guint32                        n_points);

  gboolean             (*set_channel)                          (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                guint                          channel);

  gboolean             (*set_latency)                          (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                gint64                         latency);

  gboolean             (*set_enable)                           (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                gboolean                       enable);
};

HYSCAN_API
GType                  hyscan_sensor_get_type                  (void);

/**
 *
 * Функция задаёт таблицу профиля скорости звука.
 *
 * \param sensor указатель на \link HyScanSensor \endlink;
 * \param sound_velocity таблица профиля скорости звука;
 * \param n_points число элементов таблицы профиля скорости звука.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_sensor_set_sound_velocity        (HyScanSensor                  *sensor,
                                                                HyScanSoundVelocity           *sound_velocity,
                                                                guint32                        n_points);

/**
 *
 * Функция устанавливает номер приёмного канала для указанного порта.
 *
 * \param sensor указатель на \link HyScanSensor \endlink;
 * \param name название порта;
 * \param channel номер канала.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_sensor_set_channel               (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                guint                          channel);

/**
 *
 * Функция устанавливает величину компенсации задержки при приёме данных портом.
 *
 * \param sensor указатель на \link HyScanSensor \endlink;
 * \param name название порта;
 * \param latency задержка приёма данных, мкс.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_sensor_set_latency               (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                gint64                         latency);

/**
 *
 * Функция включает или выключает приём данных.
 *
 * \param sensor указатель на \link HyScanSensor \endlink;
 * \param name название порта;
 * \param enable включён или выключен.
 *
 * \return TRUE - если команда выполнена успешно, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_sensor_set_enable                (HyScanSensor                  *sensor,
                                                                const gchar                   *name,
                                                                gboolean                       enable);

G_END_DECLS

#endif /* __HYSCAN_SENSOR_H__ */
