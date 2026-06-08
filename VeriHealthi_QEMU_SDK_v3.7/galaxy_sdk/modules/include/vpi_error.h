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

#ifndef VPI_ERROR_H
#define VPI_ERROR_H

/**
 * @addtogroup MODULE_VPI
 * VPI error API
 * @ingroup MODULE
 * @{
 */

/** Error codes for VeriSilicon Platform Interface(VPI) */
typedef enum VpiError {
    VPI_ERR_DEFAULT = -2,   /**< Default error */
    VPI_ERR_GENERIC = -1,   /**< Generic Error */
    VPI_SUCCESS     = 0,    /**< Success */
    VPI_ERR_INVALID,        /**< Invalid input parameter */
    VPI_ERR_NOMEM,          /**< No memory error */
    VPI_ERR_FULL,           /**< Resource is full */
    VPI_ERR_EMPTY,          /**< Resource is empty */
    VPI_ERR_IO,             /**< IO error */
    VPI_ERR_BUSY,           /**< Device is busy */
    VPI_ERR_TIMEOUT,        /**< Timeout */
    VPI_ERR_NODEVICE,       /**< No device error */
    VPI_ERR_LOST_CON,       /**< Lost connection */
    VPI_ERR_NO_ACK,         /**< No ACK */
    VPI_ERR_NOT_READY,      /**< System or data is not ready */
    VPI_ERR_LACK,           /**< Not enough data */
    VPI_ERR_BAD_DATA,       /**< Bad data */
    VPI_ERR_EXCEPTION_DATA, /**< Exception data */
    VPI_ERR_FREE,           /**< Free error */
    VPI_ERR_REMOTE,         /**< Remote client/host error */
} VpiError;

/**
 * @brief convert vsd error code to vpi error code
 * @param vsd_err the vsd error code
 * @return vpi error code
 */
int vsd_to_vpi(int vsd_err);

/**
 * @brief print error message through vpi error code
 * @param vpi_err the vpi error code
 * @param s       message
 * @return VPI_SUCCESS
 */
int vpi_error_print(int vpi_err, const char *s);

/**
 * @brief convert vsa error code to vpi error code
 * @param vsa_err the vsa error code
 * @return vpi error code
 */
int vsa_to_vpi(int vsa_err);

/** @} */

#endif // VPI_ERROR_H
