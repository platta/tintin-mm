
from dataclasses import dataclass
from typing import Type

from ._zoom_strategy import ZoomStrategy, DefaultZoomStrategy


@dataclass(frozen=True)
class ZoomInfo:
    level: int = 1
    strategy: Type[ZoomStrategy] = DefaultZoomStrategy
