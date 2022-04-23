//
// Created by 90302 on 2022/4/3.
//

#ifndef WEEK1_LOSERTREE_H
#define WEEK1_LOSERTREE_H


#include <bits/stdc++.h>
#include "_player.hpp"

using namespace std;

template<typename T>
class LoserTree
{
public:
    int *tree;//用来存player的下标的  输者树本身
    int numOfPlayers;//选手人数
    int lowExt;//最下面有多少个外界点
    int offset;//便于寻址
    T *player;//选手
    int *temp;//生成输者树的过程中 储存比赛赢者
public:
    LoserTree(T *players, int theNum);

    void initialize(T *players, int theNum);

    void play(int p, int leftChild, int rightChild);

    void replay(int theplayer);

    void output() const;

    void outPut(int nn) const;

private:
    int winner(const int &l, const int &r);

    int loser(const int &l, const int &r);

    int childIndex(const int &c);

    void outPut(int nn);
};

#endif //WEEK1_LOSERTREE_H
