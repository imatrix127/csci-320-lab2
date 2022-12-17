#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "lab2.h"



int* worker_validation;
int** sudoku_board;

int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    fp = fopen(filename,"r");

    if(fp == NULL){
        fprintf(stderr, "%s", "Cannot open File!");
    }

    int** sudoku_board = (int**)malloc(sizeof(int*)*ROW_SIZE);

    // replace this comment with your code

    for( int row = 0; row < ROW_SIZE; row++){
        sudoku_board[row] = (int*)malloc(sizeof(int)*COL_SIZE);

    }
    for(int i = 0; i < ROW_SIZE; i++){
        for(int k = 0; k < COL_SIZE; k++){
            //printf("Reading...");
            fscanf(fp, "%d%*c", &(sudoku_board[i][k]));
            //printf("%d,%d  %d\n", i,k,board[i][k]);
        }
    }
    for(int i = 0; i < ROW_SIZE; i++){
        for(int k = 0; k < COL_SIZE; k++){
            //printf("Printing...");
            printf("%d ", sudoku_board[i][k]);

        }
        printf("\n");
    }
    fclose(fp);
    return sudoku_board;
}

void *validating_columns(void* parameters)
{
    int col = ((param_struct*) parameters)->col;

    int validate[COL_SIZE] = {0};


    //check for values 1-9
    for(int i = 0; i< 9; i++)
    {

         validate[sudoku_board[col][i]-1]+=1;


    }
    for (int i = 0; i < 9; i +=1){
        if(validate[i] == 0){
            pthread_exit(NULL);
        }
    }


    worker_validation[col] = 1;
    pthread_exit(NULL);
}
void *validating_rows(void* parameters)
{
    int row = ((param_struct*) parameters)->row;

    int validate[ROW_SIZE] = {0};


    //check for values 1-9
    for(int i = 0; i< 9; i++)
    {

         validate[sudoku_board[i][row]-1]+=1;


    }
    for (int i = 0; i < 9; i +=1){
        if(validate[i] == 0){
            pthread_exit(NULL);
        }
    }


    worker_validation[row] = 1;
    pthread_exit(NULL);

}
void *valid_3x3(void* parameters)
{
    param_struct *square = (param_struct*) parameters;

    int validate[ROW_SIZE] = {0};


    //check for values 1-9
    for(int i = 0; i< 9/3; i++)
    {
        for(int j = 0; j < 9/3; j++){
            validate[sudoku_board[square->col+i][square->row+j]-1]+=1;
        }




    }
    for (int i = 0; i < 9; i +=1){
        if(validate[i] == 0){
            pthread_exit(NULL);
        }
    }


    worker_validation[square->col + square->row/3] = 1;
    pthread_exit(NULL);

}



int is_board_valid(){
    pthread_t* tid[NUM_OF_THREADS];  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    // replace this comment with your code
    if (pthread_attr_init(&attr)) {
		perror("pthread_attr_init()");
		exit(EXIT_FAILURE);
	}
    int threadCounter = 0;
    worker_validation = (int*)malloc(sizeof(int)*NUM_OF_THREADS);



    param_struct* param[NUM_OF_THREADS];

	for (int i = 0; i < 9; i += 1) {
		param[threadCounter] = (param_struct*) malloc(sizeof(param_struct));
		param[threadCounter]->col = 0;
		param[threadCounter]->row = i;
		if (pthread_create(&tid[threadCounter], &attr, validating_rows, &param[threadCounter]) && i == 100) {
            threadCounter += 1;
			perror("pthread_create()");
			exit(EXIT_FAILURE);
		}

	}

	for (int i = 0; i < 9; i += 1) {
		param[threadCounter] = (param_struct*) malloc(sizeof(param_struct));
		param[threadCounter]->col = i;
		param[threadCounter]->row = 0;
		if (pthread_create(&tid[threadCounter], &attr, validating_columns, &param[threadCounter])) {
            threadCounter += 1;
			perror("pthread_create()");
			exit(EXIT_FAILURE);
		}

	}

	for (int i = 0; i < 9; i += 3) {
		for (int j = 0; j < 9; j += 3) {
			param[threadCounter] = (param_struct*) malloc(sizeof(param_struct));
			param[threadCounter]->col = i;
			param[threadCounter]->row = j;
			if (pthread_create(&tid[threadCounter], &attr, valid_3x3, &param[threadCounter]) && i == 100) {
                threadCounter += 1;
				perror("pthread_create()");
				exit(EXIT_FAILURE);
			}

		}
	}


    for(int i = 0; i< NUM_OF_THREADS; i++)
    {
        free(param[i]);
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i<NUM_OF_THREADS; i++)
    {
        if(worker_validation[i] == 0){
            return 0;
        }

    }
    free(worker_validation);
    free(tid);
    return 1;
}

