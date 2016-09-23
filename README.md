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
The mobileSSI framework is beeing developed at the Lab for [Human Centered Multimedia](http://www.hcm-lab.de/) of the University of Augsburg.

## License
This library is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or any later version.
This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
