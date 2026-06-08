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

#ifndef VPI_EVENT_DEF_H
#define VPI_EVENT_DEF_H

#include <inttypes.h>

/**
 * @addtogroup EVENT_DEFINE
 * Event Definition
 * @ingroup VPI_EVENT
 * @{
 */

typedef enum BleStateId {
    BLE_STATE_DISCONNECTED = 0, /**< BLE is physical disconnected */
    BLE_STATE_CONNECTED,        /**< BLE is physical connected */
    BLE_STATE_ATTACHED,         /**< Connected, and attached with remote application */
    BLE_STATE_DETACHED,         /**< Connected, but detached from remote application */
    BLE_STATE_WEAK,             /**< Attached, but too weak to transfer */
} BleStateId;

typedef enum WokeUpReasonId {
    WOKE_UP_UNKNOWN = 0, /**< Source: None */
    WOKE_UP_COLD_BOOT,   /**< Source: None */
    WOKE_UP_PWR_KEY,     /**< Source: PMU - INT */
    WOKE_UP_FALL_DOWN,   /**< Source: IMU - INT */
    WOKE_UP_BLE_HS,      /**< Source: BLE - timer */
    WOKE_UP_BLE_USER,    /**< Source: BLE - App */
    WOKE_UP_USER_TOUCH,  /**< Source: IMU - INT */
    WOKE_UP_VITAL_TIMER, /**< Source: 32K timer */
} WokeUpReasonId;

typedef enum RebootStateId {
    MGR_RBT_RDY = 0, /**<Ready to reboot*/
    MGR_RBT_BUSY,    /**<Not Ready*/
} RebootStateId;

typedef enum PwrStateId {
    PWR_STATE_OFF,  /**< Power off, shutdown everything */
    PWR_STATE_ON,   /**< Power on */
    PWR_STATE_SLEEP /**< Sleep state, only IMU works */
} PwrStateId;

typedef enum PwrModeId {
    PWR_MODE_IDLE = 0, /**< Idle mode */
    PWR_MODE_SAMP,     /**< Sampling mode */
    PWR_MODE_TRANS,    /**< Transmission mode */
    PWR_MODE_ALGO,     /**< Algorithm mode */
    PWR_MODE_FLASH,    /**< Flash read/write mode */
    PWR_MODE_SLEEP,    /**< Sleep mode for LP */
    PWR_MODE_REBOOT,   /**<wait for reboot*/
} PwrModeId;

typedef enum OtaStateId {
    OTA_STATE_IDLE = 0,    /**< Idle state */
    OTA_STATE_STARTED,     /**< OTA is started */
    OTA_STATE_PROGRESS,    /**< OTA is on progress */
    OTA_STATE_PKG_DONE,    /**< OTA write a package done */
    OTA_STATE_FINISHED,    /**< OTA is finished */
    OTA_STATE_FLASH_ERR,   /**< OTA error when r/w flash */
    OTA_STATE_TIMEOUT_ERR, /**< OTA timeout */
    OTA_STATE_VERIFY_ERR,  /**< OTA verify error */
    OTA_STATE_SWITCH_ERR,  /**< Failed to switch image */
    OTA_STATE_ERR_END,     /**<*< End with error */
} OtaStateId;

typedef enum OtaActionId {
    OTA_ACT_NONE = 0, /**< None */
    OTA_ACT_START,    /**< Start OTA */
    OTA_ACT_PKG,      /**< Send Package */
    OTA_ACT_FINISH    /**< Finish OTA */
} OtaActionId;

typedef enum BatteryStateId {
    BAT_STATE_ERROR  = -1,    /**< Error happened */
    BAT_STATE_NORMAL = 0,     /**< Normal state */
    BAT_STATE_LOW,            /**< Common low state */
    BAT_STATE_LOW_FLASH_DOWN, /**< Low state for flash down */
    BAT_STATE_LOW_SYS_DOWN    /**< Low state for system down */
} BatteryStateId;

typedef enum SensorActionId {
    SEN_ACT_NONE = 0, /**< None */
    SEN_ACT_START,    /**< Start sensor */
    SEN_ACT_STOP,     /**< Stop sensor */
    SEN_ACT_PAUSE,    /**< Pause sensor */
    SEN_ACT_RESUME,   /**< Resume sensor */
    SEN_ACT_PWR_ON,   /**< Power on sensor */
    SEN_ACT_PWR_OFF,  /**< Power off sensor */
} SensorActionId;

typedef enum SensorStateId {
    SEN_STATE_IDLE = 0, /**< Idle */
    SEN_STATE_STARTED,  /**< Started */
    SEN_STATE_STOPPED,  /**< Stopped */
    SEN_STATE_PAUSED,   /**< Paused */
    SEN_STATE_RESUMED,  /**< Resumed */
    SEN_STATE_LOW_PWR,  /**< On low power */
    SEN_STATE_PWR_ON,   /**< Powered on */
    SEN_STATE_PWR_OFF,  /**< Powered off */
} SensorStateId;

typedef enum StorageActionId {
    STG_ACT_NONE = 0, /**< None */
    STG_ACT_READ,     /**< Read storage */
    STG_ACT_WRITE,    /**< Write storage */
    STG_ACT_ERASE,    /**< Erase storage */
} StorageActionId;

typedef enum UserWorkModeId {
    USER_MODE_NONE = 0, /**< None */
    USER_MODE_MOTION,   /**< only IMU works */
    USER_MODE_HR_RR,    /**< PPG green LED works for HR + RR */
    USER_MODE_SPO2,     /**< PPG LEDs work for HR + RR + SPO2 */
    USER_MODE_ECG,      /**< ECG sensor works */
    USER_MODE_TMP,      /**< TMP sensor works */
    USER_MODE_ALL,      /**< All except ECG */
} UserWorkModeId;

typedef struct EventAck_st {
    uint16_t id;  /**< manager_id, trans_id, action_id, state_id, reason_id */
    int8_t error; /**< EVENT_OK/EVENT_ERROR */
} EventAck;

typedef struct SensorRawData_st {
    uint16_t trans_id; /**< BLE transmission ID */
    uint32_t size;     /**< Size of data */
    uint8_t *data;     /**< Pointer of data need to be transferred or stored */
} SensorRawData;

typedef struct SensorDataPacket_st {
    EventAck ack;   /**< ack for sensor data transmission/storing */
    uint8_t action; /**< @see StorageActionId */
    SensorRawData rawdata;
} SensorDataPacket;

typedef struct OtaDataPacket_st {
    uint8_t action;  /**< @see OtaActionId  */
    uint32_t offset; /**< offset of firmware */
    uint32_t len;    /**< start:total length, pkg:package length */
    uint8_t *pdata;  /**< Pointer of data need to be transferred or stored */
} OtaDataPacket;

typedef struct PowerState_st {
    EventAck ack;          /**< ack for event */
    enum PwrModeId mode;   /**< target for requester, current for notifier */
    enum PwrStateId state; /**< new power state */
} PowerState;

/**
 * @brief common struct for events which defined by application
 */
typedef struct AppData_st {
    EventAck ack;  /**< ack for event */
    uint32_t size; /**< size of following data */
    void *data;    /**< pointer of data */
} AppData;

/** @} */

#endif // VPI_EVENT_DEF_H
