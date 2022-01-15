CXX = g++
CXXFLAGS = -static -std=c++11 -O3
LDFLAGS = -pthread
# TARGETS = main reader_test producer_test consumer_test writer_test ts_queue_test
TARGETS = main
DEPS = transformer.cpp

.PHONY: all
all: $(TARGETS)

.PHONY: docker-build
docker-build:
	docker-compose run --rm build

.PHONY: clean
clean:
	rm -f $(TARGETS)

.PHONY: trans00
trans00:
	./scripts/auto_gen_transformer --input ./tests/00_spec.json --output transformer.cpp

.PHONY: trans01
trans01:
	./scripts/auto_gen_transformer --input ./tests/01_spec.json --output transformer.cpp

.PHONY: var00
var00:
	./scripts/verify --output ./tests/00.out --answer ./tests/00.ans

.PHONY: var01
var01:
	./scripts/verify --output ./tests/01.out --answer ./tests/01.ans

%: %.cpp $(DEPS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^
