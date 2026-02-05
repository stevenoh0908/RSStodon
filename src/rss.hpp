/*
 * rss.hpp
 * -----
 * Author: Yooshin Oh (stevenoh0908@snu.ac.kr, Dept of Earth & Environmental Sciences, 2021-14725)
 * -----
 * Last Modified: 2026-02-05 18:30:02
 * Modified By: Yooshin Oh (stevenoh0908@snu.ac.kr)
 * -----
 * Description: Header file for RSS feed handling.
 */

#ifndef RSS_HPP
#define RSS_HPP

#include <vector>
#include <string>
#include <libxml/parser.h>
using namespace std;

class RSSFeedItem {
public:
    string title; // title of the article
    string link; // link to the article
    string guid; // unique identifier for the article
    string description; // description or summary of the article
    string pubDate; // publication date of the article
    string author; // author of the article
    vector<string> categories; // array of categories/tags
    RSSFeedItem();
    RSSFeedItem(const xmlNode* itemNode);
    RSSFeedItem(const RSSFeedItem& other);
    ~RSSFeedItem();
    void parseFromXMLNode(const xmlNode* itemNode);
    void print(const string& prefix) const;
};

class RSSFeed {
public:
    string title; // title of the channel
    string link; // link of the channel
    string description; // description of the channel
    vector<RSSFeedItem> items; // array of feed items
    RSSFeed();
    RSSFeed(const string& url);
    RSSFeed(const RSSFeed& other);
    ~RSSFeed();
    void fetchFromURL(const string& url);
    void print() const;
};

#endif