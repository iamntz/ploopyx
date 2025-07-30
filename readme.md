ln -s  ~/custom-keyboards/ploopyx ~/vial-qmk/keyboards/ntz/ploopyx

git submodule add https://github.com/drashna/qmk_modules.git ~/vial-qmk/modules/drashna

qmk compile -kb ntz/ploopyx -km vial 

