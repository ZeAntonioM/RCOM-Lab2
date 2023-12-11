CC = gcc
CFLAGS = -Wall

SRC = src
INCLUDE = include
BIN = bin

URL = ftp://rcom:rcom@netlab1.fe.up.pt/files/pic1.jpg

# ftp://netlab1.fe.up.pt/pub.txt
# ftp://rcom:rcom@netlab1.fe.up.pt/pipe.txt
# ftp://rcom:rcom@netlab1.fe.up.pt/files/pic1.jpg
# ftp://rcom:rcom@netlab1.fe.up.pt/files/pic2.png
# ftp://rcom:rcom@netlab1.fe.up.pt/files/crab.mp4
# ftp://ftp.up.pt/pub/kodi/timestamp.txt

# Targets
.PHONY: all
all: $(BIN)/download

$(BIN)/download:  $(SRC)/download.c
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE)

.PHONY: run
run: $(BIN)/download
	./$< $(URL)

.PHONY: clean
clean:
	rm -f $(BIN)/download
