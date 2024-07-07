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
    interval(const interval& a, const interval& b) {
        // Create the interval tightly enclosing the two input intervals.
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }
    double size() const { 
        return max - min;
    }
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
    interval expand(double delta) const {
        auto padding = delta / 2;
        return interval(min - padding, max + padding);
    }

    static const interval empty, universe;
};

const interval interval::empty(+infinity, -infinity); //빈 공간 (양의 무한대~음의 무한대의 공간은 존재하지 않으므로)
const interval interval::universe(-infinity, +infinity); //모든 공간(음의 무한대~양의 무한대)
interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}
#endif