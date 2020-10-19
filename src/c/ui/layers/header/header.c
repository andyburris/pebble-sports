#include <pebble.h>
#include "header.h"


static void rect_header_update_proc(Layer *layer, GContext *ctx) {

    HeaderData *data = (HeaderData *)layer_get_data(layer);

    graphics_context_set_fill_color(ctx, GColorElectricUltramarine);
    GRect layer_bounds = layer_get_bounds(layer);
    graphics_fill_rect(ctx, layer_bounds, 0, GCornerNone);

    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GSize title_size = graphics_text_layout_get_content_size(data->title, font, layer_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
    int icon_padding = 8;
    if (data->icon != NULL) {
        icon_padding = 32;
    }
    GRect title_bounds = GRect(icon_padding, 0, title_size.w, 18);
    graphics_draw_text(ctx, data->title, font, title_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    if (data->icon != NULL) {
        GRect icon_bounds = GRect(8, 4, 16, 16);
        //gdraw_command_image_set_bounds_size(data->icon, icon_bounds.size);
        //gdraw_command_image_draw(ctx, data->icon, icon_bounds.origin);
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, data->icon, icon_bounds);
    }   
}

static void circle_header_update_proc(Layer *layer, GContext *ctx) {

    HeaderData *data = (HeaderData *)layer_get_data(layer);

    graphics_context_set_fill_color(ctx, GColorElectricUltramarine);
    GRect layer_bounds = layer_get_bounds(layer);
    int radius = layer_bounds.size.w;
    int horz_center = layer_bounds.origin.x + (layer_bounds.size.w/2);
    int vert_center = (layer_bounds.size.h + layer_bounds.origin.y) - radius;
    graphics_fill_circle(ctx, GPoint(horz_center, vert_center), radius);

    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GSize title_size = graphics_text_layout_get_content_size(data->title, font, layer_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
    int icon_padding = 0;
    if (data->icon != NULL) {
        icon_padding = 8;
    }
    GRect title_bounds = GRect(layer_bounds.size.w / 2 - title_size.w /2 + icon_padding, 0, title_size.w, 18);
    graphics_draw_text(ctx, data->title, font, title_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    if (data->icon != NULL) {
        GRect icon_bounds = GRect(layer_bounds.size.w / 2 - title_size.w /2 - 12, 4, 16, 16);
        //gdraw_command_image_set_bounds_size(data->icon, icon_bounds.size);
        //gdraw_command_image_draw(ctx, data->icon, icon_bounds.origin);
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, data->icon, icon_bounds);
    }   
}

Layer *create_header_layer(GRect window_bounds, HeaderData data) {

    Layer *header; 

    printf("header y starts at %d", window_bounds.origin.y);
    GRect header_bounds = window_bounds; 

    header_bounds.size.h = 26;
    header = layer_create_with_data(header_bounds, sizeof(HeaderData));
    HeaderData *layer_data = (HeaderData *)layer_get_data(header);
    layer_data->icon = data.icon;
    layer_data->title = data.title;

    layer_set_update_proc(header, PBL_IF_RECT_ELSE(rect_header_update_proc, circle_header_update_proc));

    return header;
}
