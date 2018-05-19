CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: dirs $(BIN_DIR)/test_tcl_image

#all: dirs $(BIN_DIR)/test_tcl_image $(BIN_DIR)/test_tcl_canvas 

dirs:
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

SRC1 = \
test_tcl_image.cpp \

SRC2 = \
test_tcl_canvas.cpp \

TCL_FILES = \
test_tcl_image.tcl \
test_tcl_canvas.tcl \

K_FILES = $(patsubst %.tcl,%.k,$(TCL_FILES))

$(K_FILES): %.k: %.tcl
	echo "Preprocessing $<"
	my_tk_to_k $<

OBJS1 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC1))

OBJS2 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC2))

CPPFLAGS = \
$(CDEBUG) \
-std=c++14 \
-I. \
-I$(INC_DIR) \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I/usr/include/tcl \

LFLAGS = \
$(LDEBUG) \
-L$(LIB_DIR) \
-L../../CXLib/lib \
-L../../CRenderer/lib \
-L../../CConfig/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CTimer/lib \
-L../../CMath/lib \
-L../../CReadLine/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-L../../CUtil/lib \
-L../../COS/lib \

LIBS = \
-lCTclApp \
-lCXLib \
-lCXRenderer \
-lCRenderer \
-lCConfig \
-lCImageLib \
-lCFont \
-lCTimer \
-lCReadLine \
-lCFileUtil \
-lCFile \
-lCMath \
-lCRegExp \
-lCStrUtil \
-lCUtil \
-lCOS \
-lreadline -ltk -ltcl \
-lXt -lX11 -lfreetype -lpng -ljpeg -ltre

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/test_tcl_image

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(OBJ_DIR)/test_tcl_image.o: test_tcl_image.cpp
	$(CC) -c test_tcl_image.cpp -o $(OBJ_DIR)/test_tcl_image.o $(CPPFLAGS)

$(BIN_DIR)/test_tcl_image: $(K_FILES) $(OBJS1) $(LIB_DIR)/libCTclApp.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/test_tcl_image $(OBJS1) $(LFLAGS) $(LIBS)

$(OBJ_DIR)/test_tcl_canvas.o: test_tcl_canvas.cpp
	$(CC) -c test_tcl_canvas.cpp -o $(OBJ_DIR)/test_tcl_canvas.o $(CPPFLAGS)

$(BIN_DIR)/test_tcl_canvas: $(K_FILES) $(OBJS2) $(LIB_DIR)/libCTclApp.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/test_tcl_canvas $(OBJS2) $(LFLAGS) $(LIBS)
