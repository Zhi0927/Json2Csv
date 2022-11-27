#include "ConvertMethods.h"

bool jsonToCsv(std::shared_ptr<Json::Value> jsonInput, const char* input, const char* output) {

    bool parsingSuccessful = true;
    Json::Reader reader;
    std::ifstream ifs(input);

    if (ifs.is_open()) {
        std::istream& s = ifs;
        parsingSuccessful = reader.parse(s, *jsonInput);
        if (!parsingSuccessful) {
            std::cout << "Error parsing file => " << input << "\n";
        }
    }
    else {
        std::cout << "File not found => " << input << "\n";
        parsingSuccessful = false;
    }
    ifs.close();
    return parsingSuccessful;
}

std::string joinVector(std::vector<std::string>& v, const char* delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i != 0)
            ss << delimiter;
        ss << v[i];
    }
    return ss.str();
}

void toKeyValuePairs(std::shared_ptr<std::vector<std::pair<std::string, std::string>>>& builder, Json::Value& source, std::vector<std::string>& ancestors, const char* delimiter) {

    if (source.isObject()) {
        for (Json::Value::iterator it = source.begin(); it != source.end(); it++) {
            Json::Value key = it.key();
            Json::Value val = (*it);
            std::vector<std::string> objKeys;
            objKeys.insert(objKeys.end(), ancestors.begin(), ancestors.end());
            objKeys.push_back(key.asString());
            toKeyValuePairs(builder, val, objKeys, "/");
        }
    }
    else if (source.isArray()) {
        int count = 0;
        std::for_each(source.begin(), source.end(), [&builder, &count, &ancestors](Json::Value& val) {
            std::stringstream ss;
            ss << count;
            std::vector<std::string> arrKeys;
            arrKeys.insert(arrKeys.end(), ancestors.begin(), ancestors.end());
            arrKeys.push_back(ss.str());
            toKeyValuePairs(builder, val, arrKeys, "/");
            count++;
            });
    }
    else {
        std::string key = joinVector(ancestors, delimiter);
        auto tpl = std::make_pair(key, source.asString());
        builder->push_back(tpl);
    }

}

objects_t jsonToDicts(std::shared_ptr<Json::Value> jsonInput) {

    //convert json into array if not already
    if (!jsonInput->isArray()) {
        Json::Value jv;
        jv.append(std::move(*jsonInput));
        *jsonInput = jv;
    }

    auto objects = std::make_shared<std::vector<std::shared_ptr<std::vector<std::pair<std::string, std::string>>>>>();

    std::for_each(jsonInput->begin(), jsonInput->end(), [&objects](Json::Value& d) {
        auto builder = std::make_shared<std::vector<std::pair<std::string, std::string>>>();
        objects->push_back(builder);
        auto o = objects->back();
        std::vector<std::string> keys;
        toKeyValuePairs(o, d, keys, "/");
        });

    return objects;
}

std::shared_ptr<lines_t> dictsToCsv(objects_t o) {

    std::set<std::string> k;
    auto lines = std::make_shared<lines_t>();
    std::regex newline("\\r|\\n");
    std::regex quote("\"");

    auto buildKeys = [&k](object_t& e) {
        for (auto& g : *e) {
            k.insert(g.first);
        }
    };

    auto buildHeaders = [&k, &lines]() {
        auto l = std::make_shared<line_t>();
        for (auto& h : k) {
            l.get()->push_back(std::make_unique<std::string>(h));
        }
        lines.get()->push_back(l);
    };

    auto buildRows = [&k, &lines, &newline, &quote](object_t& e) {
        auto l = std::make_shared<line_t>();
        for (auto& h : k) {
            auto it = std::find_if(e.get()->begin(), e.get()->end(), [&h](const std::pair<std::string, std::string>& p)->bool {
                return p.first == h;
                });

            if (it != e.get()->end()) {
                std::string n = std::regex_replace((it)->second, newline, "\\\\n");
                n = std::regex_replace(n, quote, "\"\"");
                l.get()->push_back(std::make_unique<std::string>("\"" + n + "\""));
                e.get()->erase(it);
            }
            else {
                l.get()->push_back(std::make_unique<std::string>());
            }
        }
        lines.get()->push_back(l);

    };

    std::for_each(o->begin(), o->end(), buildKeys);
    buildHeaders();
    std::for_each(o->begin(), o->end(), buildRows);

    return lines;
}