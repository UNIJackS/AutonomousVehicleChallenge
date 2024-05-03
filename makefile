f1: f1.o 
        c++ -o f1 f1.o 
f1.o: f1.cpp 
        c++ -c main.cpp 