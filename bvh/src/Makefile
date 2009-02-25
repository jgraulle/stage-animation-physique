PREFIX= $(HOME)/local
PROJECT= libbvh

CC= gcc 
LD= gcc 

CFLAGS=  -Wall -pipe -fPIC
LDFLAGS= 

LIB= -lm 

SRC=motion.c array.c \
	bvh.c parser.c
	
OBJ= $(SRC:.c=.o)
DEP= $(SRC:.c=.d)

# numero de version auto
include build

SO= liblibbvh.so

.PHONY: all
all: $(SO)

build:
	echo BUILD= 1 > build

rebuild: build
	@echo BUILD= `expr $(BUILD) + 1` > build

liblibbvh.so: $(OBJ)
	@echo .... build $(BUILD)
	$(LD) $(LDFLAGS) -shared -o $@ $+ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

%.d: %.c
	$(CC) $(CFLAGS) -o $@ -MM $<

.PHONY: clean
clean: rebuild
	rm -f $(SO) $(OBJ) $(DEP)

.PHONY: tarball
tarball: 
	@echo bvh_b$(BUILD).tar.gz
	tar --ignore-failed-read -zcf $(PROJECT)_b$(BUILD).tar.gz Makefile build *.c *.h 
	@rm -f $(PROJECT)_latest.tar.gz
	@ln -s $(PROJECT)_b$(BUILD).tar.gz $(PROJECT)_latest.tar.gz

.PHONY: install
install: $(SO)
	mkdir -p $(PREFIX)/lib
	cp $(SO) $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp *.h $(PREFIX)/include

ifneq ($(MAKECMDGOALS), clean)
-include $(DEP)
endif
