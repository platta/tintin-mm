"""Symbol Strategy parent class."""

from abc import ABCMeta, abstractmethod
from collections import defaultdict
from typing import Iterable

from ._types import RoomInfo


class SymbolStrategy(metaclass=ABCMeta):
    """Abstract class representing a symbol combining strategy."""

    @staticmethod
    @abstractmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Given multiple rooms, decide the symbol of the combined room."""


class MostCommonSymbolStrategy(SymbolStrategy):
    """Chooses the symbol that appears most often."""

    @staticmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Choose the color that appears most often."""
        counts = defaultdict(int)
        for room in iter(rooms):
            counts[room.symbol] += 1
        return max(counts, key=counts.get)


class LeastCommonSymbolStrategy(SymbolStrategy):
    """Chooses the symbol that appears least often."""

    @staticmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Choose the color that appears least often."""
        counts = defaultdict(int)
        for room in iter(rooms):
            counts[room.symbol] += 1
        return min(counts, key=counts.get)


class LowestValueSymbolStrategy(SymbolStrategy):
    """Choose the lowest numeric symbol value.

    Good for finding blank spots.
    """

    @staticmethod
    def combine(rooms: Iterable[RoomInfo]) -> int:
        """Choose the lowest numeric symbol value."""
        return min(*(i.symbol for i in iter(rooms)))
