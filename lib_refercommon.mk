$(call create_library_with_defaults,Common,lib/Common)

Common_INC_DIRS:=\
	$(Common_LIB_ROOT)/Source/Gea \

INC_DIRS+=\
	$(Common_INC_DIRS) \
