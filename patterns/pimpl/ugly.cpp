#include "bad.h"
#include "ugly.h"

struct SplineImpl {
public:
    SplineImpl(const std::vector<double> &v, const std::vector<double> &x1,
               const std::vector<double> &y2)
        : vec(v), x(x1), y(y2) {
    }
    const std::vector<double> vec;
    const std::vector<double> x;
    const std::vector<double> y;
};

Spline::Spline(const std::vector<double> &x, const std::vector<double> &y, double a, double b) {
    std::vector<double> answer;
    answer.resize(x.size());

    impl_ = std::shared_ptr<SplineImpl>();
    mySplineSnd(x.data(), y.data(), x.size(), a, b, answer.data());
    impl_ = std::make_shared<SplineImpl>(answer, x, y);
}
double Spline::Interpolate(double x) {
    std::vector<double> answer;
    answer.resize(impl_->vec.size());
    //    for (size_t j = 0; j < impl_->vec.size(); ++j) {
    //        answer += impl_->vec[j] * x;
    //    }
    //    return answer;
    //    unsigned long answer;
    //    std::vector<double> a;
    //    a.push_back(x);
    mySplintCube(impl_->x.data(), impl_->y.data(), impl_->vec.data(), this->impl_->vec.size(), x,
                 answer.data());
    return answer[0];
}
