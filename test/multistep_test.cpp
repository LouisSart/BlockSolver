#include "multistep.hpp"

#include "222.hpp"
#include "223.hpp"
#include "F2L-1.hpp"
#include "algorithm.hpp"

int main() {
    auto scramble = Algorithm(
        "R' U' F L2 D L' B R D' B' U' D2 L' U2 L B2 R2 B2 U2 F' L B2 "
        "R' U' "
        "F");
    scramble.show();

    CubieCube scramble_cc(scramble);
    auto root = std::make_shared<StepNode>(scramble_cc);
    auto solutions = root->expand(block_solver_222::cc_initialize,
                                  block_solver_222::solve, 5);
    auto node = solutions.back();
    assert(node->parent == root);
    assert(node->seq.size() > 0);
    assert(node->depth == node->seq.size());

    auto children = node->expand(block_solver_223::cc_initialize,
                                 block_solver_223::solve, 5);
    auto child = children.back();
    assert(child->parent == node);
    assert(child->seq.size() > 0);
    assert(child->depth == child->seq.size() + node->seq.size());

    auto grandchildren = child->expand(block_solver_F2Lm1::cc_initialize,
                                       block_solver_F2Lm1::solve, 6);
    auto grandchild = grandchildren.back();
    grandchild->get_skeleton({"2x2x2", "2x2x3", "F2L-1"}).show();
    assert(grandchild->parent == child);
    assert(grandchild->seq.size() > 0);
    assert(grandchild->depth == child->depth + grandchild->seq.size());
    return 0;
}