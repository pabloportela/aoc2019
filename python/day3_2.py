#!/usr/bin/env python3

import sys
from typing import List, Tuple, Iterator, Set, Dict


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


def get_lines(filename: str) -> List[str]:
    with open(filename) as f:
        lines = f.readlines()

    return lines


def get_intersection_steps(
    steps: List[Tuple[int, int]], xs: Set[Tuple[int, int]]
) -> Dict[Tuple[int, int], int]:
    return {s: i for i, s in enumerate(steps, start=1) if s in xs}


def main():
    instructions = get_lines(sys.argv[1])

    steps1 = list(generate_positions(instructions[0]))
    steps2 = list(generate_positions(instructions[1]))
    xs = set(steps1).intersection(set(steps2))

    if not xs:
        raise Exception("Paths don't intersect")

    q_steps_x_1 = get_intersection_steps(steps1, xs)
    q_steps_x_2 = get_intersection_steps(steps2, xs)

    answer = min(q_steps_x_1[s] + q_steps_x_2[s] for s in xs)

    print(f"Least steps intersection distance: {answer}")


if __name__ == "__main__":
    main()
