# for testing
build-test:
	g++ -O2 -Wall -o test.out test.cpp
test: build-test
	./test.out

# build
seed=1
build:
	g++ -O2 -Wall CrossStitch.cpp
run: build
	java -jar ./tester.jar -exec ./a.out -seed $(seed) -novis

# example
example: build
	sh ./example.sh
