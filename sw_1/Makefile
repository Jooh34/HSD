CC = gcc
make: app
	./app
debug: app
	./app -d
app: main.c
	gcc main.c -o app
clean : app app.output.txt app.input.txt
	rm app app.output.txt app.input.txt

