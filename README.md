# Compressed_pair

## Purpose

The compressed pair class is very similar to std::pair, but if either of the
template arguments are empty classes, then the "empty base-class
optimization" is applied to compress the size of the pair.

```c++

#include <iostream>
#include <format>
#include "compressed_pair.hxx"


struct point
{
    float x, y;
    constexpr point(float a, float b) noexcept : x(a), y(b) {}
};

int main()
{

    // direct initialization of compressed_pair with multi-parameter constructors
    [[maybe_unused]] auto points = compressed_pair<point, point>( 
                                            std::piecewise_construct,
                                            std::forward_as_tuple(2.35f, 3.5f),
                                            std::forward_as_tuple(6.35f, 7.89f));


    std::cout 
        << std::format("p1 [ x: {.2f}, y: {.2f} ]", points.first().x, point.first().y) <<
    std::endl;

    std::cout 
        << std::format("p1 [ x: {.2f}, y: {.2f} ]", points.second().x, point.second().y) <<
    std::endl;

    std::puts("\n------------------------------\n");

    // structured binding support
    auto [p1, p2] = points;


    std::cout 
        << std::format("p1 [ x: {.2f}, y: {.2f} ]", p1.x, p1.y) <<
    std::endl;

    std::cout 
        << std::format("p1 [ x: {.2f}, y: {.2f} ]", p2.x, p2.y) <<
    std::endl;
}

```
