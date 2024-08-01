#ifndef POPCOUNT_H
#define POPCOUNT_H

#include <bitset>

#ifndef __builtin_popcount
inline unsigned int popcount(unsigned int x) {
    return std::bitset<32>(x).count();
}
#else
inline unsigned int popcount(unsigned int x) {
    return __builtin_popcount(x);
}
#endif

#endif // POPCOUNT_H
