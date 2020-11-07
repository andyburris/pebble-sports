#include "pebble.h"
#include "../../../data/model/models.h"
#include "../../../data/comms/favorites/favorites-handler.h"
#include "action-menu.h"

// Milliseconds between frames
#define DELTA 13

typedef struct {
    Layer *content_layer;
    AppTimer *close_timer;
} ResultWindowData;

typedef struct {
    GDrawCommandSequence *command_seq;
    int frame;
    char *label;
} ResultLayerData;

static void on_unload(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "destroying window data");
    ResultWindowData *window_data = (ResultWindowData *)window_get_user_data(window);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "retrieved window data");
    ResultLayerData *layer_data = (ResultLayerData *) layer_get_data(window_data->content_layer);
    gdraw_command_sequence_destroy(layer_data->command_seq);
    //app_timer_cancel(window_data->animation_timer);
    if(window_data->close_timer) {
        app_timer_cancel(window_data->close_timer);
    }
    //free(layer_data->label);
    layer_destroy(window_data->content_layer);
}

static void close_timer_callback(void *context) {
    Window *window = (Window *) context;
    window_stack_remove(context, true);
}

static void next_frame_handler(void *context) {

    Layer *layer = (Layer *) context;
    ResultLayerData *layer_data = (ResultLayerData *) layer_get_data(layer);


    // Advance to the next frame if animation isn't done
    int num_frames = gdraw_command_sequence_get_num_frames(layer_data->command_seq);
    if (layer_data->frame < num_frames - 1) {
        layer_data->frame++;
        // Draw the next frame
        layer_mark_dirty(layer);
        
        // Continue the sequence
        app_timer_register(DELTA, next_frame_handler, context);
    }
}


static void result_layer_update_proc(Layer *layer, GContext *ctx) {
    ResultLayerData *layer_data = (ResultLayerData *) layer_get_data(layer);

    GRect bounds = layer_get_bounds(layer);

    GFont label_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

    GSize text_size = graphics_text_layout_get_content_size(layer_data->label, label_font, bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter);
    
    int content_height = 80 + 4 + text_size.h;

    GRect content_bounds = GRect(bounds.origin.x, bounds.size.h / 2 - (content_height / 2), bounds.size.w, content_height);

    GPoint icon_position = GPoint(content_bounds.size.w / 2 - 40, content_bounds.origin.y);
    // Get the next frame
    GDrawCommandFrame *frame = gdraw_command_sequence_get_frame_by_index(layer_data->command_seq, layer_data->frame);
    
    // If another frame was found, draw it
    if (frame) {
        gdraw_command_frame_draw(ctx, layer_data->command_seq, frame, icon_position);
    }

    GRect text_bounds = GRect(content_bounds.origin.x, content_bounds.origin.y + 84, content_bounds.size.w, text_size.h);
    graphics_draw_text(ctx, layer_data->label, label_font, text_bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

}

Window *result_window_create_window(Game *game, MenuAction action, FavoriteChangeResult result) {
    Window *result_window = window_create();
    
    window_set_background_color(result_window, GColorOxfordBlue);

    Layer *root_layer = window_get_root_layer(result_window);

    GRect content_bounds = layer_get_bounds(root_layer);
    content_bounds.origin.x += PBL_IF_RECT_ELSE(16, 32);
    content_bounds.size.w -= PBL_IF_RECT_ELSE(32, 64);
    Layer *content_layer = layer_create_with_data(content_bounds, sizeof(ResultLayerData));
    ResultLayerData *layer_data = (ResultLayerData *) layer_get_data(content_layer);
    layer_data->frame = 0;

    if (result == FavoriteChangeFailed) {
        layer_data->command_seq = gdraw_command_sequence_create_with_resource(RESOURCE_ID_ANIM_DELETED);
        layer_data->label = malloc(15);
        strcpy(layer_data->label, "No connection");
    } else {    
        char *action_label;

        switch (result) {
            case FavoriteAdded:
                action_label = " added to ";
                layer_data->command_seq = gdraw_command_sequence_create_with_resource(RESOURCE_ID_ANIM_CONFIRM);
                break;
            case FavoriteRemoved:
                action_label = " removed from ";
                layer_data->command_seq = gdraw_command_sequence_create_with_resource(RESOURCE_ID_ANIM_DELETED);
                break;
            default:
                action_label = "";
                break;
        }
        

        switch (action)
        {
        case ACTION_GAME:
            layer_data->label = malloc(strlen(game->summary) + strlen(action_label) + 12);
            strcpy(layer_data->label, game->summary);
            break;
        
        case ACTION_TEAM_1:
            layer_data->label = malloc(strlen(game->team1.name) + strlen(action_label) + 12);
            strcpy(layer_data->label, game->team1.name);
            break;

        case ACTION_TEAM_2:
            layer_data->label = malloc(strlen(game->team2.name) + strlen(action_label) + 15);
            strcpy(layer_data->label, game->team2.name);
            break;
        default:
            break;
        }

        strcat(layer_data->label, " ");
        strcat(layer_data->label, action_label);
        strcat(layer_data->label, "Favorites");

    }
    
    layer_set_update_proc(content_layer, result_layer_update_proc);

    layer_add_child(root_layer, content_layer);

    ResultWindowData *data = malloc(sizeof(ResultWindowData)); 
    data->content_layer = content_layer;

    window_set_user_data(result_window, data);

    window_set_window_handlers(result_window, (WindowHandlers) {
        .unload = on_unload
    });

    // Start the animation
    app_timer_register(DELTA, next_frame_handler, content_layer);

    data->close_timer = app_timer_register(2000, close_timer_callback, result_window);

    return result_window;
}