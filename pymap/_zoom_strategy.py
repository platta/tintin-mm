"""Zoom Strategy abstract class."""

from abc import ABCMeta, abstractmethod
from typing import Iterable

from ._types import RoomInfo


class ZoomStrategy(metaclass=ABCMeta):
    """Abstract class representing a zoom strategy."""

    @classmethod
    @abstractmethod
    def combine(cls, rooms: Iterable[RoomInfo]) -> RoomInfo:
        """Given multiple rooms, return a single room to render."""


class DefaultZoomStrategy(ZoomStrategy):
    """Default zoom strategy."""

    # Colors:
    # 0 - black
    # 1 - red
    # 2 - green
    # 3 - yellow
    # 4 - blue
    # 5 - magenta
    # 6 - dark cyan?
    PRIORITY_ROOMS = [
        {
            'name': 'Entrances',
            'items': [
                ('O', 11)
            ]
        },
        {
            'name': 'Roads',
            'items': [
                ('#', 11),
                (':', 5),
                ('#', 184),
                (':', 11)
            ]
        },
        {
            'name': 'Town Walls',
            'items': [
                ('I', 5)
            ]
        },
        {
            'name': 'Town Insides',
            'items': [
                ('=', 13)
            ]
        },
        # {
        #     'name': 'CPK Stripes',
        #     'items': [
        #         ('v', 15)
        #     ]
        # }
    ]

    @classmethod
    def combine(cls, rooms: Iterable[RoomInfo]) -> RoomInfo:
        """Combine multiple rooms into one."""
        p_inventory = {}
        inventory = {}

        # Loop through rooms and categorize.
        for room in rooms:
            # Try to match to prioritized inventory.
            for priority_item in cls.PRIORITY_ROOMS:
                for match_item in priority_item['items']:
                    if room.symbol == match_item[0] and room.color == match_item[1]:
                        if priority_item['name'] not in p_inventory:
                            p_inventory[priority_item['name']] = {}
                        if match_item not in p_inventory[priority_item['name']]:
                            p_inventory[priority_item['name']][match_item] = 0
                        p_inventory[priority_item['name']][match_item] += 1

            # Also record in regular inventory.
            tup = (room.symbol, room.color)
            if room.symbol != ' ':
                if tup not in inventory:
                    inventory[tup] = 0
                inventory[tup] += 1

        # Check to see if we matched any of the prioritized items.
        for priority_item in cls.PRIORITY_ROOMS:
            if priority_item['name'] in p_inventory:
                # Got a match. Return the most frequently seen item from this
                # category.
                sorted_items_desc = sorted(
                    p_inventory[priority_item['name']].items(),
                    key=lambda item: item[1],
                    reverse=True
                )
                tup = sorted_items_desc[0][0]
                return RoomInfo(color=tup[1], symbol=tup[0])

        # Failing that, just return whatever nonzero thing you see the most of.
        if inventory:
            sorted_items_desc = sorted(
                inventory.items(),
                key=lambda item: item[1],
                reverse=True
            )
            tup = sorted_items_desc[0][0]
            return RoomInfo(color=tup[1], symbol=tup[0])

        return RoomInfo()
