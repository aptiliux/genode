VBOX_MACH := $(filter $(SPECS), x86_32 x86_64)

VBOX_MACH_CC_OPT_x86_32  = -DRT_ARCH_X86 -D__X86__ -DHC_ARCH_BITS=32
VBOX_MACH_ASM_OPT_x86_32 = -f elf32
VBOX_MACH_CC_OPT_x86_64  = -DRT_ARCH_AMD64 -D__AMD64__ -DHC_ARCH_BITS=64
VBOX_MACH_ASM_OPT_x86_64 = -f elf64

include $(REP_DIR)/ports/virtualbox.inc

ifeq ($(shell which yasm),)
REQUIRES += installation_of_yasm
REQUIRES += installation_of_iasl
endif

VIRTUALBOX_DIR = $(REP_DIR)/contrib/$(VIRTUALBOX)
VBOX_DIR       = $(VIRTUALBOX_DIR)/src/VBox

VBOX_CC_OPT += -DIN_RING3 -DVBOX -DVBOX_OSE \
               -DGC_ARCH_BITS=64 \
               -D_FILE_OFFSET_BITS=64 -DLOG_ENABLED

VBOX_CC_OPT += $(VBOX_MACH_CC_OPT_$(VBOX_MACH))

# Required if on a 32bit host 64bit VMs should be executed
# VBOX_CC_OPT += -DVBOX_WITH_64_BITS_GUESTS

VBOX_CC_OPT += -DIN_SUP_R3 -DIN_VMM_R3

# we use the libc headers from FreeBSD
VBOX_CC_OPT += -DRT_OS_FREEBSD

VBOX_CC_OPT += -DVBOX_WITH_REM

VBOX_CC_OPT += -DVBOXBFE_WITHOUT_COM
VBOX_CC_OPT += -DVBOX_WITHOUT_TESTING_FEATURES

VBOX_CC_OPT += -DUSE_SDL

VBOX_CC_OPT += -DRTLOG_REL_ENABLED -DRT_STRICT -DVBOX_STRICT

VBOX_CC_OPT += -DVBOX_VERSION_MAJOR=$(VIRTUALBOX_VERSION_MAJOR) \
               -DVBOX_VERSION_MINOR=$(VIRTUALBOX_VERSION_MINOR) \
               -DVBOX_VERSION_BUILD=$(VIRTUALBOX_VERSION_BUILD)

VBOX_CC_OPT += -DVBOX_HDD_NO_DYNAMIC_BACKENDS

CC_WARN += -Wno-trigraphs

CC_OPT += $(VBOX_CC_OPT)

LIBS += virtualbox_libc_support

INC_DIR += $(REP_DIR)/src/virtualbox/include
INC_DIR += $(VIRTUALBOX_DIR)/include
INC_DIR += $(VBOX_DIR)/Devices/build

vpath %.cpp $(VBOX_DIR)
vpath %.c   $(VBOX_DIR)
vpath %.asm $(VBOX_DIR)

%.o: %.asm
	$(MSG_ASSEM)$@
	$(VERBOSE)yasm $(VBOX_MACH_ASM_OPT_$(VBOX_MACH)) -DASM_FORMAT_ELF -D__YASM__ \
	               $(addprefix -I,$(INC_DIR)) $(VBOX_CC_OPT) -o $@ $<

# vi: set ft=make :
