/**
* @file SnakeV4.c
* @brief programme snake
* @author Dréan Valentin 1A2 
* @version version 4.0
* @date 24/11/2024
*
* programme d'un serpent aux coordonnées x et y qui peut se déplacer dans les quatre direction avec x et y
* dans un plateau avec 4 pavées et qui s'arrête si l'utilisateur appuie sur A ou si le serpent rentre en colision avec quelque'chose.
* Le serpent peut manger une pomme qui lui fait augmenter sa taille et sa vitesse, le jeu se termine quand le serpent a manger 10 pommes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>
const int X = 40;
const int Y = 20;
const char TFIN = 'a';
#define DIREC_Z 'z'
#define DIREC_Q 'q'
#define DIREC_S 's'
#define DIREC_D 'd'
const char ANNEAUX = 'X';
const char BORDER = '#';
const char ESPACE = ' ';
const char PAVE = '#' ;
#define LARGEUR_MAX 80
#define HAUTEUR_MAX 40
#define LARGEUR_MIN 1
#define HAUTEUR_MIN 1
const int NOMBRE_PAVES = 4;
const int TAILLE_PAVES = 5;
const char ISSUE = ' ';
const char POMME = '6' ;
const int ACCELERATION = 6000;
const int TAILLE_MAX = 10;
const int TAILLE = 10 ;
int DUREE = 100000;
char plateau[HAUTEUR_MAX][LARGEUR_MAX];

#define PROFIL_LARMAX 75
#define PROFIL_HAUMAX 30
#define PROFIL_LARMIN 25
#define PROFIL_HAUMIN 20
char carte[PROFIL_HAUMAX][PROFIL_LARMAX];

const char BARRE = 'X';
const char TETE_HAUT = '^';
const char TETE_BAS = 'v';
const char TETE_DROIT = '>';
const char TETE_GAUCHE = '<';
char tete = '>';
int taille_serpent = TAILLE;
const int DUREE_INITIALE = 100000;




int kbhit();

void gotoXY(int x, int y);

void afficher(int x, int y, char c);

void effacer(int x, int y);

void dessinerSerpent(int lesX[], int lesY[]);

void progresser(int lesX[], int lesY[], char direction, bool *colision, bool *est_manger );

void disableEcho();

void enableEcho();

void initPlateau(char tab[HAUTEUR_MAX][LARGEUR_MAX]);

void dessinerPlateau(char tab[HAUTEUR_MAX][LARGEUR_MAX]);

void ajouterPomme(char tab[HAUTEUR_MAX][LARGEUR_MAX], int lesX[], int lesY[]);

void menu(int *rep);

void carte_profil(char tab[PROFIL_HAUMAX][PROFIL_LARMAX], const char *nomp, int taille_nom, float niveau, int nb_parti, int nb_win, int nb_trophe);

void parametre_jeu();

void compteur_trophe(int nb_pomme_manger, int *nb_trophe);

int main(){
	int reponse;
	char pseudo[15];
	char direc;
	char touche;
	int tabX[TAILLE+TAILLE_MAX], tabY[TAILLE+TAILLE_MAX];
	bool col, mange;
	int compteur;
	float niv = 0;
	compteur = 0;
	direc =  DIREC_D;
	int parti = 0;
	int win = 0;
	int trophe = 0;
	printf("Ecrivez votre Pseudo ( 15 caracteres max )");
    scanf("%14s", pseudo);
	menu(&reponse);
	while(reponse != 2){
		switch(reponse){
			case 1:
			    parti = parti + 1;
				enableEcho();
				system("clear");
				for (int i = 0; i < taille_serpent ; i++){
					if(X-i>0){
						tabX[i] = X - i;
					}
					else{
						tabX[i] = 1;
					}
						tabY[i] = Y;
				}
				initPlateau(plateau);
				dessinerPlateau(plateau);
				ajouterPomme(plateau, tabX, tabY);
				taille_serpent = TAILLE;
            	compteur = 0;
            	direc = DIREC_D;
            	DUREE = DUREE_INITIALE;
				while (1){
					disableEcho();
					if (kbhit() == 1){
						touche = getchar();
						if (touche == TFIN){
							compteur_trophe(compteur, &trophe);
							system("clear");
							enableEcho();
							break;
						}
						else if ((touche == DIREC_Z && direc != DIREC_S) || (touche == DIREC_S && direc != DIREC_Z ) || (touche == DIREC_Q && direc != DIREC_D) || (touche == DIREC_D && direc != DIREC_Q)){
							direc = touche;
						}
					}
					progresser(tabX, tabY, direc, &col, &mange);
					if (col == true){
						compteur_trophe(compteur, &trophe);
						system("clear");
						enableEcho();
						gotoXY(50, 23);
						printf("Vous avez perdu, la parti est fini:");
						break;
					}
					if (mange == true){
						taille_serpent ++;
						compteur = compteur + 1; 
						DUREE = DUREE - ACCELERATION;
						niv = niv + 0.1;
						ajouterPomme(plateau, tabX, tabY);
					}
					if (compteur == TAILLE_MAX){
						compteur_trophe(compteur, &trophe);
						niv = niv + 1;
						win = win + 1;
						system("clear");
						enableEcho();
						gotoXY(50, 23);
						printf("Vous avez gagner, la parti est fini");
						break;
					}
					dessinerSerpent(tabX, tabY);
					usleep(DUREE);
				}
				break;
			case 3:
				system("clear");
				carte_profil(carte, pseudo, strlen(pseudo), niv, parti, win, trophe);
				disableEcho();
				while (1) {
					if (kbhit() == 1){
        				char retour = getchar();
        				if (retour == TFIN) {
							system("clear");
            				break;
        				}
					}
    			}
				enableEcho();
				break;
			case 4:
				parametre_jeu();
		}
		menu(&reponse);
	}
	enableEcho();
	system("clear");
	return EXIT_SUCCESS;
}

int kbhit(){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere present
	
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}

void gotoXY(int x, int y)
/**
* @brief procedure t'envoie a la position x y 
* @param x de type entier, Entrée : cordonnée x.
* @param y de type entier, Entrée : cordonnée y.
* 
*/
{ 
    printf("\033[%d;%df", y, x);
}

void afficher(int x, int y, char c)
/**
* @brief Procedure qui affiche un caractère a la position x y
* @param x de type entier, Entrée: cordonnée x.
* @param y de type entier, Entrée: cordonnée y.
* @param c de type caractère, Entrée: caractère c
* @return le caractère c a la position x y
*/
{
    gotoXY(x, y);
    printf("%c",c);
}

void effacer(int x, int y)
/**
* @brief Procedure qui efface un caractère a la position x y
* @param x de type entier, Entrée: cordonnée x.
* @param y de type entier, Entrée: cordonnée y.
* @return un espace a la position x y
*/
{
    gotoXY(x, y);
    printf(" ");
}

void dessinerSerpent(int lesX[], int lesY[])
/**
* @brief Procedure qui dessine un serpent composé de X et d'un O pour la tête
* @param lesX de type tableau, Entrée: tableau lesX.
* @param lesY de type tableau, Entrée: tableau lesY.
* @return un serpent composé de x et d'un o
*/
{
	
	afficher(lesX[0], lesY[0], tete);
	for(int i = 1; i < taille_serpent; i=i+1 )
	{
		afficher(lesX[i], lesY[i], ANNEAUX);
	}
}

void progresser(int lesX[], int lesY[], char direction, bool *colision, bool *est_manger)
/**
* @brief Procédure qui fais progresser d'un element dans un tableau
* @param lesX de type tableau, Entrée: tableau lesX.
* @param lesY de type tableau, Entrée: tableau lesY.
* @param direction de type caractère, Entrée: caractère direction.
* @param colision de type boolean , Sortie: true ou false .
* @param est_manger de type boolean, Sortie: true ou false.
* @return change la position du serpent en le faisant aller dans la direction de char direction, retourne si le serpent rentre en colision avec un objet, et retourne si le serpent à manger une pome.
*/
{
	effacer(lesX[taille_serpent-1], lesY[taille_serpent-1]);
	for (int i = taille_serpent - 1; i > 0; i--) {
		lesX[i] = lesX[i - 1];
		lesY[i] = lesY[i - 1];
	}
	switch (direction){
		case DIREC_Z:
			lesY[0] = lesY[0] -1;
			tete = TETE_HAUT;
			break;
		case DIREC_S:
			lesY[0] = lesY[0] + 1;
			tete = TETE_BAS;
			break;
		case DIREC_Q:
			lesX[0] = lesX[0] -1;
			tete = TETE_GAUCHE;
            break;
		case DIREC_D:
			lesX[0] = lesX[0] + 1;
			tete = TETE_DROIT;
            break;
	}

	if (lesY[0] == HAUTEUR_MAX / 2 && lesX[0] == 1) { //a gauche
        lesX[0] = LARGEUR_MAX - 1;
        lesY[0] = HAUTEUR_MAX / 2;
    } else if (lesY[0] == HAUTEUR_MAX / 2 && lesX[0] == LARGEUR_MAX - 1) { //a droite
        lesX[0] = 1;
        lesY[0] = HAUTEUR_MAX / 2;
    } else if (lesX[0] == LARGEUR_MAX / 2 && lesY[0] == 1) { //en haut
        lesX[0] = LARGEUR_MAX / 2;
        lesY[0] = HAUTEUR_MAX - 1;
    } else if (lesX[0] == LARGEUR_MAX / 2 && lesY[0] == HAUTEUR_MAX - 1) { //en bas
        lesX[0] = LARGEUR_MAX / 2;
        lesY[0] = 1;
    }
	
	if(plateau[lesY[1]][lesX[1]] == PAVE){
		*colision = true;
	}
	else if(plateau[lesY[0]][lesX[0]] == ISSUE){
		*colision = false;
	}
	for (int i = 1 ; i < taille_serpent ; i++){
		if (lesX[0] == lesX[i] && lesY[0] == lesY[i]){
			*colision = true;
		}
	}

	if (plateau[lesY[0]][lesX[0]] == POMME){
		*est_manger = true;
	}
	else{
		*est_manger = false;
	}
}

void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}



void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}


void initPlateau(char tab[HAUTEUR_MAX][LARGEUR_MAX])
/**
* @brief Procédure qui initialise un plateau avec des pavées
* @param tab de type tableau , Entrée: tableau a double entré .
* @return ne retourne rien , modifie le plateau en parametre 
*/
{
	for (int i = 0; i < HAUTEUR_MAX ; i++){
		for ( int j = 0; j < LARGEUR_MAX ; j++){
			if (i == HAUTEUR_MIN || i == HAUTEUR_MAX - 1 || j == LARGEUR_MIN || j == LARGEUR_MAX - 1){
				tab[i][j] = BORDER;
			}
			else{
				tab[i][j] = ESPACE;
			}
		}
	}
	tab[HAUTEUR_MAX / 2][1] = ISSUE;
    tab[HAUTEUR_MAX / 2][LARGEUR_MAX - 1] = ISSUE;
	tab[1][LARGEUR_MAX/2] = ISSUE;
    tab[HAUTEUR_MAX - 1][LARGEUR_MAX / 2] = ISSUE;

	srand(time(NULL));
	for (int k = 0 ; k < NOMBRE_PAVES; k++){
		int x , y;
		bool pos = false;
		while (pos != true){
			x = rand() % (LARGEUR_MAX - 7) + 3;
			y = rand() % (HAUTEUR_MAX - 7) + 3;
			pos = true;
        	for (int i = 0; i < TAILLE_PAVES && pos; i++) {
				for (int j = 0; j < TAILLE_PAVES && pos; j++) {
                    if (tab[y + i][x + j] == BORDER || tab[y + i][x + j] == PAVE ||
                        ((y + i == Y) && (x + j >= X - taille_serpent + 1) && (x + j <= X))) {
                        pos = false;
                    }
                }
            }
        }
		for (int i=0; i < TAILLE_PAVES; i++){
			for (int j =0 ; j < TAILLE_PAVES; j++){
				tab[y + i][x + j] = PAVE;
			}
		}
	}
}


void dessinerPlateau(char tab[HAUTEUR_MAX][LARGEUR_MAX])
/**
* @brief Procédure qui dessine un plateau avec des pavées
* @param tab de type tableau , Entrée: tableau a double entré .
* @return affiche le contenue tu tableau tab
*/
{
	for (int i = 0 ; i < HAUTEUR_MAX ; i++){
		for (int j = 0; j < LARGEUR_MAX; j++){
			afficher(j, i, tab[i][j]);
		}
	}
}

void ajouterPomme(char tab[HAUTEUR_MAX][LARGEUR_MAX], int lesX[], int lesY[]){
/**
* @brief procedure qui ajoute une pomme représenter par un 6 au plateau.
* @param tab de type tableau, Entrée: tableau a double entré.
* @param lesX de type tableau, Entrée: tableau lesX[].
* @param lesY de type tableau, Entrée: tableau lesY[].
* @return affiche une pomme représenter par un 6.
*/
	srand(time(NULL));
	int x , y;
	bool pos = false;
	while (pos != true){
		x = rand() % (LARGEUR_MAX - 2) + 1;
		y = rand() % (HAUTEUR_MAX -2 ) + 1;
		pos = (tab[y][x] == ESPACE);
        for (int i = 0; i < taille_serpent && pos; i++) {
			if (lesX[i] == x && lesY[i] == y){
                pos = false;
            }
		}
	}
	tab[y][x] = POMME;
    afficher(x, y, POMME);
}

void menu(int *rep){
	system("clear");
	gotoXY(50, 25);
    printf("Appuyer sur 1 pour lancer une partie \n");
	gotoXY(50, 26);
    printf("Appuyer sur 2 Quitter le jeu \n");
	gotoXY(50, 27);
	printf("Appuyer sur 3 pour acceder au profil \n");
	gotoXY(50, 28);
	printf("Appuyer sur 4 pour acceder aux paramètres");
	while (1) {
		disableEcho();
    	if (kbhit() == 1) {
            char choix = getchar();
            if (choix >= '1' && choix <= '4') {
                *rep = choix - '0'; // Convertit le caractère en entier
                break;
            }
        }
    }
	enableEcho();
}

void carte_profil(char tab[PROFIL_HAUMAX][PROFIL_LARMAX], const char *nomp, int taille_nom, float niveau, int nb_parti, int nb_win, int nb_trophe){
	for (int i = PROFIL_HAUMIN ; i < PROFIL_HAUMAX  ; i++)
		for ( int j = PROFIL_LARMIN; j < PROFIL_LARMAX ; j++){
			if (i == PROFIL_HAUMIN || i == PROFIL_HAUMAX - 1 || j == PROFIL_LARMIN || j == PROFIL_LARMAX - 1){
				tab[i][j] = BORDER;
			}
			else{
				tab[i][j] = ESPACE;
			}
		}
    for (int i = PROFIL_HAUMIN; i < PROFIL_HAUMAX; i++) {
        for (int j = PROFIL_LARMIN; j < PROFIL_LARMAX; j++) {
            afficher(j, i, tab[i][j]);
        }
    }
    for (int i = 0; i < taille_nom; i++) {
        afficher(PROFIL_LARMIN + 2 + i, PROFIL_HAUMIN + 2, nomp[i]);
    }
	for (int i = 0; i < strlen("Appuyez sur 'a' pour revenir au menu."); i++){
		afficher(PROFIL_LARMIN + i, PROFIL_HAUMIN - 2, "Appuyez sur 'a' pour revenir au menu."[i]);
	}
	char buffer[20];
	sprintf(buffer, "Niveau: %d", (int)niveau);
	for (int i = 0; buffer[i] != '\0'; i++ ){
        afficher(PROFIL_LARMIN + 2 + i, PROFIL_HAUMIN + 4,  buffer[i]);
    }
	char buffer2[20];
	sprintf(buffer2, "Parti: %d", nb_parti);
	for (int i = 0; buffer2[i] != '\0'; i++ ){
		afficher(PROFIL_LARMIN + 2 + i, PROFIL_HAUMIN + 6,  buffer2[i]);
	}
	char buffer3[20];
	sprintf(buffer3, "WIN: %d", nb_win);
	for (int i = 0; buffer3[i]!= '\0'; i++ ){
        afficher(PROFIL_LARMIN + 2 + i, PROFIL_HAUMIN + 8,  buffer3[i]);
    }
	char buffer4[20];
	sprintf(buffer4, "Tr: %d", nb_trophe);
	for (int i = 0; buffer4[i]!= '\0'; i++ ){
        afficher(PROFIL_LARMIN + 30 + i, PROFIL_HAUMIN + 1,  buffer4[i]);
    }

	float progression = niveau - (int)niveau;
	int hauteur_barre = PROFIL_HAUMAX - PROFIL_HAUMIN - 6;	
	int progression_remplie = (int)(progression * hauteur_barre);
	for (int i = PROFIL_HAUMIN + 2; i < PROFIL_HAUMAX - 2; i++) {
    	for (int j = PROFIL_LARMAX - 9; j < PROFIL_LARMAX - 3; j++) {
        	if (i == PROFIL_HAUMIN + 2 || i == PROFIL_HAUMAX - 3 || j == PROFIL_LARMAX - 9 || j == PROFIL_LARMAX - 4) {
            	tab[i][j] = BORDER;
        	}
    	}
	}
	int barre_x1 = PROFIL_LARMAX - 7;
	for (int i = 0; i < progression_remplie; i++) {
    	int barre_y1 = (PROFIL_HAUMIN + 3) + i;
    	tab[barre_y1][barre_x1] = BARRE;
	}
	int barre_x2 = PROFIL_LARMAX - 6;
	for (int i = 0; i < progression_remplie; i++) {
        int barre_y2 = (PROFIL_HAUMIN + 3) + i;
        tab[barre_y2][barre_x2] = BARRE;
    }
	for (int i = PROFIL_HAUMIN + 2; i < PROFIL_HAUMAX - 2; i++) {
    	for (int j = PROFIL_LARMAX - 9; j < PROFIL_LARMAX - 3; j++) {
        	afficher(j, i, tab[i][j]);
    	}
	}

	char buffer5[20];
	sprintf(buffer5, "%d", (int)niveau + 1);
	for (int i = 0; buffer5[i] != '\0'; i++ ){
		afficher(PROFIL_LARMAX - 7 + i, PROFIL_HAUMAX - 2,  buffer5[i]);
	}
}

void parametre_jeu(){
	int rep;
	system("clear");
	gotoXY(50, 25);
	printf("Appuyer sur 1 pour rien faire \n");
	gotoXY(50, 26);
    printf("Appuyer sur 2 Quitter Parametre \n");
	disableEcho();
	scanf("%d", &rep);
	enableEcho();
	switch(rep){
		case 1:
			printf("Feur");
			break;
		case 2:
			break;
	}
}

void compteur_trophe(int nb_pomme_manger, int *nb_trophe){
	switch(nb_pomme_manger){
	    default:
			*nb_trophe -= 5;
			break;
		case 1:
            *nb_trophe -= 4;
            break;
        case 2:
            *nb_trophe -= 3;
            break;
        case 3:
            *nb_trophe -= 2;
            break;
        case 4:
            *nb_trophe -= 1;
            break;
		case 5:
		    *nb_trophe = *nb_trophe;
		case 6:
		    *nb_trophe = *nb_trophe + 2;
			break;
		case 7:
		    *nb_trophe = *nb_trophe + 4;
			break;
		case 8:
		    *nb_trophe = *nb_trophe + 6;
			break;
		case 9:
		    *nb_trophe = *nb_trophe + 8;
			break;
		case 10:
		    *nb_trophe = *nb_trophe + 10;
			break;	
	}
}