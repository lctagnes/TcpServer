pwd=${shell pwd}
src_dir=$(pwd)/src
include_dir=$(pwd)/include/
tool_dir=$(pwd)/tools/
test_dir=$(pwd)/test/

out_dir=.
target=TcpServer



########################## src section #################################
obj_src_dir=$(src_dir)
obj_src=${shell find $(obj_src_dir) -name "*.cpp"}
objs=${patsubst %.cpp,%.o,$(obj_src)}
deps=${patsubst %.cpp,%.d,$(obj_src)}


#########################################################################

########################## tools #######################################

########################################################################

####################### flag section ########################################
VER ?= 0.1

OF=-g
CFLAGS=-I${include_dir}

CC=g++

COMMIT_ID ?= ${shell git rev-parse --short HEAD}


libs=
LDFLAGS=

#############################################################################


.PHONY: all
all: $(target)
$(target): $(objs)
	$(CC) $(CFLAGS)  -o $@ $(objs)

%.o:%.cpp
	$(CC) ${CFLAGS} -c -o $@ $<


clean:
	-rm $(target)
	-rm $(objs)