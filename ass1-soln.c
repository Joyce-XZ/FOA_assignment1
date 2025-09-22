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
Dated: 19/09/2025
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
void run_stage1(int votes[][MAX_M], char c_names[][MAX_NAME_LEN], int m, int n,
    int lastVote[]);
void run_stage2(int votes[][MAX_M], char c_names[][MAX_NAME_LEN], int m, int n);
void run_stage3(int votes[][MAX_M], char c_names[][MAX_NAME_LEN], int m, int n);

void count_votes(int current_votes[], int eliminated[], int vote_counts[],
    int m, int n);
void redistribute_votes(int votes[][MAX_M], int current_votes[], 
    int eliminated[], int to_elim, int m, int n);
int find_eliminate_candidate(int vote_counts[], int eliminated[], int m);
int check_winner(int vote_counts[], int eliminated[], int m, int n);

void print_round_results(char c_names[][MAX_NAME_LEN], int vote_counts[], 
    int eliminated[], int m, int n, int round);
void print_sorted_results(char c_names[][MAX_NAME_LEN], int vote_counts[], 
    int eliminated[], int m, int n, int round);

void insertion_sort(int sorted[], int vote_counts[], 
    char c_names[][MAX_NAME_LEN], int m);
int compare(int a, int b, int vote_counts[], char c_names[][MAX_NAME_LEN]);
void swap(int arr[], int i, int j);
///////////////////////////////////////////////////////////////////////
// main program provides traffic control
int
main(int argc, char *argv[]) {
    int m;
    if (scanf("%d", &m) != 1) {
        return 0;
    }

    char c_names[MAX_M][MAX_NAME_LEN] = {0};

    /* read candidate names */
    for (int i = 0; i < m; i++) {
        getword(c_names[i], MAX_NAME_LEN - 1);
    }

    int n = 0;
    int votes[MAX_VOTES][MAX_M] = {0};
    int lastVote[MAX_M] = {0};
    
    /* read in all ballots */
    for (n = 0; n < MAX_VOTES && scanf("%d", &votes[n][0]) == 1; n++) {
        for (int j = 1; j < m; j++) {
            scanf("%d", &votes[n][j]);
        }
    }
    
    /* store last vote for Stage 1 */
    for (int i = 0; i < m; i++) {
        lastVote[i] = votes[n-1][i];
    }

    /* Stage 1 output */
    printf("\n");
    run_stage1(votes, c_names, m, n, lastVote);

    /* run subsequent stages */
    run_stage2(votes, c_names, m, n);
    run_stage3(votes, c_names, m, n);

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
/*******************************************************************/
/* Stage 1: Display input data and voter preferences */
void
run_stage1(int votes[][MAX_M], char c_names[][MAX_NAME_LEN], int m, int n,
    int lastVote[]) {
    printf("Stage 1\n");
    printf("=======\n");
    printf("read %d candidates and %d votes\n", m, n);
    printf("voter %d preferences...\n", n);
    for (int rank = 1; rank <= m; rank++) {
        for (int k = 0; k < m; k++) {
            if (lastVote[k] == rank) {
                printf("    rank  %d: %s\n", rank, c_names[k]);
                break;
            }
        }
    }
}

/*******************************************************************/
/* Stage 2: Preferential voting with candidate elimination
   - repeatedly count votes
   - check for majority winner
   - otherwise eliminate lowest candidate and redistribute */
void
run_stage2(int votes[][MAX_M], char c_names[][MAX_NAME_LEN],int m, int n) {
    printf("\n");
    printf("Stage 2\n");
    printf("=======\n");

    int eliminated[MAX_M] = {0};
    int vote_counts[MAX_M] = {0};
    int current_votes[MAX_VOTES] = {0};

    /* initialize voters’ current choice to first preference */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (votes[i][j] == 1) {
                current_votes[i] = j;
                break;
            }
        }
    }

    int round = 1;
    int winner = -1;

    while (winner == -1) {
        count_votes(current_votes, eliminated, vote_counts, m, n);
        print_round_results(c_names, vote_counts, eliminated, m, n, round);

        winner = check_winner(vote_counts, eliminated, m, n);
        if (winner != -1) {
            printf("    ----\n");
            printf("    %s is declared elected\n", c_names[winner]);
            printf("\n");
            break;
        }

        int to_elim = find_eliminate_candidate(vote_counts, eliminated, m);
        printf("    ----\n");
        printf("    %s is eliminated and votes distributed\n",
               c_names[to_elim]);
        printf("\n");
        eliminated[to_elim] = 1;

        redistribute_votes(votes, current_votes, eliminated,
                           to_elim, m, n);
        round++;
    }
}

/*******************************************************************/
/* Stage 3: As Stage 2, but results reported in sorted order
   - sorting by vote count (desc), then by candidate name */
void
run_stage3(int votes[][MAX_M], char c_names[][MAX_NAME_LEN], int m, int n) {
    printf("\n");
    printf("Stage 3\n");
    printf("=======\n");

    int eliminated[MAX_M] = {0};
    int vote_counts[MAX_M] = {0};
    int current_votes[MAX_VOTES] = {0};

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (votes[i][j] == 1) {
                current_votes[i] = j;
                break;
            }
        }
    }

    int round = 1;
    int winner = -1;

    while (winner == -1) {
        count_votes(current_votes, eliminated, vote_counts, m, n);
        print_sorted_results(c_names, vote_counts, eliminated, m, n, round);

        winner = check_winner(vote_counts, eliminated, m, n);
        if (winner != -1) {
            printf("    ----\n");
            printf("    %s is declared elected\n", c_names[winner]);
            printf("\n");
            break;
        }

        int to_elim = find_eliminate_candidate(vote_counts, eliminated, m);
        printf("    ----\n");
        printf("    %s is eliminated and votes distributed\n",
               c_names[to_elim]);
        printf("\n");
        eliminated[to_elim] = 1;

        redistribute_votes(votes, current_votes, eliminated,
                           to_elim, m, n);
        round++;
    }
}

/*******************************************************************/
/* count votes for all non-eliminated candidates,
   based on each voter's current active preference */
void
count_votes(int current_votes[], int eliminated[], int vote_counts[],
    int m, int n) {
    for (int i = 0; i < m; i++) {
        vote_counts[i] = 0;
    }
    for (int i = 0; i < n; i++) {
        if (!eliminated[current_votes[i]]) {
            vote_counts[current_votes[i]]++;
        }
    }
}

/*******************************************************************/
/* when a candidate is eliminated, redistribute their votes
   to the next valid preference for each affected voter */
void
redistribute_votes(int votes[][MAX_M], int current_votes[],int eliminated[],
    int to_elim, int m, int n) {
    for (int i = 0; i < n; i++) {
        if (current_votes[i] == to_elim) {
            int found = 0;
            for (int rank = 2; rank <= m && !found; rank++) {
                for (int j = 0; j < m && !found; j++) {
                    if (votes[i][j] == rank && !eliminated[j]) {
                        current_votes[i] = j;
                        found = 1;
                    }
                }
            }
        }
    }
}

/*******************************************************************/
/* find the candidate with the smallest vote total,
   breaking ties by input order */
int
find_eliminate_candidate(int vote_counts[], int eliminated[], int m) {
    int min_votes = MAX_VOTES + 1;
    int to_elim = -1;
    for (int i = 0; i < m; i++) {
        if (!eliminated[i] && vote_counts[i] < min_votes) {
            min_votes = vote_counts[i];
            to_elim = i;
        }
    }
    return to_elim;
}

/*******************************************************************/
/* check if any candidate has >50% of the active votes */
int
check_winner(int vote_counts[], int eliminated[], int m, int n) {
    for (int i = 0; i < m; i++) {
        if (!eliminated[i] && vote_counts[i] > n/2) {
            return i;
        }
    }
    return -1;
}

/*******************************************************************/
/* print round results in original candidate order */
void
print_round_results(char c_names[][MAX_NAME_LEN], int vote_counts[], 
    int eliminated[], int m, int n, int round) {
    printf("round %d...\n", round);
    for (int i = 0; i < m; i++) {
        if (!eliminated[i]) {
            double percent = (double)vote_counts[i] / n * 100.0;
            printf("    %-20s:   %d votes, %.1f%%\n", 
                c_names[i], vote_counts[i], percent);
        }
    }
}

/**************************************************************/
/* print round results in sorted order, using insertion_sort */
void
print_sorted_results(char c_names[][MAX_NAME_LEN], int vote_counts[], int eliminated[],
    int m, int n, int round) {
    int sorted[MAX_M];
    for (int i = 0; i < m; i++) {
        sorted[i] = i;
    }

    insertion_sort(sorted, vote_counts, c_names, m);

    printf("round %d...\n", round);
    for (int i = 0; i < m; i++) {
        int idx = sorted[i];
        if (!eliminated[idx]) {
            double percent = (double)vote_counts[idx] / n * 100.0;
            printf("    %-20s:   %d votes, %.1f%%\n", 
                c_names[idx], vote_counts[idx], percent);
        }
    }
}

/************************************************************/
/* insertion sort of candidate indices,
   comparing via vote totals and then candidate names */
void
insertion_sort(int sorted[], int vote_counts[], char c_names[][MAX_NAME_LEN],
    int m) {
    for (int i = 1; i < m; i++) {
        int j = i;
        while (j > 0 && compare(sorted[j-1], sorted[j], vote_counts, 
            c_names) > 0) {
            swap(sorted, j-1, j);
            j--;
        }
    }
}

/*******************************************************************/
/* comparison function for insertion_sort:
   - sort by vote count (descending)
   - break ties by candidate name (ascending) */
int
compare(int a, int b, int vote_counts[], char c_names[][MAX_NAME_LEN]) {
    if (vote_counts[a] > vote_counts[b]) {
        return -1;
    } else if (vote_counts[a] < vote_counts[b]) {
        return 1;
    } else {
        return strncmp(c_names[a], c_names[b], MAX_NAME_LEN);
    }
}

/*******************************************************************/
/* swap two integer indices in an array */
void
swap(int arr[], int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}
//Algorithms are fun!