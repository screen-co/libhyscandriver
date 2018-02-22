/* hyscan-sonar-schema.h
 *
 * Copyright 2016-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_SONAR_SCHEMA_H__
#define __HYSCAN_SONAR_SCHEMA_H__

#include <hyscan-sonar-info.h>
#include <hyscan-data-schema-builder.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_SONAR_SCHEMA             (hyscan_sonar_schema_get_type ())
#define HYSCAN_SONAR_SCHEMA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_SONAR_SCHEMA, HyScanSonarSchema))
#define HYSCAN_IS_SONAR_SCHEMA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_SONAR_SCHEMA))
#define HYSCAN_SONAR_SCHEMA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_SONAR_SCHEMA, HyScanSonarSchemaClass))
#define HYSCAN_IS_SONAR_SCHEMA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_SONAR_SCHEMA))
#define HYSCAN_SONAR_SCHEMA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_SONAR_SCHEMA, HyScanSonarSchemaClass))

typedef struct _HyScanSonarSchema HyScanSonarSchema;
typedef struct _HyScanSonarSchemaPrivate HyScanSonarSchemaPrivate;
typedef struct _HyScanSonarSchemaClass HyScanSonarSchemaClass;

struct _HyScanSonarSchema
{
  GObject parent_instance;

  HyScanSonarSchemaPrivate *priv;
};

struct _HyScanSonarSchemaClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_sonar_schema_get_type                    (void);

HYSCAN_API
HyScanSonarSchema *    hyscan_sonar_schema_new                         (HyScanDataSchemaBuilder       *builder);

HYSCAN_API
gboolean               hyscan_sonar_schema_set_software_ping           (HyScanSonarSchema             *schema);

HYSCAN_API
gboolean               hyscan_sonar_schema_source_add_full             (HyScanSonarSchema             *schema,
                                                                        HyScanSonarInfoSource         *info);

HYSCAN_API
gboolean               hyscan_sonar_schema_source_add                  (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        HyScanSonarReceiverModeType    receiver_capabilities,
                                                                        HyScanSonarGeneratorModeType   generator_capabilities,
                                                                        HyScanSonarTVGModeType         tvg_capabilities);

HYSCAN_API
gboolean               hyscan_sonar_schema_source_set_description      (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        const gchar                   *description);

HYSCAN_API
gboolean               hyscan_sonar_schema_source_set_master           (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        HyScanSourceType               master);

HYSCAN_API
gboolean               hyscan_sonar_schema_source_set_position         (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        HyScanAntennaPosition         *position);

HYSCAN_API
gboolean               hyscan_sonar_schema_antenna_set_params          (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        gdouble                        antenna_vpattern,
                                                                        gdouble                        antenna_hpattern,
                                                                        gdouble                        antenna_frequency,
                                                                        gdouble                        antenna_bandwidth);

HYSCAN_API
gboolean               hyscan_sonar_schema_receiver_set_params         (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        gdouble                        min_time,
                                                                        gdouble                        max_time);

HYSCAN_API
gboolean               hyscan_sonar_schema_receiver_add_channel        (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        guint                          channel,
                                                                        gdouble                        antenna_voffset,
                                                                        gdouble                        antenna_hoffset,
                                                                        gint                           adc_offset,
                                                                        gdouble                        adc_vref);

HYSCAN_API
gboolean               hyscan_sonar_schema_generator_add_preset        (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        gint                           preset_id,
                                                                        const gchar                   *preset_name,
                                                                        const gchar                   *preset_description);

HYSCAN_API
gboolean               hyscan_sonar_schema_generator_add_auto          (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source);

HYSCAN_API
gboolean               hyscan_sonar_schema_generator_set_params        (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        HyScanSonarGeneratorSignalType signal,
                                                                        gdouble                        min_duration,
                                                                        gdouble                        max_duration,
                                                                        gdouble                        dirty_cycle);

HYSCAN_API
gboolean               hyscan_sonar_schema_tvg_set_params              (HyScanSonarSchema             *schema,
                                                                        HyScanSourceType               source,
                                                                        gdouble                        min_gain,
                                                                        gdouble                        max_gain,
                                                                        gboolean                       can_decrease);

G_END_DECLS

#endif /* __HYSCAN_SONAR_SCHEMA_H__ */
