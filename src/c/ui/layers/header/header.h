#include <pebble.h>

#ifndef HEADER_H_
#define HEADER_H_

typedef Layer HeaderLayer;

typedef struct {
    const char *title;
    GBitmap *icon;
    const char *info;
    bool under_status_bar;
} HeaderData;

#endif

HeaderLayer *create_header_layer(GRect window_bounds, HeaderData data);
void header_layer_set_under_status_bar(HeaderLayer *header_layer, bool under_status_bar);