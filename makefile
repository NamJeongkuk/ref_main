MAKEFLAGS:=--no-print-directory -j16 $(MAKEFLAGS)

.PHONY: *

target:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N

release:
	@$(MAKE) -f rockhopper-target.mk RELEASE=Y DEBUG=N package

upload:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N upload

upload_release:
	@$(MAKE) RELEASE=Y DEBUG=N upload

json:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N erd_definitions

tdd:
	@$(MAKE) -f tdd.mk

verbose:
	@$(MAKE) -f tdd.mk verbose

debug:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=Y

gdb_server:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=Y gdb_server

clean:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N clean
	@$(MAKE) -f tdd.mk RELEASE=N DEBUG=N clean

erase:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N erase

parametric:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=Y build_parametric

integration:
	@$(MAKE) tdd INTEGRATION=Y
