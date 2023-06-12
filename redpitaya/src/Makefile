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
$(info LAGO_DAQ is not set.)
$(info Please run lago-configs.pl before to compile)
$(error )
endif
ifneq ($(LAGO_DAQ), $(PWD))
$(info LAGO_DAQ is defined, but you are not in the directory pointed by LAGO_DAQ.)
$(info LAGO_DAQ=$(LAGO_DAQ))
$(info current dir=$(PWD))
$(error )
endif
