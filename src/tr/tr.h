#pragma once

#include <string>

std::string tr(const std::string &txt);

namespace Translation {

std::vector<std::string> availableLanguages();
std::string getCurrentLanguage();
bool setCurrentLanguage(const std::string &language);
std::string getDefaultLanguage();
bool setDefaultLanguage(const std::string &language);

}//Translation