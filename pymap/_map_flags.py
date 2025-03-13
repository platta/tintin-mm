"""Map Flags enum."""

from enum import Enum


class MapFlags(Enum):
    # Flag for if the map should wrap at the left and right.
    HWRAP = 0b00000001

    # Flag for if the map should wrap at the top and bottom.
    VWRAP = 0b00000010
