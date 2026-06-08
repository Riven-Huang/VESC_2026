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

#ifndef HAL_IMU_H
#define HAL_IMU_H

#include <stdbool.h>
#include "hal_bus.h"
#include "base_device.h"

/**
 * @addtogroup HAL_IMU
 * IMU HAL API and definition
 * @ingroup DRIVER_HAL
 * Hardware Abstraction Layer
 * @{
 */

/**
 * @brief gpio struct declaration
 */
typedef struct GpioPort_st GpioPort;

#define FIFO_BUFF_SIZE       (255)  /* In byte, used by driver */
#define FIFO_WATERMARK_LEVEL (49)

/**
 * @brief Definition for IMU device ID
 */
typedef enum ImuDeviceId {
    IMU_DEV_ID_0 = 0,
    IMU_DEV_ID_MAX,
} ImuDeviceId;

typedef void (*ImuDataReadyHandler)(void);

/**
 * @brief FIFO type
 */
typedef enum ImuFifoType {
    IMU_FIFO_GYRO     = 0x80, /**< Gyro data */
    IMU_FIFO_ACCEL    = 0x40, /**< Accel data */
    IMU_FIFO_AUX      = 0x20, /**< Aux data */
    IMU_FIFO_TAG_IRQ1 = 0x08, /**< FIFO tag for INT1 pin */
    IMU_FIFO_TAG_IRQ2 = 0x04, /**< FIFO tag for INT2 pin*/
    IMU_FIFO_TIME     = 0x02, /**< Sensor time frame */
    IMU_FIFO_HEADER   = 0x10, /**< Header */
} ImuFifoType;

/**
 * @brief IMU sensor type
 */
typedef enum ImuSensorType {
    IMU_ACCEL = 1,  /**< Accel sensor */
    IMU_GYRO,       /**< Gyro sensor*/
    IMU_ACCEL_GYRO, /**< Accel and gyro sensors */
    IMU_MAG,        /**< Magnet sensor */
    IMU_ALL,        /**< All sensor */
} ImuSensorType;

/**
 * @brief IMU sensor configuration type
 */
typedef enum ImuSensorCfgType {
    IMU_SENSOR_RANGE = 0x01, /**< Sensor range */
    IMU_SENSOR_ODR   = 0x02, /**< Sensor output dadta rate */
    IMU_SENSOR_BWP   = 0x04, /**< Sensor bandwidth parameter */
} ImuSensorCfgType;

/**
 * @brief IMU pin type
 */
typedef enum ImuPinType {
    IMU_DATA_PIN = 1, /**< Data pin */
    IMU_WAKE_PIN,     /**< Wake pin */
    IMU_POWER_PIN,    /**< Power pin */
} ImuPinType;

/**
 * @brief IMU bit value: 0-diable, 1-enable
 */
typedef enum ImuBitValue {
    IMU_DISABLE = 0x00, /**< Disable */
    IMU_ENABLE  = 0x01, /**< Enable */
} ImuBitValue;

/**
 * @brief Latch Duration
 */
typedef enum ImmLatchDuration {
    IMU_LATCH_DUR_NONE            = 0x00, /**< Non-latched */
    IMU_LATCH_DUR_312_5_MICRO_SEC = 0x01, /**< Temporary, 312.5us */
    IMU_LATCH_DUR_625_MICRO_SEC   = 0x02, /**< Temporary, 625us */
    IMU_LATCH_DUR_1_25_MILLI_SEC  = 0x03, /**< Temporary, 1.25ms */
    IMU_LATCH_DUR_2_5_MILLI_SEC   = 0x04, /**< Temporary, 2.5ms */
    IMU_LATCH_DUR_5_MILLI_SEC     = 0x05, /**< Temporary, 5ms */
    IMU_LATCH_DUR_10_MILLI_SEC    = 0x06, /**< Temporary, 10ms */
    IMU_LATCH_DUR_20_MILLI_SEC    = 0x07, /**< Temporary, 20ms */
    IMU_LATCH_DUR_40_MILLI_SEC    = 0x08, /**< Temporary, 40ms */
    IMU_LATCH_DUR_80_MILLI_SEC    = 0x09, /**< Temporary, 80ms */
    IMU_LATCH_DUR_160_MILLI_SEC   = 0x0A, /**< Temporary, 160ms */
    IMU_LATCH_DUR_320_MILLI_SEC   = 0x0B, /**< Temporary, 320ms */
    IMU_LATCH_DUR_640_MILLI_SEC   = 0x0C, /**< Temporary, 640ms */
    IMU_LATCH_DUR_1_28_SEC        = 0x0D, /**< Temporary, 1.28s */
    IMU_LATCH_DUR_2_56_SEC        = 0x0E, /**< Temporary, 2.56s */
    IMU_LATCHED                   = 0x0F, /**< Latched */
} ImmLatchDuration;

/**
 * @brief Definition of interrupt channel selection
 */
typedef enum ImuIntrChannel {
    IMU_INTERRUPT_CHANNEL_NONE, /**< Un-map both channels */
    IMU_INTERRUPT_CHANNEL_1,    /**< Interrupt Channel 1 */
    IMU_INTERRUPT_CHANNEL_2,    /**< Interrupt Channel 2 */
    IMU_INTERRUPT_CHANNEL_BOTH, /**< Map both channels */
} ImuIntrChannel;

/**
 * @brief Definition of interrupt types
 */
typedef enum ImuIntrType {
    IMU_ACC_ANY_MOTION_INTERRUPT,          /**< Slope/Any-motion interrupt */
    IMU_ACC_SIG_MOTION_INTERRUPT,          /**< Significant motion interrupt */
    IMU_STEP_DETECT_INTERRUPT,             /**< Step detector interrupt */
    IMU_ACC_DOUBLE_TAP_INTERRUPT,          /**< Double tap interrupt */
    IMU_ACC_SINGLE_TAP_INTERRUPT,          /**< Single tap interrupt */
    IMU_ACC_ORIENT_INTERRUPT,              /**< Orientation interrupt */
    IMU_ACC_FLAT_INTERRUPT,                /**< Flat interrupt */
    IMU_ACC_HIGH_G_INTERRUPT,              /**< High-g interrupt */
    IMU_ACC_LOW_G_INTERRUPT,               /**< Low-g interrupt */
    IMU_ACC_SLOW_NO_MOTION_INTERRUPT,      /**< Slow/no-motion interrupt */
    IMU_ACC_GYRO_DATA_RDY_INTERRUPT,       /**< Data ready interrupt */
    IMU_ACC_GYRO_FIFO_FULL_INTERRUPT,      /**< FIFO full interrupt */
    IMU_ACC_GYRO_FIFO_WATERMARK_INTERRUPT, /**< FIFO watermark interrupt */
    IMU_FIFO_TAG_INTERRUPT_PIN,            /**< FIFO tagging feature support */
} ImuIntrType;

/**
 * @brief IMU sensor work modes
 */
typedef enum ImuSensorMode {
    IMU_SEN_MODE_OFF = 0, /**< Stop mode */
    IMU_SEN_MODE_IDLE,    /**< sensor register is initialized but not started */
    IMU_SEN_MODE_LOW_PWR, /**< Low power mode */
    IMU_SEN_MODE_NORMAL,  /**< Normal work mode */
} ImuSensorMode;

/**
 * @brief Struct of high interrupt condition
 * @note Can use `hal_imu_get_accel_range()` to get the range of accel.
 * The interrupt condition is cleared when the absolute value of acceleration
 * data of all selected axes falls below the threshold [threshold] minus the
 * hysteresis [hysteresis] or if the sign of the acceleration value changes
 */
typedef struct IMUHighGCondition_st {
    /**
     * Threshold of high-g interrupt according to
     * [threshold]*7.81mg (2g range), or [threshold]*15.63mg (4g range),
     * or [threshold]*31.25mg (8g range), or [threshold]*62.5mg (16g range)
     */
    uint8_t threshold;
    /**
     * High-g range interrupt trigger delay according to
     * [duration]*2.5ms in a range from 2.5 to 640ms
     */
    uint8_t duration;
    /**
     * Range is 0 to 3. hysteresis of high-g interrupt according
     * to [hysteresis]*125mg (2g range), or [hysteresis]*250mg (4g range), or
     * [hysteresis]*500mg (8g range), or [hysteresis]*1000mg (16g range)
     */
    uint8_t hysteresis;
} IMUHighGCondition;

/**
 * @brief Interrupt pin setting
 */
typedef struct IMUInterruptPinSetting_st {
    /** To enable either INT1 or INT2 pin as output.
     * 0- output disabled ,1- output enabled */
    uint16_t output_en : 1;

    /** 0 - Push-pull 1- open drain,only valid if output_en is set 1 */
    uint16_t output_mode : 1;

    /** 0 - Active low , 1 - active high level.
     * If output_en is 1,this applies to interrupts,else PMU_trigger */
    uint16_t output_type : 1;

    /** 0 - Level trigger , 1 - edge trigger  */
    uint16_t edge_ctrl : 1;

    /** To enable either INT1 or INT2 pin as input.
     * 0 - input disabled ,1 - input enabled */
    uint16_t input_en : 1;

    /** Latch duration*/
    uint16_t latch_dur : 4;
} IMUInterruptPinSetting;

/**
 * @brief Accel tap interrupt configuration
 */
typedef struct IMUAccTapInterruptCfg_st {
    /** Tap threshold */
    uint16_t tap_thr : 5;
    /** Tap shock */
    uint16_t tap_shock : 1;
    /** Tap quiet */
    uint16_t tap_quiet : 1;
    /** Tap duration */
    uint16_t tap_dur : 3;
    /** Data source 0- filter & 1 pre-filter*/
    uint16_t tap_data_src : 1;
    /** Tap enable, 1 - enable, 0 - disable */
    uint16_t tap_en : 1;
} IMUAccTapInterruptCfg;

/**
 * @brief Accel any motion interrupt configuration
 */
typedef struct IMUAccAnyMotInterruptCfg_st {
    /** 1 any-motion enable, 0 - any-motion disable */
    uint8_t anymotion_en : 1;
    /** Slope interrupt x, 1 - enable, 0 - disable */
    uint8_t anymotion_x : 1;
    /** Slope interrupt y, 1 - enable, 0 - disable */
    uint8_t anymotion_y : 1;
    /** Slope interrupt z, 1 - enable, 0 - disable */
    uint8_t anymotion_z : 1;
    /** Slope duration */
    uint8_t anymotion_dur : 2;
    /** Data source 0- filter & 1 pre-filter*/
    uint8_t anymotion_data_src : 1;
    /** Slope threshold */
    uint8_t anymotion_thr;
} IMUAccAnyMotInterruptCfg;

/**
 * @brief Accel significant motion interrupt configuration
 */
typedef struct IMUAccSigMotInterruptCfg_st {
    /** Skip time of sig-motion interrupt */
    uint8_t sig_mot_skip : 2;
    /** Proof time of sig-motion interrupt */
    uint8_t sig_mot_proof : 2;
    /** Data source 0- filter & 1 pre-filter*/
    uint8_t sig_data_src : 1;
    /** 1 - enable sig, 0 - disable sig & enable anymotion */
    uint8_t sig_en : 1;
    /** Sig-motion threshold */
    uint8_t sig_mot_thres;
} IMUAccSigMotInterruptCfg;

/**
 * @brief Accel step detect interrupt configuration
 */
typedef struct IMUAccStepDetectInterruptCfg_st {
    /** 1- step detector enable, 0- step detector disable */
    uint16_t step_detector_en : 1;
    /** Minimum threshold */
    uint16_t min_threshold : 2;
    /** Minimal detectable step time */
    uint16_t steptime_min : 3;
    /** Enable step counter mode setting */
    uint16_t step_detector_mode : 2;
    /** Minimum step buffer size*/
    uint16_t step_min_buf : 3;
} IMUAccStepDetectInterruptCfg;

/**
 * @brief Accel no motion interrupt configuration
 */
typedef struct IMUAccNoMotionInterruptCfg_st {
    /** No motion interrupt x */
    uint16_t no_motion_x : 1;
    /** No motion interrupt y */
    uint16_t no_motion_y : 1;
    /** No motion interrupt z */
    uint16_t no_motion_z : 1;
    /** No motion duration */
    uint16_t no_motion_dur : 6;
    /** No motion sel , 1 - enable no-motion ,0- enable slow-motion */
    uint16_t no_motion_sel : 1;
    /** Data source 0- filter & 1 pre-filter*/
    uint16_t no_motion_src : 1;
    /** No motion threshold */
    uint8_t no_motion_thres;
} IMUAccNoMotionInterruptCfg;

/**
 * @brief Accel orient interrupt configuration
 */
typedef struct IMUAccOrientInterruptCfg_st {
    /** Thresholds for switching between the different orientations */
    uint16_t orient_mode : 2;
    /** Blocking_mode */
    uint16_t orient_blocking : 2;
    /** Orientation interrupt hysteresis */
    uint16_t orient_hyst : 4;
    /** Orientation interrupt theta */
    uint16_t orient_theta : 6;
    /** Enable/disable Orientation interrupt */
    uint16_t orient_ud_en : 1;
    /** Exchange x- and z-axis in algorithm ,0 - z, 1 - x */
    uint16_t axes_ex : 1;
    /** 1 - orient enable, 0 - orient disable */
    uint8_t orient_en : 1;
} IMUAccOrientInterruptCfg;

/**
 * @brief Accel flat detect interrupt configuration
 */
typedef struct IMUAccFlatDetectInterruptCfg_st {
    /** Flat threshold */
    uint16_t flat_theta : 6;
    /** Flat interrupt hysteresis */
    uint16_t flat_hy : 3;
    /** Delay time for which the flat value must remain stable for the
     *  flat interrupt to be generated */
    uint16_t flat_hold_time : 2;
    /** 1 - flat enable, 0 - flat disable */
    uint16_t flat_en : 1;
} IMUAccFlatDetectInterruptCfg;

/**
 * @brief Accel log-g interrupt configuration
 */
typedef struct IMUAccLowGInterruptCfg_st {
    /** Low-g interrupt trigger delay */
    uint8_t low_dur;
    /** Low-g interrupt trigger threshold */
    uint8_t low_thres;
    /** Hysteresis of low-g interrupt */
    uint8_t low_hyst : 2;
    /** 0 - single-axis mode ,1 - axis-summing mode */
    uint8_t low_mode : 1;
    /** Data source 0- filter & 1 pre-filter */
    uint8_t low_data_src : 1;
    /** 1 - enable low-g, 0 - disable low-g */
    uint8_t low_en : 1;
} IMUAccLowGInterruptCfg;

/**
 * @brief Accel high-g interrupt configuration
 */
typedef struct IMUAccHighGInterruptCfg_st {
    /** High-g interrupt x, 1 - enable, 0 - disable */
    uint8_t high_g_x : 1;
    /** High-g interrupt y, 1 - enable, 0 - disable */
    uint8_t high_g_y : 1;
    /** High-g interrupt z, 1 - enable, 0 - disable */
    uint8_t high_g_z : 1;
    /** High-g hysteresis  */
    uint8_t high_hy : 2;
    /** Data source 0- filter & 1 pre-filter */
    uint8_t high_data_src : 1;
    /** High-g threshold */
    uint8_t high_thres;
    /** High-g duration */
    uint8_t high_dur;
} IMUAccHighGInterruptCfg;

typedef union ImuIntrTypeCfg_un {
    /** Tap interrupt structure */
    IMUAccTapInterruptCfg acc_tap_irq;
    /** Slope interrupt structure */
    IMUAccAnyMotInterruptCfg acc_any_motion_irq;
    /** Significant motion interrupt structure */
    IMUAccSigMotInterruptCfg acc_sig_motion_irq;
    /** Step detector interrupt structure */
    IMUAccStepDetectInterruptCfg acc_step_detect_irq;
    /** No motion interrupt structure */
    IMUAccNoMotionInterruptCfg acc_no_motion_irq;
    /** Orientation interrupt structure */
    IMUAccOrientInterruptCfg acc_orient_irq;
    /** Flat interrupt structure */
    IMUAccFlatDetectInterruptCfg acc_flat_irq;
    /** Low-g interrupt structure */
    IMUAccLowGInterruptCfg acc_low_g_irq;
    /** High-g interrupt structure */
    IMUAccHighGInterruptCfg acc_high_g_irq;
} ImuIntrTypeCfg;

/**
 * @brief Interrupt configuration
 */
typedef struct IMUInterruptSetting_st {
    uint8_t irq_channel; /**< Interrupt channel */
    uint8_t irq_type;    /**< Select Interrupt */
    /** Structure configuring Interrupt pins */
    IMUInterruptPinSetting irq_pin_settg;
    /** Union configures required interrupt */
    ImuIntrTypeCfg irq_type_cfg;
    /** FIFO FULL INT 1-enable, 0-disable */
    uint8_t fifo_full_irq_en : 1;
    /** FIFO WTM INT 1-enable, 0-disable */
    uint8_t fifo_wtm_irq_en : 1;
} IMUInterruptSetting;

/**
 * @brief Structure of IMU sensor mode
 */
typedef struct ImuPmuStatus_st {
    uint8_t accel_pmu_status; /**< Accel sensor mode */
    uint8_t gyro_pmu_status;  /**< Gyro sensor mode */
    uint8_t aux_pmu_status;   /**< Aux sensor mode */
} ImuPmuStatus;

/**
 * @brief Structure of IMU sensor configuration
 * @note The members of this structure are actual IMU register configuration
 *       values, not physical values. For example, odr is the register value of
 *       output data rate, not the real frequency in Hz. To configure the real
 *       sensor sampling frequency, use hal_imu_set_accel_cfg() and
 *       hal_imu_set_gyro_cfg() instead.
 */
typedef struct ImuSensorConfig_st {
    uint8_t power; /**< Power mode */
    uint8_t odr;   /**< Output data rate */
    uint8_t range; /**< Range */
    uint8_t bw;    /**< Bandwidth */
} ImuSensorConfig;

/**
 * @brief Structure of IMU sensor data
 */
typedef struct ImuSensorData_st {
    /** X-axis sensor data */
    int16_t x;
    /** Y-axis sensor data */
    int16_t y;
    /** Z-axis sensor data */
    int16_t z;
    /** Sensor time or data set index */
    union {
        uint32_t sensortime;
        uint32_t data_set_idx;
    };
} __attribute((__packed__)) ImuSensorData;

/**
 * @brief Structure of IMU gyro and accel data
 */
typedef struct ImuGyroAccelData_st {
    /** Gyro X-axis sensor data */
    int16_t gx;
    /** Gyro Y-axis sensor data */
    int16_t gy;
    /** Gyro Z-axis sensor data */
    int16_t gz;
    /** Accel X-axis sensor data */
    int16_t ax;
    /** Accel Y-axis sensor data */
    int16_t ay;
    /** Accel Z-axis sensor data */
    int16_t az;
    /** Sensor time or data set index */
    union {
        uint32_t sensortime;
        uint32_t data_set_idx;
    };
} __attribute((__packed__)) ImuGyroAccelData;

/**
 * @brief Aux data structure which comprises of 8 bytes of accel data
 */
typedef struct ImuAuxData_st {
    /** Auxiliary data */
    uint8_t data[8];
} ImuAuxData;

/**
 * @brief Accel motion states
 */
typedef enum AccelMotionState {
    ANY_SIG_DISABLE = -1, /**< Disable anymotion and signifition motion */
    ANY_ENABLE      = 0,  /**< Enable anymotion */
    SIG_ENABLE      = 1,  /**< Enable signifition motion */
} AccelMotionState;

/**
 * @brief IMU configuration
 */
typedef struct ImuHwConfig_st {
    const GpioPort *power_pin; /**< Power pin */
    const GpioPort *data_pin;  /**< Data pin */
    const GpioPort *wake_pin;  /**< Wake pin */
} ImuHwConfig;

/**
 * @brief IMU FIFO configuration
 */
typedef struct ImuFifoConfig_st {
    /** Data buffer of user defined length is to be mapped here */
    uint8_t *data;
    /** While calling the API  "get_fifo_data" , length stores
     *  number of bytes in FIFO to be read (specified by user as input)
     *  and after execution of the API ,number of FIFO data bytes
     *  available is provided as an output to user
     */
    uint16_t length;
    /** FIFO time enable */
    uint8_t fifo_time_enable;
    /** Enabling of the FIFO header to stream in header mode */
    uint8_t fifo_header_enable;
    /** Streaming of the Accelerometer, Gyroscope
     * sensor data or both in FIFO */
    uint8_t fifo_data_enable;
    /** Will be equal to length when no more frames are there to parse */
    uint16_t accel_byte_start_idx;
    /** Will be equal to length when no more frames are there to parse */
    uint16_t gyro_byte_start_idx;
    /** Will be equal to length when no more frames are there to parse */
    uint16_t aux_byte_start_idx;
    /** Will be equal to length when no more frames are there to parse */
    uint16_t accel_gyro_byte_start_idx;
    /** Value of FIFO sensor time time */
    uint32_t sensor_time;
    /** Value of Skipped frame counts */
    uint8_t skipped_frame_count;
} ImuFifoConfig;

/**
 * @brief Aux sensor configuration structure
 */
typedef struct ImuAuxCfg_st {
    /** Aux sensor, 1 - enable 0 - disable */
    uint8_t aux_sensor_enable : 1;
    /** Aux manual/auto mode status */
    uint8_t manual_enable : 1;
    /** Aux read burst length */
    uint8_t aux_rd_burst_len : 2;
    /** Output data rate */
    uint8_t aux_odr : 4;
    /** I2C addr of auxiliary sensor */
    uint8_t aux_i2c_addr;
    uint8_t power; /**< Power mode */
} ImuAuxCfg;

/**
 * @brief FOC configuration structure
 */
typedef struct ImuFocCfg_st {
    uint8_t foc_gyr_en;
    uint8_t foc_acc_x;
    uint8_t foc_acc_y;
    uint8_t foc_acc_z;
    uint8_t acc_off_en;
    uint8_t gyro_off_en;
} ImuFocCfg;

/**
 * @brief Accel gyro offsets
 */
typedef struct ImuOffsets_st {
    /** Accel offset for x axis */
    int8_t off_acc_x;
    /** Accel offset for y axis */
    int8_t off_acc_y;
    /** Accel offset for z axis */
    int8_t off_acc_z;
    /** Gyro offset for x axis */
    int16_t off_gyro_x;
    /** Gyro offset for y axis */
    int16_t off_gyro_y;
    /** Gyro offset for z axis */
    int16_t off_gyro_z;
} ImuOffsets;

/**
 * @brief IMU config struct for hal
 */
typedef struct DeviceConfig_st ImuConfig;

typedef struct ImuContext_st {
    uint8_t chip_id;                /**< Chip id */
    uint8_t capability;             /**< Capability of the sensor */
    ImuFifoConfig *fifo_cfg;        /**< FIFO configuration */
    uint8_t accel_motion_state;     /**< Accel motion state, definition @see
                                         AccelMotionState */
    ImuSensorConfig accel_cfg;      /**< Structure to configure Accel sensor */
    ImuSensorConfig prev_accel_cfg; /**< Structure to hold previous/old accel
                                         config parameters */
    ImuSensorConfig gyro_cfg;       /**< Structure to configure gyro sensor */
    ImuSensorConfig prev_gyro_cfg;  /**< Structure to hold previous/old gyro
                                         config parameters */
    ImuAuxCfg aux_cfg;              /**< Structure to configure auxiliary sensor */
    ImuAuxCfg prev_aux_cfg;         /**< Structure to hold previous/old aux
                                         config parameters */
} ImuContext;

typedef struct ImuDevice_st {
    BaseDevice base;
    ImuConfig *cfg;
    const ImuHwConfig *hw_cfg;
    /** Flag set by sensor hal */
    uint8_t flag;
    /** Capability of the sensor */
    uint8_t capability;
    /** FIFO buffer */
    uint8_t *fifo_buff;
} ImuDevice;
typedef ImuDevice imuDevice;

/**
 * @brief Structure of operations for IMU.
 */
typedef struct ImuOperations_st {
    int (*device_init)(ImuDevice *dev);
    /** Init device */
    int (*init)(ImuDevice *dev);
    /** Deinit device */
    int (*deinit)(ImuDevice *dev);
    /** Make soft reset */
    int (*soft_reset)(ImuDevice *dev);
    /** Read registers from sensor */
    int (*read_reg)(ImuDevice *dev, uint8_t reg_addr, uint8_t *data, uint16_t len);
    /** Write register to sensor */
    int (*write_reg)(ImuDevice *dev, uint8_t reg_addr, uint8_t *data, uint16_t len);
    /** Read accel/gyro samples */
    int (*read_sample)(ImuDevice *dev, uint8_t select, ImuSensorData *accel, ImuSensorData *gyro,
                       uint8_t count);
    /** Configures the default power mode, range and bandwidth of sensor */
    int (*set_sensor_default_cfg)(ImuDevice *dev);
    /** Configures accelerometer sensor with actual val */
    int (*set_accel_cfg)(ImuDevice *dev, uint8_t range, uint8_t bwp, uint16_t odr,
                         uint8_t select_type);
    /** Configures gyro sensor with actual val */
    int (*set_gyro_cfg)(ImuDevice *dev, uint16_t range, uint8_t bwp, uint16_t odr,
                        uint8_t select_type);
    /** Configures the power mode, range and bandwidth of sensor */
    int (*set_sensor_cfg)(ImuDevice *dev, ImuSensorConfig *accel, ImuSensorConfig *gyro,
                          ImuAuxCfg *mag);
    /** Get configuration of sensor */
    int (*get_sensor_cfg)(ImuDevice *dev, ImuSensorConfig *accel, ImuSensorConfig *gyro,
                          ImuAuxCfg *mag);
    /** Get range of accel */
    int (*get_accel_range)(ImuDevice *dev, uint8_t *range);
    /** Set the low power mode of the selected sensor */
    int (*set_low_power)(ImuDevice *dev, uint8_t sensor);
    /** Set the normal mode of the sensor */
    int (*set_normal_mode)(ImuDevice *dev, uint8_t sensor);
    /** Set the stop mode of the sensor */
    int (*set_stop_mode)(ImuDevice *dev, uint8_t sensor);
    int (*get_power_mode)(ImuDevice *dev, ImuPmuStatus *pmu_status);
    /** Set FIFO watermark level */
    int (*set_fifo_wm)(ImuDevice *dev, uint8_t fifo_wm);
    /** Set the down sampling ratios of the accel and gyro data for FIFO */
    int (*set_fifo_down)(ImuDevice *dev, uint8_t fifo_down);
    /** Set FIFO configuration */
    int (*set_fifo_cfg)(ImuDevice *dev, uint8_t config, bool enable);
    /** Flush the sample buffer, discarding all unread samples */
    int (*flush_fifo)(ImuDevice *dev);
    /** Read accel/gyro samples from fifo */
    int (*get_fifo_data)(ImuDevice *dev);
    /** Read fifo data and packet with Accel format */
    int (*fifo_parsing_accel)(ImuDevice *dev, ImuSensorData *accel_data, int8_t frame_request,
                              uint16_t *available_frame);
    int (*fifo_parsing_gyro_accel)(ImuDevice *dev, ImuGyroAccelData *gyro_accel_data,
                                   int8_t frame_request, uint16_t *available_frame);
    /** Configure different interrupts */
    int (*set_interrupt_cfg)(ImuDevice *dev, bool enable, uint8_t irq_type,
                             IMUInterruptSetting *setting);
    /** Check interrupt status */
    int (*check_interrupt_status)(ImuDevice *dev, uint8_t *check_type);
    /** Set the threshold and duration of highg interrupt */
    int (*set_highg_threshold)(ImuDevice *dev, IMUHighGCondition highg_condition);
    /** Clear step count register value */
    int (*reset_step_counter)(ImuDevice *dev);
    /** Enables/disable the step counter feature */
    int (*set_step_counter)(ImuDevice *dev, uint8_t step_cnt_enable);
    /** Read the step counter value. */
    int (*get_step_counter)(ImuDevice *dev, uint16_t *step_val);
    /** Enable/disable the pin's interrupt */
    int (*enable_interrupt)(ImuDevice *dev, uint8_t pin, bool enable,
                            ImuDataReadyHandler data_ready_callback);
    /** Enable/disable power */
    int (*enable_power)(ImuDevice *dev, bool enable);
    /** Perform self test of accel/gyro of the imu sensor */
    int (*selftest)(ImuDevice *dev, uint8_t sensor_select);
} ImuOperations;
typedef ImuOperations imuOperations;

/**
 * @brief Initialize IMU device instance
 * @param[in] list Device list of IMU
 * @param[in] count Count number of IMU device
 * @return  VSD_SUCCESS on success, others on failure
 */
int hal_imu_device_init(ImuDevice *list, uint8_t count);

/**
 * @brief Get IMU device instance
 * @param id The id of IMU
 * @return Return the IMU instance, NULL on failure
 */
ImuDevice *hal_imu_get_device(uint8_t id);

/**
 * @brief This API used to initialize imu device driver
 * @note This API has to be called once before calling other IMU APIs except
 * hal_imu_enable_power
 * @param dev Handle of IMU device
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_init(ImuDevice *dev);

/**
 *  @brief This API used to release IMU sensor context
 *  @note IMU APIs can not be called anymore after calling this API except the
 *  hal_imu_init & hal_imu_enable_power
 *  @param dev Handle of IMU device
 *  @return VSD_SUCCESS on success, others on failure
 *
 */
int hal_imu_deinit(ImuDevice *dev);

/**
 *  @brief This API used to soft reset the IMU sensor
 *  @note User has to allocate the FIFO buffer along with
 *  corresponding fifo length from his side before calling this API
 *  @param dev Handle of IMU device
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_soft_reset(ImuDevice *dev);

/**
 * @brief Function to read data from the given register address of IMU
 * sensor
 * @note This API is for debug only
 * @param dev Handle of IMU device
 * @param reg_addr Register address from where the data to be read
 * @param data Pointer to data buffer to store the read data
 * @param len Number of bytes of data to be read
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_read_reg(ImuDevice *dev, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * @brief Function to write the given data to the register address
 * of sensor
 * @note This API is for debug only
 * @param dev Handle of IMU device
 * @param reg_addr Register address from where the data to be written
 * @param data Pointer to data buffer which is to be written in the sensor
 * @param len Number of bytes of data to write
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_write_reg(ImuDevice *dev, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * @brief This API configures the default power mode, range and bandwidth of
 * sensor
 * @param dev Handle of IMU device
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_sensor_default_cfg(ImuDevice *dev);

/**
 * @brief This API configures the range, bandwidth and output data rate
 * of accelerometer sensor with actual val
 * @param dev Handle of IMU device
 * @param range Accelerometer range in unit of G, 2 for ±2G
 * @param bwp Accelerometer bandwidth parameters
 * @param odr Accelerometer output data rate in unit of Hz
 * @param select_type Configuration type, definition @see ImuSensorCfgType
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_accel_cfg(ImuDevice *dev, uint8_t range, uint8_t bwp, uint16_t odr,
                          uint8_t select_type);

/**
 * @brief This API configures the range, bandwidth and output data rate
 * of gyro sensor with actual val
 * @param dev Handle of IMU device
 * @param range Gyro range in unit of DPS(degrees per second), 500 for ±500°/s
 * @param bwp Actual gyro oversampling rate, the gyroscope bandwidth coefficient
 * defines the 3 dB cutoff frequency of the low pass filter for the sensor data
 * @param odr Actual gyro output data rate in unit of Hz
 * @param select_type Configuration type, definition @see ImuSensorCfgType
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_gyro_cfg(ImuDevice *dev, uint16_t range, uint8_t bwp, uint16_t odr,
                         uint8_t select_type);

/**
 * @brief This API configures the power mode, range and bandwidth of sensor
 * @note The members of accel_cfg, gyro_cfg and aux_cfg are configuration values
 *       written directly to the actual IMU registers. If you want to configure
 *       the real sensor sampling frequency, use hal_imu_set_accel_cfg() and
 *       hal_imu_set_gyro_cfg() instead.
 * @param dev Handle of IMU device
 * @param accel_cfg Accel sensor configuration
 * @param gyro_cfg Gyro sensor configuration
 * @param aux_cfg Magnet sensor configuration
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_sensor_cfg(ImuDevice *dev, ImuSensorConfig *accel_cfg, ImuSensorConfig *gyro_cfg,
                           ImuAuxCfg *aux_cfg);

/**
 * @brief This API get the current power mode, range and bandwidth
 * configurations of sensor
 * @param dev Handle of IMU device
 * @param accel_cfg Accel sensor configuration
 * @param gyro_cfg Gyro sensor configuration
 * @param aux_cfg Magnet sensor configuration
 * @return Result of API execution status
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_imu_get_sens_cfg(ImuDevice *dev, ImuSensorConfig *accel_cfg, ImuSensorConfig *gyro_cfg,
                         ImuAuxCfg *aux_cfg);

/**
 * @brief This API get the G range of accel
 * @param dev Handle of IMU device
 * @param range Range value of accel in G, as an example, 8 for ±8G
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_get_accel_range(ImuDevice *dev, uint8_t *range);

/**
 * @brief This API sets work mode of the sensor
 * @param dev Handle of IMU device
 * @param sensor Sensor type to set work mode, can be IMU_ACCEL, IMU_GYRO,
 * IMU_ACCEL_GYRO, IMU_MAG or IMU_ALL. Definition @see ImuSensorType
 * @param mode The work mode to set, definition @see ImuSensorMode
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_work_mode(ImuDevice *dev, uint8_t sensor, uint8_t mode);

/**
 * @brief Function to get the power mode of the sensor
 * @param dev Handle of IMU device
 * @param pmu_status Power mode of the sensor
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_get_power_mode(ImuDevice *dev, ImuPmuStatus *pmu_status);

/**
 *  @brief This API sets the FIFO watermark level in the sensor
 *  @note The FIFO watermark is issued when the FIFO fill level is
 *  equal or above the watermark level and units of watermark is 4 bytes
 *  @param dev Handle of IMU device
 *  @param fifo_wm Variable used to set the FIFO water mark level
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_fifo_wm(ImuDevice *dev, uint8_t fifo_wm);

/**
 *  @brief This API is used to configure the down sampling ratios of
 *  the accel and gyro data for FIFO. Also, it configures filtered or
 *  pre-filtered data for accel and gyro.
 *  @param dev Handle of IMU device
 *  @param fifo_down the down sampling ratios of the accel and gyro data for FIFO
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_fifo_down(ImuDevice *dev, uint8_t fifo_down);

/** @brief This API sets the FIFO configuration in the sensor
 *  @param dev Handle of IMU device
 *  @param config Configure the data types in FIFO, configurations which are to
 *  be enabled or disabled in the sensor. Definition @see ImuFifoType.
 *  @param enable Parameter used to enable or disable the above
 *  FIFO configuration, true to enable, false to disable
 *  @note The API can be used to set multiple data types, as an example,
 * (IMU_FIFO_ACCEL | IMU_FIFO_TIME) can be configured to generate both accel &
 * time in IMU sensor's FIFO, then user can read expected data from read APIs
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_fifo_cfg(ImuDevice *dev, uint8_t config, bool enable);

/**
 *  @brief This API clears all data in the FIFO without changing FIFO
 *  configuration settings
 *  @note This API should be called once after configuring FIFO
 *  @param dev Handle of IMU device
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_flush_fifo(ImuDevice *dev);

/**
 *  @brief This API reads raw data from the fifo buffer
 *  @note User has to allocate the FIFO buffer along with
 *  corresponding fifo length from application side before calling this API
 *  @note User must specify the number of bytes to read from the FIFO in
 *  dev->ctx->fifo_cfg->length , It will be updated by the number of
 *  bytes actually read from FIFO after calling this API
 *  @param dev Handle of IMU device
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_get_fifo_data(ImuDevice *dev);

/**
 *  @brief Read accel data from IMU and pack in ImuSensorData format
 *  @note User has to allocate buffer along with corresponding data length from
 *  upper layer before calling this API
 *  @param dev Handle of IMU device
 *  @param accel_data Accel buffer to store
 *  @param frame_request Requested frames number in ImuSensorData unit
 *  @param available_frame Available frame number read from IMU
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_read_accel(ImuDevice *dev, ImuSensorData *accel_data, uint16_t frame_request,
                       uint16_t *available_frame);

/**
 *  @brief Read accel & gyro data from IMU and pack in ImuGyroAccelData format
 *  @note User has to allocate buffer along with corresponding data length from
 *  upper layer before calling this API
 *  @param dev Handle of IMU device
 *  @param gyro_accel_data Gyro and accel buffer to store
 *  @param frame_request Requested frames number in ImuGyroAccelData unit
 *  @param available_frame Available frame number read from IMU
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_read_gyro_accel(ImuDevice *dev, ImuGyroAccelData *gyro_accel_data,
                            uint16_t frame_request, uint16_t *available_frame);

/**
 *  @brief This API used to configure IMU sensor interrupt
 *  @param dev Handle of IMU device
 *  @param enable  Sensor interrupt enable/disable
 *  @param irq_type Sensor interrupt type, definition @see ImuIntrType
 *  @param irq_settings Configuration for this interrupt, NULL can be used to
 *  set default settings
 *  @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_cfg_interrupt(ImuDevice *dev, bool enable, uint8_t irq_type,
                          IMUInterruptSetting *irq_settings);

/**
 * @brief This API checks if the interrupt is triggered
 * @param dev Handle of IMU device
 * @param check_type enum to select interrupt, including
 * IMU_ACC_ANY_MOTION_INTERRUPT, IMU_ACC_DOUBLE_TAP_INTERRUPT,
 * IMU_ACC_SINGLE_TAP_INTERRUPT, IMU_ACC_HIGH_G_INTERRUPT
 * @return  VSD_SUCCESS on success, others on failure
 */
int hal_imu_check_interrupt_status(ImuDevice *dev, uint8_t *check_type);

/**
 * @brief This API set the threshold and duration of highg interrupt
 * @param dev Handle of IMU device
 * @param highg_condition struct of high interrupt condition
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_highg_threshold(ImuDevice *dev, IMUHighGCondition highg_condition);

/**
 * @brief This API writes step count reset command to command register
 * This action clears step count register value
 * @param dev Handle of IMU device
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_reset_step_counter(ImuDevice *dev);

/**
 * @brief This API enables or disable the step counter feature
 * @param dev Handle of IMU device
 * @param step_cnt_enable 1 - enable step counter, 0 - disable
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_set_step_counter(ImuDevice *dev, uint8_t step_cnt_enable);

/**
 * @brief This API reads the step counter value
 * @param dev Handle of IMU device
 * @param step_val Pointer to store step value
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_get_step_counter(ImuDevice *dev, uint16_t *step_val);

/**
 * @brief This API use to enable/disable the pin's interrupt
 * @note The API should be called again to re-enable interrupt after a trigger
 * if the trigger mode of pin is level trigger; driver will re-enable trigger if
 * the trigger mode is edge trigger. Check the pin's GPIO configuration to get
 * trigger mode
 * @param dev Handle of IMU device
 * @param pin Selected pin, can be IMU_DATA_PIN, IMU_WAKE_PIN or
 * IMU_POWER_PIN, definition @see ImuPinType
 * @param enable true to enable interrupt, false to disable interrupt for
 * specific pin
 * @param data_ready_callback The callback function when pin interrupt is
 * triggered
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_enable_interrupt(ImuDevice *dev, uint8_t pin, bool enable,
                             ImuDataReadyHandler data_ready_callback);

/**
 * @brief This API use to enable/disable IMU power
 * @note This API should be called once before any other IMU APIs if IMU sensor
 * is powered off by default
 * @param dev Handle of IMU device
 * @param enable Check if enable power
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_enable_power(ImuDevice *dev, bool enable);

/**
 * @brief This is used to perform self test of accel/gyro of the imu sensor
 * @param dev Handle of IMU device
 * @param[in] select_sensor  : enum to choose accel or gyro for self test
 * @note self test can be performed either for accel/gyro at any instant.
 *
 *     value of select_sensor       |  Inference
 *----------------------------------|--------------------------------
 *   IMU_ACCEL                      | Accel self test enabled
 *   IMU_GYRO                       | Gyro self test enabled
 *   IMU_ACCEL_GYRO                 | NOT TO BE USED
 *
 * @note The return value of this API gives us the result of self test
 * @note Performing self test does soft reset of the sensor, User can
 * set the desired settings after performing the self test
 * @return VSD_SUCCESS on success, others on failure
 */
int hal_imu_perform_self_test(ImuDevice *dev, uint8_t select_sensor);

/** @} */

#endif // HAL_IMU_H
