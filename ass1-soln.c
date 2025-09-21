/* Program to implement Australian House of Representatives preferential
voting and determine election outcomes from vote preference orderings.
Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
August 2025, with the intention that it be modified by students
to add functionality, as required by the assignment specification.
All included code is (c) Copyright University of Melbourne, 2025
Student Authorship Declaration:
(1) I certify that except for the code provided in the initial skeleton
file, the program contained in this submission is completely my own
individual work, except where explicitly noted by further comments that
provide details otherwise. I understand that work that has been
developed by another student, or by me in collaboration with other
students, or by non-students as a result of request, solicitation, or
payment, may not be submitted for assessment in this subject. I
understand that submitting for assessment work developed by or in
collaboration with other students or non-students constitutes Academic
Misconduct, and may be penalized by mark deductions, or by other
penalties determined via the University of Melbourne Academic Honesty
Policy, as described at https://academicintegrity.unimelb.edu.au.
(2) I also certify that I have not provided a copy of this work in either
softcopy or hardcopy or any other form to any other student, and nor will
I do so until after the marks are released. I understand that providing
my work to other students, regardless of my intention or any undertakings
made to me by that other student, is also Academic Misconduct.
(3) I further understand that providing a copy of the assignment
specification to any form of code authoring or assignment tutoring
service, or drawing the attention of others to such services and code
that may have been made available via such a service, may be regarded as
Student General Misconduct (interfering with the teaching activities of
the University and/or inciting others to commit Academic Misconduct). I
understand that an allegation of Student General Misconduct may arise
regardless of whether or not I personally make use of such solutions or
sought benefit from such actions.
Signed by: Zhuyirui Xu 1750652
Dated: 15/09/2025
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define MAX_M 20
#define MAX_NAME_LEN 21
#define MAX_VOTES 999
///////////////////////////////////////////////////////////////////////
int getword(char W[], int limit);
// add your other function prototypes here
///////////////////////////////////////////////////////////////////////
// main program provides traffic control
int
main(int argc, char *argv[]) {
// write your main program here
/*First, read the number of candidates in the first line of input*/
    int m;
    if (scanf("%d",&m) != 1) {
    /*Invalid input,exit program*/
        return 0;
    }

    /*Declare a char array to store the names of the candidates*/
    char c_names[MAX_M][MAX_NAME_LEN];

    /*Read m candidate names from the second line of input*/
    for (int i = 0; i < m; i++) {
        getword(c_names[i], MAX_NAME_LEN - 1);
    }

    /*Initialise the vote count*/
    int votes[MAX_VOTES][MAX_M];
    int n = 0;
    int last_vote[MAX_M];

    /*Read the first rank values of a vote*/
    while (scanf("%d", &vote[n][0]) == 1) {
        /*Read rest of the ranks ofr this voter*/
        for (int j = 1; j < m; j++) {
            scanf("%d", &vote[n][j]);
        }
        /*Increase the vote count after reading one completed vote*/
        n++;
    }

    /*Print Stage 1 Output in required format*/
    printf("\n");
    printf("Stage 1\n");
    printf("=======\n");
    printf("read %d candidates and %d votes\n", m, n);
    printf("voter %d preferences...\n", n);

    /*Map the last voter's preference rankings to candidates'names*/
    for (int rank = 1; rank <= m; rank++){
        for (int k = 0; k < m; k++){
            if (vote[n-1][k] == rank){
                printf("    rank  %d: %s\n", rank, c_names[k]);
                break;
            }
        }
    }
/*Stage 2*/
stage2(c_names,votes,m,n)
/*Stage 3*/
stage3(c_names,votes,m,n)
// all done, time to go home
    printf("tadaa!\n");
    return 0;
}
///////////////////////////////////////////////////////////////////////
// extract a single word out of the standard input, of not
// more than limit characters, argument array W must be
// limit+1 characters or bigger.
// function adapted from Figure 7.13 of Programming, Problem Solving,
// and Abstraction with C, by Alistair Moffat
//
int
getword(char W[], int limit) {
int c, len=0;
/* first, skip over any non alphabetics */
while ((c=getchar())!=EOF && !isalpha(c)) {
/* do nothing more */
}
if (c==EOF) {
return EOF;
}
/* ok, first character of next word has been found */
W[len++] = c;
while (len<limit && (c=getchar())!=EOF && isalpha(c)) {
/* another character to be stored */
W[len++] = c;
}
/* now close off the string */
W[len] = '\0';
return 0;
}
///////////////////////////////////////////////////////////////////////
// add your other functions here
/*count total votes each candidate currently has*/
int count_votes(int cand, int votes[][MAX_M], int m, int n, int eliminated[]){
    int total = 0;
    /*check each voter*/
    for (int i = 0; i < n; i++){
        int best = -1;
        int bestrank = m + 1;
        for (int j = 0; j < m; j++){
            if (!eliminated[j]&&votes[i][j] < bestrank){
                besteank = votes[i][j];
                best = j;
            }
        }
        if (best == cand){
            total++;
        }
    }
    return total; 
}
//algorithms are fun!
