#include "pebble.h"
#include "../model/models.h"
#include "comms.h"
#include "games/games-handler.h"



// Largest expected inbox and outbox message sizes
const uint32_t inbox_size = 256;
const uint32_t outbox_size = 256;

static bool s_js_ready;

void request_games(Sport sport, GamesSuccessCallback on_success, GamesErrorCallback on_error) {
    handle_request_games(sport, on_success, on_error);
}

void clear_games() {
    handle_clear_games();
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "recieved js message");
    // A new message has been successfully received
    Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_READY);

    // A list of games has been recieved
    Tuple *games_tuple = dict_find(iter, MESSAGE_KEY_SEND_GAME);

    if(ready_tuple) {
        // PebbleKit JS is ready! Safe to send messages
        APP_LOG(APP_LOG_LEVEL_DEBUG, "JS Ready");
        s_js_ready = true;
    }

    if(games_tuple) {
        handle_games_recieved(iter);
    }


}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context) {
  // The message just sent has been successfully delivered

}

static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  // The message just sent failed to be delivered
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message send failed. Reason: %d", (int)reason);
}

void setup_comms() {
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_open(inbox_size, outbox_size);
}

void destroy_comms() {
    app_message_deregister_callbacks();
}