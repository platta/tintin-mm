"""Color Strategy parent class."""

from abc import ABCMeta, abstractmethod
from collections import defaultdict
from typing import Iterable

from ._types import RoomInfo


class ColorStrategy(metaclass=ABCMeta):
    """Abstract class representing a color combining strategy."""

    @staticmethod
    @abstractmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Given multiple rooms, decide the color of the combined room."""


class MostCommonColorStrategy(ColorStrategy):
    """Chooses the color that appears most often."""

    @staticmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Choose the color that appears most often."""
        counts = defaultdict(int)
        for room in iter(rooms):
            counts[room.color] += 1
        return max(counts, key=counts.get)


class LeastCommonColorStrategy(ColorStrategy):
    """Choose the color that appears least often."""

    @staticmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Choose the color that appears least often."""
        counts = defaultdict(int)
        for room in iter(rooms):
            counts[room.color] += 1
        return min(counts, key=counts.get)


class LowestValueColorStrategy(ColorStrategy):
    """Choose the lowest numeric color value.

    Good for finding blank spots.
    """

    @staticmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Choose the lowest numeric color value."""
        return min(*(i.color for i in iter(rooms)))
