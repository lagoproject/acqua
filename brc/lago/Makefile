all: check-lago
	@sdcc -h > /dev/null 2> /dev/null || echo "\nREAD THE README!!!!\n"
	@sdcc -h > /dev/null 2> /dev/null
	cd $(LAGO_DAQ)/makestuff/libs/libfpgalink && make depclean && make deps
	cd $(LAGO_DAQ)/makestuff/apps/lago && make
	cp -f $(LAGO_DAQ)/makestuff/apps/lago/lago .

clean: check-lago
	cd $(LAGO_DAQ)/makestuff/libs/libfpgalink && make clean deps
	cd $(LAGO_DAQ)/makestuff/libs/libfpgalink && make depclean && make clean
	cd $(LAGO_DAQ)/makestuff/apps/lago && make clean
	rm lago
check-lago:
ifndef LAGO_DAQ
$(warning LAGO_DAQ is not set. Using $(PWD))
LAGO_DAQ=$(PWD)
$(warning LAGO_DAQ was set to $(LAGO_DAQ))
endif
