
V ?= 0
ECHO := @echo
ECHO_BLANK_LINE := echo
ifeq ($(V),0)
	Q := @
else
	Q:=
endif

MAKE_DEP = -Wp,-MD,$(DEP) -MT $$@ -MP


define SHELL_REMOVE_DIR
	-${Q}rm -rf  "${1}"
endef

# NOTE: The line continuation '\' is required in the next define otherwise we
# end up with a line-feed characer at the end of the last c filename.
# Also bear this issue in mind if extending the list of supported filetypes.
define SOURCES_TO_OBJS_NDIR
        $(notdir $(patsubst %.c,%.o,$(filter %.c,$(1)))) \
        $(notdir $(patsubst %.S,%.o,$(filter %.S,$(1))))
endef

define SOURCES_TO_OBJS
        $(patsubst %.c,%.o,$(filter %.c,$(1))) \
        $(patsubst %.S,%.o,$(filter %.S,$(1)))
endef

# MAKE_C builds a C source file and generates the dependency file
#   $(1) = output directory
#   $(2) = source file (%.c)
#   $(3) = BL stage
define MAKE_C

$(eval OBJ := $(1)/$(patsubst %.c,%.o,$(notdir $(2))))
$(eval DEP := $(patsubst %.o,%.d,$(OBJ)))
$(eval COMPILER_OPTIONS := $(3))

$(OBJ): $(2) $(filter-out %.d,$(MAKEFILE_LIST))
	$$(Q)mkdir -p $(1)
	$$(ECHO) "  CC      $$<"
	$$(Q)$$(CC) $$(CFLAGS) $(COMPILER_OPTIONS) $(MAKE_DEP) -c $$< -o $$@

-include $(DEP)

endef

# MAKE_S builds an assembly source file and generates the dependency file
#   $(1) = output directory
#   $(2) = assembly file (%.S)
#   $(3) = BL stage
define MAKE_S

$(eval OBJ := $(1)/$(patsubst %.S,%.o,$(notdir $(2))))
$(eval DEP := $(patsubst %.o,%.d,$(OBJ)))
$(eval COMPILER_OPTIONS := $(3))

$(OBJ): $(2) $(filter-out %.d,$(MAKEFILE_LIST))
	$$(Q)mkdir -p $(1)
	$$(ECHO) "  AS      $$<"
	$$(Q)$$(AS) $(COMPILER_OPTIONS) $(MAKE_DEP) -c $$< -o $$@

-include $(DEP)

endef


# MAKE_OBJS builds both C and assembly source files
#   $(1) = output directory
#   $(2) = list of source files (both C and assembly)
#   $(3) = BL stage
define MAKE_OBJS
        $(eval C_OBJS := $(filter %.c,$(2)))
        $(eval REMAIN := $(filter-out %.c,$(2)))
        $(eval $(foreach obj,$(C_OBJS),$(call MAKE_C,$(1),$(obj),$(3))))

        $(eval S_OBJS := $(filter %.S,$(REMAIN)))
        $(eval REMAIN := $(filter-out %.S,$(REMAIN)))
        $(eval $(foreach obj,$(S_OBJS),$(call MAKE_S,$(1),$(obj),$(3))))

        $(and $(REMAIN),$(error Unexpected source files present: $(REMAIN)))
endef
