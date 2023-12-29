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
/// \file DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeX = 3*m;
  G4double world_sizeY = 2.5*m;
  G4double world_sizeZ  = 5*m;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidWorld =
    new G4Box("World",                       //its name
       0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);     //its size

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // 
  // human parameters
  //
  G4Material* human = nist->FindOrBuildMaterial("G4_A-150_TISSUE");
  G4RotationMatrix* rotm = new G4RotationMatrix();
  rotm->rotateX(90*deg); // rotate the human to stand

  // human 1
  G4double human_height = 200*cm, human_waistradius = 30*cm;
  G4Tubs*  human_soild = new G4Tubs("human",0,human_waistradius,0.5*human_height,0,360*deg);
  G4LogicalVolume* human_1_logic = new G4LogicalVolume(human_soild, human, "human_1");
  new G4PVPlacement(rotm,G4ThreeVector(-1.4*m,0,0),human_1_logic,"human_1",logicWorld,false,0,checkOverlaps); 
  
  // human 2
  G4LogicalVolume* human_2_logic = new G4LogicalVolume(human_soild, human, "human_2");
  new G4PVPlacement(rotm,G4ThreeVector(0.1*m,0,2.0*m),human_2_logic,"human_2",logicWorld,false,0,checkOverlaps); 
  
  G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0)); // RGB for orange
  visAttributes->SetVisibility(true);
  visAttributes->SetForceWireframe(true); // Set to wireframe
  visAttributes->SetForceAuxEdgeVisible(true); // Show edges
  human_1_logic->SetVisAttributes(visAttributes);
  human_2_logic->SetVisAttributes(visAttributes);

  
  // 
  // wall parameters
  //
  G4Material* concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
  G4Material* material_Pb = nist->FindOrBuildMaterial("G4_Pb");

  // wall 1
  G4double wall_1_sizeX = 150*cm, wall_1_sizeY = 200*cm, wall_1_sizeZ = 10*cm;
  G4double Pb_1_sizeX = wall_1_sizeX, Pb_1_sizeY = wall_1_sizeY, Pb_1_sizeZ = 0.4*cm;
  G4Box* wall_1_soild = new G4Box("wall_1",0.5*wall_1_sizeX, 0.5*wall_1_sizeY, 0.5*wall_1_sizeZ); 
  G4Box* Pb_1_soild = new G4Box("Pb_1",0.5*Pb_1_sizeX, 0.5*Pb_1_sizeY, 0.5*Pb_1_sizeZ); 
  G4LogicalVolume* wall_1_logic = new G4LogicalVolume(wall_1_soild, concrete, "wall_1");
  G4LogicalVolume* Pb_1_logic = new G4LogicalVolume(Pb_1_soild, material_Pb, "Pb_1");
  new G4PVPlacement(0,G4ThreeVector(),Pb_1_logic,"wall",wall_1_logic,false,0,checkOverlaps); 
  new G4PVPlacement(0,G4ThreeVector(0,0,0),wall_1_logic,"wall",logicWorld,false,0,checkOverlaps); 
  
  // wall 2
  G4double wall_2_sizeX = 10*cm, wall_2_sizeY = 200*cm, wall_2_sizeZ = 100*cm;
  G4double Pb_2_sizeX = 0.4*cm, Pb_2_sizeY = wall_2_sizeY, Pb_2_sizeZ = wall_2_sizeZ;
  G4Box* wall_2_soild = new G4Box("wall_2",0.5*wall_2_sizeX, 0.5*wall_2_sizeY, 0.5*wall_2_sizeZ); 
  G4Box* Pb_2_soild = new G4Box("Pb_2",0.5*Pb_2_sizeX, 0.5*Pb_2_sizeY, 0.5*Pb_2_sizeZ); 
  G4LogicalVolume* wall_2_logic = new G4LogicalVolume(wall_2_soild, concrete, "wall_2");
  G4LogicalVolume* Pb_2_logic = new G4LogicalVolume(Pb_2_soild, concrete, "Pb_2");
  new G4PVPlacement(0,G4ThreeVector(),Pb_2_logic,"wall",wall_2_logic,false,0,checkOverlaps); 
  new G4PVPlacement(0,G4ThreeVector(-0.75*m,0,-0.46*m),wall_2_logic,"wall",logicWorld,false,0,checkOverlaps); 



  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
