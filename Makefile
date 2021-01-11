CXX = g++
LCXXFLAGS = -Wall -Wextra $(CXXFLAGS)
# -O3 -march=native -DNDEBUG
LLDFLAGS = -Wall -Wextra $(LDFLAGS)

EXE_TEST = test.x
OBJS_TEST = test.o

EXE_PROF = profile.x
OBJS_PROF = profile.o

default: $(EXE_TEST) $(EXE_PROF)
.PHONY: default

clean:
	rm -f $(EXE_TEST) $(OBJS_TEST) $(EXE_PROF) $(OBJS_PROF)
.PHONY: clean

$(EXE_TEST): $(OBJS_TEST)
	$(CXX) $^ -o $@ $(LLDFLAGS)
	
$(EXE_PROF): $(OBJS_PROF)
	$(CXX) $^ -o $@ $(LLDFLAGS)
	
%.o: %.cpp
	$(CXX) -c $< -o $@ $(LCXXFLAGS)

test.o: test.cpp
profile.o: profile.cpp
