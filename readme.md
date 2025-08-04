## What's this?

[Ploopy's default](https://github.com/vial-kb/vial-qmk/tree/vial/keyboards/ploopyco) config are OK, but could be better. Some tweaks/changes:

1. Add the ability to have both `DRAG_SCROLL` _and_ `TOGGLE_DRAG_SCROLL`
2. Add the `SCROLL_OR_CLICK3` code, which will act like a tap dance, but without the `TAPPING_TERM` delay:
    - if you click, it's click 3
    - if you hold, it enables scroll _and_ switch to layer 2
    - adds `VOLUME_ON_SCROLL` code, which will allow you to control volume by moving the trackball

### Missing functionality?

Ploopy's repo includes common code used in all of their products. I've cleaned up the stuff that I've considered to not be relevant, so please note that it **removes** some of the default functionality that are not useful on Adept hardware (e.g. encoders). So if you rely on them, please pick whatever you'd like from this repo and implement it.

## Installation
I keep all my qmk/vial configs in `~/custom-keyboards`, so I have to create a link:

```
ln -s  ~/custom-keyboards/ploopyx ~/vial-qmk/keyboards/ntz/ploopyx
```

Since we're using some modules, install them.

```
git submodule add https://github.com/drashna/qmk_modules.git ~/vial-qmk/modules/drashna 
```

Git submodule will make the qmk / vial repo dirty, so it will be trickier to update in the future. If you don't want to deal with that, just clone it:

```
git clone https://github.com/drashna/qmk_modules.git ~/vial-qmk/modules/drashna
```
_(this means that you'll have to deal with manually update the repo)_

Compile it with

```
qmk compile -kb ntz/ploopyx -km vial
```

### Post Installation

After flashing, you should also load `ploopy.vil` in your vial app, because it will add a couple of extra combos:
- `KC_BTN3` & `SCROLL_OR_CLICK3` -> `KC_BTN3`
- `KC_BTN4` & `KC_BTN5` -> media play/pause
- `KC_BTN1` & `KC_BTN2` -> trackball will change volume 
- `KC_BTN1` & `KC_BTN4` -> win+tab
