//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4SmartVoxelProxy.cc,v 1.3 2006-06-29 18:33:48 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Class G4SmartVoxelProxy
//
// Implementation
//
// --------------------------------------------------------------------

#include "GPSmartVoxelProxy.h"

// Empty destructor
//
/*
FQUALIFIER
void GPSmartVoxelProxy_Destructor()
{
  ;
}
*/
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: GPSmartVoxelProxy.icc,v 1.4 2006-06-29 18:32:19 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// GPSmartVoxelProxy Inline implementation
//
// --------------------------------------------------------------------

FQUALIFIER
G4bool GPSmartVoxelProxy_IsHeader(GEOMETRYLOC GPSmartVoxelProxy *This)
{
  //  return (This->fHeader) ? true:false;
  return ( This->fHeader != GEOMETRYNULL );
}   

FQUALIFIER
G4bool GPSmartVoxelProxy_IsNode(GEOMETRYLOC GPSmartVoxelProxy *This)
{
  //  return (This->fNode) ? true:false;
  return ( This->fNode != GEOMETRYNULL ); 
}

FQUALIFIER
GEOMETRYLOC GPSmartVoxelNode* GPSmartVoxelProxy_GetNode(GEOMETRYLOC GPSmartVoxelProxy *This)
{
  //  assert(fNode != 0);
  return This->fNode;
}

FQUALIFIER
GEOMETRYLOC GPSmartVoxelHeader* GPSmartVoxelProxy_GetHeader(GEOMETRYLOC GPSmartVoxelProxy *This)
{
  //  assert(fHeader != 0);
  return This->fHeader;
}

FQUALIFIER
G4bool GPSmartVoxelProxy_operator_equal(GPSmartVoxelProxy *This,
					GPSmartVoxelProxy *v) 
{
  return (This==v) ? true : false;
}
