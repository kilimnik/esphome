#include "lis2dh12.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace lis2dh12 {

static const char *const TAG = "lis2dh12";

const float GRAVITY_EARTH = 9.80665f;

const char *res_to_string(Resolution resolution) {
  switch (resolution) {
    case Resolution::RES_12BIT:
      return "12-bit";
    case Resolution::RES_10BIT:
      return "10-bit";
    case Resolution::RES_8BIT:
      return "8-bit";
    default:
      return "Unknown";
  }
}

const char *range_to_string(Range range) {
  switch (range) {
    case Range::RANGE_2G:
      return "±2g";
    case Range::RANGE_4G:
      return "±4g";
    case Range::RANGE_8G:
      return "±8g";
    case Range::RANGE_16G:
      return "±16g";
    default:
      return "Unknown";
  }
}

const char *data_rate_to_string(DataRate data_rate) {
  switch (data_rate) {
    case DataRate::ODR_OFF:
      return "Off";
    case DataRate::ODR_1HZ:
      return "1 Hz";
    case DataRate::ODR_10HZ:
      return "10 Hz";
    case DataRate::ODR_25HZ:
      return "25 Hz";
    case DataRate::ODR_50HZ:
      return "50 Hz";
    case DataRate::ODR_100HZ:
      return "100 Hz";
    case DataRate::ODR_200HZ:
      return "200 Hz";
    case DataRate::ODR_400HZ:
      return "400 Hz";
    case DataRate::ODR_1620HZ:
      return "1.62 kHz";
    case DataRate::ODR_1344HZ_5376HZ:
      return "1.344 kHz or 5.376 kHz";
    default:
      return "Unknown";
  }
}

const float res_to_factor(Resolution resolution) {
  switch (resolution) {
    case Resolution::RES_12BIT:
      return 1.0f;
    case Resolution::RES_10BIT:
      return 4.0f;
    case Resolution::RES_8BIT:
      return 16.0f;
  }

  return 0.0f;
}

const float range_to_factor(Range range) {
  switch (range) {
    case Range::RANGE_2G:
      return 1.0f;
    case Range::RANGE_4G:
      return 2.0f;
    case Range::RANGE_8G:
      return 4.0f;
    case Range::RANGE_16G:
      return 12.0f;
  }

  return 0.0f;
}

void LIS2DH12Component::setup() {
  ESP_LOGCONFIG(TAG, "Running setup");

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::WHO_AM_I));

  if (reg.get() != LIS2DH12_ID) {
    ESP_LOGE(TAG, "Part ID is wrong or missing. Got 0x%02X", reg.get());
    this->mark_failed();
    return;
  }

  this->setup_block_data_update_(this->block_data_update_);
  this->setup_odr_(this->datarate_);
  this->setup_resolution_(this->resolution_);
  this->setup_range_(this->range_);
}

void LIS2DH12Component::dump_config() {
  ESP_LOGCONFIG(TAG, "LIS2DH12:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }

  ESP_LOGCONFIG(TAG, "  Block Data Update: %s", YESNO(this->block_data_update_));
  ESP_LOGCONFIG(TAG, "  Data Rate: %s", data_rate_to_string(this->datarate_));
  ESP_LOGCONFIG(TAG, "  Range: %s", range_to_string(this->range_));
  ESP_LOGCONFIG(TAG, "  Resolution: %s", res_to_string(this->resolution_));
  LOG_UPDATE_INTERVAL(this);

  LOG_SENSOR("  ", "Acceleration X", this->acceleration_x_sensor_);
  LOG_SENSOR("  ", "Acceleration Y", this->acceleration_y_sensor_);
  LOG_SENSOR("  ", "Acceleration Z", this->acceleration_z_sensor_);
}

void LIS2DH12Component::read_data_() {
  RegStatusReg status_reg;
  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::STATUS_REG));
  status_reg.raw = reg.get();

  auto raw_to_x_bit = [](uint16_t lsb, uint16_t msb) -> uint16_t { return ((msb << 8) | lsb); };

  if (status_reg.x_data_available) {
    uint8_t accel_x_data[2];

    auto reg_x_l = this->reg(static_cast<uint8_t>(RegisterMap::OUT_X_L));
    auto reg_x_h = this->reg(static_cast<uint8_t>(RegisterMap::OUT_X_H));

    this->data_.lsb_x = raw_to_x_bit(reg_x_l.get(), reg_x_h.get());
    this->data_.x = (((float) this->data_.lsb_x / (res_to_factor(this->resolution_) * 16.0f)) *
                     (range_to_factor(this->range_) * res_to_factor(this->resolution_))) /
                    1000.0f * GRAVITY_EARTH;
  }

  if (status_reg.y_data_available) {
    uint8_t accel_y_data[2];

    auto reg_y_l = this->reg(static_cast<uint8_t>(RegisterMap::OUT_Y_L));
    auto reg_y_h = this->reg(static_cast<uint8_t>(RegisterMap::OUT_Y_H));

    this->data_.lsb_y = raw_to_x_bit(reg_y_l.get(), reg_y_h.get());
    this->data_.y = (((float) this->data_.lsb_y / (res_to_factor(this->resolution_) * 16.0f)) *
                     (range_to_factor(this->range_) * res_to_factor(this->resolution_))) /
                    1000.0f * GRAVITY_EARTH;
  }

  if (status_reg.z_data_available) {
    uint8_t accel_x_data[2];

    auto reg_z_l = this->reg(static_cast<uint8_t>(RegisterMap::OUT_Z_L));
    auto reg_z_h = this->reg(static_cast<uint8_t>(RegisterMap::OUT_Z_H));

    this->data_.lsb_z = raw_to_x_bit(reg_z_l.get(), reg_z_h.get());
    this->data_.z = (((float) this->data_.lsb_z / (res_to_factor(this->resolution_) * 16.0f)) *
                     (range_to_factor(this->range_) * res_to_factor(this->resolution_))) /
                    1000.0f * GRAVITY_EARTH;
  }
}

void LIS2DH12Component::loop() {
  if (!this->is_ready()) {
    return;
  }

  this->read_data_();
}

void LIS2DH12Component::update() {
  ESP_LOGV(TAG, "Updating LIS2DH12Component...");

  if (!this->is_ready()) {
    ESP_LOGV(TAG, "Component LIS2DH12Component not ready for update");
    return;
  }
  ESP_LOGV(TAG, "Acceleration: {x = %+1.3f m/s², y = %+1.3f m/s², z = %+1.3f m/s²}, temp=%.3f°C", this->data_.x,
           this->data_.y, this->data_.z, this->data_.temp);

  if (this->acceleration_x_sensor_ != nullptr)
    this->acceleration_x_sensor_->publish_state(this->data_.x);
  if (this->acceleration_y_sensor_ != nullptr)
    this->acceleration_y_sensor_->publish_state(this->data_.y);
  if (this->acceleration_z_sensor_ != nullptr)
    this->acceleration_z_sensor_->publish_state(this->data_.z);

  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(this->data_.temp);

  this->status_clear_warning();
}

float LIS2DH12Component::get_setup_priority() const { return setup_priority::DATA; }

void LIS2DH12Component::setup_block_data_update_(bool enable) {
  RegCRTLReg4 ctrl_reg;

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::CTRL_REG4));
  ctrl_reg.raw = reg.get();

  ctrl_reg.block_data_update = enable;

  reg = ctrl_reg.raw;
}

void LIS2DH12Component::setup_odr_(DataRate rate) {
  RegCRTLReg1 reg_odr;

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::CTRL_REG1));
  reg_odr.raw = reg.get();

  reg_odr.x_axis_enable = true;
  reg_odr.y_axis_enable = true;
  reg_odr.z_axis_enable = true;
  reg_odr.odr = rate;

  reg = reg_odr.raw;
}

void LIS2DH12Component::setup_resolution_(Resolution resolution) {
  RegCRTLReg1 ctrl_reg1;

  auto reg1 = this->reg(static_cast<uint8_t>(RegisterMap::CTRL_REG1));
  ctrl_reg1.raw = reg1.get();

  RegCRTLReg4 ctrl_reg4;

  auto reg4 = this->reg(static_cast<uint8_t>(RegisterMap::CTRL_REG4));
  ctrl_reg4.raw = reg4.get();

  switch (resolution) {
    case Resolution::RES_8BIT:
      ctrl_reg1.low_power_mode_enable = 1;
      ctrl_reg4.high_resolution = 0;
      break;
    case Resolution::RES_10BIT:
      ctrl_reg1.low_power_mode_enable = 0;
      ctrl_reg4.high_resolution = 0;
      break;
    case Resolution::RES_12BIT:
      ctrl_reg1.low_power_mode_enable = 0;
      ctrl_reg4.high_resolution = 1;
      break;
  }

  reg1 = ctrl_reg1.raw;
  reg4 = ctrl_reg4.raw;
}

void LIS2DH12Component::setup_range_(Range range) {
  RegCRTLReg4 ctrl_reg;

  auto reg = this->reg(static_cast<uint8_t>(RegisterMap::CTRL_REG4));
  ctrl_reg.raw = reg.get();

  ctrl_reg.full_scale = range;
  reg = ctrl_reg.raw;
}
}  // namespace lis2dh12
}  // namespace esphome
