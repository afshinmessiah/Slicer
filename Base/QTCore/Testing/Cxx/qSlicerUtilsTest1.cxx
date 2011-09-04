/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// QT includes
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <QTime>

// CTK includes
#include <ctkUtils.h>

// SlicerQt includes
#include <qSlicerUtils.h>

// STD includes
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
//-----------------------------------------------------------------------------
bool createFile(int line, const QDir& dir, const QString& relativePath, const QString& fileName)
{
  QDir newDir(dir);
  newDir.mkpath(relativePath);
  newDir.cd(relativePath);
  QString filePath = QFileInfo(newDir, fileName).filePath();
  QFile file(filePath);
  file.open(QIODevice::Text | QIODevice::WriteOnly);
  QTextStream out(&file);
  out << "Generated by qSlicerUtilsTest1" << endl;
  file.close();

  if (!QFile::exists(filePath))
    {
    std::cerr << "Line " << line << " - Failed to create file" << qPrintable(filePath) << std::endl;
    return false;
    }

  return true;
}

//-----------------------------------------------------------------------------
bool isPluginInstalledTest(int line, bool expectedResult,
                           const QString& path, const QString& applicationHomeDir)
{
  bool res = qSlicerUtils::isPluginInstalled(path, applicationHomeDir);
  if (res != expectedResult)
    {
//    std::cerr << "Line " << line << " - Problem with isPluginInstalled()\n"
//              << "\tpath: " << qPrintable(path) << "\n"
//              << "\tapplicationHomeDir: " << qPrintable(applicationHomeDir) << std::endl;
    QString msg("Line %1 - Problem with isPluginInstalled()\n\tpath: %2\n\tapplicationHomeDir: %3");
    throw std::runtime_error(qPrintable(msg.arg(line).arg(path).arg(applicationHomeDir)));
    }
  return res;
}

} // end of anonymous namespace

int qSlicerUtilsTest1(int, char * [] )
{
  //-----------------------------------------------------------------------------
  // Test isExecutableName()
  //-----------------------------------------------------------------------------
  QStringList executableNames;
  executableNames << "Threshold.bat" << "Threshold.com"
                  << "Threshold.sh" << "Threshold.csh"
                  << "Threshold.tcsh" << "Threshold.pl"
                  << "Threshold.py" << "Threshold.tcl"
                  << "Threshold.m" << "Threshold.exe";

  foreach(const QString& executableName, executableNames)
    {
    bool isExecutable = qSlicerUtils::isExecutableName(executableName);
    if (!isExecutable)
      {
      std::cerr << __LINE__ << " - Error in  isExecutableName()" << std::endl
                            << "[" << qPrintable(executableName)
                            << "] should be an executable" << std::endl;
      }
    }

  QStringList notExecutableNames;
  notExecutableNames << "Threshold.ini" << "Threshold.txt" << "Threshold";
  foreach(const QString& notExecutableName, notExecutableNames)
    {
    bool isExecutable = qSlicerUtils::isExecutableName(notExecutableName);
    if (isExecutable)
      {
      std::cerr << __LINE__ << " - Error in  isExecutableName()" << std::endl
                            << "[" << qPrintable(notExecutableName)
                            << "] should *NOT* be an executable" << std::endl;
      }
    }

  //-----------------------------------------------------------------------------
  // Test executableExtension()
  //-----------------------------------------------------------------------------
#ifdef _WIN32
  QString expectedExecutableExtension = ".exe";
#else
  QString expectedExecutableExtension = "";
#endif
  QString executableExtension = qSlicerUtils::executableExtension(); 
  if (executableExtension != expectedExecutableExtension)
    {
    std::cerr << __LINE__ << " - Error in  executableExtension()" << std::endl
                          << "executableExtension = " << qPrintable(executableExtension) << std::endl
                          << "expectedExecutableExtension = " << qPrintable(expectedExecutableExtension) << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Test extractModuleNameFromLibraryName()
  //-----------------------------------------------------------------------------
  QStringList libraryNames;
  libraryNames << "ThresholdLib.dll"
               << "Threshold.dll"
               << "libThreshold.so"
               << "libThreshold.so.2.3"
               << "libThreshold.dylib"
               << "qSlicerThresholdModule.so"
               << "qSlicerThreshold.dylib";
             
  QString expectedModuleName = "threshold";

  foreach (const QString& libraryName, libraryNames)
    {
    QString moduleName = qSlicerUtils::extractModuleNameFromLibraryName(libraryName);
    if (moduleName != expectedModuleName)
      {
      std::cerr << __LINE__ << " - Error in  extractModuleName()" << std::endl
                            << "moduleName = " << qPrintable(moduleName) << std::endl
                            << "expectedModuleName = " << qPrintable(expectedModuleName) << std::endl;
      return EXIT_FAILURE;
      }
    }

  //-----------------------------------------------------------------------------
  // Test isPluginInstalled()
  //-----------------------------------------------------------------------------

  QStringList directoriesToRemove;

  //
  // Case 1: Application and plugins are located inside the application build tree
  //

  // 'tmp1' is considered to be the application and plugins build tree

  QDir tmp1 = QDir::temp();
  QString temporaryDirName =
      QString("qSlicerUtilsTest1-tmp1.%1").arg(QTime::currentTime().toString("hhmmsszzz"));

  tmp1.mkdir(temporaryDirName);
  tmp1.cd(temporaryDirName);

  QString debug("Debug");
  QString release("Release");
  QString relWithDebInfo("RelWithDebInfo");
  QString minSizeRel("MinSizeRel");
  QString foo("foo");
  QString fooDebug("foo/Debug");
  QString fooRelease("foo/Release");
  QString fooBar("foo/bar");
  QString fooBarDebug("foo/bar/Debug");
  QString fooBarRelease("foo/bar/Release");

  createFile(__LINE__, tmp1, ".", "CMakeCache.txt");

  foreach(const QString& relativePath,
          QStringList() << debug << release << relWithDebInfo << minSizeRel << foo << fooDebug
                        << fooRelease << fooBar << fooBarDebug << fooBarRelease)
    {
    createFile(__LINE__, tmp1, relativePath, "plugin.txt");
    }

  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + debug + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + release + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + relWithDebInfo + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + minSizeRel + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + foo + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooDebug + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooRelease + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBar + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBarDebug + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBarRelease + "/plugin.txt", tmp1.path());

  directoriesToRemove << tmp1.path();

  //
  // Case 2: Application and plugins are in two different build trees
  //

  // 'tmp1' is considered to be the plugins build tree
  // 'tmp2' is considered to be the application build tree

  temporaryDirName =
      QString("qSlicerUtilsTest1-tmp2.%1").arg(QTime::currentTime().toString("hhmmsszzz"));

  QDir tmp2 = QDir::temp();
  tmp2.mkdir(temporaryDirName);
  tmp2.cd(temporaryDirName);

  createFile(__LINE__, tmp2, ".", "CMakeCache.txt");

  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + debug + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + release + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + relWithDebInfo + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + minSizeRel + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + foo + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooDebug + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooRelease + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBar + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBarDebug + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBarRelease + "/plugin.txt", tmp2.path());

  directoriesToRemove << tmp2.path();

  //
  // Case 3: Application is in its own build tree, plugins are installed in a different location
  //

  // 'tmp2' is considered to be the application build tree
  // 'tmp3' is considered to be the plugins installed tree

  temporaryDirName =
      QString("qSlicerUtilsTest1-tmp3.%1").arg(QTime::currentTime().toString("hhmmsszzz"));
  QDir tmp3 = QDir::temp();
  tmp3.mkdir(temporaryDirName);
  tmp3.cd(temporaryDirName);

  foreach(const QString& relativePath, QStringList() << foo << fooBar)
    {
    createFile(__LINE__, tmp1, relativePath, "plugin.txt");
    }

  isPluginInstalledTest(__LINE__, true, tmp3.path() + "/" + foo + "/plugin.txt", tmp2.path());
  isPluginInstalledTest(__LINE__, true, tmp3.path() + "/" + fooBar + "/plugin.txt", tmp2.path());

  directoriesToRemove << tmp3.path();

  //
  // Case 4: Application is installed, plugins are in their own build tree
  //

  // 'tmp4' is considered to be the application installed tree
  // 'tmp1' is considered to be the plugins build tree

  temporaryDirName =
      QString("qSlicerUtilsTest1-tmp4.%1").arg(QTime::currentTime().toString("hhmmsszzz"));
  QDir tmp4 = QDir::temp();
  tmp4.mkdir(temporaryDirName);
  tmp4.cd(temporaryDirName);

  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + debug + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + release + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + relWithDebInfo + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + minSizeRel + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + foo + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooDebug + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooRelease + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBar + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBarDebug + "/plugin.txt", tmp1.path());
  isPluginInstalledTest(__LINE__, false, tmp1.path() + "/" + fooBarRelease + "/plugin.txt", tmp1.path());

  directoriesToRemove << tmp4.path();

  // Clean temporary directories
  foreach(const QString& dir, directoriesToRemove)
    {
    ctk::removeDirRecursively(dir);
    }

  return EXIT_SUCCESS;
}

