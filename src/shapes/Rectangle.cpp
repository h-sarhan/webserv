/**
 * @file Rectangle.cpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file implements the rectangle class
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Rectangle.hpp"
#include <algorithm>

Rectangle::Rectangle(void) : _height(0), _width(0), _origin()
{
}

Rectangle::Rectangle(size_t h, size_t w, const Point &origin)
    : _height(h), _width(w), _origin(origin)
{
}

Rectangle::Rectangle(const Rectangle &rect)
    : _height(rect.height()), _width(rect.width()), _origin(rect.origin())
{
}

Rectangle &Rectangle::operator=(const Rectangle &rect)
{
    if (this == &rect)
    {
        return (*this);
    }
    _height = rect.height();
    _width = rect.width();
    _origin = rect.origin();
    return (*this);
}

Rectangle::~Rectangle(void)
{
}

size_t Rectangle::height(void) const
{
    return _height;
}

size_t Rectangle::width(void) const
{
    return _width;
}

Point Rectangle::origin(void) const
{
    return _origin;
}

size_t Rectangle::area(void) const
{
    return _width * _height;
}

bool Rectangle::overlap(const Rectangle &other) const
{
    const Point orgA = origin();
    const Point orgB = other.origin();
    return std::max(orgA.x, orgB.x) <
               std::min(orgA.x + width(), orgB.x + other.width()) &&
           std::max(orgA.y, orgB.y) <
               std::min(orgA.y + height(), orgB.y + other.height());
}
