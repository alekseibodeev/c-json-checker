all: test

test: test.c json.c json.h string_view.h scanner.h
	gcc -Wall -Werror -o test test.c json.c

clean:
	rm test
