all:
	g++ -std=c++1y sesame.cpp -o sesame

clean: 
	$(RM) sesame

install:
	sudo mkdir /usr/share/sesame
	sudo cp sesame /usr/share/sesame/sesame 
	sudo ln -s /usr/share/sesame/sesame /usr/bin/sesame
	sudo chmod 777 -R /usr/share/sesame

remove:
	sudo rm -R /usr/share/sesame
	sudo rm /usr/bin/sesame