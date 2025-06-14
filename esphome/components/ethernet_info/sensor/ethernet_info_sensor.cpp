#include "ethernet_info_sensor.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace ethernet_info {

static const char *const TAG = "ethernet_info";

void LinkSpeedEthernetInfo::dump_config() { LOG_SENSOR("", "EthernetInfo Link Speed", this); }
void DuplexModeEthernetInfo::dump_config() { LOG_SENSOR("", "EthernetInfo Duplex Mode", this); }

}  // namespace ethernet_info
}  // namespace esphome

#endif  // USE_ESP32
