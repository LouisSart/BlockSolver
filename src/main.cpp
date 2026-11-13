#include "../extern/EpiCube/lib/option.hpp"
#include "../extern/EpiCube/lib/utils.hpp"
#include "../src/block.hpp"
#include <cstring>


int main (int argc, const char* argv[]){

    auto scramble = make_sequence(argv[argc - 1]);
    unsigned max_depth = get_option("-M", argc, argv, 30);

    if (strcmp(argv[1], "l3c") == 0) {

        auto root = make_root(Cube333::CubieCube(scramble));
        solvers::three_step_l3c_solver.solve({root}).show<Move>();

    } else {
        print("Invalid or no argument");
    };
    return 0;
}