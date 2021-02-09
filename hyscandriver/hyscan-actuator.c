/* hyscan-actuator.c
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

/**
 * SECTION: hyscan-actuator
 * @Short_description: интерфейс управления приводами
 * @Title: HyScanActuator
 *
 * Привод обеспечивает вращение антенны или всего гидролокатора в процессе
 * съёмки. Данный интерсфейс позволяет управлять параметрами и режимом работы
 * привода, такими как диапазон углов сканирования и скоростью вращения.
 *
 * Возможны следующие режимы работы привода:
 *
 * - #HYSCAN_ACTUATOR_MODE_SCAN - сканирование в заданном секторе;
 * - #HYSCAN_ACTUATOR_MODE_MANUAL - ручная установка положения.
 *
 * В режиме сканирования в заданном секторе, привод будет обеспечивать
 * автоматическое вращение в заданном диапазоне углов с указанной скоростью.
 * Включение этого режима осуществляется функцией #hyscan_actuator_scan.
 *
 * В ручном режиме, пользователь может указать конкретный угол, в который
 * привод повернёт установленное на него устройство (гидролокатор). Выбор
 * угла в ручном режиме осуществляется функцией #hyscan_actuator_manual.
 *
 * Обычно, начало вращения осуществляется одновременно с переводом локатора
 * в рабочий режим (#hyscan_sonar_start) и завершается при остановке
 * (#hyscan_sonar_stop). Но это поведение зависит от самого привода и драйвера
 * устройства. Если в процессе работы привода требуется изменить текущие
 * параметры, это можно сделать в любой момент. Но для их применения
 * необходимо вызвать функцию #hyscan_device_sync.
 *
 * При отключении управления приводом функцией #hyscan_actuator_disable, привод
 * автоматически повернётся в начальное положение (режим парковки).
 */

#include "hyscan-actuator.h"

G_DEFINE_INTERFACE (HyScanActuator, hyscan_actuator, G_TYPE_OBJECT)

static void
hyscan_actuator_default_init (HyScanActuatorInterface *iface)
{
}

/**
 * hyscan_actuator_disable:
 * @actuator: указатель на #HyScanActuator
 * @name: название привода
 *
 * Функция отключает управление приводом.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_actuator_disable (HyScanActuator *actuator,
                         const gchar    *name)
{
  HyScanActuatorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_ACTUATOR (actuator), FALSE);

  iface = HYSCAN_ACTUATOR_GET_IFACE (actuator);
  if (iface->disable != NULL)
    return (* iface->disable) (actuator, name);

  return FALSE;
}

/**
 * hyscan_actuator_scan:
 * @actuator: указатель на #HyScanActuator
 * @name: название привода
 * @from: начальный угол сектора обзора, десятичный градус
 * @to: конечный угол сектора обзора, десятичный градус
 * @speed: скорость вращения привода, десятичный градус/с
 *
 * Функция включает режима сканирования в заданном секторе обзора.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_actuator_scan (HyScanActuator *actuator,
                      const gchar    *name,
                      gdouble         from,
                      gdouble         to,
                      gdouble         speed)
{
  HyScanActuatorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_ACTUATOR (actuator), FALSE);

  iface = HYSCAN_ACTUATOR_GET_IFACE (actuator);
  if (iface->scan != NULL)
    return (* iface->scan) (actuator, name, from, to, speed);

  return FALSE;
}

/**
 * hyscan_actuator_manual:
 * @actuator: указатель на #HyScanActuator
 * @name: название привода
 * @angle: угол направления привода, десятичный градус
 *
 * Функция включает режима ручного выбора угла направления привода.
 *
 * Returns: %TRUE если команда выполнена успешно, иначе %FALSE.
 */
gboolean
hyscan_actuator_manual (HyScanActuator *actuator,
                        const gchar    *name,
                        gdouble         angle)
{
  HyScanActuatorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_ACTUATOR (actuator), FALSE);

  iface = HYSCAN_ACTUATOR_GET_IFACE (actuator);
  if (iface->manual != NULL)
    return (* iface->manual) (actuator, name, angle);

  return FALSE;
}
