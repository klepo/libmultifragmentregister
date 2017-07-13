/**
 * @file        optimisationmodel.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing optimization models declarations.
 *
 * @copyright   Copyright (C) 2016 Ondrej Klima, Petr Kleparnik. All Rights Reserved.
 *
 * @license     This file may be used, distributed and modified under the terms of the LGPL version 3
 *              open source license. A copy of the LGPL license should have
 *              been recieved with this file. Otherwise, it can be found at:
 *              http://www.gnu.org/copyleft/lesser.html
 *              This file has been created as a part of the Traumatech project:
 *              http://www.fit.vutbr.cz/research/grants/index.php.en?id=733.
 *
 */

#ifndef OPTIMISATIONMODEL_H
#define OPTIMISATIONMODEL_H

#include <dlib/matrix.h>
#include <dlib/algs.h>
#include <dlib/optimization/optimization_stop_strategies_abstract.h>
#include <dlib/optimization/optimization_stop_strategies.h>

#include "Observer/tobserver.h"

#include <QDebug>

/**
 * @brief Optimisation model involving Levenberg-Marquardt algorithm
 */
template <
    typename column_vector_type,
    typename funct_type,
    typename funct_der_type,
    typename vector_type,
    typename libmfr_type
    >
class libmfr_least_squares_lm_function_model
{
public:
    libmfr_least_squares_lm_function_model (
        const funct_type& f_,
        const funct_der_type& der_,
        const vector_type& list_,
        libmfr_type *libmfr_
    ) : f(f_), der(der_), list(list_), libmfr(libmfr_)
    {
        r.set_size(list.size(),1);
    }

    const funct_type& f;
    const funct_der_type& der;
    const vector_type& list;
    libmfr_type *libmfr;

    typedef typename column_vector_type::type type;
    typedef typename column_vector_type::mem_manager_type mem_manager_type;
    typedef typename column_vector_type::layout_type layout_type;
    const static long NR = column_vector_type::NR;

    typedef column_vector_type column_vector;
    typedef dlib::matrix<type,NR,NR,mem_manager_type,layout_type> general_matrix;

    mutable dlib::matrix<type,0,1,mem_manager_type,layout_type> r;
    mutable column_vector vtemp;

    type operator() (
        const column_vector& x
    ) const
    {
        type result = 0;
        for (long i = 0; i < list.size(); ++i)
        {
            const type temp = (libmfr->*f)(list(i), x);
            // save the residual for later
            r(i) = temp;
            result += temp*temp;
        }

        return 0.5*result;
    }

    void get_derivative_and_hessian (
        const column_vector& x,
        column_vector& d,
        general_matrix& h
    ) const
    {
        d = 0;
        h = 0;
        for (long i = 0; i < list.size(); ++i)
        {
            vtemp = (libmfr->*der)(list(i), x);
            d += r(i)*vtemp;
            h += vtemp*trans(vtemp);
        }
    }

};

// ----------------------------------------------------------------------------------------

template <
    typename column_vector_type,
    typename funct_type,
    typename funct_der_type,
    typename vector_type,
    typename libmfr_type
    >
libmfr_least_squares_lm_function_model<column_vector_type,funct_type,funct_der_type,vector_type,libmfr_type> libmfr_least_squares_lm_model (
    const funct_type& f,
    const funct_der_type& der,
    const vector_type& list,
    libmfr_type *libmfr
)
{
    return libmfr_least_squares_lm_function_model<column_vector_type,funct_type,funct_der_type,vector_type,libmfr_type>(f,der,list,libmfr);
}

// ----------------------------------------------------------------------------------------

template <
    typename stop_strategy_type,
    typename funct_type,
    typename funct_der_type,
    typename vector_type,
    typename T,
    typename libmfr_type
    >
double libmfr_solve_least_squares_lm (
    stop_strategy_type stop_strategy,
    const funct_type& f,
    const funct_der_type& der,
    const vector_type& list,
    T& x,
    libmfr_type *libmfr,
    double radius = 1
)
{
    // The starting point (i.e. x) must be a column vector.
    COMPILE_TIME_ASSERT(T::NC <= 1);

    // make sure requires clause is not broken
    DLIB_ASSERT(is_vector(mat(list)) && list.size() > 0 &&
                is_col_vector(x) && radius > 0,
        "\t double solve_least_squares_lm()"
        << "\n\t invalid arguments were given to this function"
        << "\n\t is_vector(list):  " << is_vector(mat(list))
        << "\n\t list.size():      " << list.size()
        << "\n\t is_col_vector(x): " << is_col_vector(x)
        << "\n\t radius:           " << radius
        );

    return dlib::find_min_trust_region(stop_strategy,
                                       libmfr_least_squares_lm_model<T>(f, der, mat(list), libmfr),
                                       x,
                                       radius);
}

// ----------------------------------------------------------------------------------------

/**
 * @brief Stop strategy for the optimisation model
 */
template <typename libmfr_type>
class libmfr_objective_delta_stop_strategy
{
public:
    explicit libmfr_objective_delta_stop_strategy (
        double min_delta = 1e-7
    ) : _verbose(false), _been_used(false), _min_delta(min_delta), _max_iter(0), _cur_iter(0), _prev_funct_value(0)
    {
        DLIB_ASSERT (
            min_delta >= 0,
            "\t objective_delta_stop_strategy(min_delta)"
            << "\n\t min_delta can't be negative"
            << "\n\t min_delta: " << min_delta
        );
    }

    libmfr_objective_delta_stop_strategy (
        double min_delta,
        unsigned long max_iter,
        libmfr_type *libmfr
    ) : _verbose(false), _been_used(false), _min_delta(min_delta), _max_iter(max_iter), _cur_iter(0), _prev_funct_value(0), _libmfr(libmfr)
    {
        DLIB_ASSERT (
            min_delta >= 0 && max_iter > 0,
            "\t objective_delta_stop_strategy(min_delta, max_iter)"
            << "\n\t min_delta can't be negative and max_iter can't be 0"
            << "\n\t min_delta: " << min_delta
            << "\n\t max_iter:  " << max_iter
        );
    }

    libmfr_objective_delta_stop_strategy<libmfr_type>& be_verbose(

    )
    {
        _verbose = true;
        return *this;
    }

    template <typename T>
    bool should_continue_search (
        const T& ,
        const double funct_value,
        const T&
    )
    {

        TObserver * observer = _libmfr->getObserver();
        if(observer != NULL)
        {
            //std::cout << _libmfr->pointToPointDistance() << "\n";
            observer->iteration(_cur_iter, funct_value);
            _libmfr->paramsChanged();
            if(observer->images())
                observer->downloadImages(_libmfr->getImages());
            else
                _libmfr->getImages();
        }


        if (_verbose)
        {
            using namespace std;
            cout << "iteration: " << _cur_iter << "   objective: " << funct_value << endl;
        }

        ++_cur_iter;
        if (_been_used)
        {
            // Check if we have hit the max allowable number of iterations.  (but only
            // check if _max_iter is enabled (i.e. not 0)).
            if (_max_iter != 0 && _cur_iter > _max_iter)
                return false;

            // check if the function change was too small
            if (std::abs(funct_value - _prev_funct_value) < _min_delta)
                return false;
        }

        _been_used = true;
        _prev_funct_value = funct_value;
        return true;
    }

private:
    bool _verbose;

    bool _been_used;
    double _min_delta;
    unsigned long _max_iter;
    unsigned long _cur_iter;
    double _prev_funct_value;
    libmfr_type *_libmfr;
};

#endif // OPTIMISATIONMODEL_H

