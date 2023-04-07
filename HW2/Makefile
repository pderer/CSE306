all: comp decomp

comp: main.c lib_fastlz/fastlz.c
	gcc main.c lib_fastlz/fastlz.c -o comp -DCOMP

decomp: main.c lib_fastlz/fastlz.c
	gcc main.c lib_fastlz/fastlz.c -o decomp -DDECOMP

clean:
	rm -rf *comp
