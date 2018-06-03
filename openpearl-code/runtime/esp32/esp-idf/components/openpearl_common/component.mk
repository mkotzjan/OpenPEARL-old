#
# Component Makefile
#
# Adds platform independet PEARL classes
#

COMMON_OBJS := \
	Array.o \
	Bolt.o \
	LogFile.o\
	Log.o \
	Clock.o  \
	Duration.o  \
	PutDuration.o GetDuration.o \
	Signals.o \
	ScheduleSignalAction.o \
	Fixed63.o \
	Fixed.o \
	Prio.o \
	CharSlice.o \
	Character.o RefChar.o \
        compare.o \
	Interrupt.o \
	Source.o \
	RefCharSink.o RefCharSource.o \
	SystemDationNBSource.o SystemDationNBSink.o \
	PutClock.o GetClock.o \
	PutBitString.o \
	GetHelper.o PutHelper.o \
	TaskWhenLinks.o \
	TaskCommon.o \
	TaskList.o \
	TaskMonitor.o \
	MutexCommon.o CSemaCommon.o \
	Semaphore.o PriorityQueue.o \
	Rst.o \
	SystemDation.o \
	SystemDationNB.o \
	DationRW.o \
	IOFormats.o \
	IOJob.o \
	DationPG.o \
	StringDationConvert.o \
	DationTS.o \
	UserDationNB.o UserDation.o \
	TFUBuffer.o \
	DationDim.o  DationDim1.o \
	DationDim2.o DationDim3.o \
	FloatHelper.o \
	SoftInt.o \
	Control.o \
	ConsoleCommon.o \
	SampleBasicDation.o

COMPONENT_OBJS := $(addprefix ../../../../common/,$(COMMON_OBJS))

COMPONENT_SRCDIRS := ../../../../common
COMPONENT_ADD_INCLUDEDIRS := ../../../../common
CXXFLAGS += -frtti
