all: checker

checker: main.c json.c json.h string_view.h scanner.h read_file.c read_file.h
	gcc -Wall -Werror -o json-checker main.c json.c read_file.c

test: test.c json.c json.h string_view.h scanner.h
	gcc -Wall -Werror -o test test.c json.c

clean:
	rm -f json-checker test
