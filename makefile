UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
MAKE=make -f rx130-target.mk
else
MAKE=dmake -f rx130-target.mk
endif
MAKE_RELEASE=$(MAKE) RELEASE=Y DEBUG=N OLD_HW=Y package
MAKE_NO_RELEASE=$(MAKE) RELEASE=N

.PHONY: target
target:
	$(MAKE_NO_RELEASE) DEBUG=N OLD_HW=Y

.PHONY: release
release:
	$(MAKE_RELEASE)

.PHONY: upload
upload:
	 $(MAKE_NO_RELEASE) DEBUG=N OLD_HW=Y upload

.PHONY: upload_release
upload_release:
	 $(MAKE) RELEASE=Y DEBUG=N OLD_HW=Y upload

.PHONY: json
json:
	$(MAKE_NO_RELEASE) DEBUG=N OLD_HW=Y erd_definitions

.PHONY: tdd
tdd:
	make -f tdd.mk -j16

.PHONY: verbose
verbose:
	make -f tdd.mk -j16 verbose

.PHONY: debug
debug:
	$(MAKE_NO_RELEASE) DEBUG=Y OLD_HW=Y

.PHONY: gdb_server
gdb_server:
	$(MAKE_NO_RELEASE) DEBUG=Y OLD_HW=Y gdb_server

.PHONY: clean
clean:
	$(MAKE_NO_RELEASE) DEBUG=N OLD_HW=Y clean
	make -f tdd.mk RELEASE=N DEBUG=N clean

.PHONY: erase
erase:
	$(MAKE_NO_RELEASE) DEBUG=N OLD_HW=Y erase

.PHONY: parametric
parametric:
	$(MAKE_NO_RELEASE) DEBUG=Y OLD_HW=Y build_parametric
