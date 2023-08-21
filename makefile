MAKE:=$(MAKE) --no-print-directory

.PHONY: target
target:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N

.PHONY: release
release:
	@$(MAKE) -f rockhopper-target.mk RELEASE=Y DEBUG=N package

.PHONY: upload
upload:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N upload

.PHONY: upload_release
upload_release:
	@$(MAKE) RELEASE=Y DEBUG=N upload

.PHONY: json
json:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N erd_definitions

.PHONY: tdd
tdd:
	@$(MAKE) -f tdd.mk -j16

.PHONY: verbose
verbose:
	@$(MAKE) -f tdd.mk -j16 verbose

.PHONY: debug
debug:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=Y

.PHONY: gdb_server
gdb_server:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=Y gdb_server

.PHONY: clean
clean:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N clean
	@$(MAKE) -f tdd.mk RELEASE=N DEBUG=N clean

.PHONY: erase
erase:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=N erase

.PHONY: parametric
parametric:
	@$(MAKE) -f rockhopper-target.mk RELEASE=N DEBUG=Y build_parametric
