#include "../extern/EpiCube/lib/option.hpp"
#include "../src/block.hpp"
#include <cstring>

using namespace Cube333;

int main (int argc, const char* argv[]){
    auto scramble = make_sequence(argv[argc - 1]);

    // unsigned slackness = get_option("-s", argc, argv, 0);
    // unsigned max_depth = get_option("-M", argc, argv, 15);
    // unsigned breadth = get_option("-b", argc, argv, 5000);
    // bool linear = find_option("-L", argc, argv);

    if (strcmp(argv[1], "fourstep") == 0) {

        auto root = make_root(CubieCube(scramble));
        auto multistep = make_multistep_solver(b222_solver, b223_solver, xxcross_solver, l3c_solver);
        multistep.solve({root}).show<Move>();

    } else if (strcmp(argv[1], "threestep") == 0) {

        auto root = make_root(CubieCube(scramble));
        auto multistep = make_multistep_solver(b222_solver, xxcross_solver, l3c_solver);
        multistep.solve({root}).show<Move>();

    };
    return 0;
}