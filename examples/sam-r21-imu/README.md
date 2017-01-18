You will need to symlink the libraries/ directory to your project.

The libraries/ folder should contain a symlink to your copy of the 
Atmel Software Framework (such as asf-3.21.0/).

Symlink the boards/femtobeacon/ folder to make it available as 
libraries/<your asf folder>/sam0/boards/femtobeacon/

Finally, grab a checkout of the Femto.io "MPU9250" library from git at 
https://github.com/femtoio/MPU9250, and symlink it to the 
libraries/<your asf folder>/thirdparty/ folder, resulting in a directory 
that looks like this:

libraries/<your asf folder/thirdparty/MPU9250

...Eclipse C++ with the GNU ARM Eclipse plugin should compile your project 
just fine now.
