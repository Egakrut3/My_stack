BIN_DIR = ./bin/
BIN_SUF = .o
make_bin_path = $(addprefix $(BIN_DIR), $(addsuffix $(BIN_SUF), $(1)))

SRC_DIR = ./src/
SRC_SUF = .cpp
make_src_path = $(addprefix $(SRC_DIR), $(addsuffix $(SRC_SUF), $(1)))

H_DIR = ./include/

LIB_DIR = static_libs/
LIBS = Colored_printf My_stack

CXX = g++
CXX_FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline   \
-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default  \
-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy \
-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                      \
-Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith          \
-Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits           \
-Wwrite-strings -Werror=vla -D_EJUDGE_CLIENT_SIDE -D__USE_MINGW_ANSI_STDIO -D_DEBUG

TARGET_LIB = $(addprefix $(LIB_DIR), libMy_stack.a)
TARGET = $(addprefix $(BIN_DIR), Stack.exe)

OBJ = My_stack_basic My_stack_functions

make_object = $(call make_bin_path, $(1)) : $(call make_src_path, $(1)); \
@$(CXX) $(CXX_FLAGS) -c $$< -I $(H_DIR) -o $$@

.PHONY : all prepare test clean

all : prepare $(call make_bin_path, $(OBJ))
	@ar rcs $(TARGET_LIB) $(call make_bin_path, $(OBJ))
	@echo Compilation end

test : all $(call make_bin_path, main)
	@$(CXX) $(CXX_FLAGS) $(call make_bin_path, main) -L$(LIB_DIR) $(addprefix -l, $(LIBS)) -o $(TARGET)
	@$(TARGET)

prepare :
	@mkdir -p bin

$(call make_object, My_stack_basic)

$(call make_object, My_stack_functions)

$(call make_object, main)

clean:
	@rm -rf bin
