#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace lis2dh12 {

// LIS2DH12 register map
enum class RegisterMap : uint8_t {
  STATUS_REG_AUX = 0x07,
  OUT_TEMP_L = 0x0C,
  OUT_TEMP_H = 0x0D,
  WHO_AM_I = 0x0F,
  CTRL_REG0 = 0x1E,
  TEMP_CFG_REG = 0x1F,
  CTRL_REG1 = 0x20,
  CTRL_REG2 = 0x21,
  CTRL_REG3 = 0x22,
  CTRL_REG4 = 0x23,
  CTRL_REG5 = 0x24,
  CTRL_REG6 = 0x25,
  REFERENCE = 0x26,
  STATUS_REG = 0x27,
  OUT_X_L = 0x28,
  OUT_X_H = 0x29,
  OUT_Y_L = 0x2A,
  OUT_Y_H = 0x2B,
  OUT_Z_L = 0x2C,
  OUT_Z_H = 0x2D,
  FIFO_CTRL_REG = 0x2E,
  FIFO_SRC_REG = 0x2F,
  INT1_CFG = 0x30,
  INT1_SRC = 0x31,
  INT1_THS = 0x32,
  INT1_DURATION = 0x33,
  INT2_CFG = 0x34,
  INT2_SRC = 0x35,
  INT2_THS = 0x36,
  INT2_DURATION = 0x37,
  CLICK_CFG = 0x38,
  CLICK_SRC = 0x39,
  CLICK_THS = 0x3A,
  TIME_LIMIT = 0x3B,
  TIME_LATENCY = 0x3C,
  TIME_WINDOW = 0x3D,
  ACT_THS = 0x3E,
  ACT_DUR = 0x3F,
};

enum class Resolution : uint8_t {
  RES_8BIT = 0,
  RES_10BIT = 1,
  RES_12BIT = 2,
};

enum class Range : uint8_t {
  RANGE_2G = 0b00,
  RANGE_4G = 0b01,
  RANGE_8G = 0b10,
  RANGE_16G = 0b11,
};

enum class DataRate : uint8_t {
  ODR_OFF = 0b0000,
  ODR_1HZ = 0b0001,
  ODR_10HZ = 0b0010,
  ODR_25HZ = 0b0011,
  ODR_50HZ = 0b0100,
  ODR_100HZ = 0b0101,
  ODR_200HZ = 0b0110,
  ODR_400HZ = 0b0111,
  ODR_1620HZ = 0b1000,         // not available in normal or high resolution power mode
  ODR_1344HZ_5376HZ = 0b1001,  // 5.576 kHz in low power mode, 1.344 kHz in normal or high resolution power mode
};

enum class TempEnable : uint8_t {
  TEMP_ENABLE = 0b00,
  TEMP_DISABLE = 0b11,
};

enum class HighPassFilterMode : uint8_t {
  HPM_NORMAL = 0b00,
  HPM_REFERENCE_SIGNAL = 0b01,
  HPM_NORMAL2 = 0b10,
  HPM_AUTORESET = 0b11,
};

enum class SelfTestMode : uint8_t { ST_NORMAL = 0b00, ST_0 = 0b01, ST_1 = 0b10 };

// 0x00
union RegStatusRegAux {
  struct {
    bool reserved_0 : 1;
    bool reserved_1 : 1;
    bool temperature_new_data_available : 1;
    bool reserved_3 : 1;
    bool reserved_4 : 1;
    bool reserved_5 : 1;
    bool temperature_data_overrun : 1;
    bool reserved_7 : 1;
  } __attribute__((packed));
  uint8_t raw;
};

// 0x0C
union RegOutTempLow {
  uint8_t value;
};

// 0x0D
union RegOutTempHigh {
  uint8_t value;
};

#define LIS2DH12_ID 0x33
// 0x0F
union RegWhoAmI {
  uint8_t id{0x33};
};

// 0x1E
union RegCTRLReg0 {
  struct {
    bool reserved_0 : 1;
    bool reserved_1 : 1;
    bool reserved_2 : 1;
    bool reserved_3 : 1;
    bool reserved_4 : 1;
    bool reserved_5 : 1;
    bool reserved_6 : 1;
    bool sdo_pullup_disconnect : 1;
  } __attribute__((packed));
  uint8_t raw{0x10};
};

// 0x1F
union RegTempCFGReg {
  struct {
    bool reserved_0 : 1;
    bool reserved_1 : 1;
    bool reserved_2 : 1;
    bool reserved_3 : 1;
    bool reserved_4 : 1;
    bool reserved_5 : 1;
    TempEnable temp_en : 2;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x20
union RegCRTLReg1 {
  struct {
    bool x_axis_enable : 1;
    bool y_axis_enable : 1;
    bool z_axis_enable : 1;
    bool low_power_mode_enable : 1;
    DataRate odr : 4;
  } __attribute__((packed));
  uint8_t raw{0x07};
};

// 0x21
union RegCRTLReg2 {
  struct {
    bool high_pass_filter_int1 : 1;
    bool high_pass_filter_int2 : 1;
    bool high_pass_filter_clk : 1;
    bool filtered_data_selection : 1;
    uint8_t high_pass_cutoff_frequency : 2;
    HighPassFilterMode high_pass_filter_mode : 2;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x22
union RegCRTLReg3 {
  struct {
    bool reserved_0 : 1;
    bool interrupt1_overrun : 1;
    bool interrupt1_watermark : 1;
    bool reserved_3 : 1;
    bool interrupt1_zyxda : 1;
    bool interrupt1_ia2 : 1;
    bool interrupt1_ia1 : 1;
    bool interrupt1_click : 1;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x23
union RegCRTLReg4 {
  struct {
    bool spi_serial_mode : 1;
    SelfTestMode self_test : 2;
    bool high_resolution : 1;
    Range full_scale : 2;
    bool big_endian : 1;
    bool block_data_update : 1;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x24
union RegCRTLReg5 {
  struct {
    bool int2_4d_detection : 1;
    bool int2_latch_interrupt : 1;
    bool int1_4d_detection : 1;
    bool int1_latch_interrupt : 1;
    bool fifo_enable : 1;
    bool reserved_4 : 1;
    bool reserved_5 : 1;
    bool reboot : 1;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x25
union RegCRTLReg6 {
  struct {
    bool reserved_0 : 1;
    bool int_polarity : 1;
    bool reserved_2 : 1;
    bool interrupt2_activity_en : 1;
    bool interrupt2_boot_en : 1;
    bool interrupt2_ia2 : 1;
    bool interrupt2_ia1 : 1;
    bool interrupt2_click : 1;
  } __attribute__((packed));
  uint8_t raw{0x00};
};

// 0x27
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
  uint8_t raw;
};

// 0x28
union REGOutXLow {
  uint8_t value;
};

// 0x29
union REGOutXHigh {
  uint8_t value;
};

// 0x2A
union REGOutYLow {
  uint8_t value;
};

// 0x2B
union REGOutYHigh {
  uint8_t value;
};

// 0x2C
union REGOutZLow {
  uint8_t value;
};

// 0x2D
union REGOutZHigh {
  uint8_t value;
};

class LIS2DH12Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;

  void loop() override;
  void update() override;

  float get_setup_priority() const override;

  void set_offset(float offset_x, float offset_y, float offset_z);
  void set_range(Range range) { this->range_ = range; }
  void set_odr(DataRate datarate) { this->datarate_ = datarate; }
  void set_resolution(Resolution resolution) { this->resolution_ = resolution; }

  SUB_SENSOR(acceleration_x)
  SUB_SENSOR(acceleration_y)
  SUB_SENSOR(acceleration_z)
  SUB_SENSOR(temperature)

 protected:
  bool block_data_update_{false};
  DataRate datarate_{DataRate::ODR_10HZ};
  Range range_{Range::RANGE_2G};
  Resolution resolution_{Resolution::RES_10BIT};

  struct {
    int16_t lsb_x, lsb_y, lsb_z, lsb_temp;
    float x, y, z, temp;
  } data_{};

  void setup_block_data_update_(bool enable);
  void setup_odr_(DataRate rate);
  void setup_resolution_(Resolution resolution);
  void setup_range_(Range range);

  void read_data_();
};

}  // namespace lis2dh12
}  // namespace esphome
