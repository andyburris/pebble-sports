#include <stdbool.h>
#include "pebble.h"
#include "games-menu.h"
#include "../../layers/progress/progress-layer.h"
#include "../../layers/error/error-layer.h"
#include "../../layers/header/header.h"
#include "../score/score-screen.h"
#include "../../../data/model/models.h"
#include "../../../data/comms/comms.h"
#include "../../../data/comms/games/games-handler.h"
#include "../../../utils/utils.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 3
#define NUM_SECOND_MENU_ITEMS 1

static Window *gamesWindow;
static StatusBarLayer *s_status_bar;
static HeaderLayer *s_header;
static MenuLayer *s_menu_layer;
static GBitmap *s_icon_image;
static TextLayer *s_loading_text;
static ProgressLayer *s_loading_progress;
static ErrorLayer *s_error_layer;
#if defined(PBL_ROUND)
static ContentIndicator *s_content_indicator;
static Layer *s_indicator_layer;
#endif


static int game_count;
static Game **games;
static Sport s_sport;
static bool refreshing;

static void on_games_loaded(int loaded_game_count, Game **loaded_games) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "games loaded callback");
    game_count = loaded_game_count;
    games = loaded_games;
    if(s_menu_layer != NULL) {
        menu_layer_reload_data(s_menu_layer);
    }
    #if defined(PBL_ROUND)
    content_indicator_set_content_available(s_content_indicator, ContentIndicatorDirectionDown, game_count > 2);
    #endif
    layer_set_hidden(text_layer_get_layer(s_loading_text), true); 
    progress_layer_set_hidden(s_loading_progress, true); 
    refreshing = false;
}

static void on_games_error(AppError error) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "games error callback");
    layer_set_hidden(text_layer_get_layer(s_loading_text), true); 
    progress_layer_set_hidden(s_loading_progress, true); 
    error_layer_set_error(s_error_layer, error);
    layer_set_hidden(s_error_layer, false);
    refreshing = false;
}

static void refresh_games(Sport sport) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "refreshing games = %s", refreshing ? "true" : "false");
    refreshing = true;
    game_count = 0;

    layer_set_hidden(s_error_layer, true);
    layer_set_hidden(text_layer_get_layer(s_loading_text), false); 
    progress_layer_set_hidden(s_loading_progress, false); 
    handle_request_games(sport, on_games_loaded, on_games_error);
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return game_count;
}

static int16_t menu_get_row_height_callback (MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    #if defined(PBL_RECT)
        /* Rectangular UI code */
        return 58;
    #elif defined(PBL_ROUND)
        /* Round UI code */
        bool selected = menu_layer_get_selected_index(s_menu_layer).row == cell_index->row;
        if (selected) {
            return 66;
        } else {
            return 26;
        }
    #endif
} 

static void menu_cell_game_large_draw(GContext* ctx, const Layer *cell_layer, bool selected, const Game *game) {
    GFont font_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GFont font_regular = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    GRect cell_bounds = layer_get_bounds(cell_layer);

    int horz_padding = PBL_IF_ROUND_ELSE(16, 8); 
    int vert_padding = PBL_IF_ROUND_ELSE(4, 0); 

    GSize team_1_name_size = graphics_text_layout_get_content_size(game->team1.name, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_1_name_bounds = GRect(horz_padding, vert_padding, team_1_name_size.w, 18);
    graphics_draw_text(ctx, game->team1.name, font_bold, team_1_name_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
    
    GSize team_2_name_size = graphics_text_layout_get_content_size(game->team2.name, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_2_name_bounds = GRect(horz_padding,vert_padding + 18, team_2_name_size.w, 18);
    graphics_draw_text(ctx, game->team2.name, font_bold, team_2_name_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

    GSize team_1_score_size = graphics_text_layout_get_content_size(game->team1.score, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_1_score_bounds = GRect(cell_bounds.size.w - horz_padding - team_1_score_size.w, vert_padding, team_1_score_size.w, 18);
    graphics_draw_text(ctx, game->team1.score, font_bold, team_1_score_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

    GSize team_2_score_size = graphics_text_layout_get_content_size(game->team2.score, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_2_score_bounds = GRect(cell_bounds.size.w - horz_padding - team_2_score_size.w, vert_padding + 18, team_2_score_size.w, 18);
    graphics_draw_text(ctx, game->team2.score, font_bold, team_2_score_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

    GSize details_size = graphics_text_layout_get_content_size(game->details, font_regular, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect details_bounds = GRect(horz_padding, vert_padding + 36, details_size.w, 14);
    graphics_draw_text(ctx, game->details, font_regular, details_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

    GSize time_size = graphics_text_layout_get_content_size(game->time, font_regular, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect time_bounds = GRect(cell_bounds.size.w - horz_padding - time_size.w, vert_padding + 36, time_size.w, 14);
    graphics_draw_text(ctx, game->time, font_regular, time_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

    GColor fill_color = GColorBlack;
    if (selected) {
        fill_color = GColorWhite;
    }
    graphics_context_set_fill_color(ctx, fill_color);
    if (game->possession == Team1) {
        graphics_fill_circle(ctx, GPoint(horz_padding + team_1_name_size.w + 6, vert_padding + 12), 2);
    } else if (game->possession == Team2) {
        graphics_fill_circle(ctx, GPoint(horz_padding + team_2_name_size.w + 6, vert_padding + 30), 2);
    }
}

static void menu_cell_game_small_draw(GContext* ctx, const Layer *cell_layer, const Game *game) {
    graphics_draw_text(ctx, game->summary, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), layer_get_bounds(cell_layer), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

    int index = cell_index->row;
    const Game *game = games[index];
    bool selected = menu_layer_get_selected_index(s_menu_layer).row == cell_index->row;

    #if defined(PBL_RECT)
        /* Rectangular UI code */
        menu_cell_game_large_draw(ctx, cell_layer, selected, game);
    #elif defined(PBL_ROUND)
        /* Round UI code */
        if (selected) {
            menu_cell_game_large_draw(ctx, cell_layer, selected, game);
        } else {
            menu_cell_game_small_draw(ctx, cell_layer, game);
        }
    #endif
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context){
    APP_LOG(APP_LOG_LEVEL_WARNING, "menu select, refreshing = %s", refreshing ? "true" : "false");
    if(game_count > 0) {
        Game *game = games[cell_index->row];
        printf("Opening %s - %s", game->team1.name, game->team2.name);
        show_score_screen(game);
    } else if (!refreshing) {
        refresh_games(s_sport);
    }
}

static void menu_selection_changed_callback(MenuLayer *menu_layer, MenuIndex new_index, MenuIndex old_index, void *callback_context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "menu selection changed from %d to %d", old_index.row, new_index.row);
    GRect old_header_bounds = layer_get_bounds(s_header);
    if (new_index.row == 0) {
        layer_set_hidden(status_bar_layer_get_layer(s_status_bar), false);
        header_layer_set_under_status_bar(s_header, true);
        //layer_set_bounds(s_header, GRect(0, STATUS_BAR_LAYER_HEIGHT, old_header_bounds.size.w, 26));
        APP_LOG(APP_LOG_LEVEL_DEBUG, "expanded, header y from %d to %d", old_header_bounds.origin.y, layer_get_bounds(s_header).origin.y);
        //header_layer_set_under_status_bar(s_header, true);
    } else if (old_index.row == 0) { //only do scroll conversion if coming from index 0
        layer_set_hidden(status_bar_layer_get_layer(s_status_bar), true);
        header_layer_set_under_status_bar(s_header, false);
        //layer_set_bounds(s_header, GRect(0, 0, old_header_bounds.size.w, 56));
        APP_LOG(APP_LOG_LEVEL_DEBUG, "collapsed, header y from %d to %d", old_header_bounds.origin.y, layer_get_bounds(s_header).origin.y);
        //header_layer_set_under_status_bar(s_header, false);
    }
    #if defined(PBL_ROUND)
    content_indicator_set_content_available(s_content_indicator, ContentIndicatorDirectionDown, new_index.row < game_count - 2);
    #endif
    
}

static void initialise_ui(Window *window, Sport sport)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "opening games-menu");
    s_sport = sport;
    s_icon_image = gbitmap_create_with_resource(sport_get_icon_res_small(sport));

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    s_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_status_bar, GColorDukeBlue, GColorWhite);

    // bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
    // bounds.size.h -= STATUS_BAR_LAYER_HEIGHT; 


    s_header = create_header_layer(bounds, (HeaderData) {
        .icon = s_icon_image, 
        .title = sport_get_name(sport),
        .under_status_bar = true,
    });
    int header_height = PBL_IF_RECT_ELSE(layer_get_bounds(s_header).size.h, 8);

    // bounds.origin.y -= STATUS_BAR_LAYER_HEIGHT;
    // bounds.size.h += STATUS_BAR_LAYER_HEIGHT; 
    
    bounds.origin.y += header_height + 4;
    bounds.size.h -= header_height + 4; 

    s_menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
        .get_num_rows = menu_get_num_rows_callback,
        .get_cell_height = menu_get_row_height_callback,
        .draw_row = menu_draw_row_callback,
        .select_click = menu_select_callback,
        .selection_changed = menu_selection_changed_callback,
    });

    menu_layer_set_highlight_colors(s_menu_layer, GColorDukeBlue, GColorWhite);
    menu_layer_set_click_config_onto_window(s_menu_layer, window);

    layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

    layer_add_child(window_layer, s_header);
    layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));

    GRect loading_section_bounds = bounds;
    loading_section_bounds.origin.y += bounds.size.h / 2 - 16;
    loading_section_bounds.origin.x += PBL_IF_ROUND_ELSE(32, 16);
    loading_section_bounds.size.w -= PBL_IF_ROUND_ELSE(64, 32);
    loading_section_bounds.size.h = 28;


    GRect loading_bar_bounds = loading_section_bounds;
    loading_bar_bounds.size.h = 4;
    s_loading_progress = progress_layer_create(loading_bar_bounds);
    progress_layer_set_background_color(s_loading_progress, GColorVeryLightBlue);
    progress_layer_set_foreground_color(s_loading_progress, GColorDukeBlue);
    progress_layer_set_corner_radius(s_loading_progress, 2);
    layer_add_child(window_layer, s_loading_progress);

    GRect loading_text_bounds = loading_section_bounds;
    loading_text_bounds.origin.y += 4;
    loading_text_bounds.size.h = 24;
    s_loading_text = text_layer_create(loading_text_bounds);
    text_layer_set_font(s_loading_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(s_loading_text, GTextAlignmentCenter);
    text_layer_set_text(s_loading_text, "Loading games");
    layer_add_child(window_layer, text_layer_get_layer(s_loading_text));

    GRect error_layer_bounds = GRect(bounds.origin.x + PBL_IF_ROUND_ELSE(32, 16), bounds.origin.y + bounds.size.h / 2 - PBL_IF_ROUND_ELSE(24, 36), bounds.size.w - PBL_IF_ROUND_ELSE(64, 32), 61);
    s_error_layer = error_layer_create(error_layer_bounds);
    layer_set_hidden(s_error_layer, true);
    layer_add_child(window_layer, s_error_layer);

    #if defined(PBL_ROUND)
    s_content_indicator = content_indicator_create();
    s_indicator_layer = layer_create(GRect(0, layer_get_frame(window_layer).size.h - STATUS_BAR_LAYER_HEIGHT, bounds.size.w, STATUS_BAR_LAYER_HEIGHT));
    const ContentIndicatorConfig down_config = (ContentIndicatorConfig) {
        .layer = s_indicator_layer,
        .times_out = false,
        .alignment = GAlignCenter,
        .colors = {
            .foreground = GColorBlack,
            .background = GColorWhite
        }
    };
    content_indicator_configure_direction(s_content_indicator, ContentIndicatorDirectionDown, &down_config);
    layer_add_child(window_layer, s_indicator_layer);
    #endif

    refresh_games(sport);
}

static void destroy_ui(Window *window)
{
    menu_layer_destroy(s_menu_layer);
    status_bar_layer_destroy(s_status_bar);
    layer_destroy(s_header);
    progress_layer_destroy(s_loading_progress);
    text_layer_destroy(s_loading_text);
    gbitmap_destroy(s_icon_image);
    error_layer_destroy(s_error_layer);
    #if defined(PBL_ROUND)
    layer_destroy(s_indicator_layer);
    #endif
}

static void handle_window_unload(Window *window){
    destroy_ui(window);
}
void show_games_menu(Sport sport)
{
    gamesWindow = window_create();
    initialise_ui(gamesWindow, sport);
    window_set_window_handlers(
        gamesWindow,
        (WindowHandlers){
            .unload = handle_window_unload,
        });
    window_stack_push(gamesWindow, true);
}

void hide_games_menu(void)
{
    window_stack_remove(gamesWindow, true);
    window_destroy(gamesWindow);
}
