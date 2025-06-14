#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/ethernet/ethernet_component.h"

#ifdef USE_ESP32

namespace esphome {
namespace ethernet_info {

class LinkSpeedEthernetInfo : public PollingComponent, public sensor::Sensor {
 public:
  void update() override {
    auto link_speed = (ethernet::global_eth_component->get_link_speed() == ETH_SPEED_100M) ? 100 : 10;

    if (link_speed != this->last_results_) {
      this->last_results_ = link_speed;
      this->publish_state(link_speed);
    }
  }
  float get_setup_priority() const override { return setup_priority::ETHERNET; }
  std::string unique_id() override { return get_mac_address() + "-ethernetinfo-link-speed"; }
  void dump_config() override;

 protected:
  int last_results_;
};

class DuplexModeEthernetInfo : public PollingComponent, public sensor::Sensor {
 public:
  void update() override {
    auto duplex_mode = (ethernet::global_eth_component->get_duplex_mode() == ETH_DUPLEX_FULL);

    if (duplex_mode != this->last_results_) {
      this->last_results_ = duplex_mode;
      this->publish_state(duplex_mode);
    }
  }
  float get_setup_priority() const override { return setup_priority::ETHERNET; }
  std::string unique_id() override { return get_mac_address() + "-ethernetinfo-duplex-mode"; }
  void dump_config() override;

 protected:
  bool last_results_;
};

}  // namespace ethernet_info
}  // namespace esphome

#endif  // USE_ESP32
