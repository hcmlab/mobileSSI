![Logo](/logo/mobileSSI.png)
# mobileSSI
mobileSSI is a port of the [Social Signal Interpretation (SSI) framework](http://openssi.net/) to Linux and Android devices as well as macOS. Like SSI, mobileSSI is written in C++. Windows-platform dependend code was replaced by crossplatform compatible code like C++11 standard threading.

## Features
* same features as the SSI framework (Windows platform dependend plugins are not included)
* most plugins can be used on Android and Linux without changes
* crossplatform C / C++ libraries can be easily included in plugins
* Android / Android-wear support:
  * background service (allowing to run even during standby)
  * plugins to access Android sensors / audio device
  * native code (ndk) is used that provides better performance than interpreted Java code; native code also makes it possible to use hardware acceleration like ARM's [NEON](https://www.arm.com/products/processors/technologies/neon.php)
  * can be used in combination with [ssj](https://github.com/hcmlab/ssj) pipelines for example to access additional sensors and to  use additional Java code

## Documentation
Please see our [Github Wiki](https://github.com/hcmlab/mobileSSI/wiki) to get started with mobileSSI.

## Simple setup with Docker
If you just want to try mobileSSI on your Linux PC oder Raspberry Pi you can try our Docker images that can also be used for compiling mobileSSI: [Wiki](https://github.com/hcmlab/mobileSSI/wiki/mobileSSI-with-Docker%3A-run)

## About
The mobileSSI framework is beeing developed at the [Lab for Human Centered Multimedia](http://www.hcm-lab.de/) of the [University of Augsburg](http://www.uni-augsburg.de/).

## License
Core libraries are released under LGPL (see LICENSE in core folder).
By default plugins are released under GPL, but individual licences may apply (see LICENSE file in plugin folder).
For 3rd party dependencies see the README file and additional license files.
