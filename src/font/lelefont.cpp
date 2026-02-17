#include "lelefont.h"

#include <debug_logger/debug_logger.h>
#include <filesystem>
#include <regex>

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {

void updateFontDb(std::unordered_map<std::string, std::unordered_map<int, LeleFont::Font>> &font_db) {

  std::string current_dir = std::filesystem::current_path().string() + "/fonts";
  for (const auto& entry : std::filesystem::directory_iterator(current_dir)) {
      const std::string LVF_EXT = ".lvf";
      if (std::filesystem::is_regular_file(entry.status()) &&
          entry.path().extension().string() == LVF_EXT) {
            std::regex rgx("(.*)\\.([0-9]*)pt\\.lvf");
            std::smatch matches;
            std::string file_name(std::filesystem::path(entry.path().string()).filename().string());
            std::regex_search(file_name, matches, rgx);
            if(matches.size() > 2) {
              std::string font_name = matches[1].str();
              int pt_size = std::stoi(matches[2].str());

              for (unsigned char* c = (unsigned char*)font_name.data(); *c; ++c) {
                    *c = std::tolower(*c);
              }
              font_db[font_name].insert({pt_size, LeleFont::Font(entry.path().string())});
            }
      }
  }
}
}//namespace

LeleFont::Font::Font(const std::string &lvf_file)
: _lvf_file(lvf_file) {}

LeleFont::Font::~Font() {
  if(_lv_font) {
    lv_binfont_destroy(_lv_font);
  }
}

LeleFont &LeleFont::getLeleFont() {
  static LeleFont _lele_font;
  return _lele_font;
}

const std::unordered_map<std::string, std::unordered_map<int, LeleFont::Font>> &LeleFont::getFontDb() const {
  return _font_db;
}

const lv_font_t *LeleFont::getFont(const std::string &family_, int size) const {
  std::string family(family_);
  for (unsigned char* c = (unsigned char*)family.data(); *c; ++c) {
        *c = std::tolower(*c);
  }
  // std::transform(family_.begin(), family_.end(), family.begin(),
  //   [](unsigned char c){ return std::tolower(c); });
    
  if(family == "montserrat") {
    if(size <= 12) {
      return &lv_font_montserrat_12;
    }
    else if(size <= 14) {
      return &lv_font_montserrat_14;
    }
    else if(size <= 16) {
      return &lv_font_montserrat_16;
    }
    else if(size <= 18) {
      return &lv_font_montserrat_18;
    }
    else if(size <= 20) {
      return &lv_font_montserrat_20;
    }
    else if(size <= 22) {
      return &lv_font_montserrat_22;
    }
    else if(size <= 24) {
      return &lv_font_montserrat_24;
    }
    else if(size <= 26) {
      return &lv_font_montserrat_26;
    }
    else if(size <= 28) {
      return &lv_font_montserrat_28;
    }
    else if(size <= 30) {
      return &lv_font_montserrat_30;
    }
    else if(size <= 32) {
      return &lv_font_montserrat_32;
    }
    else if(size <= 34) {
      return &lv_font_montserrat_34;
    }
    else if(size <= 36) {
      return &lv_font_montserrat_36;
    }
    else if(size <= 38) {
      return &lv_font_montserrat_38;
    }
    else if(size <= 40) {
      return &lv_font_montserrat_40;
    }
    else if(size <= 42) {
      return &lv_font_montserrat_42;
    }
    else if(size <= 44) {
      return &lv_font_montserrat_44;
    }
    else if(size <= 46) {
      return &lv_font_montserrat_46;
    }
    else if(size <= 48) {
      return &lv_font_montserrat_48;
    }
  }
  else if(family == "unscii") {
    if(size <= 8) {
      return &lv_font_unscii_8;
    }
  }
  else if(family == "dejavu") {
    if(size <= 16) {
      return &lv_font_dejavu_16_persian_hebrew;
    }
  }
  else {
    //run command: fc-list :family| grep -i ubuntu # to see paths of font files
    //Sans vs. Serif: 
    // Use Noto Sans for digital screens and UI (neutral, modern), 
    // Use Noto Serif for printed materials or longer reading (better readability).
    {
      const auto &it_family = _font_db.find(family);
      if(it_family == _font_db.end()) {
        updateFontDb(_font_db);
      }
    }
    const auto &it_family = _font_db.find(family);
    if(it_family != _font_db.end()) {
      const auto &it_pt = it_family->second.find(size);
      if(it_pt != it_family->second.end()) {
        Font &font = it_pt->second;
        if(font._lv_font) {
          return font._lv_font;
        }
        font._lv_font = lv_binfont_create(font._lvf_file.c_str());
        return font._lv_font;
      }
    }
  }
  LL(WARNING, LVSIM) << "Failed to find font '" << family << "' of size " << size << ". Defaulting to default";
  return &lv_font_dejavu_16_persian_hebrew; //default
}