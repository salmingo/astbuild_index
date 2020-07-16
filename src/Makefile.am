bin_PROGRAMS=astbuild_index
astbuild_index_SOURCES=bl.cpp ucac4api.cpp kdtree.cpp codetree.cpp \
                       ATimeSpace.cpp \
                       index.cpp build_index.cpp astbuild_index.cpp

if DEBUG
  AM_CFLAGS = -g3 -O0 -Wall -DNDEBUG
  AM_CXXFLAGS = -g3 -O0 -Wall -DNDEBUG
else
  AM_CFLAGS = -O3 -Wall
  AM_CXXFLAGS = -O3 -Wall
endif

astbuild_index_LDADD = -lm -lcfitsio
