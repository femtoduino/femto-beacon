# femto-beacon
FemtoBeacon - Atmel SAM R21 (ARM Cortex M0+, SAM D21 with built in AT86RF233)

To contribute work, please do the following:

 - Fork https://github.com/femtoio/femto-beacon
 - Checkout your fork of femto-beacon to your machine. You should now have something like `https://github.com/<your_username>/femto-beacon`
 - Add the femtoio/femto-beacon repository as the 'upstream' remote repository of your fork.

```
# Inside the checkout of your fork, add us as the remote repository named 'upstream'
git remote add upstream https://github.com/femtoio/femto-beacon.git

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
On Ubuntu machines, you will need Terry Guo's arm-none-eabi-* toolchain, along with the build-essentials package, openocd 0.9.x with cmsis-dap and hidapi-libusb enabled, and our fork of BOSSA.

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

Additionally, our fork of the BOSSA utility includes support for the ATSAMR21E18A chip w/ SAM-BA bootloader (in the 'arduino' branch)
See https://github.com/femtoio/BOSSA


## Setup
You can symlink the femto-beacon examples to your Eclipse workspace folder. For example, if you want to make the 'samr21-blinky' project availabe in Eclipse:

```
# Use ~ or full path! Do not use ./
ln -s /path/to/femto-beacon/examples/samr21-blinky /path/to/eclipse/workspace/samr21-blinky
```

...Refresh the Project Explorer panel to view it as a project.

Download the Atmel Software Framework into the libraries/ folder before compiling examples.
For example, if you have the Atmel Software framework files in a directory named `asf-3.21.0`, then you would need it as `libraries/asf-3.21.0`

Update the example's config.mk file accordingly
```
# Path to top level ASF directory relative to this project directory.
PRJ_PATH = ../../libraries/asf-3.21.0
```

Then, open up the example in Eclipse, and build. This should generate a BIN file you can then upload to your board using the BOSSA shell.