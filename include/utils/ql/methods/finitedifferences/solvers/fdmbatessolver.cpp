/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2010, 2011 Klaus Spanderen

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file fdmbatessolver.cpp
*/

#include <ql/processes/batesprocess.hpp>
#include <ql/methods/finitedifferences/operators/fdmbatesop.hpp>
#include <ql/methods/finitedifferences/solvers/fdm2dimsolver.hpp>
#include <ql/methods/finitedifferences/solvers/fdmbatessolver.hpp>


namespace QuantLib {
    
    FdmBatesSolver::FdmBatesSolver(
            const Handle<BatesProcess>& process,
            const FdmSolverDesc& solverDesc,
            const FdmSchemeDesc& schemeDesc,
            Size integroIntegrationOrder,
            const Handle<FdmQuantoHelper>& quantoHelper)
    : process_(process),
      solverDesc_(solverDesc),
      schemeDesc_(schemeDesc),
      integroIntegrationOrder_(integroIntegrationOrder),
      quantoHelper_(quantoHelper) {
        registerWith(process_);
        registerWith(quantoHelper_);
    }
          
    void FdmBatesSolver::performCalculations() const {
        boost::shared_ptr<FdmLinearOpComposite> op(
            new FdmBatesOp(solverDesc_.mesher, process_.currentLink(),
                           solverDesc_.bcSet, integroIntegrationOrder_,
                           (!quantoHelper_.empty()) 
                                   ? quantoHelper_.currentLink()
                                   : boost::shared_ptr<FdmQuantoHelper>()));

        solver_ = boost::shared_ptr<Fdm2DimSolver>(
                               new Fdm2DimSolver(solverDesc_, schemeDesc_, op));
    }

    Real FdmBatesSolver::valueAt(Real s, Real v) const {
        calculate();
        return solver_->interpolateAt(std::log(s), v);
    }

    Real FdmBatesSolver::deltaAt(Real s, Real v) const {
        calculate();
        return solver_->derivativeX(std::log(s), v)/s;
    }

    Real FdmBatesSolver::gammaAt(Real s, Real v) const {
        calculate();
        const Real x = std::log(s);
        return (solver_->derivativeXX(x, v)-solver_->derivativeX(x, v))/(s*s);
    }

    Real FdmBatesSolver::thetaAt(Real s, Real v) const {
        calculate();
        return solver_->thetaAt(std::log(s), v);
    }

}
