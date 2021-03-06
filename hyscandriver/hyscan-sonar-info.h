/* hyscan-sonar-info.h
 *
 * Copyright 2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_SONAR_INFO_H__
#define __HYSCAN_SONAR_INFO_H__

#include <hyscan-sonar.h>
#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_SONAR_INFO_SOURCE        (hyscan_sonar_info_source_get_type ())
#define HYSCAN_TYPE_SONAR_INFO_CAPABILITIES  (hyscan_sonar_info_capabilities_get_type ())
#define HYSCAN_TYPE_SONAR_INFO_RECEIVER      (hyscan_sonar_info_receiver_get_type ())
#define HYSCAN_TYPE_SONAR_INFO_GENERATOR     (hyscan_sonar_info_generator_get_type ())
#define HYSCAN_TYPE_SONAR_INFO_TVG           (hyscan_sonar_info_tvg_get_type ())
#define HYSCAN_TYPE_SONAR_INFO_SIGNAL        (hyscan_sonar_info_signal_get_type ())

#define HYSCAN_TYPE_SONAR_INFO               (hyscan_sonar_info_get_type ())
#define HYSCAN_SONAR_INFO(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SONAR_INFO, HyScanSonarInfo))
#define HYSCAN_IS_SONAR_INFO(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SONAR_INFO))
#define HYSCAN_SONAR_INFO_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_SONAR_INFO, HyScanSonarInfoClass))
#define HYSCAN_IS_SONAR_INFO_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_SONAR_INFO))
#define HYSCAN_SONAR_INFO_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_SONAR_INFO, HyScanSonarInfoClass))

typedef struct _HyScanSonarInfo HyScanSonarInfo;
typedef struct _HyScanSonarInfoPrivate HyScanSonarInfoPrivate;
typedef struct _HyScanSonarInfoClass HyScanSonarInfoClass;
typedef struct _HyScanSonarInfoSource HyScanSonarInfoSource;
typedef struct _HyScanSonarInfoReceiver HyScanSonarInfoReceiver;
typedef struct _HyScanSonarInfoTVG HyScanSonarInfoTVG;

struct _HyScanSonarInfo
{
  GObject parent_instance;

  HyScanSonarInfoPrivate *priv;
};

struct _HyScanSonarInfoClass
{
  GObjectClass parent_class;
};

/**
 * HyScanSonarInfoSource:
 * @source: тип источника данных
 * @link: связанный источник данных
 * @dev_id: уникальный идентификатор устройства
 * @description: (nullable): описание источника данных
 * @actuator: (nullable): используемый привод
 * @offset: смещение приёмной антенны по умолчанию
 * @receiver: параметры приёмника
 * @presets: (element-type HyScanDataSchemaEnumValue) (transfer none): режимы генератора
 * @tvg: параметры ВАРУ
 *
 * Параметры гидролокационного источника данных.
 */
struct _HyScanSonarInfoSource
{
  HyScanSourceType             source;
  HyScanSourceType             link;
  const gchar                 *dev_id;
  const gchar                 *description;
  const gchar                 *actuator;
  HyScanAntennaOffset         *offset;
  HyScanSonarInfoReceiver     *receiver;
  GList                       *presets;
  HyScanSonarInfoTVG          *tvg;
};

/**
 * HyScanSonarInfoReceiver:
 * @capabilities: допустимые режимы работы приёмника
 * @min_time: минимальное время приёма данных, с
 * @max_time: максимальное время приёма данных, с
 *
 * Параметры гидролокационного приёмника.
 */
struct _HyScanSonarInfoReceiver
{
  HyScanSonarReceiverModeType  capabilities;
  gdouble                      min_time;
  gdouble                      max_time;
};

/**
 * HyScanSonarInfoTVG:
 * @min_gain: минимальное значение коэффициента усиления, дБ
 * @max_gain: максимальное значение коэффициента усиления, дБ
 * @decrease: возможность уменьшения коэффициента усиления
 * @capabilities: допустимые режимы работы ВАРУ
 *
 * Параметры системы ВАРУ.
 */
struct _HyScanSonarInfoTVG
{
  HyScanSonarTVGModeType       capabilities;
  gdouble                      min_gain;
  gdouble                      max_gain;
  gboolean                     decrease;
};

HYSCAN_API
GType                          hyscan_sonar_info_source_get_type       (void);

HYSCAN_API
GType                          hyscan_sonar_info_receiver_get_type     (void);

HYSCAN_API
GType                          hyscan_sonar_info_tvg_get_type          (void);

HYSCAN_API
GType                          hyscan_sonar_info_get_type              (void);

HYSCAN_API
HyScanSonarInfo *              hyscan_sonar_info_new                   (HyScanDataSchema                  *schema);

HYSCAN_API
const HyScanSourceType *       hyscan_sonar_info_list_sources          (HyScanSonarInfo                   *info,
                                                                        guint32                           *n_sources);

HYSCAN_API
const HyScanSonarInfoSource *  hyscan_sonar_info_get_source            (HyScanSonarInfo                   *info,
                                                                        HyScanSourceType                   source);

HYSCAN_API
HyScanSonarInfoSource *        hyscan_sonar_info_source_copy           (const HyScanSonarInfoSource       *info);

HYSCAN_API
void                           hyscan_sonar_info_source_free           (HyScanSonarInfoSource             *info);

HYSCAN_API
HyScanSonarInfoReceiver *      hyscan_sonar_info_receiver_copy         (const HyScanSonarInfoReceiver     *info);

HYSCAN_API
void                           hyscan_sonar_info_receiver_free         (HyScanSonarInfoReceiver           *info);

HYSCAN_API
HyScanSonarInfoTVG *           hyscan_sonar_info_tvg_copy              (const HyScanSonarInfoTVG          *info);

HYSCAN_API
void                           hyscan_sonar_info_tvg_free              (HyScanSonarInfoTVG                *info);

G_END_DECLS

#endif /* __HYSCAN_SONAR_INFO_H__ */
