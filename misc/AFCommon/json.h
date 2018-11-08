#ifndef _JSON_2609181119_
#define _JSON_2609181119_

#include <string>
#include <vector>

using namespace std;

int JsonValue(const string &key, const string &json, string &valJson);
string JsonValueString(const string &key, const string &json);
int JsonValueInt(const string &key, const string &json);
int JsonObjectsArray(const string &json, vector<string> &objects);


#endif
