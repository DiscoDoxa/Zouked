# Variables
CC = g++
CFLAGS = -Wall -g
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = Zouked

# Règle par défaut
all: $(TARGET)

# Lien de l'exécutable
# $@ -> $(TARGET)
# $^ -> $(OBJECTS)
$(TARGET): $(OBJECTS)
	echo "Linking $@..."
    $(CC) -o $@ $^

# Compilation des fichiers .cpp en .o
# $< -> Premier element des pr
%.o: %.cpp
	echo "Compiling $<..."
    $(CC) $(CFLAGS) -c $< -o $@

# Déclaration des cibles phony
.PHONY: all clean fclean re

# Nettoyage des fichiers objets
clean:
	echo "Cleaning object files..."
    rm -f $(OBJECTS)

# Nettoyage complet (fichiers objets et exécutable)
neat: clean
	echo "Cleaning executable..."
    rm -f $(TARGET)

# Refaire la compilation
re: neat all