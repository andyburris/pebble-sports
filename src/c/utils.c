#include <pebble.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *concatfree(const char *str1, const char *str2, bool *free1, bool *free2) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "concating strings");

    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int totalLen = len1 + len2;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "concating strings with length %d", totalLen);

    char *str = malloc(totalLen + 1);

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "before concat = %s", str);

    strcpy(str, str1);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "after copy = %s", str);

    strncat(str, str2, len2);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "concatenated strings = %s", "test");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "concatenated strings = %s", str);

    return str;
}

char *concat(const char *str1, const char *str2) {
    return concatfree(str1, str2, false, false);
}