uARToolkit - a bare metal ARToolKit
-----------------------------------


## Introduction
uARToolkit is a lightweight derivate of the original ARToolkit
by Hirokazu Kato and Mark Billinghurst. It reduces dependencies
on APIs to a very minimum, or rather zero. Furthermore, this version
is able to change video formats at runtime. uARToolkit is
distributed under the terms of the GPL2 and was engineered by Hartmut Seichter.

## Features

* pure ANSI C API
* video format agnostic
* graphics library agnostic
* not depending on video capturing facilities
* not depending on graphics library

## Goals

This derivate version was born out of necessity to
provide a high performance and lightweight alternative
to the original AR Toolkit. Most developers do not use stereo,
drawing code and video capture facilities within the
original AR Toolkit.

* all (or critical) code reentrant
* remove all static storage
* vision code configurable at runtime

## Drawbacks

If your are using this version of AR Toolkit you should be aware
of the drawbacks to fully understand what uARToolkit targets:

* there is no video capture facility - this is out of scope for uARToolkit
* there is no drawing code, however this version can generate matrices for OpenGL and DirectX
* you are dealing with a 5% performance hit due to the configurability of uARToolkit on a frame to frame basis
* no explicit stereo mode is provided. You need to use two arToolkit handles instead
* due to above constraints it is not API compatible to ARToolkit 2.7 or later

## Authors

* Hirokazu Kato
* Mark Billinghurst
* Thomas Pintaric
* Hartmut Seichter


## Questions

Please contact Hartmut Seichter http://www.technotecture.com if you have any questions.
