#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

include ../../../configuration/.config

PROJECT_NAME := app-template

ifeq ($(CONFIG_ESP32_CHECK_STACK_OVERFLOW),y)
EXTRA_CXXFLAGS += -finstrument-functions 
endif

include ../esp-idf/make/project.mk

