#!/usr/bin/env python3

from functools import total_ordering
import sys
from typing import List, Tuple, Optional, Iterator


def generate_positions(path: str) -> Iterator[Tuple[int, int]]:
    def get_deltas(c: str) -> Tuple[int, int]:
        if c == "R":
            return 1, 0
        if c == "U":
            return 0, 1
        if c == "L":
            return -1, 0
        if c == "D":
            return 0, -1

        raise ValueError(f"Unknown direction {c}")

    current_x, current_y = 0, 0

    for d in path.split(","):
        delta_x, delta_y = get_deltas(d[0])
        for _ in range(int(d[1:])):
            current_x += delta_x
            current_y += delta_y
            yield current_x, current_y


def distance(position: Tuple[int, int]) -> int:
    return abs(position[0]) + abs(position[1])


def closest_distance(positions: List[Tuple[int, int]]) -> Optional[int]:
    closest = None
    for p in positions:
        if closest is None:
            closest = distance(p)
        else:
            closest = min(distance(p), closest)

    return closest


def get_lines(filename: str) -> List[str]:
    with open(filename) as f:
        lines = f.readlines()

    return lines


def main():
    instructions = get_lines(sys.argv[1])

    unique_positions1 = set(list(generate_positions(instructions[0])))
    unique_positions2 = set(list(generate_positions(instructions[1])))

    intersections = unique_positions1.intersection(unique_positions2)
    answer = closest_distance(intersections)

    if not answer:
        raise Exception("Paths don't intersect")

    print(f"Closest intersection distance: {answer}")


if __name__ == "__main__":
    main()
