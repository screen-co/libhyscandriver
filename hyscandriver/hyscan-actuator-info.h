/* hyscan-actuator-info.h
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

#ifndef __HYSCAN_ACTUATOR_INFO_H__
#define __HYSCAN_ACTUATOR_INFO_H__

#include <hyscan-actuator.h>
#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_ACTUATOR_INFO_ACTUATOR    (hyscan_actuator_info_actuator_get_type ())

#define HYSCAN_TYPE_ACTUATOR_INFO             (hyscan_actuator_info_get_type ())
#define HYSCAN_ACTUATOR_INFO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_ACTUATOR_INFO, HyScanActuatorInfo))
#define HYSCAN_IS_ACTUATOR_INFO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_ACTUATOR_INFO))
#define HYSCAN_ACTUATOR_INFO_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_ACTUATOR_INFO, HyScanActuatorInfoClass))
#define HYSCAN_IS_ACTUATOR_INFO_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_ACTUATOR_INFO))
#define HYSCAN_ACTUATOR_INFO_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_ACTUATOR_INFO, HyScanActuatorInfoClass))

typedef struct _HyScanActuatorInfo HyScanActuatorInfo;
typedef struct _HyScanActuatorInfoPrivate HyScanActuatorInfoPrivate;
typedef struct _HyScanActuatorInfoClass HyScanActuatorInfoClass;
typedef struct _HyScanActuatorInfoActuator HyScanActuatorInfoActuator;

struct _HyScanActuatorInfo
{
  GObject parent_instance;

  HyScanActuatorInfoPrivate *priv;
};

struct _HyScanActuatorInfoClass
{
  GObjectClass parent_class;
};


/**
 * HyScanActuatorInfoActuator:
 * @name: название привода
 * @dev_id: уникальный идентификатор устройства
 * @description: описание привода
 * @capabilities: допустимые режимы работы привода
 * @min_range: значение минимального угла поворота привода, десятичный градус
 * @max_range: значение максимального угла поворота привода, десятичный градус
 * @min_speed: минимальная скорость вращения привода, десятичный градус/с
 * @max_speed: максимальная скорость вращения привода, десятичный градус/с
 *
 * Параметры привода.
 */
struct _HyScanActuatorInfoActuator
{
  const gchar                    *name;
  const gchar                    *dev_id;
  const gchar                    *description;
  HyScanActuatorModeType          capabilities;
  gdouble                         min_range;
  gdouble                         max_range;
  gdouble                         min_speed;
  gdouble                         max_speed;
};

HYSCAN_API
GType                             hyscan_actuator_info_actuator_get_type (void);

HYSCAN_API
GType                             hyscan_actuator_info_get_type          (void);

HYSCAN_API
HyScanActuatorInfo *              hyscan_actuator_info_new               (HyScanDataSchema                 *schema);

HYSCAN_API
const gchar * const *             hyscan_actuator_info_list_actuators    (HyScanActuatorInfo               *info);

HYSCAN_API
const HyScanActuatorInfoActuator *hyscan_actuator_info_get_actuator      (HyScanActuatorInfo               *info,
                                                                          const gchar                      *name);

HYSCAN_API
HyScanActuatorInfoActuator *      hyscan_actuator_info_actuator_copy     (const HyScanActuatorInfoActuator *info);

HYSCAN_API
void                              hyscan_actuator_info_actuator_free     (HyScanActuatorInfoActuator       *info);

G_END_DECLS

#endif /* __HYSCAN_ACTUATOR_INFO_H__ */
