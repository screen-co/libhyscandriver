/* hyscan-actuator.h
 *
 * Copyright 2021 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_ACTUATOR_H__
#define __HYSCAN_ACTUATOR_H__

#include <hyscan-types.h>

G_BEGIN_DECLS

/**
 * HyScanActuatorModeType:
 * @HYSCAN_ACTUATOR_MODE_NONE: Привод отсутствует.
 * @HYSCAN_ACTUATOR_MODE_SCAN: Режим сканирования в заданном секторе.
 * @HYSCAN_ACTUATOR_MODE_MANUAL: Режим ручной установки положения.
 *
 * Режимы работы привода.
 */
typedef enum
{
  HYSCAN_ACTUATOR_MODE_NONE             = 0,

  HYSCAN_ACTUATOR_MODE_SCAN             = (1 << 0),
  HYSCAN_ACTUATOR_MODE_MANUAL           = (1 << 1)
} HyScanActuatorModeType;

#define HYSCAN_TYPE_ACTUATOR            (hyscan_actuator_get_type ())
#define HYSCAN_ACTUATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_ACTUATOR, HyScanActuator))
#define HYSCAN_IS_ACTUATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_ACTUATOR))
#define HYSCAN_ACTUATOR_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_ACTUATOR, HyScanActuatorInterface))

typedef struct _HyScanActuator HyScanActuator;
typedef struct _HyScanActuatorInterface HyScanActuatorInterface;

struct _HyScanActuatorInterface
{
  GTypeInterface       g_iface;

  gboolean             (*disable)                      (HyScanActuator        *actuator,
                                                        const gchar           *name);

  gboolean             (*scan)                         (HyScanActuator        *actuator,
                                                        const gchar           *name,
                                                        gdouble                from,
                                                        gdouble                to,
                                                        gdouble                speed);

  gboolean             (*manual)                       (HyScanActuator        *actuator,
                                                        const gchar           *name,
                                                        gdouble                angle);
};

HYSCAN_API
GType                  hyscan_actuator_get_type        (void);

HYSCAN_API
gboolean               hyscan_actuator_disable         (HyScanActuator        *actuator,
                                                        const gchar           *name);

HYSCAN_API
gboolean               hyscan_actuator_scan            (HyScanActuator        *actuator,
                                                        const gchar           *name,
                                                        gdouble                from,
                                                        gdouble                to,
                                                        gdouble                speed);

HYSCAN_API
gboolean               hyscan_actuator_manual          (HyScanActuator        *actuator,
                                                        const gchar           *name,
                                                        gdouble                angle);

G_END_DECLS

#endif /* __HYSCAN_ACTUATOR_H__ */
