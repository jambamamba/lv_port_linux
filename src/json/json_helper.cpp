#include "json_helper.h"

cJSONRAII::cJSONRAII(const std::string &json_str)
: _json(cJSON_Parse(json_str.c_str()))
, _json_str(json_str) {
}

cJSONRAII::~cJSONRAII(){
    if(_json) {
        cJSON_Delete(_json);
    }
}
cJSON *cJSONRAII::operator()(){
    return _json;
}
const std::string &cJSONRAII::str() const {
    return _json_str;
}

