OBJ=vyper.pb.o generate.o

main: $(OBJ)
	clang++ -g $(OBJ) -Lfmt-10.1.0/build -lprotobuf -o generate -lfmt

generate.o: generate.cc
	clang++ -g -c generate.cc -Ifmt-10.1.0/include

vyper.pb.o: proto/vyper.pb.cc proto/vyper.pb.h
	clang++ -g -c proto/vyper.pb.cc

clean:
	rm -rf *.o
