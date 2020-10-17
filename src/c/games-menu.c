#include <stdbool.h>
#include "pebble.h"
#include "games-menu.h"
#include "progress-layer.h"
#include "header.h"
#include "utils.h"
#include "models.h"
#include "score-screen.h"
#include "comms.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 3
#define NUM_SECOND_MENU_ITEMS 1

static Window *gamesWindow;
static StatusBarLayer *s_status_bar;
static Layer *s_header;
static MenuLayer *s_menu_layer;
static GBitmap *s_icon_image;
static TextLayer *s_loading_text;
static ProgressLayer *s_loading_progress;

static int game_count;
static Game *games;
static char *games_small[];

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

static void menu_cell_game_large_draw(GContext* ctx, const Layer *cell_layer, bool selected, const Game game) {

    
    GFont font_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    GFont font_regular = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    GRect cell_bounds = layer_get_bounds(cell_layer);

    int horz_padding = PBL_IF_ROUND_ELSE(16, 8); 
    int vert_padding = PBL_IF_ROUND_ELSE(4, 0); 

    GSize team_1_name_size = graphics_text_layout_get_content_size(game.team1, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_1_name_bounds = GRect(horz_padding, vert_padding, team_1_name_size.w, 18);
    graphics_draw_text(ctx, game.team1, font_bold, team_1_name_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
    
    GSize team_2_name_size = graphics_text_layout_get_content_size(game.team2, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_2_name_bounds = GRect(horz_padding,vert_padding + 18, team_2_name_size.w, 18);
    graphics_draw_text(ctx, game.team2, font_bold, team_2_name_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

    GSize team_1_score_size = graphics_text_layout_get_content_size(game.score1, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_1_score_bounds = GRect(cell_bounds.size.w - horz_padding - team_1_score_size.w, vert_padding, team_1_score_size.w, 18);
    graphics_draw_text(ctx, game.score1, font_bold, team_1_score_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

    GSize team_2_score_size = graphics_text_layout_get_content_size(game.score2, font_bold, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect team_2_score_bounds = GRect(cell_bounds.size.w - horz_padding - team_2_score_size.w, vert_padding + 18, team_2_score_size.w, 18);
    graphics_draw_text(ctx, game.score2, font_bold, team_2_score_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

    GSize details_size = graphics_text_layout_get_content_size(game.details, font_regular, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect details_bounds = GRect(horz_padding, vert_padding + 36, details_size.w, 14);
    graphics_draw_text(ctx, game.details, font_regular, details_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

    GSize time_size = graphics_text_layout_get_content_size(game.time, font_regular, cell_bounds, GTextOverflowModeTrailingEllipsis, GAlignTopLeft);
    GRect time_bounds = GRect(cell_bounds.size.w - horz_padding - time_size.w, vert_padding + 36, time_size.w, 14);
    graphics_draw_text(ctx, game.time, font_regular, time_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);

    GColor fill_color = GColorBlack;
    if (selected) {
        fill_color = GColorWhite;
    }
    graphics_context_set_fill_color(ctx, fill_color);
    if (game.possession == Team1) {
        graphics_fill_circle(ctx, GPoint(horz_padding + team_1_name_size.w + 6, vert_padding + 12), 2);
    } else if (game.possession == Team2) {
        graphics_fill_circle(ctx, GPoint(horz_padding + team_2_name_size.w + 6, vert_padding + 30), 2);
    }
}

static void menu_cell_game_small_draw(GContext* ctx, const Layer *cell_layer, const Game game) {
    graphics_draw_text(ctx, game.summary, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), layer_get_bounds(cell_layer), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

    int index = cell_index->row;
    const Game game = games[index];
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
    const Game game = games[cell_index->row];
    printf("Opening %s - %s", game.team1, game.team2);
    show_score_screen(game);
}

static void on_games_loaded(Sport sport, int loaded_game_count, Game *loaded_games) {
    printf("games loaded callback");
    game_count = loaded_game_count;
    games = loaded_games;
    if(s_menu_layer != NULL) {
        menu_layer_reload_data(s_menu_layer);
    }
    layer_set_hidden(text_layer_get_layer(s_loading_text), true); 
    layer_set_hidden(s_loading_progress, true); 
}

static void on_games_error(AppMessageResult reason) {
    
}

static void initialise_ui(Window *window, Sport sport)
{

    request_games(sport, on_games_loaded, on_games_error);

    s_icon_image = gbitmap_create_with_resource(sport_get_icon_res_small(sport));

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    s_status_bar = status_bar_layer_create();
    status_bar_layer_set_colors(s_status_bar, GColorElectricUltramarine, GColorWhite);
    layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));

    bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
    bounds.size.h -= STATUS_BAR_LAYER_HEIGHT;


    s_header = create_header_layer(bounds, (HeaderData) {
        .icon = s_icon_image, 
        .title = sport_get_name(sport),
    });
    int header_height = PBL_IF_RECT_ELSE(layer_get_bounds(s_header).size.h, 8);
    
    bounds.origin.y += header_height;
    bounds.size.h -= header_height; 

    s_menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
        .get_num_rows = menu_get_num_rows_callback,
        .get_cell_height = menu_get_row_height_callback,
        .draw_row = menu_draw_row_callback,
        .select_click = menu_select_callback,
    });

    menu_layer_set_highlight_colors(s_menu_layer, GColorElectricUltramarine, GColorWhite);
    menu_layer_set_click_config_onto_window(s_menu_layer, window);

    layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

    layer_add_child(window_layer, s_header);

    GRect loading_section_bounds = bounds;
    loading_section_bounds.origin.y += bounds.size.h / 2 - 20;
    loading_section_bounds.origin.x += PBL_IF_ROUND_ELSE(32, 16);
    loading_section_bounds.size.w -= PBL_IF_ROUND_ELSE(64, 32);
    loading_section_bounds.size.h = 28;


    GRect loading_bar_bounds = loading_section_bounds;
    loading_bar_bounds.size.h = 4;
    s_loading_progress = progress_layer_create(loading_bar_bounds);
    progress_layer_set_background_color(s_loading_progress, GColorBabyBlueEyes);
    progress_layer_set_foreground_color(s_loading_progress, GColorElectricUltramarine);
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
}

static void destroy_ui(Window *window)
{
    menu_layer_destroy(s_menu_layer);
    status_bar_layer_destroy(s_status_bar);
    layer_destroy(s_header);
    progress_layer_destroy(s_loading_progress);
    text_layer_destroy(s_loading_text);
    gbitmap_destroy(s_icon_image);
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
