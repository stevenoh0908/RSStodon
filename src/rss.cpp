/*
 * rss.cpp
 * -----
 * Author: Yooshin Oh (stevenoh0908@snu.ac.kr, Dept of Earth & Environmental Sciences, 2021-14725)
 * -----
 * Last Modified: 2026-02-05 18:27:23
 * Modified By: Yooshin Oh (stevenoh0908@snu.ac.kr)
 * -----
 * Description: Implementation file for RSS feed handling.
 */

#include "rss.hpp"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdexcept>
#include <regex>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;

struct MemoryBuffer {
    char *buffer;
    size_t size;
};

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryBuffer *mem = (struct MemoryBuffer *)userp;

    char *ptr = (char*)realloc(mem->buffer, mem->size + realsize + 1);
    if (ptr == NULL) {
        // out of memory
        throw runtime_error("Not enough memory (realloc returned NULL)\n");
    }

    mem->buffer = ptr;
    memcpy(&(mem->buffer[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->buffer[mem->size] = 0;

    return realsize;
}

// extract text only inside ![CDATA[ ... ]]> tags
// make sure to free the returned string after use
string extractCDATA(const char* input) {
    regex regex("<!\\[CDATA\\[([^]]*]]*[^]]*)\\]\\]>");
    smatch matches;
    string s = string(input);
    
    if (regex_search(s, matches, regex)) {
        return matches[1].str();
    } else {
        // if there's no match, return the original string
        // to avoid dangling ref, allocate new one and return
        return s;
    }
}

/**
 * Implement methods of RSSFeedItem
 */
RSSFeedItem::RSSFeedItem() {
    title.clear();
    link.clear();
    guid.clear();
    description.clear();
    pubDate.clear();
    author.clear();
    categories.clear();
    categories.shrink_to_fit();
    return;
}

RSSFeedItem::RSSFeedItem(const xmlNode* itemNode) {
    title.clear();
    link.clear();
    guid.clear();
    description.clear();
    pubDate.clear();
    author.clear();
    categories.clear();
    categories.shrink_to_fit();
    parseFromXMLNode(itemNode);
    return;
}

RSSFeedItem::RSSFeedItem(const RSSFeedItem& other) {
    title = other.title;
    link = other.link;
    guid = other.guid;
    description = other.description;
    pubDate = other.pubDate;
    author = other.author;
    categories = other.categories;
    return;
}

RSSFeedItem::~RSSFeedItem() {
    categories.clear();
    vector<string>().swap(categories); // free vector memory
    return;
}

void RSSFeedItem::parseFromXMLNode(const xmlNode* itemNode) {
    // note that we already checked the itemNode's name is "item" before calling this method.
    char *temp;
    for (xmlNode* node = itemNode->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            if (strcmp((const char*)node->name, "title") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    this->title = extractCDATA(temp);
                    xmlFree(temp);
                }
            } else if (strcmp((const char*)node->name, "link") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    this->link = extractCDATA(temp);
                    xmlFree(temp);
                }
            } else if (strcmp((const char*)node->name, "guid") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    this->guid = extractCDATA(temp);
                    xmlFree(temp);
                }
            } else if (strcmp((const char*)node->name, "description") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    this->description = extractCDATA(temp);
                    xmlFree(temp);
                }
            } else if (strcmp((const char*)node->name, "pubDate") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    this->pubDate = extractCDATA(temp);
                    xmlFree(temp);
                }
            } else if (strcmp((const char*)node->name, "author") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    this->author = extractCDATA(temp);
                    xmlFree(temp);
                }
            } else if (strcmp((const char*)node->name, "creator") == 0) { // in wordpress, author is in dc:creator
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    this->author = extractCDATA(temp);
                    xmlFree(temp);
                }
            } else if (strcmp((const char*)node->name, "category") == 0) {
                temp = (char*)xmlNodeGetContent(node);
                if (temp) {
                    string str = extractCDATA(temp);
                    this->categories.push_back(str);
                    xmlFree(temp);
                }
            }
        }
    }
    return;
}

void RSSFeedItem::print(const string& prefix="") const {
    cout << prefix << "Title: " << (title.empty() ? "N/A" : title) << endl;
    cout << prefix << "Link: " << (link.empty() ? "N/A" : link) << endl;
    cout << prefix << "GUID: " << (guid.empty() ? "N/A" : guid) << endl;
    cout << prefix << "Description: " << (description.empty() ? "N/A" : description) << endl;
    cout << prefix << "Publication Date: " << (pubDate.empty() ? "N/A" : pubDate) << endl;
    cout << prefix << "Author: " << (author.empty() ? "N/A" : author) << endl;
    cout << prefix << "Categories (" << (int)categories.size() << "): ";
    for (int j = 0; j < (int)categories.size(); j++) {
        cout << categories[j] << " ";
    }
    cout << endl;
    return;
}

RSSFeed::RSSFeed() {
    title.clear();
    link.clear();
    description.clear();
    items.clear();
    items.shrink_to_fit();
    return;
}

RSSFeed::RSSFeed(const string& url) {
    title.clear();
    link.clear();
    description.clear();
    items.clear();
    items.shrink_to_fit();
    fetchFromURL(url);
    return;
}

RSSFeed::RSSFeed(const RSSFeed& other) {
    title = other.title;
    link = other.link;
    description = other.description;
    items = other.items;
    return;
}

RSSFeed::~RSSFeed() {
    items.clear();
    vector<RSSFeedItem>().swap(items); // free vector memory
    return;
}

void RSSFeed::fetchFromURL(const string& url) {
    
    struct MemoryBuffer chunk = {0};
    char* temp;

    // read RSS feed using libcurl
    CURL *curl = curl_easy_init();
    if (!curl) {
        throw runtime_error("Failed to initialize CURL\n");
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        free(chunk.buffer);
        throw runtime_error("CURL perform failed");
    }
    curl_easy_cleanup(curl);

    // parse RSS feed using libxml2
    xmlDocPtr doc = xmlReadMemory(chunk.buffer, chunk.size, "parse.xml", NULL, XML_PARSE_NOERROR|XML_PARSE_NOWARNING);
    free(chunk.buffer);
    if (doc == NULL) {
        throw runtime_error("Failed to parse RSS feed");
    }
    xmlNode *root = xmlDocGetRootElement(doc);
    for (xmlNode *channel = root->children; channel; channel = channel->next) {
        if (channel->type == XML_ELEMENT_NODE && strcmp((const char*)channel->name, "channel") == 0) {
            for (xmlNode *node = channel->children; node; node = node->next) {
                if (node->type == XML_ELEMENT_NODE) {
                    if (strcmp((const char*)node->name, "title") == 0) {
                        temp = (char*)xmlNodeGetContent(node);
                        if (temp) {
                            this->title = extractCDATA(temp);
                            xmlFree(temp);
                        }
                    } else if (strcmp((const char*)node->name, "link") == 0) {
                        temp = (char*)xmlNodeGetContent(node);
                        if (temp) {
                            this->link = extractCDATA(temp);
                            xmlFree(temp);
                        }
                    } else if (strcmp((const char*)node->name, "description") == 0) {
                        temp = (char*)xmlNodeGetContent(node);
                        if (temp) {
                            this->description = extractCDATA(temp);
                            xmlFree(temp);
                        }
                    } else if (strcmp((const char*)node->name, "item") == 0) {
                        // parse feed item
                        RSSFeedItem item(node);
                        this->items.push_back(item);
                    }
                }
            }
        }
    }
    xmlFreeDoc(doc);
    return;
}

void RSSFeed::print() const {
    cout << "- Title: " << (title.empty() ? "N/A" : title) << endl;
    cout << "- Link: " << (link.empty() ? "N/A" : link) << endl;
    cout << "- Description: " << (description.empty() ? "N/A" : description) << endl;
    cout << "- Items (" << (int)items.size() << "):" << endl;
    for (int i = 0; i < (int)items.size(); i++) {
        RSSFeedItem item = items.at(i);
        cout << "    Item " << (i + 1) << ":" << endl;
        item.print("    |_ ");
    }
    return;
}