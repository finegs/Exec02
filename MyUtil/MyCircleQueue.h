#pragma once

#include <stdio.h>
#include <stdlib.h>

#define SIZE 8

typedef struct CirQue_ {
    int *data;
    int front;
    int rear;
    int count;
    int capacity;

    int(*init)(struct CirQue_ *que, int capacity);
    int(*dispose)(struct CirQue_ *que);
    int(*insert)(struct CirQue_ *que, const int newData);
    int(*remove)(struct CirQue_ *que, int* data);
    int(*print)(struct CirQue_ *que);
} CirQue;

#define cirque_rear(que) ((que)->rear)
#define cirque_front(que) ((que)->front)
#define cirque_count(que) ((que)->count)
#define cirque_capacity(que) ((que)->capacity)
#define cirque_data(que, idx) (*((que)->data+idx))

int initCirQue(CirQue *que, int capacity);
int disposeCirQue(CirQue *que);
int insertCirQue(CirQue *que, int newData);
int removeCirQue(CirQue *que, int *data);
int printCirQue(CirQue *que);