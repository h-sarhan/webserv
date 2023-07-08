/**
 * @file Rectangle.hpp
 * @author Hassan Sarhan (hassanAsarhan@outlook.com)
 * @brief This file defines a class to represent a rectangle
 * @date 2023-05-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Vec2.hpp"
#include "common.hpp"

/**
 * @brief Represents a rectangle
 */
class Rectangle
{
  private:
    /* Rectangle properties */
    size_t _height;
    size_t _width;
    Point _origin;

  public:
    /**
     * @brief Default constructor a new Rectangle object
     */
    Rectangle(void);

    /**
     * @brief Construct a new Rectangle object
     *
     * @param h Height of a rectangle
     * @param w Width of a rectangle
     * @param origin Origin point of the rectangle
     */
    Rectangle(size_t h, size_t w, const Point &origin);

    /**
     * @brief Copy constructor for a new Rectangle object
     *
     * @param rect Rectangle to copy
     */
    Rectangle(const Rectangle &rect);

    /**
     * @brief Assignment overload for Rectangle object
     *
     * @param rect Rectangle to copy
     * @return Rectangle& A reference to itself
     */
    Rectangle &operator=(const Rectangle &rect);

    /**
     * @brief Destroy the Rectangle object
     */
    ~Rectangle(void);

    /**
     * @brief Return the height of a rectangle
     *
     * @return size_t Rectangle height
     */
    size_t height(void) const;

    /**
     * @brief Return the width of a rectangle
     *
     * @return size_t Rectangle width
     */
    size_t width(void) const;

    /**
     * @brief Return the area of a rectangle
     *
     * @return size_t Rectangle area
     */
    size_t area(void) const;

    /**
     * @brief Returns the origin point of the rectangle
     *
     * @return Point Rectangle origin
     */
    Point origin(void) const;

    /**
     * @brief Checks if a rectangle intersects with another rectangle
     *
     * @param other Other rectangle
     * @return true Intersection
     * @return false No intersection
     */
    bool overlap(const Rectangle &other) const;
};
