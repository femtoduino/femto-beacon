# femto-beacon
FemtoBeacon - Atmel SAM R21 (ARM Cortex M0+, SAM D21 with built in AT86RF233)

To contribute work, please do the following:

 - Fork https://github.com/femtoduino/femto-beacon
 - Checkout your fork of femto-beacon to your machine. You should now have something like `https://github.com/<your_username>/femto-beacon`
 - Add the femtoio/femto-beacon repository as the 'upstream' remote repository of your fork.

```
# Inside the checkout of your fork, add us as the remote repository named 'upstream'
git remote add upstream https://github.com/femtoduino/femto-beacon.git

# Remember to fetch from upstream to get all branch information
git fetch upstream
```

Now, you can pull the latest changes from 'upstream' into your fork, and push all work to your own fork
```
# To list branches.
git branch -a

# To pull changes from upstream 'master' branch into your 'master' branch (assuming you're in the master branch already)
git pull upstream master

# To switch into an existing branch named 'develop'
git checkout develop

# To create a new branch based on the develop branch
git checkout develop
# ...then, make sure you get the latest from upstream
git pull upstream develop
# ...now, create your new branch.
git checkout -b my-new-branch

# To push your new branch to your github fork
git push origin my-new-branch
```

...Once you push your changes to your fork, you may submit a Merge Request via github.
We will review and assess the changes before accepting or rejecting your changes.

## Requirements

For Arduino support, see https://github.com/femtoduino/ArduinoCore-atsamd21e18a/

For bare-metal programming, see below.

On Ubuntu machines, you will need Terry Guo's arm-none-eabi-* toolchain, along with the build-essentials package, openocd 0.9.x with cmsis-dap and hidapi-libusb enabled, and the BOSSA utility.

See the following on how to build OpenOCD on your machine: https://github.com/RIOT-OS/RIOT/wiki/OpenOCD
You must assert the required libraries are installed on your machine.

Compile OpenOCD with the following flags to enable cmsis-dap and hidapi support:
```
# Inside the openocd source code folder...
./configure --enable-maintainer-mode --enable-cmsis-dap --enable-hidapi-libusb
make
sudo make install
```

Install Eclipse C++, and the GNU ARM Eclipse plugins.

Install the BOSSA utility.

See https://github.com/shumatech/BOSSA


## Setup
Download the Atmel Software Framework into the libraries/ folder before compiling examples.
If you have the Atmel Software framework files in a directory named `asf-3.21.0`, then you would need it as `femto-beacon/libraries/asf-3.21.0`

Symlink the boards/femtobeacon folder into your copy of the ASF framework (libraries/asf-x.xx.x/sam0/boards/femtobeacon).
Symlink the femtoIO MPU9250 library folder into your copy of the ASF framework (libraries/asf-x.xx.x/thirdparty/MPU9250).

You can symlink the femto-beacon examples to your Eclipse workspace folder. For example, if you want to make the 'sam-r21-blink' project availabe in Eclipse:

```
# Use ~ or full path! Do not use ./
ln -s /path/to/femto-beacon/examples/sam-r21-blink /path/to/eclipse/workspace/sam-r21-blink
```

...Refresh the Project Explorer panel to view it as a project.

Update the example's config.mk file accordingly
```
# Path to top level ASF directory relative to this project directory.
PRJ_PATH = ../../libraries/asf-3.21.0
```

Then, open up the example in Eclipse, and build. This should generate a BIN file you can then upload to your board using the BOSSA shell.

## How to create a new project
Create a new C project in Eclipse, symlink the libraries/ folder from femto-beacon/libraries into your project's directory.
```
cd MyNewProject/
ln -s /path/to/femto-beacon/libraries libraries
```
You can then copy/modify the asf.h, config.mk, and Makefile into your new project. Remember to modify config.mk to include the paths of the ASF modules you wish to use.


## FemtoBeacon BT support (Nordic nRF52832 Bluetooth 4.2 + NFC, ARM Cortex M4L)
This board is unreleased at this time. These notes are here for future use.
For toolchain setup, see https://devzone.nordicsemi.com/tutorials/7/
