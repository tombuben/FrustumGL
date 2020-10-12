# C++ compiler to run
CC = g++

# Flags for compiling
CFLAGS = -Wall -std=gnu++17 -O3

# Includes (using pkg-config)
INCLUDES = $(shell pkg-config --cflags sdl2 gl glew glm)

# Libraries (using pkg-config)
LIBS = -lm -lstdc++fs $(shell pkg-config --libs sdl2 gl glew glm)

# object file directory
ODIR = obj

# Define C++ source files
SRCS = AABB.cpp BVHNode.cpp cCamera.cpp Frustum.cpp main.cpp Mesh.cpp ObjLoader.cpp RenderObject.cpp SceneLoader.cpp ShaderProgram.cpp SplitMesh.cpp


# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)

# define the executable file 
MAIN = FrustumGL

.PHONY: depend clean


all:    $(MAIN)
	@echo Finished compiling

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^