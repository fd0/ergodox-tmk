tmk keyboard firmware for the ErgoDox keyboard, based on https://github.com/cub-uanic/tmk_keyboard

Most significant change a newly implemented low-latency debouncing mechanism
which does debouncing for every key individually.

This imports the [tmk core](https://github.com/tmk/tmk_core) library into the
path tmk/core with a subtree merge.
