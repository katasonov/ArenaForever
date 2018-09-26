#include "json.h"

int JsonValueString(const string &key, const string &json, string &val)
{
	size_t pos = json.find(key, 0);

	if (pos == string::npos)
		return -1;

	pos = json.find_first_of(':', pos);

	if (pos == string::npos)
		return -1;

	pos = json.find_first_of('"', pos);
	if (pos == string::npos)
	{
		return -1;
	}
	pos++;
	while (pos < json.size() && json[pos] != '"') 
	{
		val += json[pos];
		pos++;
	}

	return 0;
}

int JsonValueInt(const string &key, const string &json, int &val)
{
	size_t pos = json.find(key, 0);

	if (pos == string::npos)
		return -1;

	pos = json.find_first_of(':', pos);

	if (pos == string::npos)
		return -1;

	//Trim spaces
	pos++;
	while (json[pos] == ' ' && pos < json.size())
		pos++;

	if (!(pos < json.size()))
		return -1;

	val = 0;
	while (true)
	{
		if (json[pos] < '0' || json[pos] > '9')
			break;
		val *= 10;
		val += json[pos] - '0';
		pos++;
	}

	return 0;
}