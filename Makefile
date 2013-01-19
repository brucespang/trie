all: compile run

compile:
	gcc -I/usr/local/Cellar/glib/2.32.4/include/glib-2.0 -I/usr/local/Cellar/glib/2.32.4/lib/glib-2.0/include -I/usr/local/Cellar/gettext/0.18.1.1/include -L/usr/local/Cellar/glib/2.32.4/lib -L/usr/local/Cellar/gettext/0.18.1.1/lib -lglib-2.0 -lintl -Wall -Wextra -g compressed.c

run:
	time ./a.out /usr/share/dict/words
