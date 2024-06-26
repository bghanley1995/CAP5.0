/* **********************************************************************
 * Copyright (C) 2019-2022, Claude Pruneau, Victor Gonzalez, Sumit Basu
 * All rights reserved.
 *
 * Based on the ROOT package and environment
 *
 * For the licensing terms see LICENSE.
 *
 * Author: Claude Pruneau,   04/01/2022
 *
 * *********************************************************************/
#include "ParticleDbManager.hpp"
#include <string.h>
using CAP::ParticleDbManager;
using namespace std;

ClassImp(ParticleDbManager);

const double factorials[7] = {1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0 };


ParticleDbManager:: ParticleDbManager(const String & _name,
                                      const Configuration & _configuration)
:
EventTask(_name,_configuration),
particleDbImport(true),
particleDbImportPath("ParticleData/"),
particleDbImportFile("particles.data"),
particleDbImportDecaysFile("decays.data"),
particleDbExport(false),
particleDbExportPath("ParticleData/"),
particleDbExportFile("newParticles.data"),
particleDbExportDecaysFile("newDecays.data"),
particleDbDisableAllParticles(false),
particleDbDisableSelected(false),
particleDbEnableAllParticles(false),
particleDbEnableSelected(false),
particleDbDisableAllDecays(false),
particleDbDisableSelectedDecays(false),
particleDbDisableWeakDecays(true),
particleDbDisableLongLivedDecays(false),
particleDbDisableShortLivedDecays(false),
particleDbEnableAllDecays(false),
particleDbEnableSelectedDecays(false),
particleDbEnableWeakDecays(false),
particleDbEnableLongLivedDecays(false),
particleDbEnableShortLivedDecays(false),
particleDbMaxLifeTime(0.0),
particleDbs()
{
  appendClassName("ParticleDbManager");
}

void ParticleDbManager::setDefaultConfiguration()
{
  EventTask::setDefaultConfiguration();
  addParameter("ParticleDbImport",            particleDbImport);
  addParameter("ParticleDbImportPath",        particleDbImportPath);
  addParameter("ParticleDbImportFile",        particleDbImportFile);
  addParameter("ParticleDbImportDecaysFile",  particleDbImportDecaysFile);
  addParameter("ParticleDbExport",            particleDbExport);
  addParameter("ParticleDbExportPath",        particleDbExportPath);
  addParameter("ParticleDbExportFile",        particleDbExportFile);
  addParameter("ParticleDbExportDecaysFile",  particleDbExportDecaysFile);
  
  addParameter("ParticleDbDisableAllParticles",    particleDbDisableAllParticles);
  addParameter("ParticleDbDisableSelected",        particleDbDisableSelected);
  addParameter("ParticleDbEnableAllParticles",     particleDbEnableAllParticles);
  addParameter("ParticleDbEnableSelected",         particleDbEnableSelected);

  addParameter("ParticleDbDisableAllDecays",       particleDbDisableAllDecays);
  addParameter("ParticleDbDisableSelectedDecays",  particleDbDisableSelectedDecays);
  addParameter("ParticleDbDisableWeakDecays",      particleDbDisableWeakDecays);
  addParameter("ParticleDbDisableLongLivedDecays", particleDbDisableLongLivedDecays);
  addParameter("ParticleDbDisableShortLivedDecays",particleDbDisableShortLivedDecays);
  addParameter("ParticleDbEnableAllDecays",        particleDbEnableAllDecays);
  addParameter("ParticleDbEnableSelectedDecays",   particleDbEnableSelectedDecays);
  addParameter("ParticleDbEnableWeakDecays",       particleDbEnableWeakDecays);
  addParameter("ParticleDbEnableLongLivedDecays",  particleDbEnableLongLivedDecays);
  addParameter("ParticleDbEnableShortLivedDecays", particleDbEnableShortLivedDecays);
  addParameter("ParticleDbMaxLifeTime",            particleDbMaxLifeTime);

  // Setup disabling selected particles
  String none("none");
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbDisable"; s += k;
    addParameter(s,none);
    }

  // Setup enabling selected particles
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbEnable"; s += k;
    addParameter(s,none);
    }

  // Setup decay disabling of selected particles
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbDisableDecay"; s += k;
    addParameter(s,none);
    }

  // Setup decay enabling of selected particles
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbEnableDecay"; s += k;
    addParameter(s,none);
    }
}


void ParticleDbManager::configure()
{
  EventTask::configure();
  particleDbImport            = getValueBool(  "ParticleDbImport"           );
  particleDbImportPath        = getValueString("ParticleDbImportPath"       );
  particleDbImportFile        = getValueString("ParticleDbImportFile"       );
  particleDbImportDecaysFile  = getValueString("ParticleDbImportDecaysFile" );
  particleDbExport            = getValueBool(  "ParticleDbExport"           );
  particleDbExportPath        = getValueString("ParticleDbExportPath"       );
  particleDbExportFile        = getValueString("ParticleDbExportFile"       );
  particleDbExportDecaysFile  = getValueString("ParticleDbExportDecaysFile" );

  particleDbDisableAllParticles    = getValueBool(  "ParticleDbDisableAllParticles"       );
  particleDbDisableSelected        = getValueBool(  "ParticleDbDisableSelected"           );
  particleDbEnableAllParticles     = getValueBool(  "ParticleDbEnableAllParticles"        );
  particleDbEnableSelected         = getValueBool(  "ParticleDbEnableSelected"            );
  particleDbDisableAllDecays       = getValueBool(  "ParticleDbDisableAllDecays"          );
  particleDbDisableSelectedDecays  = getValueBool(  "ParticleDbDisableSelectedDecays"     );
  particleDbDisableWeakDecays      = getValueBool(  "ParticleDbDisableWeakDecays"         );
  particleDbDisableLongLivedDecays = getValueBool(  "ParticleDbDisableLongLivedDecays"    );
  particleDbDisableShortLivedDecays= getValueBool(  "ParticleDbDisableShortLivedDecays"   );
  particleDbEnableAllDecays        = getValueBool(  "ParticleDbEnableAllDecays"           );
  particleDbEnableSelectedDecays   = getValueBool(  "ParticleDbEnableSelectedDecays"      );
  particleDbEnableWeakDecays       = getValueBool(  "ParticleDbEnableWeakDecays"          );
  particleDbEnableLongLivedDecays  = getValueBool(  "ParticleDbEnableLongLivedDecays"     );
  particleDbEnableShortLivedDecays = getValueBool(  "ParticleDbEnableLongLivedDecays"     );
  particleDbMaxLifeTime            = getValueDouble("ParticleDbMaxLifeTime"               );



  if (reportInfo(__FUNCTION__))
    {
    cout << endl;
    printItem("ParticleDbImport"           ,particleDbImport);
    printItem("ParticleDbImportPath"       ,particleDbImportPath);
    printItem("ParticleDbImportFile"       ,particleDbImportFile);
    printItem("ParticleDbImportDecaysFile" ,particleDbImportDecaysFile);
    printItem("ParticleDbExport"           ,particleDbExport);
    printItem("ParticleDbExportPath"       ,particleDbExportPath);
    printItem("ParticleDbExportFile"       ,particleDbExportFile);
    printItem("ParticleDbExportDecaysFile" ,particleDbExportDecaysFile);
    printItem("ParticleDbDisableAllParticles"    ,particleDbDisableAllParticles);
    printItem("ParticleDbDisableSelected"        ,particleDbDisableSelected);
    printItem("ParticleDbEnsableAllParticles"    ,particleDbEnableAllParticles);
    printItem("ParticleDbEnableSelected"         ,particleDbEnableSelected);
    printItem("ParticleDbDisableAllDecays"       ,particleDbDisableAllDecays);
    printItem("ParticleDbDisableSelectedDecays"  ,particleDbDisableSelectedDecays);
    printItem("ParticleDbDisableWeakDecays"      ,particleDbDisableWeakDecays);
    printItem("ParticleDbDisableLongLivedDecays" ,particleDbDisableLongLivedDecays);
    printItem("ParticleDbDisableShortLivedDecays",particleDbDisableShortLivedDecays);
    printItem("ParticleDbEnableSelectedDecays"   ,particleDbEnableSelectedDecays);
    printItem("ParticleDbEnableWeakDecays"       ,particleDbEnableWeakDecays);
    printItem("ParticleDbEnableLongLivedDecays"  ,particleDbEnableLongLivedDecays);
    printItem("ParticleDbEnableLongLivedDecays"  ,particleDbEnableShortLivedDecays);
    printItem("ParticleDbMaxLifeTime"            ,particleDbMaxLifeTime);

    for (int k=0; k<40; k++)
      {
      String s = "ParticleDbEnable"; s += k;
      cout << k << "   " << s << "  :  " << getValueString(s) << endl;;
      }
    for (int k=0; k<40; k++)
      {
      String s = "ParticleDbDisable"; s += k;
      cout << k << "   " << s << "  :  " << getValueString(s) << endl;;
      }
    for (int k=0; k<40; k++)
      {
      String s = "ParticleDbDisableDecay"; s += k;
      cout << k << "   " << s << "  :  " << getValueString(s) << endl;;
      }
    for (int k=0; k<40; k++)
      {
      String s = "ParticleDbEnableDecay"; s += k;
      cout << k << "   " << s << "  :  " << getValueString(s) << endl;;
      }
    cout << endl;
    }

  //if (reportDebug(__FUNCTION__)) printConfiguration(cout);
}

void ParticleDbManager::execute()
{  }

void ParticleDbManager::initialize()
{
  incrementTaskExecuted();
  if (particleDbImport || particleDbExport)
    {
    importParticleDb();
    }



  if (particleDbDisableAllParticles) disableAllParticles();

  if (particleDbDisableSelected)
    {
    // First disable all particles, then selectively
    // choose those to be re-enabled.
    enableAllParticles();
    disableSelectedParticles();
    }
  if (particleDbEnableAllParticles) enableAllParticles();

  if (particleDbEnableSelected)
    {
    // First disable all particles, then selectively
    // choose those to be re-enabled.
    disableAllParticles();
    enableSelectedParticles();
    }


  if (particleDbDisableAllDecays) disableAllDecays();
  if (particleDbDisableSelectedDecays) // default
    {
    // first turn on all decays
    // next optionally turn off selected decays
    enableAllDecays();
    if (particleDbDisableWeakDecays) disableWeakDecays();
    if (particleDbDisableLongLivedDecays)  disableLongLivedWith(particleDbMaxLifeTime);
    if (particleDbDisableShortLivedDecays) disableShortLivedWith(particleDbMaxLifeTime);
    disableSelectedDecays();
    }
  if (particleDbEnableAllDecays) enableAllDecays();
  if (particleDbEnableSelectedDecays)
    {
    // first turn off all decays
    // next optionally turn on selected decays
    disableAllDecays();
    if (particleDbEnableWeakDecays) enableWeakDecays();
    if (particleDbEnableLongLivedDecays)  enableLongLivedWith(particleDbMaxLifeTime);
    if (particleDbEnableShortLivedDecays) enableShortLivedWith(particleDbMaxLifeTime);
    enableSelectedDecays();
    }
  listParticleStatus();
  //exit(1);





}


// legacy code from Chun Shen -- now using Therminator DB
//void ParticleDbManager::importParticleDbCAP()
//{
//  String dbPath = taskDbPath;
//  dbPath += "/";
//  dbPath += particleDbImportPath;
//
//  ifstream & inputFile = openInputAsciiFile(dbPath,particleDbImportFile,".dat");
//  string name, title;
//  double mass, width, decayBranchingRatio;
//  int pdgCode, gSpin, gIsospin, baryonNumber, strangeNumber, charmNumber, bottomNumber, charge;
//  int nDecayModes, decayNpart, dummy_int;
//  int decayPart[5] = {0, 0, 0, 0, 0};
//  int index = 0;
//  ParticleType * particleType;
//  ParticleType * antiParticleType;
//  String theName;
//  String theAntiName;
//  String theTitle;
//  String theAntiTitle;
//  ParticleDb * particleDb = new ParticleDb();
//  particleDbs.push_back(particleDb);
//  ParticleDb::setDefaultParticleDb(particleDb);
//  try
//  {
//  while (1)
//    {
//    if (reportDebug(__FUNCTION__))
//      cout << "Reading in ParticleType." << endl;
//    inputFile >> pdgCode;
//    if (inputFile.eof())
//      {
//      if (reportDebug(__FUNCTION__))
//        cout << "Reached EOF." << endl;
//      break;
//      }
//    inputFile >> name;
//    inputFile >> mass;
//    inputFile >> width;
//    inputFile >> gSpin;
//    inputFile >> baryonNumber;
//    inputFile >> strangeNumber;
//    inputFile >> charmNumber;
//    inputFile >> bottomNumber;
//    inputFile >> gIsospin;
//    inputFile >> charge;
//    inputFile >> nDecayModes;
//    inputFile >> title;
//    theName  = name;
//    theTitle = title;
//    particleType= new ParticleType();
//    //pdgCode, theName, theTitle, mass, width, gSpin, baryonNumber, strangeNumber,
//    //                               charmNumber, bottomNumber, gIsospin, charge);
//    particleType->setIndex(index); index++;
//    particleDb.addParticleType(particleType);
//
//    if (particleType->isFermion())
//      {
//      theAntiName  = "Anti-";
//      theAntiName  += theName;
//      theAntiTitle = "#bar ";
//      theAntiTitle += theTitle;
//      antiParticleType =  new ParticleType();
//      //      -pdgCode, theAntiName, theAntiTitle, mass, width, gSpin,
//      //                                           -baryonNumber, -strangeNumber, -charmNumber, -bottomNumber, gIsospin,
//      //                                           -charge);
//      antiParticleType->setIndex(index); index++;
//      particleDb.addParticleType(antiParticleType);
//      }
//    // read decay information
//    for (int j = 0; j < nDecayModes; j++)
//      {
//      inputFile >> dummy_int;
//      inputFile >> decayNpart;
//      inputFile >> decayBranchingRatio;
//      inputFile >> decayPart[0];
//      inputFile >> decayPart[1];
//      inputFile >> decayPart[2];
//      inputFile >> decayPart[3];
//      inputFile >> decayPart[4];
//      decayNpart = abs(decayNpart);
//      std::vector<int> children;
//      std::vector<int> antiChildren;
//      for (int k=0; k<decayNpart; k++)
//        {
//        children.push_back(decayPart[k]);
//        }
//      if (!particleType->isFermion())
//        {
//        particleType->addDecayMode(decayBranchingRatio,children);
//        }
//      else
//        {
//        for (int k=0; k<decayNpart; k++)
//          {
//          antiChildren.push_back(particleDb.findPdgCode(decayPart[k])->getAntiParticlePdgCode());
//          }
//        particleType->addDecayMode(decayBranchingRatio,children);
//        antiParticleType->addDecayMode(decayBranchingRatio,antiChildren);
//        }
//      }
//    }
//  }
//  catch (...)
//  {
//  throw FileException(particleDbImportPath,particleDbImportFile,"Error reading particle data base file","ParticleDbManager::importParticleDbCAP()");
//  }
//  inputFile.close();
//  if (reportInfo(__FUNCTION__))
//    cout << "Total number of particles read: " <<  particleDb.getNumberOfTypes() << endl;
//  particleDb.resolveTypes();
//  particleDb.sortByMass();
//}

//!
//!Import particle data from DB files.
//!mass, width are in GeV
//!The files were originally from Therminator
//!The files are assumed to be at:
//!    taskDbPath/particleDbImportPath
//! The variable taskDbPath is set as an environment variable
//! The variable particleDbImportPath is set in the project file
//!
void ParticleDbManager::importParticleDb()
{
  String dbPath = taskDbPath;
  dbPath += "/";
  dbPath += particleDbImportPath;

  // read particle file
  if (reportInfo(__FUNCTION__))
    {
    cout << "DB Path............: " << dbPath << endl;
    cout << "Particle File......: " << particleDbImportFile << endl;
    cout << "Expected extension.: .data"<< endl;
    }
  ifstream & inputFile = openInputAsciiFile(dbPath,particleDbImportFile,".data");
  ParticleDb * particleDb = new ParticleDb();
  particleDbs.push_back(particleDb);
  ParticleDb::setDefaultParticleDb(particleDb);
  ParticleType * particleType;
  char   buff[300];
  char   name[20];
  double mass, width, spin, isospin, isospin3, Nq, Ns, Naq, Nas, Nc, Nac, pdgCode;
  int    index = 0;
  try
  {
  while (!inputFile.eof())
    {
    inputFile.getline(buff,300);
    if (!(*buff) || (*buff == '#'))
      {
//      cout << "Buffer:" << buff << endl;
//      cout << "Skip comment" << endl;
      continue;
      }
    istringstream * iss = new istringstream(buff);
    (*iss) >> name >> mass >> width >> spin >> isospin >> isospin3 >> Nq >> Ns >> Naq >> Nas >> Nc >> Nac >> pdgCode;
    index++;
//    if (reportDebug(__FUNCTION__))
//      {
//      cout << '\t'<<index<<" "<<name<<" "<<mass<<" "<<width<<" "<<spin<<" "<<isospin<<" "<<isospin3<<" "<<Nq<<" "<<Naq<<" "<<Ns<<" "<<Nas<<" "<<Nc<<" "<<Nac<<" "<<pdgCode << endl;
//      }
    particleType = new ParticleType();
    particleType->setIndex(index);
    particleType->setName(name);
    particleType->setTitle(name);
    particleType->setMass(mass);
    particleType->setWidth(width);
    particleType->setSpin(spin);
    particleType->setIsospin(isospin);
    particleType->setIsospin3(isospin3);
    particleType->setNumberQ(static_cast<int>  (Nq));
    particleType->setNumberAQ(static_cast<int> (Naq));
    particleType->setNumberS(static_cast<int>  (Ns));
    particleType->setNumberAS(static_cast<int> (Nas));
    particleType->setNumberC(static_cast<int>  (Nc));
    particleType->setNumberAC(static_cast<int> (Nac));
    particleType->setNumberB(static_cast<int>  (0));
    particleType->setNumberAB(static_cast<int> (0));
    double netS   = Nas - Ns;
    double netC   = Nc  - Nac;
    double baryon = (Nq + Ns + Nc)/3.0 - (Naq + Nas + Nac)/3.0;
    double charge = isospin3 +(baryon +netS)/2.0;
    particleType->setCharge( static_cast<int> ( charge ));
    particleType->setBaryonNumber( static_cast<int> ( baryon ));
    particleType->setStrangessNumber(static_cast<int> ( netS   ));
    particleType->setCharmNumber(  static_cast<int> ( netC   ));
    particleType->setBottomNumber( 0 );
    particleType->setPdgCode(static_cast<int> ( pdgCode));
    particleType->setStable(true);
    particleType->disableDecay();
    particleType->enable();            
    particleDb->addParticleType(particleType);
    delete iss;
    }
  }
  catch (...)
  {
  throw FileException(particleDbImportPath,particleDbImportFile,"Error reading particle data base file","ParticleDbManager::importParticleDbNative()");
  }
  inputFile.close();

  // read decay file
  if (reportInfo(__FUNCTION__))
    {
    cout << "DB Path............: " << dbPath << endl;
    cout << "Decay File.........: " << particleDbImportDecaysFile << endl;
    cout << "Expected extension.: .data"<< endl;
    }

  ifstream & inputFileDecays = openInputAsciiFile(dbPath,particleDbImportDecaysFile,".data");
  double tBRatio, tRatio;
  int    CGcoeff; // complete branching ratio by Clebsch-Gordan coefficient: 0-no 1-yes
  ParticleType * parentType = nullptr;
  ParticleType * childType1 = nullptr;
  ParticleType * childType2 = nullptr;
  ParticleType * childType3 = nullptr;
  ParticleType * childType4 = nullptr;
  double mass0 = 0;
  double mass1 = 0;
  double mass2 = 0;
  double mass3 = 0;
  double mass4 = 0;

  while (!inputFileDecays.eof())
    {
    //cout << " BUG 0" << endl;

    inputFileDecays.getline(buff,300);
    if (!(*buff) || (*buff == '#'))
      {
//      cout << "Buffer:" << buff << endl;
//      cout << "Skip comment" << endl;
      continue;
      }
    //cout << " BUG 0a" << endl;

    char   parentName[20], childName1[20], childName2[20], childName3[20], childName4[20];
    istringstream * isss = new istringstream(buff);
    *isss >> parentName >> childName1 >> childName2 >> childName3 >> childName4 >> tBRatio >> CGcoeff;;
    //cout << parentName << "  " << childName1 << "  " << childName2 << "  " << childName3 << "  " <<  childName4 <<"  " << tBRatio << "  " <<CGcoeff <<  endl;;
    delete isss;

    parentType = nullptr;
    childType1 = nullptr;
    childType2 = nullptr;
    childType3 = nullptr;
    childType4 = nullptr;
    mass0 = 0;
    mass1 = 0;
    mass2 = 0;
    mass3 = 0;
    mass4 = 0;

    parentType = particleDb->getParticleType(parentName);
    if (!parentType)
      {
      String s("Did not find the father particle named: ");
      s += parentName;
      throw FileException(particleDbImportPath,particleDbImportDecaysFile,s,"ParticleDbManager::importParticleDbNative()");
      }

//    int pdg = std::fabs(parentType->getPdgCode());
//    bool flag = false;
//    if (pdg==3112 || pdg==3212)
//      {
//      cout << "Inserting decay pdg:" << pdg << endl;
//      flag = true;
//      }


    mass0 = parentType->getMass();
    //if (flag) cout << "Parent:" << parentType->getName() << endl;
    childType1 = particleDb->getParticleType(childName1);
    if (!childType1)
      {
      String s("Did not find child1 named: ");
      s += childName1;
      throw FileException(particleDbImportPath,particleDbImportDecaysFile,s,"ParticleDbManager::importParticleDbNative()");
      }
    mass1 = childType1->getMass();
    //if (flag)  cout << "Child1:" << childType1->getName() << endl;
    childType2 = particleDb->getParticleType(childName2);
    if (!childType2)
      {
      String s("Did not find child2 named: ");
      s += childName2;
      throw FileException(particleDbImportPath,particleDbImportDecaysFile,s,"ParticleDbManager::importParticleDbNative()");
      }
    mass2 = childType2->getMass();
    //if (flag)  cout << "Child2:" << childType2->getName() << endl;
    String name3(childName3);
    if (!name3.EqualTo("none"))
      {
      childType3 = particleDb->getParticleType(childName3);
      if (!childType3)
        {
        String s("Did not find child3 named: ");
        s += childName3;
        throw FileException(particleDbImportPath,particleDbImportDecaysFile,s,"ParticleDbManager::importParticleDbNative()");
        }
      mass3 = childType3->getMass();
      //if (flag)  cout << "Child3:" << childType3->getName() << endl;
      }
    String name4(childName4);
    if (!name4.EqualTo("none"))
      {
      childType4 = particleDb->getParticleType(childName4);
      if (!childType4)
        {
        String s("Did not find child4 named: ");
        s += childName4;
        throw FileException(particleDbImportPath,particleDbImportDecaysFile,s,"ParticleDbManager::importParticleDbNative()");
        }
      mass4 = childType4->getMass();
      //cout << "Child4:" << childType4->getName() << endl;
      }
    double massDiff = mass1+mass2+mass3+mass4-mass0;
    if (massDiff>0)
      {
       if (reportDebug(__FUNCTION__))
        {
        cout << "Mass exception for " << parentName << "->" <<  childName1 <<"+"<< childName2 <<"+"<< childName3 <<"+"<< childName4 << " with Mass Diff=" << massDiff << endl;
        }
      continue;
      }
    if (CGcoeff)
      {// complete branching ratio by Clebsch-Gordan coefficient
        double j1, m1, j2, m2, J, M, CB;
        J  = parentType->getIsospin();
        M  = parentType->getIsospin3();
        j1 = childType1->getIsospin();
        m1 = childType1->getIsospin3();
        j2 = childType2->getIsospin();
        m2 = childType2->getIsospin3();
        CB = clebschGordan(J, M, j1, m1, j2, m2);
        tRatio = CB*CB * tBRatio;
        // Multiply the Clebsh by two?
        // The same spin, ampss, strangeness (charm?)
        // and different I3?
        if (  (TMath::Abs(childType1->getSpin()  - childType2->getSpin()) < 0.01)
            && (TMath::Abs(childType1->getMass() - childType2->getMass()) < 0.01)
            && (TMath::Abs(childType1->getIsospin3()-childType2->getIsospin3())   > 0.01)
            && (childType1->getStrangessNumber()-childType2->getStrangessNumber() == 0)
            && (childType1->getCharmNumber()- childType2->getCharmNumber() == 0)        )
          {
          tRatio *= 2.0;
          }
      }
    else
      {
      tRatio = tBRatio;
      }
    //if (flag) cout << "Creating decay mode" << endl;

    ParticleDecayMode decayMode;
    decayMode.setBranchingRatio(tRatio);
    decayMode.addChild(childType1);
    decayMode.addChild(childType2);
    if (childType3) decayMode.addChild(childType3);
    if (childType4) decayMode.addChild(childType3);
    parentType->addDecayMode(decayMode);
    }
  if (reportDebug(__FUNCTION__))
    cout << "Total index of particles read: " <<  particleDb->getNumberOfTypes() << endl;
  inputFileDecays.close();
  particleDb->mapAntiParticleIndices();
  particleDb->setupDecayGenerator();
  //dbAnalyzer();
}

void ParticleDbManager::exportParticleDb()
{
 // TO be developed...
}

//void ParticleDbManager::exportParticleDbCAP()
//{
//  ofstream & outputFile = openOutputAsciiFile(particleDbExportPath,particleDbExportFile,".dat");
//  for (unsigned int iType=0; iType<particleDb->size(); iType++)
//    {
//    ParticleType * type = particleDb->getParticleType(iType);
//    if (type->getPdgCode()<0) continue; // not printing antiparticles
//    // this code is incomplete.
//    }
//  outputFile.close();
//  if (reportEnd(__FUNCTION__))
//    ;
//}

double ParticleDbManager::clebschGordan(double aJot,  double aEm,
                                        double aJot1, double aEm1,
                                        double aJot2, double aEm2)
{
  int mint, maxt;
  double cgc = 0.0;
  int tIter;
  double coef;

  maxt = lrint(aJot1 + aJot2 - aJot);
  mint = 0;
  if (lrint(aJot1 - aEm1)  < maxt) maxt = lrint(aJot1 - aEm1);
  if (lrint(aJot2 + aEm2)  < maxt) maxt = lrint(aJot2 + aEm2);
  if (lrint(-(aJot-aJot2+aEm1))  > mint) mint = lrint(-(aJot-aJot2+aEm1));
  if (lrint(-(aJot-aJot1-aEm2))  > mint) mint = lrint(-(aJot-aJot1-aEm2));

  //  // PRINT_DEBUG_3("\t\t\tmint " << mint << " j1 " <<  aJot1 << " m1 " << aEm1);
  //  // PRINT_DEBUG_3("\t\t\tampxt " << maxt << " j2 " <<  aJot2 << " m2 " << aEm2);

  for (tIter = mint; tIter<=maxt; tIter ++)
    {
    coef = TMath::Power(-1, tIter);
    // PRINT_DEBUG_3("\t\t\tcoef1 " << coef);
    coef *= sqrt( (2*aJot+1)
                 * factorials[lrint(aJot1+aEm1)]
                 * factorials[lrint(aJot1-aEm1)]
                 * factorials[lrint(aJot2+aEm2)]
                 * factorials[lrint(aJot2-aEm2)]
                 * factorials[lrint(aJot+aEm)]
                 * factorials[lrint(aJot-aEm)]
                 );
    // PRINT_DEBUG_3("\t\t\tcoef2 " << coef);
    coef /= factorials[tIter]
    * factorials[lrint(aJot1+aJot2-aJot-tIter)]
    * factorials[lrint(aJot1-aEm1-tIter)]
    * factorials[lrint(aJot2+aEm2-tIter)]
    * factorials[lrint(aJot-aJot2+aEm1+tIter)]
    * factorials[lrint(aJot-aJot1-aEm2+tIter)];
    // PRINT_DEBUG_3("\t\t\tcoef3 " << coef);
    cgc += coef;
    }
  cgc *= deltaJ(aJot1, aJot2, aJot);

  return cgc;
}

double ParticleDbManager::deltaJ(double aJot1, double aJot2, double aJot)
{
  double res = sqrt( 1.0
                    * factorials[lrint( aJot1 + aJot2 - aJot)]
                    * factorials[lrint( aJot1 - aJot2 + aJot)]
                    * factorials[lrint(-aJot1 + aJot2 + aJot)]
                    / factorials[lrint( aJot1 + aJot2 + aJot + 1)]
                    );
  return res;
}


void ParticleDbManager::dbAnalyzer()
{
  // check for charge, strangeness, and baryon number conservation...
  ParticleDb * particleDb = ParticleDb::getDefaultParticleDb();
  if (particleDb==nullptr) throw TaskException("particleDb==nullptr","ParticleDbManager::dbAnalyzer()");

  unsigned int nTypes = particleDb->getParticleTypeCount();
  // check for double pdg entries
  int count = 0;
  for (unsigned int iType=0; iType<nTypes; iType++)
    {

    ParticleType * particleType = particleDb->getParticleType(iType);
    int     pdg     = particleType->getPdgCode();
//    double  charge  = particleType->getCharge();
//    double  strange = particleType->getStrangessNumber();
//    double  baryon  = particleType->getBaryonNumber();
    count = 0;
    for (unsigned int kType=0; kType<nTypes; kType++)
      {
      ParticleType * particleType2 = particleDb->getParticleType(kType);
      int     pdg2     = particleType2->getPdgCode();
//      double  charge2  = particleType2->getCharge();
//      double  strange2 = particleType2->getStrangessNumber();
//      double  baryon2  = particleType2->getBaryonNumber();
      if (pdg == pdg2) count++;
      }
    if (count>1)
      {
      cout << "iType: " << iType << " Count: " << count << endl;
      cout << " name1: " << particleType->getName() << "   PDG:" <<pdg<< endl;
      //cout << " name2: " << particleType2->getName() << endl;
      }
    }

  for (unsigned int iType=0; iType<nTypes; iType++)
    {
    ParticleType * type = particleDb->getParticleType(iType);
    String  name    = type->getName();
    int     pdg     = type->getPdgCode();
    double  charge  = type->getCharge();
    double  strange = type->getStrangessNumber();
    double  baryon  = type->getBaryonNumber();
    bool    stable  = type->isStable();
    int     nModes  = type->getNDecayModes();
    if (stable) continue;
    if (nModes<1) throw TaskException("nModes<1 && !isStable()","ParticleDbManager::dbAnalyzer()");

    cout << "Analysing particle named: " << name << " PDG Index:" << pdg << endl;
    cout << "    Charge: " << charge << endl;
    cout << "   Strange: " << strange << endl;
    cout << "    Baryon: " << baryon << endl;
    cout << "    Stable: " << stable << endl;
    cout << "    nModes: " << nModes << endl;

    std::vector<ParticleDecayMode> decayModes =  type->getDecayModes();

    for (int iMode=0; iMode<nModes; iMode++)
      {
      ParticleDecayMode & decayMode = decayModes[iMode];
      int nChildren = decayMode.getNChildren();
      //cout << "Decay mode: " << iMode << "  nChildren: " << nChildren << endl;

      switch (nChildren)
        {
          case 1:
          {
          String s = "Single particle decay for ";
          s += name;
          throw TaskException(s,"ParticleDbManager::dbAnalyzer()");
          }

          case 2:
          {
          ParticleType  & childType1 = decayMode.getChildType(0);
          ParticleType  & childType2 = decayMode.getChildType(1);
          String name1 = childType1.getName();
          String name2 = childType2.getName();
          double sumCharge  = childType1.getCharge()          + childType2.getCharge();
          double sumStrange = childType1.getStrangessNumber() + childType2.getStrangessNumber();
          double sumBaryon  = childType1.getBaryonNumber()    + childType2.getBaryonNumber();
          if (charge!=sumCharge || strange!=sumStrange || baryon!=sumBaryon)
            {
            cout << "Check sanity!!!" << endl;
            cout << "Analysing particle named: " << name << " PDG Index:" << pdg << endl;
            //    cout << "    Charge: " << charge << endl;
            //    cout << "   Strange: " << strange << endl;
            //    cout << "    Baryon: " << baryon << endl;
            //    cout << "    Stable: " << stable << endl;
            //    cout << "    nModes: " << nModes << endl;
            cout << name << "  -->  " << name1 << "  +  " << name2 << endl;
            cout << " Charge: " << charge <<  "    sumCharge: " << sumCharge << endl;
            cout << "Strange: " << strange << "   sumStrange: " << sumStrange << endl;
            cout << " Baryon: " << baryon <<  "   sumBaryon: " << sumBaryon << endl;
            }
          break;
          }

          case 3:
          {
          ParticleType  & childType1 = decayMode.getChildType(0);
          ParticleType  & childType2 = decayMode.getChildType(1);
          ParticleType  & childType3 = decayMode.getChildType(2);
          String name1 = childType1.getName();
          String name2 = childType2.getName();
          String name3 = childType3.getName();
          double sumCharge  = childType1.getCharge()          + childType2.getCharge()+ childType3.getCharge();
          double sumStrange = childType1.getStrangessNumber() + childType2.getStrangessNumber()+ childType3.getStrangessNumber();
          double sumBaryon  = childType1.getBaryonNumber()    + childType2.getBaryonNumber()+ childType3.getBaryonNumber();
          if (charge!=sumCharge || strange!=sumStrange || baryon!=sumBaryon)
            {
            cout << "ERROR!!!!!!!!!!!" << endl;
            cout << "Analysing particle named: " << name << " PDG Index:" << pdg << endl;
            cout << name << "  -->  " << name1 << "  +  " << name2 << "  +  " << name3 << endl;
            cout << " Charge: " << charge <<  "    sumCharge: " << sumCharge << endl;
            cout << "Strange: " << strange << "   sumStrange: " << sumStrange << endl;
            cout << " Baryon: " << baryon <<  "   sumBaryon: " << sumBaryon << endl;
            }
          break;
          }
        }

      }
    }
}

CAP::ParticleDb & ParticleDbManager::getActiveParticleDb()
{
  ParticleDb * particleDb = particleDbs[0];
  if (!particleDb)
    {
    String s = "No ParticleDb available";
    throw TaskException(s,"ParticleDbManager::getActiveParticleDb()");
    }
  return *particleDb;
}

void ParticleDbManager::enableAllParticles()
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    particleDb[k]->enable();
    }
}

void ParticleDbManager::enableSelectedParticles()
{
  ParticleDb & particleDb = getActiveParticleDb();
  // Setup parameters to enable disabling selected particles
  String none("none");
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbEnable"; s += k;
    String particleName = getValueString(s);
    if (particleName.EqualTo(none)) continue;
    ParticleType * type = particleDb.getParticleType(particleName);
    if (!type)
      {
      String except = "No Particle called ";
      except += particleName;
      throw TaskException(s,"ParticleDbManager::enableSelectedParticles()");
      }
    type->enable();
    }
}

void ParticleDbManager::disableAllParticles()
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    particleDb[k]->disable();
    }
}

void ParticleDbManager::disableSelectedParticles()
{
  ParticleDb & particleDb = getActiveParticleDb();
  // Setup parameters to enable disabling selected particles
  String none("none");
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbDisable"; s += k;
    String particleName = getValueString(s);
    if (particleName.EqualTo(none)) continue;
    ParticleType * type = particleDb.getParticleType(particleName);
    if (!type)
      {
      String except = "No Particle called ";
      except += particleName;
      throw TaskException(s,"ParticleDbManager::disableSelectedParticles()");
      }
    type->disable();
    }
}

void ParticleDbManager::disableAllDecays()
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    particleDb[k]->disableDecay();
    }
}

void ParticleDbManager::disableWeakDecays()
{
  ParticleDb & particleDb = getActiveParticleDb();
  vector<String> selectedParticles;
  // kaons
  selectedParticles.push_back( String("Ka0492zer")); // 311);   // neutral kaons
  selectedParticles.push_back( String("Ka0492zrb")); // -311);
  // lambda
  selectedParticles.push_back( String("Lm1115zer")); //  3122); // lambda
  selectedParticles.push_back( String("Lm1115zrb")); // -3122);
  // sigma
  selectedParticles.push_back( String("Sg1189min")); //  3112); // sigmas
  selectedParticles.push_back( String("Sg1189mnb")); // -3112);
  selectedParticles.push_back( String("Sg1192zer")); //  3212);
  selectedParticles.push_back( String("Sg1192zrb")); // -3212);
  selectedParticles.push_back( String("Sg1189plu")); //  3222);
  selectedParticles.push_back( String("Sg1189plb")); // -3222);
  // xi (cascade)
  selectedParticles.push_back( String("Xi1321min")); //  3312);  // cascades
  selectedParticles.push_back( String("Xi1321mnb")); // -3312);
  selectedParticles.push_back( String("Xi1321zer")); //  3322);
  selectedParticles.push_back( String("Xi1321zrb")); // -3322);
  // Omega
  selectedParticles.push_back( String("UM1672min")); //  3334);  // Omegas
  selectedParticles.push_back( String("UM1672mnb")); // -3334);

  Size_t nSelected = selectedParticles.size();
  for (Size_t k=0; k<nSelected; k++)
    {
    ParticleType * type = particleDb.getParticleType(selectedParticles[k]);
    if (!type)
      {
      String except = "No Particle with Name=";
      except += selectedParticles[k];
      throw TaskException(except,"ParticleDbManager::disableWeakDecays()");
      }
    type->disableDecay();
    }
}

void ParticleDbManager::disableLongLivedWith(double lifeTime)
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    if (!particleDb[k]->isStable())
      {
      double tau = particleDb[k]->getLifeTime();
      if (tau>lifeTime) particleDb[k]->disableDecay();
      }
    }
}

void ParticleDbManager::disableShortLivedWith(double lifeTime)
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    if (!particleDb[k]->isStable())
      {
      double tau = particleDb[k]->getLifeTime();
      if (tau<=lifeTime) particleDb[k]->disableDecay();
      }
    }
}


void ParticleDbManager::disableSelectedDecays()
{
  ParticleDb & particleDb = getActiveParticleDb();
  // Setup parameters to enable decay disable of selected particles
  String none("none");
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbDisableDecay"; s += k;
    String particleName = getValueString(s);
    if (particleName.EqualTo(none)) continue;
    ParticleType * type = particleDb.getParticleType(particleName);
    if (!type)
      {
      String except = "No Particle called ";
      except += particleName;
      throw TaskException(s,"ParticleDbManager::disableSelectedDecays()");
      }
    type->disableDecay();
    }
}

void ParticleDbManager::enableAllDecays()
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    particleDb[k]->enableDecay();
    }
}

void ParticleDbManager::enableWeakDecays()
{
  ParticleDb & particleDb = getActiveParticleDb();
  vector<String> selectedParticles;
  // kaons
  selectedParticles.push_back( String("Ka0492zer")); // 311);   // neutral kaons
  selectedParticles.push_back( String("Ka0492zrb")); // -311);
  // lambda
  selectedParticles.push_back( String("Lm1115zer")); //  3122); // lambda
  selectedParticles.push_back( String("Lm1115zrb")); // -3122);
  // sigma
  selectedParticles.push_back( String("Sg1189min")); //  3112); // sigmas
  selectedParticles.push_back( String("Sg1189mnb")); // -3112);
  selectedParticles.push_back( String("Sg1192zer")); //  3212);
  selectedParticles.push_back( String("Sg1192zrb")); // -3212);
  selectedParticles.push_back( String("Sg1189plu")); //  3222);
  selectedParticles.push_back( String("Sg1189plb")); // -3222);
  // xi (cascade)
  selectedParticles.push_back( String("Xi1321min")); //  3312);  // cascades
  selectedParticles.push_back( String("Xi1321mnb")); // -3312);
  selectedParticles.push_back( String("Xi1321zer")); //  3322);
  selectedParticles.push_back( String("Xi1321zrb")); // -3322);
  // Omega
  selectedParticles.push_back( String("UM1672min")); //  3334);  // Omegas
  selectedParticles.push_back( String("UM1672mnb")); // -3334);

  Size_t nSelected = selectedParticles.size();
  for (Size_t k=0; k<nSelected; k++)
    {
    ParticleType * type = particleDb.getParticleType(selectedParticles[k]);
    if (!type)
      {
      String except = "No Particle with Name=";
      except += selectedParticles[k];
      throw TaskException(except,"ParticleDbManager::disableWeakDecays()");
      }
    type->enableDecay();
    }
}

void ParticleDbManager::enableLongLivedWith(double lifeTime)
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    if (!particleDb[k]->isStable())
      {
      double tau = particleDb[k]->getLifeTime();
      if (tau>lifeTime) particleDb[k]->enableDecay();
      }
    }
}

void ParticleDbManager::enableShortLivedWith(double lifeTime)
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    if (!particleDb[k]->isStable())
      {
      double tau = particleDb[k]->getLifeTime();
      if (tau<lifeTime) particleDb[k]->enableDecay();
      }
    }
}

void ParticleDbManager::enableSelectedDecays()
{
  ParticleDb & particleDb = getActiveParticleDb();
  // Setup parameters to enable decay disable of selected particles
  String none("none");
  for (int k=0; k<40; k++)
    {
    String s = "ParticleDbEnableDecay"; s += k;
    String particleName = getValueString(s);
    if (particleName.EqualTo(none)) continue;
    ParticleType * type = particleDb.getParticleType(particleName);
    if (!type)
      {
      String except = "No Particle called ";
      except += particleName;
      throw TaskException(s,"ParticleDbManager::enableSelectedDecays()");
      }
    type->disableDecay();
    }
}

void ParticleDbManager::listParticleStatus()
{
  ParticleDb & particleDb = getActiveParticleDb();
  Size_t n = particleDb.getNumberOfTypes();
  for (Size_t k=0; k<n; k++)
    {
    cout << k << "   " << particleDb[k]->getName();
    if (particleDb[k]->isStable())
      cout << "  Stable";
    else
      cout << "  Unstable";

    if (particleDb[k]->isEnabled())
      cout << "  Enabled";
    else
      cout << "  Disabled";

    if (particleDb[k]->isDecayEnabled())
      cout << "  DecayEnabled";
    else
      cout << "  DecayDisabled";
    cout << endl;
    }

}
