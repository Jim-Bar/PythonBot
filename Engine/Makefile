EXEC=pythonbot_core
TARGETS=Bot.o BotConnector.o Bullet.o Circle.o Controller.o main.o Model.o Object.o Scan.o LocalView.o RemoteView.o
FLAGS=-Wall -Wextra -O3
LIBS=-lsfml-system -lsfml-window -lsfml-graphics -lsfml-network

all: $(EXEC)

$(EXEC): $(TARGETS)
	g++ -o $(EXEC) $^ $(LIBS)

main.o: main.cpp
	g++ -c $(FLAGS) $^

%.o: %.cpp %.h
	g++ -c $(FLAGS) $<

clean:
	rm -f *.o

mrproper: clean
	rm -f $(EXEC) *.pyc Bots/*.pyc

