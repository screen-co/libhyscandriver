/*
 * \file hyscan-sonar.c
 *
 * \brief Исходный файл интерфейса управления гидролокатором
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2017
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-sonar.h"
#include "hyscan-driver-marshallers.h"

G_DEFINE_INTERFACE (HyScanSonar, hyscan_sonar, G_TYPE_OBJECT)

static void
hyscan_sonar_default_init (HyScanSonarInterface *iface)
{
  g_signal_new ("sonar-signal", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_UINT_POINTER,
                G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT64, G_TYPE_UINT, G_TYPE_POINTER);

  g_signal_new ("sonar-tvg", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_UINT_POINTER,
                G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT64, G_TYPE_UINT, G_TYPE_POINTER);

  g_signal_new ("sonar-noise-data", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_UINT_INT64_INT_UINT_POINTER,
                G_TYPE_NONE, 6, G_TYPE_INT, G_TYPE_UINT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  g_signal_new ("sonar-raw-data", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_UINT_INT64_INT_UINT_POINTER,
                G_TYPE_NONE, 6, G_TYPE_INT, G_TYPE_UINT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  g_signal_new ("sonar-acoustic-data", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_INT_UINT_POINTER,
                G_TYPE_NONE, 5, G_TYPE_INT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  g_signal_new ("sonar-log", HYSCAN_TYPE_SONAR, G_SIGNAL_RUN_LAST, 0,
                NULL, NULL,
                hyscan_driver_marshal_VOID__INT_INT64_INT_POINTER,
                G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT64, G_TYPE_INT, G_TYPE_POINTER);
}

gboolean
hyscan_sonar_set_sync_type (HyScanSonar         *sonar,
                            HyScanSonarSyncType  sync_type)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->set_sync_type != NULL)
    return (* iface->set_sync_type) (sonar, sync_type);

  return FALSE;
}

gboolean
hyscan_sonar_receiver_set_time (HyScanSonar      *sonar,
                                HyScanSourceType  source,
                                gdouble           receive_time)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->receiver_set_time != NULL)
    return (* iface->receiver_set_time) (sonar, source, receive_time);

  return FALSE;
}

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

gboolean
hyscan_sonar_receiver_set_enable (HyScanSonar      *sonar,
                                  HyScanSourceType  source,
                                  gboolean          enable)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->receiver_set_enable != NULL)
    return (* iface->receiver_set_enable) (sonar, source, enable);

  return FALSE;
}

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

gboolean
hyscan_sonar_generator_set_auto (HyScanSonar                    *sonar,
                                 HyScanSourceType                source,
                                 HyScanSonarGeneratorSignalType  signal)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_auto != NULL)
    return (* iface->generator_set_auto) (sonar, source, signal);

  return FALSE;
}

gboolean
hyscan_sonar_generator_set_simple (HyScanSonar                    *sonar,
                                   HyScanSourceType                source,
                                   HyScanSonarGeneratorSignalType  signal,
                                   gdouble                         power)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_simple != NULL)
    return (* iface->generator_set_simple) (sonar, source, signal, power);

  return FALSE;
}

gboolean
hyscan_sonar_generator_set_extended (HyScanSonar                    *sonar,
                                     HyScanSourceType                source,
                                     HyScanSonarGeneratorSignalType  signal,
                                     gdouble                         duration,
                                     gdouble                         power)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_extended != NULL)
    return (* iface->generator_set_extended) (sonar, source, signal, duration, power);

  return FALSE;
}

gboolean
hyscan_sonar_generator_set_enable (HyScanSonar      *sonar,
                                   HyScanSourceType  source,
                                   gboolean          enable)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->generator_set_enable != NULL)
    return (* iface->generator_set_enable) (sonar, source, enable);

  return FALSE;
}

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

gboolean
hyscan_sonar_tvg_set_points (HyScanSonar      *sonar,
                             HyScanSourceType  source,
                             gdouble           time_step,
                             gdouble          *gains,
                             guint32           n_gains)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_points != NULL)
    return (* iface->tvg_set_points) (sonar, source, time_step, gains, n_gains);

  return FALSE;
}

gboolean
hyscan_sonar_tvg_set_linear_db (HyScanSonar      *sonar,
                                HyScanSourceType  source,
                                gdouble           gain0,
                                gdouble           gain_step)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_linear_db != NULL)
    return (* iface->tvg_set_linear_db) (sonar, source, gain0, gain_step);

  return FALSE;
}

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

gboolean
hyscan_sonar_tvg_set_enable (HyScanSonar      *sonar,
                             HyScanSourceType  source,
                             gboolean          enable)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->tvg_set_enable != NULL)
    return (* iface->tvg_set_enable) (sonar, source, enable);

  return FALSE;
}

gboolean
hyscan_sonar_start (HyScanSonar     *sonar,
                    const gchar     *track_name,
                    HyScanTrackType  track_type)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->start != NULL)
    return (* iface->start) (sonar, track_name, track_type);

  return FALSE;
}

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

gboolean
hyscan_sonar_ping (HyScanSonar *sonar)
{
  HyScanSonarInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_SONAR (sonar), FALSE);

  iface = HYSCAN_SONAR_GET_IFACE (sonar);
  if (iface->ping != NULL)
    return (* iface->ping) (sonar);

  return FALSE;
}
