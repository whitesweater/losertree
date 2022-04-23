//
// Created by 90302 on 2022/4/3.
//

#ifndef WEEK1_EXTERNALSORT_H
#define WEEK1_EXTERNALSORT_H

#include <bits/stdc++.h>

#include <utility>
#include "loserTree.h"
#include "_player.hpp"

using namespace std;


template<typename T>
class ExternalSort
{
public:
    /****************归并相关设置值*******************/
    _player *player;//用于输者树比赛的选手
    int bufferSize;//总缓存区大小
    int buffer;//单个缓存区大小
    int k;//归并路数
    int IOcount;//IO次数
    /****************归并相关设置值*******************/

    /****************文件读取相关*******************/
    string inFolder;//指定输入文件夹路径
    string outFolder;//制定输出文件夹
    string fileUrl;//输入文件路径
    /****************文件读取相关*******************/

    /****************过程中产生的一些数据*******************/
    int numSeg;//初始归并段的个数

    vector<vector<int>> resDisk;//虚拟磁盘--储存结果
    vector<queue<int>> pm;//虚拟磁盘--储存归并段
    vector<queue<int>> pooling;//缓存区
    /****************过程中产生的一些数据*******************/

private:
    void genreta(int m, int s, const vector<vector<int>> &startS);

public:
    ExternalSort(int theK, int theSize, const string &theInFoleUrl, const string &theOutFoleUrl);//只是文件夹路径

    void initialize();

    void merge();

    void loadData(int rounds);

    bool DiskToBuffer2(int cycle, int no, int last);

    bool DiskToBuffer(int cycle, int no);

    void BufferToDisk(int j);

};


#endif //WEEK1_EXTERNALSORT_H
