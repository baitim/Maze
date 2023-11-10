FILES_LIFE = Life.c
FILES_LAB  = Lab.c

LAB = lab.txt

build : life gen lab search

life : $(FILES_LIFE)
	gcc $(FILES_LIFE) -o life

gen :
	./life

lab : $(FILES_LAB)
	gcc $(FILES_LAB) -o lab

search :
	./lab $(LAB)