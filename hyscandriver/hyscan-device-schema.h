/* hyscan-device-schema.h
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

#ifndef __HYSCAN_DEVICE_SCHEMA_H__
#define __HYSCAN_DEVICE_SCHEMA_H__

#include <hyscan-data-schema-builder.h>

G_BEGIN_DECLS

/**
 * HYSCAN_DEVICE_SCHEMA_ID:
 *
 * Идентификатор схемы устройства.
 */
#define HYSCAN_DEVICE_SCHEMA_ID                        1374652938475623487

/**
 * HYSCAN_DEVICE_SCHEMA_VERSION:
 *
 * Версия схемы устройства.
 */
#define HYSCAN_DEVICE_SCHEMA_VERSION                   20180100

/**
 * HYSCAN_DEVICE_SCHEMA_STATUS_OK:
 *
 * Устройство функционирует в штатном режиме.
 */
#define HYSCAN_DEVICE_SCHEMA_STATUS_OK                 "ok"

/**
 * HYSCAN_DEVICE_SCHEMA_STATUS_WARNING:
 *
 * В работе устройства присутствуют кратковременные сбои.
 */
#define HYSCAN_DEVICE_SCHEMA_STATUS_WARNING            "warning"

/**
 * HYSCAN_DEVICE_SCHEMA_STATUS_CRITICAL:
 *
 * В работе устройства присутствуют постоянные сбои.
 */
#define HYSCAN_DEVICE_SCHEMA_STATUS_CRITICAL           "critical"

/**
 * HYSCAN_DEVICE_SCHEMA_STATUS_ERROR:
 *
 * Ошибка, устройство не может продолжать работу.
 */
#define HYSCAN_DEVICE_SCHEMA_STATUS_ERROR              "error"

gboolean       hyscan_device_schema_set_id             (HyScanDataSchemaBuilder       *builder);

gboolean       hyscan_device_schema_check_id           (HyScanDataSchema              *schema);

gboolean       hyscan_device_schema_get_boolean        (HyScanDataSchema              *schema,
                                                        const gchar                   *name,
                                                        gboolean                      *value);

gboolean       hyscan_device_schema_get_integer        (HyScanDataSchema              *schema,
                                                        const gchar                   *name,
                                                        gint64                        *min_value,
                                                        gint64                        *max_value,
                                                        gint64                        *default_value,
                                                        gint64                        *value_step);

gboolean       hyscan_device_schema_get_double         (HyScanDataSchema              *schema,
                                                        const gchar                   *name,
                                                        gdouble                       *min_value,
                                                        gdouble                       *max_value,
                                                        gdouble                       *default_value,
                                                        gdouble                       *value_step);

const gchar *  hyscan_device_schema_get_string         (HyScanDataSchema              *schema,
                                                        const gchar                   *name);

G_END_DECLS

#endif /* __HYSCAN_DEVICE_SCHEMA_H__ */
