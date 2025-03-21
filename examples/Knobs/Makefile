#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = d_knobs

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	KnobsPlugin.cpp

FILES_UI = \
	KnobsUI.cpp \
	NanoButton.cpp

# --------------------------------------------------------------
# Do some magic

USE_FILE_BROWSER = true
include ../../Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_OPENGL),true)
TARGETS += jack
# TARGETS += lv2_sep
else
# TARGETS += lv2_dsp
endif

TARGETS += vst2
# TARGETS += vst3

all: $(TARGETS)

# --------------------------------------------------------------
