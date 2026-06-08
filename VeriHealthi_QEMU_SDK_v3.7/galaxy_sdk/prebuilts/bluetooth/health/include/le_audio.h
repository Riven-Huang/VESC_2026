/*
 * Copyright 2022 - 2023 Verisilicon
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LE_AUDIO_H_
#define _LE_AUDIO_H_

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum bt_audio_dir {
	/**
	 * @brief Audio direction sink
	 *
	 * For a BAP Unicast Client or Broadcast Source this is considered outgoing audio (TX).
	 * For a BAP Unicast Server or Broadcast Sink this is considered incoming audio (RX).
	 */
	BT_AUDIO_DIR_SINK = 0x01,
	/**
	 * @brief Audio direction source
	 *
	 * For a BAP Unicast Client or Broadcast Source this is considered incoming audio (RX).
	 * For a BAP Unicast Server or Broadcast Sink this is considered outgoing audio (TX).
	 */
	BT_AUDIO_DIR_SOURCE = 0x02,
};
enum le_audio_evt_type {
    LE_AUDIO_EVT_CONFIG_RECEIVED,
    LE_AUDIO_EVT_STREAMING,
    LE_AUDIO_EVT_NOT_STREAMING,
    LE_AUDIO_EVT_SYNC_LOST,
    LE_AUDIO_EVT_NO_VALID_CFG,
    LE_AUDIO_EVT_PREVIOUS_STREAM,
    LE_AUDIO_EVT_NEXT_STREAM,
    LE_AUDIO_EVT_NUM_EVTS
};

enum bt_mgmt_evt_type {
    BT_MGMT_EXT_ADV_WITH_PA_READY = 1,
    BT_MGMT_CONNECTED,
    BT_MGMT_SECURITY_CHANGED,
    BT_MGMT_PA_SYNCED,
    BT_MGMT_PA_SYNC_LOST,
    BT_MGMT_DISCONNECTED,
};
struct le_audio_evt {
    enum le_audio_evt_type le_audio_evt_type;
    struct bt_conn *conn;
    struct bt_le_per_adv_sync *pa_sync;
    enum bt_audio_dir dir;
    uint8_t set_size;
    uint8_t const *sirk;
};

struct bt_mgmt_evt {
    enum bt_mgmt_evt_type event;
    struct bt_conn *conn;
    uint8_t index;
    struct bt_le_ext_adv *ext_adv;
    struct bt_le_per_adv_sync *pa_sync;
    uint32_t broadcast_id;
    uint8_t pa_sync_term_reason;
};
/**
 * @brief Callback for receiving Bluetooth LE Audio data
 *
 * @param data		    Pointer to received data
 * @param size		    Size of received data
 * @param bad_frame	    Indicating if the frame is a bad frame or not
 * @param ctlr_clk_ref	Bluetooth RF clk
 * @param audio_timer	Audio Timer
 */
typedef void (*le_audio_receive_cb)(const uint8_t *const data, size_t size, bool bad_frame,
                                    uint32_t ctlr_clk_ref, uint32_t audio_timer);
/**
 * @brief Callback for feeding Bluetooth LE Audio data
 *
 * @param data		Pointer to received data
 * @param size		Size of received data
 */
typedef void (*le_audio_feeding_cb)(const uint8_t *const data, size_t size);
struct audio_stream_configure {
    uint32_t sample_rate;
    uint32_t bit_rate;
    uint16_t frame_ms;
    uint8_t channels;
    uint32_t latency;
    uint32_t sdu_interval;
};
struct audio_codec_configure {
    uint32_t sample_rate;
    bool sync_enable;
};
struct le_audio_uop_vcp_cb {
    int (*audio_init)(struct audio_codec_configure *cfg);
    int (*audio_create_stream)(uint32_t stream_type,
                               struct audio_stream_configure *cfg); /* decoder/encoder*/
    int (*audio_stream_start)(uint32_t stream_type);
    int (*audio_stream_stop)(uint32_t stream_type);
    int (*audio_destory_stream)(uint32_t stream_type);
    int (*volume_mute)(uint32_t stream_type, bool mute);
    int (*volume_set)(uint32_t stream_type, uint32_t value);
};

struct le_audio_uop_mcp_cb {
    int (*previous)(void);
    int (*next)(void);
    int (*pause)(uint32_t stream_type);
    int (*resume)(uint32_t stream_type);
};

int le_audio_uop_vcp_register_cb(struct le_audio_uop_vcp_cb *cb);

int le_audio_uop_vcp_unregister_cb(struct le_audio_uop_vcp_cb *cb);

int le_audio_uop_mcp_register_cb(struct le_audio_uop_mcp_cb *cb);

int le_audio_uop_mcp_unregister_cb(struct le_audio_uop_mcp_cb *cb);

struct le_audio_datasource_cb {
    /** @brief data source initialize callback
     *
     *  init callback is called whenever a data source is requested to created
     *
     *  @param[in] stream_num number of source
     *
     *  @return 0 in case of success or negative value in case of error.
     */
    int (*init)(uint8_t stream_num);

    /** @brief read request callback
     *
     *  read callback is called when read request from le audio profile
     *
     *  @param[in]  buff    target buf for read out.
     *  @param[in]  len     request length.
     *  @param[out] count   read out count
     *  @param[in]  idx     stream idx.
     *
     *  @return 0 in case of success or negative value in case of error.
     */
    int (*read)(uint8_t *buff, uint32_t len, uint32_t *count, uint8_t idx);

    /** @brief write request callback
     *
     *  write callback is called when write request from le audio profile
     *
     *  @param[in]  buff    source buf for write into file.
     *  @param[in]  len     request length.
     *  @param[out] count   write count
     *  @param[in]  idx     stream idx.
     *
     *  @return 0 in case of success or negative value in case of error.
     */
    int (*write)(uint8_t *buff, uint32_t len, uint32_t count, uint8_t idx);

    /** @brief seek request callback
     *
     *  seek callback is called whenever seek is requested to
     *  be accessed from a file.
     *
     *  @param seek_offset    offset value.
     *  @param idx      file index.
     *  @return 0 in case of success or negative value in case of error.
     */
    int (*seek)(size_t seek_offset, uint8_t idx);

    /** @brief open request callback
     *
     *  open is called whenever a file is requested to
     *  be opened for read/write.
     *
     *  @param filename file name.
     *  @param idx      file index.
     *
     *  @return 0 in case of success or negative value in case of error.
     */
    int (*open)(const char *filename, void *lc3_format, uint8_t idx);

    /** @brief close request callback
     *
     *  close is called whenever a file requested to be closed
     *
     *  @param idx      file index.
     *  @return 0 in case of success or negative value in case of error.
     */
    int (*close)(uint8_t idx);
};

/** @brief Register data source callbacks.
 *
 *  Only one callback structure can be registered, and attempting to
 *  registering more than one will result in an error.
 *
 *  @param cb  data source callback structure.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int le_audio_datasource_register_cb(struct le_audio_datasource_cb *cb);

/** @brief Unregister data source callbacks.
 *
 *  May only unregister a callback structure that has previously been
 *  registered by le_audio_datasource_register_cb().
 *
 *  @param cb  data source callback structure.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int le_audio_datasource_unregister_cb(struct le_audio_datasource_cb *cb);
/**
 * @brief Callback for receiving Bluetooth LE Audio data.
 *
 * @param       data            Pointer to received data.
 * @param       size            Size of received data.
 * @param       bad_frame       Indicating if the frame is a bad frame or not.
 * @param       sdu_ref         ISO timestamp.
 * @param       channel_index   Audio channel index.
 */
typedef void (*le_audio_receive_new_cb)(const uint8_t *const data, size_t size, bool bad_frame,
                                        uint32_t sdu_ref, uint8_t channel_index,
                                        size_t desired_size);

/**
 * @brief       Encoded audio data and information.
 *
 * @note        Container for SW codec (typically LC3) compressed audio data.
 */
struct le_audio_encoded_audio {
    uint8_t const *const data;
    size_t size;
    uint8_t num_ch;
};

struct sdu_ref_msg {
    uint32_t timestamp;
    bool adjust;
};
/**
 * @brief Callback for using the timestamp of the previously sent audio packet
 *
 * @note  Can be used for drift calculation/compensation
 *
 * @param timestamp     The timestamp
 * @param adjust        Indicate if the sdu_ref should be used to adjust timing
 */
typedef void (*le_audio_timestamp_cb)(uint32_t timestamp, bool adjust);

enum le_audio_user_defined_action {
    LE_AUDIO_USER_DEFINED_ACTION_1,
    LE_AUDIO_USER_DEFINED_ACTION_2,
    LE_AUDIO_USER_DEFINED_ACTION_NUM
};

/**
 * @brief Encoded audio data and information.
 * Container for SW codec (typically LC3) compressed audio data.
 */
struct encoded_audio {
    uint8_t const *const data;
    size_t size;
    uint8_t num_ch;
};

/**
 * @brief Generic function for a user defined button press
 *
 * @param action	User defined action
 *
 * @return	0 for success,
 *		error otherwise
 */
int le_audio_user_defined_button_press(enum le_audio_user_defined_action action);

/**
 * @brief Get configuration for audio stream
 *
 * @param bitrate	Pointer to bitrate used
 * @param sampling_rate	Pointer to sampling rate used
 *
 * @return	0 for success,
 *		-ENXIO if the feature is disabled,
 *		error otherwise
 */
int le_audio_config_get(uint32_t *bitrate, uint32_t *sampling_rate, uint32_t *pres_delay);

/**
 * @brief	Increase volume by one step
 *
 * @return	0 for success,
 *		-ENXIO if the feature is disabled,
 *		error otherwise
 */
int le_audio_volume_up(void);

/**
 * @brief	Decrease volume by one step
 *
 * @return	0 for success,
 *		-ENXIO if the feature is disabled,
 *		error otherwise
 */
int le_audio_volume_down(void);

/**
 * @brief	Mute volume
 *
 * @return	0 for success,
 *		-ENXIO if the feature is disabled,
 *		error otherwise
 */
int le_audio_volume_mute(void);

/**
 * @brief	UnMute volume
 *
 * @return	0 for success,
 *		-ENXIO if the feature is disabled,
 *		error otherwise
 */
int le_audio_volume_unmute(void);

/**
 * @brief	Either resume or pause the Bluetooth LE Audio stream,
 *              depending on the current state of the stream
 *
 * @return	0 for success, error otherwise
 */
int le_audio_play_pause(void);

/**
 * @brief	for local play to select next Bluetooth LE Audio stream,
 *              depending on the current state of the stream
 *
 * @return	0 for success, error otherwise
 */
int le_audio_play_next(void);

/**
 * @brief	for local play to select next Bluetooth LE Audio stream,
 *              depending on the current state of the stream
 *
 * @return	0 for success, error otherwise
 */
int le_audio_play_previous(void);

/**
 * @brief Send Bluetooth LE Audio data
 *
 * @param data	Data to send
 * @param size	Size of data to send
 *
 * @return	0 for success,
 *		-ENXIO if the feature is disabled,
 *		error otherwise
 */
int le_audio_send(uint8_t const *const data, size_t size);

/**
 * @brief Enable Bluetooth LE Audio
 *
 * @param recv_cb	Callback for receiving Bluetooth LE Audio data
 * @param timestmp_cb	Callback for using the timestamp
 * @param feeding_cb	Callback for feeding Bluetooth LE Audio data
 *
 * @return		0 for success, error otherwise
 */
int le_audio_enable(le_audio_receive_cb recv_cb, le_audio_timestamp_cb timestamp_cb,
                    le_audio_feeding_cb feeding_cb);
/**
 * @brief Disable Bluetooth LE Audio
 *
 * @return	0 for success, error otherwise
 */
int le_audio_disable(void);

/**
 * @brief Select input source for Bluetooth LE Audio
 *
 * @param datasource  data source handler for Bluetooth LE Audio input data
 *
 * @return		0 for success, error otherwise
 */
int le_audio_set_data_source();

/**
 * This interface is expected to be called before 'bt_enable()'.
 * The interface is an alternative to initialize BLE related
 * resources by application developer before 'bt_enable()'.
 * For example, the 'queue_read_cb' for the ISO link can be
 * registered here before the RPMSG is initialized.
 */
int le_audio_init();

#endif /* _LE_AUDIO_H_ */
