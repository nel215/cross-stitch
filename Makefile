# for testing
build-test:
	g++ -O2 -Wall -o test.out test.cpp
test: build-test
	./test.out

# profile
pbuild:
	g++ -O2 -Wall -pg -g CrossStitch.cpp
profile: pbuild
	java -jar ./tester.jar -exec ./a.out -backOnly -seed 2


# build
seed=1
build:
	g++ -O2 -Wall CrossStitch.cpp
run: build
	java -jar ./tester.jar -exec ./a.out -seed $(seed) -backOnly

# parallel
n_test=10
parallel: build
	python parallel.py -n $(n_test) && python gen-report.py
