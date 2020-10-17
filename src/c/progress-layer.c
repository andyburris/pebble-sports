#include "progress-layer.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


static int16_t start = 0;
static int16_t end = 100;

typedef struct {
    PropertyAnimationImplementation prop_anim_impl;
    PropertyAnimation *prop_anim;
    Animation *animation;
    int16_t progress_percent;
    int16_t corner_radius;
    GColor foreground_color;
    GColor background_color;
} ProgressLayerData;

static GRect progress_bar_get_bounds(unsigned int progress_percent, GRect layer_bounds) {
    int width = sin_lookup(progress_percent * 180 / 100) * 2 + 0.5;
    width = 100;
    return GRect(((layer_bounds.size.w + width) * progress_percent / 100) - width, layer_bounds.origin.y, width, layer_bounds.size.h);
}

static void progress_layer_update_proc(ProgressLayer* progress_layer, GContext* ctx) {
    ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
    GRect bounds = layer_get_bounds(progress_layer);

    graphics_context_set_fill_color(ctx, data->background_color);
    graphics_fill_rect(ctx, bounds, data->corner_radius, GCornersAll);

    GRect progress_bar_bounds = progress_bar_get_bounds(data->progress_percent, bounds);
    graphics_context_set_fill_color(ctx, data->foreground_color);
    graphics_fill_rect(ctx, progress_bar_bounds, data->corner_radius, GCornersAll);

    #ifdef PBL_PLATFORM_APLITE
        graphics_context_set_stroke_color(ctx, data->background_color);
        graphics_draw_rect(ctx, progress_bar_bounds);
    #endif
}

void update_anim_progress(void *subject, int16_t int16) {
    ProgressLayer *progress_layer = (ProgressLayer*) subject;
    progress_layer_set_progress(progress_layer, int16);
}

int16_t get_anim_progress(void *subject) {
    ProgressLayer *progress_layer = (ProgressLayer*) subject;
    return progress_layer_get_progress(progress_layer);
    //return 0;
}

ProgressLayer* progress_layer_create(GRect frame) {
    ProgressLayer *progress_layer = layer_create_with_data(frame, sizeof(ProgressLayerData));
    layer_set_update_proc(progress_layer, progress_layer_update_proc);
    layer_mark_dirty(progress_layer);

    ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
    data->progress_percent = 0;
    data->corner_radius = 1;
    data->foreground_color = GColorBlack;
    data->background_color = GColorWhite;


    data->prop_anim_impl = (PropertyAnimationImplementation) {
        .base = {
            // using the "stock" update callback:
            .update = (AnimationUpdateImplementation) property_animation_update_int16,
        },
        .accessors = {
            .getter = { .int16 = get_anim_progress, },
            .setter = { .int16 = update_anim_progress, },
        },
    };    

    int to_int = 100;

    data->prop_anim = property_animation_create(&data->prop_anim_impl, progress_layer, NULL, &to_int);

    property_animation_set_to_gpoint(data->prop_anim, &to_int);

    data->animation = property_animation_get_animation(data->prop_anim);
    animation_set_curve(data->animation, AnimationCurveEaseInOut);
    animation_set_duration(data->animation, 1000);
    animation_set_play_count(data->animation, 4);

    animation_schedule(data->animation);

    return progress_layer;
}

void progress_layer_destroy(ProgressLayer* progress_layer) {
    if (progress_layer) {
        ProgressLayerData *layer_data = (ProgressLayerData*) layer_get_data(progress_layer);
        animation_destroy(layer_data->animation);
        property_animation_destroy(layer_data->prop_anim);
        layer_destroy(progress_layer);
    }
}

static void progress_layer_set_progress(ProgressLayer* progress_layer, int16_t progress_percent) {
    ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
    data->progress_percent = MIN(100, progress_percent);
    layer_mark_dirty(progress_layer);
}

static int16_t progress_layer_get_progress(ProgressLayer* progress_layer) {
    ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
    return data->progress_percent;
}

void progress_layer_set_corner_radius(ProgressLayer* progress_layer, uint16_t corner_radius) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->corner_radius = corner_radius;
  layer_mark_dirty(progress_layer);
}

void progress_layer_set_foreground_color(ProgressLayer* progress_layer, GColor color) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->foreground_color = color;
  layer_mark_dirty(progress_layer);
}

void progress_layer_set_background_color(ProgressLayer* progress_layer, GColor color) {
  ProgressLayerData *data = (ProgressLayerData *)layer_get_data(progress_layer);
  data->background_color = color;
  layer_mark_dirty(progress_layer);
}