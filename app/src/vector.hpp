#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <functional>

struct Vec2
{
    int x;
    int y;
    Vec2() {
        x = 0;
        y = 0;
    }

    Vec2(int _x, int _y) {
        x = _x;
        y = _y;
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template <>
    struct hash<Vec2> {
        std::size_t operator()(const Vec2& v) const noexcept {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}


#endif