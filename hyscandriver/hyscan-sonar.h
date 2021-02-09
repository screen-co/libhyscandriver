/* hyscan-sonar.h
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

#ifndef __HYSCAN_SONAR_H__
#define __HYSCAN_SONAR_H__

#include <hyscan-types.h>

G_BEGIN_DECLS

/**
 * HyScanSonarReceiverModeType:
 * @HYSCAN_SONAR_RECEIVER_MODE_NONE: Приёмник отсутствует.
 * @HYSCAN_SONAR_RECEIVER_MODE_MANUAL: Ручной режим работы.
 * @HYSCAN_SONAR_RECEIVER_MODE_AUTO: Автоматический режим работы.
 *
 * Режимы работы приёмника данных.
 */
typedef enum
{
  HYSCAN_SONAR_RECEIVER_MODE_NONE              = 0,

  HYSCAN_SONAR_RECEIVER_MODE_MANUAL            = (1 << 0),
  HYSCAN_SONAR_RECEIVER_MODE_AUTO              = (1 << 1),
} HyScanSonarReceiverModeType;

/**
 * HyScanSonarTVGModeType:
 * @HYSCAN_SONAR_TVG_MODE_INVALID: Система ВАРУ отсутствует.
 * @HYSCAN_SONAR_TVG_MODE_AUTO: Автоматический режим работы.
 * @HYSCAN_SONAR_TVG_MODE_CONSTANT: Постоянный уровень усиления.
 * @HYSCAN_SONAR_TVG_MODE_LINEAR_DB: Линейное увеличение усиления в дБ / 100 метров.
 * @HYSCAN_SONAR_TVG_MODE_LOGARITHMIC: Управление усилением по логарифмическому закону.
 *
 * Режимы работы системы ВАРУ.
 */
typedef enum
{
  HYSCAN_SONAR_TVG_MODE_NONE                   = 0,

  HYSCAN_SONAR_TVG_MODE_AUTO                   = (1 << 0),
  HYSCAN_SONAR_TVG_MODE_CONSTANT               = (1 << 1),
  HYSCAN_SONAR_TVG_MODE_LINEAR_DB              = (1 << 2),
  HYSCAN_SONAR_TVG_MODE_LOGARITHMIC            = (1 << 3)
} HyScanSonarTVGModeType;

#define HYSCAN_TYPE_SONAR            (hyscan_sonar_get_type ())
#define HYSCAN_SONAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SONAR, HyScanSonar))
#define HYSCAN_IS_SONAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SONAR))
#define HYSCAN_SONAR_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_SONAR, HyScanSonarInterface))

typedef struct _HyScanSonar HyScanSonar;
typedef struct _HyScanSonarInterface HyScanSonarInterface;

/**
 * HyScanSonarInterface:
 * @g_iface: Базовый интерфейс.
 * @antenna_set_offset: Функция задаёт смещение приёмных антенн гидролокатора.
 * @receiver_set_time: Функция задаёт время приёма эхосигнала.
 * @receiver_set_auto: Функция задаёт автоматический выбор времени приёма эхосигнала.
 * @receiver_disable: Функция отключает приём эхосигнала.
 * @generator_set_preset: Функция устанавливает режим работы генератора.
 * @generator_disable: Функция отключает излучение сигнала генератором.
 * @tvg_set_auto: Функция включает автоматический режим управления системой ВАРУ.
 * @tvg_set_constant: Функция устанавливает постоянный уровень усиления.
 * @tvg_set_linear_db: Функция устанавливает линейное увеличение усиления в дБ на 100 метров.
 * @tvg_set_logarithmic: Функция устанавливает логарифмический вид закона усиления системой ВАРУ.
 * @tvg_disable: Функция отключает управление усилением.
 * @start: Функция переводит гидролокатор в рабочий режим.
 * @stop: Функция переводит гидролокатор в ждущий режим и отключает запись данных.
 */
struct _HyScanSonarInterface
{
  GTypeInterface       g_iface;

  gboolean             (*antenna_set_offset)                   (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                const HyScanAntennaOffset      *offset);

  gboolean             (*receiver_set_time)                    (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         receive_time,
                                                                gdouble                         wait_time);

  gboolean             (*receiver_set_auto)                    (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

  gboolean             (*receiver_disable)                     (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

  gboolean             (*generator_set_preset)                 (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gint64                          preset);

  gboolean             (*generator_disable)                    (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

  gboolean             (*tvg_set_auto)                         (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         level,
                                                                gdouble                         sensitivity);

  gboolean             (*tvg_set_constant)                     (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         gain);

  gboolean             (*tvg_set_linear_db)                    (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         gain0,
                                                                gdouble                         gain_step);

  gboolean             (*tvg_set_logarithmic)                  (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         gain0,
                                                                gdouble                         beta,
                                                                gdouble                         alpha);

  gboolean             (*tvg_disable)                          (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

  gboolean             (*start)                                (HyScanSonar                    *sonar,
                                                                const gchar                    *project_name,
                                                                const gchar                    *track_name,
                                                                HyScanTrackType                 track_type,
                                                                const HyScanTrackPlan          *track_plan);

  gboolean             (*stop)                                 (HyScanSonar                    *sonar);
};

HYSCAN_API
GType                  hyscan_sonar_get_type                   (void);

HYSCAN_API
gboolean               hyscan_sonar_antenna_set_offset         (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                const HyScanAntennaOffset      *offset);

HYSCAN_API
gboolean               hyscan_sonar_receiver_set_time          (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         receive_time,
                                                                gdouble                         wait_time);

HYSCAN_API
gboolean               hyscan_sonar_receiver_set_auto          (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

HYSCAN_API
gboolean               hyscan_sonar_receiver_disable           (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

HYSCAN_API
gboolean               hyscan_sonar_generator_set_preset       (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gint64                          preset);

HYSCAN_API
gboolean               hyscan_sonar_generator_disable          (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

HYSCAN_API
gboolean               hyscan_sonar_tvg_set_auto               (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         level,
                                                                gdouble                         sensitivity);

HYSCAN_API
gboolean               hyscan_sonar_tvg_set_constant           (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         gain);

HYSCAN_API
gboolean               hyscan_sonar_tvg_set_linear_db          (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         gain0,
                                                                gdouble                         step);

HYSCAN_API
gboolean               hyscan_sonar_tvg_set_logarithmic        (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source,
                                                                gdouble                         gain0,
                                                                gdouble                         beta,
                                                                gdouble                         alpha);

HYSCAN_API
gboolean               hyscan_sonar_tvg_disable                (HyScanSonar                    *sonar,
                                                                HyScanSourceType                source);

HYSCAN_API
gboolean               hyscan_sonar_start                      (HyScanSonar                    *sonar,
                                                                const gchar                    *project_name,
                                                                const gchar                    *track_name,
                                                                HyScanTrackType                 track_type,
                                                                const HyScanTrackPlan          *track_plan);

HYSCAN_API
gboolean               hyscan_sonar_stop                       (HyScanSonar                    *sonar);

G_END_DECLS

#endif /* __HYSCAN_SONAR_H__ */
