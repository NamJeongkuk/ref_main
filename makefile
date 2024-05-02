export RELEASE?=N
export DEBUG?=N

MAKEFLAGS:=--no-print-directory -j16 $(MAKEFLAGS)
NVM_DIR?=$$NVM_DIR

.PHONY: % $(MAKECMDGOALS)
.ONESHELL:

MAKE_TARGET=+ $(MAKE) -f rockhopper-target.mk
MAKE_TDD=+ $(MAKE) -f tdd.mk

target:
	$(MAKE_TARGET)

debug:
	$(MAKE_TARGET) DEBUG=Y

release:
	$(MAKE_TARGET) RELEASE=Y package

upload_release:
	$(MAKE_TARGET) RELEASE=Y upload

erd_definitions upload erase:
	$(MAKE_TARGET) $@

clean:
	$(MAKE_TARGET) clean
	$(MAKE_TDD) clean

tdd:
	$(MAKE_TDD)

verbose:
	$(MAKE_TDD) verbose

integration:
	$(MAKE_TDD) INTEGRATION=Y

group-%:
	$(MAKE_TDD) group GROUP=$*

node:
	@. $(NVM_DIR)/nvm.sh
	@nvm install
	@npm install

bench_test: erd_definitions node
	@npm run bench_test
