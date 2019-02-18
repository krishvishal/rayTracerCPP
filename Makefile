OBJ = main.o
INC = -I "./"

RayTracing: $(OBJ)
	g++ $(OBJ) -o RayTracing
	rm -f $(OBJ)

main.o:
	g++ -c main.cpp $(INC)

clean:
	rm -f $(OBJ) RayTracing