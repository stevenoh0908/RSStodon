/*
 * feed.cpp
 * -----
 * Author: Yooshin Oh (stevenoh0908@snu.ac.kr, Dept of Earth & Environmental Sciences, 2021-14725)
 * -----
 * Last Modified: 2026-02-05 00:08:45
 * Modified By: Yooshin Oh (stevenoh0908@snu.ac.kr)
 * -----
 * Description: Implementation file for RSS feed handling.
 */

#include "feed.hpp"
#include <stack>
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <string>

using namespace std;

/**
 * Get guids from RSS feed ... capture top maxCount items at most from the feed, and put it in the given stack in reverse order (oldest to top)
 * @param rssFeed Pointer to the RSSFeed structure
 * @param stack Output stack to hold the GUIDs
 * @param maxCount Maximum number of GUIDs to hold
 */
void getRSSFeedGUIDs(const RSSFeed* rssFeed, std::stack<std::string>& stack, int maxCount) {
    if (!rssFeed) {
        throw invalid_argument("rssFeed is null");
    }
    int count = 0;
    for (int i = (int)rssFeed->items.size() - 1; i >= 0 && count < maxCount; i--) {
        stack.push(rssFeed->items.at(i).guid);
        count++;
    }
    return;
}

/**
 * Extract newly added RSS feed items compared to the given set of existing GUIDs (assuming the given set of GUIDs is read from previous fetch)
 * @param rssFeed Pointer to the RSSFeed structure
 * @param existingGUIDs Set of existing GUIDs to compare against
 * @param newItems Output vector to hold the newly added RSSFeedItem structures
 */
void extractNewRSSFeedItems(const RSSFeed* rssFeed, const stack<string>& existingGUIDs, vector<RSSFeedItem>& newItems) {
    if (!rssFeed) {
        throw invalid_argument("rssFeed is null");
    }
    // create a set of existing GUIDs for quick lookup
    unordered_set<string> existingSet;
    stack<string> tempStack = existingGUIDs; // make a copy to pop from
    while (!tempStack.empty()) {
        existingSet.insert(tempStack.top());
        tempStack.pop();
    }

    // iterate through the feed items and check if their GUIDs are in the existing set
    for (const auto& item : rssFeed->items) {
        if (existingSet.find(item.guid) == existingSet.end()) {
            // not found in existing set, this is a new item
            newItems.push_back(item);
        }
    }
    return;
}