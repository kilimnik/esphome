import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

DEPENDENCIES = ["ethernet"]

UNIT_SPEED = "Mbps"
CONF_LINK_SPEED = "link_speed"
CONF_DUPLEX_MODE = "duplex_mode"

ethernet_info_ns = cg.esphome_ns.namespace("ethernet_info")

LinkSpeedEthernetInfo = ethernet_info_ns.class_(
    "LinkSpeedEthernetInfo", sensor.Sensor, cg.PollingComponent
)
DuplexModeEthernetInfo = ethernet_info_ns.class_(
    "DuplexModeEthernetInfo", sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_LINK_SPEED): sensor.sensor_schema(
            LinkSpeedEthernetInfo,
            unit_of_measurement=UNIT_SPEED,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_DUPLEX_MODE): sensor.sensor_schema(
            DuplexModeEthernetInfo,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    if conf := config.get(CONF_LINK_SPEED):
        sens = await sensor.new_sensor(conf)
        await cg.register_component(sens, conf)

    if conf := config.get(CONF_DUPLEX_MODE):
        sens = await sensor.new_sensor(conf)
        await cg.register_component(sens, conf)
