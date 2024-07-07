#ifndef INTERVAL_H
#define INTERVAL_H

/// <summary>
/// ���۰� ���� ������ ����.
/// </summary>
class interval {
public:
    double min, max; //����(min), ��(max)

    interval() : min(+infinity), max(-infinity) {} //�⺻������ �� �������� �����մϴ�.
    interval(double _min, double _max) : min(_min), max(_max) {} //������
    interval(const interval& a, const interval& b) {
        // Create the interval tightly enclosing the two input intervals.
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }
    double size() const { 
        return max - min;
    }
    //���� ���� ���� �����ϴ��� Ȯ���մϴ�.
    bool contains(double x) const {
        return min <= x && x <= max;
    }
    //���� ���� ���� �����ϴ��� Ȯ���մϴ�.
    bool surrounds(double x) const {
        return min < x && x < max;
    }
    //���� ������ ������ �����մϴ�.
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

const interval interval::empty(+infinity, -infinity); //�� ���� (���� ���Ѵ�~���� ���Ѵ��� ������ �������� �����Ƿ�)
const interval interval::universe(-infinity, +infinity); //��� ����(���� ���Ѵ�~���� ���Ѵ�)
interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}
#endif