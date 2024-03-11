#pragma once
#include <iostream>
#include <vector>

constexpr int ipow(int k, uint n) {
    // integer power
    // computes ret = k^n
    // k can be negative hence using int
    uint ret{1};
    for (uint i = 0; i < n; i++) {
        ret *= k;
    }
    return ret;
};

constexpr uint factorial(int n) {
    // recursive impl of the factorial: n!
    if (n < 0) {
        std::cout << "factorial error: n= " << n << '\n';
        return -1;
    } else if (n == 0) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
};
constexpr uint binomial(uint n, uint k) {
    // binomial coefficient C_{n,k}
    if (n < k) {
        return 0;
    } else {
        return factorial(n) / (factorial(k) * factorial(n - k));
    }
};
extern uint perm_coord(uint*, uint);
extern uint layout_coord(uint*, uint);
extern uint co_coord(uint*, uint);
extern uint eo_coord(uint*, uint);
extern void perm_from_coord(uint, uint, uint*);
extern void layout_from_coord(uint, uint, uint, uint*);
extern void co_from_coord(uint, uint, uint*);
extern void eo_from_coord(uint, uint, uint*);
extern void corner_move_table_size(uint);
extern void edge_move_table_size(uint);

uint perm_coord(uint* p, uint n) {
    // p: the permutation array. Sorted position (coord = 0)
    // must have strictly increasing values
    // n: the size of the array
    uint count{0}, coord{0};
    std::vector<uint> icount;

    for (size_t i = 0; i < n; i++) {
        count = 0;
        for (size_t k = 0; k < i; k++) {
            if (p[k] > p[i]) {
                count++;
            }
        }
        icount.push_back(count);
    }

    for (size_t i = 0; i < icount.size(); i++) {
        coord += factorial(i) * icount[i];
    }
    return coord;
}

uint layout_coord(uint* l, uint n) {
    // l: an array containing the positions of the
    // pieces, ex {0,1,1,0,0,1,0,0,0} for a 3-corner
    // layout (3 1s among 8 possible positions)
    // n: number of possible positions
    // (usually 12 for edges, 8 for corners)
    // Solved position NEEDS to have all pieces on the left (c=0)
    // for the layout_from_coord function to work. Reorder the
    // ep/cp array if necessary
    uint c{0}, x{0};
    for (uint i = 0; i < n; i++) {
        if (l[i] == 1) {
            c += binomial(i, x + 1);
            x += 1;
        }
    }
    return c;
}

uint co_coord(uint* co, uint n) {
    // co: the orientation array of the corners
    // n: the number of corners to compute  the coordinate from.
    // For a complete cube, coord is computed from only the first
    // 7 corners because the orientation of the last is forced by the others
    uint coord{0};
    for (size_t i = 0; i < n; i++) {
        coord += co[i] * ipow(3, i);
    }
    return coord;
}

uint eo_coord(uint* eo, uint n) {
    // eo: the orientation array of the edges
    // n: the number of edges to compute  the coordinate from.
    // For a complete cube, coord is computed from only the first
    // 11 edges because the orientation of the last is forced by the others
    uint coord{0};
    for (size_t i = 0; i < n; i++) {
        coord += eo[i] * ipow(2, i);
    }
    return coord;
}

void perm_from_coord(uint coord, uint n, uint* perm) {
    // c: the coordinate of the permutation
    // n: the length of the permutation
    // out: the permutation, aka the array
    // containing the uintegers 0 to n-1
    // correctly permuted
    uint icount[n]{0}, k{2};
    uint c = coord;

    while (c > 0) {
        icount[k - 1] = c % k;
        c = c / k;
        k += 1;
    }

    uint l = n;
    std::vector<uint> range(n);
    for (uint i = 0; i < n; i++) {
        range[i] = i;
    }

    for (int i = n - 1; i >= 0; i--) {
        k = icount[i];
        l = range.size() - k - 1;
        perm[i] = range[l];
        range.erase(range.begin() + l);
    }
}

void layout_from_coord(uint c, uint n, uint k, uint* l) {
    // c: the layout coordinate
    // n: number of possible positions (12 for edges, 8 for corners)
    // k: number of pieces
    // Builds the layout state that corresponds to c

    int b, x = k, cc = c;
    for (uint i = 0; i < n; i++) {
        b = binomial(n - i - 1, x);
        if (cc - b >= 0) {
            l[n - 1 - i] = 1;
            x -= 1;
            cc -= b;
        } else {
            l[n - 1 - i] = 0;
        }
    }
}

void eo_from_coord(uint c, uint n, uint* eo) {
    // c: coordinate
    // n: number of edges of the block
    uint cc = c;
    for (uint i = 0; i < n; i++) {
        eo[i] = cc % 2;
        cc = cc / 2;
    }
}

void co_from_coord(uint c, uint n, uint* co) {
    // c: coordinate
    // n: number of corners in the block

    uint cc = c;
    for (uint i = 0; i < n; i++) {
        co[i] = cc % 3;
        cc = cc / 3;
    }
}

void corner_move_table_size(uint k) {
    std::cout << binomial(8, k) * factorial(k) << '\n';
}

void edge_move_table_size(uint k) {
    std::cout << binomial(12, k) * factorial(k) << '\n';
}

// ########################## TESTS ##################################

// void test_perm_coord()
// {
//   uint n=10;
//   uint perm[] {1,2,0,4,7,5,8,9,3,6};
//   uint c = perm_coord(perm, n);
//   std::cout << c << '\n';
//   std::cout << "-------------" << '\n';
//
//   uint* p;
//   p = perm_from_coord(c, n);
//   for (size_t i = 0; i < n; i++)
//   {
//     std::cout << *(p+i) << '\n';
//   }
// }
//
// void test_eo_coord()
// {
//   uint n {10};
//   uint eo[n] = {0,1,1,0,0,1,0,1,0,1};
//   uint c = eo_coord(eo, n);
//   std::cout << c << '\n';
//   uint* eeo = eo_from_coord(c, n);
//   for (size_t i = 0; i < n; i++)
//   {
//     std::cout << eeo[i] << ' ';
//   }
//   std::cout << '\n';
// }
//
// void test_co_coord()
// {
//   uint n {10};
//   uint co[n] = {0,1,2,0,2,1,0,2,0,1};
//   uint c = co_coord(co, n);
//   std::cout << c << '\n';
//   uint* cco = co_from_coord(c, n);
//   for (size_t i = 0; i < n; i++)
//   {
//     std::cout << cco[i] << ' ';
//   }
//   std::cout << '\n';
// }
//
// void test_layout_coord()
// {
//   uint n=8, k=3;
//   uint cp[n]{0,0,0,1,0,0,1,1};
//   uint c = layout_coord(cp, n);
//   std::cout << c << '\n';
//   uint* l = layout_from_coord(c, n, k);
//   for (uint i=0;i<n;i++)
//   {
//     std::cout << l[i] << ' ';
//   }
//   std::cout << '\n';
// }
