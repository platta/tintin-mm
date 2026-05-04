"""Interactive mapper utilities."""

from io import BufferedReader, BufferedRandom
import sys
from typing import Generator, Iterable, Dict, Optional, Union

from ._constants import HEADER_BYTES, ROOM_BYTES
from ._map_flags import MapFlags
from ._types import MapInfo, RoomInfo, BoundingBox
from ._zoom_info import ZoomInfo


class Map:
    """Represents a map file."""

    def __init__(self, filename: str, zoom: Optional[ZoomInfo] = None) -> None:
        """Initialize and instance of the class."""
        self._filename = filename
        self._info = self._read_info()
        self._zoom = zoom or ZoomInfo()

        self._handle: Optional[Union[BufferedReader, BufferedRandom]] = None

        self._cache: Dict[int, Dict[int, RoomInfo]] = {}

    @staticmethod
    def _get_bounding_box(x, y, width, height) -> BoundingBox:
        """Get a bounding box of specified size at specified coordinates."""
        top = y - (height - 1) // 2
        left = x - (width - 1) // 2
        return BoundingBox(
            top=top,
            left=left,
            bottom=top + height,
            right=left+width
        )

    @property
    def filename(self) -> str:
        """Get the filename property."""
        return self._filename

    @property
    def info(self) -> MapInfo:
        """Get the info property."""
        return self._info

    @property
    def zoom(self) -> ZoomInfo:
        """Get the zoom property."""
        return self._zoom

    def set_zoom(self, zoom: ZoomInfo) -> None:
        """Adjust the zoom of the map."""
        # Making this a method instead of a setter to keep with the whole
        # read-only feel.
        self._zoom = zoom

    def read(
        self,
        x: int,
        y: int,
        width: int = 1,
        height: int = 1
    ) -> Generator[Generator[RoomInfo, None, None], None, None]:
        """Read the map and return information for rendering it."""
        # Calculate bounding box for map to read.
        box = self._get_bounding_box(x, y, width, height)

        with open(self._filename, 'rb') as handle:
            self._handle = handle
            for current_y in range(box.top, box.bottom, self._zoom.level):
                yield self._read_row(box.left, current_y, width)

    def print(
        self,
        x: int,
        y: int,
        width: int = 1,
        height: int = 1,
    ) -> None:
        """Read the map and print the results to the console."""
        data = self.read(x, y, width, height)

        current_color = 0

        for row in data:
            for room in row:
                # if room.symbol == ' ':
                #     room = RoomInfo(color=1, symbol='X')
                if room.color != current_color:
                    current_color = room.color
                    if current_color == 0:
                        print('\x1b[0m', end='')
                    else:
                        print(f'\x1b[38;5;{current_color}m', end='')
                print(room.symbol, end='')
            current_color = 0
            print('\x1b[0m')

    def _read_info(self) -> MapInfo:
        """Read and store the header information of the map."""
        info = {}

        with open(self._filename, 'rb') as handle:
            # Read width and height.
            info['width'] = int.from_bytes(handle.read(2), sys.byteorder)
            info['height'] = int.from_bytes(handle.read(2), sys.byteorder)

            # Read and parse flags.
            flags = int.from_bytes(handle.read(1), sys.byteorder)
            info['hwrap'] = flags & MapFlags.HWRAP.value == MapFlags.HWRAP.value
            info['vwrap'] = flags & MapFlags.VWRAP.value == MapFlags.VWRAP.value

        return MapInfo(**info)

    def _read_row(
        self,
        x: int,
        y: int,
        length: int
    ) -> Generator[RoomInfo, None, None]:
        """Read a row of map data."""
        # Cache the room data for this entire row.
        first_box = self._get_bounding_box(
            x,
            y,
            self._zoom.level,
            self._zoom.level
        )
        last_box = self._get_bounding_box(
            x + length - 1,
            y,
            self._zoom.level,
            self._zoom.level
        )
        self._cache_map_block(BoundingBox(
            first_box.top,
            first_box.left,
            last_box.bottom,
            last_box.right
        ))

        for current_x in range(x, x + length, self._zoom.level):
            yield self._apply_zoom(self._read_rooms_for_zoom(current_x, y))

    def _cache_map_block(self, box: BoundingBox) -> None:
        """Ensure the cache contains the specified bounding box of data."""
        # Remove rows not within the current bounding box.
        for row in list(self._cache.keys()):
            if row < box.top or row >= box.bottom:
                self._cache.pop(row, None)

        for raw_y in range(box.top, box.bottom):
            y = self._wrap_y_coordinate(raw_y)

            # If coordinate did not wrap, skip it.
            if not y:
                continue

            # Ensure row is in cache.
            if y not in self._cache:
                self._cache[y] = {}
            else:
                # If row already exists, remove columns not within the current
                # bounding box.
                for col in list(self._cache[y].keys()):
                    if col < box.left or col >= box.right:
                        self._cache[y].pop(col, None)

            for raw_x in range(box.left, box.right):
                x = self._wrap_x_coordinate(raw_x)

                # If coordinate didn't wrap, or this room is already in the
                # cache, skip it.
                if not x or x in self._cache[y]:
                    continue

                if x in self._cache[y]:
                    continue

                self._cache[y][x] = self._read_single_room(x, y)

    def _read_rooms_for_zoom(
        self,
        x: int,
        y: int
    ) -> Generator[RoomInfo, None, None]:
        """Read a room and surrounding rooms so zoom can be applied."""
        box = self._get_bounding_box(x, y, self._zoom.level, self._zoom.level)

        for current_y in range(box.top, box.bottom):
            wrapped_y = self._wrap_y_coordinate(current_y)
            for current_x in range(box.left, box.right):
                wrapped_x = self._wrap_x_coordinate(current_x)

                if wrapped_x is None or wrapped_y is None:
                    # One or both coordinates was out of range in a way that
                    # isn't wrapped. Return an empty room.
                    yield RoomInfo.empty()
                else:
                    yield self._read_single_room(wrapped_x, wrapped_y)

    def _wrap_x_coordinate(self, x: int) -> Optional[int]:
        """Wrap x coordinates that are negative or larger than max."""
        if x < 0 or x >= self._info.width:
            if not self._info.hwrap:
                return None
            else:
                return (x % self._info.width) % self._info.width
        return x

    def _wrap_y_coordinate(self, y: int) -> Optional[int]:
        """Wrap y coordinates that are negative or larger than max."""
        if y < 0 or y >= self._info.height:
            if not self._info.vwrap:
                return None
            else:
                return (y % self._info.height) % self._info.height
        return y

    def _seek_room(self, x: int, y: int) -> None:
        """Make sure the file pointer is sitting on the room we want."""
        if self._handle is None:
            return None

        offset = HEADER_BYTES + (y * self._info.width + x) * ROOM_BYTES
        if self._handle.tell() != offset:
            # Only seek when necessary.
            self._handle.seek(offset)

    def write_single_room(self, x: int, y: int, room: RoomInfo) -> None:
        """Write a single room to the map file."""
        opened = False
        if not self._handle or self._handle.closed:
            self._handle = open(self._filename, 'r+b')
            opened = True

        self._seek_room(x, y)
        print(self._handle.write(room.symbol.encode('ascii')))
        print(self._handle.write(room.color.to_bytes(1, sys.byteorder)))

        if opened:
            self._handle.close()
            self._handle = None

    def _read_single_room(self, x: int, y: int) -> RoomInfo:
        """Read a single room from the map file."""
        # Check the cache first.
        if y in self._cache and x in self._cache[y]:
            return self._cache[y][x]

        if not self._handle or self._handle.closed:
            raise ValueError

        self._seek_room(x, y)

        return RoomInfo(
            symbol=self._handle.read(1).decode('ascii').rstrip('\x00') or ' ',
            color=int.from_bytes(self._handle.read(1), sys.byteorder)
        )

    def _apply_zoom(
        self,
        rooms: Iterable[RoomInfo]
    ) -> RoomInfo:
        """Given multiple rooms, combine them into one based on zoom rules."""
        if self._zoom.level == 1:
            # If no zoom info, or we haven't zoomed out at all, there should
            # only be one room. Return the first room of the collection.
            return next(iter(rooms))

        # Because we're going to pass room_list to multiple functions that will
        # iterate over it, we need to run through the generator and store the
        # results as a list.
        room_list = list(rooms)

        return RoomInfo(
            color=self._zoom.colors.combine(room_list),
            symbol=self._zoom.symbols.combine(room_list)
        )
