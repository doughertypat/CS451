/************************************************
 * CSci 451 - HW3
 * Patrick Dougherty
 * patrick.r.dougherty@und.edu
 * 21Sep20
 * *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

struct keywordSearchData
{
    char *buf;
    char *keyword;
};

int fileSize(FILE *fp)
{

    fseek(fp, 0L, SEEK_END);
    int length = ftell(fp);
    rewind(fp);
    return length;
}

char* fileToBuffer(char *f_name)
{
    int buf_pos = 0;
    char c;
    FILE *fp = fopen(f_name, "r");
    if(fp == NULL)
    {
        printf("Make sure the file exits, I couldn't find it.\n");
        return "";
    }
    int f_len = fileSize(fp);
    char *buffer = malloc(sizeof(char) * (f_len + 1));
    while(EOF != (c = fgetc(fp)))
    {
        buffer[buf_pos] = tolower(c);
        ++buf_pos;
    }
    buffer[buf_pos] = '\0';
    fclose(fp);
    return buffer;
}

void *countKeywords(void *arg)
{
    struct keywordSearchData *data;
    data = (struct keywordSearchData *) arg;
    
    int found, count, bufLen, keywordLen;

    bufLen = strlen(data->buf);
    keywordLen = strlen(data->keyword);
    count=0;
    for(int i=0; i<=bufLen-keywordLen; i++)
    {
        found = 1;
        for(int j=0; j<keywordLen; j++)
        {
            if(data->buf[i + j] != data->keyword[j])
            {
                found = 0;
                break;
            }
        }

        if(found == 1)
        {
            count++;
        }
    }
    printf("There were %d instances of %s\n", count, data->keyword);
    pthread_exit(NULL);
}

int main()
{
    system("wget -q http://undcemcs01.und.edu/~ronald.marsh/CLASS/CS451/hw3-data.txt");
    
    char f_name[] = { "hw3-data.txt" };
    char *buffer = fileToBuffer(f_name);
    if((strcmp(buffer, "") == 0) || (strcmp(buffer, "\0") == 0))
    {
        printf("File doesn't exist or was empty.\n");
        return 0;
    }
    
    pthread_t thread[2];
    struct keywordSearchData data[2];
    data[0].buf = buffer;
    data[0].keyword = "polar";
    data[1].buf = buffer;
    data[1].keyword = "easy";

    for(int i=0; i<2; i++)
    {
        pthread_create(&thread[i], NULL, countKeywords, (void *) &data[i]);
    } 
    
    for(int i=0; i<2; i++)
    {   
        pthread_join(thread[i], NULL);
    }

    free(buffer);
    
    return 0;
}
