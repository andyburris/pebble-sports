#include "pebble.h"
#include "score-screen.h"
#include "score-layer.h"
#include "schedule-layer.h"
#include "../../layers/header/header.h"
#include "../action/action-menu.h"
#include "../../../data/model/models.h"
#include "../../../data/comms/comms.h"
#include "../../../utils/utils.h"


static Window *scoreWindow;
static StatusBarLayer *s_status_bar;
static Layer *s_header;
static TextLayer *s_time;
static TextLayer *s_details;
static Layer *s_score;
static GBitmap *s_icon_image;
static Game *s_game;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    game_action_menu_open(s_game);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "team_1_label = %s", ((ActionMenuLabels *)action_menu_get_context(action_menu))->team_1_label);
}

static void click_config_provider(void *context) {
    // Subcribe to button click events here

    ButtonId id = BUTTON_ID_SELECT;  // The Select button
    window_single_click_subscribe(id, select_click_handler);
}

static void initialise_ui(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    s_icon_image = gbitmap_create_with_resource(sport_get_icon_res_small(s_game->sport));

    s_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_status_bar, GColorOxfordBlue, GColorWhite);
    layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));

    bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
    bounds.size.h -= STATUS_BAR_LAYER_HEIGHT;

    s_header = create_header_layer(bounds, (HeaderData) {
        .icon = s_icon_image, 
        .title = sport_get_name(s_game->sport),
        .info = s_game->time,
    });

    layer_add_child(window_layer, s_header);

    int header_height = layer_get_bounds(s_header).size.h;
    
    bool is_scheduled = strlen(s_game->team1.score) == 0;

    bounds.origin.y += header_height + (is_scheduled ? 12 : 4);
    bounds.size.h -= header_height + (is_scheduled ? 12 : 4); 

    s_score = is_scheduled ? schedule_layer_create(bounds, s_game) : score_layer_create(bounds, s_game);
    layer_add_child(window_layer, s_score);

    bounds.origin.y += layer_get_bounds(s_score).size.h + 4;
    bounds.size.h -= layer_get_bounds(s_score).size.h + 4; 

    GRect details_bounds = bounds;
    details_bounds.size.h = 18;
    s_details = text_layer_create(details_bounds);
    text_layer_set_font(s_details, fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_text_alignment(s_details, GTextAlignmentCenter);
    text_layer_set_text(s_details, s_game->details);
    Layer *details_layer =  text_layer_get_layer(s_details);
    layer_set_clips(details_layer, false);
    layer_add_child(window_layer, details_layer);

    bounds.origin.y += 14;
    bounds.size.h -= 14; 

    GRect time_bounds = bounds;
    time_bounds.size.h = 24;
    s_time = text_layer_create(time_bounds);
    text_layer_set_font(s_time, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(s_time, GTextAlignmentCenter);
    text_layer_set_text(s_time, s_game->time);
    Layer *time_layer =  text_layer_get_layer(s_time);
    layer_set_clips(time_layer, false);
    layer_add_child(window_layer, time_layer);



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
void show_score_screen(Game *game)
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
