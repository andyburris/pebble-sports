#include "pebble.h"
#include "models.h"

#ifndef COMMS_H_
#define COMMS_H_

//! Function signature for the callback on a successful request for games
//! @param reason The reason for the error
typedef void (*GamesErrorCallback)(AppMessageResult reason);


//! Function signature for the callback on a successful request for games
//! @param sport The sport that was requested
//! @param games The array of games returned for the sport
typedef void (*GamesSuccessCallback)(Sport sport, int game_count, Game *games);

#endif

void request_games(Sport sport, GamesSuccessCallback on_success, GamesErrorCallback on_error);
void clear_games();
void setup_comms();
void destroy_comms();