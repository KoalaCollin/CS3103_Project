CC:=gcc
CFLAGS:= -Wall -pthread -g -O3
TARGET:=pmo
TARGET_STD:=pmo_std

all: $(TARGET)

gen_data:
	@bash tests/bin/gen_data.sh

gen_ans:
	@bash tests/bin/test-pmo.sh $(TARGET_STD) gen_std

test_std:
	@bash tests/bin/test-pmo.sh $(TARGET_STD)

test:
	@bash tests/bin/test-pmo.sh $(TARGET) build

run:
	@bash tests/bin/test-pmo.sh $(TARGET)

clean:
	@rm -f $(TARGET)
