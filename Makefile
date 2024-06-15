# Makefile

# Variables
POETRY=poetry
GEN_TESTCASES_DIR=tools/gen_testcases
TEST_VECTOR_DIR=test/testcases
CXX=clang++
CXXFLAGS=-std=c++20 -Wall -Wextra -O2 -I src
TEST_SRC=test/main.cpp
TEST_BIN=test/main

# Targets
.PHONY: init gen test clean

init:
	@echo "Initializing project"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) install

# Generate simple_p.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_p.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_p.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_p --arch PID --kp 0.1 --ki 0.0 --kd 0.0 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_p.hpp

# Generate simple_i.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_i.hpp:  init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_i.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_i --arch PID --kp 0.0 --ki 0.1 --kd 0.0 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_i.hpp

# Generate simple_d.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_d.hpp:  init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_d.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_d --arch PID --kp 0.0 --ki 0.0 --kd 0.1 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_d.hpp

# Generate simple_pi.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_pi.hpp:  init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_pi.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_pi --arch PID --kp 0.1 --ki 0.1 --kd 0.0 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_pi.hpp

# Generate simple_pd.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_pd.hpp:  init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_pd.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_pd --arch PID --kp 0.1 --ki 0.0 --kd 0.1 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_pd.hpp

# Generate simple_pid.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_pid.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_pid.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_pid --arch PID --kp 0.1 --ki 0.1 --kd 0.1 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_pid.hpp

# Generate occilate_p.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/occilate_p.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating occilate_p.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  occilate_p --arch PID --kp 1.0 --ki 0.0 --kd 0.0 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/occilate_p.hpp

# Generate simple_pid.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/general_pid.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating general_pid.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  general_pid --arch PID --kp 0.8 --ki 2.3 --kd 0.05 --g 1.2 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/general_pid.hpp

# Generate simple_pid.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_pi_d.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_pi_d.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_pi_d --arch PI_D --kp 0.1 --ki 0.1 --kd 0.1 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_pi_d.hpp

# Generate simple_pid.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_i_pd.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating simple_i_pd.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  simple_i_pd --arch I_PD --kp 0.1 --ki 0.1 --kd 0.1 --g 1.0 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/simple_i_pd.hpp

# Generate simple_pid.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/general_pi_d.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating general_pi_d.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  general_pi_d --arch PI_D --kp 0.8 --ki 2.3 --kd 0.05 --g 1.2 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/general_pi_d.hpp

# Generate simple_pid.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/general_i_pd.hpp: init $(GEN_TESTCASES_DIR)/gen_testcases/main.py
	@echo "Generating general_i_pd.hpp"
	$(POETRY) --directory $(GEN_TESTCASES_DIR) run gen_testcases  general_i_pd --arch I_PD --kp 0.8 --ki 2.3 --kd 0.05 --g 1.2 --sp 0.1 --n 32 > $(TEST_VECTOR_DIR)/general_i_pd.hpp

# Generate all test vectors
gen: $(TEST_VECTOR_DIR)/simple_p.hpp $(TEST_VECTOR_DIR)/simple_i.hpp $(TEST_VECTOR_DIR)/simple_d.hpp $(TEST_VECTOR_DIR)/simple_pi.hpp $(TEST_VECTOR_DIR)/simple_pd.hpp $(TEST_VECTOR_DIR)/simple_pid.hpp $(TEST_VECTOR_DIR)/occilate_p.hpp $(TEST_VECTOR_DIR)/general_pid.hpp $(TEST_VECTOR_DIR)/general_pi_d.hpp $(TEST_VECTOR_DIR)/general_i_pd.hpp 
	@echo "All test vectors generated"

# Build the test program
$(TEST_BIN): $(TEST_SRC) gen
	@echo "Building test program"
	$(CXX) $(CXXFLAGS) -o $(TEST_BIN) $(TEST_SRC)

# Run the test program
test: $(TEST_BIN)
	@echo "Running test program"
	$(TEST_BIN)

# Clean up build artifacts
clean:
	@echo "Cleaning up"
	rm -f $(TEST_BIN)
	rm -f $(TEST_VECTOR_DIR)/simple_p.hpp
	rm -f $(TEST_VECTOR_DIR)/simple_i.hpp
	rm -f $(TEST_VECTOR_DIR)/simple_d.hpp
	rm -f $(TEST_VECTOR_DIR)/simple_pi.hpp
	rm -f $(TEST_VECTOR_DIR)/simple_pd.hpp
	rm -f $(TEST_VECTOR_DIR)/simple_pid.hpp
	rm -f $(TEST_VECTOR_DIR)/general_pid.hpp
	rm -f $(TEST_VECTOR_DIR)/occilate_p.hpp
	rm -f $(TEST_VECTOR_DIR)/simple_pi_d.hpp
	rm -f $(TEST_VECTOR_DIR)/simple_i_pd.hpp
	rm -f $(TEST_VECTOR_DIR)/general_pi_d.hpp
	rm -f $(TEST_VECTOR_DIR)/general_i_pd.hpp
