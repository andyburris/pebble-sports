#include "pebble.h"
#include "score-layer.h"

static void score_update_proc(Layer *layer, GContext *ctx) {
    Game *game = *(Game **)layer_get_data(layer);
    bool has_long_score = strlen(game->team1.score) > 2 || strlen(game->team2.score) > 2;

    GRect layer_bounds = layer_get_bounds(layer);

    GFont font_score = fonts_get_system_font(has_long_score ? FONT_KEY_LECO_36_BOLD_NUMBERS : FONT_KEY_LECO_42_NUMBERS);
    GFont font_team = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GFont font_record = fonts_get_system_font(FONT_KEY_GOTHIC_14);

    graphics_context_set_fill_color(ctx, GColorBlack);
    GRect separator_bounds = GRect(layer_bounds.size.w / 2 - 1, 12, 2, 48);

    graphics_context_set_text_color(ctx, GColorBlack);

    //record 1 is centered between the left edge and the divider
    GRect record_1_bounds = GRect(0, 0, separator_bounds.origin.x, 14);
    graphics_draw_text(ctx, game->team1.record, font_record, record_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    //record 2 is centered between the divider and the right edge
    GRect record_2_bounds = GRect(separator_bounds.origin.x + separator_bounds.size.w, 0, separator_bounds.origin.x, 14);
    graphics_draw_text(ctx, game->team2.record, font_record, record_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    //score 1 is centered between the left edge and the divider
    GRect score_1_bounds = GRect(0, has_long_score ? 11 : 7, separator_bounds.origin.x, 42);
    graphics_draw_text(ctx, game->team1.score, font_score, score_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    //score 2 is centered between the divider and the right edge
    GRect score_2_bounds = GRect(separator_bounds.origin.x + separator_bounds.size.w, has_long_score ? 11 : 7, separator_bounds.origin.x, 42);
    graphics_draw_text(ctx, game->team2.score, font_score, score_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    //if a possession indicator is showing, 10px more of content needs to be centered (6px padding + 4px indicator)
    //so offset text by half of that
    int possession_offset = 5;

    bool team_1_possession = (game->possession) == Team1;
    GSize team_1_size = graphics_text_layout_get_content_size(game->team1.name, font_team, score_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
    GRect team_1_bounds = GRect(score_1_bounds.size.w/2 - team_1_size.w/2 - (team_1_possession ? possession_offset : 0), 50, team_1_size.w, 26);
    graphics_draw_text(ctx, game->team1.name, font_team, team_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    if (team_1_possession) {
        graphics_fill_circle(ctx, GPoint(team_1_bounds.origin.x + team_1_bounds.size.w + 6 - 2, team_1_bounds.origin.y + team_1_bounds.size.h /2), 2);
    }

    bool team_2_possession = (game->possession) == Team2;
    GSize team_2_size = graphics_text_layout_get_content_size(game->team2.name, font_team, score_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
    GRect team_2_bounds = GRect(score_2_bounds.origin.x + score_2_bounds.size.w/2 - team_2_size.w/2 - (team_2_possession ? possession_offset : 0), 50, team_2_size.w, 26);
    graphics_draw_text(ctx, game->team2.name, font_team, team_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    if (team_2_possession) {
        graphics_fill_circle(ctx, GPoint(team_2_bounds.origin.x + team_2_bounds.size.w + 6 - 2, team_2_bounds.origin.y + team_2_bounds.size.h /2), 2);
    }
    
}

Layer *score_layer_create(GRect bounds, Game *game) {
    Layer *score; 
    bounds.size.h = 74;

    #ifdef PBL_ROUND
        bounds.size.w -= 32;
        bounds.origin.x += 16;
    #endif

    score = layer_create_with_data(bounds, sizeof(Game*));

    Game **layer_data = (Game **)layer_get_data(score);
    *layer_data = game;

    layer_set_update_proc(score, score_update_proc);

    return score;
}