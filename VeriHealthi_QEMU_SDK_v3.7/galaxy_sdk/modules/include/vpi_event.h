/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 *
 * 1. Redistributing the source code of this software is only allowed after
 * receiving explicit, written permission from VeriSilicon. The copyright notice,
 * this list of conditions and the following disclaimer must be retained in all
 * source code distributions.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 */

#ifndef VPI_EVENT_H
#define VPI_EVENT_H

#include <inttypes.h>
#include "vs_conf.h"

/**
 * @addtogroup VPI_EVENT
 * Event system
 * @ingroup MODULE_VPI
 * @{
 */

/**
 * @brief Event manager data type
 */
typedef void *EventManager;

/**
 * @brief Event group data type
 */
typedef void *EventGroup;

/**
 * @brief Event parameters data type
 */
typedef void *EventParam;

#define EVENT_OK    0  /**< Event OK */
#define EVENT_ERROR -1 /**< Event generic error code */

/**
 * @enum EventManagerId
 * @brief Predefined managers
 */
typedef enum EventManagerId {
    EVENT_MGR_NULL = 0,
    EVENT_MGR_SYS,     /**< System manager */
    EVENT_MGR_BLE,     /**< BLE manager */
    EVENT_MGR_PWR,     /**< Power manager */
    EVENT_MGR_BAT,     /**< Battery manager */
    EVENT_MGR_SEN,     /**< Sensor manager */
    EVENT_MGR_OTA,     /**< OTA manager */
    EVENT_MGR_STG,     /**< Storage manager */
    EVENT_MGR_AUD,     /**< Audio manager */
    EVENT_MGR_IPC,     /**< IPC manager */
    EVENT_MGR_KEY,     /**< Key manager */
    EVENT_MGR_ALGO,    /**< Algorithm manager */
    EVENT_MGR_RES,     /**< Result manager */
    EVENT_MGR_USB,     /**< USB manager */
    EVENT_MGR_DISPLAY, /**< Display manager */
    EVENT_MGR_CUSTOM,  /**< Custom manager */
    EVENT_MGR_NBIOT,   /**< NBIOT manager */
} EventManagerId;

/**
 * @enum EventId
 * @brief Predefined events
 */
typedef enum EventId {
    EVENT_INVALID = 0,
    EVENT_BAT_LEVEL,           /**< Battery level event */
    EVENT_CHARGER_UPDATE,      /**< Charger update event */
    EVENT_BLE_TRAN_REQ,        /**< BLE transfer request event */
    EVENT_BLE_TRANSFERRED,     /**< BLE packet transferred event */
    EVENT_BLE_COMMON_TRAN_REQ, /**< BLE common transfer request */
    EVENT_BLE_MTU,             /**< BLE MTU change event */
    EVENT_AUD_PARAM,           /**< Audio parameter event */
    EVENT_AUD_PCM_DATA,        /**< Audio pcm data event */
    EVENT_AUD_ENC_DATA,        /**< Audio encoded data event */
    EVENT_IPC_MESSAGE,         /**< ipc message event */
    EVENT_CMD_UPDATE,          /**< Cmd update event */
    EVENT_KEY_INPUT,           /**< Key input event */
    EVENT_OTA_ACTION_REQ,      /**< OTA action request event */
    EVENT_PWR_MODE,            /**< Power mode event */
    EVENT_PWR_MODE_REQ,        /**< Power mode request event */
    EVENT_SEN_DATA_READY,      /**< Sensor data ready event */
    EVENT_SEN_COMMON,          /**< Common event for sensor */
    EVENT_SEN_SW_TIMEOUT,      /**< Sensor SW timer timeout event */
    EVENT_SEN_IMU_TAP,         /**< IMU tap event */
    EVENT_SEN_IRQ,             /**< Sensor interrupt event */
    EVENT_ALGO_SW_TIMEOUT,     /**< Algorithm SW timer timeout event */
    EVENT_ALGO_RES,            /**< Algorithm result data event */
    EVENT_SYS_PING,            /**< System ping event */
    EVENT_SYS_STATE_UPDATE,    /**< System state update event */
    EVENT_SYS_REBOOT_START,    /**< System reboot start event */
    EVENT_SYS_REBOOT_REQ,      /**< System reboot request event */
    EVENT_SYS_REBOOT_STATE,    /**< System reboot state event */
    EVENT_SYS_WOKE_UP,         /**< System wake up event */
    EVENT_USER_INFO,           /**< User info event */
    EVENT_USER_SEDENTARY_CFG,  /**< Sedentary config event */
    EVENT_USER_PPG_BP_HIS,     /**< PPG blood pressure history event */
    EVENT_USER_PPG_BP_CALIB,   /**< PPG blood pressure calibration event */
    EVENT_USER_PPG_BP_CALCU,   /**< PPG blood pressure calculation event */
    EVENT_DISPLAY_INFO_UPDATE, /**< Display info update event */
    EVENT_NBIOT_TRAN_REQ,      /**< NBIOT transfer msg request event */
    EVENT_NBIOT_TRANSFERRED,   /**< NBIOT packet transferred event */
    EVENT_NBIOT_RECEIVE_PROC,  /**< NBIOT receive data process event */
    EVENT_NBIOT_SW_TIMEOUT,    /**< NBIOT SW timer timeout event */
    EVENT_SYS_TEST,            /**< System test event */
    EVENT_SDK_END,             /**< Last one on SDK side */
} EventId;

/**
 * @brief Event handler definition
 * @param manager The pointer of manager
 * @param event_id The event id sent by vpi_event_notify needs to be handled, @see EventId
 * @param param The param pointer sent by vpi_event_notify
 */
typedef int (*EventHandler)(EventManager manager, EventId event_id, EventParam param);

/**
 * @brief Set event queue size
 * @note Initialize event queue size for the event system(must be called before calling the
 * vpi_event_register, the event queue size is 10 by default)
 * @param size size of each event queue
 * @return EVENT_OK on success, EVENT_ERROR when fail
 */
int vpi_event_set_queue_size(uint32_t size);

/**
 * @brief Create new event manager.
 * @note We need to create a manager to handle event system
 * @param manager_id manager ID, refer to @see EventManagerId
 * @param handler Event handler of manager.
 * @return Manager Handler
 */
EventManager vpi_event_new_manager(EventManagerId manager_id, EventHandler handler);

/**
 * @brief Create new event manager group
 * @note Generally, each manager runs in one task. If you want to reduce the number of tasks and
 * lower RAM consumption, you can use the group function to put multiple managers in one task.
 * @return Event Group Handler
 */
EventGroup vpi_event_new_group(void);

/**
 * @brief Add event manager to group
 * @note You can add multiple managers in a group and they will run in one task.
 * @param manager Event manager
 * @param group Event manager group
 * @return EVENT_OK on success, EVENT_ERROR when fail
 */
int vpi_event_add_to_group(EventManager manager, EventGroup group);

/**
 * @brief Register manager to event.
 * @note Manager need to register the event they care about
 * @param event_id Event id, @see EventId
 * @param manager Event manager
 * @return EVENT_OK on success, EVENT_ERROR when fail
 */
int vpi_event_register(EventId event_id, EventManager manager);

/**
 * @brief Unregister manager from event.
 * @note After registration, if you no longer care about an event, you can unregister.
 * @param event_id Event id, @see EventId
 * @param manager Event manager
 * @return EVENT_OK on success, EVENT_ERROR when fail
 */
int vpi_event_unregister(EventId event_id, EventManager manager);

/**
 * @brief Event Manager or event manager group start to listen to the event
 * @note This is a blocking function that needs to be called all the time in a task. When a manager
 * or manager group monitors an event sent by vpi_event_notify, the callback function( @see
 * EventHandler) in the vpi_event_new_manager parameter will be called.
 * @param manager_or_group Event manager or event manager group @see EventManager or @see EventGroup
 * @return EVENT_OK on success, EVENT_ERROR when fail
 */
int vpi_event_listen(void *manager_or_group);

/**
 * @brief Notify all managers the event
 * @note This is a non-blocking function that can send a message to all managers that care about the
 * current event id. This function must be called in a task.
 * @param event_id Event id, @see EventId
 * @param param Event parameter
 * @return EVENT_OK on success, EVENT_ERROR when fail
 */
int vpi_event_notify(EventId event_id, EventParam param);

/**
 * @brief Notify all managers the event in isr
 * @note This function is used to send messages in interrupt
 * @param event_id Event id, @see EventId
 * @param param Event parameter
 * @return EVENT_OK on success, EVENT_ERROR when fail
 */
int vpi_event_notify_from_isr(EventId event_id, EventParam param);

/**
 * @brief Get event name, easy for debug
 * @param event_id Event id, @see EventId
 * @return event name
 */
char *vpi_get_event_name(EventId event_id);

/**
 * @brief Get manager name, easy for debug
 * @param manager_id Manager id, @see EventManagerId
 * @return manager name
 */
const char *vpi_get_manager_name(EventManagerId manager_id);

/**
 * @brief Get manager ID
 * @param manager Manager pointer
 * @return manager ID
 */
EventManagerId vpi_get_manager_id(EventManager manager);

/** @} */

#endif // VPI_EVENT_H
