# ![The LAGO Project](http://lagoproject.org/images/lago-logo-90.png "The LAGO Project") The LAGO ACQUA suite


Only a few libraries are needed to compile and operate LAGO ACQUA BRC suite. In ubuntu (recomended) and other debian based Linux, just run this command:

```bash
sudo apt-get install build-essential libusb-dev cc1111
cd lago
make
make clean
make
```
and that's it (*please note the first make is to assure the correct identification of the libraries path, while the make clean and further make are intendend to avoid residuals from previous compilations*). If everything was fine, you will find a binary file called **lago** in the current directory. This directory is self-contained: all you need to run the LAGO acquisition is here.

For further information, please check the ACQUA resources section in the [project wiki](http://wiki.lagoproject.org).

## Raspberry Pi

Coming soon.

## Other OS

We have some untested builds for other operative system. If you want to test or help to developed this system, please don't hesitate to contact us.

## About LAGO

The [Latin American Giant Observatory (LAGO)](http://lagoproject.org) is an extended Astroparticle Observatory at global scale. Please contact us directly at [lago@lagoproject.org](mailto:lago@lagoproject.org) or stay tuned with us at [@lagoproject](https://twitter.com/lagoproject).
