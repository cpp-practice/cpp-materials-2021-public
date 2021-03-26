
myglobals_o: 03_myglobals.cpp
	g++ -c 03_myglobals.cpp -o 03_myglobals.o



run_01_printf: 01_printf.cpp
	gcc 01_printf.cpp
	du -h ./a.out; echo
	sudo strace ./a.out

run_01_printf_static: 01_printf.cpp
	gcc -static 01_printf.cpp
	du -h ./a.out; echo
	sudo strace ./a.out



_myglobals_o_pic: 03_myglobals.cpp
	g++ -c 03_myglobals.cpp -fPIC -o 03_myglobals_pic.o  # PIC for Position Independent Code

myglobals_so: _myglobals_o_pic
	g++ -shared -o libmyglobals.so 03_myglobals_pic.o

run_myglobals_explicit_usage: myglobals_so
	g++ 04_use_myglobals_explicit.cpp -rdynamic -ldl
	LD_LIBRARY_PATH="." ./a.out

run_myglobals_implicit_usage: myglobals_so
	g++ -L"." 05_use_myglobals_implicit.cpp -lmyglobals
	LD_LIBRARY_PATH="." ./a.out



clean:
	rm -i ./*.o ./*.so
