#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

//definice barev použitých pro nápovědu a chybová hlášení.
#define RED		"\x1B[31m"
#define CYAN	"\x1b[36m"
#define RESET	"\x1b[0m"

//Vypisuje nápovědu.
void help(){
	printf(CYAN "\n==help==\n"
				"A program used to edit text formating.\n"
				"EXAMPLE OF USE:\n\n"
				"./proj1 commands.txt<input.txt\n\n"
				"where file 'commands.txt' contains a set of commands for formating text in file 'input.txt'. Each command on a new line ended with command 'q'.\n\n"
				"iCONTENT\t\tinserts CONTENT above the current line.\n"
				"bCONTENT\t\tinserts CONTENT at the beginning of the current line\n"
				"aCONTENT\t\tinserts CONTENT at the end of the current line\n"
				"s PATTERN REPLACEMENT\t replaces Pattern with replacement\n"
				"s:PATTERN:REPLACEMENT\t replaces Pattern with replacement. (Pattern and replacement can be seperated by any printable character.)\n"
				"S:PATTERN:REPLACEMENT\t replaces all patterns on the current line with replacement.\n"
				"d\t\t\tdeletes the current line\n"
				"dN\t\t\tdeletes N lines (N is representing a number > 0)\n"
				"r\t\t\tremoves the END OF LINE (EOL) on the current line\n"
				"e\t\t\tadds the END OF LINE (EOL) to the current line\n"
				"n\t\t\tmoves to editing next line\n"
				"nN\t\t\tmoves to editing Ns line (N is representing a number > 0)\n"
				"gX\t\t\tgoes back or forward to executing commands\n"
				"q\t\t\tquits executing commands and prints the rest of unedited lines\t//must be used at the end of the document containing commands\n" RESET);
}

//převádí číslá z příkazů, které následují po identefikátoru příkazu na datový typ "int".
int my_atoi(char commands[1002]){
	int i = strlen(commands) -1, result = 0, tmp = 1;
	while(i > 0)
	{
		if (commands[i] >= 48 && commands[i] <= 57){		//pokud je znak číslo, tak se převede na datový typ int.
			result += (commands[i] - 48) * tmp;
			tmp *= 10;
			i--;
		}
		else{
			//Pokud znak není číslo, program bude varovat uživatele.
			//Tento warning se převádí na funkce, které používají tuto funkci.
			fprintf(stderr, RED "Warning. In commands 'gN' and 'nN' must be number instead of 'N'.\n" RESET);
			help();
			exit(1);
		}
	}
	return result;
}

//Přidává konec řádku.
void add_endl(char *line){
	line[strlen(line)] = '\n';
}

//Maže konce řádků.
void rm_endl(char *line){
	int len = strlen(line);
	if(line[len - 1] == '\n')
	{
		line[len-1] = '\0';
	}
}

//Maže celé řádky.
void delete_line(char *line){
	int len = strlen(line);
	for(int i = 0; i < len; i++)
	{
		line[i] = '\0';
	}
}

//Jestliže je použit ukončující příkaz "q", tato funkce zařídí, že se vypíše zbytek řádků ze vstupu, se kterýma nebylo manipulováno.
void no_more_commands(){
	char line[1002]; 
	while(fgets(line, 1001, stdin) != 0)
	{
		printf("%s", line);
	}
}

//Přeskakuje řádky vstupu o počet n.
void nN(char commands[1002], char *line){
	int count = my_atoi(commands);
		for(int j = 1; j < count; j++)
			{
				printf("%s", line);
				delete_line(line);
				fgets(line, 1001, stdin);
			}
}

//Vkládá obsah na začátku řádku.
void bCONTENT(char commands[1002], char *line){
	int i = 1;
	char tmp[1001];
	rm_endl(line);
	strcpy(tmp, line);
	delete_line(line);
	while(commands[i] != '\0')
	{
		line[i-1] = commands[i];
		i++;
	}
	strcat(line, tmp);
	add_endl(line);
}

//Vkládá obsah před aktualní řádek.
void iCONTENT(char commands[1002], char *line){
	int i = 1;
	char tmp[1001];
	rm_endl(line);
	strcpy(tmp, line);
	delete_line(line);
	while(commands[i] != '\0')
	{
		line[i-1] = commands[i];
		i++;
	}
	line[i-1] = '\n';
	strcat(line, tmp);
	add_endl(line);
}

//Vkládá obsah na konec řádku.
void aCONTENT(char commands[1002], char *line){
	rm_endl(line);
	int len = strlen(line) - 1, i = 1;
	while(commands[i] != '\0')
	{
		line[len + i] = commands[i];
		i++;
	}
	add_endl(line);
}

//Nahrazuje slova v proměnné "line".
int sreplace(char commands[1002], char *line){
	int i = 2, j = 0;
	char pattern[1002], replacement[1002], separator, *position;
	rm_endl(line);
	separator = commands[1];
	//Rozkládá příkaz na pattern a replacement.
	while(commands[i] != separator)
	{
		pattern[i - 2] = commands[i];
		i++;
	}
	i++;
	while(commands[i] != '\0')
	{
		replacement[j] = commands[i];
		i++;
		j++;
	}

	position = strstr(line, pattern);						//Navrací pointer na začátek patternu v promměné "line".
	
	if(position != NULL){									//Pokud něco navrací.
		char remains[1002];
		strcpy(remains, position + strlen(pattern));		//Do proměnné zbytek "remains" kopíruje znaky z proměnné "line" od konce patternu.
		int replacement_len = strlen(replacement);
		for(i = 0; i < replacement_len; i++){
			position[i] = replacement[i];
		}
		position[i] = '\0';
		strcat(line, remains);
		add_endl(line);
		return 1;											//Opakuje se, dokud proměnná "line" obsahuje pattern.
	}
	else{
		add_endl(line);
		return 0;											//Pokud už proměnná "line" neobsahuje pattern. Neopakuje se.
	}
}

int main(int argc, char *argv[])
{
	char commands[1002][1002], line[1002];
	int i = 0;
	FILE *soubor;

	//Pokud není při spouštění programu uveden soubor s příkazy, vypíše se chyba.
	if(argc < 2)
	{
		fprintf(stderr, RED "Warning. Program needs an input.\n" RESET);
		help();
		return 1;
	}

	//Vyvolání funkce help, pokud je použit jeden z argumentů uvedených v podmínce.
	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "-h") == 0){
			help();
			return 0;
		}
	}
		
	//Otevření souboru a kontrola úspěšnosti.
	soubor = fopen(argv[1], "r");
	if(soubor == NULL)
	{
		fprintf(stderr, RED "\nWarning. An error occurred while loading a file. %s. Program will be terminated.\n" RESET, strerror(errno));
		help();
		return 1;
	}

	//Načítaní příkazů do paměti.
	while(fgets(commands[i], 1001, soubor) != 0)
	{	
		rm_endl(commands[i]);
		i++;
	}

	//Postupné načítaní jednotlivých řádků ze vstupu do paměti.
	int index = 0;
	while(fgets(line, 1001, stdin) != 0)
	{
		//Pokud je použit příkaz "n", program přeskočí na exekuci dalšího příkazu v pořadí.
		while(commands[index][0] != 'n')
		{
			//Rozhodování, který příkaz použít.
			switch(commands[index][0])
			{
				case 'i':
					iCONTENT(commands[index], line);
					break;
				case 'b':
					bCONTENT(commands[index], line);
					break;
				case 'a':
					aCONTENT(commands[index], line);
					break;
				case 's':
					sreplace(commands[index], line);
					break;
				case 'S':
					//Bude se opakovat, dokud bude funkce navracet hodnotu 1.
					while(sreplace(commands[index], line) != 0);
					break;
				case 'd':
					//pokud příkaz "d" obsahuje další znaky, převedou se tyto znaky na číslo a vymaže se tolik řádků.
					if(strlen(commands[index]) > 1){
						int number_of_lines = my_atoi(commands[index]);
						for(int i = 0; i < number_of_lines; i++){
							delete_line(line);
							fgets(line, 1001, stdin);
						}
					}
					else{
						delete_line(line);
						fgets(line, 1001, stdin);
					}
					break;
				case 'r':
					rm_endl(line);
					break;
				case 'g':
					index = my_atoi(commands[index]) - 2;
					break;
				case 'e':
					add_endl(line);
					break;
				case 'q':
					printf("%s", line);
					no_more_commands();
					return 0;
				default:
					break;
			}
			index++;
		}

		//Pokud jsou za příkazem "n" obsažené další znaky, program příkaz vyhodnotí jako příkaz "nN".
		if(strlen(commands[index]) > 1)
		{
			nN(commands[index], line);
		}
		printf("%s", line);
		delete_line(line);
		index++;
	}

	//Uzavření vstupního souboru.
	if (soubor != NULL)
	{
		fclose(soubor);
	}
	return 0;
}