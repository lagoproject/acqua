all: check-lago
	cd $(LAGO_DAQ)/src && make clean && make
	cp -f $(LAGO_DAQ)/src/lago .

clean: check-lago
	cd $(LAGO_DAQ)/src && make clean 
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
