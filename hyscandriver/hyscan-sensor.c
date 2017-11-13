/*
 * \file hyscan-sensor.c
 *
 * \brief Исходный файл интерфейса управления датчиками
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2017
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-sensor.h"
#include "hyscan-driver-marshallers.h"

G_DEFINE_INTERFACE (HyScanSensor, hyscan_sensor, G_TYPE_OBJECT)

static void
hyscan_sensor_default_init (HyScanSensorInterface *iface)
{
  g_signal_new ("sensor-data", HYSCAN_TYPE_SENSOR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__POINTER_INT64_INT_UINT_POINTER,
                G_TYPE_NONE, 5, G_TYPE_POINTER, G_TYPE_INT64, G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  g_signal_new ("sensor-log", HYSCAN_TYPE_SENSOR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__POINTER_INT64_INT_POINTER,
                G_TYPE_NONE, 4, G_TYPE_POINTER, G_TYPE_INT64, G_TYPE_INT, G_TYPE_POINTER);
}

gboolean
hyscan_sensor_set_sound_velocity (HyScanSensor        *sensor,
                                  HyScanSoundVelocity *sound_velocity,
                                  guint32              n_points)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_sound_velocity != NULL)
    return (* iface->set_sound_velocity) (sensor, sound_velocity, n_points);

  return FALSE;
}

gboolean
hyscan_sensor_set_channel (HyScanSensor *sensor,
                           const gchar  *name,
                           guint         channel)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_channel != NULL)
    return (* iface->set_channel) (sensor, name, channel);

  return FALSE;
}

gboolean
hyscan_sensor_set_latency (HyScanSensor *sensor,
                           const gchar  *name,
                           gint64        latency)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_latency != NULL)
    return (* iface->set_latency) (sensor, name, latency);

  return FALSE;
}

gboolean
hyscan_sensor_set_enable (HyScanSensor *sensor,
                          const gchar  *name,
                          gboolean      enable)
{
  HyScanSensorInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SENSOR (sensor), FALSE);

  iface = HYSCAN_SENSOR_GET_IFACE (sensor);
  if (iface->set_enable != NULL)
    return (* iface->set_enable) (sensor, name, enable);

  return FALSE;
}
