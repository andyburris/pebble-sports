#include "pebble.h"
#include "models.h"

int sport_get_icon_res_small(Sport sport) {
    switch (sport){
        case SportNFL:
            return RESOURCE_ID_FOOTBALL_16;
        
         case SportMLB:
            return RESOURCE_ID_BASEBALL_16;

        case SportNHL:
            return RESOURCE_ID_HOCKEY_16;

        case SportNBA:
            return RESOURCE_ID_BASKETBALL_16;

        default:
            return RESOURCE_ID_STAR_16;
            break;
    }
}

int sport_get_icon_res_large(Sport sport) {
    switch (sport){
        case SportNFL:
            return RESOURCE_ID_FOOTBALL_25;
        
         case SportMLB:
            return RESOURCE_ID_BASEBALL_25;

        case SportNHL:
            return RESOURCE_ID_HOCKEY_25;

        case SportNBA:
            return RESOURCE_ID_BASKETBALL_25;

        default:
            return RESOURCE_ID_STAR_25;
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