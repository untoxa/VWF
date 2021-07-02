GBDK = ../../../gbdk
OBJ = obj
CC = $(GBDK)/bin/lcc -Wl-j -Wm-yS -tempdir=$(OBJ)
ASMINC = $(GBDK)/lib/small/asxxxx 

CFLAGS = -Wa-I$(ASMINC)

#RELEASE = 1
#DEBUG = 1

ifdef RELEASE
CFLAGS += -Wf'--max-allocs-per-node 50000'
endif

ifdef DEBUG
CFLAGS += -Wf--debug -Wf--nolospre -Wl-m -Wl-w -Wl-y
endif


TARGET = rom.gb

SRC = $(foreach dir,src,$(wildcard $(dir)/*.c)) 
ASRC = $(foreach dir,src,$(wildcard $(dir)/*.s)) 
DRVOBJ = $(foreach dir,src,$(wildcard player/*.obj.o)) 

all:	clean prepare rom

%.gb:
	$(CC) $(CFLAGS) -o $@ $(SRC) $(ASRC) $(DRVOBJ)

clean:
	rm -rf $(patsubst %.gb,%.*,$(TARGET))

prepare:
	mkdir -p $(OBJ)

rom: $(TARGET)
	@echo "DONE!"
