#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "lab2.h"


int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    int** sudoku_board = (int**)malloc(sizeof(int*)*ROW_SIZE);

    // replace this comment with your code
    fp = fopen(filename,"r");
    for( int row = 0; row < ROW_SIZE; row++){
        sudoku_board[row] = (int*)malloc(sizeof(int)*COL_SIZE);
    }
    for(int i = 0; i < ROW_SIZE; i++){
        for(int k = 0; k < COL_SIZE; k++){

            fscanf(fp, "%d%*c", &sudoku_board[i][k]);
        }
    }
    for(int i = 0; i < ROW_SIZE; i++){
        for(int k = 0; k < COL_SIZE; k++){
            printf("%d ", sudoku_board[i][k]);
        }
        printf("/n");
    }
    fclose(fp);
    return sudoku_board;
}

void *validating_columns(void* parameters)
{
    param_struct *p = (param_struct*) parameters;

    int validate[9] = {0};
    int row = p -> starting_row;
    int column = p -> starting_col;

    for(int i = 0; i< 9; i++)
    {
        int target = sudoku_board[i][column];
        if (target > 9 || target < 1 || validate[target - 1]==1)
        {
            fprintf(stderr, "INVALID Column at row=%d, column=%d \n", row + 1, i + 1);
            pthread_exit(NULL);
        }

        else{
            validate[target - 1] = 1;
        }
    }

    worker_validation[ 18 + column] = 1;
    pthread_exit(NULL);
}
void *validating_rows(void* parameters)
{
    param_struct *p = (param_struct*) parameters;

    int validate[9] = {0};
    int row = p -> starting_row;
    int column = p -> starting_col;

    for(int i = 0; i< 9; i++)
    {
        int target = sudoku_board[row][i];
        if (target > 9 || target < 1 || validate[target - 1]==1)
        {
            fprintf(stderr, "INVALID Column at row=%d, column=%d \n", row + 1, i + 1);
            pthread_exit(NULL);
        }
        else{
            validate[target - 1] = 1;
        }
    }

    worker_validation[9 + row] = 1;
    pthread_exit(NULL);

}
void *valid_3x3(void* parameters)
{
    param_struct *p = (param_struct*) parameters;
    int validate[9] = {0};
    int row = p -> starting_row;
    int column = p -> starting_col;

    for(int i = row; i < row + 3; i++){
        for(int k = column; k<column + 3; k++)
        {
            int target = sudoku_board[i][k];
            if (target > 9 || target < 1 || validate[target - 1]==1)
            {
                fprintf(stderr, "INVALID subsection at row=%d, column=%d \n", i + 1, k + 1);
                pthread_exit(NULL);
            }
            else{
                validate[target-1] = 1;
            }
        }
    }
    worker_validation[row + column / 3] = 1;
    pthread_exit(NULL);

}



int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    // replace this comment with your code

    int threadCounter = 0;
    worker_validation = (int*)malloc(sizeof(int)*NUM_OF_THREADS);
    tid = (pthread_t*)malloc(sizeof(int)*NUM_OF_THREADS);

    for(int i = 0; i < ROW_SIZE; i++)
    {
        for(int j = 0; j < COL_SIZE; j++)
        {
            if(i%3 == 0 && j%3 == 0){
                param_struct *worker3x3 = (param_struct*) malloc(sizeof(param_struct));
                worker3x3->starting_row = i;
                worker3x3->starting_col = j;
                pthread_create(&tid[threadCounter++], NULL, valid_3x3, worker3x3);
            }
            if(i==0){
                param_struct *workerColumn = (param_struct*) malloc(sizeof(param_struct));
                workerColumn->starting_row = i;
                workerColumn->starting_col = j;
                pthread_create(&tid[threadCounter++], NULL,validating_columns,workerColumn);
            }
            if(j==0){
                param_struct *workerRow = (param_struct*) malloc(sizeof(param_struct));
                workerRow->starting_row = i;
                workerRow->starting_col = j;
                pthread_create(&tid[threadCounter++], NULL, validating_rows, workerRow);
            }

        }
    }

    for(int i = 0; i< NUM_OF_THREADS; i++)
    {
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

