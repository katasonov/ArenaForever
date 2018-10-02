#ifndef _JSON_2609181119_
#define _JSON_2609181119_

#include <string>
#include <vector>

using namespace std;

int JsonValueString(const string &key, const string &json, string &val);
int JsonValueInt(const string &key, const string &json, intmax_t &val);
int JsonObjectsArray(const string &json, vector<string> &objects);


#endif
