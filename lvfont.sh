#!/bin/bash
set -xe

if [ ! -d lv_font_conv ]; then
git clone https://github.com/lvgl/lv_font_conv.git
fi

cd lv_font_conv/
#sudo apt-get install -y npm
#npm i lv_font_conv -g

words=$(fc-query --format='%{charset}\n' /usr/local/src/libgd-gd-2.3.3/tests/freetype/DejaVuSans.ttf)
ranges=""
for word in "${words[@]}"; do ranges="$ranges --range 0x$(echo $word)"; done
lv_font_conv --font /usr/local/src/libgd-gd-2.3.3/tests/freetype/DejaVuSans.ttf --size 16 $ranges --format bin --bpp 3 --no-compress -o /tmp/output.fnt

  
#   lv_font_t *font_1_bin = lv_binfont_create(/tmp/output.fnt");
#   lv_binfont_destroy(font_1_bin);

#lv_font_conv \
#--font /usr/local/src/libgd-gd-2.3.3/tests/freetype/DejaVuSans.ttf \
#--size 16 \
#--range 0x20-0xFFFF \
#--format bin \
#--bpp 3 \
#--no-compress \
#-o output.font
