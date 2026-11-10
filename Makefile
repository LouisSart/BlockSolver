EPICUBE_DIR = extern/EpiCube
333_OBJ = $(EPICUBE_DIR)/obj/333.o
CFLAGS = -Wall
STD = -std=c++20
INC = -I$(EPICUBE_DIR)/333
OPT = -O1 
OBJ_DIR = obj

block: obj $(EPICUBE_DIR)/obj/333.o
	$(CXX) $(STD) $(OPT) $(INC) -c test/block_solver.cpp -o $(OBJ_DIR)/block_test.o
	$(CXX) $(STD) $(OPT) $(INC) $(333_OBJ) obj/block_test.o -o $(OBJ_DIR)/block

$(EPICUBE_DIR)/obj/333.o: $(EPICUBE_DIR)/333/333.cpp $(EPICUBE_DIR)/lib/*
	$(MAKE) obj/333.o -C $(EPICUBE_DIR)

# Objects directory
obj:
	mkdir -p $(OBJ_DIR)

# Règle de nettoyage
clean:
# 	$(MAKE) clean -C $(EPICUBE_DIR) # probably not a good idea
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/test 

.PHONY: all clean