#include "../../model/models.h"
#include "../comms.h"

void handle_request_games(Sport sport, GamesSuccessCallback on_success, GamesErrorCallback on_error);
void handle_clear_games();
void handle_games_recieved(DictionaryIterator *iter);