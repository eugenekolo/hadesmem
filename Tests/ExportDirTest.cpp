/*
This file is part of HadesMem.
Copyright (C) 2011 Joshua Boyce (a.k.a. RaptorFactor).
<http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

HadesMem is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

HadesMem is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with HadesMem.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BOOST_TEST_MODULE ExportDirTest
#pragma warning(push, 1)
#include <boost/test/unit_test.hpp>
#pragma warning(pop)

#include "HadesMemory/Memory.hpp"

BOOST_AUTO_TEST_CASE(BOOST_TEST_MODULE)
{
  Hades::Memory::MemoryMgr MyMemory(GetCurrentProcessId());
    
  for (Hades::Memory::ModuleIter ModIter(MyMemory); *ModIter; ++ModIter)
  {
    Hades::Memory::Module const Mod = **ModIter;
      
    // Todo: Also test FileType_Data
    Hades::Memory::PeFile MyPeFile(MyMemory, Mod.GetBase());
      
    Hades::Memory::ExportDir MyExportDir(MyPeFile);
    if (!MyExportDir.IsValid())
    {
      continue;
    }
    
    auto ExpDirRaw = MyMemory.Read<IMAGE_EXPORT_DIRECTORY>(
      MyExportDir.GetBase());
    
    BOOST_CHECK_EQUAL(MyExportDir.IsValid(), true);
    MyExportDir.EnsureValid();
    MyExportDir.SetCharacteristics(MyExportDir.GetCharacteristics());
    MyExportDir.SetTimeDateStamp(MyExportDir.GetTimeDateStamp());
    MyExportDir.SetMajorVersion(MyExportDir.GetMajorVersion());
    MyExportDir.SetMinorVersion(MyExportDir.GetMinorVersion());
    MyExportDir.SetOrdinalBase(MyExportDir.GetOrdinalBase());
    MyExportDir.SetNumberOfFunctions(MyExportDir.GetNumberOfFunctions());
    MyExportDir.SetNumberOfNames(MyExportDir.GetNumberOfNames());
    MyExportDir.SetAddressOfFunctions(MyExportDir.GetAddressOfFunctions());
    MyExportDir.SetAddressOfNames(MyExportDir.GetAddressOfNames());
    MyExportDir.SetAddressOfNameOrdinals(MyExportDir.GetAddressOfNameOrdinals());
    MyExportDir.SetCharacteristics(MyExportDir.GetCharacteristics());
    BOOST_CHECK(!MyExportDir.GetName().empty());
    MyExportDir.GetExportDirRaw();
    
    auto ExpDirRawNew = MyMemory.Read<IMAGE_EXPORT_DIRECTORY>(
      MyExportDir.GetBase());
      
    BOOST_CHECK_EQUAL(std::memcmp(&ExpDirRaw, &ExpDirRawNew, sizeof(
      IMAGE_SECTION_HEADER)), 0);
      
    boost::optional<Hades::Memory::Export> TestEnum(*Hades::Memory::
      ExportIter(MyPeFile));
    BOOST_CHECK(TestEnum);
      
    for (Hades::Memory::ExportIter i(MyPeFile); *i; ++i)
    {
      Hades::Memory::Export const Current = **i;
      Hades::Memory::Export const Test(MyPeFile, Current.GetOrdinal());
        
      if (Test.ByName())
      {
        BOOST_CHECK(!Test.GetName().empty());
      }
      else
      {
        BOOST_CHECK(Test.GetOrdinal() >= MyExportDir.GetOrdinalBase());
      }
      
      if (Test.Forwarded())
      {
        BOOST_CHECK(!Test.GetForwarder().empty());
        BOOST_CHECK(!Test.GetForwarderModule().empty());
        BOOST_CHECK(!Test.GetForwarderFunction().empty());
      }
      else
      {
        BOOST_CHECK(Test.GetRva() != 0);
        BOOST_CHECK(Test.GetVa() != nullptr);
      }
    }
  }
}
