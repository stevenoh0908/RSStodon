/*
 * feed.hpp
 * -----
 * Author: Yooshin Oh (stevenoh0908@snu.ac.kr, Dept of Earth & Environmental Sciences, 2021-14725)
 * -----
 * Last Modified: 2026-02-05 00:04:09
 * Modified By: Yooshin Oh (stevenoh0908@snu.ac.kr)
 * -----
 * Description: Header file for RSS feed handling.
 */

#ifndef FEED_HPP
#define FEED_HPP

#include "rss.hpp"
#include <stack>

/**
 * Get guids from RSS feed ... capture top maxCount items at most from the feed, and put it in the given stack in reverse order (oldest to top)
 * @param rssFeed Pointer to the RSSFeed structure
 * @param stack Output stack to hold the GUIDs
 * @param maxCount Maximum number of GUIDs to hold
 */
void getRSSFeedGUIDs(const RSSFeed* rssFeed, stack<string>& stack, int maxCount);

/**
 * Extract newly added RSS feed items compared to the given set of existing GUIDs (assuming the given set of GUIDs is read from previous fetch)
 * @param rssFeed Pointer to the RSSFeed structure
 * @param existingGUIDs Set of existing GUIDs to compare against
 * @param newItems Output vector to hold the newly added RSSFeedItem structures
 */
void extractNewRSSFeedItems(const RSSFeed* rssFeed, const stack<string>& existingGUIDs, vector<RSSFeedItem>& newItems);

#endif

