/** @file
 *  @brief Verisilicon utility interface
 */

/*
 * Copyright (C) 2023 VeriSilicon Holdings Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _VS_UTIL_H_
#define _VS_UTIL_H_
#include <addr.h>
/**
 * @brief Checks if a value is within range.
 *
 * @note @p val is evaluated twice.
 *
 * @param val Value to be checked.
 * @param min Lower bound (inclusive).
 * @param max Upper bound (inclusive).
 *
 * @retval true If value is within range
 * @retval false If the value is not within range
 */
#define IN_RANGE(val, min, max) ((val) >= (min) && (val) <= (max))

struct bt_vs_set_rf_ctrl_param_t {
    /**
     *  @brief RF power on/off.
     */
    uint8_t power;

    /**
     *  @brief This field indicates whether the parameters change in RF power on.
     */
    uint8_t option;

    /**
     *  @brief RF rapid clock frequency adjust value(OSC internal CL adjust).
     */
    uint8_t clval;

    /**
     *  @brief RF slow clock configurations.
     */
    uint8_t slow_clock;

    /**
     *  @brief Set tx power in power on.
     */
    uint8_t tx_power;

    /**
     *  @brief Set RF option.
     */
    uint8_t rf_option;
};

struct bt_le_vs_hci_vs_evt_cb_t {
#if defined(CONFIG_BT_ISO_DUAL_PATH_SUPPORT_HOST)
    void (*vs_evt_ts_rpt_cb)(struct net_buf *buf);
#endif
#if defined(CONFIG_BT_MESH_CLOCK_MODEL)
    void (*vs_evt_time_sync_tx_tm_rpt_cb)(struct net_buf *buf);
    void (*vs_evt_time_sync_rx_tm_rpt_cb)(struct net_buf *buf);
#endif
};

/**
 * @brief Set BD Address
 *
 * Set BD Address, include public address or random address.
 *
 * Call the function provided by the controller directly.
 *
 * @param l_addr address to be set.
 *
 * @return Zero on success or (negative) error code otherwise.
 * @return -EINVAL address type is invalid. Valid range is 0x00 - 0x01.
 *                 or l_addr is NULL.
 */
int bt_le_vs_set_bd_addr(bt_addr_le_t *l_addr);


/**
 * @brief Set BD Address by VS HCI command
 *
 * Set BD Address, include public address or random address.
 *
 * @param l_addr address to be set.
 *
 * @return Zero on success or (negative) error code otherwise.
 * @return -ENOMEM No free buffer to send HCI command.
 * @return -EINVAL address type is invalid. Valid range is 0x00 - 0x01.
 *                 or l_addr is NULL.
 * @return -ECONNREFUSED When connectable advertising is requested and there
 *                       is already maximum number of connections established
 *                       in the controller.
 *                       Up to controllers code returned in this case may be
 *                       -EIO.
 */
int bt_le_vs_hci_set_bd_addr(bt_addr_le_t *l_addr);

/**
 * @brief Set RF control by VS HCI command
 *
 * This function performs power control on RF.
 *
 * @param p_rf_ctrl the rf control param.
 *
 * @return Zero on success or (negative) error code otherwise.
 * @return -ENOMEM No free buffer to send HCI command.
 * @return -EINVAL address type is invalid. Valid range is 0x00 - 0x01.
 *                 or l_addr is NULL.
 * @return -ECONNREFUSED When connectable advertising is requested and there
 *                       is already maximum number of connections established
 *                       in the controller.
 *                       Up to controllers code returned in this case may be
 *                       -EIO.
 */
int bt_le_vs_hci_set_rf_control(struct bt_vs_set_rf_ctrl_param_t *p_rf_ctrl);

#if CONFIG_BT_HCI_SETUP
/**
 * @brief Register user callback for HCI driver callback `setup`
 *
 * This function register callback to be call in HCI driver callback `setup`.
 *
 * @param setup_cb the callback to be registered.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_le_vs_hci_register_setup_cb(int (*setup_cb)(void));
#endif /* CONFIG_BT_HCI_SETUP */

/**
 * @brief Write data to specified memory of controller by VS HCI command
 *
 * This function writes data to specified memory of controller.
 *
 * @param dst_addr the specified memory of controller to be written.
 * @param mem_size the size of the data.
 * @param mem_content the pointer points at the data.
 *
 * @return Zero on success or (negative) error code otherwise.
 * @return -ENOMEM No free buffer to send HCI command.
 */
int bt_le_vs_hci_write_memory(uint32_t dst_addr, uint8_t mem_size, uint8_t *mem_content);

#endif /* _VS_UTIL_H_ */
