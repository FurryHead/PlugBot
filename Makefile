CPP=g++
#windows use this line
#RM=del /Q
RM=rm -f
SRCS=$(wildcard src/*.cxx)
OBJS=$(patsubst src/%.cxx,obj/%.o,$(SRCS))
CFLAGS=`Sockets-config`
LFLAGS=-lSockets -lssl -lpthread -llua

OUT=plugbot

all: $(OUT)
	echo "Finished compiling"

$(OUT) : $(OBJS)
	echo "Linking..."
	$(CPP) $(OBJS) $(LFLAGS) -o $(OUT)

obj/%.o : src/%.cxx
	echo "Compiling $< to $@..."
	$(CPP) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) $(OUT)
	
.SILENT:
