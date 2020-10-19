#include "pebble.h"
#include "../../../data/model/models.h"

typedef Layer ErrorLayer;

ErrorLayer* error_layer_create(GRect bounds);
void error_layer_set_error(ErrorLayer *error_layer, AppError error);
void error_layer_destroy(ErrorLayer *error_layer);