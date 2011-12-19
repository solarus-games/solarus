/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Common.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Provides functions to make mathematic computations.
 */
class Geometry {

  public:

    // Mathematic constants:
    // we don't use the ones from cmath
    // because they are not ANSI
    static const double PI;
    static const double TWO_PI;
    static const double PI_OVER_2;
    static const double THREE_PI_OVER_2;
    static const double SQRT_2;

    static const int radians_to_degrees(double radians);
    static const double degrees_to_radians(double degrees);

    static double get_distance(int x1, int y1, int x2, int y2);
    static double get_distance(const Rectangle& xy1, const Rectangle& xy2);
    static double get_angle(int x1, int y1, int x2, int y2);
    static const Rectangle get_xy(double angle, int distance);
    static const Rectangle get_xy(const Rectangle &xy1, double angle, int distance);
};

