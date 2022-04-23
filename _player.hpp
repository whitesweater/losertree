//
// Created by 90302 on 2022/4/12.
//

#ifndef WEEK1__PLAYER_HPP
#define WEEK1__PLAYER_HPP

#include <io.h>
#include <string>
#include <vector>

//#include <wsman.h>
//获取一个文件夹下面所有的文件名
void getFileNames(std::string path, std::vector<std::string> &files);


class _player
{
public:
    int _low;//低优先级
    int _high;//高优先级

    _player()
            : _low(0), _high(1)
    {}

    bool operator<=(_player &right) const//比赛规则
    {
        if (_high == right._high)//归并段号优先处理
            return _low <= right._low;
        else
            return _high <= right._high;
    }

};


#endif //WEEK1__PLAYER_HPP
