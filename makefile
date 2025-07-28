bin_folder='./bin'

executable ?=PATH

ex:
	gcc main.c -o $(bin_folder)/test.o

release:
	gcc main.c -o $(PATH)/tomato_in_line
