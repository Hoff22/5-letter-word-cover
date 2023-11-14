all:
	g++ -std=c++20 $(f).cpp -o $(f) -O2 -Wall -Wextra -Wshadow -Wno-unused-result -Wno-sign-compare -fsanitize=undefined -fno-sanitize-recover