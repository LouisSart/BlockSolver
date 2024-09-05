#pragma once
constexpr unsigned NC = 8, NE = 12;
using Cubie = unsigned;
using Orientation = unsigned;
enum Corner : Cubie { ULF, URF, URB, ULB, DLF, DRF, DRB, DLB, NONE_C };
enum Edge : Cubie { UF, UR, UB, UL, LF, RF, RB, LB, DF, DR, DB, DL, NONE_E };

std::array<std::string, NC + 1> corner_str{"ULF", "URF", "URB", "ULB", "DLF",
                                           "DRF", "DRB", "DLB", "-"};
std::array<std::string, NE + 1> edge_str{"UF", "UR", "UB", "UL", "LF",
                                         "RF", "RB", "LB", "DF", "DR",
                                         "DB", "DL", "-"};
std::ostream& operator<<(std::ostream& os, const Corner& c) {
    os << corner_str[c];
    return os;
}
std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << edge_str[e];
    return os;
}