#ifndef weather_cli_H
#define weather_cli_H

#define URL_HETKEILM "https://www.ilmateenistus.ee/ilma_andmed/xml/observations.php"
#define URL_ILMAPROGNOOS "https://www.ilmateenistus.ee/ilma_andmed/xml/forecast.php"
#define MAX_FAV 100
#define MAX_RIDA 50
#define TUND 3600
#define URL 256
#define FAV_FILE "lemmikud.txt"

typedef struct {
    char *maluruum;
    size_t suurus;
} Malustruktuur;

// Struktuur, mis salvestab viimati laaditud andmed ja vastava URL-i
typedef struct {
    char *data;
    char url[URL];
} LaaditudAndmed;

static size_t KirjutaMalusseCallback(void *sisu, size_t suurus, size_t nmemb, void *kasutaja);
void laadiLemmikud(char lemmikud[MAX_FAV][MAX_RIDA], int *lemmikuteArv);
void salvestaLemmikud(char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv);
int kasJaamOnLemmik(char *jaam, char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv);
void tootleHetkeilmXML(const char *xmlSisu, char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv);
void tootleIlmaprognoosXML(const char *xmlSisu);
void vabastaLaaditudAndmed(LaaditudAndmed *laaditudAndmed);
void laadiJaTootleXML(const char *url, void (*tootleXML)(const char *, char[MAX_FAV][MAX_RIDA], int), char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv);
int onValidJaam(char *jaam, const char *xmlSisu);
int onTuhiVoiNull(char *str);

#endif 
