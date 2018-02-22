/* hyscan-sonar.c
 *
 * Copyright 2016-2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * Contact the Screen LLC in this case - info@screen-co.ru
 */

/* HyScanDriver имеет двойную лицензию.
 *
 * Во-первых, вы можете распространять HyScanDriver на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
 */

/**
 * SECTION: hyscan-sonar
 * @Short_description: интерфейс управления гидролокатором
 * @Title: HyScanSonar
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
 * Перед началом работы рекомендуется задать профиль скорости звука. Для этого
 * используется функция #hyscan_sonar_set_sound_velocity. По умолчанию используется
 * фиксированное значение скорости звука, равное 1500 м/с.
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
 * Управление излучением в штатном режиме осуществляется с использованием внутреннего
 * таймера гидролокатора или внешней команды. Для некоторых гидролокаторов существует
 * возможность программного управления синхронизацией излучения. Выбрать режим
 * синхонизации излучения можно с помощью функции #hyscan_sonar_set_software_ping.
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
 */

#include "hyscan-sonar.h"
#include "hyscan-driver-marshallers.h"

G_DEFINE_INTERFACE (HyScanSonar, hyscan_sonar, G_TYPE_OBJECT)

static void
hyscan_sonar_default_init (HyScanSonarInterface *iface)
{
  /**
   * HyScanSonar::sonar-signal:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @source: идентификатор источника данных
   * @time: время начала действия сигнала, мкс
   * @n_points: число точек образа сигнала
   * @points: образ сигнала
   *
   * Данный сигнал посылается при изменении излучаемого сигнала.
   * В нём передаёт новый образ сигнала для свёртки.
   */
  g_signal_new ("sonar-signal", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_UINT_POINTER,
                G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT64, G_TYPE_UINT, G_TYPE_POINTER);

  /**
   * HyScanSonar::sonar-tvg:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @time: время начала действия параметров ВАРУ, мкс
   * @n_gains: число коэффициентов усиления
   * @gains: коэффициенты усиления.
   *
   * Данный сигнал посылается при изменении параметров ВАРУ.
   * В нём передаются новые коэффициенты усиления.
   */
  g_signal_new ("sonar-tvg", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_UINT_POINTER,
                G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT64, G_TYPE_UINT, G_TYPE_POINTER);

  /**
   * HyScanSonar::sonar-raw-data:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @channel: индекс канала данных
   * @time: время приёма данных, мкс
   * @type: тип данных #HyScanDataType
   * @size: размер данных в байтах
   * @data: данные
   *
   * Данный сигнал посылается при получении сырых данных от гидролокатора.
   */
  g_signal_new ("sonar-raw-data", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_UINT_INT64_INT_UINT_POINTER,
                G_TYPE_NONE, 6, G_TYPE_INT, G_TYPE_UINT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  /**
   * HyScanSonar::sonar-noise-data:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @channel: индекс канала данных
   * @time: время приёма данных, мкс
   * @type: тип данных #HyScanDataType
   * @size: размер данных в байтах
   * @data: данные
   *
   * Данный сигнал посылается при получении сырых данных от гидролокатора
   * при выключенном излучении - данные шума.
   */
  g_signal_new ("sonar-noise-data", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_UINT_INT64_INT_UINT_POINTER,
                G_TYPE_NONE, 6, G_TYPE_INT, G_TYPE_UINT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  /**
   * HyScanSonar::sonar-acoustic-data:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @time: время приёма данных, мкс
   * @type: тип данных #HyScanDataType
   * @size: размер данных в байтах
   * @data: данные
   *
   * Данный сигнал посылается при получении сырых данных от гидролокатора
   * при выключенном излучении - данные шума.
   */
  g_signal_new ("sonar-acoustic-data", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_INT_UINT_POINTER,
                G_TYPE_NONE, 5, G_TYPE_INT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  /**
   * HyScanSonar::sonar-log:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @time: время приёма сообщения, мкс
   * @level: тип сообщения #HyScanLogLevel
   * @message: сообщение (NULL терминированная строка)
   *
   * В процессе работы драйвер может отправлять различные информационные и
   * диагностические сообщения. При получении такого сообщения интерфейс
   * посылает данный сигнал, в котором передаёт их пользователю. Принятое
   * сообщение имеет отнощение к тому источнику данных, идентификатор которого
   * указан в source. Если source = 0, сообщение относится к драйверу в целом.
   */
  g_signal_new ("sonar-log", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_INT_STRING,
                G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_STRING);
}

/**
 * hyscan_sonar_set_sound_velocity:
 * @sonar: указатель на #HyScanSonar
 * @svp: (element-type: HyScanSoundVelocity) (transfer none): таблица профиля скорости звука
 *
 * Функция задаёт таблицу профиля скорости звука.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_set_sound_velocity (HyScanSonar *sonar,
                                 GList       *svp)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->set_sound_velocity != NULL)
    return (* iface->set_sound_velocity) (sonar, svp);

  return FALSE;
}

/**
 * hyscan_sonar_receiver_set_time:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @receive_time: время приёма эхосигнала, секунды
 *
 * Функция задаёт время приёма эхосигнала.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_receiver_set_time (HyScanSonar      *sonar,
                                HyScanSourceType  source,
                                gdouble           receive_time)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->receiver_set_time != NULL)
    return (* iface->receiver_set_time) (sonar, source, receive_time);

  return FALSE;
}

/**
 * hyscan_sonar_receiver_set_auto:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 *
 * Функция задаёт автоматический выбор времени приёма эхосигнала.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_receiver_set_auto (HyScanSonar      *sonar,
                                HyScanSourceType  source)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->receiver_set_auto != NULL)
    return (* iface->receiver_set_auto) (sonar, source);

  return FALSE;
}

/**
 * hyscan_sonar_receiver_set_enable:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @enable: включён или выключен
 *
 * Функция включает или выключает приём эхосигнала.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_receiver_set_enable (HyScanSonar      *sonar,
                                  HyScanSourceType  source,
                                  gboolean          enable)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->receiver_set_enable != NULL)
    return (* iface->receiver_set_enable) (sonar, source, enable);

  return FALSE;
}

/**
 * hyscan_sonar_generator_set_preset:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @preset: идентификатор преднастройки
 *
 * Функция включает преднастроенный режим работы генератора.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_generator_set_preset (HyScanSonar      *sonar,
                                   HyScanSourceType  source,
                                   gint64            preset)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_preset != NULL)
    return (* iface->generator_set_preset) (sonar, source, preset);

  return FALSE;
}

/**
 * hyscan_sonar_generator_set_auto:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @signal: тип сигнала #HyScanSonarGeneratorSignalType
 *
 * Функция включает автоматический режим работы генератора.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_generator_set_auto (HyScanSonar                    *sonar,
                                 HyScanSourceType                source,
                                 HyScanSonarGeneratorSignalType  signal)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_auto != NULL)
    return (* iface->generator_set_auto) (sonar, source, signal);

  return FALSE;
}

/**
 * hyscan_sonar_generator_set_simple:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @signal: тип сигнала #HyScanSonarGeneratorSignalType
 * @power: энергия сигнала, проценты
 *
 * Функция включает упрощённый режим работы генератора.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_generator_set_simple (HyScanSonar                    *sonar,
                                   HyScanSourceType                source,
                                   HyScanSonarGeneratorSignalType  signal,
                                   gdouble                         power)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_simple != NULL)
    return (* iface->generator_set_simple) (sonar, source, signal, power);

  return FALSE;
}

/**
 * hyscan_sonar_generator_set_extended:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @signal: тип сигнала #HyScanSonarGeneratorSignalType
 * @duration: длительность сигнала, с
 * @power: энергия сигнала, проценты
 *
 * Функция включает расширенный режим работы генератора.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_generator_set_extended (HyScanSonar                    *sonar,
                                     HyScanSourceType                source,
                                     HyScanSonarGeneratorSignalType  signal,
                                     gdouble                         duration,
                                     gdouble                         power)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_extended != NULL)
    return (* iface->generator_set_extended) (sonar, source, signal, duration, power);

  return FALSE;
}

/**
 * hyscan_sonar_generator_set_enable:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @enable: включён или выключен
 *
 * Функция включает или выключает формирование сигнала генератором.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_generator_set_enable (HyScanSonar      *sonar,
                                   HyScanSourceType  source,
                                   gboolean          enable)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_enable != NULL)
    return (* iface->generator_set_enable) (sonar, source, enable);

  return FALSE;
}

/**
 * hyscan_sonar_tvg_set_auto:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @level: целевой уровень сигнала
 * @sensitivity: чувствительность автомата регулировки
 *
 * Функция включает автоматический режим управления системой ВАРУ.
 *
 * Если в качестве значений параметров уровня сигнала и (или) чувствительности
 * передать отрицательное число, будут установлены значения по умолчанию.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_set_auto (HyScanSonar      *sonar,
                           HyScanSourceType  source,
                           gdouble           level,
                           gdouble           sensitivity)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_auto != NULL)
    return (* iface->tvg_set_auto) (sonar, source, level, sensitivity);

  return FALSE;
}

/**
 * hyscan_sonar_tvg_set_points:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @time_step: интервал времени между точками, с
 * @gains: (array length=n_gains) (transfer none): массив коэффициентов усиления, дБ
 * @n_gains: число коэффициентов усиления
 *
 * Функция устанавливает усиление определённое контрольными точками,
 * заданными на равномерной временной оси.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_set_points (HyScanSonar      *sonar,
                             HyScanSourceType  source,
                             gdouble           time_step,
                             gdouble          *gains,
                             guint32           n_gains)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_points != NULL)
    return (* iface->tvg_set_points) (sonar, source, time_step, gains, n_gains);

  return FALSE;
}

/**
 * hyscan_sonar_tvg_set_linear_db:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @gain0: начальный уровень усиления, дБ
 * @gain_step: величина изменения усиления каждые 100 метров, дБ
 *
 * Функция устанавливает линейное увеличение усиления в дБ на 100 метров.
 *
 * Начальный уровень усиления должен находится в пределах от -20 дБ, до максимально
 * возможного усиления. Величина изменения усиления должна находится в пределах от 0
 * до 40 дБ.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_set_linear_db (HyScanSonar      *sonar,
                                HyScanSourceType  source,
                                gdouble           gain0,
                                gdouble           gain_step)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_linear_db != NULL)
    return (* iface->tvg_set_linear_db) (sonar, source, gain0, gain_step);

  return FALSE;
}

/**
 * hyscan_sonar_tvg_set_logarithmic:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @gain0: начальный уровень усиления, дБ
 * @beta: коэффициент поглощения цели, дБ
 * @alpha: коэффициент затухания, дБ/м
 *
 * Функция устанавливает логарифмический вид закона усиления системой ВАРУ.
 *
 * Начальный уровень усиления должен находится в пределах от -20 дБ, до максимально
 * возможного усиления. Значение коэффициента поглощения цели должно находится в
 * пределах от 0 дБ до 40 дБ. Значение коэффициента затухания должно находится в
 * пределах от 0 дБ/м до 1 дБ/м.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_set_logarithmic (HyScanSonar      *sonar,
                                  HyScanSourceType  source,
                                  gdouble           gain0,
                                  gdouble           beta,
                                  gdouble           alpha)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_logarithmic != NULL)
    return (* iface->tvg_set_logarithmic) (sonar, source, gain0, beta, alpha);

  return FALSE;
}

/**
 * hyscan_sonar_tvg_set_enable:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @enable: включёна или выключена
 *
 * Функция включает или выключает систему ВАРУ.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_set_enable (HyScanSonar      *sonar,
                             HyScanSourceType  source,
                             gboolean          enable)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_enable != NULL)
    return (* iface->tvg_set_enable) (sonar, source, enable);

  return FALSE;
}

/**
 * hyscan_sonar_set_software_ping:
 * @sonar: указатель на #HyScanSonar
 * @enable: признак программного управления излучением
 *
 * Функция устанавливает программное управление излучением. Данная функция должна
 * быть вызвана перед запуском гидролокатора с помощью функции #hyscan_sonar_start,
 * если до этого он находился в ждущем режиме.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_set_software_ping (HyScanSonar *sonar,
                                gboolean     enable)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->set_software_ping != NULL)
    return (* iface->set_software_ping) (sonar, enable);

  return FALSE;
}

/**
 * hyscan_sonar_start:
 * @sonar: указатель на #HyScanSonar
 * @project_name: название проекта, в который записывать данные
 * @track_name: название галса, в который записывать данные
 * @track_type: тип галса
 *
 * Функция переводит гидролокатор в рабочий режим.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_start (HyScanSonar     *sonar,
                    const gchar     *project_name,
                    const gchar     *track_name,
                    HyScanTrackType  track_type)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->start != NULL)
    return (* iface->start) (sonar, project_name, track_name, track_type);

  return FALSE;
}

/**
 * hyscan_sonar_stop:
 * @sonar: указатель на #HyScanSonar
 *
 * Функция переводит гидролокатор в ждущий режим и отключает запись данных.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_stop (HyScanSonar *sonar)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->stop != NULL)
    return (* iface->stop) (sonar);

  return FALSE;
}

/**
 * hyscan_sonar_sync:
 * @sonar: указатель на #HyScanSonar
 *
 * Функция синхронизирует состояние гидролокатора в соответствии с заданными
 * параметрами. Данная функция должна вызываться при изменении параметров во
 * время работы гидролокатора. При переводе гидролокатора в рабочий режим данная
 * функция вызывается автоматически.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_sync (HyScanSonar *sonar)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->sync != NULL)
    return (* iface->sync) (sonar);

  return FALSE;
}

/**
 * hyscan_sonar_ping:
 * @sonar: указатель на #HyScanSonar
 *
 * Функция выполняет один цикл излучения сигнала и приёма данных. Для использования
 * этой функции данный тип синхронизации должен быть включён функцией
 * #hyscan_sonar_set_software_ping. Гидролокатор должен находится в рабочем режиме
 * (#hyscan_sonar_start).
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_ping (HyScanSonar *sonar)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->ping != NULL)
    return (* iface->ping) (sonar);

  return FALSE;
}
