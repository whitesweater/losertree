//
// Created by 90302 on 2022/4/3.
//

#include "loserTree.h"

template<typename T>
LoserTree<T>::LoserTree(T *players, int theNum)
{
    tree = NULL;
    initialize(players, theNum);//相较于直接写在构造函数里面  这样更方便 要用另一组的时候 不用重新定义一个变量。
}

template<typename T>
void LoserTree<T>::initialize(T *players, int theNum)
{
    if (theNum < 2)//至少两个人才能比赛
        exit(-1);

    int i;
    numOfPlayers = theNum;

    player = players;
    tree = new int[numOfPlayers];//N个选手 需要N-1个节点 但我们不用第0个  刚好是1 ~ theNum-1
    temp = new int[numOfPlayers];

    int s;
    for (s = 1; 2 * s <= numOfPlayers - 1; s += s);

    //最后一层 选手的人数
    lowExt = 2 * (numOfPlayers - s);
    //offsets
    offset = 2 * s - 1;

    for (int i = 2; i <= lowExt; i += 2)//可以看到 0号节点是没有用捏
        play((offset + i) / 2, i - 1, i);//这里必然是双数的


    if (numOfPlayers % 2 == 1)
    {
        play(numOfPlayers / 2, temp[numOfPlayers - 1], lowExt + 1);
        i = lowExt + 3;
    }
    else i = lowExt + 2;

    for (; i <= numOfPlayers; i += 2)
        play((i - lowExt + numOfPlayers - 1) / 2, i - 1, i);

    //遍历一次 转化为输者树
    tree[0] = temp[1];

    int q, q2;// q q2 是左右孩子的索引
    for (int j = 1; j < numOfPlayers; ++j)
    {
        q = childIndex(2 * j);
        q2 = childIndex(2 * j + 1);
        tree[j] = loser(q, q2);
    }
}


template<typename T>
void LoserTree<T>::play(int p, int leftChild, int rightChild)
{
    temp[p] = (player[leftChild] <= player[rightChild]) ? leftChild : rightChild;//那个小 那个就赢

    while (p % 2 == 1 && p > 1)//p是右孩子 就可以继续往上初始化
    {
        temp[p / 2] = (player[temp[p - 1]] <= player[temp[p]]) ? temp[p - 1] : temp[p];
        p /= 2;
    }
}

template<typename T>
void LoserTree<T>::replay(int thePlayer)
{
    if (thePlayer <= 0 || thePlayer > numOfPlayers) exit(-1);

    int matchNode, leftChild, rightChild;

    if (thePlayer <= lowExt)
    {//如果要比赛的结点在最下层
        matchNode = (offset + thePlayer) / 2;
        leftChild = 2 * matchNode - offset;
        rightChild = leftChild + 1;
    }
    else
    {//要比赛的结点在次下层
        matchNode = (thePlayer - lowExt + numOfPlayers - 1) / 2;
        if (2 * matchNode == numOfPlayers - 1)
        {//特殊情况，其中一方是晋级后的人
            leftChild = temp[2 * matchNode];
            rightChild = thePlayer;
        }
        else
        {
            leftChild = 2 * matchNode - numOfPlayers + 1 + lowExt;//这个操作是因为上面matchNode计算中/2取整了
            rightChild = leftChild + 1;
        }
    }
//只有第一次涉及到 外部节点了
//后面都是 tree里面的事情

    if (thePlayer == tree[0])
    {
        for (; matchNode >= 1; matchNode /= 2)
        {
            temp[matchNode] = winner(tree[matchNode], thePlayer);//缓存 下来tree【matchnode】
            tree[matchNode] = loser(tree[matchNode], thePlayer);
            thePlayer = temp[matchNode];
        }
    }
    else
    {
        tree[matchNode] = loser(leftChild, rightChild);
        temp[matchNode] = winner(leftChild, rightChild);
        if (matchNode == numOfPlayers - 1 && numOfPlayers % 2 == 1)
        {//特殊情况，比赛结点是最后一层的最后一场比赛
            //特殊在matchNode/2后，左儿子是晋级的选手，右儿子是一场都没有比过赛的选手
            matchNode /= 2;
            tree[matchNode] = loser(temp[numOfPlayers - 1], lowExt + 1);
            temp[matchNode] = winner(temp[numOfPlayers - 1], lowExt + 1);
        }

        matchNode /= 2;
        for (; matchNode >= 1; matchNode /= 2)
        {
            tree[matchNode] = loser(temp[matchNode * 2], temp[matchNode * 2 + 1]);
            temp[matchNode] = winner(temp[matchNode * 2], temp[matchNode * 2 + 1]);
        }
    }
    tree[0] = temp[1];
}


template<typename T>
int LoserTree<T>::winner(const int &l, const int &r)
{ return player[l] <= player[r] ? l : r; }

template<typename T>
int LoserTree<T>::loser(const int &l, const int &r)
{ return player[l] <= player[r] ? r : l; }

template<typename T>
int LoserTree<T>::childIndex(const int &c)
{
    if (c <= numOfPlayers - 1)
        return temp[c];
    else
    {// child is an external node  是一个外节点
        if (c <= offset)
            return c + lowExt - numOfPlayers + 1;
        else
            return c - offset;//最底层
    }
}

template<typename T>
void LoserTree<T>::output() const
{
    cout << "number of players  = " << numOfPlayers
         << " lowExt = " << lowExt
         << " offset = " << offset << endl;
    cout << "complete loser tree pointers are" << endl;
    for (int i = 1; i < numOfPlayers; i++)
        cout << tree[i] << ' ';
    cout << endl;
    cout << "最小的" << tree[0] << "= " << player[tree[0]] << endl;
}


template<typename T>
void LoserTree<T>::outPut(int nn)
{
    for (int i = 1; i <= nn - 1; ++i)
    {
        cout << this->tree[i] << ' ';
        double a = log(i + 1);
        double b = log(2);
        if ((a / b) - floor(a / b) <= 0.00000000000001)
            cout << endl;
    }
    cout << endl;
    cout << "------------上 loser-------------------" << endl;
    for (int i = 1; i <= nn - 1; ++i)
    {
        cout << this->temp[i] << ' ';
        double a = log(i + 1);
        double b = log(2);
        if ((a / b) - floor(a / b) <= 0.00000000000001)
            cout << endl;
    }
    cout << endl;
    cout << "------------上 winner----------------" << endl;
}