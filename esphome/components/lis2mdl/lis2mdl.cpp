#include "lis2mdl.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace lis2mdl {

static const char *const TAG = "lis2mdl";

const float SENSITIVITY = 1.500f;

const char *data_rate_to_string(DataRate data_rate) {
  switch (data_rate) {
    case DataRate::ODR_10HZ:
      return "10 Hz";
    case DataRate::ODR_20HZ:
      return "20 Hz";
    case DataRate::ODR_50HZ:
      return "50 Hz";
    case DataRate::ODR_100HZ:
      return "100 Hz";
    default:
      return "Unknown";
  }
}

void LIS2MDLComponent::setup() {
  ESP_LOGCONFIG(TAG, "Running setup");

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::WHO_AM_I));

  if (reg.get() != LIS2MDL_ID) {
    ESP_LOGE(TAG, "Part ID is wrong or missing. Got 0x%02X", reg.get());
    this->mark_failed();
    return;
  }

  this->setup_temp_compensation_(this->temp_compensation_);
  this->setup_block_data_update_(this->block_data_update_);
  this->setup_odr_(this->datarate_);
  this->setup_mode_(this->mode_);
}

void LIS2MDLComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "LIS2MDL:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }

  ESP_LOGCONFIG(TAG, "  Block Data Update: %s", YESNO(this->block_data_update_));
  ESP_LOGCONFIG(TAG, "  Data Rate: %s", data_rate_to_string(this->datarate_));
  LOG_UPDATE_INTERVAL(this);

  LOG_SENSOR("  ", "Field Strength X", this->field_strength_x_sensor_);
  LOG_SENSOR("  ", "Field Strength Y", this->field_strength_y_sensor_);
  LOG_SENSOR("  ", "Field Strength Z", this->field_strength_z_sensor_);
}

void LIS2MDLComponent::read_data_() {
  RegStatusReg status_reg;
  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::STATUS_REG));
  status_reg.raw = reg.get();

  auto raw_to_x_bit = [](uint16_t lsb, uint16_t msb) -> uint16_t { return ((msb << 8) | lsb); };

  if (status_reg.x_data_available) {
    auto reg_x_l = this->reg(static_cast<uint8_t>(RegisterMap::OUTX_L_REG));
    auto reg_x_h = this->reg(static_cast<uint8_t>(RegisterMap::OUTX_H_REG));

    this->data_.lsb_x = raw_to_x_bit(reg_x_l.get(), reg_x_h.get());
    this->data_.x = this->data_.lsb_x * SENSITIVITY;
  }

  if (status_reg.y_data_available) {
    auto reg_y_l = this->reg(static_cast<uint8_t>(RegisterMap::OUTY_L_REG));
    auto reg_y_h = this->reg(static_cast<uint8_t>(RegisterMap::OUTY_H_REG));

    this->data_.lsb_y = raw_to_x_bit(reg_y_l.get(), reg_y_h.get());
    this->data_.y = this->data_.lsb_y * SENSITIVITY;
  }

  if (status_reg.z_data_available) {
    auto reg_z_l = this->reg(static_cast<uint8_t>(RegisterMap::OUTZ_L_REG));
    auto reg_z_h = this->reg(static_cast<uint8_t>(RegisterMap::OUTZ_H_REG));

    this->data_.lsb_z = raw_to_x_bit(reg_z_l.get(), reg_z_h.get());
    this->data_.z = this->data_.lsb_z * SENSITIVITY;
  }
}

void LIS2MDLComponent::loop() {
  if (!this->is_ready()) {
    return;
  }

  this->read_data_();
}

void LIS2MDLComponent::update() {
  ESP_LOGV(TAG, "Updating LIS2MDLComponent...");

  if (!this->is_ready()) {
    ESP_LOGV(TAG, "Component LIS2MDLComponent not ready for update");
    return;
  }

  if (this->field_strength_x_sensor_ != nullptr)
    this->field_strength_x_sensor_->publish_state(this->data_.x);
  if (this->field_strength_y_sensor_ != nullptr)
    this->field_strength_y_sensor_->publish_state(this->data_.y);
  if (this->field_strength_z_sensor_ != nullptr)
    this->field_strength_z_sensor_->publish_state(this->data_.z);

  this->status_clear_warning();
}

float LIS2MDLComponent::get_setup_priority() const { return setup_priority::DATA; }

void LIS2MDLComponent::setup_block_data_update_(bool enable) {
  RegCFGRegC ctrl_reg;

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::CFG_REG_C));
  ctrl_reg.raw = reg.get();

  ctrl_reg.block_data_update = enable;

  reg = ctrl_reg.raw;
}

void LIS2MDLComponent::setup_temp_compensation_(bool enable) {
  RegCFGRegA ctrl_reg;

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::CFG_REG_A));
  ctrl_reg.raw = reg.get();

  ctrl_reg.temp_compensation = enable;

  reg = ctrl_reg.raw;
}

void LIS2MDLComponent::setup_odr_(DataRate rate) {
  RegCFGRegA reg_odr;

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::CFG_REG_A));
  reg_odr.raw = reg.get();

  reg_odr.datarate = rate;

  reg = reg_odr.raw;
}

void LIS2MDLComponent::setup_mode_(Mode mode) {
  RegCFGRegA reg_odr;

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::CFG_REG_A));
  reg_odr.raw = reg.get();

  reg_odr.mode = mode;

  reg = reg_odr.raw;
}
}  // namespace lis2mdl
}  // namespace esphome
