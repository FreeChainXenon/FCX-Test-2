// Test program written in C, for a (very early version of) FreeChainXenon
// Displays the E74 UEM
//
// Copyright (c) 2025 Aiden Isik
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// EVERYTHING here is written in main(), for good reason.
// As of writing, the C compiler is unable to properly handle function calls.
// When I get Newlib ported, then I should be able to build a second-stage GCC and get it working.

// Also, as of writing, as I only have a first-stage GCC, with a stub in place of
// the __eabi calling convention. This has an adverse effect on variables.
// They get overwritten if you call a function after setting them!

#include "stdint.h"

int main()
{
  // Find the address of the VdDisplayFatalError kernel function (ordinal 434)
  uint16_t ordinal = 434;
  
  // Get address of PE header
  uint32_t peHeader = 0;

  for(int8_t i = 3; i >= 0; i--)
    {
      peHeader |= (*((uint8_t*)(0x8004003C + i))) << (i * 8);
    }

  peHeader += 0x80040000;
  
  // Get address of export directory table
  uint32_t exportDirTable = 0;

  for(int8_t i = 3; i >= 0; i--)
    {
      exportDirTable |= (*((uint8_t*)(peHeader + 0x78 + i))) << (i * 8);
    }

  exportDirTable += 0x80040000;

  // Here, we would check if the ordinal exists, but since we're hard-coding
  // VdDisplayFatalError (ordinal 434), we already know it does.
  
  // Get address of export address table
  uint32_t exportAddrTable = 0;

  for(int8_t i = 3; i >= 0; i--)
    {
      exportAddrTable |= (*((uint8_t*)(exportDirTable + 0x1C + i))) << (i * 8);
    }

  exportAddrTable += 0x80040000;
  
  // Finally, read the address of VdDisplayFatalError using the ordinal as an index
  uint32_t VdDisplayFatalErrorAddr = 0;

  for(int8_t i = 3; i >= 0; i--)
    {
      VdDisplayFatalErrorAddr |= (*((uint8_t*)(exportAddrTable + ((ordinal - 1) * 4) + i))) << (i * 8);
    }

  VdDisplayFatalErrorAddr += 0x80040000;
  
  // And call it.
  typedef void VdDisplayFatalError_t(uint32_t);
  VdDisplayFatalError_t *VdDisplayFatalError = (VdDisplayFatalError_t*)VdDisplayFatalErrorAddr;
  VdDisplayFatalError(0x1244A);
}
