CFLAGS=-Wall -Werror -Wextra -pedantic

bufdis: bufdis.c

.PHONY: clean
clean:
	$(RM) bufdis

