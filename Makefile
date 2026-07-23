# Variables

CC = g++
C = gcc
SOURCES = $(wildcard *.cpp) #All .cpp files in folder
#All Warnings, Linking the right librairies and indicating include folder
CFLAGS = -Wall -g -Iinclude
LFLAGS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
OBJECTS = $(SOURCES:.cpp=.o) glad.o
TARGET = Zouked


# Règle par défaut
all: $(TARGET)

# Lien de l'exécutable
# $@ -> $(TARGET)
# $^ -> $(OBJECTS)
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CC) -o $@ $^ $(LFLAGS) 

# Compilation de glad.c

glad.o: src/glad.c
	@echo "Compiling $<..."
	$(C) $(CFLAGS) -c $< -o $@

# Compilation des fichiers .cpp en .o

%.o: %.cpp
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets
clean:
	@echo "Cleaning object files..."
	rm -f $(OBJECTS)

# Nettoyage complet (fichiers objets et exécutable)
neat: clean
	@echo "Cleaning executable..."
	rm -f $(TARGET)

# Refaire la compilation
re: neat all