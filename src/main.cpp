/**
 * @file main.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief The entrypoint to our program
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "../include/shapes/Rectangle.hpp"
#include <iostream>

/**
 * @brief Entrypoint to our program
 *
 * @return int Exit code
 */
int main(void)
{
    const Point a = {99.0, 0};
    const Rectangle rect1(100, 100, a);
    const Point b = {0, -1};
    const Rectangle rect2(100, 100, b);
    if (rect1.overlap(rect2))
    {
        std::cout << "The two rectangles overlap" << std::endl;
    }
    else
    {
        std::cout << "The two rectangles do not overlap" << std::endl;
    }
}
