#!/usr/bin/make -f
# Makefile for DPF #
# ---------------- #
# Created by falkTX
#

AR  ?= ar
CC  ?= gcc
CXX ?= g++

# ---------------------------------------------------------------------------------------------------------------------
# Auto-detect OS if not defined

TARGET_MACHINE := $(shell $(CC) -dumpmachine)

ifneq ($(BSD),true)
ifneq ($(HAIKU),true)
ifneq ($(HURD),true)
ifneq ($(LINUX),true)
ifneq ($(MACOS),true)
ifneq ($(WINDOWS),true)

ifneq (,$(findstring bsd,$(TARGET_MACHINE)))
BSD=true
endif
ifneq (,$(findstring haiku,$(TARGET_MACHINE)))
HAIKU=true
endif
ifneq (,$(findstring gnu,$(TARGET_MACHINE)))
HURD=true
endif
ifneq (,$(findstring linux,$(TARGET_MACHINE)))
LINUX=true
endif
ifneq (,$(findstring apple,$(TARGET_MACHINE)))
MACOS=true
endif
ifneq (,$(findstring mingw,$(TARGET_MACHINE)))
WINDOWS=true
endif

endif
endif
endif
endif
endif
endif

# ---------------------------------------------------------------------------------------------------------------------
# Auto-detect the processor

TARGET_PROCESSOR := $(firstword $(subst -, ,$(TARGET_MACHINE)))

ifneq (,$(filter i%86,$(TARGET_PROCESSOR)))
CPU_I386=true
CPU_I386_OR_X86_64=true
endif
ifneq (,$(filter x86_64,$(TARGET_PROCESSOR)))
CPU_X86_64=true
CPU_I386_OR_X86_64=true
endif
ifneq (,$(filter arm%,$(TARGET_PROCESSOR)))
CPU_ARM=true
CPU_ARM_OR_AARCH64=true
endif
ifneq (,$(filter aarch64%,$(TARGET_PROCESSOR)))
CPU_AARCH64=true
CPU_ARM_OR_AARCH64=true
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set PKG_CONFIG (can be overridden by environment variable)

ifeq ($(WINDOWS),true)
# Build statically on Windows by default
PKG_CONFIG ?= pkg-config --static
else
PKG_CONFIG ?= pkg-config
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set LINUX_OR_MACOS

ifeq ($(LINUX),true)
LINUX_OR_MACOS=true
endif

ifeq ($(MACOS),true)
LINUX_OR_MACOS=true
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set MACOS_OR_WINDOWS and HAIKU_OR_MACOS_OR_WINDOWS

ifeq ($(HAIKU),true)
HAIKU_OR_MACOS_OR_WINDOWS=true
endif

ifeq ($(MACOS),true)
MACOS_OR_WINDOWS=true
HAIKU_OR_MACOS_OR_WINDOWS=true
endif

ifeq ($(WINDOWS),true)
MACOS_OR_WINDOWS=true
HAIKU_OR_MACOS_OR_WINDOWS=true
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set UNIX

ifeq ($(BSD),true)
UNIX=true
endif

ifeq ($(HURD),true)
UNIX=true
endif

ifeq ($(LINUX),true)
UNIX=true
endif

ifeq ($(MACOS),true)
UNIX=true
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set build and link flags

BASE_FLAGS = -Wall -Wextra -pipe -MD -MP
BASE_OPTS  = -O3 -ffast-math -fdata-sections -ffunction-sections

ifeq ($(CPU_I386_OR_X86_64),true)
BASE_OPTS += -mtune=generic -msse -msse2
endif

ifeq ($(CPU_ARM),true)
BASE_OPTS += -mfpu=neon-vfpv4 -mfloat-abi=hard
endif

ifeq ($(MACOS),true)
# MacOS linker flags
LINK_OPTS  = -fdata-sections -ffunction-sections -Wl,-dead_strip -Wl,-dead_strip_dylibs
else
# Common linker flags
LINK_OPTS  = -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-O1 -Wl,--as-needed
ifneq ($(SKIP_STRIPPING),true)
LINK_OPTS += -Wl,--strip-all
endif
endif

ifeq ($(NOOPT),true)
# Non-CPU-specific optimization flags
BASE_OPTS  = -O2 -ffast-math -fdata-sections -ffunction-sections
endif

ifeq ($(WINDOWS),true)
# mingw has issues with this specific optimization
# See https://github.com/falkTX/Carla/issues/696
BASE_OPTS  += -fno-rerun-cse-after-loop
# See https://github.com/falkTX/Carla/issues/855
BASE_OPTS  += -mstackrealign
else
# Not needed for Windows
BASE_FLAGS += -fPIC -DPIC
endif

ifeq ($(DEBUG),true)
BASE_FLAGS += -DDEBUG -O0 -g
LINK_OPTS   =
else
BASE_FLAGS += -DNDEBUG $(BASE_OPTS) -fvisibility=hidden
CXXFLAGS   += -fvisibility-inlines-hidden
endif

BUILD_C_FLAGS   = $(BASE_FLAGS) -std=gnu99 $(CFLAGS)
BUILD_CXX_FLAGS = $(BASE_FLAGS) -std=gnu++0x $(CXXFLAGS)
LINK_FLAGS      = $(LINK_OPTS) $(LDFLAGS)

ifneq ($(MACOS),true)
# Not available on MacOS
LINK_FLAGS     += -Wl,--no-undefined
endif

ifeq ($(MACOS_OLD),true)
BUILD_CXX_FLAGS = $(BASE_FLAGS) $(CXXFLAGS) -DHAVE_CPP11_SUPPORT=0
endif

ifeq ($(WINDOWS),true)
# Always build statically on windows
LINK_FLAGS     += -static
endif

# ---------------------------------------------------------------------------------------------------------------------
# Strict test build

ifeq ($(TESTBUILD),true)
BASE_FLAGS += -Werror -Wcast-qual -Wconversion -Wformat -Wformat-security -Wredundant-decls -Wshadow -Wstrict-overflow -fstrict-overflow -Wundef -Wwrite-strings
BASE_FLAGS += -Wpointer-arith -Wabi -Winit-self -Wuninitialized -Wstrict-overflow=5
# BASE_FLAGS += -Wfloat-equal
ifeq ($(CC),clang)
BASE_FLAGS += -Wdocumentation -Wdocumentation-unknown-command
BASE_FLAGS += -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors -Wno-float-equal
else
BASE_FLAGS += -Wcast-align -Wunsafe-loop-optimizations
endif
ifneq ($(MACOS),true)
BASE_FLAGS += -Wmissing-declarations -Wsign-conversion
ifneq ($(CC),clang)
BASE_FLAGS += -Wlogical-op
endif
endif
CFLAGS     += -Wold-style-definition -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes
CXXFLAGS   += -Weffc++ -Wnon-virtual-dtor -Woverloaded-virtual
endif

# ---------------------------------------------------------------------------------------------------------------------
# Check for required libraries

HAVE_CAIRO  = $(shell $(PKG_CONFIG) --exists cairo && echo true)
HAVE_VULKAN = $(shell $(PKG_CONFIG) --exists vulkan && echo true)

ifeq ($(MACOS_OR_WINDOWS),true)
HAVE_OPENGL = true
else
HAVE_OPENGL = $(shell $(PKG_CONFIG) --exists gl && echo true)
ifneq ($(HAIKU),true)
HAVE_X11     = $(shell $(PKG_CONFIG) --exists x11 && echo true)
HAVE_XCURSOR = $(shell $(PKG_CONFIG) --exists xcursor && echo true)
HAVE_XEXT    = $(shell $(PKG_CONFIG) --exists xext && echo true)
HAVE_XRANDR  = $(shell $(PKG_CONFIG) --exists xrandr && echo true)
endif
endif

# ---------------------------------------------------------------------------------------------------------------------
# Check for optional libraries

HAVE_JACK  = $(shell $(PKG_CONFIG) --exists jack && echo true)
HAVE_LIBLO = $(shell $(PKG_CONFIG) --exists liblo && echo true)

# ---------------------------------------------------------------------------------------------------------------------
# Set Generic DGL stuff

ifeq ($(HAIKU),true)
DGL_SYSTEM_LIBS += -lbe
endif

ifeq ($(MACOS),true)
DGL_SYSTEM_LIBS += -framework Cocoa
endif

ifeq ($(WINDOWS),true)
DGL_SYSTEM_LIBS += -lgdi32 -lcomdlg32
endif

ifneq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_X11),true)
DGL_FLAGS       += $(shell $(PKG_CONFIG) --cflags x11)
DGL_SYSTEM_LIBS += $(shell $(PKG_CONFIG) --libs x11)
ifeq ($(HAVE_XCURSOR),true)
# TODO -DHAVE_XCURSOR
DGL_FLAGS       += $(shell $(PKG_CONFIG) --cflags xcursor)
DGL_SYSTEM_LIBS += $(shell $(PKG_CONFIG) --libs xcursor)
endif
ifeq ($(HAVE_XEXT),true)
DGL_FLAGS       += $(shell $(PKG_CONFIG) --cflags xext) -DHAVE_XEXT -DHAVE_XSYNC
DGL_SYSTEM_LIBS += $(shell $(PKG_CONFIG) --libs xext)
endif
ifeq ($(HAVE_XRANDR),true)
DGL_FLAGS       += $(shell $(PKG_CONFIG) --cflags xrandr) -DHAVE_XRANDR
DGL_SYSTEM_LIBS += $(shell $(PKG_CONFIG) --libs xrandr)
endif
endif
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set Cairo specific stuff

ifeq ($(HAVE_CAIRO),true)

DGL_FLAGS   += -DHAVE_CAIRO

CAIRO_FLAGS  = $(shell $(PKG_CONFIG) --cflags cairo)
CAIRO_LIBS   = $(shell $(PKG_CONFIG) --libs cairo)

HAVE_CAIRO_OR_OPENGL = true

endif

# ---------------------------------------------------------------------------------------------------------------------
# Set OpenGL specific stuff

ifeq ($(HAVE_OPENGL),true)

DGL_FLAGS   += -DHAVE_OPENGL

ifeq ($(HAIKU),true)
OPENGL_FLAGS = $(shell $(PKG_CONFIG) --cflags gl)
OPENGL_LIBS  = $(shell $(PKG_CONFIG) --libs gl)
endif

ifeq ($(MACOS),true)
OPENGL_LIBS  = -framework OpenGL
endif

ifeq ($(WINDOWS),true)
OPENGL_LIBS  = -lopengl32
endif

ifneq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
OPENGL_FLAGS = $(shell $(PKG_CONFIG) --cflags gl x11)
OPENGL_LIBS  = $(shell $(PKG_CONFIG) --libs gl x11)
endif

HAVE_CAIRO_OR_OPENGL = true

endif

# ---------------------------------------------------------------------------------------------------------------------
# Set Stub specific stuff

ifeq ($(HAIKU_OR_MACOS_OR_WINDOWS),true)
HAVE_STUB = true
else
HAVE_STUB = $(HAVE_X11)
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set Vulkan specific stuff

ifeq ($(HAVE_VULKAN),true)

DGL_FLAGS   += -DHAVE_VULKAN

VULKAN_FLAGS  = $(shell $(PKG_CONFIG) --cflags vulkan)
VULKAN_LIBS   = $(shell $(PKG_CONFIG) --libs vulkan)

ifneq ($(WINDOWS),true)
VULKAN_LIBS  += -ldl
endif

endif

# ---------------------------------------------------------------------------------------------------------------------
# Set optional libraries specific stuff

ifeq ($(HAVE_JACK),true)
JACK_FLAGS   = $(shell $(PKG_CONFIG) --cflags jack)
JACK_LIBS    = $(shell $(PKG_CONFIG) --libs jack)
endif

ifeq ($(HAVE_LIBLO),true)
LIBLO_FLAGS  = $(shell $(PKG_CONFIG) --cflags liblo)
LIBLO_LIBS   = $(shell $(PKG_CONFIG) --libs liblo)
endif

# ---------------------------------------------------------------------------------------------------------------------
# Backwards-compatible HAVE_DGL

ifeq ($(MACOS_OR_WINDOWS),true)
HAVE_DGL = true
else ifeq ($(HAVE_OPENGL),true)
ifeq ($(HAIKU),true)
HAVE_DGL = true
else
HAVE_DGL = $(HAVE_X11)
endif
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set app extension

ifeq ($(WINDOWS),true)
APP_EXT = .exe
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set shared lib extension

LIB_EXT = .so

ifeq ($(MACOS),true)
LIB_EXT = .dylib
endif

ifeq ($(WINDOWS),true)
LIB_EXT = .dll
endif

# ---------------------------------------------------------------------------------------------------------------------
# Set shared library CLI arg

ifeq ($(MACOS),true)
SHARED = -dynamiclib
else
SHARED = -shared
endif

# ---------------------------------------------------------------------------------------------------------------------
# Handle the verbosity switch

ifeq ($(VERBOSE),true)
SILENT =
else
SILENT = @
endif

# ---------------------------------------------------------------------------------------------------------------------
