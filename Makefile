all: jsonparser.c
	gcc jsonparser.c -o jsonparser

clean:
	rm -rf jsonparser *~

test:
	./jsonparser valid.json
	./jsonparser invalid.json
