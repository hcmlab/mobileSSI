![Logo](/logo/mobileSSI.png)
# mobileSSI
mobileSSI is a port of the [Social Signal Interpretation (SSI) framework](http://openssi.net/) to Linux and Android devices. Like SSI, mobileSSI is written in C++. Windows-platform dependend code was replaced by crossplatform compatible code like C++11 standard threading.

## Features
* same features as the SSI framework (Windows platform dependend plugins can't be included)
* Android / Android-wear support:
  * background service (allowing to run even during standby)
  * plugins to access Android sensors / audio device
  * uses native code (ndk)
  * can be used in combination with [ssj](https://github.com/hcmlab/ssj) pipelines for example to access additional sensors and to  use additional Java code

## Documentation
Please see our Github Wiki to get started with mobileSSI.

## About
The mobileSSI framework is beeing developed at the [Lab for Human Centered Multimedia](http://www.hcm-lab.de/) of the University of Augsburg.

## License
Core libraries are released under LGPL (see LICENSE in core folder).
By default plugins are released under GPL, but individual licences may apply (see LICENSE file in plugin folder).
For 3rd party dependencies see the README file and additional license files.
