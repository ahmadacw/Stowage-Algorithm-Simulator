COMP = g++-9.3.0

SHARED_OBJ1 = _314636663_a.so 
SHARED_OBJ2 = _314636663_b.so
OBJ1=  _314636663_a.o 
OBJ2=  _314636663_b.o
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden

CPP_LINK_FLAG = -shared

all: $(SHARED_OBJ1) $(SHARED_OBJ2)




_314636663_a.so: $(OBJ1)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^


_314636663_a.o: _314636663_a.cpp 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 

_314636663_b.so: $(OBJ2)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_314636663_b.o: _314636663_b.cpp 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 


clean:
	rm -f $(OBJ1) $(SHARED_OBJ1) $(OBJ2) $(SHARED_OBJ2)

