import sys


def has_no_downward_sequence(number: str) -> bool:
    assert len(number) > 1
    return all(int(number[i]) <= int(number[i + 1]) for i in range(len(number) - 1))


def has_two_consecutive_digits(number: str) -> bool:
    assert len(number) > 1
    return any(number[i] == number[i + 1] for i in range(len(number) - 1))


def meets_criteria(number: str) -> bool:
    return has_no_downward_sequence(number) and has_two_consecutive_digits(number)


def main():
    begin = int(sys.argv[1])
    end = int(sys.argv[2])
    assert begin < end
    answer = sum(1 if meets_criteria(str(i)) else 0 for i in range(begin, end + 1))
    print(f"There are {answer} potentials passwords")


main()
