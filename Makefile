build:
	g++ -O2 -Wall CrossStitch.cpp
test:
	java -jar ./tester.jar -exec ./a.out -seed 1 -novis
