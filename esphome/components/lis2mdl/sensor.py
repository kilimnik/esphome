import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_FIELD_STRENGTH_X,
    CONF_FIELD_STRENGTH_Y,
    CONF_FIELD_STRENGTH_Z,
    CONF_ID,
    ICON_MAGNET,
    STATE_CLASS_MEASUREMENT,
)

DEPENDENCIES = ["i2c"]

CONF_LIS2MDL_ID = "lis2mdl_id"
FIELD_STRENGTH_SENSORS = (
    CONF_FIELD_STRENGTH_X,
    CONF_FIELD_STRENGTH_Y,
    CONF_FIELD_STRENGTH_Z,
)

lis2mdl_ns = cg.esphome_ns.namespace("lis2mdl")
LIS2MDLComponent = lis2mdl_ns.class_(
    "LIS2MDLComponent", cg.PollingComponent, i2c.I2CDevice
)

field_strength_schema = sensor.sensor_schema(
    unit_of_measurement="mG",
    icon=ICON_MAGNET,
    accuracy_decimals=1,
    state_class=STATE_CLASS_MEASUREMENT,
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LIS2MDLComponent),
            cv.Optional(CONF_FIELD_STRENGTH_X): field_strength_schema,
            cv.Optional(CONF_FIELD_STRENGTH_Y): field_strength_schema,
            cv.Optional(CONF_FIELD_STRENGTH_Z): field_strength_schema,
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(i2c.i2c_device_schema(0x1E))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    for filed_strength_key in FIELD_STRENGTH_SENSORS:
        if field_strength := config.get(filed_strength_key):
            sens = await sensor.new_sensor(field_strength)
            cg.add(getattr(var, f"set_{filed_strength_key}_sensor")(sens))
