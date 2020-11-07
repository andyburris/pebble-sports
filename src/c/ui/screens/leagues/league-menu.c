#include "pebble.h"
#include "league-menu.h"
#include "../games/games-menu.h"
#include "../../../data/model/models.h"
#include "../../../data/comms/comms.h"
#include "../../../utils/utils.h"

#define NUM_MENU_SECTIONS 1
#define NUM_LEAGUES 5

static Window *leagueMenuWindow;
static MenuLayer *s_menu_layer;
static Sport s_sports_items[NUM_LEAGUES] = {Favorites, SportNFL, SportMLB, SportNHL, SportNBA};
static GBitmap *s_icons[NUM_LEAGUES];


static void menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
    show_games_menu(s_sports_items[cell_index->row]);
}


static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
    Sport sport = s_sports_items[cell_index->row];
    // Draw this row's index
    menu_cell_basic_draw(ctx, cell_layer, sport_get_name(sport), NULL, s_icons[cell_index->row]);
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NUM_LEAGUES;
}

static int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
    const int16_t cell_height = 64;
    return cell_height;
}

static void initialise_ui(Window *window) {
    for (size_t i = 0; i < NUM_LEAGUES; i++){
        Sport sport = s_sports_items[i];

        s_icons[i] = gbitmap_create_with_resource(sport_get_icon_res_large(sport));
    }

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    s_menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
        .get_num_rows = menu_get_num_rows_callback,
        .get_cell_height = menu_get_cell_height_callback,
        .draw_row = menu_draw_row_callback,
        .select_click = menu_select_callback,
    });
    menu_layer_set_highlight_colors(s_menu_layer, GColorOxfordBlue, GColorWhite);
    menu_layer_set_click_config_onto_window(s_menu_layer, window);


    layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void destroy_ui(Window *window) {
    menu_layer_destroy(s_menu_layer);
    for (size_t i = 0; i < NUM_LEAGUES; i++){
        gbitmap_destroy(s_icons[i]);
    }
}

static void handle_window_appear(Window *window) {
    clear_games();
}

static void handle_window_unload(Window *window) {
    destroy_ui(window);
}
void show_league_menu(void)
{
    leagueMenuWindow = window_create();
    initialise_ui(leagueMenuWindow);
    window_set_window_handlers(
        leagueMenuWindow,
        (WindowHandlers){
            .appear = handle_window_appear,
            .unload = handle_window_unload,
        });
    window_stack_push(leagueMenuWindow, true);
}

void hide_league_menu(void)
{
    window_stack_remove(leagueMenuWindow, true);
    window_destroy(leagueMenuWindow);
}
