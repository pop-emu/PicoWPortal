# PicoWPortal
This is a project that aims to emulate a Skylanders Portal of Power, specifically the Traptanium portal, on a RaspberryPi Pico W.

## How to build
### Setting up the SDK
To build, first install the [pico sdk](https://github.com/raspberrypi/pico-sdk) and set the `PICO_SDK_PATH` enviroment variable. Don't forget to also set the `BOARD` enviroment variable to `pico-w` A more detailed guid on how to set up the sdk and required toolchains can be found [in their getting started guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf).

### Configuring the project
To configure the project, run cmake in the root folder with the arguments `-DWIFI_SSID=<SIDD>` and `-DWIFI_PASSWORD=<PASSWORD>`.
An example would be `cmake -DWIFI_SSID=test -DWIFI_PASSWORD=test -B ./build`

### Building
To build, just run cmake with the build flag and the path to the build directory. For example, `cmake --build ./build`.

After this, the final result should reside at `BUILD_DIR/PicoWPortal.uf2`. This file can then be uploaded to the Pico W.

## Errors
### Incorrect network credentials
If the network SSID does not exist or the network password is incorrect, the device will not connect and the built-in LED will not turn on. In this case, the project will need to be rebuild with the correct credentials.

### USB crash
On devices that run a traditional operating system, like a windows PC, it is possible for the usb handling to crash. If you get a message that the usb device that was inserted could not be recognised, or if the device does not show up, unplugging the device and plugging it back in should fix it. This may take multiple tries. This issue is more prominent on older systems. This issue can be subverted by connecting the dvice to a usb port that has a usb controller that isn't being used yet.

### Server crash
The way chrome handles sending data has the ability to confuse and crash the server running on the Pico W. I have tried to minimize the chance of this happening, but it seems like this is an issue with the networking layer, LWIP, that I'm using. An isolated network, like a hotspot, fixes this issue.

## How to acquire the IP address of the device
There are several ways to obtain the IP address of the device.

### Using SWD
When connected to the device using the Serial Wire Debug interface, setting a breakpoint at the line `printf("My ip is %s", ip);` (approximately line 33), will allow you to read the ip variable which contains the ip address as a char* to a string.

### Using UART
When connected to the device using the UARTS pins, the device should log its IP address.

### Using a network with a small device count
For this, I reccomend using a mobile hotspot. Make sure the device you are using and the Pico W are connected. Then, use a program like [advanced IP scanner](https://www.advanced-ip-scanner.com/) to find all the devices on the netowrk. Then, try the devices addresses listed.
