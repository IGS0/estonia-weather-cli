#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <time.h>
#include "weather_cli.h"


int main(void) {
    char lemmikud[MAX_FAV][MAX_RIDA];
    int lemmikuteArv = 0;

    laadiLemmikud(lemmikud, &lemmikuteArv);

    printf("\nAndmed olid võetud veebilehelt: https://www.ilmateenistus.ee.\n");
    
    printf("Hetkeilm lemmik-asukohtades:\n");
    laadiJaTootleXML(URL_HETKEILM, tootleHetkeilmXML, lemmikud, lemmikuteArv);

    while (1) {
        int valik;
        char sisend[MAX_RIDA];
		printf("\nAndmed olid võetud veebilehelt: ilmateenistus.ee.");
        printf("\nVali toiming:\n");
        printf("1. Naita hetkeilma\n");
        printf("2. Naita ilmaprognoosi\n");
        printf("3. Lisa lemmik-asukoht\n");
        printf("4. Eemalda lemmik-asukoht\n");
        printf("0. Valju\n");
        printf("Sisesta valik (0-4): ");

        fgets(sisend, sizeof(sisend), stdin);
        if (sscanf(sisend, "%d", &valik) == 1) {
            if (valik == 0) {
                printf("Programm lopetatakse.\n");
                break;
            } else if (valik == 1) {
                printf("\nHetkeilm lemmik-asukohtades:\n");
                laadiJaTootleXML(URL_HETKEILM, tootleHetkeilmXML, lemmikud, lemmikuteArv);
            } else if (valik == 2) {
                printf("\nIlmaprognoos:\n");
                laadiJaTootleXML(URL_ILMAPROGNOOS, tootleIlmaprognoosXML, lemmikud, lemmikuteArv);
                

            } else if (valik == 3) {
                printf("Sisesta uus lemmik-asukoht: ");
                fgets(sisend, sizeof(sisend), stdin);
                sisend[strcspn(sisend, "\n")] = '\0';  // Eemalda reavahetus

                // Laadi hetkeilm, et kontrollida jaama nime kehtivust
                Malustruktuur chunk = { .maluruum = NULL, .suurus = 0 };
                CURL *curl_handle = curl_easy_init();
                curl_easy_setopt(curl_handle, CURLOPT_URL, URL_HETKEILM);
                curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, KirjutaMalusseCallback);
                curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
                CURLcode res = curl_easy_perform(curl_handle);
                if(res != CURLE_OK) {
                    fprintf(stderr, "curl_easy_perform() ebaonnestus: %s\n", curl_easy_strerror(res));
                }
                curl_easy_cleanup(curl_handle);

                if (chunk.maluruum && onValidJaam(sisend, chunk.maluruum)) {
                    if (lemmikuteArv < MAX_FAV) {
                        strcpy(lemmikud[lemmikuteArv++], sisend);
                        salvestaLemmikud(lemmikud, lemmikuteArv);
                        printf("Lemmik-asukoht lisatud: %s\n", sisend);
                    } else {
                        printf("Lemmik-asukohtade arv on tais!\n");
                    }
                } else {
                    printf("Vigane asukoht voi ei leitud: %s\n", sisend);
                }

                free(chunk.maluruum);
            } else if (valik == 4) {
                printf("Sisesta eemaldatav lemmik-asukoht: ");
                fgets(sisend, sizeof(sisend), stdin);
                sisend[strcspn(sisend, "\n")] = '\0';  // Eemalda reavahetus

                int leitud = 0;
                for (int i = 0; i < lemmikuteArv; i++) {
                    if (strcmp(lemmikud[i], sisend) == 0) {
                        leitud = 1;
                        for (int j = i; j < lemmikuteArv - 1; j++) {
                            strcpy(lemmikud[j], lemmikud[j + 1]);
                        }
                        lemmikuteArv--;
                        salvestaLemmikud(lemmikud, lemmikuteArv);
                        printf("Lemmik-asukoht eemaldatud: %s\n", sisend);
                        break;
                    }
                }
                if (!leitud) {
                    printf("Lemmik-asukohta ei leitud: %s\n", sisend);
                }
            } else {
                printf("Vale valik! Palun sisesta uuesti.\n");
            }
        } else {
            printf("Vale sisend! Palun sisesta uuesti.\n");
        }
        
    }

    return 0;
}

/**
 * Description:    Kontrollib kas on vajalikud andmed
 * 
 * Parameters:     str - XML-i sisu
 * 
 * Return:         none
 */
// Abifunktsioon tühja stringi kontrollimiseks
int onTuhiVoiNull(char *str) {
    return str == NULL || str[0] == '\0';
}

/**
 * Description:    Callback-funktsioon, mida cURL kasutab andmete kirjutamiseks mällu.
 * 
 * Parameters:     sisu - allalaaditud andmete plokk
 *                 suurus - ühe elemendi suurus
 *                 nmemb - elementide arv
 *                 kasutaja - kasutaja andmed (Malustruktuur)
 * 
 * Return:         Tegeliku kirjutatud andmete suurus
 */
static size_t KirjutaMalusseCallback(void *sisu, size_t suurus, size_t nmemb, void *kasutaja) {
    size_t tegelik_suurus = suurus * nmemb;
    Malustruktuur *malustr = (Malustruktuur *)kasutaja;

    char *ptr = realloc(malustr->maluruum, malustr->suurus + tegelik_suurus + 1);
    if(ptr == NULL) {
        printf("Pole piisavalt malu (realloc tagastas NULL)\n");
        return 0;
    }

    malustr->maluruum = ptr;
    memcpy(&(malustr->maluruum[malustr->suurus]), sisu, tegelik_suurus);
    malustr->suurus += tegelik_suurus;
    malustr->maluruum[malustr->suurus] = 0;

    return tegelik_suurus;
}

/**
 * Description:    Laadib lemmikjaamade nimed failist või määrab vaikeväärtused.
 * 
 * Parameters:     lemmikud - massiiv lemmikjaamade nimedega
 *                 lemmikuteArv - viide lemmikjaamade arvule
 * 
 * Return:         none
 */
void laadiLemmikud(char lemmikud[MAX_FAV][MAX_RIDA], int *lemmikuteArv) {
    FILE *file = fopen(FAV_FILE, "r");
    if (file) {
        while (fgets(lemmikud[*lemmikuteArv], MAX_RIDA, file)) {
            lemmikud[*lemmikuteArv][strcspn(lemmikud[*lemmikuteArv], "\n")] = '\0';  // Eemalda reavahetus
            (*lemmikuteArv)++;
        }
        fclose(file);
    } else {
        // Faili pole olemas, kasutame vaikevaartusi
        strcpy(lemmikud[0], "Tallinn-Harku");
        strcpy(lemmikud[1], "Tartu-Tõravere");
        strcpy(lemmikud[2], "Narva");
        *lemmikuteArv = 3;
    }
}

/**
 * Description:    Salvestab lemmikjaamade nimed faili.
 * 
 * Parameters:     lemmikud - massiiv lemmikjaamade nimedega
 *                 lemmikuteArv - lemmikjaamade arv
 * 
 * Return:         none
 */
void salvestaLemmikud(char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv) {
    FILE *file = fopen(FAV_FILE, "w");
    if (file) {
        for (int i = 0; i < lemmikuteArv; i++) {
            fprintf(file, "%s\n", lemmikud[i]);
        }
        fclose(file);
    } else {
        printf("Lemmikute salvestamine ebaõnnestus!\n");
    }
}

/**
 * Description:    Kontrollib, kas jaam on lemmikjaamade nimekirjas.
 * 
 * Parameters:     jaam - jaama nimi
 *                 lemmikud - massiiv lemmikjaamade nimedega
 *                 lemmikuteArv - lemmikjaamade arv
 * 
 * Return:         1, kui jaam on lemmikjaam, vastasel juhul 0
 */
int kasJaamOnLemmik(char *jaam, char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv) {
    for (int i = 0; i < lemmikuteArv; i++) {
        if (strcmp(jaam, lemmikud[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Description:    Töötleb hetkeilma XML-i ja kuvab lemmikjaamade andmed.
 * 
 * Parameters:     xmlSisu - XML-i sisu string
 *                 lemmikud - massiiv lemmikjaamade nimedega
 *                 lemmikuteArv - lemmikjaamade arv
 * 
 * Return:         none
 */
void tootleHetkeilmXML(const char *xmlSisu, char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv) {
    xmlDocPtr doc = xmlReadMemory(xmlSisu, strlen(xmlSisu), "noname.xml", NULL, 0);
    if (doc == NULL) {
        printf("XML-i parsimine ebaõnnestus\n");
        return;
    }

    xmlNodePtr juur_element = xmlDocGetRootElement(doc);
    xmlNodePtr jaam = juur_element->children;

    while (jaam) {
        if (jaam->type == XML_ELEMENT_NODE && xmlStrcmp(jaam->name, (const xmlChar *)"station") == 0) {
            xmlNodePtr nimiNode = jaam->children;
            char *jaamaNimi = NULL;
            char *ilmastikunahtus = NULL;
            char *sademed = NULL;
            char *ohurohk = NULL;
            char *suhtelineOhuniiskus = NULL;
            char *ohutemperatuur = NULL;
            char *tuuleKiirus = NULL;
            char *tuuleKiirusMax = NULL;
            char *longitude = NULL;
            char *latitude = NULL;

            while (nimiNode) {
                if (nimiNode->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(nimiNode->name, (const xmlChar *)"name") == 0) {
                        jaamaNimi = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"phenomenon") == 0) {
                        ilmastikunahtus = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"precipitations") == 0) {
                        sademed = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"airpressure") == 0) {
                        ohurohk = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"relativehumidity") == 0) {
                        suhtelineOhuniiskus = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"airtemperature") == 0) {
                        ohutemperatuur = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"windspeed") == 0) {
                        tuuleKiirus = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"windspeedmax") == 0) {
                        tuuleKiirusMax = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"longitude") == 0) {
                        longitude = (char *)xmlNodeGetContent(nimiNode);
                    } else if (xmlStrcmp(nimiNode->name, (const xmlChar *)"latitude") == 0) {
                        latitude = (char *)xmlNodeGetContent(nimiNode);
                    }
                }
                nimiNode = nimiNode->next;
            }

            if (jaamaNimi && kasJaamOnLemmik(jaamaNimi, lemmikud, lemmikuteArv)) {
                printf("\nJaam: %s\n", jaamaNimi);
                printf("  Ilmastikunahtus: %s\n", onTuhiVoiNull(ilmastikunahtus) ? "N/A" : ilmastikunahtus);
                printf("  Sademed: %s mm\n", onTuhiVoiNull(sademed) ? "N/A" : sademed);
                printf("  Õhurõhk: %s hPa\n", onTuhiVoiNull(ohurohk) ? "N/A" : ohurohk);
                printf("  Suhteline õhuniiskus: %s%%\n", onTuhiVoiNull(suhtelineOhuniiskus) ? "N/A" : suhtelineOhuniiskus);
                printf("  Õhutemperatuur: %s°C\n", onTuhiVoiNull(ohutemperatuur) ? "N/A" : ohutemperatuur);
                printf("  Tuule kiirus: %s m/s\n", onTuhiVoiNull(tuuleKiirus) ? "N/A" : tuuleKiirus);
                printf("  Tuule maksimaalne kiirus: %s m/s\n", onTuhiVoiNull(tuuleKiirusMax) ? "N/A" : tuuleKiirusMax);
                if (longitude && latitude) {
                    printf("  Asukoht: https://www.openstreetmap.org/?mlat=%s&mlon=%s\n", latitude, longitude);
                }
            }

            if (jaamaNimi) xmlFree(jaamaNimi);
            if (ilmastikunahtus) xmlFree(ilmastikunahtus);
            if (sademed) xmlFree(sademed);
            if (ohurohk) xmlFree(ohurohk);
            if (suhtelineOhuniiskus) xmlFree(suhtelineOhuniiskus);
            if (ohutemperatuur) xmlFree(ohutemperatuur);
            if (tuuleKiirus) xmlFree(tuuleKiirus);
            if (tuuleKiirusMax) xmlFree(tuuleKiirusMax);
            if (longitude) xmlFree(longitude);
            if (latitude) xmlFree(latitude);
        }
        jaam = jaam->next;
    }

    xmlFreeDoc(doc);
}

/**
 * Description:    Töötleb ilmaprognoosi XML-i ja kuvab andmed.
 * 
 * Parameters:     xmlSisu - XML-i sisu string
 * 
 * Return:         none
 */
void tootleIlmaprognoosXML(const char *xmlSisu) {
    xmlDocPtr doc = xmlReadMemory(xmlSisu, strlen(xmlSisu), "noname.xml", NULL, 0);
    if (doc == NULL) {
        printf("XML-i parsimine ebaõnnestus\n");
        return;
    }

    xmlNodePtr juur_element = xmlDocGetRootElement(doc);
    xmlNodePtr prognoos = juur_element->children;

    while (prognoos) {
        if (prognoos->type == XML_ELEMENT_NODE && xmlStrcmp(prognoos->name, (const xmlChar *)"forecast") == 0) {
            xmlNodePtr kuupaevNode = prognoos->children;
            char *kuupaev = (char *)xmlGetProp(prognoos, (const xmlChar *)"date");
            char *ooTempMin = NULL;
            char *ooTempMax = NULL;
            char *paevTempMin = NULL;
            char *paevTempMax = NULL;

            while (kuupaevNode) {
                if (kuupaevNode->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(kuupaevNode->name, (const xmlChar *)"night") == 0) {
                        xmlNodePtr ooNode = kuupaevNode->children;
                        while (ooNode) {
                            if (xmlStrcmp(ooNode->name, (const xmlChar *)"tempmin") == 0) {
                                ooTempMin = (char *)xmlNodeGetContent(ooNode);
                            } else if (xmlStrcmp(ooNode->name, (const xmlChar *)"tempmax") == 0) {
                                ooTempMax = (char *)xmlNodeGetContent(ooNode);
                            }
                            ooNode = ooNode->next;
                        }
                    } else if (xmlStrcmp(kuupaevNode->name, (const xmlChar *)"day") == 0) {
                        xmlNodePtr paevNode = kuupaevNode->children;
                        while (paevNode) {
                            if (xmlStrcmp(paevNode->name, (const xmlChar *)"tempmin") == 0) {
                                paevTempMin = (char *)xmlNodeGetContent(paevNode);
                            } else if (xmlStrcmp(paevNode->name, (const xmlChar *)"tempmax") == 0) {
                                paevTempMax = (char *)xmlNodeGetContent(paevNode);
                            }
                            paevNode = paevNode->next;
                        }
                    }
                }
                kuupaevNode = kuupaevNode->next;
            }

            printf("Kuupaev: %s\n", kuupaev);
            printf("  Öö: Min temp: %s°C, Max temp: %s°C\n", onTuhiVoiNull(ooTempMin) ? "N/A" : ooTempMin, onTuhiVoiNull(ooTempMax) ? "N/A" : ooTempMax);
            printf("  Päev: Min temp: %s°C, Max temp: %s°C\n", onTuhiVoiNull(paevTempMin) ? "N/A" : paevTempMin, onTuhiVoiNull(paevTempMax) ? "N/A" : paevTempMax);

            if (kuupaev) xmlFree(kuupaev);
            if (ooTempMin) xmlFree(ooTempMin);
            if (ooTempMax) xmlFree(ooTempMax);
            if (paevTempMin) xmlFree(paevTempMin);
            if (paevTempMax) xmlFree(paevTempMax);
        }
        prognoos = prognoos->next;
    }

    xmlFreeDoc(doc);
}

/**
 * Description:    Vabastab eelnevalt laaditud andmed.
 * 
 * Parameters:     laaditudAndmed - viide laaditud andmete struktuurile
 * 
 * Return:         none
 */
// Funktsioon eelnevalt laaditud andmete vabastamiseks
void vabastaLaaditudAndmed(LaaditudAndmed *laaditudAndmed) {
    if (laaditudAndmed->data != NULL) {
        free(laaditudAndmed->data);
        laaditudAndmed->data = NULL;
    }
}

/**
 * Description:    Laadib XML-i veebist ja töötleb seda(kontrollib laadimise aega).
 * 
 * Parameters:     url - XML-i URL
 *                 tootleXML - funktsioon, mis töötleb laaditud XML-i
 *                 lemmikud - massiiv lemmikjaamade nimedega
 *                 lemmikuteArv - lemmikjaamade arv
 * 
 * Return:         none
 */
void laadiJaTootleXML(const char *url, void (*tootleXML)(const char *, char[MAX_FAV][MAX_RIDA],
						int), char lemmikud[MAX_FAV][MAX_RIDA], int lemmikuteArv) {
    static time_t lastFetchTime_URL_HETKEILM = 0;
    static time_t lastFetchTime_URL_ILMAPROGNOOS = 0;
    static LaaditudAndmed laaditudAndmed_HETKEILM;
    static LaaditudAndmed laaditudAndmed_ILMAPROGNOOS;
    
    time_t currentTime = time(NULL);
    time_t *lastFetchTime;
    LaaditudAndmed *laaditudAndmed;

    // Kontrolli, kas antud URL on tuntud
    if (strcmp(url, URL_HETKEILM) != 0 && strcmp(url, URL_ILMAPROGNOOS) != 0) {
        fprintf(stderr, "Tundmatu URL: %s\n", url);
        return;
    }

    // Maara sobivad ajatempli ja eelnevalt laaditud andmete muutujad vastavalt URL-le
    if (strcmp(url, URL_HETKEILM) == 0) {
        lastFetchTime = &lastFetchTime_URL_HETKEILM;
        laaditudAndmed = &laaditudAndmed_HETKEILM;
    } else if (strcmp(url, URL_ILMAPROGNOOS) == 0) {
        lastFetchTime = &lastFetchTime_URL_ILMAPROGNOOS;
        laaditudAndmed = &laaditudAndmed_ILMAPROGNOOS;
    }

    // Kontrolli, kas on moodunud vahemalt tund viimasest allalaadimisest
    if (difftime(currentTime, *lastFetchTime) < TUND) {
        printf("Andmeid ei laadita sagedamini kui kord tunnis. Viimane allalaadimine oli %ld sekundit tagasi.\n", (long)difftime(currentTime, *lastFetchTime));
        if (laaditudAndmed->data != NULL) {
            // Kasutame eelnevalt laaditud andmeid
            tootleXML(laaditudAndmed->data, lemmikud, lemmikuteArv);
        }
        return;
    }

    CURL *curl_handle;
    CURLcode res;

    Malustruktuur chunk;
    chunk.maluruum = malloc(1);
    chunk.suurus = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, KirjutaMalusseCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() ebaõnnestus: %s\n", curl_easy_strerror(res));

    } else {
        tootleXML(chunk.maluruum, lemmikud, lemmikuteArv);
        *lastFetchTime = currentTime; // Uuenda viimase allalaadimise aega
        
        // Vabastame eelnevalt laaditud andmed
        vabastaLaaditudAndmed(laaditudAndmed);
        
        // Salvestame uued laaditud andmed ja vastava URL-i
        laaditudAndmed->data = chunk.maluruum;
        strcpy(laaditudAndmed->url, url);

    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
 
//~ free(chunk.maluruum);/**#########################################################################################**/


}

/**
 * Description:    Kontrollib, kas jaama nimi on kehtiv, võrreldes seda XML-i sisuga.
 * 
 * Parameters:     jaam - jaama nimi
 *                 xmlSisu - XML-i sisu string
 * 
 * Return:         1, kui jaama nimi on kehtiv, vastasel juhul 0
 */
int onValidJaam(char *jaam, const char *xmlSisu) {
    xmlDocPtr doc = xmlReadMemory(xmlSisu, strlen(xmlSisu), "noname.xml", NULL, 0);
    if (doc == NULL) {
        printf("XML-i parsimine ebaonnestus\n");
        return 0;
    }

    xmlNodePtr juur_element = xmlDocGetRootElement(doc);
    xmlNodePtr jaamNode = juur_element->children;
    int valid = 0;

    while (jaamNode) {
        if (jaamNode->type == XML_ELEMENT_NODE && xmlStrcmp(jaamNode->name, (const xmlChar *)"station") == 0) {
            xmlNodePtr nameNode = jaamNode->children;
            while (nameNode) {
                if (xmlStrcmp(nameNode->name, (const xmlChar *)"name") == 0) {
                    char *stationName = (char *)xmlNodeGetContent(nameNode);
                    if (strcmp(stationName, jaam) == 0) {
                        valid = 1;
                        xmlFree(stationName);
                        break;
                    }
                    xmlFree(stationName);
                }
                nameNode = nameNode->next;
            }
        }
        jaamNode = jaamNode->next;
    }

    xmlFreeDoc(doc);
    return valid;
}
