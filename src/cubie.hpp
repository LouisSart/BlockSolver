constexpr unsigned NC = 8, NE = 12;
using Cubie = unsigned;
using Orientation = unsigned;
enum Corner : Cubie { ULF, URF, URB, ULB, DLF, DRF, DRB, DLB };
enum Edge : Cubie { UF, UR, UB, UL, LF, RF, RB, LB, DF, DR, DB, DL };

std::array<std::string, NC> corner_str{"ULF", "URF", "URB", "ULB",
                                       "DLF", "DRF", "DRB", "DLB"};
std::array<std::string, NE> edge_str{"UF", "UR", "UB", "UL", "LF", "RF",
                                     "RB", "LB", "DF", "DR", "DB", "DL"};
std::ostream& operator<<(std::ostream& os, const Corner& c) {
    os << corner_str[c];
    return os;
}
std::ostream& operator<<(std::ostream& os, const Edge& e) {
    os << edge_str[e];
    return os;
}