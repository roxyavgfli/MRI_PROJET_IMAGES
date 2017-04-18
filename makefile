all:sampler read_image

sampler:
	gcc -D_GNU_SOURCE sampler.c -o sampler

read_image:proc.h
	gcc -D_GNU_SOURCE rdjpeg.c read_image.c proc.c -o read_image

clean:
	rm read_image sampler
