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
/// \file EventAction.cc
/// \brief Implementation of the B1::EventAction class
#include "G4Run.hh"
#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4CsvAnalysisManager.hh"

#include "G4SystemOfUnits.hh"


using AnalysisManager = G4CsvAnalysisManager;

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction)
: fEnergyWall(0), 
  fEnergyHuman1(0), 
  fEnergyHuman2(0),
  fRunAction(runAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event*)
{
  fEnergyWall = 0;
  fEnergyHuman1 = 0;
  fEnergyHuman2 = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // G4cout << "\n\nHere is the EndOfEventAction" << G4endl;

  // 每跑完 10000 个事例时，打印进度
  if (event->GetEventID() % 1000000 == 0)
  {
    G4int totalNums = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();
    G4cout << "Progress: " << event->GetEventID() << " / " << totalNums << G4endl;
  }

  size_t indexWall = static_cast<size_t>(fEnergyWall / 0.1);
  size_t indexHuman1 = static_cast<size_t>(fEnergyHuman1 / 0.1);
  size_t indexHuman2 = static_cast<size_t>(fEnergyHuman2 / 0.1);

  
  // G4cout 每一个值
  // G4cout << "Energy deposited in wall: " << fEnergyWall << " keV" << G4endl;
  // G4cout << "Energy deposited in human1: " << fEnergyHuman1 << " keV" << G4endl;
  // G4cout << "Energy deposited in human2: " << fEnergyHuman2 << " keV" << G4endl;
  // G4cout <<"indexWall = " << indexWall << G4endl;
  // G4cout <<"indexHuman1 = " << indexHuman1 << G4endl;
  // G4cout <<"indexHuman2 = " << indexHuman2 << G4endl;
  // G4cout <<"fRunAction->fEnergySpectrumWall.size() = " << fRunAction->fEnergySpectrumWall.size() << G4endl;

  if (indexWall < fRunAction->fEnergySpectrumWall.size())
  {
    fRunAction->fEnergySpectrumWall[indexWall] += 1;
    // G4cout << "accumulate energy in wall for index = " << indexWall << G4endl;
  }
  if (indexHuman1 < fRunAction->fEnergySpectrumHuman1.size())
  {
    fRunAction->fEnergySpectrumHuman1[indexHuman1] += 1;
    // G4cout << "accumulate energy in human1 for index = " << indexHuman1 << G4endl;
  }
  if (indexHuman2 < fRunAction->fEnergySpectrumHuman2.size())
  {
    fRunAction->fEnergySpectrumHuman2[indexHuman2] += 1;
    // G4cout << "accumulate energy in human2 for index = " << indexHuman2 << G4endl;
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
