CFLAGS ?= -Wall -Werror -Wextra -pedantic

SRC = bufdis.c

all: bufdis

bufdis: $(SRC)

check:
	! include-what-you-use $(SRC) | awk 'NF' | grep -v 'has correct #includes'
clean:
	$(RM) bufdis

install: bufdis
	install -d $(DESTDIR)$(prefix)/bin
	install -m +x bufdis $(DESTDIR)$(prefix)/bin

uninstall:
	rm -f $(DESTDIR)$(prefix)/bin/bufdis

.PHONY: all check clean install uninstall
