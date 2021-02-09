/* hyscan-sonar-driver.h
 *
 * Copyright 2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_SONAR_DRIVER_H__
#define __HYSCAN_SONAR_DRIVER_H__

#include <hyscan-sonar-schema.h>
#include <hyscan-buffer.h>

G_BEGIN_DECLS

HYSCAN_API
void           hyscan_sonar_driver_send_source_info    (gpointer                   sonar,
                                                        HyScanSourceType           source,
                                                        guint                      channel,
                                                        const gchar               *description,
                                                        const gchar               *actuator,
                                                        HyScanAcousticDataInfo    *info);

HYSCAN_API
void           hyscan_sonar_driver_send_signal         (gpointer                   sonar,
                                                        HyScanSourceType           source,
                                                        guint                      channel,
                                                        gint64                     time,
                                                        HyScanBuffer              *image);

HYSCAN_API
void           hyscan_sonar_driver_send_tvg            (gpointer                   sonar,
                                                        HyScanSourceType           source,
                                                        guint                      channel,
                                                        gint64                     time,
                                                        HyScanBuffer              *gains);

HYSCAN_API
void           hyscan_sonar_driver_send_acoustic_data  (gpointer                   sonar,
                                                        HyScanSourceType           source,
                                                        guint                      channel,
                                                        gboolean                   noise,
                                                        gint64                     time,
                                                        HyScanBuffer              *data);

G_END_DECLS

#endif /* __HYSCAN_SONAR_DRIVER_H__ */
