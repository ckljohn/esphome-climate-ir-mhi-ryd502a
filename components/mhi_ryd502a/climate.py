import esphome.codegen as cg
from esphome.components import climate_ir
import esphome.config_validation as cv
from esphome.types import ConfigType

ns = cg.esphome_ns.namespace("mhi_ryd502a")

CODEOWNERS = ["@johncheng"]

AUTO_LOAD = ["climate_ir"]
climate = ns.class_("MhiClimate", climate_ir.ClimateIR)

# Default horizontal direction
CONF_HORIZONTAL_DEFAULT = "horizontal_default"
HorizontalDirections = ns.enum("HorizontalDirection")
HORIZONTAL_DIRECTIONS = {
    "left": HorizontalDirections.HORIZONTAL_DIRECTION_LEFT,
    "middle-left": HorizontalDirections.HORIZONTAL_DIRECTION_MIDDLE_LEFT,
    "middle": HorizontalDirections.HORIZONTAL_DIRECTION_MIDDLE,
    "middle-right": HorizontalDirections.HORIZONTAL_DIRECTION_MIDDLE_RIGHT,
    "right": HorizontalDirections.HORIZONTAL_DIRECTION_RIGHT,
    "split": HorizontalDirections.HORIZONTAL_DIRECTION_SPLIT,
    "focus": HorizontalDirections.HORIZONTAL_DIRECTION_FOCUS,
}

# Default vertical direction
CONF_VERTICAL_DEFAULT = "vertical_default"
VerticalDirections = ns.enum("VerticalDirection")
VERTICAL_DIRECTIONS = {
    "up": VerticalDirections.VERTICAL_DIRECTION_UP,
    "middle-up": VerticalDirections.VERTICAL_DIRECTION_MIDDLE_UP,
    "middle": VerticalDirections.VERTICAL_DIRECTION_MIDDLE,
    "middle-down": VerticalDirections.VERTICAL_DIRECTION_MIDDLE_DOWN,
    "down": VerticalDirections.VERTICAL_DIRECTION_DOWN,
}

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(climate).extend(
    {
        cv.Optional(CONF_HORIZONTAL_DEFAULT, default="middle"): cv.enum(
            HORIZONTAL_DIRECTIONS
        ),
        cv.Optional(CONF_VERTICAL_DEFAULT, default="middle"): cv.enum(
            VERTICAL_DIRECTIONS
        ),
    }
)


async def to_code(config: ConfigType) -> None:
    var = await climate_ir.new_climate_ir(config)

    cg.add(var.set_horizontal_default(config[CONF_HORIZONTAL_DEFAULT]))
    cg.add(var.set_vertical_default(config[CONF_VERTICAL_DEFAULT]))
