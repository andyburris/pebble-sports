#include "pebble.h"
#include "schedule-layer.h"

static void schedule_update_proc(Layer *layer, GContext *ctx) {
    Game *game = *(Game **)layer_get_data(layer);

    GRect layer_bounds = layer_get_bounds(layer);

    GFont font_team = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
    GFont font_record = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

    graphics_context_set_fill_color(ctx, GColorBlack);
    GRect separator_bounds = GRect(layer_bounds.size.w / 2 - 1, 4, 2, 48);

    graphics_fill_rect(ctx, separator_bounds, 0, GCornerNone);

    graphics_context_set_text_color(ctx, GColorBlack);


    GRect team_1_bounds = GRect(0, 0, separator_bounds.origin.x, 32);
    graphics_draw_text(ctx, game->team1.name, font_team, team_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    GRect team_2_bounds = GRect(separator_bounds.origin.x + separator_bounds.size.w, 0, separator_bounds.origin.x, 32);
    graphics_draw_text(ctx, game->team2.name, font_team, team_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    //record 1 is centered between the left edge and the divider
    GRect record_1_bounds = GRect(0, 28, separator_bounds.origin.x, 18);
    graphics_draw_text(ctx, game->team1.record, font_record, record_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    //record 2 is centered between the divider and the right edge
    GRect record_2_bounds = GRect(separator_bounds.origin.x + separator_bounds.size.w, 28, separator_bounds.origin.x, 18);
    graphics_draw_text(ctx, game->team2.record, font_record, record_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    
}

Layer *schedule_layer_create(GRect bounds, Game *game) {
    Layer *schedule; 
    bounds.size.h = 56;

    #ifdef PBL_ROUND
        bounds.size.w -= 32;
        bounds.origin.x += 16;
    #endif

    schedule = layer_create_with_data(bounds, sizeof(Game*));

    Game **layer_data = (Game **)layer_get_data(schedule);
    *layer_data = game;

    layer_set_update_proc(schedule, schedule_update_proc);

    return schedule;
}