#
#


DEBUGIT=1

PRJNAME=libtmb++

SRCDIR=tmb
BUILDDIR=build
LIBFILE=$(BUILDDIR)/libtmb++.a
INCPATH=-I. -I/usr/include/libusb-1.0

FEATURES:=

ifeq ($(DEBUGIT),1)
 DEBUGFLAGS= -g3 -DDEBUG=1
 OPTIMIZE=
else
 DEBUGFLAGS= -g
 OPTIMIZE= -O3 -funroll-loops
endif

DEFS= -D_REENTRANT=1 -D_GNU_SOURCE $(FEATURES)

export CFLAGS= $(DEBUGFLAGS) $(DEFS) -Wall $(OPTIMIZE)


export CC=gcc
export CXX=g++

SRCFILES:=$(wildcard $(SRCDIR)/*/*.cpp)
INCFILES:=$(wildcard $(SRCDIR)/*/*.h)
OBJFILES:=$(addprefix $(BUILDDIR)/,$(SRCFILES:.cpp=.o))

.PHONY: all release debug clean

all: $(LIBFILE)
	@echo "--- $(PRJNAME) build done ---"
debug:
	make DEBUGIT=1 BUILDDIR=debug
release:
	make DEBUGIT=0 BUILDDIR=release


$(LIBFILE): $(OBJFILES)
	/bin/rm -f $(LIBFILE)
	ar r $(LIBFILE) $(OBJFILES)
	ranlib $(LIBFILE)
	if [ -n "$(BALAGI_SRC)" -a -d "$(BALAGI_SRC)" ] ; then \
		/bin/cp -p "$(LIBFILE)" "$(BALAGI_SRC)/lib/"; \
		/bin/cp -p --parent $(INCFILES) "$(BALAGI_SRC)/include/"; \
	fi

$(OBJFILES): $(BUILDDIR)/%.o: %.cpp
	@/bin/mkdir -p "$$(dirname $@)"
	$(CXX) $(CFLAGS) $(INCPATH) -o "$@" -c "$<"


clean:
	/bin/rm -rf build debug release
	@echo "--- $(PRJNAME) clean done ---"


include $(BUILDDIR)/depend

$(BUILDDIR)/depend: $(INCFILES) $(SRCFILES)
	@/bin/mkdir -p $(BUILDDIR)
	./makedep -objdir $(BUILDDIR) -gxxopts "$(CFLAGS) $(INCPATH)" $(SRCFILES) >$(BUILDDIR)/depend
