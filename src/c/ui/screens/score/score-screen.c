#include "pebble.h"
#include "score-screen.h"
#include "../../layers/header/header.h"
#include "../action/action-menu.h"
#include "../../../data/model/models.h"
#include "../../../data/comms/comms.h"
#include "../../../utils/utils.h"


static Window *scoreWindow;
static StatusBarLayer *s_status_bar;
static Layer *s_header;
static TextLayer *s_time;
static Layer *s_score;
static GBitmap *s_icon_image;
static Game s_game;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    game_action_menu_open(s_game);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "team_1_label = %s", ((ActionMenuLabels *)action_menu_get_context(action_menu))->team_1_label);
}

static void click_config_provider(void *context) {
    // Subcribe to button click events here

    ButtonId id = BUTTON_ID_SELECT;  // The Select button
    window_single_click_subscribe(id, select_click_handler);
}

static void score_update_proc(Layer *layer, GContext *ctx) {
    ScoreData *data = (ScoreData *)layer_get_data(layer);

    GRect layer_bounds = layer_get_bounds(layer);
    GFont font_score = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
    if (strlen(data->score1) > 2 || strlen(data->score2) > 2) { //scores in the hundreds won't fit with 42pt fonts
        font_score = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
    }
    
    GFont font_team = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);


    graphics_context_set_fill_color(ctx, GColorBlack);
    GRect dash_bounds = GRect(layer_bounds.size.w / 2 - 6, 24, 12, 6);

    graphics_context_set_text_color(ctx, GColorBlack);
    GRect score_1_bounds = GRect(0, 0, dash_bounds.origin.x, 42);
    graphics_draw_text(ctx, data->score1, font_score, score_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    GRect score_2_bounds = GRect(dash_bounds.origin.x + dash_bounds.size.w, 0, dash_bounds.origin.x, 42);
    graphics_draw_text(ctx, data->score2, font_score, score_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

    //if a possession indicator is showing, 14px more of content needs to be centered (8px padding + 6px indicator)
    //so offset text by half of that
    int possession_offset = 7;

    bool team_1_possession = (data->possession) == Team1;
    GSize team_1_size = graphics_text_layout_get_content_size(data->team1, font_team, score_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
    GRect team_1_bounds = GRect(score_1_bounds.size.w/2 - team_1_size.w/2 - (team_1_possession ? possession_offset : 0), 42, team_1_size.w, 24);
    graphics_draw_text(ctx, data->team1, font_team, team_1_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    if (team_1_possession) {
        graphics_fill_circle(ctx, GPoint(team_1_bounds.origin.x + team_1_bounds.size.w + 8 + 3, team_1_bounds.origin.y + team_1_bounds.size.h /2), 3);
    }

    bool team_2_possession = (data->possession) == Team2;
    GSize team_2_size = graphics_text_layout_get_content_size(data->team2, font_team, score_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
    GRect team_2_bounds = GRect(score_2_bounds.origin.x + score_2_bounds.size.w/2 - team_2_size.w/2 - (team_2_possession ? possession_offset : 0), 42, team_2_size.w, 24);
    graphics_draw_text(ctx, data->team2, font_team, team_2_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    if (team_2_possession) {
        graphics_fill_circle(ctx, GPoint(team_2_bounds.origin.x + team_2_bounds.size.w + 8 + 3, team_2_bounds.origin.y + team_2_bounds.size.h /2), 3);
    }
    
}

static Layer *score_layer_create(GRect bounds, const Game game) {
    Layer *score; 
    bounds.size.h = 60;

    #ifdef PBL_ROUND
        bounds.size.w -= 32;
        bounds.origin.x += 16;
    #endif

    score = layer_create_with_data(bounds, sizeof(ScoreData));

    ScoreData *layer_data = (ScoreData *)layer_get_data(score);
    layer_data->team1 = game.team1;
    layer_data->score1 = game.score1;
    layer_data->team2 = game.team2;
    layer_data->score2 = game.score2;
    layer_data->possession = game.possession;

    layer_set_update_proc(score, score_update_proc);

    return score;
}


static void initialise_ui(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    s_icon_image = gbitmap_create_with_resource(sport_get_icon_res_small(s_game.sport));

    s_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_status_bar, GColorElectricUltramarine, GColorWhite);
    layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));

    bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
    bounds.size.h -= STATUS_BAR_LAYER_HEIGHT;

    s_header = create_header_layer(bounds, (HeaderData) {
        .icon = s_icon_image, 
        .title = sport_get_name(s_game.sport),
        .info = s_game.time,
    });

    layer_add_child(window_layer, s_header);

    int header_height = layer_get_bounds(s_header).size.h;
    
    bounds.origin.y += header_height;
    bounds.size.h -= header_height; 

    #ifdef PBL_ROUND
        bounds.origin.y += 4;
        bounds.size.h -= 4; 
        GRect time_bounds = bounds;
        time_bounds.size.h = 24;
        s_time = text_layer_create(time_bounds);
        text_layer_set_font(s_time, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
        text_layer_set_text_alignment(s_time, GTextAlignmentCenter);
        text_layer_set_text(s_time, s_game.time);
        Layer *time_layer =  text_layer_get_layer(s_time);
        layer_set_clips(time_layer, false);
        layer_add_child(window_layer, time_layer);

        bounds.origin.y += 16;
        bounds.size.h -= 16; 
    #endif

    s_score = score_layer_create(bounds, s_game);
    layer_add_child(window_layer, s_score);
}

static void destroy_ui(Window *window) {
    status_bar_layer_destroy(s_status_bar);
    layer_destroy(s_header);
    text_layer_destroy(s_time);
    layer_destroy(s_score);
    gbitmap_destroy(s_icon_image);
}

static void handle_window_unload(Window *window) {
    destroy_ui(window);
}
void show_score_screen(const Game game)
{
    scoreWindow = window_create();
    s_game = game;
    initialise_ui(scoreWindow);
    window_set_window_handlers(
        scoreWindow,
        (WindowHandlers){
            .unload = handle_window_unload,
        });
    window_set_click_config_provider(scoreWindow, click_config_provider);
    window_stack_push(scoreWindow, true);
}

void hide_score_screen(void)
{
    window_stack_remove(scoreWindow, true);
    window_destroy(scoreWindow);
}
