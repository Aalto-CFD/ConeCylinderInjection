/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of ConeCylinderInjection submodel, derived from OpenFOAM.

    https://github.com/Aalto-CFD/ConeCylinderInjection

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::ConeCylinderInjection

Description
    This injection submodel extends standard ConeInjection model from OpenFOAM
    into 3D. This is achieved by extending the "disc" injection model into 
    "cylinder" injection model. The parameters for position, direction, cone
    angles, and disc inner and outer diameters are all preserved for the new 
    model, while the parameter "hCylinder" is required to set cylinder height.
    Therefore, instead of injecting parcels over a disc (2D area), they are
    introduced randomly within finite volume cylinder (3D).

Usage
    \table
    Property        | Description                    | Required | Default
    position        | The injection position         | yes      |
    direction       | The injection direction        | yes      |
    thetaInner      | The inner cone angle           | yes      |
    thetaOuter      | The outer cone angle           | yes      |
    injectionMethod | Inject into a cylinder         | yes      |
    dInner          | The inner cylinder diameter    | yes      |
    dInner          | The outer cylinder diameter    | yes      |
    hCylinder       | The cylinder height            | yes      |
    offsetCylinder  | Offset cylinder from injector position | yes |
    flowType        | Inject with constantVelocity, pressureDrivenVelocity \\
                                 or flowRateAndDischarge | no | constantVelocity
    Umag            | The injection velocity         | if constantVelocity |
    Pinj            | The injection pressure         |\\
                                                     if pressureDrivenVelocity |
    Cd              | The discharge coefficient      | if flowRateAndDischarge |
    \endtable

    Example specification:

    \verbatim
    injectionModels
    {
        model1
        {
            type            coneCylinderInjection;
            // Times
            SOI             0;
            duration        1;
            // Quantities
            massTotal       0; // <-- not used with these settings
            parcelBasisType fixed;
            parcelsPerSecond 1000000;
            flowRateProfile constant 1;
            nParticle       1;
            // Sizes
            sizeDistribution
            {
                type        fixedValue;
                fixedValueDistribution
                {
                    value   0.0025;
                }
            }
            // Geometry
            position        (-0.15 -0.1 0);
            direction       (1 0 0);
            thetaInner      0;
            thetaOuter      45;
            injectionMethod cylinder;
            dInner          0;
            dOuter          0.05;
            hCylinder       0.05;
            offsetCylinder  0.0;

            // Velocity
            // - Inject with constant velocity
            flowType        constantVelocity;
            Umag            1;
            //// - Or, inject with flow rate and discharge coefficient
            ////   This also requires massTotal, dInner and dOuter
            //flowType        flowRateAndDischarge;
            //Cd              0.9;
            //// - Or, inject at a pressure
            //flowType        pressureDrivenVelocity;
            //Pinj            10e5;
        }
    }
    \endverbatim

SourceFiles
    ConeCylinderInjection.C


\*---------------------------------------------------------------------------*/

#ifndef ConeCylinderInjection_H
#define ConeCylinderInjection_H

#include "InjectionModel.H"
#include "distributionModel.H"
#include "TimeFunction1.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class ConeCylinderInjection Declaration
\*---------------------------------------------------------------------------*/

// ConeCylinderInjection model is in same inheritance level as ConeInjection
template<class CloudType>
class ConeCylinderInjection
:
    public InjectionModel<CloudType>
{
public:

    //- Injection method enumeration
    enum injectionMethod
    {
        imCylinder
    };

    //- Flow type enumeration
    enum flowType
    {
        ftConstantVelocity,
        ftPressureDrivenVelocity,
        ftFlowRateAndDischarge
    };

private:

    // Private Data

        //- Cylindrical injection method
        injectionMethod injectionMethod_;

        //- Flow type
        flowType flowType_;

        //- Position of the injector
        const TimeFunction1<vector> position_;

        //- Is the position constant?
        const bool positionIsConstant_;

        //- Centreline direction in which to inject
        const TimeFunction1<vector> direction_;

        //- Cell label corresponding to the injector position
        label injectorCell_;

        //- Tet-face label corresponding to the injector position
        label injectorTetFace_;

        //- Tet-point label corresponding to the injector position
        label injectorTetPt_;

        //- Injection duration [s]
        scalar duration_;

        //- Number of parcels to introduce per second
        const label parcelsPerSecond_;

        //- Flow rate profile relative to SOI []
        const TimeFunction1<scalar> flowRateProfile_;

        //- Inner half-cone angle relative to SOI [deg]
        const TimeFunction1<scalar> thetaInner_;

        //- Outer half-cone angle relative to SOI [deg]
        const TimeFunction1<scalar> thetaOuter_;

        //- Parcel size distribution model
        const autoPtr<distributionModel> sizeDistribution_;


        // Disc geometry

            //- The inner cylinder diameter [m]
            scalar dInner_;

            //- The outer cylinder diameter [m]
            scalar dOuter_;

        //- Cylinder height [m]
            scalar hCylinder_;

        //- Offset of the cylinder origin to injector position [m]
            scalar offsetCylinder_;


        // Velocity model coefficients

            //- Parcel velocity [m/s]
            TimeFunction1<scalar> Umag_;

            //- Discharge coefficient []
            TimeFunction1<scalar> Cd_;

            //- Injection pressure [Pa]
            TimeFunction1<scalar> Pinj_;


    // Private Member Functions

        //- Set the injection type
        void setInjectionMethod();

        //- Set the injection flow type
        void setFlowType();


public:

    //- Runtime type information
    TypeName("coneCylinderInjection");


    // Constructors

        //- Construct from dictionary
        ConeCylinderInjection
        (
            const dictionary& dict,
            CloudType& owner,
            const word& modelName
        );

        //- Construct copy
        ConeCylinderInjection(const ConeCylinderInjection<CloudType>& im);

        //- Construct and return a clone
        virtual autoPtr<InjectionModel<CloudType>> clone() const
        {
            return autoPtr<InjectionModel<CloudType>>
            (
                new ConeCylinderInjection<CloudType>(*this)
            );
        }


    //- Destructor
    virtual ~ConeCylinderInjection();


    // Member Functions

        //- Set injector locations when mesh is updated
        virtual void updateMesh();

        //- Return the end-of-injection time
        scalar timeEnd() const;

        //- Number of parcels to introduce relative to SOI
        virtual label parcelsToInject(const scalar time0, const scalar time1);

        //- Volume of parcels to introduce relative to SOI
        virtual scalar volumeToInject(const scalar time0, const scalar time1);



        // Injection geometry

            //- Set the injection position and owner cell, tetFace and tetPt
            virtual void setPositionAndCell
            (
                const label parcelI,
                const label nParcels,
                const scalar time,
                vector& position,
                label& cellOwner,
                label& tetFacei,
                label& tetPti
            );

            //- Set the parcel properties
            virtual void setProperties
            (
                const label parcelI,
                const label nParcels,
                const scalar time,
                typename CloudType::parcelType& parcel
            );

            //- Flag to identify whether model fully describes the parcel
            virtual bool fullyDescribed() const;

            //- Return flag to identify whether or not injection of parcelI is
            //  permitted
            virtual bool validInjection(const label parcelI);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "ConeCylinderInjection.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
