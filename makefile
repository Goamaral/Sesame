all:
	g++ -std=c++1y sesame.cpp -o sesame

clean: 
	$(RM) sesame
