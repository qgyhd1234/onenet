/*
 * File      : onenet.h
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-04-24     chenyong     first version
 */
#ifndef _ONENET_H_
#define _ONENET_H_

#include <rtthread.h>

#define ONENET_DEBUG                   1

#define ONENET_SW_VERSION              "0.1.0"

#ifndef ONENET_MALLOC
#define ONENET_MALLOC                  rt_malloc
#endif

#ifndef ONENET_CALLOC
#define ONENET_CALLOC                  rt_calloc
#endif

#ifndef ONENET_FREE
#define ONENET_FREE                    rt_free
#endif

/* error level log */
#ifdef  log_e
#undef  log_e
#endif
#define log_e(...)                     rt_kprintf("\033[31;22m[E/ONENET] (%s:%d) ", __FUNCTION__, __LINE__);rt_kprintf(__VA_ARGS__);rt_kprintf("\033[0m\n")

/* info level log */
#ifdef  log_i
#undef  log_i
#endif
#define log_i(...)                     rt_kprintf("\033[36;22m[I/ONENET] ");                                rt_kprintf(__VA_ARGS__);rt_kprintf("\033[0m\n")

#if ONENET_DEBUG
#ifdef assert
#undef assert
#endif
#define assert(EXPR)                                                           \
if (!(EXPR))                                                                   \
{                                                                              \
    rt_kprintf("(%s) has assert failed at %s.\n", #EXPR, __FUNCTION__);        \
    while (1);                                                                 \
}

/* debug level log */
#ifdef  log_d
#undef  log_d
#endif
#define log_d(...)                     rt_kprintf("[D/ONENET] (%s:%d) ", __FUNCTION__, __LINE__);           rt_kprintf(__VA_ARGS__);rt_kprintf("\n")

#else

#ifdef assert
#undef assert
#endif
#define assert(EXPR)                   ((void)0);

/* debug level log */
#ifdef  log_d
#undef  log_d
#endif
#define log_d(...)
#endif /* ONENET_DEBUG */

#ifndef ONENET_MQTT_SUBTOPIC
#define ONENET_MQTT_SUBTOPIC           "/topic_test"
#endif

#if !defined(ONENET_INFO_DEVID) || !defined(ONENET_INFO_APIKEY) || !defined(ONENET_INFO_PROID) || !defined(ONENET_INFO_AUTH)
#define ONENET_INFO_DEVID              "29573339"
#define ONENET_INFO_APIKEY             "a2gVVf1hggZfuATkNogulHK1V=s="
#define ONENET_INFO_PROID              "131494"
#define ONENET_INFO_AUTH               "EF4016D6658466CA3E3606"
#endif 

#define ONENET_SERVER_URL              "tcp://183.230.40.39:6002"
#define ONENET_INFO_DEVID_LEN          16
#define ONENET_INFO_APIKEY_LEN         32
#define ONENET_INFO_PROID_LEN          16
#define ONENET_INFO_AUTH_LEN           64
#define ONENET_INFO_URL_LEN            32

struct rt_onenet_info
{
    char device_id[ONENET_INFO_DEVID_LEN];
    char api_key[ONENET_INFO_APIKEY_LEN];

    char pro_id[ONENET_INFO_PROID_LEN];
    char auth_info[ONENET_INFO_AUTH_LEN];

    char server_uri[ONENET_INFO_URL_LEN];

};
typedef struct rt_onenet_info *rt_onenet_info_t;

/* OneNET MQTT initialize. */
int onenet_mqtt_init(void);

/* Publish MQTT data to subscribe topic. */
int onenet_mqtt_publish(const char *topic, const char *msg_str);

/* Device send data to OneNET cloud. */
rt_err_t onenet_http_upload_digit(const char *name, double digit);
rt_err_t onenet_http_upload_string(const char *name, char *str);

/* ========================== User port function ============================ */

/* Get MQTT data from OneNET cloud and process data. */
int onenet_port_data_process(char *recv_data, rt_size_t size);

#endif /* _ONENET_H_ */
