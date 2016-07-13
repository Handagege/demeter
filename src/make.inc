DEBUG = 0
CXX = g++
ifneq ($(DEBUG),1)
	CFLAGS = -Wall -W -fpic
else
	CFLAGS = -DDEBUG -g -Wall -W -fpic
endif
