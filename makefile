UNAME := $(shell uname)

MAKE=make -f rockhopper-target.mk
MAKE_RELEASE=$(MAKE) RELEASE=Y DEBUG=N package
MAKE_NO_RELEASE=$(MAKE) RELEASE=N

.PHONY: target
target:
	$(MAKE_NO_RELEASE) DEBUG=N

.PHONY: release
release:
	$(MAKE_RELEASE)

.PHONY: upload
upload:
	 $(MAKE_NO_RELEASE) DEBUG=N upload

.PHONY: upload_release
upload_release:
	 $(MAKE) RELEASE=Y DEBUG=N upload

.PHONY: json
json:
	$(MAKE_NO_RELEASE) DEBUG=N erd_definitions

.PHONY: tdd
tdd:
	make -f tdd.mk -j16

.PHONY: verbose
verbose:
	make -f tdd.mk -j16 verbose

.PHONY: debug
debug:
	$(MAKE_NO_RELEASE) DEBUG=Y

.PHONY: gdb_server
gdb_server:
	$(MAKE_NO_RELEASE) DEBUG=Y gdb_server

.PHONY: clean
clean:
	$(MAKE_NO_RELEASE) DEBUG=N clean
	make -f tdd.mk RELEASE=N DEBUG=N clean

.PHONY: erase
erase:
	$(MAKE_NO_RELEASE) DEBUG=N erase

.PHONY: parametric
parametric:
	$(MAKE_NO_RELEASE) DEBUG=Y build_parametric
