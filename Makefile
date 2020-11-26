CFLAGS ?= -Wall -Werror -Wextra -pedantic

sources := bufdis.c

all: bufdis

bufdis: $(sources)

.PHONY: check
check:
	! include-what-you-use $(sources) | awk 'NF' | grep -v 'has correct #includes'

.PHONY: clean
clean:
	$(RM) bufdis

.PHONY: install
install: bufdis
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m +x bufdis $(DESTDIR)$(PREFIX)/bin

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/bufdis

