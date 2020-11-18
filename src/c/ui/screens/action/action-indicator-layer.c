#include "pebble.h"

static void action_indicator_update_proc(Layer *layer, GContext *ctx) {
    GRect layer_bounds = layer_get_bounds(layer);

    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, GPoint(layer_bounds.origin.x + 12, layer_bounds.origin.y + 12), 12);
}

Layer *action_indicator_layer_create() {
    GRect bounds = PBL_IF_RECT_ELSE(GRect(138, 74, 12, 24), GRect(170, 80, 12, 24));
    Layer *indicator = layer_create(bounds);
    layer_set_update_proc(indicator, action_indicator_update_proc);
    return indicator;
}