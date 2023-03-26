#Makefile pour le projet matrixButton
#Utilise la compilation croisée pour créer un executapble compatible Raspberry 3B+

#ATTENTION :
#Ce makefile ne fonctionne qu'à deux conditions:
#1) Bien avoir installé le compilateur croisé et bien avoir renseigné son chemin 
#	d'accés dans la variable PATH_CC.
#
#2) Bien avoir créé un dossier RPI_Root/ sur son ordinateur et avoir installé wiringPi
# 	dedans. (Ci-joint un RPI_Root.zip qui contient déjà tout ça !)


##### COMPILATION CROISEE #######
PATH_CC=/home/ludwinski/Bureau/PSE/RPI/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
GCC_PI=$(PATH_CC)/arm-linux-gnueabihf-gcc

#Fake image de notre raspberry sur l'ordi
PATH_RPI_ROOT=/home/ludwinski/Bureau/PSE/RPI/RPI_Root

#Mode verbeux, tapper make V=1
ifneq ($V,1)
Q = @
endif

#Paramètres compilateur
DESTDIR =$(PATH_RPI_ROOT)

INCLUDE	= -I. -I$(DESTDIR)/include

CPFLAGS	= -Wall
LDFLAGS = -L. -L$(DESTDIR)/lib

LIBS    = -lpthread -lwiringPi

### Pour envoyer les programmes sur la Raspberry
USER_PI = pi
IP_PI   = 172.24.1.1

########################################################################

all : lib exe

lib : libmatrixButton.so

exe : pianoJoypi

libmatrixButton.so : matrixButton.c
	@ echo "Compilation de $< pour produire $@"
	$Q $(GCC_PI) $(CPFLAGS) $(INCLUDE) -c -fPIC $< -o matrixButton-fpic.o
	$Q $(GCC_PI) -shared matrixButton-fpic.o -o $@
	$Q rm -f matrixButton-fpic.o

pianoJoypi : libmatrixButton.so pianoJoypi.c
	@ echo "Compilation de pianoJoypi.c pour produire $@"
	$Q $(GCC_PI) $(CPFLAGS) $(INCLUDE) -c pianoJoypi.c -o pianoJoypi.o
	@ echo "Edition des liens pour produire $@"
	$Q $(GCC_PI) $(CPFLAGS) $(LDFLAGS) pianoJoypi.o -o $@ $(LIBS) -lmatrixButton
	$Q rm -f pianoJoypi.o
	@ echo "RAPPEL : Ajouter l'emplacement de libmatrixButton.so dans la variable d'environnement LD_LIBRARY_PATH avant l'exécution !"

sendAll : pianoJoypi libmatrixButton.so
	scp pianoJoypi libmatrixButton.so $(USER_PI)@$(IP_PI):/home/pi/Desktop

clean : 
	$Q rm -f libmatrixButton.so pianoJoypi



