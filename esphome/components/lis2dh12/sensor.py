import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ACCELERATION_X,
    CONF_ACCELERATION_Y,
    CONF_ACCELERATION_Z,
    CONF_ID,
    CONF_RANGE,
    CONF_RESOLUTION,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    ICON_ACCELERATION_X,
    ICON_ACCELERATION_Y,
    ICON_ACCELERATION_Z,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_METER_PER_SECOND_SQUARED,
)

DEPENDENCIES = ["i2c"]

CONF_LIS2DH12_ID = "lis2dh12_id"
ACCELERATION_SENSORS = (CONF_ACCELERATION_X, CONF_ACCELERATION_Y, CONF_ACCELERATION_Z)

lis2dh12_ns = cg.esphome_ns.namespace("lis2dh12")
LIS2DH12Component = lis2dh12_ns.class_(
    "LIS2DH12Component", cg.PollingComponent, i2c.I2CDevice
)

LISRange = lis2dh12_ns.enum("Range", True)
RANGES_LIS2DH12 = {
    "2G": LISRange.RANGE_2G,
    "4G": LISRange.RANGE_4G,
    "8G": LISRange.RANGE_8G,
    "16G": LISRange.RANGE_16G,
}

LISResolution = lis2dh12_ns.enum("Resolution", True)
RESOLUTIONS_LIS2DH12 = {
    12: LISResolution.RES_12BIT,
    10: LISResolution.RES_10BIT,
    8: LISResolution.RES_8BIT,
}

accel_schema = {
    "unit_of_measurement": UNIT_METER_PER_SECOND_SQUARED,
    "accuracy_decimals": 2,
    "state_class": STATE_CLASS_MEASUREMENT,
}

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LIS2DH12Component),
            cv.Optional(CONF_RANGE, default="2G"): cv.enum(RANGES_LIS2DH12, upper=True),
            cv.Optional(CONF_RESOLUTION, default=12): cv.enum(RESOLUTIONS_LIS2DH12),
            cv.Optional(CONF_ACCELERATION_X): sensor.sensor_schema(
                icon=ICON_ACCELERATION_X,
                **accel_schema,
            ),
            cv.Optional(CONF_ACCELERATION_Y): sensor.sensor_schema(
                icon=ICON_ACCELERATION_Y,
                **accel_schema,
            ),
            cv.Optional(CONF_ACCELERATION_Z): sensor.sensor_schema(
                icon=ICON_ACCELERATION_Z,
                **accel_schema,
            ),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(i2c.i2c_device_schema(0x18))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_range(RANGES_LIS2DH12[config[CONF_RANGE]]))
    cg.add(var.set_resolution(RESOLUTIONS_LIS2DH12[config[CONF_RESOLUTION]]))

    for accel_key in ACCELERATION_SENSORS:
        if accel := config.get(accel_key):
            sens = await sensor.new_sensor(accel)
            cg.add(getattr(var, f"set_{accel_key}_sensor")(sens))

    if temp := config.get(CONF_TEMPERATURE):
        sens = await sensor.new_sensor(temp)
        cg.add(var.set_temperature_sensor(sens))
