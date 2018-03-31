#!/usr/bin/env python2.7

# TODO: support PNG
# TODO: port to python3.5 (require PIL install)
# TODO: recursive dir pscan

# [2015/04/21] support only JPEG


import os
import sys
from glob import glob
from PIL import Image


def main():
    for path in glob("*.png"):
        try:
            img = Image.open(path)
        except IOError:
            pass
        else:
            w, h = img.size
            if w == h == 64:
                img.resize((24, 24), Image.ANTIALIAS).save(path, "PNG", quality=95)
            print(path, w, h)


if __name__ == '__main__':
    main()
