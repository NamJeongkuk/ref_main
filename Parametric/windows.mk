include tools/makecommon/makecommon.mk
include tools/makecommon/gh/git-hash.mk

OUTPUT_DIR:=build
ENDIANNESS:=little

export IMAGE_CRC

PACKAGE_CONTENTS=
$(call add_to_package_v1_compatibility,$(OUTPUT_DIR)/*.xml,doc)
$(call add_to_package_v1_compatibility,$(OUTPUT_DIR)/*.txt,doc)

.PHONY: all
all: parametric

.PHONY: parametric
parametric: $(OUTPUT_DIR)/rockhopper.parametric.apl summary

.PHONY: package
package: parametric
	$(eval IMAGE_CRC=$(strip $(shell ($(LUA53) lib/lua-image-header-dump/lua-image-header-dump.lua --srec $(OUTPUT_DIR)/rockhopper.parametric.apl) | findstr /r /c:"Calculated Image CRC: 0x[0-9A-Fa-f]*" | for /f "tokens=4 delims= " %%a in ('more') do @echo %%a)))
	@echo Version is defined in data\Development\rockhopper.parametric.lua
	@echo Renaming apl ...
	@$(LUA53) $(LUA_VERSION_RENAMER) --input $(OUTPUT_DIR)/rockhopper.parametric.apl --endianness $(ENDIANNESS) --output_directory $(OUTPUT_DIR) --base_name rockhopper_$(GIT_SHORT_HASH)_$(IMAGE_CRC).apl
	
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
	@echo Cleaning ...
	@$(call delete_directory,$(OUTPUT_DIR))
	@$(call delete_directory,artifacts)
