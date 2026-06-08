/*
 * Copyright (C) 2025. VeriSilicon Holdings Co., Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __VSBT_PIPE_H
#define __VSBT_PIPE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @defgroup vsbt_pipe Vendor Pipe Interface
 * @brief Vendor-specific pipe interface for Bluetooth subsystem communication
 * @{
 */

/**
 * @brief Pipe type enumeration
 * @details Defines different types of pipes that can be created
 */
typedef enum vsbt_pipe_type {
    PIPE_SHELL,     /**< Shell pipe type */
    PIPE_H4         /**< H4 pipe type */
} vsbt_pipe_type;

/**
 * @brief Invalid handle value of VSBT PIPE objects
 * @details This macro defines the invalid handle value for VSBT pipe objects
 */
#define VSBT_PIPE_INVALID_HANDLE (0x0000)

/**
 * @brief Pipe object handle type
 * @details Type definition for pipe object handles used in the pipe interface
 */
typedef long unsigned int vsbt_pipe_t;

/**
 * @brief Pipe callback function type
 * @details Function pointer type for pipe receive callback functions
 * @return Number of bytes received
 */
typedef int (*vsbt_pipe_recv_cb)(void);

/**
 * @brief Initializes a pipe for receiving and sending data
 * @details This function creates and initializes a pipe of the specified type for data communication.
 *          The callback function will be invoked to notify the upper module when data is available.
 *
 * @param type The type of the created pipeline (PIPE_SHELL or PIPE_H4)
 * @param cb Upper module receive callback function
 * @return Handle to the created pipe. VSBT_PIPE_INVALID_HANDLE if the pipe could not be created.
 *
 * @note User needs to invoke the callback function to make the upper module start receiving commands.
 */
vsbt_pipe_t vsbt_pipe_setup(vsbt_pipe_type type, vsbt_pipe_recv_cb cb);

/**
 * @brief Shut down the pipe created by vsbt_pipe_setup
 * @details This function shuts down and cleans up the pipe that was previously created.
 *
 * @param pipe Pipe object handle
 * @param type The type of the created pipeline
 */
void vsbt_pipe_shutdown(vsbt_pipe_t pipe);

/**
 * @brief Send data through the pipe created by vsbt_pipe_setup
 * @details This function sends data through the specified pipe.
 *
 * @param pipe Pipe object handle
 * @param tx_data Data to send
 * @param len Length of data to send
 * @return Number of bytes sent
 */
int vsbt_pipe_send(vsbt_pipe_t pipe, const uint8_t *tx_data, const int len);

/**
 * @brief Read data through the pipe created by vsbt_pipe_setup
 * @details This function reads data from the specified pipe.
 *
 * @param pipe Pipe object handle
 * @param rx_data Data container
 * @param len Length of data to read
 * @return Number of bytes read
 */
int vsbt_pipe_read(vsbt_pipe_t pipe, uint8_t *rx_data, const int len);

/**
 * @brief Check if there is pending data to be read
 * @details This function checks if there is pending data in the pipe that can be read.
 *
 * @param pipe Pipe object handle
 * @return true if data is pending in the pipe; false otherwise
 */
bool vsbt_pipe_rx_pending(vsbt_pipe_t pipe);

/**
 * @}
 */

#endif
