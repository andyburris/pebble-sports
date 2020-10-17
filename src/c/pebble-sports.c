#include "pebble.h"
#include "league-menu.h"
#include "comms.h"

static void init() {
    setup_comms();
    show_league_menu();
}

static void deinit() {
    destroy_comms();
    hide_league_menu();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}