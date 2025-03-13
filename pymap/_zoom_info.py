
from dataclasses import dataclass
from typing import Type

from ._color_strategy import ColorStrategy, MostCommonColorStrategy
from ._symbol_strategy import SymbolStrategy, MostCommonSymbolStrategy


@dataclass(frozen=True)
class ZoomInfo:
    level: int = 1
    colors: Type[ColorStrategy] = MostCommonColorStrategy
    symbols: Type[SymbolStrategy] = MostCommonSymbolStrategy
