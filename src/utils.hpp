#pragma once

void init_array(unsigned* l, unsigned size, unsigned value) {
    for (unsigned i = 0; i < size; ++i) {
        l[i] = value;
    }
}

template <std::size_t n>
void print_array(const typename std::array<unsigned, n>& a) {
    for (unsigned k : a) {
        std::cout << k << " ";
    }
    std::cout << std::endl;
}