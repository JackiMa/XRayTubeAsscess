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
/// \file RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
// #include "Run.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4RootAnalysisManager.hh"
#include "time.h"

#include <fstream>

using AnalysisManager = G4RootAnalysisManager;

// 在类外部定义全局静态变量
static std::vector<G4double> gEnergySpectrumWall(500, 0);
static std::vector<G4double> gEnergySpectrumHuman1(500, 0);
static std::vector<G4double> gEnergySpectrumHuman2(500, 0);
static std::mutex gMutex;

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction() 
: fEnergySpectrumWall(500, 0), 
  fEnergySpectrumHuman1(500, 0), 
  fEnergySpectrumHuman2(500, 0) 
{ 

} 


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  // clean fEnergySpectrum
  for (size_t i = 0; i < fEnergySpectrumHuman2.size(); ++i)
  {
    fEnergySpectrumHuman1[i] = 0;
    fEnergySpectrumHuman2[i] = 0;
    gEnergySpectrumWall[i] = 0;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run*)
{

  
  if (IsMaster()) {
        // 遍历G4cout一下所有的spectrum
    G4cout << "gEnergySpectrumWall: " << G4endl;
    for (size_t i = 0; i < gEnergySpectrumWall.size(); ++i)
    {
      G4cout << gEnergySpectrumWall[i] << " ";
    }
    G4cout << G4endl;
    G4cout << "gEnergySpectrumHuman1: " << G4endl;
    for (size_t i = 0; i < gEnergySpectrumHuman1.size(); ++i)
    {
      G4cout << gEnergySpectrumHuman1[i] << " ";
    }
    G4cout << G4endl;
    G4cout << "gEnergySpectrumHuman2: " << G4endl;
    for (size_t i = 0; i < gEnergySpectrumHuman2.size(); ++i)
    {
      G4cout << gEnergySpectrumHuman2[i] << " ";
    }
    G4cout << G4endl;

      G4String filename = "New_Edep_spectrum";
    time_t now = time(0);
    //把now转换为string形式
    tm *ltm = localtime(&now);
    filename += std::to_string(ltm->tm_year + 1900);
    filename += std::to_string(ltm->tm_mon + 1);
    filename += std::to_string(ltm->tm_mday);
    filename += std::to_string(ltm->tm_hour);
    filename += std::to_string(ltm->tm_min);
    filename += std::to_string(ltm->tm_sec);
    filename += ".csv";

    // 创建一个ofstream对象，用于写入文件
    std::ofstream outFile;
    outFile.open(filename);

    // 写入列名
    outFile << "Energy,Wall,Human1,Human2\n";

    // 写入数据
    for (size_t i = 0; i < gEnergySpectrumWall.size(); ++i)
    {
      outFile << i * 0.1 << ","
              << gEnergySpectrumWall[i] << ","
              << gEnergySpectrumHuman1[i] << ","
              << gEnergySpectrumHuman2[i] << "\n";
    }

    // 关闭文件
    outFile.close();
    
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------"
     << G4endl;
  }
  else {
    // 在worker线程中，将结果合并到主线程
    const RunAction* localRun = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());

    G4cout << "--------------------Local Run-----------------------------" << G4endl;
    G4cout << "workID = " << G4Threading::G4GetThreadId() << G4endl;

    // 使用互斥锁保护共享数据
    std::lock_guard<std::mutex> lock(gMutex);

    // 合并结果
    for (size_t i = 0; i < gEnergySpectrumWall.size(); ++i)
    {
      gEnergySpectrumWall[i] += localRun->fEnergySpectrumWall[i];
      gEnergySpectrumHuman1[i] += localRun->fEnergySpectrumHuman1[i];
      gEnergySpectrumHuman2[i] += localRun->fEnergySpectrumHuman2[i];
    }

    // 遍历G4cout一下所有的spectrum
    G4cout << "fEnergySpectrumWall: " << G4endl;
    for (size_t i = 0; i < fEnergySpectrumWall.size(); ++i)
    {
      G4cout << fEnergySpectrumWall[i] << " ";
    }
    G4cout << G4endl;
    G4cout << "fEnergySpectrumHuman1: " << G4endl;
    for (size_t i = 0; i < fEnergySpectrumHuman1.size(); ++i)
    {
      G4cout << fEnergySpectrumHuman1[i] << " ";
    }
    G4cout << G4endl;
    G4cout << "fEnergySpectrumHuman2: " << G4endl;
    for (size_t i = 0; i < fEnergySpectrumHuman2.size(); ++i)
    {
      G4cout << fEnergySpectrumHuman2[i] << " ";
    }
    G4cout << G4endl;
    G4cout

     << G4endl
     << "--------------------End of Local Run------------------------"
     << G4endl;
  }
}


}
