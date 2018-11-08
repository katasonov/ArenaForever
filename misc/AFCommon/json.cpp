#include "json.h"

string JsonValueString(const string &key, const string &json)
{
	size_t pos = json.find(key, 0);

	if (pos == string::npos)
		throw std::exception("JsonValueString: failed");

	pos = json.find_first_of(':', pos);

	if (pos == string::npos)
		throw std::exception("JsonValueString: failed");

	pos = json.find_first_of('"', pos);
	if (pos == string::npos)
	{
		throw std::exception("JsonValueString: failed");
	}
	pos++;
	string ret;
	while (pos < json.size() && json[pos] != '"') 
	{
		ret += json[pos];
		pos++;
	}

	return ret;
}

int JsonValueInt(const string &key, const string &json)
{
	size_t pos = json.find(key, 0);

	if (pos == string::npos)
		return -1;

	pos = json.find_first_of(':', pos);

	if (pos == string::npos)
		throw std::exception("JsonValueInt: failed to parse int value");

	//Trim spaces
	pos++;
	while (json[pos] == ' ' && pos < json.size())
		pos++;

	if (!(pos < json.size()))
		throw std::exception("JsonValueInt: failed to parse int value");

	int val = 0;
	while (true)
	{
		if (json[pos] < '0' || json[pos] > '9')
			break;
		val *= 10;
		val += json[pos] - '0';
		pos++;
	}

	return val;
}


int ReadNextJsonObject(const char *json, string &obj)
{
	int openCounter = 0;
	int closeCounter = 0;

	char ch;
	int offset = 0;
	do
	{
		if (*json == '\0')
			break;
		if (openCounter == closeCounter && (openCounter > 0))
			break;
		if (openCounter > 0) 
		{
			obj += *json;
		}
		if (*json == '{')
		{
			obj += *json;
			openCounter++;
		}
		if (*json == '}')
			closeCounter++;
		json++;
		offset++;
	} while (true);

	return offset;
}

int JsonObjectsArray(const string &json, vector<string> &objects)
{
	string obj;
	int offset = 0;

	do 
	{
		obj = "";
		offset += ReadNextJsonObject(json.c_str() + offset, obj);
		if (obj.size() > 0)
		{
			objects.push_back(obj);
		}
	} while (obj.size() > 0 && offset < json.size());


	return 0;
}

//int JsonValue(const string &key, const string &json, string &valJson)
//{
//	size_t pos = json.find(key, 0);
//
//	if (pos == string::npos)
//		return -1;
//
//	pos = json.find_first_of(':', pos);
//
//	if (pos == string::npos)
//		return -1;
//
//	while (pos < json.size())
//	{
//		if (comma - 1 == opened)
//	}
//
//
//	return 0;
//}