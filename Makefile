all: calculator user

calculator: Calculator.c shared_mem.h
	gcc -o calculator Calculator.c

user: User.c shared_mem.h
	gcc -o user User.c
