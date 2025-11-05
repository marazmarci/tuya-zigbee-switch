#include "telink_size_t_hack.h"
#pragma pack(push, 1)
#include "tl_common.h"
#pragma pack(pop)

#include "hal/zigbee.h"
#include "telink_zigbee_hal.h"
#include "../../zigbee/consts.h"

/**
 * Main Zigbee HAL initialization - coordinates between network and ZCL modules
 */
void hal_zigbee_init(hal_zigbee_endpoint *endpoints, uint8_t endpoints_cnt) {
  // Initialize network layer (BDB, ZDO callbacks, basic Zigbee stack)
  telink_zigbee_hal_network_init();

  // Initialize ZCL layer (clusters, attributes, endpoints)
  telink_zigbee_hal_zcl_init(endpoints, endpoints_cnt);

  // Start BDB with the first endpoint descriptor
  telink_zigbee_hal_bdb_init(telink_zigbee_hal_zcl_get_descriptors());
}

/**
 * Get endpoint descriptors - delegates to ZCL module
 */
af_simple_descriptor_t *telink_zigbee_hal_get_descriptors(void) {
  return telink_zigbee_hal_zcl_get_descriptors();
}

aps_binding_entry_t *find_first_binding_entry(uint8_t src_endpoint, uint16_t cluster_id) {
  for (int i = 0; i < APS_BINDING_TABLE_SIZE; i++) {
    aps_binding_entry_t *entry = &g_apsBindingTbl[i];
    if (entry->srcEp == src_endpoint &&
        entry->clusterId == cluster_id &&
        entry->used) {
      return entry;
    }
  }
  return NULL;
}

bool aps_binding_dstaddr_equal(const aps_binding_entry_t *a, const aps_binding_entry_t *b) {
  if (a->dstAddrMode != b->dstAddrMode) {
    return false;
  }
  if (a->dstAddrMode == APS_SHORT_GROUPADDR_NOEP) {
    return a->groupAddr == b->groupAddr;
  }
  return memcmp(&a->dstExtAddrInfo, &b->dstExtAddrInfo, sizeof(aps_binding_extaddr_t)) == 0;
}

bool hal_zigbee_is_onoff_bound_to_same_dst_as_levelctrl(uint8_t src_endpoint) {

  // TODO support multiple binding entries for the same cluster
  aps_binding_entry_t *levelCtrlBindingEntry = find_first_binding_entry(src_endpoint, ZCL_CLUSTER_LEVEL_CONTROL);

  if (levelCtrlBindingEntry == NULL) {
    return false;
  }

  for (int i = 0; i < APS_BINDING_TABLE_SIZE; i++) {
    aps_binding_entry_t *entry = &g_apsBindingTbl[i];
    if (entry->srcEp == src_endpoint &&
        entry->clusterId == ZCL_CLUSTER_ON_OFF &&
        aps_binding_dstaddr_equal(levelCtrlBindingEntry, entry) &&
        entry->used) {
      return true;
    }
  }
  return false;
}
