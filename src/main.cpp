#include <iostream>
#include <string>
#include <stack>
#include "rss.hpp"
#include "feed.hpp"

#define MAX_FETCH_GUIDS 128
using namespace std;

int main() {
    // test out newly-created feed scripts
    string url;
    cout << "Enter RSS feed URL: ";
    getline(cin, url);
    RSSFeed* feed = new RSSFeed(url);
    // let us say we read all feeds except for top 5 in previous fetch
    // to implement this, hereby we get guids and remove bottom 5 from the stack
    stack<string> existingGUIDs, reverseStack;
    getRSSFeedGUIDs(feed, existingGUIDs, MAX_FETCH_GUIDS);
    int totalGUIDs = (int)existingGUIDs.size();
    while (!existingGUIDs.empty()) {
        reverseStack.push(existingGUIDs.top());
        existingGUIDs.pop();
    } // now reverseStack has all guids in order (oldest to newest)
    // put back all but last 5 into existingGUIDs
    for (int i = 0; i < totalGUIDs - 5; i++) {
        existingGUIDs.push(reverseStack.top());
        reverseStack.pop();
    }
    // now existingGUIDs has all but top 5 guids from previous fetch
    // let us print out newly added items compared to existingGUIDs (actually, top 5 items in feed)
    vector<RSSFeedItem> newItems;
    extractNewRSSFeedItems(feed, existingGUIDs, newItems);
    cout << "New Articles from < " << feed->title << ">" << endl;
    cout << "----------------------------------------" << endl;
    vector<RSSFeedItem>::iterator it;
    int i;
    for (it = newItems.begin(), i = 0; it != newItems.end(); ++it, i++) {
        cout << "- item " << i << endl;
        (*it).print("    |_ ");
    }

    return 0;
}