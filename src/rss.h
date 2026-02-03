/*
 * rss.h
 * -----
 * Author: Yooshin Oh (stevenoh0908@snu.ac.kr, Dept of Earth & Environmental Sciences, 2021-14725)
 * -----
 * Last Modified: 2026-02-03 23:11:20
 * Modified By: Yooshin Oh (stevenoh0908@snu.ac.kr)
 * -----
 * Description: Header file for RSS feed handling.
 */

#ifndef RSS_H
#define RSS_H
#endif

/**
 * Structure to represent an RSS Feed
 */
typedef struct RSSFeed {
    char* title; // title of the channel
    char* link; // link of the channel
    char* description; // description of the channel
    struct RSSFeedItem* items; // array of feed items
    int itemCount; // number of feed items
} RSSFeed;

/**
 * Structure to represent an RSS Feed Item
 */
typedef struct RSSFeedItem {
    char* title; // title of the article
    char* link; // link to the article
    char* guid; // unique identifier for the article
    char* description; // description or summary of the article
    char* pubDate; // publication date of the article
    char* author; // author of the article
    char** categories; // array of categories/tags
    int categoryCount; // number of categories/tags
} RSSFeedItem;

/**
 * Function to fetch and parse RSS feed from a given URL
 * @param url The URL of the RSS feed
 * @return Pointer to the RSSFeed structure containing parsed data
 */
RSSFeed* fetchAndParseRSS(const char* url);

/**
 * Function to free the memory allocated for an RSSFeed structure
 * @param feed Pointer to the RSSFeed structure to be freed
 */
void freeRSSFeed(RSSFeed* feed);

/**
 * Function to print the RSS feed to console (for debugging purposes)
 * @param feed Pointer to the RSSFeed structure to be printed
 */
void printRSSFeed(const RSSFeed* feed);
