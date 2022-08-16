CC=cc
CPP=g++
ASM=am
AR=ar
SRC_DIR=src
TARGET_PKG=target
TARGET_LIB=lib
TARGET_BIN=bin
TARGET=$(TARGET_LIB)/libnpu.so $(TARGET_LIB)/libnpudb.so $(TARGET_LIB)/libnpuhttp.so $(TARGET_BIN)/npufu.o $(TARGET_BIN)/npusu.o $(TARGET_BIN)/npurc.o  $(TARGET_LIB)/libnpusu.a $(TARGET_LIB)/libnpufu.a $(TARGET_LIB)/libnpurc.a $(TARGET_PKG)/npou $(TARGET_PKG)/npu $(TARGET_PKG)/npudb
install: build
	@cp $(TARGET_LIB)/*.so /usr/lib
	@cp $(TARGET_PKG)/* /usr/bin
	@mkdir -p /var/npou /etc/npou/
	@cat mirror > /etc/npou/mirror
	@echo "Success!"

init:
	@mkdir -p $(TARGET_LIB)
	@mkdir -p $(TARGET_BIN)
	@mkdir -p $(TARGET_PKG)

_dummy := init

build: init $(TARGET)

$(TARGET_LIB)/libnpu.so: $(SRC_DIR)/npu.c
	$(CC) -fPIC -shared $< -o $@

$(TARGET_LIB)/libnpudb.so: $(SRC_DIR)/npu_db.c
	$(CC) -fPIC -shared $< -o $@

$(TARGET_LIB)/libnpuhttp.so: $(SRC_DIR)/http_download.c
	$(CC) -fPIC -shared $< -o $@

$(TARGET_BIN)/npufu.o: $(SRC_DIR)/file_util.c
	$(CC) -c $< -o $@

$(TARGET_BIN)/npusu.o: $(SRC_DIR)/string_util.c
	$(CC) -c $< -o $@

$(TARGET_BIN)/npurc.o: $(SRC_DIR)/config_file_read.c
	$(CC) -c $< -o $@

$(TARGET_LIB)/libnpusu.a: $(TARGET_BIN)/npufu.o
	$(AR) -cr $@ -o $<

$(TARGET_LIB)/libnpufu.a: $(TARGET_BIN)/npusu.o
	$(AR) -cr $@ -o $<

$(TARGET_LIB)/libnpurc.a: $(TARGET_BIN)/npurc.o
	$(AR) -cr $@ -o $<

$(TARGET_PKG)/npou: $(SRC_DIR)/npou.c
	$(CC) $< -L./$(TARGET_LIB) -lnpu -lnpudb -lnpusu -lnpurc -lnpusu -lnpufu -lnpuhttp -o $@

$(TARGET_PKG)/npu: $(SRC_DIR)/npu_cli.c
	$(CC) $< -L./$(TARGET_LIB) -lnpu -lnpudb -lnpusu -lnpurc -lnpusu -lnpufu -lnpuhttp -o $@

$(TARGET_PKG)/npudb: $(SRC_DIR)/npu_db_cli.c
	$(CC) $< -L./$(TARGET_LIB) -lnpu -lnpudb -lnpusu -lnpurc -lnpusu -lnpufu -o $@

clear:
	@rm -rvf $(TARGET_BIN)
	@rm -rvf $(TARGET_LIB)
	@rm -rvf $(TARGET_PKG)

uninstall:
	@rm -rfv /usr/lib/libnpu.so
	@rm -rfv /usr/lib/libnpudb.so
	@rm -rfv /usr/lib/libnpuhttp.so
	@rm -rfv /usr/bin/npou
	@rm -rfv /usr/bin/npu
	@rm -rfv /usr/bin/npudb
	@rm -rfv /var/npou
	@rm -rfv /etc/npou
	@echo "Uninstall success！"

remove:
	@rm -rfv ./*