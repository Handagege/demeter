include make.inc

SRC_UTILITY_PATH = ./utility/
SRC_EXTRACT_PATH = ./extract/
SRC_SERVICE_PATH = ./service/

OBJS = $(SRC_UTILITY_PATH)/StringArray.o
OBJS += $(SRC_UTILITY_PATH)/Config.o
OBJS += $(SRC_UTILITY_PATH)/Timer.o
OBJS += $(SRC_UTILITY_PATH)/TimeFmt.o
OBJS += $(SRC_UTILITY_PATH)/Random.o
OBJS += $(SRC_EXTRACT_PATH)/WsPair.o
OBJS += $(SRC_EXTRACT_PATH)/StringFilter.o
OBJS += $(SRC_EXTRACT_PATH)/WordSpliter.o
OBJS += $(SRC_EXTRACT_PATH)/InfoWordExtr.o
OBJS += $(SRC_EXTRACT_PATH)/KeyWordExtr.o
OBJS += $(SRC_EXTRACT_PATH)/ExtendWordExtr.o
OBJS += $(SRC_EXTRACT_PATH)/TopicReco.o
OBJS += $(SRC_EXTRACT_PATH)/TextAnalyze.o

SERV_OBJS = $(SRC_SERVICE_PATH)/Ether.o 
SERV_OBJS += $(SRC_SERVICE_PATH)/ServConf.o 
SERV_OBJS += $(SRC_SERVICE_PATH)/JsonTransf.o 
SERV_OBJS += $(SRC_SERVICE_PATH)/ThreadRWLock.o 
SERV_OBJS += $(SRC_SERVICE_PATH)/Worker.o 

LIB_OUT_PATH = ../lib/
LIB_OUT = $(LIB_OUT_PATH)/libdemeter.a
INC_OUT_PATH = ../include/

BIN_PATH = ../bin/


BIN = $(BIN_PATH)/demeter_serv
BIN += $(BIN_PATH)/demeter_cli


.PHONY: all

all: $(LIB_OUT) $(BIN)

$(LIB_OUT): $(OBJS)
	mkdir -p $(LIB_OUT_PATH)
	ar -rv $@ $(OBJS)
	mkdir -p $(INC_OUT_PATH)/utility/
	cp $(SRC_UTILITY_PATH)/*.h $(INC_OUT_PATH)/utility/
	mkdir -p $(INC_OUT_PATH)/extract/
	cp $(SRC_EXTRACT_PATH)/*.h $(INC_OUT_PATH)/extract/
	cp ./*.h $(INC_OUT_PATH)
	
$(BIN_PATH)/demeter_serv: $(SRC_SERVICE_PATH)/demeter_serv.o $(SERV_OBJS)
	$(CXX) $(CFLAGS) -o $@ $< $(SERV_OBJS) -L$(LIB_OUT_PATH) -L../depend/woo/lib/ -L../depend/jsoncpp/lib/ -ldemeter -lwoo -ljsoncpp -pthread

$(BIN_PATH)/demeter_cli: $(SRC_SERVICE_PATH)/demeter_cli.o $(SERV_OBJS)
	$(CXX) $(CFLAGS) -o $@ $< $(SERV_OBJS) -L$(LIB_OUT_PATH) -L../depend/woo/lib/ -L../depend/jsoncpp/lib/ -ldemeter -lwoo -ljsoncpp -pthread


.SUFFIXES: .o .cpp .h

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $< -I$(SRC_UTILITY_PATH) -I$(SRC_EXTRACT_PATH) -I$(INC_OUT_PATH) -I../depend/woo/include/ -I../depend/jsoncpp/include/

.PHONY: clean

clean: 
	rm -rf $(LIB_OUT_PATH)
	rm -rf $(INC_OUT_PATH)
	rm -rf $(OBJS)
	rm -rf $(SERV_OBJS)
	rm -rf $(SRC_SERVICE_PATH)/*.o
	rm -rf *.o
	rm -rf $(BIN)

