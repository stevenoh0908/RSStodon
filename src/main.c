#include <stdio.h>
#include <string.h>
#include "rss.h"

int main() {
    // test out rss.h and rss.c
    char url[128];
    printf("Enter RSS feed URL: ");
    fgets(url, sizeof(url), stdin);
    // remove trailing newline character
    size_t len = strlen(url);
    if (len > 0 && url[len - 1] == '\n') {
        url[len - 1] = '\0';
    }

    RSSFeed* feed = fetchAndParseRSS(url);
    if (feed) {
        printRSSFeed(feed);
        freeRSSFeed(feed);
    } else {
        fprintf(stderr, "Failed to fetch or parse RSS feed from %s\n", url);
    }

    return 0;
}