//
//  main.c
//  OSaSP_C
//
//  Created by Ilya Baryko on 3/27/20.
//  Copyright © 2020 Ilya Baryko. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define PI 3.141592653589793238
#define filePath "/Users/shine/Desktop/outputTaylor.txt"
#define fileResultPath "/Users/shine/Desktop/resultTaylor.txt"

struct taylorInfo {
    double value;
    long childpid;
    long parentpid;
};

void createTwoChildProcesses(void);
void calcualteTaylorSeries(void);
void getResult(double x, int numOfSeries);
double factorial(int number);
double getValueOfTaylorSeries(int n);
bool validateVariables(int numOfSeries, int n);

int main() {
    createTwoChildProcesses();
//    calcualteTaylorSeries();
    return 0;
}

// MARK: - y[i] = sin(2*PI*i/N) - Individual task (Second part)
void calcualteTaylorSeries() {
    int numOfSeries, n = 0;
    do {
    printf("Enter n: ");
    scanf("%d", &numOfSeries);
    printf("Enter N: ");
    scanf("%d", &n);
    } while (!validateVariables(numOfSeries, n));
    
    double x, y[n];
    FILE *file = NULL;
    file = fopen(fileResultPath, "w");
    fclose(file);
    
    for(int i = 0; i < n; i++) {
        file = fopen(filePath, "w");
        fclose(file);
        x = 2 * PI * i / n;
        getResult(x, numOfSeries);
        y[i] = getValueOfTaylorSeries(numOfSeries);
        file = fopen(fileResultPath, "a+");
        fprintf(file, "y[%d] = %f, where x = %f\n", i, y[i], x);
        fclose(file);
    }
}

double getValueOfTaylorSeries(int n) {
    FILE *file = NULL;
    double value = 0.0;
    struct taylorInfo item;
    
    file = fopen(filePath, "r+");
    for (int i = 0; i < n; i++) {
        fscanf(file, "%lf %ld %ld", &item.value, &item.childpid, &item.parentpid);
        value += item.value;
    }
    fclose(file);
    printf("\nValue of Taylor Series = %f, currentpid = %d\n", value, getpid());
    return value;
}

void getResult(double x, int numOfSeries) {
    FILE *file = NULL;
    printf("\nx = %f", x);
    int denominator = 1;
    for (int i = 0; i < numOfSeries; i++) {
        pid_t pid = fork();
        if(pid == 0) {
            /* Child process */
            file = fopen(filePath, "a+");
            double tmp = (i % 2 == 1) ? (-1 * pow(x, denominator) / factorial(denominator)) : pow(x, denominator) / factorial(denominator);
            printf("\n%d. Value of seria = %f, childpid = %ld, parentpid = %ld", i, tmp, (long)getpid(), (long)getppid());
            fprintf(file, "%f %ld %ld ", tmp, (long)getpid(), (long)getppid());
            fclose(file);
            exit(0);
        } else {
            /* Parent process */
            waitpid(pid, NULL, 0);
            denominator += 2;
        }
        wait(NULL);
    }
}

bool validateVariables(int numOfSeries, int n) {
    return (numOfSeries > 0) && (n > 0) ? true : false;
}

double factorial(int number) {
    double result = 1;
    for (int i = 2; i < number + 1; i++) {
        result *= i;
    }
    return result;
}

//  MARK: - Fisrt part
void createTwoChildProcesses() {
    long int countTicks;
    for (int kid = 0; kid < 2; kid++) {
        int pid = fork();
        if(pid < 0) {
            printf("Error!");
            exit(1);
        }
        else if (pid > 0) {
            /* Parent process */
            countTicks = time (NULL);
            printf("Current time: %s", ctime (&countTicks));
            printf("Parent pid=%d\n\n", getpid());
        }
        else {
            /* Child process */
            waitpid(pid, NULL, 0);
            countTicks = time (NULL);
            printf("Current time: %s", ctime (&countTicks) );
            printf("Child pid = %d, Parent pid = %d\n\n", getpid(), getppid());
            exit(0);
        }
    }
    
    system("ps -x");
}
