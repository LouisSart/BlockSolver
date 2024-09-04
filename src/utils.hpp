#pragma once

void init_array(unsigned* l, unsigned size, unsigned value) {
    for (unsigned i = 0; i < size; ++i) {
        l[i] = value;
    }
}

template <typename T, std::size_t n, typename cast_t = T>
void print_array(const typename std::array<T, n>& a) {
    std::cout << "{";
    for (unsigned k : a) {
        std::cout << static_cast<cast_t>(k) << " ";
    }
    std::cout << "\b}" << std::endl;
}