#Source
SV_FILE = mem_model.sv
TOP_NAME = mem_model
SRC = mem_model.c
DPI_OBJ = svdpi
#Tool
MODELSIM_HOME = /mnt/sdb1/altera/13.1/modelsim_ase/
VLIB = vlib
VLOG = vlog
VSIM = vsim
CC = gcc
#CC = clang

OBJ = $(SRC:%.c=%.o)

run : vlib vlog vsim

vlib:
	$(VLIB) work

vlog:
	$(VLOG) -sv -dpiheader dpiheader.h $(SV_FILE)

vsim: $(DPI_OBJ).so
	$(VSIM) -c -sv_lib $(DPI_OBJ) $(TOP_NAME)  -do "run -all; quit "

.c.o:
	$(CC) -c -g -fpic -m32 -I$(MODELSIM_HOME)/include $<

$(DPI_OBJ).so :$(OBJ)
	$(CC) -O0 -shared -m32 $(OBJ) -o $(DPI_OBJ).so

clean:
	rm -rf work dpi.so $(DPI_OBJ).so $(OBJ)  transcript *.wlf dpiheader.h

