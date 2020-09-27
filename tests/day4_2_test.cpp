#include "catch.hpp"
#include "day4_2_lib.hpp"


TEST_CASE("Valid passwords", "[day 4 - 2]") {
    REQUIRE(is_valid_password("112233"));
    REQUIRE(!is_valid_password("123444"));
    REQUIRE(is_valid_password("111122"));
}
