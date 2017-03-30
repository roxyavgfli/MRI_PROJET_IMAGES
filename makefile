all: proc.h
	cc rdjpeg.c read_image.c proc.c -o read_image
