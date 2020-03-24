//
// Created by jlebas01 on 23/03/2020.
//

#ifndef OPENGL_UTILITOR_HPP
#define OPENGL_UTILITOR_HPP

#include <c3ga/Mvec.hpp>
#include <c3ga/Tools.hpp>

namespace utility {

    template<typename T>
    class Utilitor {

    public :
        static c3ga::Mvec <T> translator(const c3ga::Mvec <T> &multivector, T x, T y, T z);

        static c3ga::Mvec <T> dilator(const c3ga::Mvec <T> &multivector, T scale);

        static c3ga::Mvec<T> test(const c3ga::Mvec<T> &X1, const c3ga::Mvec<T> &X2, T alpha, T beta);

        static c3ga::Mvec<T> interpolator(c3ga::Mvec<T> sphere1,  c3ga::Mvec<T> sphere2, T alpha);
    };

    template<typename T>
    c3ga::Mvec<T> Utilitor<T>::translator(const c3ga::Mvec<T> &multivector, T x, T y, T z) {
        c3ga::Mvec<T> t = 1 - 0.5 * (x * c3ga::e1<T>() + y * c3ga::e2<T>() + z * c3ga::e3<T>()) * c3ga::ei<T>();
        return t * multivector * t.inv();
    }

    template<typename T>
    c3ga::Mvec<T> Utilitor<T>::dilator(const c3ga::Mvec<T> &multivector, T scale) {
        c3ga::Mvec<T> dilator = 1 - ( (1 - scale) / (1 + scale) )  * c3ga::e0i<T>();
        return dilator * multivector * dilator.inv();
    }

    template<typename T>
    c3ga::Mvec<T> Utilitor<T>::test(const c3ga::Mvec<T> &X1, const c3ga::Mvec<T> &X2, T alpha, T beta) {
        c3ga::Mvec<T> interpol = (alpha + beta * (X1 * X2 + X2 * X1)) * (X1 + X2);
        return interpol;
    }

    template<typename T>
    c3ga::Mvec<T> Utilitor<T>::interpolator(c3ga::Mvec<T> sphere1, c3ga::Mvec<T> sphere2, T alpha) {

        c3ga::Mvec<GLfloat> dual = sphere1.dual();
        dual /= dual[c3ga::E0];
        GLfloat rayon1 = dual | dual;
        c3ga::Mvec<GLfloat> center1 = c3ga::point<GLfloat>(dual[c3ga::E1], dual[c3ga::E2], dual[c3ga::E3]);

        c3ga::Mvec<GLfloat> dual2 = sphere2.dual();
        dual2 /= dual2[c3ga::E0];
        GLfloat rayon2 = dual2 | dual2;
        c3ga::Mvec<GLfloat> center2 = c3ga::point<GLfloat>(dual2[c3ga::E1], dual2[c3ga::E2], dual2[c3ga::E3]);

        c3ga::Mvec<GLfloat> zero = c3ga::point(0.0, 0.0, 0.0);
        c3ga::Mvec<T> interpol = alpha * center2 + (1 - alpha) * center1 - 0.5 * (alpha * rayon2 + (1 - alpha)*rayon1) * c3ga::ei<T>();


        c3ga::Mvec<GLfloat> resolve = interpol;
        resolve /= resolve[c3ga::E0];
        c3ga::Mvec<GLfloat> rayon3 = std::abs(resolve | resolve);
        c3ga::Mvec<GLfloat> center3 = c3ga::point<GLfloat>(resolve[c3ga::E1], resolve[c3ga::E2], resolve[c3ga::E3]);

      /*  std::cout << "rayon 1 sqrt : " << std::sqrt(rayon1) << " " << "rayon2 sqrt : " << std::sqrt(rayon2) << std::endl;
        std::cout << "rayon 1: " << rayon1 << " " << "rayon2 : " << rayon2 << std::endl;
        std::cout << "rayon3 : " << std::sqrt(std::abs((alpha*rayon1) + (1- alpha) * rayon2) )<< std::endl;*/

        return !interpol;
    }
}

#endif //OPENGL_UTILITOR_HPP
