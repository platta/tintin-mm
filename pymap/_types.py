"""Definitions for TypedDict types."""

from dataclasses import dataclass


@dataclass(frozen=True)
class MapInfo:
    width: int = 0
    height: int = 0
    hwrap: bool = False
    vwrap: bool = False


@dataclass(frozen=True)
class RoomInfo:
    color: int = 0
    symbol: str = ' '

    @classmethod
    def empty(cls) -> 'RoomInfo':
        """Get the default empty room object."""
        if not hasattr(cls, '__empty'):
            cls.__empty = RoomInfo()
        return cls.__empty


@dataclass(frozen=True)
class BoundingBox:
    top: int = 0
    left: int = 0
    bottom: int = 0
    right: int = 0
