CFLAGS ?= -Wall -Werror -Wextra -pedantic

sources := bufdis.c

bufdis: $(sources)

.PHONY: check
check:
	! include-what-you-use $(sources) | awk 'NF' | grep -v 'has correct #includes'

.PHONY: clean
clean:
	$(RM) bufdis

