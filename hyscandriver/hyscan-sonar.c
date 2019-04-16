/* hyscan-sonar.c
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
 * В зависимости от типа, гидролокатор может иметь различное число источников
 * данных. Каждый из источников гидролокационных данных связан с одним из
 * «бортов» гидролокатора.
 *
 * Задать смещение приёмных антенн гидролокатора можно с помощью функции
 * #hyscan_sonar_antenna_set_offset.
 *
 * Рабочая дистанция для каждого источника данных определяется временем приёма
 * эхосигнала. Управлять временем приёма можно только если такая возможность
 * предусмотрена источником данных. Для этого используются функции
 * #hyscan_sonar_receiver_set_time и #hyscan_sonar_receiver_set_auto.
 * Функция #hyscan_sonar_receiver_set_time устанавливает фиксированное время
 * приёма эхосигнала, функция #hyscan_sonar_receiver_set_auto включает
 * автоматическую настройку времени приёма.
 *
 * С источником гидролокационных данных, может быть связан генератор
 * излучаемого сигнала. Пользователь может выбрать один из предварительно
 * настроенных режимов. Установка режима работы генератора осуществляется
 * функцией #hyscan_sonar_generator_set_preset. Режим работы генератора может
 * задаваться независимо для каждого источника данных.
 *
 * С источником гидролокационных данных может быть связана подсистема
 * регулировки усиления - ВАРУ. Управление усилением может осуществляться в
 * одном из доступных режимов. Режим работы усиления может задаваться
 * независимо для каждого источника данных.
 *
 * Возможны следующие режимы управления усилением:
 *
 * - #HYSCAN_SONAR_TVG_MODE_AUTO - автоматический;
 * - #HYSCAN_SONAR_TVG_MODE_CONSTANT - постоянный уровень усиления;
 * - #HYSCAN_SONAR_TVG_MODE_LINEAR_DB - линейное изменение усиления в дБ;
 * - #HYSCAN_SONAR_TVG_MODE_LOGARITHMIC - управление усилением по логарифмическому закону.
 *
 * При выборе автоматического режима работы, система ВАРУ будет самостоятельно
 * управлять усилением приёмных каналов. Включение автоматического режима
 * осуществляется функцией #hyscan_sonar_tvg_set_auto.
 *
 * Постоянный уровень усиления можно задать с помощью функции
 * #hyscan_sonar_tvg_set_constant.
 *
 * Функция #hyscan_sonar_tvg_set_linear_db активирует режим ВАРУ, при котором
 * коэффициент усиления, в дБ, линейно изменяется на указанную величину каждые
 * 100 метров.
 *
 * Управление усилением по логарифмическому закону включается функцией
 * #hyscan_sonar_tvg_set_logarithmic. При этом используется следующая формула
 * при расчёте коэффициента усиления:
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
 * Включить гидролокатор в рабочий режим, в соответствии с установленными
 * параметрами, можно при помощи функции #hyscan_sonar_start, остановить
 * #hyscan_sonar_stop.
 *
 * Настройку режимов работы гидролокатора необходимо выполнять каждый раз перед
 * его переводом в рабочий режим из режиме останова. Если для какого-либо
 * источника данных не задан режим работы приёмника, этот источник данных будет
 * отключен. Аналогично, если не заданы параметры генератора или системы ВАРУ,
 * будут отключены эти подсистемы.
 *
 * Параметры включенных подсистем гидролокатора можно менять в рабочем состоянии.
 * Отключить приём, излучение или усиление во время работы можно с помощью
 * функций #hyscan_sonar_receiver_disable, #hyscan_sonar_generator_disable
 * и #hyscan_sonar_tvg_disable. Для включения этих подсистем обратно необходимо
 * выбрать один из доступных для них режимов работы. После изменения режимов
 * работы, необходимо сигнализировать об этом гидролокатору с помощью функции
 * #hyscan_sonar_sync. Это необходимо для атомарного применения нескольких
 * изменившихся параметров, например для одновременного изменения времени приёма
 * для левого и правого бортов и т.п.
 */

#include "hyscan-sonar.h"
#include "hyscan-driver-marshallers.h"

#include <hyscan-buffer.h>

G_DEFINE_INTERFACE (HyScanSonar, hyscan_sonar, G_TYPE_OBJECT)

static void
hyscan_sonar_default_init (HyScanSonarInterface *iface)
{
  /**
   * HyScanSonar::sonar-signal:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @channel: индекс канала данных
   * @time: время начала действия сигнала, мкс
   * @image: (nullable): образ сигнала #HyScanBuffer
   *
   * Данный сигнал посылается при изменении излучаемого сигнала.
   * В нём передаётся новый образ сигнала для свёртки.
   *
   * Для каждого из источников данных, этот сигнал должен посылаться
   * из одного и того же потока. Также, не допускается отправка данных
   * с меткой времени меньшей или равной отправленной ранее.
   */
  g_signal_new ("sonar-signal", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_UINT_INT64_OBJECT,
                G_TYPE_NONE, 4,
                G_TYPE_INT,
                G_TYPE_UINT,
                G_TYPE_INT64,
                HYSCAN_TYPE_BUFFER);

  /**
   * HyScanSonar::sonar-tvg:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @channel: индекс канала данных
   * @time: время начала действия параметров ВАРУ, мкс
   * @gains: коэффициенты усиления #HyScanBuffer
   *
   * Данный сигнал посылается при изменении параметров ВАРУ.
   * В нём передаются новые коэффициенты усиления.
   *
   * Для каждого из источников данных, этот сигнал должен посылаться
   * из одного и того же потока. Также, не допускается отправка данных
   * с меткой времени меньшей или равной отправленной ранее.
   */
  g_signal_new ("sonar-tvg", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_UINT_INT64_OBJECT,
                G_TYPE_NONE, 4,
                G_TYPE_INT,
                G_TYPE_UINT,
                G_TYPE_INT64,
                HYSCAN_TYPE_BUFFER);

  /**
   * HyScanSonar::sonar-acoustic-data:
   * @sonar: указатель на #HyScanSonar
   * @source: идентификатор источника данных #HyScanSourceType
   * @channel: индекс канала данных
   * @noise: признак данных шума (выключенное излучение)
   * @time: время приёма данных, мкс
   * @info: параметры данных #HyScanAcousticDataInfo
   * @data: данные #HyScanBuffer
   *
   * Данный сигнал посылается при получении гидроакустических данных.
   *
   * Для каждого из источников данных, этот сигнал должен посылаться
   * из одного и того же потока. Также, не допускается отправка данных
   * с меткой времени меньшей или равной отправленной ранее.
   */
  g_signal_new ("sonar-acoustic-data", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_UINT_BOOLEAN_INT64_POINTER_OBJECT,
                G_TYPE_NONE, 6,
                G_TYPE_INT,
                G_TYPE_UINT,
                G_TYPE_BOOLEAN,
                G_TYPE_INT64,
                G_TYPE_POINTER,
                HYSCAN_TYPE_BUFFER);
}

/**
 * hyscan_sonar_antenna_set_offset:
 * @control: указатель на #HyScanControl
 * @source: идентификатор источника данных #HyScanSourceType
 * @offset: смещение приёмной антенны
 *
 * Функция задаёт смещение приёмных антенн гидролокатора. Подробное
 * описание параметров приводится в #HyScanTypes.
 *
 * Если для источника задано смещение по умолчанию, изменить его нельзя.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_antenna_set_offset (HyScanSonar               *sonar,
                                 HyScanSourceType           source,
                                 const HyScanAntennaOffset *offset)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->antenna_set_offset != NULL)
    return (* iface->antenna_set_offset) (sonar, source, offset);

  return FALSE;
}

/**
 * hyscan_sonar_receiver_set_time:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @receive_time: время приёма эхосигнала, секунды
 * @wait_time: время задержки излучения после приёма, секунды
 *
 * Функция задаёт время приёма эхосигнала.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_receiver_set_time (HyScanSonar      *sonar,
                                HyScanSourceType  source,
                                gdouble           receive_time,
                                gdouble           wait_time)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->receiver_set_time != NULL)
    return (* iface->receiver_set_time) (sonar, source, receive_time, wait_time);

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
 * hyscan_sonar_receiver_disable:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 *
 * Функция отключает приём эхосигнала.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_receiver_disable (HyScanSonar      *sonar,
                               HyScanSourceType  source)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->receiver_disable != NULL)
    return (* iface->receiver_disable) (sonar, source);

  return FALSE;
}

/**
 * hyscan_sonar_generator_set_preset:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @preset: идентификатор режима работы генератора
 *
 * Функция устанавливает режим работы генератора.
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
 * hyscan_sonar_generator_disable:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 *
 * Функция отключает излучение сигнала генератором.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_generator_disable (HyScanSonar      *sonar,
                                HyScanSourceType  source)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_disable != NULL)
    return (* iface->generator_disable) (sonar, source);

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
 * Уровень сигнала и чувствительность должны находиться в диапазоне значений от
 * 0.0 до 1.0 включительно. Если в качестве значений параметров уровня сигнала
 * и (или) чувствительности передать отрицательное число, будут установлены
 * значения по умолчанию.
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
 * hyscan_sonar_tvg_set_constant:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @gain: уровень усиления, дБ
 *
 * Функция устанавливает постоянный уровень усиления. Величина усиления должна
 * находится в пределах от минимального до максимально возможного для источника
 * данных.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_set_constant (HyScanSonar      *sonar,
                               HyScanSourceType  source,
                               gdouble           gain)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_constant != NULL)
    return (* iface->tvg_set_constant) (sonar, source, gain);

  return FALSE;
}

/**
 * hyscan_sonar_tvg_set_linear_db:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @gain0: начальный уровень усиления, дБ
 * @step: величина изменения усиления каждые 100 метров, дБ
 *
 * Функция устанавливает линейное увеличение усиления в дБ на 100 метров.
 *
 * Начальный уровень усиления может находится в любых пределах. Однако реальное
 * усиление будет находится в пределах от минимального до максимально возможного
 * для источника данных. Величина изменения усиления должна быть положительной
 * или равной нулю.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_set_linear_db (HyScanSonar      *sonar,
                                HyScanSourceType  source,
                                gdouble           gain0,
                                gdouble           step)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_linear_db != NULL)
    return (* iface->tvg_set_linear_db) (sonar, source, gain0, step);

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
 * Начальный уровень усиления может находится в любых пределах. Однако реальное
 * усиление будет находится в пределах от минимального до максимально возможного
 * для источника данных. Значения коэффициентов поглощения цели и затухания
 * должны быть положителными или равными нулю.
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
 * hyscan_sonar_tvg_disable:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 *
 * Функция отключает управление усилением.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_sonar_tvg_disable (HyScanSonar      *sonar,
                          HyScanSourceType  source)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_disable != NULL)
    return (* iface->tvg_disable) (sonar, source);

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
