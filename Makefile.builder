include ../../common.mk

CFLAGS= -O2 -Wall -I. -I$(ROOTDIR)/include \
	-I$(ROOTDIR)/$(FSDIR)/include

LDFLAGS= -L$(ROOTDIR)/$(FSDIR)/lib

all: config
	$(MAKE)

config: configure
	if [ ! -f Makefile ]; then \
		export CFLAGS="$(CFLAGS)"; \
		export LDFLAGS="$(LDFLAGS)"; \
		./configure \
		--host=powerpc-linux \
		--build='i386' \
		--prefix=$(ROOTDIR)/fs; \
	fi

configure:
	autoreconf -i

install:
	if [ ! -f Makefile ]; then \
		$(MAKE) install; \
	fi

clean:
	if [ ! -f Makefile ]; then \
		$(MAKE) clean; \
	fi

distclean:
	if [ ! -f Makefile ]; then \
		$(MAKE) distclean; \
	fi
