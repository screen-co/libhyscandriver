/* hyscan-sonar-driver.c
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

/**
 * SECTION: hyscan-sonar-driver
 * @Short_description: вспомогательные функции для интерфейса #HyScanSonar
 * @Title: HyScanSonarDriver
 *
 * Функции предназначены для отправки сигналов интерфейса #HyScanSonar.
 * Эти функции предназначены для использования в драйверах устройств.
 */

#include "hyscan-sonar-driver.h"

/*
 * hyscan_sonar_driver_send_signal:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @channel: индекс канала данных
 * @description: (nullable): описание источника данных
 * @actuator: (nullable): описание источника данных
 * @info: параметры данных #HyScanAcousticDataInfo
 *
 * Функция отправляет сигнал #HyScanSonar::sonar-source-info.
 */
void
hyscan_sonar_driver_send_source_info (gpointer                sonar,
                                      HyScanSourceType        source,
                                      guint                   channel,
                                      const gchar            *description,
                                      const gchar            *actuator,
                                      HyScanAcousticDataInfo *info)
{
  g_return_if_fail (HYSCAN_IS_SONAR (sonar));

  g_signal_emit_by_name (sonar, "sonar-source-info", (gint)source, channel, description, actuator, info);
}

/*
 * hyscan_sonar_driver_send_signal:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @channel: индекс канала данных
 * @time: время начала действия сигнала, мкс
 * @image: (nullable): образ сигнала #HyScanBuffer
 *
 * Функция отправляет сигнал #HyScanSonar::sonar-signal.
 */
void
hyscan_sonar_driver_send_signal (gpointer          sonar,
                                 HyScanSourceType  source,
                                 guint             channel,
                                 gint64            time,
                                 HyScanBuffer     *image)
{
  g_return_if_fail (HYSCAN_IS_SONAR (sonar));

  g_signal_emit_by_name (sonar, "sonar-signal", (gint)source, channel, time, image);
}

/*
 * hyscan_sonar_driver_send_signal:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @channel: индекс канала данных
 * @time: время начала действия параметров ВАРУ, мкс
 * @gains: коэффициенты усиления #HyScanBuffer
 *
 * Функция отправляет сигнал #HyScanSonar::sonar-tvg.
 */
void
hyscan_sonar_driver_send_tvg (gpointer          sonar,
                              HyScanSourceType  source,
                              guint             channel,
                              gint64            time,
                              HyScanBuffer     *gains)
{
  g_return_if_fail (HYSCAN_IS_SONAR (sonar));

  g_signal_emit_by_name (sonar, "sonar-tvg", (gint)source, channel, time, gains);
}

/*
 * hyscan_sonar_driver_send_acoustic_data:
 * @sonar: указатель на #HyScanSonar
 * @source: идентификатор источника данных #HyScanSourceType
 * @channel: индекс канала данных
 * @noise: признак данных шума (выключенное излучение)
 * @time: время приёма данных, мкс
 * @data: данные #HyScanBuffer
 *
 * Функция отправляет сигнал #HyScanSonar::sonar-acoustic-data.
 */
void
hyscan_sonar_driver_send_acoustic_data (gpointer                sonar,
                                        HyScanSourceType        source,
                                        guint                   channel,
                                        gboolean                noise,
                                        gint64                  time,
                                        HyScanBuffer           *data)
{
  g_return_if_fail (HYSCAN_IS_SONAR (sonar));

  g_signal_emit_by_name (sonar, "sonar-acoustic-data", (gint)source, channel, noise, time, data);
}
