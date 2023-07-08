/**
 * @file Vec2.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines a vector class
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * @brief A vector with two members, x and y
 *
 * @tparam T The data type of the members
 */
template <typename T> struct Vec2
{
    T x;
    T y;
};

// Typedefs for convenience
typedef Vec2<float> Pointf;
typedef Vec2<double> Point;
