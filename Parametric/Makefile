include tools/makecommon/makecommon.mk
include tools/makecommon/gh/git-hash.mk

OUTPUT_DIR:=build
ENDIANNESS:=little
VERSION:=$(call version_string,versions.lua)

export IMAGE_CRC

$(call add_to_package,{ from = '$(OUTPUT_DIR)/*.xml', to = 'doc/' })
$(call add_to_package,{ from = '$(OUTPUT_DIR)/*.txt', to = 'doc/' })

.PHONY: all
all: parametric

.PHONY: parametric
parametric: $(OUTPUT_DIR)/rockhopper.parametric.apl summary

.PHONY: package
package: parametric
	$(eval IMAGE_CRC=$(shell $(LUA53) lib/lua-image-header-dump/lua-image-header-dump.lua --srec $(OUTPUT_DIR)/rockhopper.parametric.apl | grep -m1 -oE 'Calculated Image CRC: 0x([0-9A-Fa-f]+)' | cut -d' ' -f4))
	$(call add_to_package,{ from = '$(OUTPUT_DIR)/rockhopper.parametric.apl', to = 'rockhopper_$(VERSION)_$(GIT_SHORT_HASH)_$(IMAGE_CRC).apl' })
	@echo Creating artifacts rockhopper_parametric_$(VERSION)_$(GIT_SHORT_HASH)_$(IMAGE_CRC).zip
	@$(call makecommon_create_artifacts,rockhopper_parametric_$(VERSION)_$(GIT_SHORT_HASH)_$(IMAGE_CRC).zip,artifacts)

.PHONY: $(OUTPUT_DIR)/rockhopper.parametric.apl
$(OUTPUT_DIR)/rockhopper.parametric.apl: $(OUTPUT_DIR)/rockhopper.parametric.mot
	@echo Creating $@...
	@$(LUA53) $(LUA_MOT_TO_APL) --endianness $(ENDIANNESS) --srec $< --output $@

.PHONY: $(OUTPUT_DIR)/rockhopper.parametric.mot
$(OUTPUT_DIR)/rockhopper.parametric.mot: $(OUTPUT_DIR)/rockhopper.parametric.xml
	@echo Creating $@...
	@$(LUA53) $(LUA_XML_TO_SREC) --input $< --output $@

.PHONY: $(OUTPUT_DIR)/rockhopper.parametric.xml
$(OUTPUT_DIR)/rockhopper.parametric.xml: $(OUTPUT_DIR)
	@echo Creating $@...
	@$(LUA53) build_parametric.lua data/Development/rockhopper.parametric.lua > $@

.PHONY: $(OUTPUT_DIR)/parametric_summary.txt
$(OUTPUT_DIR)/parametric_summary.txt: $(OUTPUT_DIR)
	@echo Building $@...
	@$(LUA53) summarize_parametric.lua data/Development/rockhopper.parametric.lua $@

.PHONY: summary
summary: $(OUTPUT_DIR)/parametric_summary.txt

$(OUTPUT_DIR):
	@$(call make_directory,$@)

.PHONY: clean
clean:
	@$(call delete_directory,$(OUTPUT_DIR))
	@$(call delete_directory,artifacts)
