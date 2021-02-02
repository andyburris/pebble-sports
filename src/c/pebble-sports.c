#include "pebble.h"
#include "ui/screens/leagues/league-menu.h"
#include "data/comms/comms.h"
#include "data/comms/prefs/prefs-handler.h"

static void init() {
    load_settings();
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