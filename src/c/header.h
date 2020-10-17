#include <pebble.h>

#ifndef HEADER_H_
#define HEADER_H_

typedef struct {
    const char *title;
    GBitmap *icon;
    const char *info;
} HeaderData;

#endif

Layer *create_header_layer(GRect window_bounds, HeaderData data);