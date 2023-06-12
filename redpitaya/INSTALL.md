# ![The LAGO Project](../docs/images/lago-logo.png "The LAGO Project") The LAGO ACQUA suite

Only a few libraries are needed to compile and operate LAGO ACQUA v2 suite. In the Ubuntu 14.04 version of the OS installed on the Red Pitaya board, just run this command:

```bash
apt update
apt upgrade
apt install i2c-tools libi2c-dev python-smbus build-essential screen rsync
cd src/
make
```

and that's it. If everything was fine, you will find a binary file called **lago** in the current directory. This directory is self-contained: all you need to run the LAGO acquisition is here.

For further information, including the installation of the LAGO image of the SD card, please check the ACQUA resources section in the [project wiki](http://wiki.lagoproject.net).

## About LAGO

The [Latin American Giant Observatory (LAGO)](http://lagoproject.net) is an extended Astroparticle Observatory at global scale. Please contact us directly at [lago@lagoproject.net](mailto:lago@lagoproject.net).
