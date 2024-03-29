/*
 * Copyright (c) 2012-2017 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */

#ifndef _OL_TXRX__H_
#define _OL_TXRX__H_

#include <qdf_nbuf.h>           /* qdf_nbuf_t */
#include <cdp_txrx_cmn.h>       /* ol_txrx_vdev_t, etc. */
#include "cds_sched.h"

/*
 * Pool of tx descriptors reserved for
 * high-priority traffic, such as ARP/EAPOL etc
 * only for forwarding path.
 */
#define OL_TX_NON_FWD_RESERVE	100
#define OL_TXRX_PEER_UNREF_DELETE(peer) \
	ol_txrx_peer_unref_delete(peer, __func__, __LINE__)

int ol_txrx_peer_unref_delete(ol_txrx_peer_handle peer,
					      const char *fname,
					      int line);

ol_txrx_peer_handle ol_txrx_find_peer_by_addr_inc_ref(ol_txrx_pdev_handle pdev,
						uint8_t *peer_addr,
						uint8_t *peer_id);
/**
 * ol_tx_desc_pool_size_hl() - allocate tx descriptor pool size for HL systems
 * @ctrl_pdev: the control pdev handle
 *
 * Return: allocated pool size
 */
u_int16_t
ol_tx_desc_pool_size_hl(ol_pdev_handle ctrl_pdev);

#ifndef OL_TX_AVG_FRM_BYTES
#define OL_TX_AVG_FRM_BYTES 1000
#endif

#ifndef OL_TX_DESC_POOL_SIZE_MIN_HL
#define OL_TX_DESC_POOL_SIZE_MIN_HL 500
#endif

#ifndef OL_TX_DESC_POOL_SIZE_MAX_HL
#define OL_TX_DESC_POOL_SIZE_MAX_HL 5000
#endif

#ifndef FW_STATS_DESC_POOL_SIZE
#define FW_STATS_DESC_POOL_SIZE 10
#endif

#ifdef CONFIG_PER_VDEV_TX_DESC_POOL
#define TXRX_HL_TX_FLOW_CTRL_VDEV_LOW_WATER_MARK 400
#define TXRX_HL_TX_FLOW_CTRL_MGMT_RESERVED 100
#endif

#ifdef CONFIG_TX_DESC_HI_PRIO_RESERVE
#define TXRX_HL_TX_DESC_HI_PRIO_RESERVED 20
#endif

#if defined(CONFIG_HL_SUPPORT) && defined(FEATURE_WLAN_TDLS)

void
ol_txrx_hl_tdls_flag_reset(struct ol_txrx_vdev_t *vdev, bool flag);
#else

static inline void
ol_txrx_hl_tdls_flag_reset(struct ol_txrx_vdev_t *vdev, bool flag)
{
}
#endif

#if defined(CONFIG_HL_SUPPORT) && defined(FEATURE_WLAN_TDLS)
void
ol_txrx_copy_mac_addr_raw(ol_txrx_vdev_handle vdev, uint8_t *bss_addr);

void
ol_txrx_add_last_real_peer(ol_txrx_pdev_handle pdev,
			   ol_txrx_vdev_handle vdev,
			   uint8_t *peer_id);

bool
is_vdev_restore_last_peer(struct ol_txrx_peer_t *peer);

void
ol_txrx_update_last_real_peer(
	ol_txrx_pdev_handle pdev,
	struct ol_txrx_peer_t *peer,
	uint8_t *peer_id, bool restore_last_peer);
#else

static inline void
ol_txrx_copy_mac_addr_raw(ol_txrx_vdev_handle vdev, uint8_t *bss_addr)
{
}

static inline void
ol_txrx_add_last_real_peer(ol_txrx_pdev_handle pdev,
			   ol_txrx_vdev_handle vdev, uint8_t *peer_id)
{
}

static inline bool
is_vdev_restore_last_peer(struct ol_txrx_peer_t *peer)
{
	return  false;
}

static inline void
ol_txrx_update_last_real_peer(
	ol_txrx_pdev_handle pdev,
	struct ol_txrx_peer_t *peer,
	uint8_t *peer_id, bool restore_last_peer)

{
}
#endif

/**
 * ol_txrx_dump_pkt() - display the data in buffer and buffer's address
 * @nbuf: buffer which contains data to be displayed
 * @nbuf_paddr: physical address of the buffer
 * @len: defines the size of the data to be displayed
 *
 * Return: None
 */
void
ol_txrx_dump_pkt(qdf_nbuf_t nbuf, uint32_t nbuf_paddr, int len);


/**
 * ol_txrx_post_data_stall_event() - post data stall event
 * @indicator: Module triggering data stall
 * @data_stall_type: data stall event type
 * @pdev_id: pdev id
 * @vdev_id_bitmap: vdev id bitmap
 * @recovery_type: data stall recovery type
 *
 * Return: None
 */

void ol_txrx_post_data_stall_event(
			enum data_stall_log_event_indicator indicator,
			enum data_stall_log_event_type data_stall_type,
			uint32_t pdev_id, uint32_t vdev_id_bitmap,
			enum data_stall_log_recovery_type recovery_type);

/**
 * ol_txrx_fwd_desc_thresh_check() - check to forward packet to tx path
 * @vdev: which virtual device the frames were addressed to
 *
 * This API is to check whether enough descriptors are available or not
 * to forward packet to tx path. If not enough descriptors left,
 * start dropping tx-path packets.
 * Do not pause netif queues as still a pool of descriptors is reserved
 * for high-priority traffic such as EAPOL/ARP etc.
 * In case of intra-bss forwarding, it could be possible that tx-path can
 * consume all the tx descriptors and pause netif queues. Due to this,
 * there would be some left for stack triggered packets such as ARP packets
 * which could lead to disconnection of device. To avoid this, reserved
 * a pool of descriptors for high-priority packets, i.e., reduce the
 * threshold of drop in the intra-bss forwarding path.
 *
 * Return: true ; forward the packet, i.e., below threshold
 *         false; not enough descriptors, drop the packet
 */
bool ol_txrx_fwd_desc_thresh_check(struct ol_txrx_vdev_t *vdev);

ol_txrx_vdev_handle ol_txrx_get_vdev_from_vdev_id(uint8_t vdev_id);

void htt_pkt_log_init(struct ol_txrx_pdev_t *handle, void *scn);
QDF_STATUS ol_txrx_set_wisa_mode(ol_txrx_vdev_handle vdev,
			bool enable);
void ol_txrx_update_mac_id(uint8_t vdev_id, uint8_t mac_id);
void ol_txrx_peer_detach_force_delete(ol_txrx_peer_handle peer);
void peer_unmap_timer_handler(void *data);

int ol_txrx_fw_stats_desc_pool_init(struct ol_txrx_pdev_t *pdev,
				    uint8_t pool_size);
void ol_txrx_fw_stats_desc_pool_deinit(struct ol_txrx_pdev_t *pdev);
struct ol_txrx_fw_stats_desc_t
	*ol_txrx_fw_stats_desc_alloc(struct ol_txrx_pdev_t
				     *pdev);
struct ol_txrx_stats_req_internal *ol_txrx_fw_stats_desc_get_req(struct
	ol_txrx_pdev_t *pdev, uint8_t desc_id);

void ol_txrx_peer_flush_frags(ol_txrx_pdev_handle pdev, uint8_t vdev_id, uint8_t *peer_mac);
#endif /* _OL_TXRX__H_ */
