#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace lis2mdl {

// LIS2MDL register map
enum class RegisterMap : uint8_t {
  OFFSET_X_REG_L = 0x45,
  OFFSET_X_REG_H = 0x46,
  OFFSET_Y_REG_L = 0x47,
  OFFSET_Y_REG_H = 0x48,
  OFFSET_Z_REG_L = 0x49,
  OFFSET_Z_REG_H = 0x4A,
  WHO_AM_I = 0x4F,
  CFG_REG_A = 0x60,
  CFG_REG_B = 0x61,
  CFG_REG_C = 0x62,
  INT_CTRL_REG = 0x63,
  INT_SOURCE_REG = 0x64,
  INT_THS_L_REG = 0x65,
  INT_THS_H_REG = 0x66,
  STATUS_REG = 0x67,
  OUTX_L_REG = 0x68,
  OUTX_H_REG = 0x69,
  OUTY_L_REG = 0x6A,
  OUTY_H_REG = 0x6B,
  OUTZ_L_REG = 0x6C,
  OUTZ_H_REG = 0x6D,
  TEMP_OUT_L_REG = 0x6E,
  TEMP_OUT_H_REG = 0x6F,
};

enum class Mode : uint8_t {
  MD_CONTINUOUS = 0b00,
  MD_SINGLE = 0b01,
  MD_IDLE1 = 0b10,
  MD_IDLE2 = 0b11,
};

enum class DataRate : uint8_t {
  ODR_10HZ = 0b00,
  ODR_20HZ = 0b01,
  ODR_50HZ = 0b10,
  ODR_100HZ = 0b11,
};

#define LIS2MDL_ID 0x40
// 0x4F
union RegWhoAmI {
  uint8_t id{0x40};
};

// 0x60
union RegCFGRegA {
  struct {
    Mode mode : 2;
    DataRate datarate : 2;
    bool low_power : 1;
    bool reset : 1;
    bool reboot : 1;
    bool temp_compensation : 1;
  } __attribute__((packed));
  uint8_t raw{0x03};
};

// 0x61
union RegCFGRegB {
  struct {
    bool low_pass_filter : 1;
    bool enable_offset_cancellation : 1;
    bool pulse_frequency : 1;
    bool int_on_data_off : 1;
    bool offset_cancellation_single_mode : 1;
    bool reserved_5 : 1;
    bool reserved_6 : 1;
    bool reserved_7 : 1;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x62
union RegCFGRegC {
  struct {
    bool data_ready_int : 1;
    bool self_test : 1;
    bool spi_4wire : 1;
    bool big_endian : 1;
    bool block_data_update : 1;
    bool i2c_dis : 1;
    bool int_on_pin : 1;
    bool reserved_7 : 1;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x67
union RegStatusReg {
  struct {
    bool x_data_available : 1;
    bool y_data_available : 1;
    bool z_data_available : 1;
    bool xyz_data_available : 1;
    bool x_data_overrun : 1;
    bool y_data_overrun : 1;
    bool z_data_overrun : 1;
    bool xyz_data_overrun : 1;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

class LIS2MDLComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;

  void loop() override;
  void update() override;

  float get_setup_priority() const override;

  void set_mode(Mode mode) { this->mode_ = mode; }
  void set_odr(DataRate datarate) { this->datarate_ = datarate; }

  SUB_SENSOR(field_strength_x)
  SUB_SENSOR(field_strength_y)
  SUB_SENSOR(field_strength_z)
  SUB_SENSOR(temperature)

 protected:
  bool block_data_update_{false};
  bool temp_compensation_{true};
  Mode mode_{Mode::MD_CONTINUOUS};
  DataRate datarate_{DataRate::ODR_100HZ};

  struct {
    int16_t lsb_x, lsb_y, lsb_z, lsb_temp;
    float x, y, z, temp;
  } data_{};

  void setup_block_data_update_(bool enable);
  void setup_temp_compensation_(bool enable);
  void setup_odr_(DataRate rate);
  void setup_mode_(Mode mode);

  void read_data_();
};

}  // namespace lis2mdl
}  // namespace esphome
