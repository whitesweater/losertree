#include <iostream>
#include "loserTree.h"
#include "loserTree.cpp"
#include "ExternalSort.h"
#include "ExternalSort.cpp"

using namespace std;

int main()
{
    int k = 4, size = 1000;// 归并路数 buffer大小

    ExternalSort<int> a(k, size, R"(D:\cat1\input\input\data1)",
                        R"(E:\code\cppcode\datastruct_design\week1\ans)");//这两个东西目前的处理方式是直接手动输入 之后用空了再去文件读入
    a.merge();
    cout << "磁盘访问次数为： " << a.IOcount << endl;
    cout << endl;

    std::string aa;

    return 0;

}