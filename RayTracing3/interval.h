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

    static const interval empty, universe;
};

const static interval empty(+infinity, -infinity); //�� ���� (���� ���Ѵ�~���� ���Ѵ��� ������ �������� �����Ƿ�)
const static interval universe(-infinity, +infinity); //��� ����(���� ���Ѵ�~���� ���Ѵ�)

#endif