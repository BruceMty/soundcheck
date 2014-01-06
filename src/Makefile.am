
bin_PROGRAMS = soundcheck test main

AM_CPPFLAGS = 
AM_LDFLAGS = -static

SOUNDCHECK_INCS = \
	libfft.cpp \
	libfft.h \
	soundcheck.cpp

MAIN_INCS = \
	libfft.cpp \
	libfft.h \
	main.cpp

soundcheck_SOURCES = \
	$(SOUNDCHECK_INCS) portaudio.h

main_SOURCES = \
	$(MAIN_INCS) portaudio.h

test_SOURCES = \
	test.cpp
