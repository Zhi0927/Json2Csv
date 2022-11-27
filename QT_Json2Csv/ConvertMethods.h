#ifndef ConvertMethods_H_
#define ConvertMethods_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <sstream>
#include <memory>
#include <regex>
#include "json.h"

typedef std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::pair<std::string, std::string>>>>> objects_t;
typedef std::shared_ptr<std::vector<std::pair<std::string, std::string>>>								object_t;
typedef std::vector<std::unique_ptr<std::string>>														line_t;
typedef std::vector<std::shared_ptr<std::vector<std::unique_ptr<std::string>>>>							lines_t;

bool jsonToCsv(std::shared_ptr<Json::Value> jsonInput, const char* input, const char* output);
std::string joinVector(std::vector<std::string>& v, const char* delimiter);
void toKeyValuePairs(std::shared_ptr<std::vector<std::pair<std::string, std::string>>>& builder, Json::Value& source, std::vector<std::string>& ancestors, const char* delimiter);
objects_t jsonToDicts(std::shared_ptr<Json::Value> jsonInput);
std::shared_ptr<lines_t> dictsToCsv(objects_t o);

#endif 

