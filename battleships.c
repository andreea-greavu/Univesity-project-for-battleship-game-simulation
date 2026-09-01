#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//alocare dinamica pentru matrice

int **allocate(int n, int m)
{
	int **mat = (int **)calloc((n + 1), sizeof(int *));
	if (!mat) {
		printf("Nu s a putut aloca\n");
		return NULL;
	}
	for (int i = 0; i <= n; i++) {
		mat[i] = (int *)calloc((m + 1), sizeof(int));
		if (!mat[i]) {
			printf("Nu s a putut aloca\n");
			for (int j = 0; j < i; j++) {
				free(mat[j]);
			}
			free(mat);
			return NULL;
		}
	}
	return mat;
}

// dealocare matrice

void free_mat(int **mat, int n)
{
	if (!mat) {
		return;
	}
	for (int i = 0; i <= n; i++) {
		free(mat[i]);
	}
	free(mat);
}

//calculeaza numarul de nave de fiecare tip

void ship_number(int n, int m, int ship[5])
{
	int total = n * m;
	ship[0] = total / 70; //Shiano
	ship[1] = total / 55; //Yamato
	ship[2] = total / 40; //Belfast
	ship[3] = total / 30; //Laffey
	ship[4] = total / 20; //Albacore
}

// returnam lungimea navei in fuctie de tipul pe care il are

int ship_length(char type)
{
	if (type == 'S') {
		return 5;
	} else if (type == 'Y') {
		return 4;
	} else if (type == 'B') {
		return 3;
	} else if (type == 'L') {
		return 2;
	} else if (type == 'A') {
		return 1;
	} else
		return 0;
}

//returnam numele unei nave

char *ship_name(char type)
{
	if (type == 'S') {
		return "Shinano";
	} else if (type == 'Y') {
		return "Yamato";
	} else if (type == 'B') {
		return "Belfast";
	} else if (type == 'L') {
		return "Laffey";
	} else if (type == 'A') {
		return "Albacore";
	} else
		return "Unknown";
}

//verificam daca putem plasa o nava la coordonatele date

int can_place(int **mat, int n, int m, int x, int y,
			  char orientation, int length)
{
	if (orientation == 'H') {// verificam daca incape pe linie
		if (x < 1 || x > n || y < 1 || y + length - 1 > m) {
			return 0;
		}
		for (int j = y; j < y + length; j++) {
			if (mat[x][j] != 0) {
				//pentru a o plasa, celulele trebuie sa fie libere
				return 0;
			}
		}
	} else { // verificam daca incape pe coloana
		if (y < 1 || y > m || x - length + 1 < 1 || x > n) {
			return 0;
		}
		for (int i = x; i > x - length; i--) {
			if (mat[i][y] != 0) {
				//pentru a o plasa, celulele trebuie sa fie libere
				return 0;
			}
		}
	}
	//inseamna ca putem plasa nava
	return 1;
}

//plasam o nava pe tabla

void place_ship(int **mat, int x, int y, char orientation,
				char type, char *ship_type, int *ship_headx,
				int *ship_heady, char *ship_orientation,
				int *ship_lenght, int *ship_remaining_cells,
				int *ship_alive, int ship_indx)
{
	int len = ship_length(type);
	if (len <= 0) {
		return;
	}
	//retinem specificatiile pentru o nava
	ship_type[ship_indx] = type;
	ship_headx[ship_indx] = x;
	ship_heady[ship_indx] = y;
	ship_orientation[ship_indx] = orientation;
	ship_lenght[ship_indx] = len;
	ship_remaining_cells[ship_indx] = len;
	ship_alive[ship_indx] = 1;
	int id_ship = ship_indx + 1;
	// marcam tabla cu id-ul navei
	if (orientation == 'H') {
		for (int j = y; j < y + len; j++) {
			mat[x][j] = id_ship;
		}
	} else {
		for (int i = x; i > x - len; i--) {
			mat[i][y] = id_ship;
		}
	}
}

//afisam tabla de joc

void print_bord(int **mat, int n, int m, int *ship_length, int total_ships)
{
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= m; j++) {
			int id_ship = mat[i][j];
			if (id_ship == 0) {
				printf("0 "); // afisam zero daca celula este goala
			} else if (id_ship > 0 && id_ship <= total_ships) {
				//afisam id-ul navei plasate in acea celula
				printf("%d ", ship_length[id_ship - 1]);
			} else {
				printf("0");
			}
		}
		printf("\n");
	}
}

//procesam un atac

void attack_process(int **mat, int **attacked, int x, int y, int player_nr,
					char *def_ship_type, int *def_ship_headx,
					int *def_ship_heady, char *def_ship_orientation,
					int *def_ship_len, int *def_ship_remaining_cells,
					int *def_ship_alive, int *ships_alive, int *targeted_hits,
					int *total_hits, int n, int m, int total_ships)
{
	(*total_hits)++;
	if (x < 1 || x > n || y < 1 || y > m) {
		//atacul e invalid pentru ca coordonatele nu sunt in limite
		return;
	}
	//verificam daca celulele au mai fost atacate
	if (attacked[x][y] != 0) {
		printf("Coordonatele (%d, %d) au fost deja atacate de jucatorul %d.\n",
			   x, y, player_nr);
		return;
	}
	attacked[x][y] = 1;
	int id_ship = mat[x][y];
	if (id_ship == 0) {
		//celula nu contine nava, deci lovitura este ratata
		printf("Jucatorul %d a ratat o lovitura la coordonatele (%d, %d).\n",
			   player_nr, x, y);
		return;
	}
	int ship_indx = id_ship - 1;
	if (ship_indx < 0 || ship_indx >= total_ships) {
		printf("Jucatorul %d a ratat o lovitura la coordonatele (%d, %d).\n",
				player_nr, x, y);
		return;
	}
	if (!def_ship_alive[ship_indx]) {
		printf("Jucatorul %d a ratat o lovitura la coordonatele (%d, %d).\n",
				player_nr, x, y);
		return;
	}
	(*targeted_hits)++;
	int xend = def_ship_headx[ship_indx];
	int yend = def_ship_heady[ship_indx];
	if (def_ship_orientation[ship_indx] == 'H') {
		yend = yend + def_ship_len[ship_indx] - 1;
	} else {
		xend = xend - def_ship_len[ship_indx] + 1;
	}
	//verificam daca a lovit capul navei
	//in caz afirmativ, inseamna ca a fost distrusa nava
	if (def_ship_headx[ship_indx] == x && def_ship_heady[ship_indx] == y) {
		def_ship_alive[ship_indx] = 0;
		(*ships_alive)--;
		printf("Jucatorul %d a distrus o nava %s plasata intre coordonatele (%d, %d) si (%d, %d).\n",
				player_nr, ship_name(def_ship_type[ship_indx]), def_ship_headx[ship_indx],
				def_ship_heady[ship_indx],	xend, yend);
	} else { // se decrementeaza numarul de celule ramase ale acelei nave,
		//iar cand acele celule ajung sa fie 0, inseamna ca a fost distrusa nava
		def_ship_remaining_cells[ship_indx]--;
		if (def_ship_remaining_cells[ship_indx] == 0) {
			def_ship_alive[ship_indx] = 0;
			(*ships_alive)--;
			printf("Jucatorul %d a distrus o nava %s plasata intre coordonatele (%d, %d) si (%d, %d).\n",
					player_nr, ship_name(def_ship_type[ship_indx]), def_ship_headx[ship_indx],
					def_ship_heady[ship_indx], xend, yend);
		} else {
			printf("Jucatorul %d a lovit o nava %s la coordonatele (%d, %d).\n",
					player_nr, ship_name(def_ship_type[ship_indx]), x, y);
		}
	}
}

//calcul acuratete pe meci

double match_accuracy(int targeted_hits, int total_hits)
{
	if (total_hits == 0) {
		return 0.0;
	}
	return (double)targeted_hits / total_hits * 100.0;
}

//calcul acuratete ponderata

double total_accuracy(int *targeted_hits, int *total_hits,
					  int *ships_game, int play)
{
	double sum_p = 0.0;
	int sum_ships = 0;
	for (int i = 0; i < play; i++) {
		if (ships_game[i] > 0) {
			double accuracy_match = 0.0;
			if (total_hits[i] > 0) {
				accuracy_match = (double)targeted_hits[i] / total_hits[i];
			}
			sum_p = sum_p + accuracy_match * ships_game[i];
			sum_ships = sum_ships + ships_game[i];
		}
	}
	if (sum_ships == 0) {
		return 0.0;
	}
	return sum_p / sum_ships * 100.0;
}

//aflam numarul minim de meciuri cu acuratete 100%
//pentru a avea o acuratete totala >= cu X%

int min_100(int *targeted_hits, int *total_hits, int *ships_game,
			int play, double target_accuracy)
{
	double accuracy = total_accuracy(targeted_hits, total_hits, ships_game, play);
	//avem deja ceea ce dorim
	if (accuracy >= target_accuracy) {
		return 0;
	}
	int *indx = (int *)malloc(play * sizeof(int));
	double *impact = (double *)malloc(play * sizeof(double));
	//calculam ce impact are fiecare meci
	for (int i = 0; i < play; i++) {
		indx[i] = i;
		if (total_hits[i] > 0) {
			double accuracy_match = (double)targeted_hits[i] / total_hits[i];
			//cat ne lipseste pana la acuratete de 100%
			double actual_win = 1.0 - accuracy_match;
			//cate puncte am adauga daca am face meciul de 100%
			impact[i] = actual_win * ships_game[i];
		} else {
			impact[i] = 0.0;
		}
	}

	//sortam meciurile in ordine descrescatoare dupa impact
	for (int i = 0; i < play - 1; i++) {
		int index = i;
		for (int j = i + 1; j < play; j++) {
			if (impact[j] > impact[index]) {
				index = j;
			}
		}
		double aux = impact[i];
		impact[i] = impact[index];
		impact[index] = aux;
		int aux2 = indx[i];
		indx[i] = indx[index];
		indx[index] = aux2;
	}

	int *temporary_hits = (int *)malloc(play * sizeof(int));
	memcpy(temporary_hits, targeted_hits, play * sizeof(int));
	int modified = 0;
	//punem meciurile la acuratete 100% si ne oprim
	//cand ajungem la valoarea targetului
	for (int i = 0; i < play; i++) {
		int idx = indx[i];
		if (temporary_hits[idx] < total_hits[idx]) {
			temporary_hits[idx] = total_hits[idx];
			modified++;
			double new_accuracy = total_accuracy(temporary_hits,
									total_hits, ships_game, play);
			if (new_accuracy >= target_accuracy) {
				// inseamna ca am gasit numarul de meciuri minime
				free(indx);
				free(impact);
				free(temporary_hits);
				return modified;
			}
		}
	}
	free(indx);
	free(impact);
	free(temporary_hits);
	return modified;
}

//aflam numarul minim de lovituri
//ce nu pot fi ratate astfel incat acuratetea sa fie >= X%

int min_hits(int targeted_hits, int total_hits, double target_accuracy)
{
	double accuracy = match_accuracy(targeted_hits, total_hits);
	//este deja respectata cerinta
	if (accuracy >= target_accuracy) {
		return 0;
	}
	//vedem cate lovituri sunt necesare pentru acuratetea dorita
	double necessary_hits = total_hits * target_accuracy / 100;
	int necessary = (int)ceil(necessary_hits);
	double accuracy_now = (double)necessary / total_hits * 100.0;
	//daca tot nu am ajuns la targetul dorit, incrementam nr de lovituri
	if (accuracy_now < target_accuracy) {
		necessary++;
	}
	//avem nevoie de mai multe lovituri decat cele totale
	if (necessary > total_hits) {
		return total_hits - targeted_hits;
	}
	//cate lovituri ne mai trebuie
	return necessary - targeted_hits;
}

//aflam numarul maxim de meciuri care ar fi putut avea 0%
//acuratete astfel incat acuratetea totala sa fie >= X%

int max_0(int *targeted_hits, int *total_hits,
		  int *ships_game, int play, double target_accuracy)
{
	double accuracy = total_accuracy(targeted_hits, total_hits, ships_game, play);
	//acuratetea e deja mai mica decat targetul,
	//deci nu putem seta vreun meci la acuratete 0%
	if (accuracy < target_accuracy) {
		return 0;
	}
	int *indx = (int *)malloc(play * sizeof(int));
	double *impact = (double *)malloc(play * sizeof(double));
	//calculam impactul fiecarui meci ca sa vedem
	// cat pierdem din acuratete daca il setam la 0%
	for (int i = 0; i < play; i++) {
		indx[i] = i;
		if (total_hits[i] > 0) {
			double current_accuracy = (double)targeted_hits[i] / total_hits[i];
			impact[i] = current_accuracy * ships_game[i];
		} else {
			impact[i] = 0.0;
		}
	}
	//sortam meciurile in ordine crescatoare in functie de impact
	for (int i = 0; i < play - 1; i++) {
		int idx = i;
		for (int j = i + 1; j < play; j++) {
			if (impact[j] < impact[idx]) {
				idx = j;
			}
		}
		double aux = impact[i];
		impact[i] = impact[idx];
		impact[idx] = aux;
		int aux2 = indx[i];
		indx[i] = indx[idx];
		indx[idx] = aux2;
	}
	int *temporary_hits = (int *)malloc(play * sizeof(int));
	memcpy(temporary_hits, targeted_hits, play * sizeof(int));
	int match_0 = 0;
	//incercam sa setam cat mai multe meciuri la 0%
	for (int i = 0; i < play; i++) {
		int idx = indx[i];
		if (temporary_hits[idx] > 0) {
			temporary_hits[idx] = 0;
			double accuracy_now = total_accuracy(temporary_hits,
										total_hits, ships_game, play);
			//suntem inca peste targetul dorit
			if (accuracy_now >= target_accuracy) {
				match_0++;
			} else { //am atins acuratetea dorita,
			//deci ne oprim din a seta meciuri la 0%
				temporary_hits[idx] = targeted_hits[idx];
				break;
			}
		}
	}
	free(indx);
	free(impact);
	free(temporary_hits);
	return match_0;
}

//calculam numarul maxim de lovituri
//care ar fi putut fi ratate astfel incat acuratetea totala sa fie >= X%

int max_hits(int targeted_hits, int total_hits, double target_accuracy)
{
	//nu putem rata nimic daca nu avem atacuri
	if (total_hits == 0) {
		return 0;
	}
	//calculam cate lovituri minime ar trebui lovite pentru target
	double necessary_hits = total_hits * target_accuracy / 100.0;
	int min_hits = (int)ceil(necessary_hits);
	double accuracy_now = match_accuracy(min_hits, total_hits);
	//inca nu am ajuns la target, deci putem incrementa
	if (accuracy_now < target_accuracy) {
		min_hits++;
	}
	//avem mai putine lovituri decat minimul necesar
	if (targeted_hits < min_hits) {
		return 0;
	}
	//cate lovituri putem rata si
	//sa avem acuratetea inca mai mare sau egala cu targetul
	return targeted_hits - min_hits;
}

void process_command(char *command, int *targeted_hits_p1, int *total_hits_p1,
					 int *ships_game_p1, int *targeted_hits_p2, int *total_hits_p2,
					 int *ships_game_p2, int play, double *accuracy_out, int *result_out)
{
			//urmeaza sa descompunem comanda primita
			//care este de forma: jucator|tip1|indicemeci|targetacuratete|tip2
			char player = command[0]; //jucatorul 1 sau 2
			char type1 = command[1]; // toate meciurile sau doar unul
			char match[4];
			match[0] = command[2];
			match[1] = command[3];
			match[2] = command[4];
			match[3] = '\0';
			//aflam indicele meciului
			int indx_match = atoi(match);
			char x[10];
			int position = 5;
			int current_idx = 0;
			while (command[position] != 'M' && command[position] != '\0') {
				//parcurgem dupa indicele meciului si pana intalnim tip2(reprezentat de min sau max)
				x[current_idx++] = command[position++];
			}
			x[current_idx] = '\0';
			//aflam targetul de acuratete
			double target_acc = atof(x);
			//extragem tipul2, Min sau Max
			char type2[4];
			type2[0] = command[position];
			type2[1] = command[position + 1];
			type2[2] = command[position + 2];
			type2[3] = '\0';
			int *targeted_hits, *total_hits, *ships_game;
			if (player == 'O') { //primul jucator
				targeted_hits = targeted_hits_p1;
				total_hits = total_hits_p1;
				ships_game = ships_game_p1;
			} else { // al doilea jucator
				targeted_hits = targeted_hits_p2;
				total_hits = total_hits_p2;
				ships_game = ships_game_p2;
			}
			int result;
			double accuracy;
			// tratam cele 4 cazuri posibile
			if (type1 == 'T' && strcmp(type2, "MIN") == 0) { //cazul 1
				accuracy = total_accuracy(targeted_hits, total_hits, ships_game, play);
				result = min_100(targeted_hits, total_hits, ships_game, play, target_acc);
			} else if (type1 == 'U' && strcmp(type2, "MIN") == 0) { //cazul 2
				int indx = indx_match - 1; //vrem sa accesam meciul respectiv, iar indexarea s a realizat de la 0
				if (indx < 0 || indx >= play) {
					*accuracy_out = -1.0;
					*result_out = -1;
					return;
				}
				accuracy = match_accuracy(targeted_hits[indx], total_hits[indx]);
				result = min_hits(targeted_hits[indx], total_hits[indx], target_acc);
			} else if (type1 == 'T' && strcmp(type2, "MAX") == 0) { //cazul 3
				accuracy = total_accuracy(targeted_hits, total_hits, ships_game, play);
				result = max_0(targeted_hits, total_hits, ships_game, play, target_acc);
			} else if (type1 == 'U' && strcmp(type2, "MAX") == 0) {
				int indx = indx_match - 1;//la fel ca mai sus, avem nevoie de un meci anume
				if (indx < 0 || indx >= play) {
					*accuracy_out = -1.0;
					*result_out = -1;
					return;
				}
				accuracy = match_accuracy(targeted_hits[indx], total_hits[indx]);
				result = max_hits(targeted_hits[indx], total_hits[indx], target_acc);
			}
			*accuracy_out = accuracy;
			*result_out = result;
}		

//transformam un caracter in numar bazandu-ne pe alfabetul dat

int to_number(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'A' && c <= 'Z') {
		return 10 + (c - 'A');
	}
	if (c == '.') {
		return 36;
	}
	return -1;
}

//transformam un numar in caracter

char to_char(int n)
{
	if (n >= 0 && n <= 9) {
		return n + '0';
	}
	if (n >= 10 && n <= 35) {
		return 'A' + (n - 10);
	}
	if (n == 36) {
		return '.';
	}
	return '?';
}

//aplicam mod 37

int mod_37(int x)
{
	x = x % 37;
	if (x < 0) {
		x = x + 37;
	}
	return x;
}

//transformam textul in vector coloana cu numere

void text_transformation(char *text, int *v, int len)
{
	for (int i = 0; i < len; i++) {
		v[i] = to_number(text[i]);
	}
}

//construim matricea-cheie

void build_key(char *key, int **k, int n)
{
	int indx = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			k[i][j] = to_number(key[indx++]);
		}
	}
}

//aplicam mod 37 pentru fiecare element al matricei
// rezultate din inmultirea cheii cu vectorul coloana numeric

void mat_mod37(int **k, int *m, int *result, int n)
{
	for (int i = 0; i < n; i++) {
		result[i] = 0;
		for (int j = 0; j < n; j++) {
			result[i] = result[i] + k[i][j] * m[j];
		}
			result[i] = mod_37(result[i]);
	}
}

//functia pentru criptare

void encrypt(char *message, char *key, char *res)
{
	int len = strlen(key);
	int n = (int)sqrt(len);
	int final_len = strlen(message);
	int position = 0;
	//impartim mesajul in bucati de lungime n
	for (int i = 0; i < final_len; i = i + n) {
		int size;
		if (i + n <= final_len) {
			size = n; //e complet
		} else {
			size = final_len - i; // ultimul
		}
		int m[10], result[10];
		//transformam caracterele in valori numerice
		text_transformation(message + i, m, size);
		//alocam dinamic matricea cheie pe care apoi o construim
		int **k = allocate(size, size);
		if (!k) {
			res[position] = '\0';
			return;
		}
		build_key(key, k, size);
		//inmultim matricea cheie cu vectorul mesaj
		mat_mod37(k, m, result, size);
		//convertim numerele care ne au rezultat inapoi in caractere
		for (int j = 0; j < size; j++) {
			res[position++] = to_char(result[j]);
		}
		free_mat(k, size);
	}
	res[position] = '\0';
}

//inmultim o matrice cu un scalar

void multiply(int **a, int scalar, int n)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			a[i][j] = mod_37(a[i][j] * scalar);
		}
	}
}

//calculam determinantul unei matrice 2x2

int det_2(int **a)
{
	return a[0][0] * a[1][1] - a[0][1] * a[1][0];
}

//calculam adjuncta unei matrice 2X2, aplicand pentru fiecare element mod 37

void adj_2(int **a, int **adj)
{
	adj[0][0] = mod_37(a[1][1]);
	adj[0][1] = mod_37(-a[0][1]);
	adj[1][0] = mod_37(-a[1][0]);
	adj[1][1] = mod_37(a[0][0]);
}

//calculam inversul multipicitar

int multiplicative(int a)
{
	a = mod_37(a);
	for (int i = 1; i < 37; i++) {
		if ((a * i) % 37 == 1) {
			return i;
		}
	}
	return -1;
}

//calculam inversa unei matrice 2x2

void inv_2(int **a, int **inv)
{
	int det = det_2(a);
	int det_m = mod_37(det);
	if (det_m == 0) {
		printf("Matricea este neinversabila.\n");
		return;
	}
	int det_i = multiplicative(det_m);
	adj_2(a, inv);
	multiply(inv, det_i, 2);
}

//calculam determinantul unei matrice 3x3

int det_3(int **a)
{
	return ((a[0][0] * a[1][1] * a[2][2]) + (a[0][1] * a[1][2] * a[2][0]) +
	(a[0][2] * a[1][0] * a[2][1])) - ((a[0][2] * a[1][1] * a[2][0]) +
	(a[0][1] * a[1][0] * a[2][2]) + (a[0][0] * a[1][2] * a[2][1]));
}

//calculam adjuncta unei matrice 3x3

void adj_3(int **a, int **adj)
{
	// calculam factorii pentru adjuncta
	int t00 = a[1][1] * a[2][2] - a[1][2] * a[2][1];
	int t01 = -(a[1][0] * a[2][2] - a[1][2] * a[2][0]);
	int t02 = a[1][0] * a[2][1] - a[1][1] * a[2][0];
	int t10 = -(a[0][1] * a[2][2] - a[0][2] * a[2][1]);
	int t11 = a[0][0] * a[2][2] - a[0][2] * a[2][0];
	int t12 = -(a[0][0] * a[2][1] - a[0][1] * a[2][0]);
	int t20 = a[0][1] * a[1][2] - a[0][2] * a[1][1];
	int t21 = -(a[0][0] * a[1][2] - a[0][2] * a[1][0]);
	int t22 = a[0][0] * a[1][1] - a[0][1] * a[1][0];
	// transpunem si aplicam % 37
	adj[0][0] = mod_37(t00);
	adj[0][1] = mod_37(t10);
	adj[0][2] = mod_37(t20);
	adj[1][0] = mod_37(t01);
	adj[1][1] = mod_37(t11);
	adj[1][2] = mod_37(t21);
	adj[2][0] = mod_37(t02);
	adj[2][1] = mod_37(t12);
	adj[2][2] = mod_37(t22);
}

//calculam inversa unei matrice 3x3

void inv_3(int **a, int **inv)
{
	int det = det_3(a);
	int det_m = mod_37(det);
	if (det_m == 0) {
		printf("Matricea este neinversabila.\n");
		return;
	}
	int det_i = multiplicative(det_m);
	adj_3(a, inv);
	multiply(inv, det_i, 3);
}

//decriptarea
void decrypt(char *message, char *key, char *res)
{
	int len = strlen(key);
	int n = (int)sqrt(len);
	int message_len = strlen(message);
	int position = 0;
	//impartim mesajul in bucati de lungime n
	for (int i = 0; i < message_len; i = i + n) {
		int size;
		if (i + n <= message_len) {
			size = n;//e complet
		} else {
			size = message_len - i; // ultimul
		}
		int c[100], m[100];
		text_transformation(message + i, c, size);
		int **k = allocate(size, size);
		int **inv = allocate(size, size);
		if (!k || !inv) {
			free_mat(k, size);
			free_mat(inv, size);
			res[position] = '\0';
			return;
		}
		build_key(key, k, size);
		//calculam inversa matricei in functie de dimensiune
		if (size == 1) {
			int det_m = mod_37(k[0][0]);
			if (det_m == 0) {
				free_mat(k, size);
				free_mat(inv, size);
				res[position] = '\0';
				return;
			}
			inv[0][0] = multiplicative(det_m);
		} else if (size == 2) {
			inv_2(k, inv);
		} else if (size == 3) {
			inv_3(k, inv);
		} else {
			free_mat(k, size);
			free_mat(inv, size);
			res[position] = '\0';
			return;
		}
		//inmultim inversa cheii cu vectorul criptat si aplicam mod 37
		mat_mod37(inv, c, m, size);
		//convertim numerele care ne au rezultat inapoi in caractere
		for (int j = 0; j < size; j++) {
			res[position++] = to_char(m[j]);
		}
		free_mat(k, size);
		free_mat(inv, size);
	}
	res[position] = '\0';
}

int main(void)
{
	int play;
	//citim numarul de meciuri
	scanf("%d", &play);
	//alocam vectorii pentru statistici
	int *targeted_hits_p1 = (int *)calloc(play, sizeof(int));
	int *total_hits_p1 = (int *)calloc(play, sizeof(int));
	int *ships_game_p1 = (int *)calloc(play, sizeof(int));
	int *targeted_hits_p2 = (int *)calloc(play, sizeof(int));
	int *total_hits_p2 = (int *)calloc(play, sizeof(int));
	int *ships_game_p2 = (int *)calloc(play, sizeof(int));
	if (!targeted_hits_p1 || !total_hits_p1 || !ships_game_p1 ||
		!targeted_hits_p2 || !total_hits_p2 || !ships_game_p2) {
		printf("Nu s a putut aloca");
		return 1;
	}
	//parcurgem fiecare meci in parte
	for (int game = 0; game < play; game++) {
		int n, m;
		scanf("%d %d", &n, &m);
		//calculam cate nave de fiecare tip putem plasa
		int ship[5];
		ship_number(n, m, ship);
		int total_ships = ship[0] + ship[1] + ship[2] + ship[3] + ship[4];
		targeted_hits_p1[game] = 0;
		total_hits_p1[game] = 0;
		targeted_hits_p2[game] = 0;
		total_hits_p2[game] = 0;
		//alocam tablele de joc
		int **board1 = allocate(n, m);
		int **board2 = allocate(n, m);
		int **attacked1 = allocate(n, m);
		int **attacked2 = allocate(n, m);
			if (!board1 || !board2 || !attacked1 || !attacked2) {
				free_mat(board1, n);
				free_mat(board2, n);
				free_mat(attacked1, n);
				free_mat(attacked2, n);
			continue;
		}
		//alocam vectorii care retin informatii pentru
		//fiecare jucator in parte si navele pe care le plaseaza
		char *ship_type1 = (char *)calloc(total_ships, sizeof(char));
		int *ship_headx1 = (int *)calloc(total_ships, sizeof(int));
		int *ship_heady1 = (int *)calloc(total_ships, sizeof(int));
		char *ship_orientations1 = (char *)calloc(total_ships, sizeof(char));
		int *ship_length1 = (int *)calloc(total_ships, sizeof(int));
		int *ship_remaining_cells1 = (int *)calloc(total_ships, sizeof(int));
		int *ship_alive1 = (int *)calloc(total_ships, sizeof(int));
		char *ship_type2 = (char *)calloc(total_ships, sizeof(char));
		int *ship_headx2 = (int *)calloc(total_ships, sizeof(int));
		int *ship_heady2 = (int *)calloc(total_ships, sizeof(int));
		char *ship_orientations2 = (char *)calloc(total_ships, sizeof(char));
		int *ship_length2 = (int *)calloc(total_ships, sizeof(int));
		int *ship_remaining_cells2 = (int *)calloc(total_ships, sizeof(int));
		int *ship_alive2 = (int *)calloc(total_ships, sizeof(int));

		if (!ship_type1 || !ship_headx1 || !ship_heady1 || !ship_orientations1 ||
			!ship_length1 || !ship_remaining_cells1 || !ship_alive1 || !ship_type2 || !ship_headx2
			|| !ship_heady2 || !ship_orientations2 || !ship_length2 || !ship_remaining_cells2
			|| !ship_alive2) {
			free(ship_type1);
			free(ship_headx1);
			free(ship_heady1);
			free(ship_orientations1);
			free(ship_length1);
			free(ship_remaining_cells1);
			free(ship_alive1);
			free(ship_type2);
			free(ship_headx2);
			free(ship_heady2);
			free(ship_orientations2);
			free(ship_length2);
			free(ship_remaining_cells2);
			free(ship_alive2);
			free_mat(board1, n);
			free_mat(board2, n);
			free_mat(attacked1, n);
			free_mat(attacked2, n);
			continue;
		}

		//initializam niste contoare pentru meciul respectiv
		int ships_alive1 = total_ships;
		int ships_alive2 = total_ships;
		int ships_placed1 = 0;
		int ships_placed2 = 0;
		int valid_ships1 = 0;
		int valid_ships2 = 0;
		int current_player = 0;

		//asezam navele
		while (ships_placed1 < total_ships || ships_placed2 < total_ships) {
			//ne asiguram ca jucatorul a terminat de amplasat
			if ((current_player == 0 && ships_placed1 >= total_ships) ||
				(current_player == 1 && ships_placed2 >= total_ships)) {
					current_player = 1 - current_player; //schimbam jucatorul
					continue;
				}

			char type, orientation;
			int x, y;
			scanf(" %c %c %d %d", &type, &orientation, &x, &y);
			int length = ship_length(type);

			if (current_player == 0) {
				if (ships_placed1 >= total_ships) {
					current_player = 1 - current_player;
					continue;
				}
				//verificam daca nava ar putea fi plasata
				if (!can_place(board1, n, m, x, y, orientation, length)) {
					char orientation_text[10];
					if (orientation == 'H') {
						strcpy(orientation_text, "orizontal");
					} else {
						strcpy(orientation_text, "vertical");
					}
					printf("Nava %s nu poate fi amplasata %s la coordonatele (%d, %d).\n",
					ship_name(type), orientation_text, x, y);
					continue;
				}
				place_ship(board1, x, y, orientation, type, ship_type1, ship_headx1, ship_heady1,
				ship_orientations1, ship_length1, ship_remaining_cells1, ship_alive1, ships_placed1);
				ships_placed1++;
				valid_ships1++;
			} else { //se schimba jucatorul
				if (ships_placed2 >= total_ships) {
					current_player = 1 - current_player;
					continue;
				}
				if (!can_place(board2, n, m, x, y, orientation, length)) { //vedem daca poate fi plasata nava
					char orientation_text[10];
					if (orientation == 'H') {
						strcpy(orientation_text, "orizontal");
					} else {
						strcpy(orientation_text, "vertical");
					}
					printf("Nava %s nu poate fi amplasata %s la coordonatele (%d, %d).\n",
					ship_name(type), orientation_text, x, y);
					continue;
				}
				place_ship(board2, x, y, orientation, type, ship_type2, ship_headx2, ship_heady2,
				ship_orientations2, ship_length2, ship_remaining_cells2, ship_alive2, ships_placed2);
				ships_placed2++;
				valid_ships2++;
			}
			//schimbam jucatorii
			current_player = 1 - current_player;
		}
		ships_game_p1[game] = valid_ships1;
		ships_game_p2[game] = valid_ships2;
		//afisam tablele de joc
		print_bord(board1, n, m, ship_length1, total_ships);
		printf("\n");
		print_bord(board2, n, m, ship_length2, total_ships);
		current_player = 0;

		//faza atacurilor
		while (ships_alive1 > 0 && ships_alive2 > 0) {
			int attackx, attacky;
			scanf("%d %d", &attackx, &attacky);

			if (current_player == 0) {
				attack_process(board2, attacked1, attackx, attacky, 1, ship_type2, ship_headx2,
				ship_heady2, ship_orientations2, ship_length2, ship_remaining_cells2, ship_alive2,
				&ships_alive2, &targeted_hits_p1[game], &total_hits_p1[game], n, m, total_ships);
			} else {
				attack_process(board1, attacked2, attackx, attacky, 2, ship_type1, ship_headx1,
				ship_heady1, ship_orientations1, ship_length1, ship_remaining_cells1, ship_alive1,
				&ships_alive1, &targeted_hits_p2[game], &total_hits_p2[game], n, m, total_ships);
			}
			current_player = 1 - current_player;
		}
		//afisam castigatorul
		if (ships_alive1 > 0 || ships_alive2 > 0) {
			if (ships_alive1 > 0 && ships_alive2 == 0) {
				printf("Jucatorul 1 a castigat.\n");
			} else if (ships_alive2 > 0 && ships_alive1 == 0) {
				printf("Jucatorul 2 a castigat.\n");
			}
		}
		free_mat(board1, n);
		free_mat(board2, n);
		free_mat(attacked1, n);
		free_mat(attacked2, n);
		free(ship_type1);
		free(ship_headx1);
		free(ship_heady1);
		free(ship_orientations1);
		free(ship_length1);
		free(ship_remaining_cells1);
		free(ship_alive1);
		free(ship_type2);
		free(ship_headx2);
		free(ship_heady2);
		free(ship_orientations2);
		free(ship_length2);
		free(ship_remaining_cells2);
		free(ship_alive2);

	}
	//partea de statistici
	char *key = (char *)calloc(100, sizeof(char));
	if (!key) {
		free(key);
	}
	scanf("%s", key);
	// primul caz in care cheia nu este criptata
	if (strcmp(key, "-") == 0) {
		char command[1000];
		while (scanf("%s", command) == 1) {
			if (strcmp(command, "Q") == 0) {
			break;
			}
			int result;
			double accuracy;
			process_command(command, targeted_hits_p1, total_hits_p1,
							ships_game_p1, targeted_hits_p2, total_hits_p2,
							ships_game_p2, play, &accuracy, &result);
				if (accuracy < 0) {
					printf("Nu exista date.\n");
					continue;
				}
			int cpy_acc = (int)(accuracy * 100);
			printf("%03d.%02d.%d\n", cpy_acc / 100, cpy_acc % 100, result);
		}
	} else {
		//avem comenzi criptate si vrem rezultatele sa fie tot criptate
		char encrypted[1000];
		while (scanf("%s", encrypted) == 1) {
			if (strcmp(encrypted, "Q") == 0) {
				break;
			}
			char decrypted[100];
			decrypt(encrypted, key, decrypted);
			int result;
			double accuracy;
			process_command(decrypted, targeted_hits_p1, total_hits_p1,
							ships_game_p1, targeted_hits_p2, total_hits_p2,
							ships_game_p2, play, &accuracy, &result);
				if (accuracy < 0) {
					printf("Nu exista date.\n");
					continue;
				}

			//formatam acuratetea si rezultatul astfel incat sa aiba forma ceruta
			//pentru a ne asigura ca vom cripta ceva de forma xxx.yy.z
			char encrypt_f[100];
			int acc = (int)(accuracy * 100);
			int z = acc / 100; //retinem partea intreaga
			int y = acc % 100; //retinem partea zecimala
			int pos = 0;
			//forma xxx
			encrypt_f[pos++] = '0' + (z / 100);
			encrypt_f[pos++] = '0' + ((z / 10) % 10);
			encrypt_f[pos++] = '0' + (z % 10);
			//punctul1
			encrypt_f[pos++] = '.';
			//forma yy
			encrypt_f[pos++] = '0' + (y / 10);
			encrypt_f[pos++] = '0' + (y % 10);
			//punctul2
			encrypt_f[pos++] = '.';
			//rezultatul
			if (result == 0) {
				encrypt_f[pos++] = '0';
			} else {
			char temp[20];
			int temp_pos = 0;
			int rez_copy = result;
			while (rez_copy > 0) {
				temp[temp_pos++] = '0' + (rez_copy % 10);
				rez_copy /= 10;
			}
			for (int i = temp_pos - 1; i >= 0; i--) {
				encrypt_f[pos++] = temp[i];
			}
		}
			encrypt_f[pos] = '\0';
			char encrypted_res[100];
			encrypt(encrypt_f, key, encrypted_res);
			printf("%s\n", encrypted_res);
		}
	}
		free(key);
		free(targeted_hits_p1);
		free(total_hits_p1);
		free(ships_game_p1);
		free(targeted_hits_p2);
		free(total_hits_p2);
		free(ships_game_p2);
		return 0;
}
