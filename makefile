all:sampler read_image

sampler:
	cc sampler.c -o sampler

read_image:proc.h
	cc rdjpeg.c read_image.c proc.c -o read_image

clean:
	rm read_image sampler