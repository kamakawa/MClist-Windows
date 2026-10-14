# Plataforma: Windows (MSYS2/MinGW) ou Linux
ifeq ($(OS),Windows_NT)
    PLATFORM := win-x64
    EXE      := .exe
else
    PLATFORM := linux-x64
    EXE      :=
endif

PROGRAM_GPU := mc_sim$(EXE)
PROGRAM_CPU := mc_sim_cpu$(EXE)

OUT_DIR   := dist/$(PLATFORM)
BUILD_DIR := build/$(PLATFORM)

########### Gnu:
COMPILER := g++
GPUCOMP  := nvcc
FLAGS    := -O3 -fopenmp -static
GPUFLAGS := -O3
LIB      := -lm -lgsl -lgslcblas -lgomp

USECUDA := -D CUDA__="CUDA"
ifeq ($(OS),Windows_NT)
    # No Windows o nvcc exige MSVC, incompatível com objetos do g++: só build de CPU
    .DEFAULT_GOAL=CPU
    USECUDA :=
else ifeq (,$(shell which nvcc))
    .DEFAULT_GOAL=CPU
    USECUDA :=
endif

CPPS   := $(wildcard src/*.cpp)
CUDA   := $(wildcard src/*.cu)
HEADER := $(wildcard include/*.h)

OBJS     := $(patsubst src/%.cpp,${BUILD_DIR}/%.o,${CPPS})
CBJS     := $(patsubst src/%.cu,${BUILD_DIR}/%.cuda.o,${CUDA})
DBGOBJS  := $(patsubst src/%.cpp,${BUILD_DIR}/%dbg.o,${CPPS})
DBGCBJS  := $(patsubst src/%.cu,${BUILD_DIR}/%dbg.cuda.o,${CUDA})

all: ${PROGRAM_GPU}

# ---------------- GPU build ----------------
${PROGRAM_GPU}: ${OBJS} ${CBJS} | ${OUT_DIR}
	@${GPUCOMP} ${GPUFLAGS} $(filter-out ${BUILD_DIR}/simulator.o,${OBJS}) ${CBJS} ${LIB} -o ${OUT_DIR}/${PROGRAM_GPU}

# ---------------- CPU build ----------------
CPU: $(filter-out ${BUILD_DIR}/simulatorGPU.o,${OBJS}) | ${OUT_DIR}
	@${COMPILER} ${FLAGS} $(filter-out ${BUILD_DIR}/simulatorGPU.o,${OBJS}) ${LIB} -o ${OUT_DIR}/${PROGRAM_CPU}

# -------------- object rules --------------
${OBJS}: ${BUILD_DIR}/%.o: src/%.cpp ${HEADER} | ${BUILD_DIR}
	${COMPILER} ${FLAGS} -c $< -o $@ ${USECUDA}

${CBJS}: ${BUILD_DIR}/%.cuda.o: src/%.cu | ${BUILD_DIR}
	${GPUCOMP} ${GPUFLAGS} -dc -c $< -o $@ -D CUDA__="CUDA"

${BUILD_DIR}:
	@mkdir -p ${BUILD_DIR}

${OUT_DIR}:
	@mkdir -p ${OUT_DIR}

# ---------------- debug ----------------
debug: ${DBGOBJS} ${DBGCBJS} | ${OUT_DIR}
	${GPUCOMP} -O0 -g -Xcompiler -fopenmp -lineinfo \
	$(filter-out -O% -fast -static, ${GPUFLAGS}) \
	$(filter-out ${BUILD_DIR}/simulatordbg.o,${DBGOBJS}) ${DBGCBJS} ${LIB} \
	-o ${OUT_DIR}/${PROGRAM_GPU}_debug

${DBGOBJS}: ${BUILD_DIR}/%dbg.o: src/%.cpp | ${BUILD_DIR}
	${COMPILER} -O0 -g $(filter-out -O% -fast, ${FLAGS}) -c $< -o $@ ${USECUDA}

${DBGCBJS}: ${BUILD_DIR}/%dbg.cuda.o: src/%.cu | ${BUILD_DIR}
	${GPUCOMP} -O0 -g $(filter-out -O% -fast, ${GPUFLAGS}) -lineinfo -dc -c $< -o $@ ${USECUDA}

$(patsubst %.o,%dbg.o,${OBJS}): ${HEADER}

renew: clean all

clean:
	@rm -f ${OUT_DIR}/${PROGRAM_GPU} ${OUT_DIR}/${PROGRAM_CPU} ${OUT_DIR}/${PROGRAM_GPU}_debug
	@rm -fr build
	@rm -f *.o

clean-data:
	@echo "Removendo arquivos de dados: director_field_*.csv"
	@rm -f director_field_*.csv