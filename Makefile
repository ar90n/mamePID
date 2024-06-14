# Makefile

# Variables
JULIA=julia
GEN_TESTCASES_DIR=tools/gen_testcases
TEST_VECTOR_DIR=test/testcases
CXX=clang++
CXXFLAGS=-std=c++20 -Wall -Wextra -O2 -I src
TEST_SRC=test/main.cpp
TEST_BIN=test/main

# Targets
.PHONY: gen test clean

# Generate simple_p.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_p.hpp: $(GEN_TESTCASES_DIR)/src/gen_testcases.jl
	@echo "Generating simple_p.hpp"
	$(JULIA) --project=$(GEN_TESTCASES_DIR) $(GEN_TESTCASES_DIR)/src/gen_testcases.jl --name simple_p --arch PID --kp 0.1 --ki 0.0 --kd 0.0 --g 1.0 --sp 1.0 --ol 0 --n 64 > $(TEST_VECTOR_DIR)/simple_p.hpp

# Generate simple_i.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_i.hpp: $(GEN_TESTCASES_DIR)/src/gen_testcases.jl
	@echo "Generating simple_i.hpp"
	$(JULIA) --project=$(GEN_TESTCASES_DIR) $(GEN_TESTCASES_DIR)/src/gen_testcases.jl --name simple_i --arch PID --kp 0.0 --ki 0.1 --kd 0.0 --g 1.0 --sp 1.0 --ol 0 --n 64 > $(TEST_VECTOR_DIR)/simple_i.hpp

# Generate simple_d.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_d.hpp: $(GEN_TESTCASES_DIR)/src/gen_testcases.jl
	@echo "Generating simple_d.hpp"
	$(JULIA) --project=$(GEN_TESTCASES_DIR) $(GEN_TESTCASES_DIR)/src/gen_testcases.jl --name simple_d --arch PID --kp 0.0 --ki 0.0 --kd 0.1 --g 1.0 --sp 1.0 --ol 1 --n 64 > $(TEST_VECTOR_DIR)/simple_d.hpp

# Generate simple_pi.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_pi.hpp: $(GEN_TESTCASES_DIR)/src/gen_testcases.jl
	@echo "Generating simple_pi.hpp"
	$(JULIA) --project=$(GEN_TESTCASES_DIR) $(GEN_TESTCASES_DIR)/src/gen_testcases.jl --name simple_pi --arch PID --kp 0.1 --ki 0.1 --kd 0.0 --g 1.0 --sp 1.0 --ol 0 --n 64 > $(TEST_VECTOR_DIR)/simple_pi.hpp

# Generate simple_pd.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_pd.hpp: $(GEN_TESTCASES_DIR)/src/gen_testcases.jl
	@echo "Generating simple_pd.hpp"
	$(JULIA) --project=$(GEN_TESTCASES_DIR) $(GEN_TESTCASES_DIR)/src/gen_testcases.jl --name simple_pd --arch PID --kp 0.1 --ki 0.0 --kd 0.1 --g 1.0 --sp 1.0 --ol 1 --n 64 > $(TEST_VECTOR_DIR)/simple_pd.hpp

# Generate simple_pid.hpp in the test vectors directory
$(TEST_VECTOR_DIR)/simple_pid.hpp: $(GEN_TESTCASES_DIR)/src/gen_testcases.jl
	@echo "Generating simple_pid.hpp"
	$(JULIA) --project=$(GEN_TESTCASES_DIR) $(GEN_TESTCASES_DIR)/src/gen_testcases.jl --name simple_pid --arch PID --kp 0.1 --ki 0.1 --kd 0.1 --g 1.0 --sp 1.0 --ol 1 --n 64 > $(TEST_VECTOR_DIR)/simple_pid.hpp

# Generate all test vectors
gen: $(TEST_VECTOR_DIR)/simple_p.hpp $(TEST_VECTOR_DIR)/simple_i.hpp $(TEST_VECTOR_DIR)/simple_d.hpp $(TEST_VECTOR_DIR)/simple_pi.hpp $(TEST_VECTOR_DIR)/simple_pd.hpp $(TEST_VECTOR_DIR)/simple_pid.hpp
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
