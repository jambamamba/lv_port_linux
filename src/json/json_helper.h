#pragma once

#include <cJSON/cJSON.h>
#include <string>

struct cJSONRAII {
    cJSONRAII(const std::string &json_str);
    ~cJSONRAII();
    cJSON *operator()();
    const std::string &str() const;
    protected:
    cJSON *_json;
    const std::string &_json_str;
};
