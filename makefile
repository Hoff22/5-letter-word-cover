all: 
	g++ -std=c++20 $(f).cpp trie.cpp -o $(f) -O3 -fopenmp -Wall -Wextra -Wshadow -Wno-unused-result -Wno-sign-compare -fsanitize=undefined -fno-sanitize-recover
clean:
	rm -f $(f) $(wildcard *.o) $(f)