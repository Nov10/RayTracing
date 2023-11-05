#ifndef INTERVAL_H
#define INTERVAL_H

/// <summary>
/// 시작과 끝을 가지는 구간.
/// </summary>
class interval {
public:
    double min, max; //시작(min), 끝(max)

    interval() : min(+infinity), max(-infinity) {} //기본값으로 빈 공간으로 설정합니다.
    interval(double _min, double _max) : min(_min), max(_max) {} //생성자

    //닫힌 구간 내에 존재하는지 확인합니다.
    bool contains(double x) const {
        return min <= x && x <= max;
    }
    //열린 구간 내에 존재하는지 확인합니다.
    bool surrounds(double x) const {
        return min < x && x < max;
    }
    //구간 내부의 값으로 설정합니다.
    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;
};

const static interval empty(+infinity, -infinity); //빈 공간 (양의 무한대~음의 무한대의 공간은 존재하지 않으므로)
const static interval universe(-infinity, +infinity); //모든 공간(음의 무한대~양의 무한대)

#endif