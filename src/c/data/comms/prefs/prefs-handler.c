#include "pebble.h"
#include "prefs-handler.h"

ClaySettings clay_settings;

static void save_settings(ClaySettings new) {
    clay_settings = new;
    persist_write_data(SETTINGS_KEY, &clay_settings, sizeof(ClaySettings));
}

void load_settings() {
    clay_settings = (ClaySettings) {
        .show_record = ShowRecordFinalOnly,
    };
    persist_read_data(SETTINGS_KEY, &clay_settings, sizeof(ClaySettings));
}

void handle_config_recieved(DictionaryIterator *iter) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "recieved config change");
    Tuple *records_tuple = dict_find(iter, MESSAGE_KEY_CONFIG_SHOW_RECORDS);
    if(records_tuple) {
        char *records_value = records_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "recieved config change, records_value = %s", records_value);
        ShowRecordSetting show_record_setting = (strcmp(records_value, "never")==0) ? ShowRecordNever : (strcmp(records_value, "always")==0) ? ShowRecordAlways : ShowRecordFinalOnly;
        ClaySettings new = (ClaySettings) {
            .show_record = show_record_setting,
        };
        save_settings(new);
    }
}