/*
 * rss.c
 * -----
 * Author: Yooshin Oh (stevenoh0908@snu.ac.kr, Dept of Earth & Environmental Sciences, 2021-14725)
 * -----
 * Last Modified: 2026-02-03 23:20:58
 * Modified By: Yooshin Oh (stevenoh0908@snu.ac.kr)
 * -----
 * Description: Implementation file for RSS feed handling.
 */

#include "rss.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

struct MemoryBuffer {
    char *buffer;
    size_t size;
};

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryBuffer *mem = (struct MemoryBuffer *)userp;

    char *ptr = realloc(mem->buffer, mem->size + realsize + 1);
    if (ptr == NULL) {
        // out of memory
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->buffer = ptr;
    memcpy(&(mem->buffer[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->buffer[mem->size] = 0;

    return realsize;
}

// extract text only inside ![CDATA[ ... ]]> tags
// make sure to free the returned string after use
char* extractCDATA(const char* input) {
    regex_t regex;
    regmatch_t matches[2];
    const char* pattern = "<!\\[CDATA\\[([^]]*]]*[^]]*)\\]\\]>";

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        fprintf(stderr, "Could not compile regex\n");
        return NULL;
    }
    if (regexec(&regex, input, 2, matches, 0) == 0) {
        int start = matches[1].rm_so;
        int end = matches[1].rm_eo;
        int length = end - start;
        char* result = malloc(length + 1);
        if (result) {
            strncpy(result, input + start, length);
            result[length] = '\0';
        }
        regfree(&regex);
        return result;
    } else { // if there's no match, return a copy of the original string
        char* result = strdup(input);
        regfree(&regex);
        return result;
    }
}

// parse RSS feed item from an xmlNode
// returns non-zero on failure
int parseRSSFeedItem(xmlNode* itemNode, RSSFeedItem* feedItem) {
    // note that we already checked the itemNode's name is "item" before calling this method.
    char *temp;
    for (xmlNode* node = itemNode->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            if (strcmp((const char*)node->name, "title") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                feedItem->title = extractCDATA(temp);
                free(temp);
            } else if (strcmp((const char*)node->name, "link") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                feedItem->link = extractCDATA(temp);
                free(temp);
            } else if (strcmp((const char*)node->name, "guid") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                feedItem->guid = extractCDATA(temp);
                free(temp);
            } else if (strcmp((const char*)node->name, "description") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                feedItem->description = extractCDATA(temp);
                free(temp);
            } else if (strcmp((const char*)node->name, "pubDate") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                feedItem->pubDate = extractCDATA(temp);
                free(temp);
            } else if (strcmp((const char*)node->name, "author") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                feedItem->author = extractCDATA(temp);
                free(temp);
            } else if (strcmp((const char*)node->name, "creator") == 0) { // in wordpress, author is in dc:creator
                temp = (char*)xmlNodeGetContent(node);
                feedItem->author = extractCDATA(temp);
                free(temp);
            } else if (strcmp((const char*)node->name, "category") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                feedItem->categories = realloc(feedItem->categories, sizeof(char*) * (feedItem->categoryCount + 1));
                feedItem->categories[feedItem->categoryCount] = extractCDATA(temp);
                feedItem->categoryCount++;
                free(temp);
            }
        }
    }
    return 0;
}

/**
 * Function to fetch and parse RSS feed from a given URL
 * @param url The URL of the RSS feed
 * @return Pointer to the RSSFeed structure containing parsed data
 */
RSSFeed* fetchAndParseRSS(const char* url) {
    
    struct MemoryBuffer chunk = {0};
    char* temp;

    // read RSS feed using libcurl
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return NULL;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "CURL perform failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(chunk.buffer);
        return NULL;
    }
    curl_easy_cleanup(curl);

    // parse RSS feed using libxml2
    xmlDocPtr doc = xmlReadMemory(chunk.buffer, chunk.size, "parse.xml", NULL, XML_PARSE_NOERROR|XML_PARSE_NOWARNING);
    free(chunk.buffer);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse RSS feed\n");
        return NULL;
    }
    RSSFeed* feed = malloc(sizeof(RSSFeed));
    memset(feed, 0, sizeof(RSSFeed));
    xmlNode *root = xmlDocGetRootElement(doc);
    for (xmlNode *channel = root->children; channel; channel = channel->next) {
        if (channel->type == XML_ELEMENT_NODE && strcmp((const char*)channel->name, "channel") == 0) {
            for (xmlNode *node = channel->children; node; node = node->next) {
                if (node->type == XML_ELEMENT_NODE) {
                    if (strcmp((const char*)node->name, "title") == 0) {
                        temp = (char*)xmlNodeGetContent(node);
                        feed->title = extractCDATA(temp);
                        free(temp);
                    } else if (strcmp((const char*)node->name, "link") == 0) {
                        temp = (char*)xmlNodeGetContent(node);
                        feed->link = extractCDATA(temp);
                        free(temp);
                    } else if (strcmp((const char*)node->name, "description") == 0) {
                        temp = (char*)xmlNodeGetContent(node);
                        feed->description = extractCDATA(temp);
                        free(temp);
                    } else if (strcmp((const char*)node->name, "item") == 0) {
                        // parse feed item
                        RSSFeedItem item;
                        memset(&item, 0, sizeof(RSSFeedItem));
                        if (parseRSSFeedItem(node, &item) == 0) {
                            feed->items = realloc(feed->items, sizeof(RSSFeedItem) * (feed->itemCount + 1));
                            feed->items[feed->itemCount] = item;
                            feed->itemCount++;
                        }
                    }
                }
            }
        }
    }

    xmlFreeDoc(doc);
    return feed;
}

/**
 * Function to free the memory allocated for an RSSFeed structure
 * @param feed Pointer to the RSSFeed structure to be freed
 */
void freeRSSFeed(RSSFeed* feed) {
    if (!feed) return;
    // free all items first
    for (int i = 0; i < feed->itemCount; i++) {
        RSSFeedItem* item = &feed->items[i];
        free(item->title);
        free(item->link);
        free(item->guid);
        free(item->description);
        free(item->pubDate);
        free(item->author);
        for (int j = 0; j < item->categoryCount; j++) {
            free(item->categories[j]);
        }
        free(item->categories);
    }
    free(feed->items);
    free(feed->title);
    free(feed->link);
    free(feed->description);
    free(feed);
    return;
}

/**
 * Function to print the RSS feed to console (for debugging purposes)
 * @param feed Pointer to the RSSFeed structure to be printed
 */
void printRSSFeed(const RSSFeed* feed) {
    if (!feed) return;
    printf("- Title: %s\n", feed->title ? feed->title : "N/A");
    printf("- Link: %s\n", feed->link ? feed->link : "N/A");
    printf("- Description: %s\n", feed->description ? feed->description : "N/A");
    printf("- Items (%d):\n", feed->itemCount);
    for (int i = 0; i < feed->itemCount; i++) {
        RSSFeedItem* item = &feed->items[i];
        printf("    Item %d:\n", i + 1);
        printf("    |_ Title: %s\n", item->title ? item->title : "N/A");
        printf("    |_ Link: %s\n", item->link ? item->link : "N/A");
        printf("    |_ GUID: %s\n", item->guid ? item->guid : "N/A");
        printf("    |_ Description: %s\n", item->description ? item->description : "N/A");
        printf("    |_ Publication Date: %s\n", item->pubDate ? item->pubDate : "N/A");
        printf("    |_ Author: %s\n", item->author ? item->author : "N/A");
        printf("    |_ Categories (%d): ", item->categoryCount);
        for (int j = 0; j < item->categoryCount; j++) {
            printf("%s ", item->categories[j]);
        }
        printf("\n");
    }
    return;
}