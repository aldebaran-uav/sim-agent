#pragma once

#ifndef NAVIGATION_AREA_H
#define NAVIGATION_AREA_H

struct navigation_area {
    double north;
    double east;
    double south;
    double west;
    double max_altitude;
    double min_altitude;
};

#endif // NAVIGATION_AREA_H