SHELL := /bin/bash

all:
	pushd libnewgpio; make; popd
	pushd libnewi2c; make; popd
	pushd libarduino; make; popd
	pushd iotemplate; make; popd
	pushd i2cscan; make; popd
	pushd gpioirq; make; popd
	pushd gpioexpled; make; popd
	pushd omegaio; make; popd
	pushd arduino_omega; make; popd

 
libnewgpio:
	pushd libnewgpio; make; popd
 
libnewi2c:
	pushd libnewi2c; make; popd
 
libarduino:
	pushd libarduino; make; popd
 
iotemplate:
	pushd iotemplate; make; popd
 
i2cscan:
	pushd i2cscan; make; popd
 
gpioirq:
	pushd gpioirq; make; popd
 
gpioexpled:
	pushd gpioexpled; make; popd
 
omegaio:
	pushd omegaio; make; popd
 
arduino_omega:
	pushd arduino_omega; make; popd

clean: libnewgpio-clean\
	libnewi2c-clean\
	libarduino-clean\
	iotemplate-clean\
	i2cscan-clean\
	gpioirq-clean\
	gpioexpled-clean\
	omegaio-clean\
	arduino_omega-clean

libnewgpio-clean:
	pushd libnewgpio; make clean; popd
 
libnewi2c-clean:
	pushd libnewi2c; make clean; popd
 
libarduino-clean:
	pushd libarduino; make clean; popd
 
iotemplate-clean:
	pushd iotemplate; make clean; popd
 
i2cscan-clean:
	pushd i2cscan; make clean; popd
 
gpioirq-clean:
	pushd gpioirq; make clean; popd
 
gpioexpled-clean:
	pushd gpioexpled; make clean; popd
 
omegaio-clean:
	pushd omegaio; make clean; popd
 
arduino_omega-clean:
	pushd arduino_omega; make clean; popd

