# Coloque aqui o caminho para o CPLEX na sua máquina
CPLEX_HOME = /opt/ibm/ILOG/CPLEX_Studio222

lisrun:
	g++ -O3 src/*.cpp \
	-I./include \
	-I$(CPLEX_HOME)/cplex/include \
	-I$(CPLEX_HOME)/concert/include \
	-L$(CPLEX_HOME)/cplex/lib/x86-64_linux/static_pic \
	-L$(CPLEX_HOME)/concert/lib/x86-64_linux/static_pic \
	-lilocplex -lconcert -lcplex \
	-lpthread -ldl -lm \
	-std=c++17 \
	-o program

clean:
	rm program out/*.lp out/*.txt