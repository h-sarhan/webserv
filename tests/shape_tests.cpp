/**
 * @file shape_tests.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file contains unit tests for the shape classes
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Rectangle.hpp"
#include "doctest.h"

TEST_CASE("Testing the rectangle class")
{
    Rectangle a;
    Point pt;

    CHECK(a.area() == 0);
    CHECK(a.height() == 0);
    CHECK(a.width() == 0);
    SUBCASE("Checking getters")
    {
        pt.x = -1.1;
        pt.y = 23.4;
        Rectangle b(12, 23, pt);

        CHECK(b.height() == 12);
        CHECK(b.width() == 23);
        Point origin = b.origin();
        CHECK(origin.x == -1.1);
        CHECK(origin.y == 23.4);
    }

    SUBCASE("Checking area")
    {
        Rectangle c(10, 23, pt);
        CHECK(c.area() == 230);
    }

    SUBCASE("Checking area")
    {
        Rectangle c(10, 23, pt);
        CHECK(c.area() == 230);
    }

    SUBCASE("Checking if two rectangles overlap correctly")
    {
        const Point origin1 = {99.0, 0};
        const Rectangle rect1(100, 100, origin1);
        const Point origin2 = {0, -1};
        const Rectangle rect2(100, 100, origin2);
        CHECK(rect1.overlap(rect2));
    }
}
