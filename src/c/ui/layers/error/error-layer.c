#include "pebble.h"
#include "../../../data/model/models.h"
#include "error-layer.h"

typedef struct {
    GBitmap *icon;
    char *title;
    char *summary;
} ErrorLayerData;

static void error_layer_update_proc(Layer *layer, GContext *ctx) {
    ErrorLayerData *data = (ErrorLayerData *)layer_get_data(layer);
    GRect layer_bounds = layer_get_bounds(layer);

    if(data->icon != NULL) {
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        GRect icon_bounds = GRect(layer_bounds.size.w / 2 - 12, 0, 25, 25);
        graphics_draw_bitmap_in_rect(ctx, data->icon, icon_bounds); 
    }

    graphics_context_set_text_color(ctx, GColorBlack);
    GRect title_bounds = GRect(0, 25, layer_bounds.size.w, 24);
    graphics_draw_text(ctx, data->title, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), title_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    GRect summary_bounds = GRect(0, 25 + 18, layer_bounds.size.w, 18);
    graphics_draw_text(ctx, data->summary, fonts_get_system_font(FONT_KEY_GOTHIC_14), summary_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);


}

ErrorLayer* error_layer_create(GRect bounds) {
    ErrorLayer *error_layer = layer_create_with_data(bounds, sizeof(ErrorLayerData));

    ErrorLayerData *data = (ErrorLayerData *)layer_get_data(error_layer);
    data->icon = NULL;
    data->title = "";
    data->summary = "";

    layer_set_update_proc(error_layer, error_layer_update_proc);
    return error_layer;
}

void error_layer_set_error(ErrorLayer *error_layer, AppError error) {
    ErrorLayerData *data = (ErrorLayerData *) layer_get_data(error_layer);
    switch (error)
    {
        case NoGames:
            data->icon = gbitmap_create_with_resource(RESOURCE_ID_GENERIC_LARGE);
            data->title = "No Games";
            data->summary = "Press select to refresh";
            break;
        case NetworkError:
            data->icon = gbitmap_create_with_resource(RESOURCE_ID_ERROR_LARGE);
            data->title = "Network Error";
            data->summary = "Press select to refresh";
            break;
        case ConnectionError:
            data->icon = gbitmap_create_with_resource(RESOURCE_ID_ERROR_LARGE);
            data->title = "Connection Error";
            data->summary = "Press select to refresh";
            break;
        
        default:
            break;
    }
    layer_mark_dirty(error_layer);
}

void error_layer_destroy(ErrorLayer *error_layer){
    ErrorLayerData *data = (ErrorLayerData *)layer_get_data(error_layer);
    gbitmap_destroy(data->icon);
    layer_destroy(error_layer);
}