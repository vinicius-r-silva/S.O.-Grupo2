# Simulador de algoritmos de troca de página

# 1. Dependências:
``` 
sudo apt-get install mesa-common-dev qt5-default cmake g++
``` 

# 2. Compilar:
``` 
cd .../S.O.-Grupo2/Atividade_3/QT/Simulador_Memoria/
mkdir build
cd build
cmake ../.
make
``` 

# 3. Os comandos são compostos da seguinte forma
P(x) C A

x = pid do processo
            
C = comando
            
A = argumento em decimal (byte como unidade)




# 3.1 Comandos Disponíveis:

C - Create (P1 C 500 = Cria processo de pid 1, com tamanho de 500 bytes)

K - Kill (P2 K = Mata processo de pid 2)

R - Read (P1 R 1200 = Lê byte 1200 do processo de pid 1)

W - Write (P3 W 1200 = Escreve no byte 1200 do processo de pid 1)

P - CPU Operation (P2 P 7 = processo de pid 2 executa instrução 7 na CPU)

I - I/O Operation (P1 I 2 = processo de pid 1 executa instrução 2 de I/O)
