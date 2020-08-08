all : comecacas

comecacas : comecacas.c sprites.c sprites.h
	gcc -Wall $(filter %.c, $^) -o $@ \
		-lallegro -lallegro_image -lallegro_ttf -lallegro_font -lallegro_primitives -lm

clean :
	rm -f comecacas
