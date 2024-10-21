// 最小编辑距离测试

#include <iostream>
#include <string>
#include <vector>

std::string _queryWord("hello");

/* 区分中英文 */
static size_t nBytesCode(const char ch) {
    if (ch & (1 << 7)) {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx) {
            if (ch & (1 << (6 - idx))) {
                ++nBytes;
            } else
                break;
        }
        return nBytes;
    }
    return 1;
}

/* 计算字符串长度,  */
static std::size_t length(const std::string &str) {
    std::size_t ilen = 0;
    for (std::size_t idx = 0; idx != str.size();) {
        int nBytes = nBytesCode(str[idx]);
        idx += nBytes;
        ++ilen;
    }
    return ilen;
}

/* 计算三个数的最小数 */
static std::size_t triple_min(const int &a, const int &b, const int &c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/* 计算最小编辑距离 */
std::size_t minEditDist(const std::string &other) {
    // 1.计算字符串长度, 因为中英文所长字节同步, 不能str.size()计算
    std::size_t querySize = length(_queryWord);
    std::size_t otherSize = length(other);

    // 2.创建二维数组, 并初始化
    std::size_t editDist[querySize + 1][otherSize + 1]; // +1 表示从空串到完整字符串的编辑距离
    for (int idx = 0; idx <= querySize; ++idx) {
        editDist[idx][0] = idx;
    }
    for (int idx = 0; idx <= otherSize; ++idx) {
        editDist[0][idx] = idx;
    }

    // 3.最小编辑距离:动态规划
    std::string subQueryStr, subOtherStr;
    for (std::size_t dist_row = 1, query_i = 0; dist_row <= querySize; ++dist_row) {
        // 1.计算行的当前字符所占长字节数
        std::size_t rowByteChNum = nBytesCode(_queryWord[query_i]);
        subQueryStr = _queryWord.substr(query_i, rowByteChNum); // 获取当前字符
        query_i += rowByteChNum;

        // 2.遍历列, 计算行变为列需要编辑的距离
        for (std::size_t dist_col = 1, other_i = 0; dist_col <= otherSize; ++dist_col) {
            // 3.获取列的当前字符所占字节数
            std::size_t colByteChNum = nBytesCode(other[other_i]);
            subOtherStr = other.substr(other_i, colByteChNum);
            other_i += colByteChNum;

            /*
             * 4.判断 行的字符 与 列的字符 是否相等
             * 相等 : 则等于左上角的值
             * 不相等 : 上 左 坐上, 取最小值 +1
             */
            if (subQueryStr == subOtherStr) {
                editDist[dist_row][dist_col] = editDist[dist_row - 1][dist_col - 1];
            } else {
                editDist[dist_row][dist_col] =
                    triple_min(editDist[dist_row][dist_col - 1] + 1, editDist[dist_row - 1][dist_col] + 1,
                               editDist[dist_row - 1][dist_col - 1] + 1);
            }
        }
    }

    // 5.二位数组的最后即为所求值
    return editDist[querySize][otherSize];
}

int main(void) {
    // std::cout << length("hello") << "\n";

    std::cout << "hi minEditDist = " << minEditDist("hi") << "\n";
    std::cout << "world minEditDist = " << minEditDist("world") << "\n";
    std::cout << "你好 minEditDist = " << minEditDist("你好") << "\n";
    return 0;
}