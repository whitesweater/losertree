#include "ExternalSort.h"

template<typename T>
ExternalSort<T>::ExternalSort(int theK , int theSize , const string &theInFoleUrl , const string &theOutFoleUrl )//只是文件夹路径
        : IOcount(0), k(theK), bufferSize(theSize), inFolder(theInFoleUrl), outFolder(theOutFoleUrl), numSeg(0),
          buffer(theSize / (theK + 1))
{
    cout << "当前操作目录为" << theInFoleUrl << endl;
    player = NULL;
    vector<string> allFiles;
    getFileNames(inFolder, allFiles);
    cout << "`````````````````````当前目录下文件有`````````````````" << endl;
    for (auto &i: allFiles)
    { cout << i << endl; }
    cout << "````````````````````````````````````````````````````" << endl;
    cout << "以 " << allFiles.front() << "为源文件 读入数据" << endl;
    fileUrl = allFiles.front();
    initialize();
}


template<typename T>
void ExternalSort<T>::merge()
{
    int times = 1;
    for (times = 1; numSeg > pow(k, times); times++);
    pooling.resize(k + 2);// 1-k 是输入  K+1 是输出缓存
    delete[] player;
    player = new _player[k + 1];
    for (int i = 0; i < times; ++i)
    {
        int cycle = numSeg / k;
        int last = numSeg % k;
        resDisk.clear();
        resDisk.resize(cycle + 2);//虚拟磁盘

        loadData(i);//对于每一轮 都把该轮的数据放在pm中

        for (int j = 0; j < cycle; ++j)
        {//1-4 5-8 9-12
            //K个缓存区中 读取buffer大小个数据
            for (int l = 1; l <= k; ++l)
            { DiskToBuffer(j, l); }

            for (int l = 1; l <= k; ++l)
            {
                player[l]._low = l;
                player[l]._high = pooling[l].front();
                pooling[l].pop();

                if (pooling[l].empty())//输入缓冲区用光了
                    DiskToBuffer(j, l);
            }
            LoserTree<_player> mergeTree(player, k);
            while (player[mergeTree.tree[0]]._high != INT32_MAX)//当最小的哪一个也为 inf的时候就说明排完了
            {
                int winIndex = mergeTree.tree[0];
                pooling[k + 1].push(player[winIndex]._high);//储存结果

                player[winIndex]._high = pooling[player[winIndex]._low].front();//替换选手
                pooling[player[winIndex]._low].pop();

                mergeTree.replay(winIndex);

                if (pooling[player[winIndex]._low].empty())//输入缓冲区用光了
                    DiskToBuffer(j, player[winIndex]._low);
                if (pooling[k + 1].size() == buffer)//输出缓冲区满了
                { BufferToDisk(j); }//每K组 用一个输出
            }
            BufferToDisk(j);
        }
        //对剩下的last进行处理
        // K  last
        // 5   2   1 2 1 2
        for (int j = 1; j <= k; ++j)
        { DiskToBuffer2(cycle, j, last); }

        for (int l = 1; l <= k; ++l)
        {
            player[l]._low = l;
            player[l]._high = pooling[l].front();
            pooling[l].pop();

            if (pooling[l].empty())//输入缓冲区用光了
                DiskToBuffer2(cycle, l, last);
        }
        LoserTree<_player> mergeTree(player, k);

        while (player[mergeTree.tree[0]]._high != INT32_MAX)//当最小的哪一个也为 inf的时候就说明排完了
        {
            pooling[k + 1].push(player[mergeTree.tree[0]]._high);
            player[mergeTree.tree[0]]._high = pooling[player[mergeTree.tree[0]]._low].front();//替换选手
            pooling[player[mergeTree.tree[0]]._low].pop();
            mergeTree.replay(mergeTree.tree[0]);

            if (pooling[player[mergeTree.tree[0]]._low].empty())
                DiskToBuffer2(cycle, player[mergeTree.tree[0]]._low, last);
            if (pooling[k + 1].size() == buffer)
            { BufferToDisk(cycle); }
        }
        BufferToDisk(cycle);

        this->genreta(i + 1, resDisk.size() - 1, resDisk);
        numSeg = cycle;
        if (last != 0)
            numSeg++;
    }
}



template<typename T>
void ExternalSort<T>::initialize()
{
    cout << "------------开始生成初始归并段-----------" << endl;
    int n;//数据大小
    ifstream file(fileUrl.c_str());//文件？
    file >> n;
    player = new _player[n + 1];//初始化选手们
    for (int i = 1; i <= n; ++i)
    { file >> player[i]._low; }
//上面是读取文件操作

    LoserTree<_player> firstSegment(player, bufferSize);//用于生成初始归并段的树
    vector<vector<int>> startS(n);//临时存储初始归并段
    startS.clear();
    for (int i = bufferSize + 1; i <= n + bufferSize; ++i)
    {
        startS[player[firstSegment.tree[0]]._high].push_back(player[firstSegment.tree[0]]._low);
        //加入替换新节点
        if (i <= n)
        {
            if (player[firstSegment.tree[0]]._low > player[i]._low)
                player[firstSegment.tree[0]]._high++;

            if (player[firstSegment.tree[0]]._high > numSeg)
                numSeg = player[firstSegment.tree[0]]._high;

            player[firstSegment.tree[0]]._low = player[i]._low;
        }//这个位置就让出去了
        else
        {
            player[firstSegment.tree[0]]._low = INT32_MAX;
            player[firstSegment.tree[0]]._high = n - 1;
        }
        firstSegment.replay(firstSegment.tree[0]);
    }

    this->genreta(0, numSeg, startS);//生成文件
}



/*生成归并段文件*/
template<typename T>
void ExternalSort<T>::genreta(int m, int s, const vector<vector<int>> &startS)
{
    fstream Fe;//储存文件
    for (int i = 1; i <= s; i++)
    {
        string name;
        name = outFolder;
        if (_access(name.c_str(), 0) == -1)//检查文件夹是否存在
        {
            if (system(("mkdir " + name).c_str()))
                cout << "创建目标文件夹成功" << endl;
        }

        name += "\\Segs" + to_string(m) + "-" + to_string(i) + ".txt";
        Fe.open(name.c_str(), ios::out);

        for (auto ap = startS[i].begin(); ap != startS[i].end(); ap++)
        { Fe << to_string(*ap) << ' '; }
        cout << "已经生成归并段文件 " << name << endl;

        Fe.close();
    }
}


template<typename T>
void ExternalSort<T>::loadData(int rounds)
{
    pm.resize(numSeg + 1);
    for (int i = 0; i < numSeg; ++i)
    {
        string url = outFolder + "\\Segs" + to_string(rounds) + "-" + to_string(i + 1) + ".txt";
        ifstream read(url);
        while (!read.eof())
        {
            int ye;
            read >> ye;
            if (!read.eof())
                pm[i + 1].push(ye);
        }
    }
}

template<typename T>
bool ExternalSort<T>::DiskToBuffer2(int cycle, int no, int last)
{
    IOcount++;
    int index = no % last == 0 ? last : no % last;
    index += cycle * k;
    pooling[no] = queue<int>();
    while (!pm[index].empty() && pooling[no].size() < buffer)
    {
        pooling[no].push(pm[index].front());
        pm[index].pop();
    }
    if (pooling[no].size() == buffer)
        return true;
    else
    {
        while (pooling[no].size() < buffer)
            pooling[no].push(INT32_MAX);
        return false;
    }
}

template<typename T>
bool ExternalSort<T>::DiskToBuffer(int cycle, int no)
{
    IOcount++;
    pooling[no] = queue<int>();
    while (!pm[cycle * k + no].empty() && pooling[no].size() < buffer)
    {
        pooling[no].push(pm[cycle * k + no].front());
        pm[cycle * k + no].pop();
    }
    if (pooling[no].size() == buffer)
        return true;
    else
    {
        while (pooling[no].size() < buffer)
            pooling[no].push(INT32_MAX);
        return false;
    }
}

template<typename T>
void ExternalSort<T>::BufferToDisk(int j)
{
    IOcount++;
    int mm = pooling[k + 1].size();
    for (int l = 0; l < mm; ++l)
    {
        resDisk[j + 1].push_back(pooling[k + 1].front());
        pooling[k + 1].pop();
    }
    pooling[k + 1] = queue<int>();
}