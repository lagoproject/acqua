all:
	@sdcc -h > /dev/null 2> /dev/null || echo "\nREAD THE README!!!!\n"
	@sdcc -h > /dev/null 2> /dev/null
	cd makestuff/libs/libfpgalink && make deps
	cd makestuff/apps/lago && make
	cp -f makestuff/apps/lago/*/rel/* .

clean:
	cd makestuff/libs/libfpgalink && make clean deps
	cd makestuff/libs/libfpgalink && make clean
	cd makestuff/apps/lago && make clean
	rm lago lib*
