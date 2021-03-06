CC = clang++
CC = g++

OPENCV_DIR   ?= /opt/local

MYFLAGS      ?= -D WIDTH=2048 -D HEIGHT=2048 -D SIZE_X=5 -D SIZE_Y=5 -D OpenCV
CFLAGS        = $(MYFLAGS) -Wall -Wunused \
                -I/scratch-local/usr/include/dsl \
                -I$(OPENCV_DIR)/include
LDFLAGS       = -lm \
                -L$(OPENCV_DIR)/lib -lopencv_core -lopencv_gpu -lopencv_imgproc
OFLAGS        = -O3

ifeq ($(CC),clang++)
    # use libc++ for clang++
    CFLAGS   += -std=c++11 -stdlib=libc++ \
                -I`/scratch-local/usr/bin/clang -print-file-name=include` \
                -I`/scratch-local/usr/bin/llvm-config --includedir` \
                -I`/scratch-local/usr/bin/llvm-config --includedir`/c++/v1
    LDFLAGS  += -L`/scratch-local/usr/bin/llvm-config --libdir` -lc++
else
    CFLAGS   += -std=c++11
    LDFLAGS  += -lstdc++
endif


BINARY = test
BINDIR = bin
OBJDIR = obj
SOURCES = $(shell echo *.cpp)

OBJS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)
BIN = $(BINDIR)/$(BINARY)


all: $(BINARY)

$(BINARY): $(OBJS) $(BINDIR)
	$(CC) -o $(BINDIR)/$@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp $(OBJDIR)
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ -c $<

$(BINDIR):
	mkdir bin

$(OBJDIR):
	mkdir obj


clean:
	rm -f $(BIN) $(OBJS)
	@echo "all cleaned up!"

distclean: clean
	rm -rf $(BINDIR) $(OBJDIR)

run: $(BINARY)
	$(BIN)

