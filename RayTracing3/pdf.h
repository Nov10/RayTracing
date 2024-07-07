#ifndef PDF_H
#define PDF_H

#include "helper.h"
#include "hittable_list.h"
#include "onb.h"


class pdf {
public:
    virtual ~pdf() {}

    virtual double value(const vector3& direction) const = 0;
    virtual vector3 generate() const = 0;
};


class sphere_pdf : public pdf {
public:
    sphere_pdf() { }

    double value(const vector3& direction) const override {
        return 1 / (4 * pi);
    }

    vector3 generate() const override {
        return random_normalized_vector();
    }
};
class cosine_pdf : public pdf {
public:
    cosine_pdf(const vector3& w) { uvw.build_from_w(w); }

    double value(const vector3& direction) const override {
        auto cosine_theta = dot(normalized(direction), uvw.w());
        return fmax(0, cosine_theta / pi);
    }

    vector3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

private:
    onb uvw;
};

class hittable_pdf : public pdf {
public:
    hittable_pdf(const hittable& objects, const point3& origin)
        : objects(objects), origin(origin)
    {}

    double value(const vector3& direction) const override {
        return objects.pdf_value(origin, direction);
    }

    vector3 generate() const override {
        return objects.random(origin);
    }

private:
    const hittable& objects;
    point3 origin;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
        p[0] = p0;
        p[1] = p1;
    }

    double value(const vector3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    vector3 generate() const override {
        if (random_double() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }

private:
    shared_ptr<pdf> p[2];
};
#endif