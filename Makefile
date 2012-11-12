OBJs := $(patsubst %.cpp, %.o, $(wildcard *.cpp))
BIN := main

CC := g++

CPPFLAGS += -I. 
#-I./nmealib/include/nmea

# All warnings
CPPFLAGS += -Wall

# Use a modern language
CPPFLAGS += --std=c++0x

# Use boost libraries
LDFLAGS += -L./libs -lpthread -lboost_program_options -lserial -lnmea

# Put debugging info in there so we can get stack traces.
#CPPFLAGS += -g -rdynamic

# Optimize the code.
CPPFLAGS += -O3

# Fix http://gcc.gnu.org/bugzilla/show_bug.cgi?id=42748
CPPFLAGS += -Wno-psabi

# Generate .d files with dependency info
CPPFLAGS += -MD -MP

all: vector.h.gch libs $(BIN)

$(BIN) : $(OBJs)
	$(CC)  $(OBJs) -o $(BIN) $(LDFLAGS)

DEPs := $(OBJs:%.o=%.d)

vector.h.gch: vector.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@


.PHONY: libs
libs:
	@echo "Building NMEA-Library...."
	@cd nmealib; make
	@cp nmealib/lib/libnmea.a libs
	@echo "done"

	
.PHONY: clean
clean:
	@rm -fv $(BIN) $(OBJs) $(DEPs) *.o *.gch *.d
	@rm -fr docs


.PHONY: libs-clean
libs-clean:
	@echo "Cleaning nmealib..."
	@cd nmealib; make clean;
	@rm libs/libnmea*
	@echo done	

	
.PHONY: dist-clean
dist-clean: clean libs-clean	

	
.PHONY: docs
docs:
	doxygen

prefix = $(DESTDIR)/usr
bindir = $(prefix)/bin
sharedir = $(prefix)/share
mandir = $(sharedir)/man
man1dir = $(mandir)/man1

.PHONY: install
install: $(BIN)
	install $(BIN) $(bindir)
	install $(BIN)-calibrate $(bindir)
	install $(BIN)-calibrator $(bindir)
	install -m 0644 $(BIN).1 $(man1dir)
	install -m 0644 $(BIN)-calibrate.1 $(man1dir)
	install -m 0644 $(BIN)-calibrator.1 $(DESTDIR)$(man1dir)

-include $(DEPs) vector.h.d
