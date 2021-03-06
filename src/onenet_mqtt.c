/*
 * File      : onenet_mqtt.c
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
#include <stdlib.h>
#include <string.h>

#include <paho_mqtt.h>

#include <onenet.h>

static rt_bool_t init_ok = RT_FALSE;
static MQTTClient mq_client;
struct rt_onenet_info onenet_info;

static void mqtt_callback(MQTTClient *c, MessageData *msg_data)
{
    onenet_port_data_process((char *)msg_data->message->payload, msg_data->message->payloadlen);
}

static void mqtt_connect_callback(MQTTClient *c)
{
    log_d("Enter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    log_d("Enter mqtt_online_callback!");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    log_d("Enter mqtt_offline_callback!");
}

static rt_err_t onenet_mqtt_entry(void)
{
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;

    mq_client.uri = onenet_info.server_uri;
    memcpy(&(mq_client.condata), &condata, sizeof(condata));
    mq_client.condata.clientID.cstring = onenet_info.device_id;
    mq_client.condata.keepAliveInterval = 30;
    mq_client.condata.cleansession = 1;
    mq_client.condata.username.cstring = onenet_info.pro_id;
    mq_client.condata.password.cstring = onenet_info.auth_info;

    mq_client.buf_size = mq_client.readbuf_size = 1024 * 2;
    mq_client.buf = ONENET_CALLOC(1, mq_client.buf_size);
    mq_client.readbuf = ONENET_CALLOC(1, mq_client.readbuf_size);
    if (!(mq_client.buf && mq_client.readbuf))
    {
        log_e("No memory for MQTT client buffer!");
        return -RT_ENOMEM;
    }

    /* registered callback */
    mq_client.connect_callback = mqtt_connect_callback;
    mq_client.online_callback = mqtt_online_callback;
    mq_client.offline_callback = mqtt_offline_callback;

    /* set subscribe table. */
    mq_client.messageHandlers[0].topicFilter = ONENET_MQTT_SUBTOPIC;
    mq_client.messageHandlers[0].callback = mqtt_callback;

    mq_client.defaultMessageHandler = mqtt_callback;

    paho_mqtt_start(&mq_client);
    
    return RT_EOK;
}

static void onenet_get_info(void)
{
    strncpy(onenet_info.device_id, ONENET_INFO_DEVID, strlen(ONENET_INFO_DEVID));
    strncpy(onenet_info.api_key, ONENET_INFO_APIKEY, strlen(ONENET_INFO_APIKEY));
    strncpy(onenet_info.pro_id, ONENET_INFO_PROID, strlen(ONENET_INFO_PROID));
    strncpy(onenet_info.auth_info, ONENET_INFO_AUTH, strlen(ONENET_INFO_AUTH));
    strncpy(onenet_info.server_uri, ONENET_SERVER_URL, strlen(ONENET_SERVER_URL));  
} 

int onenet_mqtt_init(void)
{
    int result = 0;
    
    if (init_ok)
    {
        return 0;
    }
    
    onenet_get_info();
    
    if(onenet_mqtt_entry() < 0)
    {
        result = -1;
    }

    if(!result)
    {
        init_ok = RT_TRUE;
        log_i("OneNET MQTT initialize success.");
    }
    else
    {
        log_e("OneNET MQTT initialize failed(%d).", result);
    }

    return result;
}

int onenet_mqtt_publish(const char *topic, const char *msg_str)
{
    MQTTMessage message;
    message.qos = 1;
    message.retained = 0;
    message.payload = (void*) msg_str;
    message.payloadlen = strlen(message.payload);

    if (MQTTPublish(&mq_client, topic, &message) < 0)
    {
        return -1;
    }

    return 0;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>

int onenet_publish(int argc, char **argv)
{
    if (argc != 3)
    {
        log_e("onenet_publish [topic] [message]    -OneNET mqtt pulish message to this topic.\n");
        return 0;
    }

    onenet_mqtt_publish(argv[1], argv[2]);

    return 0;
}
MSH_CMD_EXPORT(onenet_mqtt_init, OneNET cloud mqtt initializate);
MSH_CMD_EXPORT_ALIAS(onenet_publish, onenet_mqtt_publish, OneNET cloud send data to subscribe topic);
#endif

