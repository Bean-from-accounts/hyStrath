/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2007 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Description

\*---------------------------------------------------------------------------*/

#include "dsmcSpecularTWallPatch.H"
#include "addToRunTimeSelectionTable.H"
#include "fvc.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(dsmcSpecularTWallPatch, 0);

addToRunTimeSelectionTable(dsmcPatchBoundary, dsmcSpecularTWallPatch, dictionary);



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
dsmcSpecularTWallPatch::dsmcSpecularTWallPatch
(
    Time& t,
    const polyMesh& mesh,
    dsmcCloud& cloud,
    const dictionary& dict
)
:
    dsmcPatchBoundary(t, mesh, cloud, dict),
    propsDict_(dict.subDict(typeName + "Properties"))
{
    writeInTimeDir_ = false;
    writeInCase_ = false;
    measurePropertiesAtWall_ = true;

    setProperties();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

dsmcSpecularTWallPatch::~dsmcSpecularTWallPatch()
{}



// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
void dsmcSpecularTWallPatch::initialConfiguration()
{}


void dsmcSpecularTWallPatch::calculateProperties()
{}


void dsmcSpecularTWallPatch::controlParticle(dsmcParcel& p, dsmcParcel::trackingData& td)
{
    measurePropertiesBeforeControl(p);

    vector& U = p.U();

    scalar& ERot = p.ERot();
    
    labelList& vibLevel = p.vibLevel();
    
    label& ELevel = p.ELevel();

    const label& typeId = p.typeId();

    vector nw = p.normal();
    nw /= mag(nw);

    scalar U_dot_nw = U & nw;

    if (U_dot_nw > 0.0)
    {
        U -= 2.0*U_dot_nw*nw;
    }

    const scalar& T = temperature_;

    scalar rotationalDof = cloud_.constProps(typeId).rotationalDegreesOfFreedom();
    
    scalar vibrationalDof = cloud_.constProps(typeId).nVibrationalModes();

    ERot = cloud_.equipartitionRotationalEnergy(T, rotationalDof);
    
    vibLevel = 
        cloud_.equipartitionVibrationalEnergyLevel(T, vibrationalDof, typeId);
   
    
    ELevel = cloud_.equipartitionElectronicLevel
        (
            T,
            cloud_.constProps(typeId).electronicDegeneracyList(),
            cloud_.constProps(typeId).electronicEnergyList()
        ); 
    
    measurePropertiesAfterControl(p, 0.0);
}


void dsmcSpecularTWallPatch::output
(
    const fileName& fixedPathName,
    const fileName& timePath
)
{

}


void dsmcSpecularTWallPatch::updateProperties(const dictionary& newDict)
{
    //- the main properties should be updated first
    updateBoundaryProperties(newDict);
}


void dsmcSpecularTWallPatch::setProperties()
{
    temperature_ = readScalar(propsDict_.lookup("temperature"));
}


} // End namespace Foam

// ************************************************************************* //
