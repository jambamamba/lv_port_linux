#!/bin/bash
set -xe

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


path_to_ttc_file="$1"
if [ -z "${path_to_ttc_file}" ]; then echo "syntax: ./ttc_to_ttf.sh path/to/ttc/file"; exit -1; fi

sudo apt install -y fontforge
if [ ! -f "$(which fontforge)" ]; then echo "error: font forge is not installed"; exit -1; fi

fontforge -script "${script_dir}/ttc_to_ttf.pe" ${path_to_ttc_file}
