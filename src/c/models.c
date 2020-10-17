#include "pebble.h"
#include "models.h"

int sport_get_icon_res_small(Sport sport) {
    switch (sport){
        case SportNFL:
            return RESOURCE_ID_FOOTBALL;
        
         case SportMLB:
            return RESOURCE_ID_BASEBALL;

        case SportNHL:
            return RESOURCE_ID_HOCKEY;

        case SportNBA:
            return RESOURCE_ID_BASKETBALL;

        default:
            return RESOURCE_ID_STAR;
            break;
    }
}

int sport_get_icon_res_large(Sport sport) {
    switch (sport){
        case SportNFL:
            return RESOURCE_ID_FOOTBALL_LARGE;
        
         case SportMLB:
            return RESOURCE_ID_BASEBALL_LARGE;

        case SportNHL:
            return RESOURCE_ID_HOCKEY_LARGE;

        case SportNBA:
            return RESOURCE_ID_BASKETBALL_LARGE;

        default:
            return RESOURCE_ID_STAR_LARGE;
            break;
    }
}

char* sport_get_name(Sport sport) {
    switch (sport){
        case SportNFL:
            return "NFL";
        
        case SportMLB:
            return "MLB";

        case SportNHL:
            return "NHL";

        case SportNBA:
            return "NBA";

        default:
            return "Favorite";
            break;
    }
}