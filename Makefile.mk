

TARGET_APP          :=jiamu
TARGET_LIB          :=muos

CORSS_COMPILE		:=
CC 					:=$(CORSS_COMPILE)gcc
AR					:=$(CORSS_COMPILE)ar
LD					:=$(CORSS_COMPILE)ld
STRIP				:=$(CORSS_COMPILE)strip
CFLAGS 				:=-Wall -Wall -Wstrict-prototypes -Wwrite-strings -Wshadow \
						-Winit-self -Wcast-align -Wformat=2  -Wundef -Wswitch-default
LDFLAGS 			:=-lpthread -lm -lrt