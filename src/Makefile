CC = g++
AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: dirs $(LIB_DIR)/libCTclApp.a

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../lib ]; then mkdir ../lib; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

SRC = \
CTclApp.cpp \
CTclAppCommand.cpp \
CTclAppCanvas.cpp \
CTclCanvas2D.cpp \

TCL_FILES = \
CTclAppProcs.tcl

K_FILES = $(patsubst %.tcl,%.k,$(TCL_FILES))

$(K_FILES): %.k: %.tcl
	echo "Preprocessing $<"
	my_tk_to_k $<

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++14 \
$(CDEBUG) \
-I. \
-I$(INC_DIR) \
-I../../CXLib/include \
-I../../CRenderer/tcl_include \
-I../../CRenderer/include \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CReadLine/include \
-I../../CMath/include \
-I../../CFileUtil/include \
-I../../CFile/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I/usr/include/tcl \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCTclApp.a

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(LIB_DIR)/libCTclApp.a: $(K_FILES) $(OBJS)
	$(AR) crv $(LIB_DIR)/libCTclApp.a $(OBJS)
